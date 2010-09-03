#ifndef SERVER_HPP
#define SERVER_HPP
#include "core/module_h.hpp"
#include <ctime>
#include <deque>
#include <boost/thread.hpp>
#include <ctime>

class server
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public on_timer250_i
    ,public on_timer1000_i
    ,public on_timer15000_i
    ,public samp::on_gamemode_init_i
    ,public samp::on_gamemode_exit_i
    ,public samp::on_rconcommand_i
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<server> ptr;
    static ptr instance();

public:
    server();
    virtual ~server();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(server);

public: // timer
    virtual void on_timer250();
    virtual void on_timer1000();
    virtual void on_timer15000();

public: // samp::*
    virtual void on_gamemode_init(AMX*, samp::server_ver ver);
    virtual void on_gamemode_exit(AMX*);
    virtual bool on_rconcommand(std::string const& cmd);

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);

private:
    static command_arguments_rezult cmd_reconfigure(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    static command_arguments_rezult rcon_reconfigure(std::string const& arguments, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_reboot(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

private:
    int reboot_delay;

    bool is_active_reboot;
    std::time_t reboot_time;

private:
    std::time_t start_time;

private:
    text_draw::td_item::ptr site_url;
    text_draw::td_item::ptr time_stamp;
    void update_time_stamp_params();

private:
    std::deque<std::string> rcon_cmds;
    boost::mutex            rcon_cmds_mutex;

public:
    std::string stat_get_string();
    void stat_update();

    float stat_user_utility_get() {return stat_user_utility;}
    float stat_kernel_utility_get() {return stat_kernel_utility;}
    float stat_load_average1_get() {return stat_load_average1;}
    float stat_load_average5_get() {return stat_load_average5;}
    float stat_load_average15_get() {return stat_load_average15;}
private:
    bool stat_is_trace_cpu;
    std::clock_t stat_cpu_clock_last;
    std::clock_t stat_cpu_user_last;
    std::clock_t stat_cpu_kernel_last;
    
    float stat_user_utility;
    float stat_kernel_utility;
    float stat_load_average1;
    float stat_load_average5;
    float stat_load_average15;

private: // Параметры, которые могут менять ркон переменные (если они не равны -1)
    int   rcon_lanmode;
    int   rcon_sleep;
    float rcon_stream_distance;
    int   rcon_stream_rate;
};

#endif // SERVER_HPP
