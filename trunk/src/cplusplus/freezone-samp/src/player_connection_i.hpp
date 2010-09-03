#ifndef PLAYER_CONNECTION_I_HPP
#define PLAYER_CONNECTION_I_HPP

namespace server_events {
    enum players_count_change_reason {
        players_count_change_reason_connect
        ,players_count_change_reason_disconnect
    };
    struct on_players_count_change_i {
        virtual void on_players_count_change(int players_count, int players_max, players_count_change_reason reason) = 0;
    };
}

#endif // PLAYER_CONNECTION_I_HPP
