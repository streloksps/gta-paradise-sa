#ifndef PLAYER_CLASSES_HPP
#define PLAYER_CLASSES_HPP
#include <vector>
#include "core/module_h.hpp"
#include "core/samp/samp_events_i.hpp"
#include "player_classes_t.hpp"
#include "basic_types.hpp"

class player_classes
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public samp::on_gamemode_init_i
{
public:
    player_classes();
    virtual ~player_classes();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_classes);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

public: // samp::on_gamemode_init_i
    virtual void on_gamemode_init(AMX*, samp::server_ver ver);

private:
    friend class player_classes_item;
    typedef std::vector<player_class_def_t> player_class_defs_t;

    pos4                class_spawn_def_pos;
    pos4                class_select_pos;
    pos_camera          class_select_camera;
    player_time         class_select_time;
    int                 class_select_weather_id;
    player_class_defs_t class_defs;
};

class player_classes_item
    :public player_item
    ,public player_events::on_connect_i
    ,public player_events::on_request_class_i
{
public:
    typedef std::tr1::shared_ptr<player_classes_item> ptr;

    player_classes_item(player_classes& manager);
    virtual ~player_classes_item();

    //int get_class_id() const; - может меняться во времени
    player_class_def_t const& get_class_def() const;
private:
    friend class player_classes;
    player_classes& manager;
    player_class_def_t player_class_def;

public: // player_events::*
    virtual void on_connect();
    virtual void on_request_class(int class_id);

private:
    void setup_for_select_class();
};

#endif // PLAYER_CLASSES_HPP
