#include "config.hpp"
#include "server.hpp"
#include <functional>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include "core/application.hpp"
#include "core/command/command_adder.hpp"
#include "core/samp/samp_api.hpp"
#include "core/messages/server_properties.hpp"
#include "core/logger/logger.hpp"
#include "core/samp/samp_hook_events_network.hpp"
#include "core/players.hpp"
#include "server_configuration.hpp"

REGISTER_IN_APPLICATION(server);

server::ptr server::instance() {
    return application::instance()->get_item<server>();
}

server::server()
:reboot_delay(5)
,is_active_reboot(false)
,reboot_time(0) 
,stat_cpu_clock_last(0)
,stat_cpu_user_last(0)
,stat_cpu_kernel_last(0)
,stat_user_utility(0.0f)
,stat_kernel_utility(0.0f)
,stat_load_average1(0.0f)
,stat_load_average5(0.0f)
,stat_load_average15(0.0f)
{
}

server::~server() {
}

void server::create() {
    site_url.reset(new text_draw::td_item("$(site_url_textdraw)"));
    time_stamp.reset(new text_draw::td_item("$(server_time_textdraw)"));
    command_add("reconfigure", &server::cmd_reconfigure);
    command_add("reboot", std::tr1::bind(&server::cmd_reboot, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
}

void server::configure_pre() {
    stat_is_trace_cpu = false;

    rcon_lanmode = -1;
    rcon_sleep = -1;
    rcon_stream_distance = -1.0f;
    rcon_stream_rate = -1;
}

void server::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(stat_is_trace_cpu);

    SERIALIZE_ITEM(rcon_lanmode);
    SERIALIZE_ITEM(rcon_sleep);
    SERIALIZE_ITEM(rcon_stream_distance);
    SERIALIZE_ITEM(rcon_stream_rate);
}

void server::configure_post() {
    update_time_stamp_params();
    site_url->update();

    { // Выставляем ркон переменные, если указано
        samp::api::ptr api_ptr = samp::api::instance();
        if (-1 != rcon_lanmode) {
            api_ptr->send_rcon_command((boost::format("lanmode %1%") % rcon_lanmode).str());
        }
        if (-1 != rcon_sleep) {
            api_ptr->send_rcon_command((boost::format("sleep %1%") % rcon_sleep).str());
        }
        if (-1.0f != rcon_stream_distance) {
            api_ptr->send_rcon_command((boost::format("stream_distance %1%") % rcon_stream_distance).str());
        }
        if (-1 != rcon_stream_rate) {
            api_ptr->send_rcon_command((boost::format("stream_rate %1%") % rcon_stream_rate).str());
        }
    }
}

void server::on_timer250() {
    boost::mutex::scoped_lock lock(rcon_cmds_mutex);
    if (!rcon_cmds.empty()) {
        if ("reconfig" == rcon_cmds.front()) {
            messages_item msg_item;
            rcon_reconfigure("", msg_item.get_sender(), msg_item.get_params());
        }
        rcon_cmds.pop_front();
    }
}

void server::on_timer1000() {
    if (is_active_reboot) {
        if (std::time(0) > reboot_time) {
            is_active_reboot = false;
            messages_item msg_item;
            msg_item.get_sender()("$(cmd_reboot_execute_log)", msg_log);
            samp::api::instance()->send_rcon_command("exit");
        }
    }
}

void server::on_timer15000() {
    update_time_stamp_params();
    stat_update();
}

command_arguments_rezult server::cmd_reconfigure(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }
    sender
        ("$(cmd_reconfigure_begin_admins)", msg_players_all_admins)
        ("$(cmd_reconfigure_begin_log)", msg_log);
    server_configuration::instance()->reconfigure();
    sender
        ("$(cmd_reconfigure_end_log)", msg_log)
        ("$(cmd_reconfigure_end_admins)", msg_players_all_admins);
    return cmd_arg_ok;
}

command_arguments_rezult server::cmd_reboot(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }
    if (is_active_reboot) {
        pager("$(cmd_reboot_already_rebooting)");
        return cmd_arg_process_error;
    }

    is_active_reboot = true;
    reboot_time = std::time(0) + reboot_delay;

    sender
        ("$(cmd_reboot_begin_players)", msg_players_all_users)
        ("$(cmd_reboot_begin_admins)", msg_players_all_admins)
        ("$(cmd_reboot_begin_log)", msg_log);

    return cmd_arg_ok;
}

void server::on_gamemode_init(AMX*, samp::server_ver ver) {
    messages_item msg_item;
    msg_item.get_sender()("$(server_start_log)", msg_log);
    start_time = std::time(0);

    //samp::api::instance()->connect_npc("test_npc", "freemode_v030a");
}

