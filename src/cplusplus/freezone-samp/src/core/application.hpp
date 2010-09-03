#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <string>
#include "container/container.hpp"
#include "container/application_item.hpp"
#include "samp/pawn/pawn_plugin_i.hpp"
#include "samp/samp_events_i.hpp"

class application
    :public container
    ,public pawn::plugin_on_pre_load_i
    ,public samp::on_pre_gamemode_init_i
{
public:
    typedef std::tr1::shared_ptr<application> ptr;
    static ptr instance();

public:
    application();
    virtual ~application();

public: // plugin_on_pre_load_i
    virtual void plugin_on_pre_load();

public: // samp::on_pre_gamemode_init_i
    virtual void on_pre_gamemode_init(AMX* amx, samp::server_ver ver);

public: // Служебное
    std::string get_server_version() const;
};

//Регистрация в приложении
#define REGISTER_IN_APPLICATION(CLASS_NAME) \
namespace { \
    bool register_item() { \
        application_item::ptr item(new CLASS_NAME, &application_item::do_delete); \
        application::instance()->add_item(item); \
        return true; \
    } \
    bool is_registred = register_item(); \
}

#endif // APPLICATION_HPP
