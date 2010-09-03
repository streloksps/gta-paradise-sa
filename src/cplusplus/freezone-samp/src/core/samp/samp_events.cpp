#include "config.hpp"
#include "samp_events.hpp"
#include "samp_events_i.hpp"
#include "pawn/pawn_events.hpp"
#include "core/application.hpp"
#include "core/container/container_handlers.hpp"

namespace samp {
    REGISTER_IN_APPLICATION(events);

    events::ptr events::instance() {
        return application::instance()->get_item<events>();
    }

    namespace {
        events* curr_events = 0;

        inline server_ver get_server_ver(int ver) {
            switch (ver) {
                case 1: return server_ver_022;
                case 2: return server_ver_02X;
                case 3: return server_ver_03a;
                case 4: return server_ver_03b;
                default: assert(false);
            }
            return server_ver_unknown;
        }

        inline player_disconnect_reason get_reason(int reason_int) {
            switch (reason_int) {
                case 0: return player_disconnect_reason_time_out;
                case 1: return player_disconnect_reason_quit;
                case 2: return player_disconnect_reason_kick_ban;
                default: assert(false);
            }
            return player_disconnect_reason_quit;
        }

        inline player_state get_player_state(int state_int) {
            switch (state_int) {
                case 0: return player_state_empty;
                case 1: return player_state_on_foot;
                case 2: return player_state_driver;
                case 3: return player_state_passenger;
                case 7: return player_state_wasted;
                case 8: return player_state_spawned;
                case 9: return player_state_spectating;
                default: assert(false);
            }
            return player_state_empty;
        }

        inline click_source get_click_source(int source) {
            return click_source_scoreboard;
        }
        
        void OnGameModeInit(AMX* amx, int ver_int) {
            server_ver ver = get_server_ver(ver_int);
            container_execute_handlers(application::instance(), &on_pre_pre_gamemode_init_i::on_pre_pre_gamemode_init, amx, ver);
            container_execute_handlers(application::instance(), &on_pre_gamemode_init_i::on_pre_gamemode_init, amx, ver);
            container_execute_handlers(application::instance(), &on_gamemode_init_i::on_gamemode_init, amx, ver);
        }

        void OnGameModeExit(AMX* amx) {
            container_execute_handlers(application::instance(), &on_gamemode_exit_i::on_gamemode_exit, amx);
            container_execute_handlers(application::instance(), &on_post_gamemode_exit_i::on_post_gamemode_exit, amx);
            container_execute_handlers(application::instance(), &on_post_post_gamemode_exit_i::on_post_post_gamemode_exit, amx);
        }

        void OnPlayerConnect(int player_id) {
            container_execute_handlers(application::instance(), &on_player_connect_i::on_player_connect, player_id);
        }

        void OnPlayerDisconnect(int player_id, int reason_int) {
            player_disconnect_reason reason = get_reason(reason_int);
            container_execute_handlers(application::instance(), &on_player_disconnect_i::on_player_disconnect, player_id, reason);
        }

        void OnPlayerSpawn(int player_id) {
            container_execute_handlers(application::instance(), &on_player_spawn_i::on_player_spawn, player_id);
        }

        void OnPlayerDeath(int player_id, int killer_id, int reason) {
            container_execute_handlers(application::instance(), &on_player_death_i::on_player_death, player_id, killer_id, reason);
        }

        void OnVehicleSpawn(int vehicle_id) {
            container_execute_handlers(application::instance(), &on_vehicle_spawn_i::on_vehicle_spawn, vehicle_id);
        }

        void OnVehicleDeath(int vehicle_id, int killer_id) {
            container_execute_handlers(application::instance(), &on_vehicle_death_i::on_vehicle_death, vehicle_id, killer_id);
        }

        bool OnPlayerText(int player_id, std::string const& text) {
            container_execute_handlers(application::instance(), &on_player_text_i::on_player_text, player_id, text);
            return false; // Всегда подавляем стандартный вывод текста
        }

