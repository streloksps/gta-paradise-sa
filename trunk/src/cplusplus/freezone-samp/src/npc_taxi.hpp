#ifndef NPC_TAXI_HPP
#define NPC_TAXI_HPP
#include "core/module_h.hpp"
#include "core/dynamic_items.hpp"
#include "core/time_outs.hpp"
#include "npc_taxi_t.hpp"
#include <map>

class npc_taxi_item;
class npc_taxi
    :public application_item
    ,public configuradable
    ,public samp::on_pre_pre_gamemode_init_i
    ,public samp::on_post_post_gamemode_exit_i
    ,public on_timer1000_i
    ,public npcs_events::on_npc_connect_i
    ,public npcs_events::on_npc_disconnect_i
{
    friend class npc_taxi_item;
public:
    npc_taxi();
    virtual ~npc_taxi();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(npc_taxi);

public: // samp::*
    virtual void on_pre_pre_gamemode_init(AMX*, samp::server_ver ver);
    virtual void on_post_post_gamemode_exit(AMX* amx);

public:
    virtual void on_timer1000();

public: // npcs_events::*
    virtual void on_npc_connect(npc_ptr_t const& npc_ptr);
    virtual void on_npc_disconnect(npc_ptr_t const& npc_ptr, samp::player_disconnect_reason reason);

private:
    typedef dynamic_items<npc_taxi_route_t, int> routes_t;
    typedef std::map<std::string, npc_taxi_route_vehicle_stop_t> vehicle_stops_t;
    
    routes_t        routes;
    vehicle_stops_t vehicle_stops;
    bool            is_nitro_active;
    bool            is_active;
    static void update_stops(vehicle_stops_t const& vehicle_stops, routes_t& routes);

private: // Функции работы с маршрутами (вызываются после конфигурирования)
    int route_create(npc_taxi_route_t const& route);
    void route_destroy(int route_id);

private: // Хранилище маршрутов
    struct route_npc_t {
        int                             vehicle_id;
        int                             npc_name_id;
        int                             skin_id;
        npc_taxi_route_vehicle_stop_t   stop;
        npc_ptr_t                       npc_ptr;
        route_npc_t(int vehicle_id, int npc_name_id, int skin_id, npc_taxi_route_vehicle_stop_t const& stop)
            :vehicle_id(vehicle_id)
            ,npc_name_id(npc_name_id)
            ,skin_id(skin_id)
            ,stop(stop)
        {}
    };
    typedef std::vector<route_npc_t> route_npcs_t;

    struct route_section_t {
        int                             checkpoint_id;
        route_section_t(int checkpoint_id): checkpoint_id(checkpoint_id) {}
    };

    typedef std::vector<route_section_t> route_sections_t;
    
    struct route_data_t {
        npc_taxi_route_t            route;
        route_npcs_t                route_npcs;
        route_sections_t            route_sections;
        int                         ready_count;
        time_base::millisecond_t    start_time;
        route_data_t(npc_taxi_route_t const& route): route(route), ready_count(0), start_time(0) {}
    };
    typedef std::map<int, route_data_t> route_datas_t;

    int             route_last_id;
    int             route_npc_name_last_id;

    // Рабочее
    route_datas_t   route_datas;

private:
    static std::string  make_npc_name(int route_item_id);
    static bool         parse_npc_name(std::string const& name, int& route_npc_name_id);
    bool                get_npc_route_and_item_ids(int route_npc_name_id, int& route_id, int& route_npc_id) const;
    static pos3         get_stop_pos(npc_taxi_route_section_t const& section, npc_taxi_route_vehicle_stop_t const& stop);

private:
    void stop_on_enter(player_ptr_t const& player_ptr, int id);
    void stop_on_leave(player_ptr_t const& player_ptr, int id);
};

class npc_taxi_item
    :public npc_item
    ,public on_timer250_i
    ,public npc_events::on_connect_i
    ,public npc_events::on_disconnect_i
    ,public npc_events::on_spawn_i
    ,public npc_events::on_enter_vehicle_i
    ,public npc_events::on_playback_end_i
    ,public npc_events::on_keystate_change_i
{
public:
    typedef std::tr1::shared_ptr<npc_taxi_item> ptr;

private:
    npc_taxi_item(npc_taxi& manager, int route_id, int route_npc_id);
    virtual ~npc_taxi_item();

private:
    friend class npc_taxi;
    npc_taxi&   manager;
    int         route_id;
    int         route_npc_id;
    bool        is_can_use_nitro;

private:
    struct state_t {enum state_e {oninit, onstop, onroute, onroute_end};};
    
    bool is_ready;
    state_t::state_e state;

    void start_stop(int section_id);
    void start_route(int section_id);
    void start_wait_stop();

private:
    npc_taxi::route_data_t* route_data_ptr;
    npc_taxi::route_npc_t* route_item_ptr;
    bool is_valid_data(); // Обновляет и проверяет route_data_ptr и route_item_ptr Возращает истину, если их можно использовать

public:
    virtual void on_timer250();

public: // npc_events::*
    virtual void on_connect();
    virtual void on_spawn();
    virtual void on_enter_vehicle(int vehicle_id);
    virtual void on_playback_end();
    virtual void on_disconnect(samp::player_disconnect_reason reason);
    virtual void on_keystate_change(int keys_new, int keys_old);

private:
    struct time_on_route_t {
        bool    is_stop;
        int     section_id;
        int     time_curr;
        int     time_all;
        time_on_route_t(bool is_stop, int section_id, int time_curr, int time_all)
            :is_stop(is_stop), section_id(section_id), time_curr(time_curr), time_all(time_all) {}
    };
    time_on_route_t get_time_on_route() const;
};

#endif // NPC_TAXI_HPP
