#include "config.hpp"
#include <algorithm>
#include <functional>
#include <boost/format.hpp>
#include "player.hpp"
#include "core/players.hpp"
#include "core/application.hpp"
#include "core/geo/geo_data_base.hpp"
#include "core/messages/messages_item.hpp"
#include <limits>

player::player(unsigned int id)
:id(id)
,state(player_not_conected)
,is_blocked(false)
{
    api_ptr = samp::api::instance();
    geo_ip_info = geo::data_base::instance()->get_info(api_ptr->get_player_ip(id));
    serial = api_ptr->get_serial(id);
    //name_init();

    time_connect = std::time(0);
}

player::~player() {
}

void player::on_timer250() {
    game_context_process_wait();
}

unsigned int player::get_id() const {
    return id;
}

geo::ip_info const& player::get_geo() const {
    return geo_ip_info;
}


pos4 player::pos_get() {
    if (block_get()) {
        // Если игрок заблокирован - то даем нулевые координаты
        return pos4();
    }
    float x, y, z;
    api_ptr->get_player_pos(id, x, y, z);
    return pos4(x, y, z, api_ptr->get_player_interior(id), api_ptr->get_player_virtual_world(id), api_ptr->get_player_facing_angle(id));
}

void player::pos_set(pos4 const& pos) {
    api_ptr->set_player_pos(id, pos.x, pos.y, pos.z);
    api_ptr->set_player_interior(id, pos.interior);
    api_ptr->set_player_virtual_world(id, pos.world);
    api_ptr->set_player_facing_angle(id, pos.rz);
    api_ptr->set_camera_behind_player(id);
}

void player::pos_set_only_int_world(pos4 const& pos) {
    api_ptr->set_player_interior(id, pos.interior);
    api_ptr->set_player_virtual_world(id, pos.world);
}

void player::pos_set(pos4 const& pos, pos_camera const& pos_cam) {
    api_ptr->set_player_pos(id, pos.x, pos.y, pos.z);
    api_ptr->set_player_interior(id, pos.interior);
    api_ptr->set_player_virtual_world(id, pos.world);
    api_ptr->set_player_facing_angle(id, pos.rz);
    api_ptr->set_player_camera_pos(id, pos_cam.x, pos_cam.y, pos_cam.z);
    api_ptr->set_player_camera_look_at(id, pos_cam.lock_x, pos_cam.lock_y, pos_cam.lock_z);
}

void player::world_bounds_set(world_bounds_t const& world_bounds) {
    api_ptr->set_player_world_bounds(id, world_bounds.x_max, world_bounds.x_min, world_bounds.y_max, world_bounds.y_min);
}

void player::camera_set(pos_camera const& pos_cam) {
    api_ptr->set_player_camera_pos(id, pos_cam.x, pos_cam.y, pos_cam.z);
    api_ptr->set_player_camera_look_at(id, pos_cam.lock_x, pos_cam.lock_y, pos_cam.lock_z);
}

void player::freeze() {
    api_ptr->toggle_player_controllable(id, false);
}

void player::unfreeze() {
    api_ptr->toggle_player_controllable(id, true);
}

void player::time_set(player_time const& time) {
    api_ptr->set_player_time(id, time.hour, time.minute);
}

void player::weather_set(int weather_id) {
    api_ptr->set_player_weather(id, weather_id);
}

game_context::ptr player::game_context_activate_impl(game_context::ptr const& game_context_ptr) {
    assert(game_context_ptr && "Новый контекст не валиден");

    if (!game_context_ptr || game_context_active_ptr == game_context_ptr) {
        return game_context_active_ptr;
    }

    game_context::ptr rezult;
    if (game_context_active_ptr) {
        game_context_active_ptr->on_gc_fini();
        game_context_active_ptr->gc_is_active = false;
        rezult = game_context_active_ptr;
    }
    game_context_ptr->gc_is_active = true;
    game_context_ptr->on_gc_init();
    game_context_active_ptr = game_context_ptr;
    return rezult;
}

game_context::ptr player::game_context_activate(game_context::ptr game_context_ptr) {
    game_context_process_wait();
    return game_context_activate_impl(game_context_ptr);
}

void player::game_context_activate_async(game_context::ptr game_context_ptr) {
    assert(game_context_ptr && "Новый контекст не валиден");
    game_context_process_wait();
    game_context_wait_active_ptr = game_context_ptr;
}

