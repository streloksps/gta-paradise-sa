#ifndef PLAYER_BAN_SERIAL_ITEM_HPP
#define PLAYER_BAN_SERIAL_ITEM_HPP
#include "basic_types.hpp"
#include "core/module_h.hpp"

struct player_ban_serial_item_t {
    int as_num;
    std::string serial;

    player_ban_serial_item_t();
    player_ban_serial_item_t(int as_num, std::string const& serial);
    player_ban_serial_item_t(player_ptr_t const& player_ptr);
    bool operator<(player_ban_serial_item_t const& right) const {
        if (as_num == right.as_num) return serial < right.serial;
        return as_num < right.as_num;
    }
    void dump_to_params(messages_params& params) const;
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, player_ban_serial_item_t& item) {
    is>>item.as_num>>item.serial;
    if (!is.eof()) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <player_ban_serial_item_t>: std::tr1::true_type {};
} // namespace serialization {
#endif // PLAYER_BAN_SERIAL_ITEM_HPP