        void OnPlayerCommandText(int player_id, std::string const& cmd) {
            container_execute_handlers(application::instance(), &on_player_commandtext_i::on_player_commandtext, player_id, cmd);
        }

        void OnPlayerRequestClass(int player_id, int class_id) {
            container_execute_handlers(application::instance(), &on_player_request_class_i::on_player_request_class, player_id, class_id);
        }

        void OnPlayerEnterVehicle(int player_id, int vehicle_id, bool is_passenger) {
            container_execute_handlers(application::instance(), &on_player_enter_vehicle_i::on_player_enter_vehicle, player_id, vehicle_id, is_passenger);
        }

        void OnPlayerExitVehicle(int player_id, int vehicle_id) {
            container_execute_handlers(application::instance(), &on_player_exit_vehicle_i::on_player_exit_vehicle, player_id, vehicle_id);
        }

        void OnPlayerStateChange(int player_id, int newstate_int, int oldstate_int) {
            player_state state_new = get_player_state(newstate_int);
            player_state state_old = get_player_state(oldstate_int);
            container_execute_handlers(application::instance(), &on_player_state_change_i::on_player_state_change, player_id, state_new, state_old);
        }

        void OnPlayerEnterCheckpoint(int player_id) {
            container_execute_handlers(application::instance(), &on_player_enter_checkpoint_i::on_player_enter_checkpoint, player_id);
        }

        void OnPlayerLeaveCheckpoint(int player_id) {
            container_execute_handlers(application::instance(), &on_player_leave_checkpoint_i::on_player_leave_checkpoint, player_id);
        }

        void OnPlayerEnterRaceCheckpoint(int player_id) {
            container_execute_handlers(application::instance(), &on_player_enter_racecheckpoint_i::on_player_enter_racecheckpoint, player_id);
        }

        void OnPlayerLeaveRaceCheckpoint(int player_id) {
            container_execute_handlers(application::instance(), &on_player_leave_racecheckpoint_i::on_player_leave_racecheckpoint, player_id);
        }

        bool OnRconCommand(std::string const& cmd) {
            return container_execute_handlers(application::instance(), &on_rconcommand_i::on_rconcommand, cmd, container_execute_rezult_true_if_have_one_true);
        }

        bool OnPlayerRequestSpawn(int player_id) {
            return container_execute_handlers(application::instance(), &on_player_request_spawn_i::on_player_request_spawn, player_id);
        }

        void OnObjectMoved(int object_id) {
            container_execute_handlers(application::instance(), &on_object_moved_i::on_object_moved, object_id);
        }

        void OnPlayerObjectMoved(int player_id, int object_id) {
            container_execute_handlers(application::instance(), &on_playerobject_moved_i::on_playerobject_moved, player_id, object_id);
        }

        void OnPlayerPickUpPickup(int player_id, int pickup_id) {
            container_execute_handlers(application::instance(), &on_player_pickuppickup_i::on_player_pickuppickup, player_id, pickup_id);
        }

        bool OnVehicleMod(int player_id, int vehicle_id, int component_id) {
            return container_execute_handlers(application::instance(), &on_vehicle_mod_i::on_vehicle_mod, player_id, vehicle_id, component_id);
        }
        
        void OnEnterExitModShop(int player_id, int enter_exit, int interior_id) {
            container_execute_handlers(application::instance(), &on_enter_exit_mod_shop_i::on_enter_exit_mod_shop, player_id, enter_exit, interior_id);
        }

        bool OnVehiclePaintjob(int player_id, int vehicle_id, int paintjob_id) {
            return container_execute_handlers(application::instance(), &on_vehicle_paintjob_i::on_vehicle_paintjob, player_id, vehicle_id, paintjob_id);
        }

        bool OnVehicleRespray(int player_id, int vehicle_id, int color1, int color2) {
            return container_execute_handlers(application::instance(), &on_vehicle_respray_i::on_vehicle_respray, player_id, vehicle_id, color1, color2);
        }

