#include "config.hpp"
#include "application.hpp"
#include <cstdlib>
#include <ctime>
#include <functional>

#include "createble_i.hpp"
#include "container/container_handlers.hpp"
#include "core/logger/logger.hpp"
#include "core/debug.h"
#include "samp/samp_api.hpp"
#include "server_configuration.hpp"

application::ptr application::instance() {
    static application::ptr application_ptr(new application());
    return application_ptr;
}

application::application() {
    // »циализируем генератор случайных чисел
    std::srand((unsigned)std::time(0));
    debug_init();
}

application::~application() {
}

void application::plugin_on_pre_load() {
}

void application::on_pre_gamemode_init(AMX* amx, samp::server_ver ver) {
    logger::ptr log_ptr = logger::instance();
    
    log_ptr->debug("application/init/create", "begin");
    container_execute_handlers(shared_from_this(), &createble_i::create);
    log_ptr->debug("application/init/create", "done");
    
    log_ptr->debug("application/init/configure", "begin");
    server_configuration::instance()->reconfigure();
    log_ptr->debug("application/init/configure", "done");
    
    log_ptr->log("application/init", "done");
}

std::string application::get_server_version() const {
    samp::server_ver ver = samp::api::instance()->get_ver();
    if (samp::server_ver_022 == ver) return "0.2.2";
    if (samp::server_ver_02X == ver) return "0.2X";
    if (samp::server_ver_03a == ver) return "0.3a";
    if (samp::server_ver_03b == ver) return "0.3b";
    return "unknown";
}
