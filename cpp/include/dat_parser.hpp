#pragma once

#include <string>
#include <vector>
#include <memory>

// Forward declarations - generated protobuf
namespace routercommon {
class GeoSiteList;
class GeoIPList;
}

namespace geochecker {

class GeoSiteData;
class GeoIPData;

// Load geosite.dat
std::unique_ptr<GeoSiteData> load_geosite(const std::string& path);

// Load geoip.dat
std::unique_ptr<GeoIPData> load_geoip(const std::string& path);

// Find categories for domain, returns tags like ["youtube", "google"]
std::vector<std::string> search_domain_in_geosite(const GeoSiteData* geosite, const std::string& domain);

// Get all domains from geosite tag (geosite:youtube etc)
std::vector<std::string> get_domains_from_geosite(const GeoSiteData* geosite, const std::string& tag);

// Resolve domain to IPv4 addresses
std::vector<std::string> resolve_domain_to_ip(const std::string& domain, double timeout_sec = 2.0);

// Get IP/CIDR ranges from geoip by tag (geoip:ru etc)
std::vector<std::string> get_ips_from_geoip(const GeoIPData* geoip, const std::string& tag);

} // namespace geochecker
