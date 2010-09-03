#ifndef PLAYER_CONNECTION_HPP
#define PLAYER_CONNECTION_HPP
#include "core/module_h.hpp"
#include "player_connection_i.hpp"
#include <ctime>

class player_connection_item;
class player_connection
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public players_events::on_disconnect_i
    ,public server_events::on_players_count_change_i
{
public:
    typedef std::tr1::shared_ptr<player_connection> ptr;
    static ptr instance();

public:
    static void cmd_geo_player_impl(player_ptr_t const& player_ptr, player_ptr_t const& geo_player_ptr, messages_pager& pager, messages_sender const& sender, messages_params& params);

public:
    player_connection();
    virtual ~player_connection();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_connection);

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);
    virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason);

public:
    virtual void on_players_count_change(int players_count, int players_max, server_events::players_count_change_reason reason);

public:
    static std::string geo_get_country(player_ptr_t const& player_ptr);

private:
    static command_arguments_rezult cmd_countries(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    static std::string cmd_countries_get_country(player_ptr_t const& player_ptr);
    static command_arguments_rezult cmd_geo(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    command_arguments_rezult cmd_kick(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_ban(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_admins_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

private:
    friend class player_connection_item;

private:
    std::string cmd_kick_def_reason;
    std::string cmd_ban_def_reason;
    
    bool is_connectmsg_in_chat; // Если истина, то сообщение о подключении/отключении печатается в общий чат, иначе в килл лист
    int  connect_death_reason;
    int  disconnect_death_reason;
private:
    int players_max;
    bool is_max_now;
    std::time_t players_max_start_time;
};

class player_connection_item
    :public player_item 
    ,public on_timer250_i
    ,public player_events::on_connect_i
    ,public player_events::on_spawn_i
    ,public player_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<player_connection_item> ptr;

protected:
    friend class player_connection;
    player_connection_item(player_connection& manager);
    virtual ~player_connection_item();

public: // player_events::*
    virtual void on_connect();
    virtual void on_spawn();
    virtual void on_disconnect(samp::player_disconnect_reason reason);
    virtual void on_timer250();

public:
    void hide_disconnect_msg();
    bool get_is_first_spawn();
private:
    player_connection& manager;
    bool is_first_spawn;
    std::time_t first_spawn_time;
    void on_first_spawn();

    bool is_show_disconnect;
    bool rcon_is_admin;
    std::time_t rcon_login_time;
    void on_rcon_login();

private:
    enum disconnect_msg_type_e {
        disconnect_msg_time_out
        ,disconnect_msg_quit
        ,disconnect_msg_kick
        ,disconnect_msg_ban
        ,disconnect_msg_rcon
    };
    void show_connect_ingame_msg();
    void show_disconnect_msg(disconnect_msg_type_e disconnect_msg_type, std::string const& reason = std::string());
};
#endif // PLAYER_CONNECTION_HPP
