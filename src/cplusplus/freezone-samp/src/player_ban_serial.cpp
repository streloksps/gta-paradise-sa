#include "config.hpp"
#include "player_ban_serial.hpp"
#include "core/module_c.hpp"

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

REGISTER_IN_APPLICATION(player_ban_serial);

player_ban_serial::ptr player_ban_serial::instance() {
    return application::instance()->get_item<player_ban_serial>();
}

player_ban_serial::player_ban_serial() {
}

player_ban_serial::~player_ban_serial() {
}

void player_ban_serial::create() {
    command_add("ban_serial_unban", std::tr1::bind(&player_ban_serial::cmd_ban_serial_unban, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&player_ban_serial::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("ban_serial_list", std::tr1::bind(&player_ban_serial::cmd_ban_serial_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&player_ban_serial::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

void player_ban_serial::configure_pre() {
    is_serial_bans_active = false;
    preban_min_count = 3;
    preban_timeout = 12 * 60 * 60 * 1000; // 12 часов
    bans_timeout = 4 * 24 * 60 * 60 * 1000; // 4 дня
}

void player_ban_serial::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(is_serial_bans_active);
    SERIALIZE_ITEM(preban_min_count);
    SERIALIZE_ITEM(preban_timeout);
    SERIALIZE_ITEM(bans_timeout);
    SERIALIZE_ITEM(perma_serials);
    SERIALIZE_ITEM(perma_as_nums);
    SERIALIZE_ITEM(perma_ban_items);
}

void player_ban_serial::configure_post() {
    prebans.set_time_out(preban_timeout);
    bans.set_time_out(bans_timeout);
}

// Истина - разрешить подключение
// Если хотябы 1 метод вырнул ложь - игрок кикается, никакие другие события не вызываются
bool player_ban_serial::on_rejectable_connect(player_ptr_t const& player_ptr) {
    if (is_serial_bans_active) {
        player_ban_serial_item_t item = player_ban_serial_item_t(player_ptr);
        if (is_banned(item)) {
            // Серийный номер забанен

            messages_item msg_item;

            msg_item.get_params()
                ("player_name_full", player_ptr->name_get_full())
                ("player_geo_long_desc", player_ptr->geo_get_long_desc())
                ("player_serial", player_ptr->get_serial())
                ;

            msg_item.get_sender()
                ("$(player_ban_serial_reject)", msg_player(player_ptr))
                ("$(player_ban_serial_reject_admins)", msg_players_all_admins)
                ("$(player_ban_serial_reject_log)", msg_log)
                ;

            return false;
        }
    }

    return true;
}

void player_ban_serial::ban_serial(player_ptr_t const& player_ptr) {
    if (!is_serial_bans_active) {
        return;
    }
    player_ban_serial_item_t item = player_ban_serial_item_t(player_ptr);
    int count = 0;
    if (prebans.is_exist(item)) {
        count = ++prebans.get(item);
    }
    else {
        prebans.insert(item, 1);
        count = 1;
    }

    if (count >= preban_min_count && !bans.is_exist(item)) {
        messages_item msg_item;

        msg_item.get_params()(item);

        msg_item.get_sender()
            ("$(ban_serial_ban_auto_admins)", msg_players_all_admins)
            ("$(ban_serial_ban_auto_log)", msg_log)
            ;

        bans.insert(std::tr1::bind(&player_ban_serial::on_unban_serial, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3), item);
    }
}

void player_ban_serial::on_timer15000() {
    // Удаляем из списка банов те, у которых прошел тайм-аут
    prebans.erase_timeouts();
    bans.erase_timeouts(std::tr1::bind(&player_ban_serial::on_unban_serial, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3));
}

void player_ban_serial::on_unban_serial(player_ban_serial_item_t const& item, time_base::millisecond_t const& last_update, time_outs_dummy_val const& dummy) {
    messages_item msg_item;

    msg_item.get_params()(item);

    msg_item.get_sender()
        ("$(ban_serial_unban_auto_log)", msg_log);
    ;
}

command_arguments_rezult player_ban_serial::cmd_ban_serial_unban(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    player_ban_serial_item_t item;
    {
        std::istringstream iss(arguments);
        iss>>item.as_num>>item.serial;
        boost::trim(item.serial);
        if (iss.fail() || !iss.eof() || item.serial.empty()) {
            return cmd_arg_syntax_error;
        }
    }

    params(item);

    if (!bans.is_exist(item)) {
        pager("$(ban_serial_unban_error_not_found)");
        return cmd_arg_ok;
    }

    bans.erase(item);
    pager("$(ban_serial_unban_ok)");
    sender("$(ban_serial_unban_ok_log)", msg_log);
    return cmd_arg_ok;
}

command_arguments_rezult player_ban_serial::cmd_ban_serial_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    bans_t::items_t const& bans_items = bans.get_items();
    time_base::millisecond_t tick_count = time_base::tick_count_milliseconds();

    params
        ("count", bans_items.size())
        ;
    if (bans_items.empty()) {
        pager.items_add("$(ban_serial_list_empty)");
    }
    else {
        pager.set_header_text(0, "$(ban_serial_list_header)");
        BOOST_FOREACH(bans_t::items_t::value_type const& bans_item, bans_items) {
            params(bans_item.first);
            params
                ("remain", (static_cast<time_base::millisecond_t>(bans_timeout) - tick_count + bans_item.second.last_update) / 1000)
                ;
            pager.items_add("$(ban_serial_list_item)");
        }
    }
    pager.items_done();

    return cmd_arg_auto;
}

bool player_ban_serial::access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name) {
    return is_serial_bans_active;
}

bool player_ban_serial::is_banned(player_ban_serial_item_t const& item) const {
    if (perma_serials.end() != perma_serials.find(item.serial)) {
        return true;
    }
    if (perma_as_nums.end() != perma_as_nums.find(item.as_num)) {
        return true;
    }
    if (perma_ban_items.end() != perma_ban_items.find(item)) {
        return true;
    }
    return bans.is_exist(item);
}
