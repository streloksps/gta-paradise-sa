#ifndef PLAYER_SPECTATE_HPP
#define PLAYER_SPECTATE_HPP
#include "core/module_h.hpp"
#include "core/player/player_t.hpp"
#include "basic_types.hpp"
#include <ctime>

class player_spectate_item;
class player_spectate
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public players_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<player_spectate> ptr;
    static ptr instance();

public:
    player_spectate();
    virtual ~player_spectate();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_spectate);

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);
    virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason);

public:
    void refresh_spectate_by_dest(player_ptr_t const& changed_player_ptr);

private:
    friend class player_spectate_item;

    command_arguments_rezult cmd_spectate(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    // ВОзращает истину, если выполнена успешно
    bool cmd_spectate_impl(player_ptr_t const& player_ptr, player_ptr_t const& spec_player_ptr, messages_pager& pager, messages_sender const& sender, messages_params& params);
    // Возращает первого игрока для наблюдения или 0, если не удалось найти таких
    player_ptr_t get_player_to_spectate(player_ptr_t const& player_ptr);

public:
    void on_pre_explode(float x, float y, float z);

private:
    text_draw::td_item::ptr keys_info;
    int keys_info_timeout;
    
    // Параметры проверок позиций. Если следящий находиться в заданной позиции, то рефрешим его
    bool    is_check_pos;
    pos3    check_pos;
    float   check_pos_threshold;
};

class player_spectate_item
    :public player_item
    ,public game_context
    ,public on_timer5000_i
    ,public player_events::on_disconnect_i
    ,public player_events::on_spawn_i
    ,public player_events::on_state_change_i
    ,public player_events::on_interior_change_i
    ,public player_events::on_keystate_change_i
    ,public player_events::on_click_player_i
    ,public player_events::on_player_stream_out_i
{
public:
    typedef std::tr1::shared_ptr<player_spectate_item> ptr;

public:
    void do_spectate(player_ptr_t const& spec_player_ptr);
    void do_refresh();
    void do_shift(bool is_next = true);
    bool is_spectate() const;
    bool is_spectate_ending() const;
    bool is_spectate_to(player_ptr_t const& spec_player_ptr) const;
    player_ptr_t get_spectate_to(); // Возращает игрока, за кем следит данный игрок

private:
    void do_stop_spec(bool is_error = false);
    void state_save();
    void state_preload();
    void state_load();
    void do_check_pos();

    void on_spectate_start(player_ptr_t const& spectated_player_ptr);
    void on_spectate_change(player_ptr_t const& new_spectated_player_ptr);
    void on_spectate_stop(bool is_error);
    void on_spectate_spawn_after();

protected:
    friend class player_spectate;
    player_spectate_item(player_spectate& manager);
    virtual ~player_spectate_item();

protected: // game_context
    virtual void on_gc_init();
    virtual void on_gc_fini();

public: // player_events::*
    virtual void on_disconnect(samp::player_disconnect_reason reason);
    virtual void on_spawn();
    virtual void on_state_change(samp::player_state state_new, samp::player_state state_old);
    virtual void on_interior_change(int interior_id_new, int interior_id_old);
    virtual void on_keystate_change(int keys_new, int keys_old);
    virtual void on_click_player(player_ptr_t const& clicked_player, samp::click_source source);
    virtual void on_timer5000();
    virtual void on_player_stream_out(player_ptr_t const& player_ptr);

private:
    player_spectate&        manager;
    player_ptr_t            curr_spec_player_ptr;
    bool                    is_ending;

private: // Состояние перед спеком
    game_context::ptr		state_context;
	player_health_armour	state_healht;
    pos4                    state_pos;
    int                     state_money;
    weapons_t               state_weapons;
    std::time_t             state_start;

    static void shift_it(players_t::iterator& it, players_t& players, bool is_next);

    // Возращает истину, если данный игрок может следить за кем то
    bool is_can_spectate() const;
};

#endif // PLAYER_SPECTATE_HPP
