#pragma once

#include <memory>

namespace routercommon {
class GeoSiteList;
class GeoIPList;
}

namespace geochecker {

// Wrapper around GeoSiteList for RAII
class GeoSiteData {
public:
    explicit GeoSiteData(routercommon::GeoSiteList* data) : data_(data) {}
    ~GeoSiteData();

    GeoSiteData(const GeoSiteData&) = delete;
    GeoSiteData& operator=(const GeoSiteData&) = delete;

    routercommon::GeoSiteList* get() { return data_; }
    const routercommon::GeoSiteList* get() const { return data_; }

private:
    routercommon::GeoSiteList* data_;
};

// Wrapper around GeoIPList for RAII
class GeoIPData {
public:
    explicit GeoIPData(routercommon::GeoIPList* data) : data_(data) {}
    ~GeoIPData();

    GeoIPData(const GeoIPData&) = delete;
    GeoIPData& operator=(const GeoIPData&) = delete;

    routercommon::GeoIPList* get() { return data_; }
    const routercommon::GeoIPList* get() const { return data_; }

private:
    routercommon::GeoIPList* data_;
};

} // namespace geochecker
