#include "config.hpp"
#include "server_guard.hpp"
#include "core/application.hpp"
#include "core/players.hpp"
#include "core/player/player.hpp"
#include "core/geo/geo_data_base.hpp"
#include "core/samp/samp_api.hpp"
#include "core/samp/pawn/pawn_log.hpp"
#include "core/samp/pawn/pawn_plugin.hpp"
#include "core/ver.hpp"
#include "hashlib2plus/hl_sha1wrapper.h"
#include <algorithm>
#include <iterator>
#include <functional>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include "core/logger/logger.hpp"
#include "core/samp/samp_hook_events_network.hpp"

REGISTER_IN_APPLICATION(server_guard);

// Отключает защиту от загрузки фильтрскриптов
//#define DEVELOP
static const int max_develop_slots = 4;

server_guard::ptr server_guard::instance() {
    return application::instance()->get_item<server_guard>();
}

server_guard::server_guard()
:is_configured(false)
,is_trace_connects(false)
,is_dump_network_statistics(false)
,is_gamemode_loaded(false)
,baned_ips(60 * 60 * 1000)
,valid_security_hash("fe8257c3bb88a455c5160cd98c22d7167c8e04a4")
,system_call_onban("system_call_onban")
,system_call_onunban("system_call_onunban")
{
}

server_guard::~server_guard() {
}

void server_guard::create() {
}

void server_guard::configure_pre() {
    is_configured = false;
    ban_time_out = 60 * 60 * 1000; // 60 минут
    rcon_external_not_parse_ban.get_params().set(60000, 5, 5, 0, 0);
    rcon_external_ban.get_params().set(60000, 5, 5, 0, 0);
    network_many_connections_ban.get_params().set(5000, 10, 10, 0, 0);
    is_trace_connects = false;
    is_dump_network_statistics = false;
    is_ban_on_network_many_connections = true;
    autoban_rcon_cmds.clear();
    ips_white_list.clear();

    server_full_is_active = false;
    server_full_time = 5000;
    server_full_cleanip_time = 30000;
}

void server_guard::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(ban_time_out);
    SERIALIZE_NAMED_ITEM(rcon_external_not_parse_ban.get_params(), "rcon_external_not_parse_ban_params");
    SERIALIZE_NAMED_ITEM(rcon_external_ban.get_params(), "rcon_external_ban_params");
    SERIALIZE_NAMED_ITEM(network_many_connections_ban.get_params(), "network_ban_params");
    SERIALIZE_ITEM(is_trace_connects);
    SERIALIZE_ITEM(is_dump_network_statistics);
    SERIALIZE_ITEM(is_ban_on_network_many_connections);
    SERIALIZE_ITEM(autoban_rcon_cmds);

    { // Загружаем белый список ip адресов
        std::vector<std::string> ips_white_list_tmp;
        SERIALIZE_NAMED_ITEM(ips_white_list_tmp, "ips_white_list");
        BOOST_FOREACH(std::string const& ip_string, ips_white_list_tmp) {
            int ip = geo::get_ip(ip_string);
            if (ip) {
                ips_white_list.insert(ip);
            }
        }
    }
    SERIALIZE_ITEM(system_call_onban);
    SERIALIZE_ITEM(system_call_onunban);

    SERIALIZE_ITEM(server_full_is_active);
    SERIALIZE_ITEM(server_full_time);
    SERIALIZE_ITEM(server_full_cleanip_time);
}

void server_guard::configure_post() {
    baned_ips.set_time_out(ban_time_out);
    is_configured = true;
}

void server_guard::on_bad_rcon_external(std::string const& ip_string) {
    if (!is_configured) return;
    std::string log_ip = geo::data_base::instance()->get_info(ip_string).get_long_desc();
    unsigned int ip = geo::get_ip(ip_string);

    logger::ptr logger_ptr = logger::instance();
    logger_ptr->log("guard/badrcon/external", log_ip);

    { // Баним в случае необходимости
        boost::mutex::scoped_lock lock(hooked_events_mutex);
        rcon_external_ban.increase(ip);
        if (rcon_external_ban.is_blocked(ip)) {
            logger_ptr->log("guard/ban/bad_ext_rcon_pass", log_ip);
            ban_ip(ip);
        }
    }
}

void server_guard::on_bad_rcon_local(int player_id, std::string const& password) {
    if (!is_configured) return;
    if (player::ptr player_ptr = players::instance()->get_player(player_id)) {
        logger::instance()->log("guard/badrcon/local", (boost::format("%1% '%2%'") % boost::replace_all_copy(player_ptr->name_get_full(), "~", " ") % password).str());
    }
}

