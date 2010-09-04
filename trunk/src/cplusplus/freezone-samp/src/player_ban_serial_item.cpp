#include "config.hpp"
#include "player_ban_serial_item.hpp"

player_ban_serial_item_t::player_ban_serial_item_t(): as_num(as_num) {
}

player_ban_serial_item_t::player_ban_serial_item_t(int as_num, std::string const& serial): serial(serial), as_num(as_num) {
}

player_ban_serial_item_t::player_ban_serial_item_t(player_ptr_t const& player_ptr) {
    serial = player_ptr->get_serial();
    as_num = player_ptr->geo_get_ip_info().get_as_num();
}

void player_ban_serial_item_t::dump_to_params(messages_params& params) const {
    params
        ("as_num", as_num)
        ("serial", serial)
        ;
}
