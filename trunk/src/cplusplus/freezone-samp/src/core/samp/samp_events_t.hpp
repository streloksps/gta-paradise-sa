#ifndef SAMP_EVENTS_T_HPP
#define SAMP_EVENTS_T_HPP
#include "pawn/pawn_amx_fwd.hpp"

namespace samp {
    enum server_ver {
         server_ver_unknown = 0
        ,server_ver_022 = 1
        ,server_ver_02X = 2
        ,server_ver_03a = 3
        ,server_ver_03b = 4
    };

    enum player_disconnect_reason {
         player_disconnect_reason_time_out = 0
        ,player_disconnect_reason_quit = 1
        ,player_disconnect_reason_kick_ban = 2
        ,player_disconnect_reason_server_shutdown = 100
    };

    enum player_state {
         player_state_empty = 0
        ,player_state_on_foot = 1
        ,player_state_driver = 2
        ,player_state_passenger = 3
        ,player_state_wasted = 7
        ,player_state_spawned = 8
        ,player_state_spectating = 9
    };

    enum click_source {
        click_source_scoreboard = 0
    };
} // namespace samp {

#endif // SAMP_EVENTS_T_HPP