void server::on_gamemode_exit(AMX*) {
    messages_item msg_item;
    msg_item.get_params()("work_time", static_cast<int>(std::time(0) - start_time));
    msg_item.get_sender()("$(server_stop_log)", msg_log);
}

void server::on_connect(player_ptr_t const& player_ptr) {
    site_url->show_for_player(player_ptr);
    time_stamp->show_for_player(player_ptr);
}

bool server::on_rconcommand(std::string const& cmd) {
    if (boost::iequals(cmd, "reconfig")) {
        boost::mutex::scoped_lock lock(rcon_cmds_mutex);
        rcon_cmds.push_back("reconfig");
        return true;
    }
    return false;
}

command_arguments_rezult server::rcon_reconfigure(std::string const& arguments, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }
    params("cmd_player_name", "rcon");
    sender
        ("$(cmd_reconfigure_begin_admins)", msg_players_all_admins)
        ("$(cmd_reconfigure_begin_log)", msg_log)
        ("$(cmd_reconfigure_begin_slog)", msg_serverlog);
    server_configuration::instance()->reconfigure();
    sender
        ("$(cmd_reconfigure_end_log)", msg_log)
        ("$(cmd_reconfigure_end_admins)", msg_players_all_admins)
        ("$(cmd_reconfigure_end_slog)", msg_serverlog);
    return cmd_arg_ok;
}

void server::update_time_stamp_params() {
    std::ostringstream time_string_generator;
    { // Получаем форматированное время
        boost::posix_time::ptime time_local(boost::posix_time::second_clock::local_time());
        boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
        facet->format(time_stamp->get_params().process_all_vars("$(server_time_stamp_time_format)").c_str());
        time_string_generator.imbue(std::locale(time_string_generator.getloc(), facet));
        time_string_generator<<time_local;
    }

    time_stamp->get_params()
        ("server_name_suffix", boost::to_lower_copy(server_properties::instance()->get_name_suffix()))
        ("time", time_string_generator.str())
        ;
    time_stamp->update();
}

#ifdef LINUX
#include <sys/time.h>
#include <sys/times.h>
#endif // LINUX

#ifdef LINUX
std::string server::stat_get_string() {
    std::string rezult;
    std::clock_t HZ = sysconf(_SC_CLK_TCK);
    tms ti;
    std::clock_t cl = times(&ti);
    rezult = (boost::format("clk=%d, clock=%d, tms_utime=%d, tms_stime=%d, tms_cutime=%d, tms_cstime=%d") % HZ % cl % ti.tms_utime % ti.tms_stime % ti.tms_cutime % ti.tms_cstime).str();
    return rezult;
}
void server::stat_update() {
    tms ti;
    double la[3];

    std::clock_t ticks_per_seconds = sysconf(_SC_CLK_TCK);
    std::clock_t real_time = times(&ti);
    
    getloadavg(la, 3);

    if (stat_cpu_clock_last) {
        std::clock_t clock_delta = real_time - stat_cpu_clock_last;
        std::clock_t user_delta = ti.tms_utime - stat_cpu_user_last;
        std::clock_t kernel_delta = ti.tms_stime - stat_cpu_kernel_last;

        stat_user_utility = static_cast<float>(user_delta) / static_cast<float>(clock_delta);
        stat_kernel_utility = static_cast<float>(kernel_delta) / static_cast<float>(clock_delta);
        stat_load_average1 = la[0];
        stat_load_average5 = la[1];
        stat_load_average15 = la[2];

        if (stat_is_trace_cpu) {
            std::clock_t net_ticks_per_seconds;
            std::clock_t net_clock_delta;
            std::clock_t net_user_delta;
            std::clock_t net_kernel_delta;
            
            samp::profile_network_cpu.raw_get(net_ticks_per_seconds, net_clock_delta, net_user_delta, net_kernel_delta);

            std::size_t players_count = players::instance()->get_players_count();

            logger::instance()->debug("stat", (boost::format("%1% %2% %3% %4% %5% %6% %7% %8% %9% %10% %11%") 
                % players_count
                % ticks_per_seconds 
                % clock_delta % user_delta % kernel_delta 
                % net_clock_delta % net_user_delta % net_kernel_delta
                % la[0] % la[1] % la[2]
            ).str());
        }
    }
    
    stat_cpu_clock_last = real_time;
    stat_cpu_user_last = ti.tms_utime;
    stat_cpu_kernel_last = ti.tms_stime;
}
#else // LINUX
std::string server::stat_get_string() {
    return "<Недоступна>";
}
void server::stat_update() {
}
#endif // LINUX
