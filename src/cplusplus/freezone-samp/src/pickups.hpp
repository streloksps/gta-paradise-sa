#ifndef PICKUPS_HPP
#define PICKUPS_HPP
#include "core/module_h.hpp"
#include "core/dynamic_items.hpp"
#include "core/streamer.hpp"
#include "pickups_t.hpp"

class pickups
    :public application_item
    ,public configuradable
    ,public players_events::on_connect_i
    ,public on_timer500_i
{
public:
    typedef std::tr1::shared_ptr<pickups> ptr;
    static ptr instance();

public:
    pickups();
    virtual ~pickups();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(pickups);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

public: // on_timer
    virtual void on_timer500();

public:
    enum {max_pickups = 400};

private:
    typedef dynamic_items<pickup_t, int> map_icons_t;
    map_icons_t buffer;

public: // Можно использовать извне для создания и удаления пикапов
    typedef streamer::streamer_t<pickup_t> pickups_streamer_t;
    pickups_streamer_t streamer;

private: // Текущие пикапы
    typedef dynamic_items<int /* pickup_key */, int /* id */> work_pickups_t;
    work_pickups_t work_pickups;
    int     pickup_create(int pickup_key);
    void    pickup_destroy(int id);
    void    pickup_delete_event(int pickup_key);
};
#endif // PICKUPS_HPP
