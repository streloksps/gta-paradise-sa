#include "config.hpp"
#include "geo_info_getter_country.hpp"
#include "../geo_ip_info.hpp"
#include "maxmind-geoip/GeoIP.h"

namespace geo {
    info_getter_country::info_getter_country(GeoIPTag* gi):info_getter_db(gi) {
    }

    info_getter_country::~info_getter_country() {
    }

    void info_getter_country::process_ip_info(ip_info& info) {
        if (!info.country_code2) {
            char const* rezult = GeoIP_country_code_by_addr(db, info.ip_string.c_str());
            if (rezult) {
                info.country_code2.reset(rezult);
            }
        }
        if (!info.country_code3) {
            char const* rezult = GeoIP_country_code3_by_addr(db, info.ip_string.c_str());
            if (rezult) {
                info.country_code3.reset(rezult);
            }
        }
        if (!info.country_name_en) {
            char const* rezult = GeoIP_country_name_by_addr(db, info.ip_string.c_str());
            if (rezult) {
                info.country_name_en.reset(rezult);
            }
        }
    }

    std::string info_getter_country::get_db_info_prefix() const {
        return "Country";
    }
} // namespace geo {
