#include "geochecker_types.hpp"
#include "routercommon.pb.h"
#include <google/protobuf/message.h>

namespace geochecker {

GeoSiteData::~GeoSiteData() {
    delete data_;
}

GeoIPData::~GeoIPData() {
    delete data_;
}

} // namespace geochecker
