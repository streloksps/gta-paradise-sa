#ifndef SERIALIZATION_REGEX_T_HPP
#define SERIALIZATION_REGEX_T_HPP
#include <istream>
#include <ostream>
#include <regex>

template <typename char_t, typename traits_t, typename rx_traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, std::tr1::basic_regex<char_t, rx_traits_t>& regex) {
    std::basic_string<char_t, traits_t> buff;
    std::getline(is, buff);
    if (!is.fail()) {
        try {
            regex.assign(buff);
        }
        catch (std::exception const&) {
            assert(false);
            is.setstate(std::ios_base::failbit);
        }
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <std::tr1::regex>: std::tr1::true_type {};
} // namespace serialization {

#endif // SERIALIZATION_REGEX_T_HPP
