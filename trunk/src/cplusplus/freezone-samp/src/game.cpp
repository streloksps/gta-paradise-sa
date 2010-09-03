#include "config.hpp"
#include "game.hpp"
#include "core/module_c.hpp"
#include <functional>
#include <sstream>
#include <vector>
#include <boost/format.hpp>

REGISTER_IN_APPLICATION(game);

game::ptr game::instance() {
    return application::instance()->get_item<game>();
}

game::game(): time_shift(12 * 60) {
}

game::~game() {
}

void game::create() {
    api_ptr = samp::api::instance();
}

void game::configure_pre() {
    disable_interior_enter_exits = false;
    disable_interior_weapons = true;
}

void game::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(disable_interior_enter_exits);
    SERIALIZE_ITEM(disable_interior_weapons);
}

void game::configure_post() {
    if (disable_interior_enter_exits) {
        api_ptr->disable_interior_enter_exits();
    }
    api_ptr->allow_interior_weapons(!disable_interior_weapons);
}

void game::on_gamemode_init(AMX*, samp::server_ver ver) {
    start_time = boost::posix_time::second_clock::local_time();

    set_world_time(get_time());
}

player_time game::get_time() {
    player_time rezult;
    boost::posix_time::time_duration delta = boost::posix_time::second_clock::local_time() - start_time;
    int time = (delta.total_seconds() + time_shift) % (24 * 60);
    rezult.minute = time % 60;
    rezult.hour = time / 60;
    return rezult;
}

int game::get_weather() {
    return 1;
}

float game::get_gravity() {
    return 0.008f;
}

void game::on_timer1000() {
    synchronize_gravity();
    synchronize_time();
    //synchronize_weather();    
}

void game::synchronize_gravity() {
    std::string cur_grav_str = api_ptr->get_server_var_as_string("gravity");
    std::istringstream iss(cur_grav_str);

    float curr_grav = 0.0f;
    iss>>curr_grav;
    if (iss.eof() && !iss.fail()) {
        if (get_gravity() != curr_grav) {
            api_ptr->send_rcon_command((boost::format("gravity %1%") % get_gravity()).str());
        }
    }
    else {
        assert(false);
    }
}

void game::synchronize_weather() {
    // Так не нужно
    int curr_weather = api_ptr->get_server_var_as_int("weather");
    if (get_weather() != curr_weather) {
        api_ptr->set_weather(get_weather());
        // Всем игрокам еще установить
    }
}

void game::synchronize_time() {
    // Синхронизируем время в свойствах сервера
    player_time worldtime = get_time();
    if (worldtime.hour != last_worldtime.hour || (worldtime.minute / 10) != (last_worldtime.minute / 10)) {
        set_world_time(worldtime);
    }
}

void game::set_world_time(player_time const& worldtime) {
    last_worldtime = worldtime;
    api_ptr->send_rcon_command((boost::format("worldtime %1$02d:%2%0") % worldtime.hour % (worldtime.minute/10)).str());
}

void game::on_connect(player_ptr_t const& player_ptr) {
    game_item::ptr item(new game_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

game_item::game_item(game& manager)
:manager(manager)
{
}

game_item::~game_item() {
}

bool game_item::on_update() {
    if (manager.disable_interior_weapons) {
        int player_id = get_root()->get_id();
        if (0 != manager.api_ptr->get_player_interior(player_id)) {
            // Мы находимся в интерьере
            int weapon_id = manager.api_ptr->get_player_weapon(player_id);
            if (samp::api::WEAPON_UNARMED != weapon_id/* || samp::api::WEAPON_BRASSKNUCKLE != weapon_id*/) {
                manager.api_ptr->set_player_armed_weapon(player_id, samp::api::WEAPON_UNARMED);
                return false;
            }
        }
    }
    return true;
}
