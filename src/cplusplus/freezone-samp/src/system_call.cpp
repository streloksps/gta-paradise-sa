#include "config.hpp"
#include "system_call.hpp"
#include <boost/filesystem.hpp>
#include <cstdlib>

system_call_t::system_call_t(std::string const& config_name)
:config_name(config_name)
,enable(false)
{

}

system_call_t::~system_call_t() {

}

void system_call_t::configure_pre() {
    enable = false;
    enable_if_file_exist.clear();
    command.clear();
}

void system_call_t::configure(buffer::ptr const& buff, def_t const& def) {
    configure_pre();
    SERIALIZE_ITEM(enable);
    SERIALIZE_ITEM(enable_if_file_exist);
    SERIALIZE_ITEM(command);
    configure_post();
}

void system_call_t::configure_post() {

}

char const* system_call_t::get_auto_name() const {
    return config_name.c_str();
}

int system_call_t::call(messages_params const& params) {
    if (!enable) {
        return -1;
    }
    if (!enable_if_file_exist.empty()) { // Проверяем на существования указанный файл
        if (!boost::filesystem::exists(params.process_all_vars(enable_if_file_exist))) {
            return -2;
        }
    }
    std::string cmd = params.process_all_vars(command);
    return std::system(cmd.c_str());
}

system_call_t::operator bool() const {
    return enable;
}
