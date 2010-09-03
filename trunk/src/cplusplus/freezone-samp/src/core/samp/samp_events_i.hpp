#ifndef SAMP_EVENTS_I_HPP
#define SAMP_EVENTS_I_HPP
#include <string>
#include "samp_events_t.hpp"

namespace samp {
    //События сампа

    struct on_pre_pre_gamemode_init_i {
        virtual void on_pre_pre_gamemode_init(AMX* amx, server_ver ver) = 0;
    };
    struct on_pre_gamemode_init_i {
        virtual void on_pre_gamemode_init(AMX* amx, server_ver ver) = 0;
    };
    struct on_gamemode_init_i {
        virtual void on_gamemode_init(AMX* amx, server_ver ver) = 0;
    };
    struct on_gamemode_exit_i {
        virtual void on_gamemode_exit(AMX* amx) = 0;
    };
    struct on_post_gamemode_exit_i {
        virtual void on_post_gamemode_exit(AMX* amx) = 0;
    };
    struct on_post_post_gamemode_exit_i {
        virtual void on_post_post_gamemode_exit(AMX* amx) = 0;
    };
    struct on_player_connect_i {
        virtual void on_player_connect(int player_id) = 0;
    };
    struct on_player_disconnect_i {
        virtual void on_player_disconnect(int player_id, player_disconnect_reason reason) = 0;
    };
    struct on_player_spawn_i {
        virtual void on_player_spawn(int player_id) = 0;
    };
    struct on_player_death_i {
        virtual void on_player_death(int player_id, int killer_id, int reason) = 0;
    };
    struct on_player_request_class_i {
        virtual void on_player_request_class(int player_id, int class_id) = 0;
    };
    struct on_player_enter_vehicle_i {
        virtual void on_player_enter_vehicle(int player_id, int vehicle_id, bool is_passenger) = 0;
    };
    struct on_player_exit_vehicle_i {
        virtual void on_player_exit_vehicle(int player_id, int vehicle_id) = 0;
    };
    struct on_player_state_change_i {
        virtual void on_player_state_change(int player_id, player_state state_new, player_state state_old) = 0;
    };
    struct on_player_enter_checkpoint_i {
        virtual void on_player_enter_checkpoint(int player_id) = 0;
    };
    struct on_player_leave_checkpoint_i {
        virtual void on_player_leave_checkpoint(int player_id) = 0;
    };
    struct on_player_enter_racecheckpoint_i {
        virtual void on_player_enter_racecheckpoint(int player_id) = 0;
    };
    struct on_player_leave_racecheckpoint_i {
        virtual void on_player_leave_racecheckpoint(int player_id) = 0;
    };
    struct on_player_request_spawn_i {
        // Истина = разрешить спавн
        virtual bool on_player_request_spawn(int player_id) = 0;
    };
    struct on_player_pickuppickup_i {
        virtual void on_player_pickuppickup(int player_id, int pickup_id) = 0;
    };
    struct on_player_interior_change_i {
        virtual void on_player_interior_change(int player_id, int interior_id_new, int interior_id_old) = 0;
    };
    struct on_player_keystate_change_i {
        virtual void on_player_keystate_change(int player_id, int keys_new, int keys_old) = 0;
    };
    struct on_player_update_i {
        // Истина = разрешить передавать изменения на других игроков
        virtual bool on_player_update(int player_id) = 0;
    };
    struct on_player_commandtext_i {
        virtual void on_player_commandtext(int player_id, std::string const& cmd) = 0;
    };
    struct on_player_text_i {
        virtual void on_player_text(int player_id, std::string const& text) = 0;
    };
    struct on_player_private_msg_i {
        virtual void on_player_private_msg(int player_id, int reciever_id, std::string const& text) = 0;
    };
    struct on_player_selected_menurow_i {
        virtual void on_player_selected_menurow(int player_id, int row) = 0;
    };
    struct on_player_exited_menu_i {
        virtual void on_player_exited_menu(int player_id) = 0;
    };
    struct on_vehicle_spawn_i {
        virtual void on_vehicle_spawn(int vehicle_id) = 0;
    };
    struct on_vehicle_death_i {
        virtual void on_vehicle_death(int vehicle_id, int killer_id) = 0;
    };
    struct on_enter_exit_mod_shop_i {
        virtual void on_enter_exit_mod_shop(int player_id, int enter_exit, int interior_id) = 0;
    };
    struct on_vehicle_mod_i {
        // Истина = разрешить передавать изменения на других игроков
        virtual bool on_vehicle_mod(int player_id, int vehicle_id, int component_id) = 0;
    };
    struct on_vehicle_paintjob_i {
        // Истина = разрешить передавать изменения на других игроков
        virtual bool on_vehicle_paintjob(int player_id, int vehicle_id, int paintjob_id) = 0;
    };
    struct on_vehicle_respray_i {
        // Истина = разрешить передавать изменения на других игроков
        virtual bool on_vehicle_respray(int player_id, int vehicle_id, int color1, int color2) = 0;
    };
    struct on_vehicle_damage_status_update_i {
        virtual void on_vehicle_damage_status_update(int vehicle_id, int player_id) = 0;
    };    
    struct on_object_moved_i {
        virtual void on_object_moved(int object_id) = 0;
    };
    struct on_playerobject_moved_i {
        virtual void on_playerobject_moved(int player_id, int object_id) = 0;
    };
    struct on_rconcommand_i {
        virtual bool on_rconcommand(std::string const& cmd) = 0;
    };
    struct on_rcon_login_attempt_i {
        virtual bool on_rcon_login_attempt(std::string const& ip, std::string const& password, bool is_success) = 0;
    };
    struct on_player_stream_in_i {
        virtual void on_player_stream_in(int player_id, int for_player_id) = 0;
    };
    struct on_player_stream_out_i {
        virtual void on_player_stream_out(int player_id, int for_player_id) = 0;
    };
    struct on_vehicle_stream_in_i {
        virtual void on_vehicle_stream_in(int vehicle_id, int for_player_id) = 0;
    };
    struct on_vehicle_stream_out_i {
        virtual void on_vehicle_stream_out(int vehicle_id, int for_player_id) = 0;
    };
    struct on_dialog_response_i {
        virtual void on_dialog_response(int player_id, int dialog_id, int response, int list_item, std::string const& input_text) = 0;
    };
    struct on_player_click_player_i {
        virtual void on_player_click_player(int player_id, int clicked_player_id, click_source source) = 0;
    };
} // namespace samp {
#endif // SAMP_EVENTS_I_HPP
