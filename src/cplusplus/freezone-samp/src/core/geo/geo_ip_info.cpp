#include "config.hpp"
#include "geo_ip_info.hpp"
#include "detail/geo_info_getter_ip_check.hpp"
#include "boost/format.hpp"

namespace geo {
    ip_info::ip_info():is_ip_string_valid_ip(false) {
    }

    ip_info::~ip_info() {
    }

    int ip_info::get_as_num() const {
        if (as_num) {
            return as_num.get();
        }
        else return 0;
    }

    std::string ip_info::get_as_num_str() const {
        if (as_num) {
            return (boost::format("AS%1%")%as_num.get()).str();
        }
        else return "ASnone";
    }

    std::string ip_info::get_long_desc() const {
        if (is_ip_string_valid_ip) {
            //195.19.37.71,Russia,Moscow,AS1919 BMSTU
            return ip_string 
                + "," + (country_name_en ? country_name_en.get() : "no_country") 
                + "," + (city_en ? city_en.get() : "no_city") 
                + "," + get_as_num_str() + (as_description ? " " + as_description.get() : "")
                + "," + (organization ? organization.get() : "no_org");
        }
        else {
            return ip_string;
        }
    }

    std::string get_ip_string(unsigned int ip) {
        return (boost::format("%1%.%2%.%3%.%4%") % ((ip >> 24) & 0xFF) % ((ip >> 16) & 0xFF) % ((ip >> 8) & 0xFF) % (ip & 0xFF)).str();
    }

    unsigned int get_ip(std::string const& ip_string) {
        return info_getter_ip_check::get_ip(ip_string);
    }
} // namespace geo {
