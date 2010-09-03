#ifndef PLAYERS_I_HPP
#define PLAYERS_I_HPP
#include "core/player/player_fwd.hpp"
#include "core/samp/samp_events_t.hpp"

namespace players_events {
    struct on_pre_rejectable_connect_i {
        // Истина - разрешить подключение
        // Если хотябы 1 метод вырнул ложь - игрок кикается, никакие другие события не вызываются
        virtual bool on_pre_rejectable_connect(player_ptr_t const& player_ptr) = 0;
    };

    struct on_rejectable_connect_i {
        // Истина - разрешить подключение
        // Если хотябы 1 метод вырнул ложь - игрок кикается, никакие другие события не вызываются
        virtual bool on_rejectable_connect(player_ptr_t const& player_ptr) = 0;
    };

    struct on_pre_connect_i {
        virtual void on_pre_connect(player_ptr_t const& player_ptr) = 0;
    };

    struct on_connect_i {
        virtual void on_connect(player_ptr_t const& player_ptr) = 0;
    };

    struct on_disconnect_i {
        virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) = 0;
    };
} // namespace players_events {

#endif // PLAYERS_I_HPP
