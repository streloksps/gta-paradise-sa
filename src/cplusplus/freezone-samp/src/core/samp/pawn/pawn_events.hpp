#ifndef PAWN_EVENTS_HPP
#define PAWN_EVENTS_HPP
#include <memory>
#include <vector>
#include "core/container/application_item.hpp"
//#include "core/chronometer.hpp"
#include "core/serialization/configuradable.hpp"
#include "pawn_plugin_i.hpp"
#include "pawn_demarshaling.hpp"

namespace pawn {
    class events
        :public application_item
        ,public configuradable
        ,public pawn::plugin_on_amx_load_i
    {
    public:
        typedef std::tr1::shared_ptr<events> ptr;
        typedef std::tr1::weak_ptr<events> wptr;
        static wptr single;
        static ptr instance();

    public:
        events();
        virtual ~events();

    public: // configuradable
        virtual void configure_pre();
        virtual void configure(buffer::ptr const& buff, def_t const& def);
        virtual void configure_post();
    public: // auto_name
        SERIALIZE_MAKE_CLASS_NAME(pawn_events);

    public: // plugin_on_amx_load_i
        virtual void plugin_on_amx_load(AMX* amx);

    public:
        template<typename rezut_t>
        void add_callback(std::string const& name, rezut_t (*fn)()) {
            add_callback_impl(call_base_ptr(new call_t<rezut_t>(name, fn)));
        }
        template<typename rezut_t, typename arg1_t>
        void add_callback(std::string const& name, rezut_t (*fn)(arg1_t)) {
            add_callback_impl(call_base_ptr(new call_t<rezut_t, arg1_t>(name, fn)));
        }
        template<typename rezut_t, typename arg1_t, typename arg2_t>
        void add_callback(std::string const& name, rezut_t (*fn)(arg1_t, arg2_t)) {
            add_callback_impl(call_base_ptr(new call_t<rezut_t, arg1_t, arg2_t>(name, fn)));
        }
        template<typename rezut_t, typename arg1_t, typename arg2_t, typename arg3_t>
        void add_callback(std::string const& name, rezut_t (*fn)(arg1_t, arg2_t, arg3_t)) {
            add_callback_impl(call_base_ptr(new call_t<rezut_t, arg1_t, arg2_t, arg3_t>(name, fn)));
        }
        template<typename rezut_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
        void add_callback(std::string const& name, rezut_t (*fn)(arg1_t, arg2_t, arg3_t, arg4_t)) {
            add_callback_impl(call_base_ptr(new call_t<rezut_t, arg1_t, arg2_t, arg3_t, arg4_t>(name, fn)));
        }
        template<typename rezut_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
        void add_callback(std::string const& name, rezut_t (*fn)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t)) {
            add_callback_impl(call_base_ptr(new call_t<rezut_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t>(name, fn)));
        }

        
    public: // Реализация
        int callback(int index, AMX* amx, cell* params);

    private:
        struct call_base_t {
            std::string name;
            virtual int process_call(AMX* amx, cell* params) = 0;
            call_base_t(std::string const& name): name(name) {}
        };
        typedef std::tr1::shared_ptr<call_base_t> call_base_ptr;
        typedef std::vector<call_base_ptr> calls_t;

        void add_callback_impl(call_base_ptr call_ptr);
        calls_t calls;