void server_guard::on_network_connect(unsigned int ip, unsigned short port) {
    if (!is_configured) return;
    boost::mutex::scoped_lock lock(hooked_events_mutex);
    if (!baned_ips.is_exist(ip)) {
        // ип не забанен
        logger::ptr logger_ptr = logger::instance();

        if (is_trace_connects) {
            logger_ptr->debug("guard/network/connect", (boost::format("%1%:%2%") % geo::get_ip_string(ip) % port).str());
        }

        if (is_ban_on_network_many_connections) {
            if (ips_white_list.end() == ips_white_list.find(ip)) {
                // Нашего ип нет в белом списке
                network_many_connections_ban.increase(ip);
                if (network_many_connections_ban.is_blocked(ip)) {
                    logger_ptr->log("guard/ban/network", geo::data_base::instance()->get_info(geo::get_ip_string(ip)).get_long_desc());
                    ban_ip(ip);
                }
            }
        }
    }
}

void server_guard::on_network_disconnect(unsigned int ip, unsigned short port, std::time_t connection_time, unsigned int data_send, unsigned int data_recive) {
    if (!is_configured) return;
    if (is_trace_connects) {
        logger::instance()->debug("guard/network/disconnect", (boost::format("%1%:%2% %3% %4% %5%") % geo::get_ip_string(ip) % port % connection_time % data_send % data_recive).str());
    }
}

void server_guard::on_network_statistics(unsigned int ip, std::time_t connection_time, unsigned int data_send, unsigned int data_recive, unsigned int count_send, unsigned int count_recive) {
    if (is_dump_network_statistics) {
        logger::instance()->debug("guard/network/statistics", (boost::format("%1% %2% %3% %4% %5% %6%") % geo::get_ip_string(ip) % connection_time % data_send % data_recive % count_send % count_recive).str());
    }
}

void server_guard::on_network_rcon_external(unsigned int ip, std::string const& password, std::string const& cmd) {
    if (!is_configured) return;
    if (autoban_rcon_cmds.end() != autoban_rcon_cmds.find(cmd)) {
        // Нашли запрещенную команду - баним
        boost::mutex::scoped_lock lock(hooked_events_mutex);
        logger::instance()->log("guard/ban/bad_rconcmd", (boost::format("'%1%' '%2%' (%3%)") % password % cmd % geo::data_base::instance()->get_info(geo::get_ip_string(ip)).get_long_desc()).str());
        ban_ip(ip);
    }
    logger::instance()->log("guard/rconcmd", (boost::format("'%1%' '%2%' (%3%)") % password % cmd % geo::data_base::instance()->get_info(geo::get_ip_string(ip)).get_long_desc()).str());
}

void server_guard::on_network_rcon_external_not_parse(unsigned int ip, std::string const& raw_base64) {
    if (!is_configured) return;
    std::string log_ip = geo::data_base::instance()->get_info(geo::get_ip_string(ip)).get_long_desc();

    logger::ptr logger_ptr = logger::instance();
    logger_ptr->log("guard/rconcmd/notparse", (boost::format("'%1%' (%2%)") % raw_base64 % log_ip).str());

    { // Баним в случае необходимости
        boost::mutex::scoped_lock lock(hooked_events_mutex);
        rcon_external_not_parse_ban.increase(ip);
        if (rcon_external_not_parse_ban.is_blocked(ip)) {
            logger_ptr->log("guard/ban/notparse_rcon", log_ip);
            ban_ip(ip);
        }
    }

    //
    //Прогон лога на локальном сервере
    //while read line; do echo "$line" | base64 -d | nc -u -w 1 127.0.0.1 7777; done < <(cat /cygdrive/c/_svn/dimonml_samp/trunk_2/rezult/aggregation/logs_gta-paradise-sa/log/2009/09/*.txt | grep -F '<guard/rconcmd/notparse>' | perl -ne 'if (/'"'"'(.+)'"'"'/) {print "$1\n"}')
}

// Баним указанный ип адрес
void server_guard::ban_ip(unsigned int ip) {
    if (baned_ips.is_exist(ip)) {
        std::string log_ip = geo::data_base::instance()->get_info(geo::get_ip_string(ip)).get_long_desc();
        logger::instance()->log("guard/ban/error", log_ip);
        return;
    }

    baned_ips.insert(std::tr1::bind(&server_guard::unban_ip, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3), ip);
    samp::network_activity_block(ip);
    
    if (system_call_onban) {
        // Вызываем внешний обработчик бана ип адреса (чтобы забанить еще и на уровне брандмаура)
        messages_item msg_item;
        msg_item.get_params()("ip", geo::get_ip_string(ip));
        system_call_onban.call(msg_item.get_params());
    }
}

