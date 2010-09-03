#ifndef PLAYER_INFO_HPP
#define PLAYER_INFO_HPP
#include "core/module_h.hpp"
#include "player_spectate_i.hpp"
#include "server.hpp"

class player_info_item;
class player_info
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public players_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<player_info> ptr;
    static ptr instance();

public:
    player_info();
    virtual ~player_info();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_info);

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);
    virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason);

private:
    command_arguments_rezult cmd_player_info(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

private:
    friend class player_info_item;
    int geo_max_str_len;
    server::ptr server_ptr;
};

class player_info_item
    :public player_item
    ,public on_timer500_i
    ,public player_events::on_spectate_start_i
    ,public player_events::on_spectate_change_i
    ,public player_events::on_spectate_stop_i
    ,public player_events::on_disconnect_i
{
private:
    friend class player_info;
    typedef std::tr1::shared_ptr<player_info_item> ptr;
    player_info_item(player_info& manager);
    virtual ~player_info_item();

private:
    player_info& manager;

public: // player_events::*
    virtual void on_timer500();
    virtual void on_spectate_start(player_ptr_t const& spectated_player_ptr);
    virtual void on_spectate_change(player_ptr_t const& new_spectated_player_ptr);
    virtual void on_spectate_stop(std::time_t spectate_time);
    virtual void on_disconnect(samp::player_disconnect_reason reason);

private:
    void do_start(player_ptr_t const& player_ptr, bool is_manage_internal);
    void do_stop();
    bool is_active() const;
    void do_update();
private:
    text_draw::td_item::ptr td;
    bool is_automanage;
    player_ptr_t info_src;
};

#endif // PLAYER_INFO_HPP
