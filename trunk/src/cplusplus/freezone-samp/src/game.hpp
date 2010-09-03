#ifndef GAME_HPP
#define GAME_HPP
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "core/module_h.hpp"
#include "basic_types.hpp"
#include "core/samp/samp_api.hpp"
#include <vector>

class game_item;
class game
    :public application_item
    ,public createble_i 
    ,public configuradable
    ,public on_timer1000_i
    ,public samp::on_gamemode_init_i
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<game> ptr;
    static ptr instance();

public:
    game();
    virtual ~game();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(game);

public: // timer1000_i
    virtual void on_timer1000();

public: // samp::on_gamemode_init_i
    virtual void on_gamemode_init(AMX*, samp::server_ver ver);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

public:
    player_time get_time();
    int         get_weather();
    float       get_gravity();

private:
    friend class game_item;
    boost::posix_time::ptime start_time;
    int time_shift; // Смещение времени, секунды
    samp::api::ptr api_ptr;
    player_time last_worldtime;

private:
    void synchronize_gravity();
    void synchronize_weather();
    void synchronize_time();
    void set_world_time(player_time const& worldtime);

private:
    bool disable_interior_enter_exits;
    bool disable_interior_weapons;
};

class game_item
    :public player_item
    ,public player_events::on_update_i
{
private:
    friend class game;
    typedef std::tr1::shared_ptr<game_item> ptr;

    game_item(game& manager);
    virtual ~game_item();

private:
    game& manager;

public: // player_events::*
    virtual bool on_update();
};
#endif // GAME_HPP
