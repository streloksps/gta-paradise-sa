#include "config.hpp"
#include "geo_info_getter_ip_check.hpp"
#include "../geo_ip_info.hpp"
extern "C" unsigned long _GeoIP_addr_to_num(const char *addr);

namespace geo {
    info_getter_ip_check::info_getter_ip_check() {
    }

    info_getter_ip_check::~info_getter_ip_check() {
    }

    void info_getter_ip_check::process_ip_info(ip_info& info) {
        info.is_ip_string_valid_ip = 0 != get_ip(info.ip_string);
    }

    unsigned int info_getter_ip_check::get_ip(std::string const& ip_string) {
        return _GeoIP_addr_to_num(ip_string.c_str());
    }

    bool info_getter_ip_check::get_db_info(std::string& db_info_string) const {
        return false;
    }
} // namespace geo {
