#include "config.hpp"
#include "geo_info_getter_org.hpp"
#include "maxmind-geoip/GeoIP.h"
#include "../geo_ip_info.hpp"
#include <boost/algorithm/string.hpp>

namespace geo {
    info_getter_org::info_getter_org(GeoIPTag* gi):info_getter_db(gi) {
    }

    info_getter_org::~info_getter_org() {
    }

    void info_getter_org::process_ip_info(ip_info& info) {
        if (!info.organization) {
            char const* rezult = GeoIP_name_by_addr(db, info.ip_string.c_str());
            if (rezult) {
                std::string org(rezult);
                boost::replace_all(org, ",", " ");
                boost::trim(org);
                boost::erase_all(org, "(");
                boost::erase_all(org, ")");
                info.organization.reset(org);
            }
        }
    }

    std::string info_getter_org::get_db_info_prefix() const {
        return "Organization";
    }
} // namespace geo {
