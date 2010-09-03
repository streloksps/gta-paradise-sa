#ifndef PAWN_PLUGIN_I_HPP
#define PAWN_PLUGIN_I_HPP
#include "pawn_amx_fwd.hpp"
namespace pawn {

    // Событие загрузки плагина
    struct plugin_on_pre_load_i {
        virtual void plugin_on_pre_load() = 0;
    };
    struct plugin_on_load_i {
        virtual void plugin_on_load() = 0;
    };

    // Событие выгрузки плагина
    struct plugin_on_unload_i {
        virtual void plugin_on_unload() = 0;
    };
    struct plugin_on_post_unload_i {
        virtual void plugin_on_post_unload() = 0;
    };

    // Событие подгрузки amx (мода или плагина)
    struct plugin_on_pre_amx_load_i {
        virtual void plugin_on_load(AMX* amx) = 0;
    };
    struct plugin_on_amx_load_i {
        virtual void plugin_on_amx_load(AMX* amx) = 0;
    };

    // Событие выгрузки amx (мода или плагина)
    struct plugin_on_amx_unload_i {
        virtual void plugin_on_amx_unload(AMX* amx) = 0;
    };
    struct plugin_on_post_amx_unload_i {
        virtual void plugin_on_post_amx_unload(AMX* amx) = 0;
    };

    // Событие тика сервера
    struct plugin_on_tick_i {
        virtual void plugin_on_tick() = 0;
    };
} // namespace pawn {
#endif // PAWN_PLUGIN_I_HPP
