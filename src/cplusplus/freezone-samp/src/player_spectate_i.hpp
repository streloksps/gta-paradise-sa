#ifndef PLAYER_SPECTATE_I_HPP
#define PLAYER_SPECTATE_I_HPP
#include <ctime>
#include "core/player/player_fwd.hpp"

namespace player_events {
    struct on_spectate_start_i {
        virtual void on_spectate_start(player_ptr_t const& spectated_player_ptr) = 0;
    };
    struct on_spectate_change_i {
        virtual void on_spectate_change(player_ptr_t const& new_spectated_player_ptr) = 0;
    };
    struct on_spectate_stop_i {
        virtual void on_spectate_stop(std::time_t spectate_time) = 0;
    };
} // namespace player_events {
#endif // PLAYER_SPECTATE_I_HPP
