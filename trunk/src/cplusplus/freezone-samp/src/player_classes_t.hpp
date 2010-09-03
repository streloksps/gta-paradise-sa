#ifndef PLAYER_CLASSES_T_HPP
#define PLAYER_CLASSES_T_HPP
#include <istream>
#include <string>

struct player_class_def_t {
    int skin_id;
    bool is_female;
    std::string name;
    player_class_def_t(int skin_id = 0, bool is_female = false, std::string const& name = "no_name"): skin_id(skin_id), is_female(is_female), name(name) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, player_class_def_t& cl) {
    std::basic_string<char_t, traits_t> buff;
    std::getline(is, buff, ';');
    {
        std::basic_istringstream<char_t> iss(buff);
        iss>>cl.skin_id;
        if (iss.fail() || !iss.eof()) {
            is.setstate(std::ios_base::failbit);
        }
    }
    std::getline(is, buff);
    return is;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <player_class_def_t>: std::tr1::true_type {};
} // namespace serialization {
#endif // PLAYER_CLASSES_T_HPP
