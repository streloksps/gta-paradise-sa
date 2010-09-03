#include "config.hpp"
#include "geo_info_getter_city.hpp"
#include "../geo_ip_info.hpp"
#include "maxmind-geoip/GeoIP.h"
#include "maxmind-geoip/GeoIPCity.h"

namespace geo {
    info_getter_city::info_getter_city(GeoIPTag* gi):info_getter_db(gi) {
    }

    info_getter_city::~info_getter_city() {
    }

    void info_getter_city::process_ip_info(ip_info& info) {
        if (GeoIPRecord *gir = GeoIP_record_by_addr(db, info.ip_string.c_str())) {
            if (!info.country_code2 && gir->country_code) {
                info.country_code2.reset(gir->country_code);
            }
            if (!info.country_code3 && gir->country_code3) {
                info.country_code3.reset(gir->country_code3);
            }
            if (!info.country_name_en && gir->country_name) {
                info.country_name_en.reset(gir->country_name);
            }
            if (!info.city_en && gir->city) {
                info.city_en.reset(gir->city);
            }
            if (!info.latitude) {
                info.latitude.reset(gir->latitude);
            }
            if (!info.longitude) {
                info.longitude.reset(gir->longitude);
            }

            GeoIPRecord_delete(gir);
        }
    }

    std::string info_getter_city::get_db_info_prefix() const {
        return "City";
    }
} // namespace geo {
