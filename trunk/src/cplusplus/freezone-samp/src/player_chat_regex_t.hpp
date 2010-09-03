#ifndef PLAYER_CHAT_REGEX_T_HPP
#define PLAYER_CHAT_REGEX_T_HPP
#include <regex>
#include <string>

struct string_regex_t {
    std::string     name;
    std::string     regex_str;
    string_regex_t() {}
    
    void sync() {regex.assign(regex_str);}
    std::tr1::regex const& get_regex() const {return regex;}
private:
    std::tr1::regex regex;
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, string_regex_t& regex) {
    std::getline(is, regex.name, ';');
    std::getline(is, regex.regex_str);
    if (!is.fail()) {
        regex.sync();
    }
    return is;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <string_regex_t>: std::tr1::true_type {};
} // namespace serialization {

#endif // PLAYER_CHAT_REGEX_T_HPP
