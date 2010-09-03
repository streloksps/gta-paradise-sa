#ifndef PLAYER_I_HPP
#define PLAYER_I_HPP
#include "player_fwd.hpp"
#include "core/samp/samp_events_t.hpp"

namespace player_events {
    struct on_connect_i {
        virtual void on_connect() = 0;
    };

    struct on_disconnect_i {
        virtual void on_disconnect(samp::player_disconnect_reason reason) = 0;
    };

    struct on_spawn_i {
        virtual void on_spawn() = 0;
    };

    struct on_death_i {
        virtual void on_death(player_ptr_t const& killer_ptr, int reason) = 0;
    };

    struct on_death_post_i {
        virtual void on_death_post(player_ptr_t const& killer_ptr, int reason) = 0;
    };

    struct on_info_change_i {
        virtual void on_info_change() = 0;
    };

    struct on_request_class_i {
        virtual void on_request_class(int class_id) = 0;
    };

    struct on_enter_vehicle_i {
        virtual void on_enter_vehicle(int vehicle_id, bool is_passenger) = 0;
    };

    struct on_exit_vehicle_i {
        virtual void on_exit_vehicle(int vehicle_id) = 0;
    };

    struct on_state_change_i {
        virtual void on_state_change(samp::player_state state_new, samp::player_state state_old) = 0;
    };

    struct on_enter_checkpoint_i {
        virtual void on_enter_checkpoint() = 0;
    };

    struct on_leave_checkpoint_i {
        virtual void on_leave_checkpoint() = 0;
    };

    struct on_enter_racecheckpoint_i {
        virtual void on_enter_racecheckpoint() = 0;
    };

    struct on_leave_racecheckpoint_i {
        virtual void on_leave_racecheckpoint() = 0;
    };

    struct on_request_spawn_i {
        virtual bool on_request_spawn() = 0; //Истина = разрешить спавн
    };

    struct on_pickuppickup_i {
        virtual void on_pickuppickup(int pickup_id) = 0;
    };

    struct on_interior_change_i {
        virtual void on_interior_change(int interior_id_new, int interior_id_old) = 0;
    };

    struct on_keystate_change_i {
        virtual void on_keystate_change(int keys_new, int keys_old) = 0;
    };

    struct on_update_i {
        virtual bool on_update() = 0; //Истина = разрешить передавать изменения на других игроков
    };

    struct on_commandtext_i {
        virtual void on_commandtext(std::string const& cmd) = 0;
    };

    struct on_text_i {
        virtual void on_text(std::string const& text) = 0;
    };

    struct on_private_msg_i {
        virtual void on_private_msg(player_ptr_t const& reciever_ptr, std::string const& text) = 0;
    };

    struct on_selected_menurow_i {
        virtual void on_selected_menurow(int row) = 0;
    };

    struct on_exited_menu_i {
        virtual void on_exited_menu() = 0;
    };

    struct on_click_player_i {
        virtual void on_click_player(player_ptr_t const& clicked_player, samp::click_source source) = 0;
    };

    struct on_player_stream_in_i {
        virtual void on_player_stream_in(player_ptr_t const& player_ptr) = 0;
    };

    struct on_player_stream_out_i {
        virtual void on_player_stream_out(player_ptr_t const& player_ptr) = 0;
    };

    struct on_vehicle_stream_in_i {
        virtual void on_vehicle_stream_in(int vehicle_id) = 0;
    };

    struct on_vehicle_stream_out_i {
        virtual void on_vehicle_stream_out(int vehicle_id) = 0;
    };

} // namespace player_events {

#endif // PLAYER_I_HPP
