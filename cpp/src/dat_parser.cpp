#include "dat_parser.hpp"
#include "geochecker_types.hpp"
#include "routercommon.pb.h"

#include <fstream>
#include <algorithm>
#include <regex>
#include <set>
#include <thread>
#include <future>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

namespace geochecker {

namespace {

// ---- string helpers ----

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

std::string normalize_domain(const std::string& domain) {
    std::string d = to_lower(domain);
    while (!d.empty() && (d.front() == ' ' || d.front() == '\t')) d.erase(0, 1);
    while (!d.empty() && (d.back() == ' ' || d.back() == '\t')) d.erase(d.size() - 1);
    if (d.size() >= 4 && d.substr(0, 4) == "www.") {
        d = d.substr(4);
    }
    return d;
}

bool domain_matches(const std::string& domain_value, int domain_type, const std::string& query) {
    const std::string query_norm = normalize_domain(query);
    const std::string val_lower = to_lower(domain_value);

    switch (domain_type) {
    case 0:  // Plain - keyword
        return val_lower.find(query_norm) != std::string::npos ||
               query_norm.find(val_lower) != std::string::npos;
    case 2:  // RootDomain
        return query_norm == val_lower ||
               (query_norm.size() > val_lower.size() &&
                query_norm.compare(query_norm.size() - val_lower.size() - 1,
                                   val_lower.size() + 1, "." + val_lower) == 0);
    case 3:  // Full - exact
        return query_norm == val_lower;
    case 1:  // Regex
        try {
            std::regex re(domain_value, std::regex_constants::ECMAScript | std::regex_constants::icase);
            return std::regex_search(query_norm, re);
        } catch (...) {
            return false;
        }
    default:
        return false;
    }
}

std::pair<std::string, std::string> parse_tag(const std::string& tag) {
    std::string t = tag;
    size_t pos = t.find("geosite:");
    if (pos != std::string::npos) {
        t = t.substr(pos + 8);
    }
    t = to_lower(t);
    while (!t.empty() && (t.front() == ' ' || t.front() == '\t')) t.erase(0, 1);

    size_t at_pos = t.find('@');
    if (at_pos != std::string::npos) {
        return {t.substr(0, at_pos), t.substr(at_pos + 1)};
    }
    return {t, ""};
}

bool domain_has_attribute(const routercommon::Domain& d, const std::string& attr) {
    for (const auto& a : d.attribute()) {
        if (to_lower(a.key()) == attr) return true;
    }
    return false;
}

// ---- protobuf wire format helpers ----
// Used to scan .dat files and build a lightweight index without full parsing.

bool decode_varint(const uint8_t* data, size_t size, size_t& pos, uint64_t& value) {
    value = 0;
    unsigned shift = 0;
    while (pos < size) {
        uint8_t byte = data[pos++];
        value |= static_cast<uint64_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) return true;
        shift += 7;
        if (shift >= 64) return false;
    }
    return false;
}

bool skip_wire_field(const uint8_t* data, size_t size, size_t& pos, int wire_type) {
    switch (wire_type) {
    case 0: { uint64_t v; return decode_varint(data, size, pos, v); }
    case 1: pos += 8; return pos <= size;
    case 2: {
        uint64_t len;
        if (!decode_varint(data, size, pos, len)) return false;
        pos += static_cast<size_t>(len);
        return pos <= size;
    }
    case 5: pos += 4; return pos <= size;
    default: return false;
    }
}

// Extract string field 1 (country_code) from serialized GeoSite/GeoIP bytes
std::string extract_country_code(const uint8_t* data, size_t size) {
    size_t pos = 0;
    while (pos < size) {
        uint64_t tag;
        if (!decode_varint(data, size, pos, tag)) break;
        uint32_t field = static_cast<uint32_t>(tag >> 3);
        int wire = static_cast<int>(tag & 0x07);
        if (field == 1 && wire == 2) {
            uint64_t len;
            if (!decode_varint(data, size, pos, len)) break;
            if (pos + len > size) break;
            return std::string(reinterpret_cast<const char*>(data + pos),
                               static_cast<size_t>(len));
        }
        if (!skip_wire_field(data, size, pos, wire)) break;
    }
    return "";
}

struct RawEntry {
    std::string category;
    uint32_t offset;
    uint32_t length;
};

// Scan a serialized protobuf List message (GeoSiteList or GeoIPList)
// to find each repeated field-1 entry and extract its country_code.
std::vector<RawEntry> scan_top_level_entries(const uint8_t* data, size_t size) {
    std::vector<RawEntry> entries;
    size_t pos = 0;
    while (pos < size) {
        uint64_t tag;
        if (!decode_varint(data, size, pos, tag)) break;
        uint32_t field = static_cast<uint32_t>(tag >> 3);
        int wire = static_cast<int>(tag & 0x07);
        if (field == 1 && wire == 2) {
            uint64_t len;
            if (!decode_varint(data, size, pos, len)) break;
            if (pos + len > size) break;
            RawEntry e;
            e.offset = static_cast<uint32_t>(pos);
            e.length = static_cast<uint32_t>(len);
            e.category = to_lower(extract_country_code(
                data + pos, static_cast<size_t>(len)));
            entries.push_back(std::move(e));
            pos += static_cast<size_t>(len);
        } else {
            if (!skip_wire_field(data, size, pos, wire)) break;
        }
    }
    return entries;
}

} // namespace

