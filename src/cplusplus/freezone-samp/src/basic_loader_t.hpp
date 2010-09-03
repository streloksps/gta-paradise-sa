#ifndef BASIC_LOADER_T_HPP
#define BASIC_LOADER_T_HPP
#include <string>
#include <istream>
#include <boost/algorithm/string.hpp>

struct basic_loader_item_t {
    int world;
    std::string file_name;
    basic_loader_item_t():world(0) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, basic_loader_item_t& item) {
    is>>item.world;
    if (is.eof()) {
        is.setstate(std::ios_base::failbit);
    }
    else {
        std::getline(is, item.file_name);
        boost::trim(item.file_name);
        if (item.file_name.empty()) {
            is.setstate(std::ios_base::failbit);
        }
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <basic_loader_item_t>: std::tr1::true_type {};
} // namespace serialization {

#endif // BASIC_LOADER_T_HPP