// Разбаниваем указанный ип адрес
void server_guard::unban_ip(unsigned int const& ip, time_base::millisecond_t const& last_update, time_outs_dummy_val const& connect_time) {
    samp::network_activity_unblock(ip);
    std::string log_ip = geo::data_base::instance()->get_info(geo::get_ip_string(ip)).get_long_desc();
    logger::instance()->log("guard/unban", log_ip);
    
    if (system_call_onunban) {
        // Вызываем внешний обработчик бана ип адреса (чтобы забанить еще и на уровне брандмаура)
        messages_item msg_item;
        msg_item.get_params()("ip", geo::get_ip_string(ip));
        system_call_onunban.call(msg_item.get_params());
    }
}

void server_guard::on_timer15000() {
    if (!is_configured) return;

    boost::mutex::scoped_lock lock(hooked_events_mutex);
    // Удаляем из списка банов те, у которых прошел тайм-аут
    baned_ips.erase_timeouts(std::tr1::bind(&server_guard::unban_ip, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3));
    rcon_external_not_parse_ban.erase_timeouts();
    rcon_external_ban.erase_timeouts();
    network_many_connections_ban.erase_timeouts();
}

void server_guard::on_timer5000() {
    if (!is_configured) return;
    on_network_connection_request_cleanup();
}

void server_guard::on_gamemode_init(AMX* amx, samp::server_ver ver) {
    security_tokens_t security_tokens = get_security_tokens();
    bool is_have_gm = false;
    bool is_have_fs = false;
    bool is_fail = false;
    for (security_tokens_t::const_iterator it = security_tokens.begin(), end = security_tokens.end(); end != it; ++it) {
        if (valid_security_hash == get_hash_code((*it) ^ (build_id_ver | 0x0C000000))) {
            // Нашли валидный фильтр скрипт
            if (is_have_fs) {
                is_fail = true;
            }
            else {
                is_have_fs = true;
            }
        }
        else if (valid_security_hash == get_hash_code((*it) ^ (build_id_ver | 0x0D000000))) {
            // Нашли валидный мод
            if (is_have_gm) {
                is_fail = true;
            }
            else {
                is_have_gm = true;
            }
        }
        else {
            is_fail = true;
        }
    }
    is_gamemode_loaded = true;
    if (!is_have_gm || !is_have_fs || is_fail) {
        process_error_shutdown("$(server_guard_shutdown_token)");
    }

#   ifdef DEVELOP
    if (samp::api::instance()->get_max_players() > max_develop_slots) {
        pawn::logprintf("GTA - Paradise develop version can use maximum %i slot(s)", max_develop_slots);
        exit(1);
    }
#   endif // ifdef DEVELOP
}

void server_guard::plugin_on_amx_load(AMX* amx) {
    if (is_gamemode_loaded) {
        process_error_shutdown("$(server_guard_shutdown_onload_fs)");
    }
}

std::string server_guard::get_hash_code(int value) {
    sha1wrapper sha1_hash;
    return boost::to_lower_copy(sha1_hash.getHashFromString("new_val_" + sha1_hash.getHashFromString((boost::format("%1%") % value).str())));
}

void server_guard::process_error_shutdown(std::string const& msg) {
    messages_item msg_item;
    msg_item.get_sender()(msg, msg_log);

    // Во время релиза только проверяем
#   ifndef DEVELOP
#     ifdef NDEBUG
        samp::api::instance()->send_rcon_command("exit");
#     endif // ifdef NDEBUG
#   endif // ifndef DEVELOP
}

void server_guard::on_network_connection_request(unsigned int ip, on_network_connection_request_action* action) {
    if (!server_full_is_active) {
        *action = on_network_connection_request_action_common_process;
        return;
    }

    time_base::millisecond_t curr_time = time_base::tick_count_milliseconds();
    *action = on_network_connection_request_action_send_full;

    boost::mutex::scoped_lock lock(server_full_ips_mutex);
    server_full_ips_t::iterator ips_it = server_full_ips.find(ip);
    if (server_full_ips.end() == ips_it) {
        // Ип адрес только подключается
        server_full_ips.insert(std::make_pair(ip, curr_time + server_full_time));
    }
    else {
        // Ип адрес уже у нас есть, проверяем время
        if (curr_time > ips_it->second) {
            // Время вышло, разрешаем нормально подключиться
            *action = on_network_connection_request_action_common_process;
        }
    }
}

void server_guard::on_network_connection_request_cleanup() {
    time_base::millisecond_t curr_time = time_base::tick_count_milliseconds();
    boost::mutex::scoped_lock lock(server_full_ips_mutex);
    for (server_full_ips_t::iterator ips_it = server_full_ips.begin(), ips_end = server_full_ips.end(); ips_it != ips_end;) {
        server_full_ips_t::iterator ips_work_it = ips_it++;
        if (ips_work_it->second + server_full_cleanip_time < curr_time) {
            server_full_ips.erase(ips_work_it);
        }
    }
}
