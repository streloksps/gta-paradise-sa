#include "config.hpp"
#include "geo_info_getter_asnum.hpp"
#include "maxmind-geoip/GeoIP.h"
#include "../geo_ip_info.hpp"
#include <boost/algorithm/string.hpp>

namespace geo {
    namespace {
        int str_to_int(std::string const& str) {
            return atoi(str.c_str());
        }
        std::string get_description(std::string const& src_desc) {
            std::string rezult = boost::replace_all_copy(src_desc, ",", " ");
            if (rezult.length() > 64) {
                rezult = rezult.substr(0, 64);
            }
            boost::trim(rezult);
            boost::erase_all(rezult, "(");
            boost::erase_all(rezult, ")");
            return rezult;
        }
    }
    info_getter_asnum::info_getter_asnum(GeoIPTag* gi):info_getter_db(gi) {
    }

    info_getter_asnum::~info_getter_asnum() {
    }

    void info_getter_asnum::process_ip_info(ip_info& info) {
        if (!info.as_num) {
            char const* rezult = GeoIP_name_by_addr(db, info.ip_string.c_str());
            if (rezult) {
                std::string str(rezult);
                if (0 == str.find("AS")) {
                    unsigned int pos = str.find(" ");
                    if (std::string::npos == pos && pos > 2) {
                        info.as_num.reset(str_to_int(str.substr(2)));
                    }
                    else {
                        info.as_num.reset(str_to_int(str.substr(2,pos-2)));
                        info.as_description.reset(get_description(str.substr(pos+1)));
                    }
                }
            }
        }

    }

    std::string info_getter_asnum::get_db_info_prefix() const {
        return "Asnum";
    }
} // namespace geo {