// ---- load functions (lazy: read raw bytes + build index) ----

std::unique_ptr<GeoSiteData> load_geosite(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return nullptr;

    f.seekg(0, std::ios::end);
    size_t size = f.tellg();
    f.seekg(0, std::ios::beg);

    std::string data(size, '\0');
    if (!f.read(&data[0], size)) return nullptr;

    auto raw = scan_top_level_entries(
        reinterpret_cast<const uint8_t*>(data.data()), data.size());

    std::vector<GeoSiteData::EntryLocation> entries;
    entries.reserve(raw.size());
    for (auto& r : raw)
        entries.push_back({std::move(r.category), r.offset, r.length});

    return std::make_unique<GeoSiteData>(std::move(data), std::move(entries));
}

std::unique_ptr<GeoIPData> load_geoip(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return nullptr;

    f.seekg(0, std::ios::end);
    size_t size = f.tellg();
    f.seekg(0, std::ios::beg);

    std::string data(size, '\0');
    if (!f.read(&data[0], size)) return nullptr;

    auto raw = scan_top_level_entries(
        reinterpret_cast<const uint8_t*>(data.data()), data.size());

    std::vector<GeoIPData::EntryLocation> entries;
    entries.reserve(raw.size());
    for (auto& r : raw)
        entries.push_back({std::move(r.category), r.offset, r.length});

    return std::make_unique<GeoIPData>(std::move(data), std::move(entries));
}

// ---- query functions (parse entries on demand, free after use) ----

std::vector<std::string> search_domain_in_geosite(const GeoSiteData* geosite, const std::string& domain) {
    std::vector<std::string> result;
    if (!geosite) return result;

    const std::string domain_norm = normalize_domain(domain);
    const size_t total = geosite->entry_count();
    if (total == 0) return result;

    unsigned hw = std::thread::hardware_concurrency();
    const size_t num_threads = std::max<size_t>(1, std::min<size_t>(hw > 0 ? hw : 4, total));
    const size_t chunk = (total + num_threads - 1) / num_threads;

    std::vector<std::future<std::vector<std::string>>> futures;
    for (size_t t = 0; t < num_threads; ++t) {
        size_t from = t * chunk;
        size_t to = std::min(from + chunk, total);
        if (from >= to) break;
        futures.push_back(std::async(std::launch::async,
            [geosite, &domain_norm, from, to]() {
                std::vector<std::string> local;
                for (size_t i = from; i < to; ++i) {
                    auto entry = geosite->parse_entry(i);
                    if (!entry) continue;
                    std::string category = to_lower(entry->country_code());
                    std::set<std::string> matched_attrs;
                    bool matched_base = false;
                    for (const auto& d : entry->domain()) {
                        if (domain_matches(d.value(), static_cast<int>(d.type()), domain_norm)) {
                            matched_base = true;
                            for (const auto& a : d.attribute())
                                matched_attrs.insert(to_lower(a.key()));
                        }
                    }
                    if (matched_base) {
                        local.push_back(category);
                        for (const auto& attr : matched_attrs)
                            local.push_back(category + "@" + attr);
                    }
                }
                return local;
            }));
    }

    // Merge + dedup
    std::set<std::string> seen;
    for (auto& f : futures) {
        for (auto& s : f.get()) {
            if (seen.insert(s).second)
                result.push_back(std::move(s));
        }
    }
    return result;
}

std::vector<std::string> get_domains_from_geosite(const GeoSiteData* geosite, const std::string& tag) {
    std::vector<std::string> domains;
    if (!geosite) return domains;

    auto [category, attr] = parse_tag(tag);

    // Try exact category match (O(1) via hash map)
    int idx = geosite->find_category(category);

    // If not found and tag contains "-", try splitting into category + attribute
    if (idx < 0 && attr.empty() && category.find('-') != std::string::npos) {
        for (size_t i = category.size(); i > 0; --i) {
            if (i < category.size() && category[i] == '-') {
                std::string try_cat = category.substr(0, i);
                std::string try_attr = category.substr(i + 1);
                int try_idx = geosite->find_category(try_cat);
                if (try_idx >= 0) {
                    category = try_cat;
                    attr = try_attr;
                    idx = try_idx;
                    break;
                }
            }
        }
    }

    if (idx < 0) return domains;

    // Parse only the needed entry
    auto entry = geosite->parse_entry(static_cast<size_t>(idx));
    if (!entry) return domains;

    for (const auto& d : entry->domain()) {
        if (!attr.empty() && !domain_has_attribute(d, attr)) continue;
        int dt = static_cast<int>(d.type());
        if (dt == 0) domains.push_back("keyword:" + d.value());
        else if (dt == 1) domains.push_back("regexp:" + d.value());
        else if (dt == 2) domains.push_back(d.value());
        else if (dt == 3) domains.push_back("full:" + d.value());
    }
    return domains;
}

