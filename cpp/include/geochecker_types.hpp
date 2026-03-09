#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace routercommon {
class GeoSite;
class GeoIP;
}

namespace geochecker {

// Lazy-parsed geosite.dat: keeps raw bytes in memory (~10-15 MB)
// and parses individual entries on demand instead of the full protobuf tree (~200-300 MB)
class GeoSiteData {
public:
    struct EntryLocation {
        std::string category;
        uint32_t offset;
        uint32_t length;
    };

    GeoSiteData(std::string raw_data, std::vector<EntryLocation> entries);
    ~GeoSiteData();

    GeoSiteData(const GeoSiteData&) = delete;
    GeoSiteData& operator=(const GeoSiteData&) = delete;

    size_t entry_count() const { return entries_.size(); }
    const std::string& category(size_t idx) const { return entries_[idx].category; }

    // Parse a single entry on demand — caller owns the result
    std::unique_ptr<routercommon::GeoSite> parse_entry(size_t idx) const;

    // O(1) category lookup, returns -1 if not found
    int find_category(const std::string& cat) const;

private:
    std::string raw_data_;
    std::vector<EntryLocation> entries_;
    std::unordered_map<std::string, size_t> cat_lookup_;
};

// Lazy-parsed geoip.dat: same approach
class GeoIPData {
public:
    struct EntryLocation {
        std::string category;
        uint32_t offset;
        uint32_t length;
    };

    GeoIPData(std::string raw_data, std::vector<EntryLocation> entries);
    ~GeoIPData();

    GeoIPData(const GeoIPData&) = delete;
    GeoIPData& operator=(const GeoIPData&) = delete;

    size_t entry_count() const { return entries_.size(); }
    const std::string& category(size_t idx) const { return entries_[idx].category; }

    std::unique_ptr<routercommon::GeoIP> parse_entry(size_t idx) const;
    int find_category(const std::string& cat) const;

private:
    std::string raw_data_;
    std::vector<EntryLocation> entries_;
    std::unordered_map<std::string, size_t> cat_lookup_;
};

} // namespace geochecker
