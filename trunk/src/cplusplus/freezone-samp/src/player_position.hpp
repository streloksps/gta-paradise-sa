#ifndef PLAYER_POSITION_HPP
#define PLAYER_POSITION_HPP
#include "core/module_h.hpp"
#include "basic_types.hpp"
#include "core/time_outs.hpp"

class player_position_item;
class player_position
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<player_position> ptr;

public:
    friend class player_position_item;
    player_position();
    virtual ~player_position();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_position);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

private:
    float preload_min_radius;
    int   preload_time;                     // Время в милисекундах, необходимое для предзагрузки
    float preload_camera_shift_x;
    float preload_camera_shift_y;
    float preload_camera_shift_z;
    float preload_camera_shift_look_x;
    float preload_camera_shift_look_y;
    float preload_camera_shift_look_z;
    float preload_camera_shift_distance;
    float preload_z;

    text_draw::td_item::ptr loading;

    enum {preload_world = 1};

    samp::api::ptr api_ptr;
};

class player_position_item
    :public player_item
    ,public on_timer500_i
    ,public player_events::on_death_i
    ,public player_events::on_update_i
{
public:
    typedef std::tr1::shared_ptr<player_position_item> ptr;
private:
    friend class player_position;
    player_position_item(player_position& manager);
    virtual ~player_position_item();

public:
    pos4 pos_get();
    void pos_set(pos4 const& pos);
    void pos_set(pos4 const& pos, pos_camera const& pos_cam);
    void pos_set_ex(pos4 const& pos);
    void world_bounds_set(world_bounds_t const& world_bounds);
    void camera_set(pos_camera const& pos_cam);
    bool is_preload() const;
    float get_speed(bool is_food) const;

public: // Работа с транспортом
    void pos_vehicle_set(pos4 const& pos);
    void pos_vehicle_set_ex(pos4 const& pos);

public: // player_events::*
    virtual void on_timer500();
    virtual void on_death(player_ptr_t const& killer_ptr, int reason);
    virtual bool on_update();

private:
    player_position& manager;

private:
    void pos_set_impl(pos4 const& pos);
    void pos_set_impl(pos4 const& pos, pos_camera const& pos_cam);
    void pos_set_preload(pos4 const& pos);
    void pos_set_preload_end();
    void pos_set_last(pos4 const& pos);
    pos_camera get_preload_camera_for_pos(pos4 const& pos);

private:
    pos4                        last_pos;

    bool                        is_preload_active;
    pos4                        preload_pos;
    pos_camera                  preload_camera;
    time_base::millisecond_t    preload_end_time;

private: // Работа с транспортом
    void pos_vehicle_set_impl(pos4 const& pos);
    void pos_vehicle_set_preload(pos4 const& pos);
};

#endif // PLAYER_POSITION_HPP
