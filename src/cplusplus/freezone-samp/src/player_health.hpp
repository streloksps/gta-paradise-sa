#ifndef PLAYER_HEALTH_HPP
#define PLAYER_HEALTH_HPP
#include <memory>
#include "core/module_h.hpp"
#include "core/time_count_blocker.hpp"

class player_health_item;
class player_health
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public players_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<player_health> ptr;
    static ptr instance();

public:
    player_health();
    virtual ~player_health();

public:
    enum check_type_e {
        check_explode
        ,check_silent
        ,check_random
    };

public:
    bool cmd_check_impl(player_ptr_t const& player_ptr, player_ptr_t const& explode_player_ptr, messages_pager& pager, messages_sender const& sender, messages_params& params, check_type_e check_type /*bool is_silent*/);

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_health);

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);
    virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason);

private:
    static command_arguments_rezult cmd_suicide(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_check(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    friend class player_health_item;

    time_count_blocker<player_ptr_t> counter;

    float check_silent_vehicle;
    float check_silent_player_h;
    float check_silent_player_a;
    float check_explode_player_h;
    float check_random_vehicle_min;
    float check_random_vehicle_max;
    float check_random_player_min;
    float check_random_player_max;
    int   script_death_reason;
    int   script_death_player_count; // 1 - в килл листе при самоубийстве будет 1 раз ник игрока; 2 - будет 2 раза ник
};

class player_health_item
    :public player_item
    ,public player_events::on_death_i
    ,public player_events::on_spawn_i
{
public:
    typedef std::tr1::shared_ptr<player_health_item> ptr;

public: // Публичные методы
    void kill(bool is_hide_death = false);

public: // player_events::*
    virtual void on_spawn();
    virtual void on_death(player_ptr_t const& killer_ptr, int reason);

protected:
    friend class player_health;
    player_health_item(player_health& manager);
    virtual ~player_health_item();

private:
    player_health& manager;
    bool is_killed_by_script;
    bool is_hide_death;
    bool is_spawned;

    void process_suicide(int reason, bool is_by_script);
    void process_kill(player_ptr_t const& killer_ptr, int reason);
};

#endif // PLAYER_HEALTH_HPP
