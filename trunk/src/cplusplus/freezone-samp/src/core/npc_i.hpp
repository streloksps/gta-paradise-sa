#ifndef NPC_I_HPP
#define NPC_I_HPP
#include <string>
#include "core/npc_fwd.hpp"
#include "core/samp/samp_events_t.hpp"

namespace npc_events {
    struct on_connect_i {
        virtual void on_connect() = 0;
    };

    struct on_disconnect_i {
        virtual void on_disconnect(samp::player_disconnect_reason reason) = 0;
    };

    struct on_request_class_i {
        virtual void on_request_class() = 0;
    };

    struct on_request_spawn_i {
        virtual bool on_request_spawn() = 0; //Истина = разрешить спавн
    };

    struct on_spawn_i {
        virtual void on_spawn() = 0;
    };

    struct on_enter_vehicle_i {
        virtual void on_enter_vehicle(int vehicle_id) = 0;
    };

    struct on_exit_vehicle_i {
        virtual void on_exit_vehicle(int vehicle_id) = 0;
    };

    struct on_commandtext_i {
        virtual void on_commandtext(std::string const& cmd) = 0;
    };

    struct on_playback_end_i {
        virtual void on_playback_end() = 0;
    };

    struct on_keystate_change_i {
        virtual void on_keystate_change(int keys_new, int keys_old) = 0;
    };

} // namespace npc_events {

#endif // NPC_I_HPP
