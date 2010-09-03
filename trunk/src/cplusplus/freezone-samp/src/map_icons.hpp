#ifndef MAP_ICONS_HPP
#define MAP_ICONS_HPP
#include "core/module_h.hpp"
#include "core/dynamic_items.hpp"
#include "core/streamer.hpp"
#include "map_icons_t.hpp"
#include "pos_change_checker.hpp"
#include <vector>

class map_icons_item;
class map_icons
    :public application_item
    ,public configuradable
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<map_icons> ptr;
    static ptr instance();

public:
    friend class map_icons_item;
    map_icons();
    virtual ~map_icons();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(map_icons);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

public:
    enum {max_icons = 32};
    enum {invalid_icon_id = -1};

private:
    typedef dynamic_items<map_icon_t, int> map_icons_t;
    map_icons_t buffer;
    void streamer_item_deleter(int key);

private:
    // Можно использовать извне для создания и удаления иконок
    typedef streamer::streamer_t<map_icon_t> map_icons_streamer_t;
    map_icons_streamer_t streamer;
public:
    int add(float x, float y, float z, int interior, int world, float radius, int marker_type);
    void remove(int id);
};

class map_icons_item
    :public player_item
    ,public on_timer250_i
{
public:
    typedef std::tr1::shared_ptr<map_icons_item> ptr;

private:
    friend class map_icons;
    map_icons_item(map_icons& manager);
    virtual ~map_icons_item();

public: // player_events::
    virtual void on_timer250();

private:
    int mapicon_create(int icon_key);
    void mapicon_destroy(int id);

    typedef std::vector<bool> mapicons_in_use_t;
    mapicons_in_use_t mapicons_in_use;

private:
    map_icons& manager;

    typedef dynamic_items<int /* icon_key */, int /* id */> mapicons_player_t;
    mapicons_player_t mapicons_player;
    pos_change_checker checker;

public:
    void delete_event(int icon_key);
    void reconfig_done_event();
};

#endif // MAP_ICONS_HPP
