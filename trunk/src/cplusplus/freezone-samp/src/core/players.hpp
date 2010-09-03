#ifndef PLAYERS_HPP
#define PLAYERS_HPP
#include <map>
#include <vector>
#include <memory>
#include "core/container/application_item.hpp"
#include "core/createble_i.hpp"
#include "core/samp/samp_events_i.hpp"
#include "core/player/player_fwd.hpp"
#include "core/player/player_t.hpp"
#include "core/samp/samp_api.hpp"

class players
    :public application_item
    ,public createble_i
    ,public samp::on_player_connect_i
    ,public samp::on_player_disconnect_i
    ,public samp::on_player_spawn_i
    ,public samp::on_player_death_i
    ,public samp::on_player_request_class_i
    ,public samp::on_player_enter_vehicle_i
    ,public samp::on_player_exit_vehicle_i
    ,public samp::on_player_state_change_i
    ,public samp::on_player_enter_checkpoint_i
    ,public samp::on_player_leave_checkpoint_i
    ,public samp::on_player_enter_racecheckpoint_i
    ,public samp::on_player_leave_racecheckpoint_i
    ,public samp::on_player_request_spawn_i
    ,public samp::on_player_pickuppickup_i
    ,public samp::on_player_interior_change_i
    ,public samp::on_player_keystate_change_i
    ,public samp::on_player_update_i
    ,public samp::on_player_commandtext_i
    ,public samp::on_player_text_i
    ,public samp::on_player_private_msg_i
    ,public samp::on_player_selected_menurow_i
    ,public samp::on_player_exited_menu_i
    ,public samp::on_player_click_player_i
    ,public samp::on_post_gamemode_exit_i
    ,public samp::on_player_stream_in_i
    ,public samp::on_player_stream_out_i
    ,public samp::on_vehicle_stream_in_i
    ,public samp::on_vehicle_stream_out_i
{
public:
    typedef std::tr1::shared_ptr<players> ptr;
    static ptr instance();

public:
    players();
    virtual ~players();

public: //Публичные методы
    //Возращает истину, если игрок с заданным ид подключен к серверу
    bool is_valid_player_id(unsigned int id);

    //Возращает указатель на объект угрока. Если такого игрока нет - то нулевой указатель
    player_ptr_t get_player(unsigned int id);

    //Возращает вектор указателей на игроков сервера
    players_t get_players();

    std::size_t get_players_count() const;
    std::size_t get_players_max() const;

    static std::string get_players_names(players_t const& players_list);

public: // createble_i
    virtual void create();

public: // samp::*
    virtual void on_player_connect(int player_id);
    virtual void on_player_disconnect(int player_id, samp::player_disconnect_reason reason);
    virtual void on_player_spawn(int player_id);
    virtual void on_player_death(int player_id, int killer_id, int reason);
    virtual void on_player_request_class(int player_id, int class_id);
    virtual void on_player_enter_vehicle(int player_id, int vehicle_id, bool is_passenger);
    virtual void on_player_exit_vehicle(int player_id, int vehicle_id);
    virtual void on_player_state_change(int player_id, samp::player_state state_new, samp::player_state state_old);
    virtual void on_player_enter_checkpoint(int player_id);
    virtual void on_player_leave_checkpoint(int player_id);
    virtual void on_player_enter_racecheckpoint(int player_id);
    virtual void on_player_leave_racecheckpoint(int player_id);
    virtual bool on_player_request_spawn(int player_id); // Истина = разрешить спавн
    virtual void on_player_pickuppickup(int player_id, int pickup_id);
    virtual void on_player_interior_change(int player_id, int interior_id_new, int interior_id_old);
    virtual void on_player_keystate_change(int player_id, int keys_new, int keys_old);
    virtual bool on_player_update(int player_id); // Истина = разрешить передавать изменения на других игроков
    virtual void on_player_commandtext(int player_id, std::string const& cmd);
    virtual void on_player_text(int player_id, std::string const& text);
    virtual void on_player_private_msg(int player_id, int reciever_id, std::string const& text);
    virtual void on_player_selected_menurow(int player_id, int row);
    virtual void on_player_exited_menu(int player_id);
    virtual void on_player_click_player(int player_id, int clicked_player_id, samp::click_source source);
    virtual void on_post_gamemode_exit(AMX* amx);
    virtual void on_player_stream_in(int player_id, int for_player_id);
    virtual void on_player_stream_out(int player_id, int for_player_id);
    virtual void on_vehicle_stream_in(int vehicle_id, int for_player_id);
    virtual void on_vehicle_stream_out(int vehicle_id, int for_player_id);

private:
    typedef std::map<unsigned int, player_ptr_t> players_holder_t;
    players_holder_t players_holder;
    samp::api::ptr api_ptr;
};
#endif // PLAYERS_HPP
