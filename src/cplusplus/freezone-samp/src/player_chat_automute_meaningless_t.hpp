#ifndef PLAYER_CHAT_AUTOMUTE_MEANINGLESS_T_HPP
#define PLAYER_CHAT_AUTOMUTE_MEANINGLESS_T_HPP
#include <string>
#include <istream>
#include <boost/algorithm/string.hpp>

struct player_chat_automute_meaningless_chars_type_t {
    int         min_count;          // Минимальное число символов из заданного класса, чтобы начать считать метрику (-1 - всегда считаем)
    float       min_relative_count; // Минимальное относительное число символов, чтобы начать считать метрику (-1.0f - всегда считаем)
    float       mentic_factor;      // Коэффициент метрики, для класса символов
    std::string chars;              // Символы, класса символов
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, player_chat_automute_meaningless_chars_type_t& chars_type) {
    is>>chars_type.min_count>>chars_type.min_relative_count>>chars_type.mentic_factor;
    std::getline(is, chars_type.chars);
    boost::trim(chars_type.chars);
    if (is, chars_type.chars.empty()) is, chars_type.chars = " ";
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <player_chat_automute_meaningless_chars_type_t>: std::tr1::true_type {};
} // namespace serialization {

#endif // PLAYER_CHAT_AUTOMUTE_MEANINGLESS_T_HPP