void player::game_context_process_wait() {
    if (game_context_wait_active_ptr) {
        game_context_activate_impl(game_context_wait_active_ptr);
        game_context_wait_active_ptr.reset();
    }
}

void player::weapon_reset() {
    api_ptr->reset_player_weapons(id);
}

void player::weapon_add(weapon_item_t const& weapon) {
    api_ptr->give_player_weapon(id, weapon.id, weapon.ammo);
}

enum {weapon_slot_count = 13};
weapons_t player::weapon_get() {
    weapons_t rezult;
    rezult.active_id = api_ptr->get_player_weapon(id);
    for (int i = 0; i < weapon_slot_count; ++i) {
        int weapon_id, weapoin_ammo;
        api_ptr->get_player_weapon_data(id, i, weapon_id, weapoin_ammo);
        if (0 != i && 0 == weapoin_ammo) {
            // Если нет патронов не в первом слоте - то оружия нет
            continue;
        }
        rezult.items.push_back(weapon_item_t(weapon_id, weapoin_ammo));
    }
    return rezult;
}

weapon_item_t player::weapon_get_current() {
    int weapon_id = api_ptr->get_player_weapon(id);
    return weapon_item_t(weapon_id);
}

/************************************************************************/
/* Работа с именем                                                      */
/************************************************************************/

void player::name_reset() {
    name_set(name);
}

void player::name_set(std::string const& new_name) {
    if (new_name != name_last) {
        name_last = new_name;
        api_ptr->set_player_name(id, new_name);
    }
}

std::string player::name_get() const {
    return name;
}

std::string player::name_normalized_get() const {
    return name_normalized;
}

void player::name_set_for_chat() {
    name_set(name_for_chat);
}

void player::name_set_for_chat_close() {
    name_set(name_for_chat_close);
}

std::string player::name_get_full() const {
    return name_full;
}
/*
std::string player::name_get_full_admin() const {
    return (boost::format("%1%~{none}~[%2%]") % name % id).str();
}*/

std::string player::group_get_name() const {
    bool is_admin = api_ptr->is_player_admin(id);
    if (is_admin) {
        return "admin";
    }
    else {
        return "user";
    }
}

bool player::group_is_in(std::string const& group_name) const {
    return group_get_name() == group_name;
}

std::string player::geo_get_long_desc() const {
    return geo_ip_info.get_long_desc();
}

void player::set_color(unsigned int color) {
    api_ptr->set_player_color(id, color);
}

int player::time_connection_get() {
    std::time_t diff = std::time(0) - time_connect;
    return (int)(diff);
}

void player::on_spawn() {
    state = player_in_game;
}

void player::on_health_suicide(int reason, bool is_by_script) {
    state = player_not_spawn;
}

void player::on_health_kill(player_ptr_t const& killer_ptr, int reason) {
    state = player_not_spawn;
}

player_state player::get_state() const {
    return state;
}

void player::kick() {
    api_ptr->kick(id);
}

void player::ban(std::string const& comment) {
    api_ptr->ban_ex(id, comment);
}

void player::send_client_message(unsigned color, std::string const& message) {
    api_ptr->send_client_message(id, color, message);
}

void player::set_chat_bubble(unsigned int color, float draw_distance, int expire_time, std::string const& text) {
    if (api_ptr->is_has_030_features()) {
        api_ptr->set_player_chat_bubble(id, text, (color << 8) | 0xFF, draw_distance, expire_time);
    }
}

void player::send_death_message(ptr const& killer_ptr, ptr const& killed_ptr, int weapon_id, bool is_process_names) {
    if (is_process_names) {
        killer_ptr->name_set_for_chat();
        killed_ptr->name_set_for_chat();
    }
    samp::api::instance()->send_death_message(killer_ptr->id, killed_ptr->id, weapon_id);
    if (is_process_names) {
        killed_ptr->name_reset();
        killer_ptr->name_reset();
    }
}

void player::send_death_message(ptr const& killed_ptr, int weapon_id, bool is_process_names) {
    samp::api::ptr api_ptr = samp::api::instance();
    if (is_process_names) killed_ptr->name_set_for_chat();
    api_ptr->send_death_message(api_ptr->get_invalid_player_id(), killed_ptr->id, weapon_id);
    if (is_process_names) killed_ptr->name_reset();
}

void player::send_player_message(ptr const& sender_ptr, std::string const& message) {
    api_ptr->send_player_message_to_player(id, sender_ptr->id, message);
}

