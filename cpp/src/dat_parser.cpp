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

} // namespace

std::unique_ptr<GeoSiteData> load_geosite(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return nullptr;

    f.seekg(0, std::ios::end);
    size_t size = f.tellg();
    f.seekg(0, std::ios::beg);

    std::string data(size, '\0');
    if (!f.read(&data[0], size)) return nullptr;

    auto* list = new routercommon::GeoSiteList();
    if (!list->ParseFromString(data)) {
        delete list;
        return nullptr;
    }
    return std::make_unique<GeoSiteData>(list);
}

std::unique_ptr<GeoIPData> load_geoip(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return nullptr;

    f.seekg(0, std::ios::end);
    size_t size = f.tellg();
    f.seekg(0, std::ios::beg);

    std::string data(size, '\0');
    if (!f.read(&data[0], size)) return nullptr;

    auto* list = new routercommon::GeoIPList();
    if (!list->ParseFromString(data)) {
        delete list;
        return nullptr;
    }
    return std::make_unique<GeoIPData>(list);
}

std::vector<std::string> search_domain_in_geosite(const GeoSiteData* geosite, const std::string& domain) {
    std::vector<std::string> result;
    if (!geosite) return result;

    const std::string domain_norm = normalize_domain(domain);
    const auto* list = geosite->get();

    for (const auto& entry : list->entry()) {
        std::string category = to_lower(entry.country_code());
        for (const auto& d : entry.domain()) {
            if (domain_matches(d.value(), static_cast<int>(d.type()), domain_norm)) {
                if (std::find(result.begin(), result.end(), category) == result.end()) {
                    result.push_back(category);
                }
                break;
            }
        }
    }
    return result;
}

std::vector<std::string> get_domains_from_geosite(const GeoSiteData* geosite, const std::string& tag) {
    std::vector<std::string> domains;
    if (!geosite) return domains;

    std::string tag_clean = tag;
    size_t pos = tag_clean.find("geosite:");
    if (pos != std::string::npos) {
        tag_clean = tag_clean.substr(pos + 7);
    }
    tag_clean = to_lower(tag_clean);
    while (!tag_clean.empty() && (tag_clean.front() == ' ' || tag_clean.front() == '\t')) {
        tag_clean.erase(0, 1);
    }

    const auto* list = geosite->get();
    for (const auto& entry : list->entry()) {
        if (to_lower(entry.country_code()) != tag_clean) continue;

        for (const auto& d : entry.domain()) {
            int dt = static_cast<int>(d.type());
            if (dt == 0) domains.push_back("keyword:" + d.value());
            else if (dt == 1) domains.push_back("regexp:" + d.value());
            else if (dt == 2) domains.push_back(d.value());
            else if (dt == 3) domains.push_back("full:" + d.value());
        }
        break;
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

    const auto* list = geoip->get();
    for (const auto& entry : list->entry()) {
        if (to_lower(entry.country_code()) != tag_clean) continue;

        for (const auto& cidr : entry.cidr()) {
            std::string s = cidr_to_string(cidr.ip(), cidr.prefix());
            if (!s.empty()) result.push_back(s);
        }
        break;
    }
    return result;
}

} // namespace geochecker
