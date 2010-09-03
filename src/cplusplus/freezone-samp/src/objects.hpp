#ifndef OBJECTS_HPP
#define OBJECTS_HPP
#include <map>
#include "objects_t.hpp"
#include "core/module_h.hpp"
#include "core/dynamic_items.hpp"
#include "core/streamer.hpp"
#include "core/samp/samp_api.hpp"
#include "pos_change_checker.hpp"

class objects_item;
class objects
    :public application_item
    ,public configuradable
    ,public samp::on_gamemode_init_i
    ,public players_events::on_connect_i
{
public:
    objects();
    virtual ~objects();
    friend class objects_item;

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(objects);

public: // samp::*
    virtual void on_gamemode_init(AMX* amx, samp::server_ver ver);
    virtual void on_connect(player_ptr_t const& player_ptr);

private:
    typedef dynamic_items<object_static_t, int> objects_static_t;
    typedef dynamic_items<object_dynamic_t, int> objects_dynamic_t;
    
    objects_static_t objects_static;
    objects_dynamic_t objects_dynamic;
    int max_objects_dynamic;

    object_dynamic_t fill_defs(object_dynamic_t const& obj_src) const;

public: // Работа со статическими объектами
    int  static_object_create(object_static_t const& object_static);
    void static_object_destroy(int id);

public: // Работа с динамическими объектами
    int  dynamic_object_create(object_dynamic_t const& object_dynamic);
    void dynamic_object_destroy(int id);

private:
    int max_objects;
private:
    typedef std::map<int, object_def_t> object_defs_t;
    
    object_def_t    object_def_default;
    object_defs_t   object_defs;

private:
    typedef streamer::streamer_t<object_dynamic_t> objects_streamer_t;
    objects_streamer_t streamer;

private:
    void streamer_item_deleter(int key);

private:
    bool is_debug;
    bool is_debug_config;
};

class objects_item
    :public player_item
    ,public on_timer500_i
{
public:
    typedef std::tr1::shared_ptr<objects_item> ptr;
private:
    friend class objects;

    objects_item(objects& manager);
    virtual ~objects_item();

public: // player_events::
    virtual void on_timer500();

private:
    int object_create(int object_key);
    void object_destroy(int id);

private:
    objects& manager;
    samp::api::ptr samp_api;

    typedef dynamic_items<int /* object_key */, int /* id */> objects_player_t;
    objects_player_t objects_player;
    pos_change_checker checker;

public:
    void delete_event(int object_key);
    void reconfig_done_event();
    void update_player_pos(pos3 const& pos);
};

#endif // OBJECTS_HPP
