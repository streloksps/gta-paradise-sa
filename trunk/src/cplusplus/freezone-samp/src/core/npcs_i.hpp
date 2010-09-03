#ifndef NPCS_I_HPP
#define NPCS_I_HPP
#include "npc_fwd.hpp"
#include "core/samp/samp_events_t.hpp"

namespace npcs_events {
    struct on_npc_connect_i {
        virtual void on_npc_connect(npc_ptr_t const& npc_ptr) = 0;
    };

    struct on_npc_disconnect_i {
        virtual void on_npc_disconnect(npc_ptr_t const& npc_ptr, samp::player_disconnect_reason reason) = 0;
    };
} // namespace npcs_events {
#endif // NPCS_I_HPP
