#ifndef PLAYER_FREE_FOR_ALL_HPP
#define PLAYER_FREE_FOR_ALL_HPP
#include <boost/date_time/posix_time/posix_time.hpp>
#include "core/module_h.hpp"
#include "basic_types.hpp"
#include "player_free_for_all_t.hpp"

class player_free_for_all_item;
class player_free_for_all
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<player_free_for_all> ptr;
public:
    player_free_for_all();
    virtual ~player_free_for_all();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_free_for_all);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

private:
    friend class player_free_for_all_item;
    typedef std::vector<pos4> poses_t;
    typedef std::vector<player_free_for_all_weapons_by_skin_t> weapons_by_skins_t;


    int                     spawn_money;
    weapons_t::items_t      spawn_weapons;
    weapons_by_skins_t      spawn_weapons_by_skins;
    poses_t                 spawn_poses;
    world_bounds_t          spawn_world_bounds;

private:
    boost::posix_time::ptime start_time;
    command_arguments_rezult cmd_version(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
};

class player_free_for_all_item
    :public player_item
    ,public game_context
    ,public on_timer15000_i
    ,public player_events::on_spawn_i
    ,public player_events::on_health_suicide_i
    ,public player_events::on_health_kill_i
{
public:
    typedef std::tr1::shared_ptr<player_free_for_all_item> ptr;

protected: // game_context
    virtual void on_gc_init();
    virtual void on_gc_fini();

public: // player_events::*
    virtual void on_spawn();
    virtual void on_health_suicide(int reason, bool is_by_script);
    virtual void on_health_kill(player_ptr_t const& killer_ptr, int reason);
    virtual void on_timer15000();

protected:
    friend class player_free_for_all;
    player_free_for_all_item(player_free_for_all& manager);
    virtual ~player_free_for_all_item();

private:
    player_free_for_all& manager;
};

#endif // PLAYER_FREE_FOR_ALL_HPP
