#ifndef SAMP_HOOK_EVENTS_HPP
#define SAMP_HOOK_EVENTS_HPP
#include <string>
#include <memory>
#include "core/module_h.hpp"
#include "pawn/pawn_plugin_i.hpp"

namespace samp {
    class hook_events
        :public application_item
        ,public pawn::plugin_on_load_i
        ,public pawn::plugin_on_unload_i
        ,public configuradable
    {
    public:
        typedef std::tr1::shared_ptr<hook_events> ptr;
        static ptr instance();

    public:
        hook_events();
        virtual ~hook_events();

    public: // pawn::*
        virtual void plugin_on_load();
        virtual void plugin_on_unload();

    public: // configuradable
        virtual void configure_pre();
        virtual void configure(buffer::ptr const& buff, def_t const& def);
        virtual void configure_post();
    public: // auto_name
        SERIALIZE_MAKE_CLASS_NAME(samp_hook_events);

    public:
        static bool is_hook_network;
        static bool is_disable_brief_description;
        static bool is_process_connection_request;
        static int stress_load_count; // 0, если обычное использование
    private:
        static bool is_hook_network_internal;
    };
} // namespace samp {

#endif // SAMP_HOOK_EVENTS_HPP
