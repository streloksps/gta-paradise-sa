#ifndef PLAYER_HEALTH_I_HPP
#define PLAYER_HEALTH_I_HPP
#include "core/player/player_fwd.hpp"

namespace player_events {
    struct on_health_suicide_i {
        virtual void on_health_suicide(int reason, bool is_by_script) = 0;
    };

    struct on_health_kill_i {
        virtual void on_health_kill(player_ptr_t const& killer_ptr, int reason) = 0;
    };
} // namespace player_events {
#endif // PLAYER_HEALTH_I_HPP
