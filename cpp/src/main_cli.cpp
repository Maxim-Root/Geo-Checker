// CLI-only Geo Checker (no Qt) - builds with just Protobuf
#include "dat_parser.hpp"
#include "geochecker_types.hpp"

#include <iostream>
#include <string>

void printUsage(const char* prog) {
    std::cerr << "Usage:\n"
              << "  " << prog << " search <geosite.dat> <domain>     - Find categories for domain\n"
              << "  " << prog << " domains <geosite.dat> <tag>        - Get domains (e.g. geosite:youtube)\n"
              << "  " << prog << " geoip <geoip.dat> <tag>             - Get IP ranges (e.g. geoip:ru)\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }

    std::string cmd = argv[1];
    std::string path = argv[2];
    std::string arg = argv[3];

    if (cmd == "search") {
        auto data = geochecker::load_geosite(path);
        if (!data) {
            std::cerr << "Failed to load geosite: " << path << "\n";
            return 1;
        }
        auto cats = geochecker::search_domain_in_geosite(data.get(), arg);
        for (const auto& c : cats) std::cout << "geosite:" << c << "\n";
        if (cats.empty()) std::cout << "Not found\n";
    } else if (cmd == "domains") {
        auto data = geochecker::load_geosite(path);
        if (!data) {
            std::cerr << "Failed to load geosite: " << path << "\n";
            return 1;
        }
        auto domains = geochecker::get_domains_from_geosite(data.get(), arg);
        std::cout << "Domains (" << domains.size() << "):\n\n";
        for (const auto& d : domains) std::cout << d << "\n";
    } else if (cmd == "geoip") {
        auto data = geochecker::load_geoip(path);
        if (!data) {
            std::cerr << "Failed to load geoip: " << path << "\n";
            return 1;
        }
        auto ips = geochecker::get_ips_from_geoip(data.get(), arg);
        std::cout << "IP/CIDR (" << ips.size() << "):\n\n";
        for (const auto& ip : ips) std::cout << ip << "\n";
    } else {
        printUsage(argv[0]);
        return 1;
    }
    return 0;
}
