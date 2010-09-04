#include "config.hpp"
#include "player_ban_serial.hpp"
#include "core/module_c.hpp"

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
    preban_timeout = 24 * 60 * 60; // 12 часов
    bans_timeout = 4 * 24 * 60 * 60; // 4 дня

    whitelist_ips.clear();
    whitelist_serials.clear();
    whitelist_as_nums.clear();
    whitelist_ban_items.clear();
    
    blacklist_ips.clear();
    blacklist_serials.clear();
    blacklist_as_nums.clear();
    blacklist_ban_items.clear();
}

void player_ban_serial::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(is_serial_bans_active);
    SERIALIZE_ITEM(preban_min_count);
    SERIALIZE_ITEM(preban_timeout);
    SERIALIZE_ITEM(bans_timeout);
    SERIALIZE_ITEM(whitelist_ips);
    SERIALIZE_ITEM(whitelist_serials);
    SERIALIZE_ITEM(whitelist_as_nums);
    SERIALIZE_ITEM(whitelist_ban_items);
    SERIALIZE_ITEM(blacklist_ips);
    SERIALIZE_ITEM(blacklist_serials);
    SERIALIZE_ITEM(blacklist_as_nums);
    SERIALIZE_ITEM(blacklist_ban_items);
}

void player_ban_serial::configure_post() {
    prebans.set_time_out(preban_timeout * time_base::millisecond_t(1000));
    bans.set_time_out(bans_timeout * time_base::millisecond_t(1000));
}

// Истина - разрешить подключение
// Если хотябы 1 метод вырнул ложь - игрок кикается, никакие другие события не вызываются
bool player_ban_serial::on_rejectable_connect(player_ptr_t const& player_ptr) {
    if (is_serial_bans_active) {
        player_ban_serial_item_t item = player_ban_serial_item_t(player_ptr);
        ban_reason_t::ban_reason_e reason;
        if (is_banned(item, player_ptr->geo_get_ip_info().ip_string, reason)) {
            // Серийный номер забанен

            messages_item msg_item;

            msg_item.get_params()
                ("player_name_full", player_ptr->name_get_full())
                ("player_geo_long_desc", player_ptr->geo_get_long_desc())
                ("player_serial", player_ptr->get_serial())
                ("reason", ban_reason_t::get_reason_string(reason))
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
            pager("$(ban_serial_list_item)");
        }
    }
    pager.items_done();

    return cmd_arg_auto;
}

bool player_ban_serial::access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name) {
    return is_serial_bans_active;
}

bool player_ban_serial::is_banned(player_ban_serial_item_t const& item, std::string const& ip_string, ban_reason_t::ban_reason_e& reason) const {
    reason = ban_reason_t::reason_none;

    if (whitelist_ips.end() != whitelist_ips.find(ip_string)) {
        return false;
    }
    if (whitelist_serials.end() != whitelist_serials.find(item.serial)) {
        return false;
    }
    if (whitelist_as_nums.end() != whitelist_as_nums.find(item.as_num)) {
        return false;
    }
    if (whitelist_ban_items.end() != whitelist_ban_items.find(item)) {
        return false;
    }

    if (blacklist_ips.end() != blacklist_ips.find(ip_string)) {
        reason += ban_reason_t::reason_ip;
    }
    if (blacklist_serials.end() != blacklist_serials.find(item.serial)) {
        reason += ban_reason_t::reason_serial;
    }
    if (blacklist_as_nums.end() != blacklist_as_nums.find(item.as_num)) {
        reason += ban_reason_t::reason_as_num;
    }
    if (blacklist_ban_items.end() != blacklist_ban_items.find(item)) {
        reason += ban_reason_t::reason_ban_item;
    }
    if (bans.is_exist(item)) {
        reason += ban_reason_t::reason_ban_item_dynamic;
    }
    return ban_reason_t::reason_none != reason;
}
