#ifndef PLAYER_COLOR_T_HPP
#define PLAYER_COLOR_T_HPP
#include <istream>
#include <ostream>
#include <string>
#include <boost/algorithm/string.hpp>

struct player_color_t {
    int color;
    std::string name;
    player_color_t(int color = 0xFFFFFF, std::string const& name = "Без имени"): color(color), name(name) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, player_color_t& color) {
    is>>color.color;
    std::getline(is, color.name);
    boost::trim(color.name);
    return is;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, player_color_t const& color) {
    return os<<color.color<<" "<<color.name;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <player_color_t>: std::tr1::true_type {};
    template <> struct is_streameble_write<player_color_t>: std::tr1::true_type {};
} // namespace serialization {
#endif // PLAYER_COLOR_T_HPP
