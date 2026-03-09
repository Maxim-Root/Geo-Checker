#include "geochecker_types.hpp"
#include "routercommon.pb.h"

namespace geochecker {

// --- GeoSiteData ---

GeoSiteData::GeoSiteData(std::string raw_data, std::vector<EntryLocation> entries)
    : raw_data_(std::move(raw_data)), entries_(std::move(entries)) {
    for (size_t i = 0; i < entries_.size(); ++i)
        cat_lookup_[entries_[i].category] = i;
}

GeoSiteData::~GeoSiteData() = default;

std::unique_ptr<routercommon::GeoSite> GeoSiteData::parse_entry(size_t idx) const {
    if (idx >= entries_.size()) return nullptr;
    auto entry = std::make_unique<routercommon::GeoSite>();
    if (!entry->ParseFromArray(raw_data_.data() + entries_[idx].offset,
                               static_cast<int>(entries_[idx].length)))
        return nullptr;
    return entry;
}

int GeoSiteData::find_category(const std::string& cat) const {
    auto it = cat_lookup_.find(cat);
    return it != cat_lookup_.end() ? static_cast<int>(it->second) : -1;
}

// --- GeoIPData ---

GeoIPData::GeoIPData(std::string raw_data, std::vector<EntryLocation> entries)
    : raw_data_(std::move(raw_data)), entries_(std::move(entries)) {
    for (size_t i = 0; i < entries_.size(); ++i)
        cat_lookup_[entries_[i].category] = i;
}

GeoIPData::~GeoIPData() = default;

std::unique_ptr<routercommon::GeoIP> GeoIPData::parse_entry(size_t idx) const {
    if (idx >= entries_.size()) return nullptr;
    auto entry = std::make_unique<routercommon::GeoIP>();
    if (!entry->ParseFromArray(raw_data_.data() + entries_[idx].offset,
                               static_cast<int>(entries_[idx].length)))
        return nullptr;
    return entry;
}

int GeoIPData::find_category(const std::string& cat) const {
    auto it = cat_lookup_.find(cat);
    return it != cat_lookup_.end() ? static_cast<int>(it->second) : -1;
}

} // namespace geochecker