int player::get_score() {
    return api_ptr->get_player_score(id);
}

void player::set_score(int score) {
    api_ptr->set_player_score(id, score);
}

bool player::spectate_is_active() {
    return samp::api::PLAYER_STATE_SPECTATING == api_ptr->get_player_state(id);
}

void player::spectate_do(player_ptr_t const& spec_player_ptr) {
    api_ptr->toggle_player_spectating(id, true);
    api_ptr->set_player_interior(id, api_ptr->get_player_interior(spec_player_ptr->get_id()));
    api_ptr->set_player_virtual_world(id, api_ptr->get_player_virtual_world(spec_player_ptr->get_id()));
    if (int vehicle_id = api_ptr->get_player_vehicle_id(spec_player_ptr->get_id())) {
        // Игрок в авто
        api_ptr->player_spectate_vehicle(id, vehicle_id);
    }
    else {
        api_ptr->player_spectate_player(id, spec_player_ptr->get_id());
    }
}

void player::spectate_do() {
    api_ptr->toggle_player_spectating(id, true);
}

void player::spectate_stop() {
    api_ptr->toggle_player_spectating(id, false);
}

void player::health_set(player_health_armour const& health) {
    api_ptr->set_player_health(id, health.health);
    api_ptr->set_player_armour(id, health.armour);
}

void player::health_set_inf() {
//    api_ptr->set_player_health(id, std::numeric_limits<float>::infinity());
    api_ptr->set_player_health(id, std::numeric_limits<float>::max());
}

player_health_armour player::health_get() {
	return player_health_armour(api_ptr->get_player_health(id), api_ptr->get_player_armour(id));
}

void player::kill() {
    // Устанавливаем жизнь и броньку в ноль
    health_set(player_health_armour());
}

bool player::vehicle_is_in() const {
    return 0 != api_ptr->get_player_vehicle_id(id);
}

bool player::vehicle_is_driver() const {
    return samp::api::PLAYER_STATE_DRIVER == api_ptr->get_player_state(id);
}

bool player::animation_is_use_phone() const {
    return samp::api::SPECIAL_ACTION_USECELLPHONE == api_ptr->get_player_special_action(id);
}

bool player::is_rcon_admin() const {
    return api_ptr->is_player_admin(id);
}

void player::special_action_set(samp::api::special_action action_id) {
    api_ptr->set_player_special_action(id, action_id);
}

samp::api::special_action player::special_action_get() const {
    return api_ptr->get_player_special_action(id);
}

bool player::get_vehicle(int& vehicle_id, int& model_id, bool& is_driver) {
    int local_vehicle_id = api_ptr->get_player_vehicle_id(id);
    int local_model_id = api_ptr->get_vehicle_model(local_vehicle_id);
    if (0 != local_model_id) {
        vehicle_id = local_vehicle_id;
        model_id = local_model_id;
        is_driver = vehicle_is_driver();
        return true;
    }
    return false;
}

bool player::block_get() const {
    return is_blocked;
}

void player::block(std::string const& reason_group, std::string const& reason_info) {
    if (block_get()) {
        assert(false);
        return;
    }
    is_blocked = true;
    messages_item msg_item;
    msg_item.get_params()
        ("player_name", name_get_full())
        ("player_geo_long_desc", geo_get_long_desc())
        ("player_serial", get_serial())
        ("reason_group", reason_group)
        ("reason_info", reason_info)
        ;
    if (reason_info.empty()) {
        msg_item.get_sender()
            ("$(player_block_log)", msg_log)
            //("$(player_block_admins)", msg_players_all_admins)
            ;
    }
    else {
        msg_item.get_sender()
            ("$(player_block_info_log)", msg_log)
            //("$(player_block_info_admins)", msg_players_all_admins)
            ;
    }
}

void player::on_player_stream_in(player_ptr_t const& player_ptr) {
    stream_in_players.insert(player_ptr);
}

void player::on_player_stream_out(player_ptr_t const& player_ptr) {
    stream_in_players.erase(player_ptr);
}

void player::on_disconnect(samp::player_disconnect_reason reason) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        item_player_ptr->stream_in_players.erase(shared_from_this());
    }
}

player::ptr player::shared_from_this() {
    return std::tr1::dynamic_pointer_cast<player>(container::shared_from_this());
}

std::string const& player::get_serial() const {
    return serial;
}