static std::vector<std::string> resolve_domain_impl(const std::string& domain) {
    std::vector<std::string> ips;
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return ips;
#endif

    struct addrinfo hints {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res = nullptr;
    int err = getaddrinfo(domain.c_str(), nullptr, &hints, &res);
    if (err == 0 && res) {
        for (struct addrinfo* p = res; p; p = p->ai_next) {
            if (p->ai_family == AF_INET) {
                char buf[INET_ADDRSTRLEN];
                auto* sa = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
                if (inet_ntop(AF_INET, &sa->sin_addr, buf, sizeof(buf))) {
                    ips.push_back(buf);
                }
            }
        }
        freeaddrinfo(res);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return ips;
}

std::vector<std::string> resolve_domain_to_ip(const std::string& domain, double /*timeout_sec*/) {
    std::string clean = domain;
    while (!clean.empty() && (clean.front() == ' ' || clean.front() == '\t')) clean.erase(0, 1);
    while (!clean.empty() && (clean.back() == ' ' || clean.back() == '\t')) clean.erase(clean.size() - 1);

    if (clean.find("keyword:") == 0 || clean.find("regexp:") == 0) return {};
    if (clean.find("full:") == 0) clean = clean.substr(5);

    if (clean.empty() || clean.find(' ') != std::string::npos || clean.front() == '.') return {};

    return resolve_domain_impl(clean);
}

std::string cidr_to_string(const std::string& ip_bytes, uint32_t prefix) {
    if (ip_bytes.size() == 4) {
        char buf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, ip_bytes.data(), buf, sizeof(buf))) {
            return std::string(buf) + "/" + std::to_string(prefix);
        }
    }
    if (ip_bytes.size() == 16) {
        char buf[INET6_ADDRSTRLEN];
        if (inet_ntop(AF_INET6, ip_bytes.data(), buf, sizeof(buf))) {
            return std::string(buf) + "/" + std::to_string(prefix);
        }
    }
    return "";
}

std::vector<std::string> get_ips_from_geoip(const GeoIPData* geoip, const std::string& tag) {
    std::vector<std::string> result;
    if (!geoip) return result;

    std::string tag_clean = tag;
    size_t pos = tag_clean.find("geoip:");
    if (pos != std::string::npos) {
        tag_clean = tag_clean.substr(pos + 6);
    }
    tag_clean = to_lower(tag_clean);
    while (!tag_clean.empty() && (tag_clean.front() == ' ' || tag_clean.front() == '\t')) {
        tag_clean.erase(0, 1);
    }

    // O(1) lookup instead of linear scan
    int idx = geoip->find_category(tag_clean);
    if (idx < 0) return result;

    // Parse only the needed entry
    auto entry = geoip->parse_entry(static_cast<size_t>(idx));
    if (!entry) return result;

    for (const auto& cidr : entry->cidr()) {
        std::string s = cidr_to_string(cidr.ip(), cidr.prefix());
        if (!s.empty()) result.push_back(s);
    }
    return result;
}

std::vector<std::string> list_geosite_categories(const GeoSiteData* geosite) {
    std::vector<std::string> result;
    if (!geosite) return result;

    for (size_t i = 0; i < geosite->entry_count(); ++i) {
        const std::string& cat = geosite->category(i);
        result.push_back(cat);

        // Parse entry to collect attributes
        auto entry = geosite->parse_entry(i);
        if (!entry) continue;

        std::set<std::string> attrs;
        for (const auto& d : entry->domain()) {
            for (const auto& a : d.attribute()) {
                attrs.insert(to_lower(a.key()));
            }
        }
        for (const auto& a : attrs) {
            result.push_back(cat + "@" + a);
        }
        // entry freed here — one category at a time
    }
    return result;
}

std::vector<std::string> list_geoip_categories(const GeoIPData* geoip) {
    std::vector<std::string> result;
    if (!geoip) return result;
    for (size_t i = 0; i < geoip->entry_count(); ++i)
        result.push_back(geoip->category(i));
    return result;
}

} // namespace geochecker
