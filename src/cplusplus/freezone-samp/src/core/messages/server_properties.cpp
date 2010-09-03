#include "config.hpp"
#include <functional>

#define SERVER_NAME     "[RUS] GTA - Paradise"
#define SERVER_SITE     "www.gta-paradise.ru"
#define SERVER_MODE     "FreeZone"
#define SERVER_MAP      "Russia"
#define SERVER_CRC      0x00000000

#include "server_properties.hpp"
#include "core/application.hpp"
#include "core/samp/samp_api.hpp"
#include "core/ver.hpp"
#include <boost/format.hpp>

REGISTER_IN_APPLICATION(server_properties);

server_properties::ptr server_properties::instance() {
    return application::instance()->get_item<server_properties>();
}

server_properties::server_properties():
    name(SERVER_NAME)
   ,site(SERVER_SITE)
   ,mode(SERVER_MODE)
   ,map(SERVER_MAP)
   ,crc(SERVER_CRC)
   ,mode_suffix(only_ver)
   ,is_mode_loaded(false)
   {
}

server_properties::~server_properties() {
}

void server_properties::on_gamemode_init(AMX* amx, samp::server_ver ver) {
    refresh_server();
    is_mode_loaded = true;
}

void server_properties::create() {

}

void server_properties::configure_pre() {
    name_suffix = "SA";
    address = "$(server_bind_ip)";
    map = SERVER_MAP;
}

void server_properties::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(name_suffix);
    SERIALIZE_ITEM(address);
    SERIALIZE_ITEM(map);
}

void server_properties::configure_post() {
    if (is_mode_loaded) {
        refresh_server();
    }
}

std::string server_properties::get_name() const {
    if (name_suffix.empty()) {
        return name;
    }
    else {
        return name + " " + name_suffix;
    }
}

std::string server_properties::get_name_suffix() const {
    return name_suffix;
}

std::string server_properties::get_site() const {
    return site;
}

std::string server_properties::get_site_full() const {
    return "http://" + site + "/";
}

std::string server_properties::get_mode() const {
    if (mode_suffix.empty()) {
        return mode;
    }
    else {
        return mode + " " + mode_suffix;
    }
}

std::string server_properties::get_map() const {
    return map;
}
int server_properties::get_port() const {
    return samp::api::instance()->get_server_var_as_int("port");
}

std::string server_properties::get_address() const {
    return (boost::format("%1%:%2%") % address % get_port()).str();
}

std::string server_properties::get_mode_name() const {
    return mode;
}

void server_properties::refresh_server() {
    samp::api::ptr api_ptr = this->get_root()->get_item<samp::api>();
    api_ptr->send_rcon_command("hostname " + get_name());
    api_ptr->send_rcon_command("weburl " + get_site());
    api_ptr->set_game_mode_text(get_mode());
    api_ptr->send_rcon_command("mapname " + get_map());
}

void server_properties::on_timer60000() {
    refresh_server();
}