        void OnVehicleDamageStatusUpdate(int vehicle_id, int player_id) {
            container_execute_handlers(application::instance(), &on_vehicle_damage_status_update_i::on_vehicle_damage_status_update, vehicle_id, player_id);
        }

        void OnPlayerSelectedMenuRow(int player_id, int row) {
            container_execute_handlers(application::instance(), &on_player_selected_menurow_i::on_player_selected_menurow, player_id, row);
        }

        void OnPlayerExitedMenu(int player_id) {
            container_execute_handlers(application::instance(), &on_player_exited_menu_i::on_player_exited_menu, player_id);
        }

        void OnPlayerInteriorChange(int player_id, int interior_id_new, int interior_id_old) {
            container_execute_handlers(application::instance(), &on_player_interior_change_i::on_player_interior_change, player_id, interior_id_new, interior_id_old);
        }

        void OnPlayerKeyStateChange(int player_id, int keys_new, int keys_old) {
            container_execute_handlers(application::instance(), &on_player_keystate_change_i::on_player_keystate_change, player_id, keys_new, keys_old);
        }

        void OnRconLoginAttempt(std::string const& ip, std::string const& password, bool is_success) {
            container_execute_handlers(application::instance(), &on_rcon_login_attempt_i::on_rcon_login_attempt, ip, password, is_success);
        }

        bool OnPlayerUpdate(int player_id) {
            return container_execute_handlers(application::instance(), &on_player_update_i::on_player_update, player_id);
        }

        void OnPlayerStreamIn(int player_id, int for_player_id) {
            container_execute_handlers(application::instance(), &on_player_stream_in_i::on_player_stream_in, player_id, for_player_id);
        }

        void OnPlayerStreamOut(int player_id, int for_player_id) {
            container_execute_handlers(application::instance(), &on_player_stream_out_i::on_player_stream_out, player_id, for_player_id);
        }

        void OnVehicleStreamIn(int vehicle_id, int for_player_id) {
            container_execute_handlers(application::instance(), &on_vehicle_stream_in_i::on_vehicle_stream_in, vehicle_id, for_player_id);
        }

        void OnVehicleStreamOut(int vehicle_id, int for_player_id) {
            container_execute_handlers(application::instance(), &on_vehicle_stream_out_i::on_vehicle_stream_out, vehicle_id, for_player_id);
        }

        void OnDialogResponse(int player_id, int dialog_id, int response, int list_item, std::string const& input_text) {
            container_execute_handlers(application::instance(), &on_dialog_response_i::on_dialog_response, player_id, dialog_id, response, list_item, input_text);
        }

        void OnPlayerClickPlayer(int player_id, int clicked_player_id, int source_int) {
            click_source source = get_click_source(source_int);
            container_execute_handlers(application::instance(), &on_player_click_player_i::on_player_click_player, player_id, clicked_player_id, source);
        }
    }

    events::events() {
    }

    events::~events() {
    }

    void events::configure_pre() {
    }

    void events::configure(buffer::ptr const& buff, def_t const& def) {
    }

    void events::configure_post() {
    }
    
