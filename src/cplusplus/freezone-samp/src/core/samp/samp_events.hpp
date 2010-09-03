#ifndef SAMP_EVENTS_HPP
#define SAMP_EVENTS_HPP
#include <memory>
#include "core/container/application_item.hpp"
#include "core/serialization/configuradable.hpp"
#include "pawn/pawn_plugin_i.hpp"

namespace samp {
    class events
        :public application_item
        ,public configuradable
        ,public pawn::plugin_on_load_i
    {
    public:
        typedef std::tr1::shared_ptr<events> ptr;
        static ptr instance();

    public:
        events();
        virtual ~events();

    public: // configuradable
        virtual void configure_pre();
        virtual void configure(buffer::ptr const& buff, def_t const& def);
        virtual void configure_post();
    public: // auto_name
        SERIALIZE_MAKE_CLASS_NAME(samp_events);

    public: // plugin_on_load_i
        virtual void plugin_on_load();
    
    };
} // namespace samp {
#endif // SAMP_EVENTS_HPP