        // Базовая специализация (пустая)
        template<typename rezut_t, typename arg1_t=void, typename arg2_t=void, typename arg3_t=void, typename arg4_t=void, typename arg5_t=void>
        struct call_t {
        };

    };
    

    template<>
    struct events::call_t<void, void, void, void, void, void>: call_base_t {
        typedef void (*fn_t)();
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_last_t<0> param_last_t;
            param_last_t param_last(amx, params);
            fn();
            return 1;
        }
    };
    template<typename arg1_t>
    struct events::call_t<void, arg1_t, void, void, void, void>: call_base_t {
        typedef void (*fn_t)(arg1_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_last_t<param1_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param_last_t param_last(amx, params);
            fn(param1.get());
            return 1;
        }
    };
    template<typename arg1_t, typename arg2_t>
    struct events::call_t<void, arg1_t, arg2_t, void, void, void>: call_base_t {
        typedef void (*fn_t)(arg1_t, arg2_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_t<param1_t::next_param_id, arg2_t> param2_t;
            typedef demarh_last_t<param2_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param2_t param2(amx, params);
            param_last_t param_last(amx, params);
            fn(param1.get(), param2.get());
            return 1;
        }
    };
    template<typename arg1_t, typename arg2_t, typename arg3_t>
    struct events::call_t<void, arg1_t, arg2_t, arg3_t, void, void>: call_base_t {
        typedef void (*fn_t)(arg1_t, arg2_t, arg3_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_t<param1_t::next_param_id, arg2_t> param2_t;
            typedef demarh_t<param2_t::next_param_id, arg3_t> param3_t;
            typedef demarh_last_t<param3_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param2_t param2(amx, params);
            param3_t param3(amx, params);
            param_last_t param_last(amx, params);
            fn(param1.get(), param2.get(), param3.get());
            return 1;
        }
    };
    template<typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
    struct events::call_t<void, arg1_t, arg2_t, arg3_t, arg4_t, void>: call_base_t {
        typedef void (*fn_t)(arg1_t, arg2_t, arg3_t, arg4_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_t<param1_t::next_param_id, arg2_t> param2_t;
            typedef demarh_t<param2_t::next_param_id, arg3_t> param3_t;
            typedef demarh_t<param3_t::next_param_id, arg4_t> param4_t;
            typedef demarh_last_t<param4_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param2_t param2(amx, params);
            param3_t param3(amx, params);
            param4_t param4(amx, params);
            param_last_t param_last(amx, params);
            fn(param1.get(), param2.get(), param3.get(), param4.get());
            return 1;
        }
    };
    template<typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
    struct events::call_t<void, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t>: call_base_t {
        typedef void (*fn_t)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_t<param1_t::next_param_id, arg2_t> param2_t;
            typedef demarh_t<param2_t::next_param_id, arg3_t> param3_t;
            typedef demarh_t<param3_t::next_param_id, arg4_t> param4_t;
            typedef demarh_t<param4_t::next_param_id, arg5_t> param5_t;
            typedef demarh_last_t<param5_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param2_t param2(amx, params);
            param3_t param3(amx, params);
            param4_t param4(amx, params);
            param5_t param5(amx, params);
            param_last_t param_last(amx, params);
            fn(param1.get(), param2.get(), param3.get(), param4.get(), param5.get());
            return 1;
        }
    };
    template<>
    struct events::call_t<bool, void, void, void, void, void>: call_base_t {
        typedef bool (*fn_t)();
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_last_t<1> param_last_t;
            param_last_t param_last(amx, params);
            bool rezult = fn();
            return rezult ? 1 : 0;
        }
    };
    template<typename arg1_t>
    struct events::call_t<bool, arg1_t, void, void, void, void>: call_base_t {
        typedef bool (*fn_t)(arg1_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_last_t<param1_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param_last_t param_last(amx, params);
            bool rezult = fn(param1.get());
            return rezult ? 1 : 0;
        }
    };
    template<typename arg1_t, typename arg2_t>
    struct events::call_t<bool, arg1_t, arg2_t, void, void, void>: call_base_t {
        typedef bool (*fn_t)(arg1_t, arg2_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_t<param1_t::next_param_id, arg2_t> param2_t;
            typedef demarh_last_t<param2_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param2_t param2(amx, params);
            param_last_t param_last(amx, params);
            bool rezult = fn(param1.get(), param2.get());
            return rezult ? 1 : 0;
        }
    };
    template<typename arg1_t, typename arg2_t, typename arg3_t>
    struct events::call_t<bool, arg1_t, arg2_t, arg3_t, void, void>: call_base_t {
        typedef bool (*fn_t)(arg1_t, arg2_t, arg3_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_t<param1_t::next_param_id, arg2_t> param2_t;
            typedef demarh_t<param2_t::next_param_id, arg3_t> param3_t;
            typedef demarh_last_t<param3_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param2_t param2(amx, params);
            param3_t param3(amx, params);
            param_last_t param_last(amx, params);
            bool rezult = fn(param1.get(), param2.get(), param3.get());
            return rezult ? 1 : 0;
        }
    };
    template<typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
    struct events::call_t<bool, arg1_t, arg2_t, arg3_t, arg4_t, void>: call_base_t {
        typedef bool (*fn_t)(arg1_t, arg2_t, arg3_t, arg4_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_t<param1_t::next_param_id, arg2_t> param2_t;
            typedef demarh_t<param2_t::next_param_id, arg3_t> param3_t;
            typedef demarh_t<param3_t::next_param_id, arg4_t> param4_t;
            typedef demarh_last_t<param4_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param2_t param2(amx, params);
            param3_t param3(amx, params);
            param4_t param4(amx, params);
            param_last_t param_last(amx, params);
            bool rezult = fn(param1.get(), param2.get(), param3.get(), param4.get());
            return rezult ? 1 : 0;
        }
    };
    template<typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
    struct events::call_t<bool, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t>: call_base_t {
        typedef bool (*fn_t)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t);
        fn_t fn;
        call_t(std::string const& name, fn_t fn): call_base_t(name), fn(fn) {}
        virtual int process_call(AMX* amx, cell* params) {
            typedef demarh_t<0, arg1_t> param1_t;
            typedef demarh_t<param1_t::next_param_id, arg2_t> param2_t;
            typedef demarh_t<param2_t::next_param_id, arg3_t> param3_t;
            typedef demarh_t<param3_t::next_param_id, arg4_t> param4_t;
            typedef demarh_t<param4_t::next_param_id, arg5_t> param5_t;
            typedef demarh_last_t<param5_t::next_param_id> param_last_t;
            param1_t param1(amx, params);
            param2_t param2(amx, params);
            param3_t param3(amx, params);
            param4_t param4(amx, params);
            param4_t param5(amx, params);
            param_last_t param_last(amx, params);
            bool rezult = fn(param1.get(), param2.get(), param3.get(), param4.get(), param5.get());
            return rezult ? 1 : 0;
        }
    };

} // namespace pawn {
#endif // PAWN_EVENTS_HPP