    void events::plugin_on_load() {
        // Получить список:
        // cat list | grep " On" | perl -pe 's/\(/ /g' | awk '{print $2}' | perl -pe 's/On(.+)/FZ${1} On${1}/' | awk '{printf("        pawn_events_ptr->add_callback(\"%s\", &%s);\n", $1, $2)}'
        pawn::events::ptr pawn_events_ptr = pawn::events::instance();
        pawn_events_ptr->add_callback("FZGameModeInit", &OnGameModeInit);
        pawn_events_ptr->add_callback("FZGameModeExit", &OnGameModeExit);
        pawn_events_ptr->add_callback("FZPlayerConnect", &OnPlayerConnect);
        pawn_events_ptr->add_callback("FZPlayerDisconnect", &OnPlayerDisconnect);
        pawn_events_ptr->add_callback("FZPlayerSpawn", &OnPlayerSpawn);
        pawn_events_ptr->add_callback("FZPlayerDeath", &OnPlayerDeath);
        pawn_events_ptr->add_callback("FZVehicleSpawn", &OnVehicleSpawn);
        pawn_events_ptr->add_callback("FZVehicleDeath", &OnVehicleDeath);
        pawn_events_ptr->add_callback("FZPlayerText", &OnPlayerText);
        pawn_events_ptr->add_callback("FZPlayerCommandText", &OnPlayerCommandText);
        pawn_events_ptr->add_callback("FZPlayerRequestClass", &OnPlayerRequestClass);
        pawn_events_ptr->add_callback("FZPlayerEnterVehicle", &OnPlayerEnterVehicle);
        pawn_events_ptr->add_callback("FZPlayerExitVehicle", &OnPlayerExitVehicle);
        pawn_events_ptr->add_callback("FZPlayerStateChange", &OnPlayerStateChange);
        pawn_events_ptr->add_callback("FZPlayerEnterCheckpoint", &OnPlayerEnterCheckpoint);
        pawn_events_ptr->add_callback("FZPlayerLeaveCheckpoint", &OnPlayerLeaveCheckpoint);
        pawn_events_ptr->add_callback("FZPlayerEnterRaceCheckpoint", &OnPlayerEnterRaceCheckpoint);
        pawn_events_ptr->add_callback("FZPlayerLeaveRaceCheckpoint", &OnPlayerLeaveRaceCheckpoint);
        pawn_events_ptr->add_callback("FZRconCommand", &OnRconCommand);
        pawn_events_ptr->add_callback("FZPlayerRequestSpawn", &OnPlayerRequestSpawn);
        pawn_events_ptr->add_callback("FZObjectMoved", &OnObjectMoved);
        pawn_events_ptr->add_callback("FZPlayerObjectMoved", &OnPlayerObjectMoved);
        pawn_events_ptr->add_callback("FZPlayerPickUpPickup", &OnPlayerPickUpPickup);
        pawn_events_ptr->add_callback("FZVehicleMod", &OnVehicleMod);
        pawn_events_ptr->add_callback("FZEnterExitModShop", &OnEnterExitModShop);
        pawn_events_ptr->add_callback("FZVehiclePaintjob", &OnVehiclePaintjob);
        pawn_events_ptr->add_callback("FZVehicleRespray", &OnVehicleRespray);
        pawn_events_ptr->add_callback("FZVehicleDamageStatusUpdate", &OnVehicleDamageStatusUpdate);
        pawn_events_ptr->add_callback("FZPlayerSelectedMenuRow", &OnPlayerSelectedMenuRow);
        pawn_events_ptr->add_callback("FZPlayerExitedMenu", &OnPlayerExitedMenu);
        pawn_events_ptr->add_callback("FZPlayerInteriorChange", &OnPlayerInteriorChange);
        pawn_events_ptr->add_callback("FZPlayerKeyStateChange", &OnPlayerKeyStateChange);
        pawn_events_ptr->add_callback("FZRconLoginAttempt", &OnRconLoginAttempt);
        pawn_events_ptr->add_callback("FZPlayerUpdate", &OnPlayerUpdate);
        pawn_events_ptr->add_callback("FZPlayerStreamIn", &OnPlayerStreamIn);
        pawn_events_ptr->add_callback("FZPlayerStreamOut", &OnPlayerStreamOut);
        pawn_events_ptr->add_callback("FZVehicleStreamIn", &OnVehicleStreamIn);
        pawn_events_ptr->add_callback("FZVehicleStreamOut", &OnVehicleStreamOut);
        pawn_events_ptr->add_callback("FZDialogResponse", &OnDialogResponse);
        pawn_events_ptr->add_callback("FZPlayerClickPlayer", &OnPlayerClickPlayer);
    }
} // namespace samp {
