#ifndef SERVER_GUARD_HPP
#define SERVER_GUARD_HPP
#include "core/module_h.hpp"
#include "core/samp/samp_hook_events_i.hpp"
#include "core/time_outs.hpp"
#include "core/time_count_blocker.hpp"
#include "core/samp/pawn/pawn_plugin_i.hpp"
#include "system_call.hpp"
#include <ctime>
#include <map>
#include <set>
#include <boost/thread/mutex.hpp>

class server_guard
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public samp::on_gamemode_init_i
    ,public samp::on_bad_rcon_external_i
    ,public samp::on_bad_rcon_local_i
    ,public samp::on_network_connect_i
    ,public samp::on_network_disconnect_i
    ,public samp::on_network_rcon_external_i
    ,public samp::on_network_rcon_external_not_parse_i
    ,public samp::on_network_statistics_i
    ,public samp::on_network_connection_request_i
    ,public pawn::plugin_on_amx_load_i
    ,public on_timer5000_i
    ,public on_timer15000_i
{
public:
    typedef std::tr1::shared_ptr<server_guard> ptr;
    static ptr instance();

public:
    server_guard();
    virtual ~server_guard();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();

public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(server_guard);

public: // samp::*
    virtual void on_gamemode_init(AMX* amx, samp::server_ver ver);
    
    virtual void on_bad_rcon_external(std::string const& ip_string);
    virtual void on_bad_rcon_local(int player_id, std::string const& password);
    virtual void on_network_connect(unsigned int ip, unsigned short port);
    virtual void on_network_disconnect(unsigned int ip, unsigned short port, std::time_t connection_time, unsigned int data_send, unsigned int data_recive);
    virtual void on_network_statistics(unsigned int ip, std::time_t connection_time, unsigned int data_send, unsigned int data_recive, unsigned int count_send, unsigned int count_recive);

    virtual void on_network_rcon_external(unsigned int ip, std::string const& password, std::string const& cmd);
    virtual void on_network_rcon_external_not_parse(unsigned int ip, std::string const& raw_base64);

    virtual void on_network_connection_request(unsigned int ip, on_network_connection_request_action* action);

public: // pawn::*
    virtual void plugin_on_amx_load(AMX* amx);

public: // timer15000_i
    virtual void on_timer5000();
    virtual void on_timer15000();

private:
    boost::mutex hooked_events_mutex;
    time_outs<unsigned int> baned_ips;
    time_count_blocker<unsigned int> rcon_external_ban;
    time_count_blocker<unsigned int> rcon_external_not_parse_ban;
    time_count_blocker<unsigned int> network_many_connections_ban;
    void ban_ip(unsigned int ip);

    int ban_time_out;

    bool is_trace_connects;
    bool is_dump_network_statistics;
    bool is_ban_on_network_many_connections;

    void unban_ip(unsigned int const& ip, time_base::millisecond_t const& last_update, time_outs_dummy_val const& connect_time);

    bool is_configured; // Для предотвращениея исполнения кода до момента конфигурации

    bool is_gamemode_loaded;
    void process_error_shutdown(std::string const& msg);
    static std::string get_hash_code(int value);
    std::string valid_security_hash;

    std::set<std::string> autoban_rcon_cmds;

private:
    typedef std::set<int> ips_white_list_t; // Список ип адресов, которые не нужно банить за частые подключения
    ips_white_list_t ips_white_list;

private: // Запуск внешнего приложения, при бане/разбане
    system_call_t system_call_onban;
    system_call_t system_call_onunban;

private: // Имитация полного сервера
    bool server_full_is_active;
    int server_full_time;
    int server_full_cleanip_time;

    typedef std::map<unsigned int /*ip*/, time_base::millisecond_t> server_full_ips_t;
    server_full_ips_t   server_full_ips;
    boost::mutex        server_full_ips_mutex;
    void on_network_connection_request_cleanup();
};

#endif // SERVER_GUARD_HPP
