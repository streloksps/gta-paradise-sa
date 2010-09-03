#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
#include "core/container/container.hpp"
#include "core/geo/geo_ip_info.hpp"
#include "basic_types.hpp"
#include "core/samp/samp_api.hpp"
#include "core/game_context.hpp"
#include "player_i.hpp"
#include "player_t.hpp"
#include "core/timers_i.hpp"
#include "player_health_i.hpp"

enum player_state {
    player_not_conected = 0
   ,player_not_authorized   //
   ,player_not_in_game      // ����� �� ���� ����� � ����
   ,player_in_game          // ����� ����� � ����
   ,player_not_spawn        // ����� 1 ��� ����� � ����, �� ������ �� � ����
};

class player_name;
class weapons_item;
class player_messages;
class player_messages_bubble_item;
class player_position_item;

class player
    :public container
    ,public on_timer250_i
    ,public player_events::on_spawn_i
    ,public player_events::on_health_suicide_i
    ,public player_events::on_health_kill_i
    ,public player_events::on_player_stream_in_i
    ,public player_events::on_player_stream_out_i
    ,public player_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<player> ptr;
public:
    player(unsigned int id);
    virtual ~player();
    ptr shared_from_this();

public: // player_events::*
    virtual void on_spawn();
    virtual void on_health_suicide(int reason, bool is_by_script);
    virtual void on_health_kill(player_ptr_t const& killer_ptr, int reason);
    virtual void on_timer250();
    virtual void on_player_stream_in(player_ptr_t const& player_ptr);
    virtual void on_player_stream_out(player_ptr_t const& player_ptr);
    virtual void on_disconnect(samp::player_disconnect_reason reason);

public:
    unsigned int get_id() const;
    geo::ip_info const& get_geo() const;

    std::string const& get_serial() const;

public:
    //������ � ������� ����������. ��������� ������ ��������
    game_context::ptr game_context_activate(game_context::ptr game_context_ptr);
    void game_context_activate_async(game_context::ptr game_context_ptr);

private:
    game_context::ptr game_context_active_ptr;
    game_context::ptr game_context_wait_active_ptr;
    void game_context_process_wait();
    game_context::ptr game_context_activate_impl(game_context::ptr const& game_context_ptr);

private: //������ � �������
    friend class weapons_item;
    void            weapon_reset(); // �������� ������ � ������
    void            weapon_add(weapon_item_t const& weapon); // ��������� ������ ������
    weapons_t       weapon_get(); // ��������� ������ ������ ������. ��������� ���� �������� ������
    weapon_item_t   weapon_get_current(); // ��������� ������� ������

private: // ������ � ������������
    friend class player_position_item;

    pos4 pos_get();
    void pos_set(pos4 const& pos);
    void pos_set(pos4 const& pos, pos_camera const& pos_cam);
    void world_bounds_set(world_bounds_t const& world_bounds);
    void pos_set_only_int_world(pos4 const& pos);
    void camera_set(pos_camera const& pos_cam);

    void freeze();
    void unfreeze();
public:

    void time_set(player_time const& time);
    void weather_set(int weather_id);

    //������ �� ������
    bool spectate_is_active();
    void spectate_do(player_ptr_t const& spec_player_ptr);
    void spectate_do();
    void spectate_stop();

    //������ � ������
    void					health_set(player_health_armour const& health);
    void                    health_set_inf();
    player_health_armour	health_get();
    void					kill();


/************************************************************************/
/* ������ � ������                                                      */
/************************************************************************/
public:
    void name_reset();
    void name_set(std::string const& new_name);
    void name_set_for_chat();
    void name_set_for_chat_close();
    std::string name_get() const;               // ��������� ��� ������
    std::string name_normalized_get() const;    // ��������� ��������������� ��� ������ - ����������� ��������� �����������
    std::string name_get_full() const;

private:
    friend class player_name;
    std::string name;
    std::string name_normalized;
    std::string name_for_chat;
    std::string name_for_chat_close;
    std::string name_full;
    std::string name_last; // �������� ������ ��� - ����� �� ������ ������ ��������� ���

/************************************************************************/
/* ������ � ��������                                                    */
/************************************************************************/
public:
    std::string group_get_name() const;
    bool group_is_in(std::string const& group_name) const;

/************************************************************************/
/* ������ � ���                                                         */
/************************************************************************/
    std::string geo_get_long_desc() const;
    inline geo::ip_info const& geo_get_ip_info() const {return geo_ip_info;}
private:
    unsigned int id;
    geo::ip_info geo_ip_info;
    std::string serial;

private:
    samp::api::ptr api_ptr;

public:
    void set_color(unsigned int color);

/************************************************************************/
/* ������ �� ��������                                                   */
/************************************************************************/
public:
    // ��������� ����� � ������������, ���������� � ����
    int time_connection_get();

private:
    std::time_t time_connect;


public:
    player_state get_state() const;

private:
    player_state state;

public:
    void kick();
    void ban(std::string const& comment);

private: // ����� ���������
    friend class player_messages;
    friend class player_messages_bubble_item;
    void send_client_message(unsigned color, std::string const& message);
    void set_chat_bubble(unsigned int color, float draw_distance, int expire_time, std::string const& text);
    void send_player_message(ptr const& sender_ptr, std::string const& message);

public:
    static void send_death_message(ptr const& killer_ptr, ptr const& killed_ptr, int weapon_id, bool is_process_names = true);
    static void send_death_message(ptr const& killed_ptr, int weapon_id, bool is_process_names = true);

    // ������ �� ������
    int get_score();
    void set_score(int score);

public:
    // ��������� ������, ���� ����� � ����������
    bool vehicle_is_in() const;

    // ��������� ������, ���� ����� ��������
    bool vehicle_is_driver() const;

public: // ��������
    bool animation_is_use_phone() const;

public:
    bool is_rcon_admin() const;

public:
    void special_action_set(samp::api::special_action action_id = samp::api::SPECIAL_ACTION_NONE);
    samp::api::special_action special_action_get() const;
public:
    bool get_vehicle(int& vehicle_id, int& model_id, bool& is_driver);

private:
    bool is_blocked;
public:
    bool block_get() const;
    void block(std::string const& reason_group, std::string const& reason_info = std::string());

public:
    players_t const& stream_in_players_get() const {return stream_in_players;}

private:
    players_t stream_in_players;
};

#endif // PLAYER_HPP
