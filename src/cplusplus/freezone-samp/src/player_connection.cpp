#include "config.hpp"
#include "player_connection.hpp"
#include <functional>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "core/application.hpp"
#include "core/player/player.hpp"
#include "core/players.hpp"
#include "core/messages/messages_storage.hpp"
#include "core/command/command_adder.hpp"
#include "core/geo/geo_data_base.hpp"
#include "core/samp/samp_api.hpp"
#include "rules.hpp"
#include "player_chat.hpp"
#include "core/utility/erase_if.hpp"
#include "core/container/container_handlers.hpp"
#include "player_ban_serial.hpp"

REGISTER_IN_APPLICATION(player_connection);

player_connection::ptr player_connection::instance() {
    return application::instance()->get_item<player_connection>();
}

player_connection::player_connection()
:players_max(0)
,is_max_now(false)
,players_max_start_time(0)
{
}

player_connection::~player_connection() {
}

void player_connection::create() {
    players_max = samp::api::instance()->get_max_players();

    command_add("countries", &player_connection::cmd_countries, cmd_info_only);
    command_add("geo", &player_connection::cmd_geo, cmd_info_only);

    command_add("kick", std::tr1::bind(&player_connection::cmd_kick, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
    command_add("ban", std::tr1::bind(&player_connection::cmd_ban, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));

    command_add("admins_list", std::tr1::bind(&player_connection::cmd_admins_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
}

void player_connection::configure_pre() {
    cmd_kick_def_reason = "п2";
    cmd_ban_def_reason = "п1";

    is_connectmsg_in_chat = true;
    connect_death_reason = 200;
    disconnect_death_reason = 201;
}

void player_connection::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(cmd_kick_def_reason);
    SERIALIZE_ITEM(cmd_ban_def_reason);

    SERIALIZE_ITEM(is_connectmsg_in_chat);
    SERIALIZE_ITEM(connect_death_reason);
    SERIALIZE_ITEM(disconnect_death_reason);
}

void player_connection::configure_post() {

}

void player_connection::on_connect(player_ptr_t const& player_ptr) {
    player_connection_item::ptr item(new player_connection_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);

    container_execute_handlers(application::instance(), &server_events::on_players_count_change_i::on_players_count_change, players::instance()->get_players_count(), players_max, server_events::players_count_change_reason_connect);
}

void player_connection::on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) {
    container_execute_handlers(application::instance(), &server_events::on_players_count_change_i::on_players_count_change, players::instance()->get_players_count() - 1, players_max, server_events::players_count_change_reason_disconnect);
}

std::string player_connection::geo_get_country(player_ptr_t const& player_ptr) {
    geo::ip_info const& geo_ip_info = player_ptr->geo_get_ip_info();
    if (geo_ip_info.country_name_ru) {
        return "(" + geo_ip_info.country_name_ru.get() + ")";
    }
    else {
        return "";
    }
}

command_arguments_rezult player_connection::cmd_countries(player_ptr_t const& /*player_ptr*/, std::string const& /*arguments*/, messages_pager& pager, messages_sender const& /*sender*/, messages_params& params) {
    typedef std::map<std::string, std::vector<std::string> > items_t;

    players_t players_vector = players::instance()->get_players();
    items_t items;
    BOOST_FOREACH(player_ptr_t const& player_ptr, players_vector) {
        items[cmd_countries_get_country(player_ptr)].push_back(player_ptr->name_get_full());
    }
    pager.set_header_text(0, "$(cmd_countries_header)");
    BOOST_FOREACH(items_t::value_type const& country, items) {
        params
            ("country", country.first)
            ("country_count", country.second.size());
        pager.set_header_text(1, "$(cmd_countries_header_country)");
        BOOST_FOREACH(std::string const& item, country.second) {
            params("item", item);
            pager.items_add("$(cmd_countries_item)");
        }
        pager.items_done();
    }

    return cmd_arg_auto;
}

std::string player_connection::cmd_countries_get_country(player_ptr_t const& player_ptr) {
    geo::ip_info const& geo_ip_info = player_ptr->geo_get_ip_info();
    if (geo_ip_info.country_name_ru) {
        return geo_ip_info.country_name_ru.get();
    }
    else {
        return messages_storage::instance()->get_msg_buff()->process_all_vars("$(cmd_countries_unknown)");
    }
}

command_arguments_rezult player_connection::cmd_geo(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    { // Парсинг ip адреса
        std::istringstream iss(arguments);
        std::string ip_string;
        iss>>ip_string;
        if (!iss.fail() && iss.eof()) {
            geo::ip_info info = geo::data_base::instance()->get_info(ip_string);
            if (info.is_ip_string_valid_ip) {
                params
                    ("player_name", player_ptr->name_get_full())
                    ("geo_long_desc", info.get_long_desc());
                pager("$(cmd_geo_ip_to_player)");
                sender("$(cmd_geo_ip_to_log)", msg_log);
                return cmd_arg_ok;
            }
        }
    }
    { // Парсинг ид игрока
        std::istringstream iss(arguments);
        unsigned int geo_player_id;
        iss>>geo_player_id;
        if (!iss.fail() && iss.eof()) {
            player::ptr geo_player_ptr = players::instance()->get_player(geo_player_id);
            if (!geo_player_ptr) {
                params("player_id", geo_player_id);
                pager("$(player_bad_id)");
                return cmd_arg_process_error;
            }
            cmd_geo_player_impl(player_ptr, geo_player_ptr, pager, sender, params);
            return cmd_arg_ok;
        }
    }
    return cmd_arg_syntax_error;
}

void player_connection::cmd_geo_player_impl(player_ptr_t const& player_ptr, player_ptr_t const& geo_player_ptr, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    params
        ("player_name", player_ptr->name_get_full())
        ("geo_player_name", geo_player_ptr->name_get_full())
        ("geo_long_desc", geo_player_ptr->geo_get_long_desc());
    pager("$(cmd_geo_player_to_player)");
    sender("$(cmd_geo_player_to_log)", msg_log);
}

command_arguments_rezult player_connection::cmd_kick(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int kick_player_id;
    std::string kick_reason;
    { // Парсинг
        std::istringstream iss(arguments);
        iss>>kick_player_id;
        if (iss.fail()) {
            return cmd_arg_syntax_error;
        }
        if (iss.eof() && !cmd_kick_def_reason.empty()) {
            kick_reason = cmd_kick_def_reason;
        }
        else {
            std::getline(iss, kick_reason);
            boost::trim(kick_reason);
            if (kick_reason.empty()) {
                return cmd_arg_syntax_error;
            }
        }
    }

    player_chat::instance()->process_chat_text(kick_reason);

    player::ptr kick_player_ptr = players::instance()->get_player(kick_player_id);
    if (!kick_player_ptr) {
        params("player_id", kick_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    player_connection_item::ptr kick_player_item_ptr = kick_player_ptr->get_item<player_connection_item>();

    std::string player_reason = get_rules_string(kick_reason);
    params
        ("admin_name_full", player_ptr->name_get_full())
        ("reason", player_reason)
        ("reason_raw", kick_reason)
        ("player_name_full", kick_player_ptr->name_get_full())
        ("player_geo_long_desc", kick_player_ptr->geo_get_long_desc())
        ("player_time_connection", kick_player_ptr->time_connection_get())
        ("player_time_ingame", (int)(!kick_player_item_ptr->is_first_spawn ? std::time(0) - kick_player_item_ptr->first_spawn_time : -1))
        ("player_serial", kick_player_ptr->get_serial())
        ;

    if (!kick_player_item_ptr->get_is_first_spawn()) {
        // Игрок успел побывать в игре - печатаем всем игрокам сообщение
        kick_player_item_ptr->show_disconnect_msg(player_connection_item::disconnect_msg_kick, player_reason);
    }
    sender
        ("$(kick_admins)", msg_players_all_admins)
        ("$(kick_log)", msg_log);

    kick_player_item_ptr->hide_disconnect_msg();
    kick_player_ptr->kick();
    return cmd_arg_ok;
}

command_arguments_rezult player_connection::cmd_ban(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int ban_player_id;
    std::string ban_reason;
    { // Парсинг
        std::istringstream iss(arguments);
        iss>>ban_player_id;
        if (iss.fail()) {
            return cmd_arg_syntax_error;
        }
        if (iss.eof() && !cmd_ban_def_reason.empty()) {
            ban_reason = cmd_ban_def_reason;
        }
        else {
            std::getline(iss, ban_reason);
            boost::trim(ban_reason);
            if (ban_reason.empty()) {
                return cmd_arg_syntax_error;
            }
        }
    }

    player_chat::instance()->process_chat_text(ban_reason);

    player::ptr ban_player_ptr = players::instance()->get_player(ban_player_id);

    if (!ban_player_ptr) {
        params("player_id", ban_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    player_connection_item::ptr ban_player_item_ptr = ban_player_ptr->get_item<player_connection_item>();

    std::string player_reason = get_rules_string(ban_reason);

    params
        ("admin_name_full", player_ptr->name_get_full())
        ("reason", player_reason)
        ("reason_raw", ban_reason)
        ("player_name_full", ban_player_ptr->name_get_full())
        ("player_geo_long_desc", ban_player_ptr->geo_get_long_desc())
        ("player_time_connection", ban_player_ptr->time_connection_get())
        ("player_time_ingame", (int)(!ban_player_item_ptr->is_first_spawn ? std::time(0) - ban_player_item_ptr->first_spawn_time : -1))
        ("player_serial", ban_player_ptr->get_serial())
        ;

    if (ban_player_ptr->group_is_in("admin")) {
        // Админов не баним
        pager("$(ban_cant_ban_admin)");
        sender("$(ban_cant_ban_admin_log)", msg_log);
        return cmd_arg_process_error;
    }

    if (!ban_player_item_ptr->get_is_first_spawn()) {
        // Игрок успел побывать в игре - печатаем всем игрокам сообщение
        ban_player_item_ptr->show_disconnect_msg(player_connection_item::disconnect_msg_ban, player_reason);
    }
    sender
        ("$(ban_admins)", msg_players_all_admins)
        ("$(ban_log)", msg_log);

    ban_player_item_ptr->hide_disconnect_msg();
    ban_player_ptr->ban((boost::format("[%1%]: %2% AS%3% {%4%}") % player_ptr->name_get() % ban_reason % ban_player_ptr->geo_get_ip_info().get_as_num() % ban_player_ptr->get_serial()).str());
    player_ban_serial::instance()->ban_serial(ban_player_ptr);

    return cmd_arg_ok;
}

command_arguments_rezult player_connection::cmd_admins_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    players_t players_list = players::instance()->get_players();
    erase_if(players_list, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&player::is_rcon_admin, std::tr1::placeholders::_1)));

    if (players_list.empty()) {
        pager("$(admins_list_rcon_empty)");
    }
    else {
        BOOST_FOREACH(player::ptr const& curr_admin_ptr, players_list) {
            params("rcon_admin_full_name", curr_admin_ptr->name_get_full());
            pager.items_add("$(admins_list_rcon_item)");
        }
        pager.items_done();
    }
    return cmd_arg_auto;
}

void player_connection::on_players_count_change(int players_count, int players_max, server_events::players_count_change_reason reason) {
    bool is_max = players_max == players_count;

    if (is_max != is_max_now) {
        is_max_now = is_max;
        messages_item msg_item;
        if (is_max) {
            players_max_start_time = std::time(0);
            msg_item.get_sender()("$(player_connection_max_players_begin_log)", msg_log);
        }
        else {
            msg_item.get_params()("time", static_cast<int>(std::time(0) - players_max_start_time));
            msg_item.get_sender()("$(player_connection_max_players_end_log)", msg_log);
        }
    }
}

player_connection_item::player_connection_item(player_connection& manager)
:is_first_spawn(true)
,manager(manager)
,is_show_disconnect(true)
,rcon_is_admin(false)
,rcon_login_time(0)
{
}

player_connection_item::~player_connection_item() {
}

void player_connection_item::on_spawn() {
    if (is_first_spawn) {
        is_first_spawn = false;
        on_first_spawn();
    }
}

void player_connection_item::on_first_spawn() {
    first_spawn_time = std::time(0);
    show_connect_ingame_msg();
}

void player_connection_item::on_connect() {
    player_ptr_t player_ptr = get_root();
    messages_item msg_item;

    msg_item.get_params()
        ("player_name_full", player_ptr->name_get_full())
        ("player_geo_long_desc", player_ptr->geo_get_long_desc())
        ("player_serial", player_ptr->get_serial())
        ;

    msg_item.get_sender()
        ("$(player_connection_connect_admins)", msg_players_all_admins)
        ("$(player_connection_connect_log)", msg_log)
        ;
}

void player_connection_item::on_disconnect(samp::player_disconnect_reason reason) {
    player_ptr_t player_ptr = get_root();
    messages_item msg_item;

    msg_item.get_params()
        ("player_name_full", player_ptr->name_get_full())
        ("player_geo_long_desc", player_ptr->geo_get_long_desc())
        ("player_time_connection", player_ptr->time_connection_get())
        ("player_time_ingame", static_cast<int>(!is_first_spawn ? std::time(0) - first_spawn_time : -1))
        ("player_serial", player_ptr->get_serial())
        ;

    // Разбаниваем админов
    if (samp::player_disconnect_reason_kick_ban == reason && get_root()->group_is_in("admin")) {
        samp::api::instance()->send_rcon_command("unbanip " + get_root()->geo_get_ip_info().ip_string);
    }

    if (is_show_disconnect) {
        if (!is_first_spawn) {
            // Игрок успел побывать в игре - печатаем всем игрокам сообщение
            if (samp::player_disconnect_reason_time_out == reason) {
                show_disconnect_msg(disconnect_msg_time_out);
            }
            else if (samp::player_disconnect_reason_quit == reason) {
                show_disconnect_msg(disconnect_msg_quit);
            }
            else if (samp::player_disconnect_reason_kick_ban == reason) {
                show_disconnect_msg(disconnect_msg_rcon);
            }
        }
        if (samp::player_disconnect_reason_time_out == reason) {
            msg_item.get_sender()("$(player_connection_disconect_time_out_admins)", msg_players_all_admins);
            msg_item.get_sender()("$(player_connection_disconect_time_out_log)", msg_log);
        }
        else if (samp::player_disconnect_reason_quit == reason) {
            msg_item.get_sender()("$(player_connection_disconect_quit_admins)", msg_players_all_admins);
            msg_item.get_sender()("$(player_connection_disconect_quit_log)", msg_log);
        }
        else if (samp::player_disconnect_reason_server_shutdown == reason) {
            msg_item.get_sender()("$(player_connection_disconect_quit_log)", msg_log);
        }
        else if (samp::player_disconnect_reason_kick_ban == reason) {
            msg_item.get_sender()("$(player_connection_disconect_kickban_admins)", msg_players_all_admins);
            msg_item.get_sender()("$(player_connection_disconect_kickban_log)", msg_log);
        }
    }
    if (rcon_is_admin) {
        // Игрок был ркон админом
        msg_item.get_params()("rcon_time_connection", (int)(std::time(0) - rcon_login_time));
        if (samp::player_disconnect_reason_time_out == reason) {
            msg_item.get_sender()("$(rcon_admin_disconect_time_out_log)", msg_log);
        }
        else if (samp::player_disconnect_reason_quit == reason || samp::player_disconnect_reason_server_shutdown == reason) {
            msg_item.get_sender()("$(rcon_admin_disconect_quit_log)", msg_log);
        }
        else if (samp::player_disconnect_reason_kick_ban == reason) {
            msg_item.get_sender()("$(rcon_admin_disconect_kickban_log)", msg_log);
        }
    }
}

void player_connection_item::hide_disconnect_msg() {
    is_show_disconnect = false;
}

bool player_connection_item::get_is_first_spawn() {
    return is_first_spawn;
}

void player_connection_item::on_timer250() {
    if (!rcon_is_admin) {
        if (get_root()->is_rcon_admin()) {
            // rcon админ только что вошел в админку
            rcon_is_admin = true;
            rcon_login_time = std::time(0);
            on_rcon_login();
        }
    }
}

void player_connection_item::on_rcon_login() {
    messages_item_paged msg_item(get_root());
    msg_item.get_params()
        ("rcon_admin_name_full", get_root()->name_get_full())
        ("rcon_admin_geo_long_desc", get_root()->geo_get_long_desc());

    msg_item.get_sender()
        ("$(rcon_admin_login_admins)", msg_players_all_admins - msg_player(get_root()))
        ("$(rcon_admin_login_log)", msg_log);

    players_t players_list = application::instance()->get_item<players>()->get_players();
    erase_if(players_list, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&player::is_rcon_admin, std::tr1::placeholders::_1)));
    players_list.erase(get_root());
    if (players_list.empty()) {
        msg_item.get_pager()("$(rcon_admin_login_self_empty)");
    }
    else {
        msg_item.get_params()
            ("admins_count", players_list.size())
            ("admins_names", players::get_players_names(players_list));

        msg_item.get_pager()("$(rcon_admin_login_self_item)");
    }
}

void player_connection_item::show_connect_ingame_msg() {
    player_ptr_t player_ptr = get_root();
    messages_item msg_item;

    msg_item.get_params()
        ("player_name_full", player_ptr->name_get_full())
        ("player_name", player_ptr->name_get())
        ("player_county", player_connection::geo_get_country(player_ptr))
        ("player_serial", player_ptr->get_serial())
        ;

    msg_item.get_sender()("$(player_connection_ingame_log)", msg_log);


    if (manager.is_connectmsg_in_chat) {
        msg_item.get_sender()("$(player_connection_ingame_players)", msg_players_all - msg_player(player_ptr));
    }
    else {
        geo::ip_info const& geo_ip_info = player_ptr->geo_get_ip_info();
        if (geo_ip_info.country_code3) {
            // Есть страна
            msg_item.get_params()("suffix", geo_ip_info.country_code3.get());
        }
        else {
            // Страны нет - убираем суффикс
            msg_item.get_params()("player_connection_death_reason_suffix", "");
        }
        player_ptr->name_set(msg_item.get_params().process_all_vars("$(player_connection_ingame_death_reason)"));
        player::send_death_message(player_ptr, manager.connect_death_reason, false);
        player_ptr->name_reset();
    }
}

void player_connection_item::show_disconnect_msg(disconnect_msg_type_e disconnect_msg_type, std::string const& reason) {
    player_ptr_t player_ptr = get_root();
    messages_item msg_item;

    msg_item.get_params()
        ("player_name_full", player_ptr->name_get_full())
        ("player_name", player_ptr->name_get())
        ("player_serial", player_ptr->get_serial())
        ("reason", reason)
        ;

    if (manager.is_connectmsg_in_chat || disconnect_msg_kick == disconnect_msg_type || disconnect_msg_ban == disconnect_msg_type) {
        if (disconnect_msg_time_out == disconnect_msg_type) {
            msg_item.get_sender()("$(player_connection_disconect_time_out_players)", msg_players_all_users);
        }
        else if (disconnect_msg_quit == disconnect_msg_type) {
            msg_item.get_sender()("$(player_connection_disconect_quit_players)", msg_players_all_users);
        }
        else if (disconnect_msg_kick == disconnect_msg_type) {
            msg_item.get_sender()("$(player_connection_disconect_kick_players)", msg_players_all_users);
        }
        else if (disconnect_msg_ban == disconnect_msg_type) {
            msg_item.get_sender()("$(player_connection_disconect_ban_players)", msg_players_all_users);
        }
        else if (disconnect_msg_rcon == disconnect_msg_type) {
            msg_item.get_sender()("$(player_connection_disconect_kickban_players)", msg_players_all_users);
        }
    }

    if (!manager.is_connectmsg_in_chat) {
        // Пишем уход в килл лист
        if (disconnect_msg_time_out == disconnect_msg_type) {
            msg_item.get_params()("suffix", "$(player_connection_disconect_time_out_suffix)");
        }
        else if (disconnect_msg_quit == disconnect_msg_type) {
            msg_item.get_params()("suffix", "$(player_connection_disconect_quit_suffix)");
        }
        else if (disconnect_msg_kick == disconnect_msg_type) {
            msg_item.get_params()("suffix", "$(player_connection_disconect_kick_suffix)");
        }
        else if (disconnect_msg_ban == disconnect_msg_type) {
            msg_item.get_params()("suffix", "$(player_connection_disconect_ban_suffix)");
        }
        else if (disconnect_msg_rcon == disconnect_msg_type) {
            msg_item.get_params()("suffix", "$(player_connection_disconect_kickban_suffix)");
        }
        else {
            msg_item.get_params()("player_connection_death_reason_suffix", "");
        }
        player_ptr->name_set(msg_item.get_params().process_all_vars("$(player_connection_disconect_death_reason)"));
        player::send_death_message(player_ptr, manager.disconnect_death_reason, false);
        player_ptr->name_reset();
    }
}
