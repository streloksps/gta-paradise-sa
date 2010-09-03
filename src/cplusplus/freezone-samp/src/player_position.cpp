#include "config.hpp"
#include "player_position.hpp"
#include "core/module_c.hpp"
#include "core/utility/math.hpp"
#include "core/messages/player_messages/player_messages.hpp"
#include "objects.hpp"

REGISTER_IN_APPLICATION(player_position);

player_position::player_position()
{
}

player_position::~player_position() {
}

void player_position::create() {
    loading.reset(new text_draw::td_item("$(player_position_loading_textdraw)"));
    api_ptr = samp::api::instance();
}

void player_position::configure_pre() {
    preload_min_radius = 150.0f;
    preload_time = 3000;
    preload_camera_shift_x = 0.0f;
    preload_camera_shift_y = 0.0f;
    preload_camera_shift_z = 0.0f;
    preload_camera_shift_look_x = 0.0f;
    preload_camera_shift_look_y = 0.0f;
    preload_camera_shift_look_z = 0.0f;
    preload_camera_shift_distance = -1.0f;
    preload_z = 9000.0f;
}

void player_position::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(preload_min_radius);
    SERIALIZE_ITEM(preload_time);
    SERIALIZE_ITEM(preload_camera_shift_x);
    SERIALIZE_ITEM(preload_camera_shift_y);
    SERIALIZE_ITEM(preload_camera_shift_z);
    SERIALIZE_ITEM(preload_camera_shift_look_x);
    SERIALIZE_ITEM(preload_camera_shift_look_y);
    SERIALIZE_ITEM(preload_camera_shift_look_z);
    SERIALIZE_ITEM(preload_camera_shift_distance);
    SERIALIZE_ITEM(preload_z);
}

void player_position::configure_post() {
    loading->update();
}

void player_position::on_connect(player_ptr_t const& player_ptr) {
    player_position_item::ptr item(new player_position_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

player_position_item::player_position_item(player_position& manager)
:manager(manager)
,is_preload_active(false)
{
}

player_position_item::~player_position_item() {

}

pos4 player_position_item::pos_get() {
    if (is_preload_active) {
        return last_pos;
    }
    pos4 rezult = get_root()->pos_get();
    if (manager.preload_world == rezult.world) {
        return last_pos;
    }
    if (manager.preload_z == rezult.z) {
        // Исправление бага с необновлением координаты сразу после устновки координат
        return last_pos;
    }
    //rezult.world = last_pos.world;
    return rezult;
}


void player_position_item::pos_set_last(pos4 const& pos) {
    last_pos = pos;
    get_root()->get_item<objects_item>()->update_player_pos(pos);
}

void player_position_item::pos_set(pos4 const& pos) {
    pos_set_last(pos);
    pos_set_impl(pos);
}

void player_position_item::pos_set(pos4 const& pos, pos_camera const& pos_cam) {
    pos_set_last(pos);
    pos_set_impl(pos, pos_cam);
}
void player_position_item::pos_set_ex(pos4 const& pos) {
    if (is_points_in_sphere(pos_get(), pos, manager.preload_min_radius)) {
        // Игрок не долеко от старой позиции: можно изменять координату без задержки
        pos_set(pos);
    }
    else {
        pos_set_preload(pos);
    }
}

void player_position_item::world_bounds_set(world_bounds_t const& world_bounds) {
    get_root()->world_bounds_set(world_bounds);
}

void player_position_item::camera_set(pos_camera const& pos_cam) {
    get_root()->camera_set(pos_cam);
}

void player_position_item::pos_set_impl(pos4 const& pos) {
    get_root()->pos_set(pos);
}

void player_position_item::pos_set_impl(pos4 const& pos, pos_camera const& pos_cam) {
    get_root()->pos_set(pos, pos_cam);
}

void player_position_item::pos_set_preload(pos4 const& pos) {
    messages_item msg_item;
    msg_item.get_params()("preload_time", manager.preload_time);
    msg_item.get_sender()("$(player_position_loading_bubble)", msg_player_bubble(get_root()));

    pos_set_last(pos);

    preload_pos = pos;
    preload_pos.world = manager.preload_world;
    preload_pos.z = manager.preload_z;
    preload_camera = get_preload_camera_for_pos(pos);

    pos_set_impl(preload_pos, preload_camera);
    get_root()->freeze();
    manager.loading->show_for_player(get_root());
    preload_end_time = time_base::tick_count_milliseconds() + manager.preload_time;
    is_preload_active = true;
}

void player_position_item::pos_set_preload_end() {
    pos_set_impl(last_pos);
    get_root()->unfreeze();
    manager.loading->hide_for_player(get_root());
    is_preload_active = false;
}

pos_camera player_position_item::get_preload_camera_for_pos(pos4 const& pos) {
    pos_camera rezult;
    rezult.lock_x = pos.x + manager.preload_camera_shift_x;
    rezult.lock_y = pos.y + manager.preload_camera_shift_y;
    rezult.lock_z = pos.z + manager.preload_camera_shift_z;
    rezult.x = pos.x + manager.preload_camera_shift_look_x;
    rezult.y = pos.y + manager.preload_camera_shift_look_y;
    rezult.z = pos.z + manager.preload_camera_shift_look_z;
    get_rotated_xy(rezult.x, rezult.y, manager.preload_camera_shift_distance, pos.rz);
    return rezult;
}

void player_position_item::on_timer500() {
    if (!is_preload_active) {
        return;
    }
    if (time_base::tick_count_milliseconds() >= preload_end_time) {
        pos_set_preload_end();
    }
}

void player_position_item::on_death(player_ptr_t const& killer_ptr, int reason) {
    is_preload_active = false;
}

bool player_position_item::on_update() {
    /*
    int const curr_world = manager.api_ptr->get_player_virtual_world(get_root()->get_id());
    if (is_preload_active && curr_world != manager.preload_world) {
        return false;
    }
    if (curr_world != last_pos.world && curr_world != manager.preload_world) {
        return false;
    }
    */
    return true;
}

bool player_position_item::is_preload() const {
    return is_preload_active;
}

float player_position_item::get_speed(bool is_food) const {
    samp::api::ptr api_ptr = samp::api::instance();
    if (api_ptr->is_has_030_features()) {
        float coff = 162.56f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        if (is_food) {
            api_ptr->get_player_velocity(get_root()->get_id(), x, y, z);
        }
        else {
            int vehicle_id = api_ptr->get_player_vehicle_id(get_root()->get_id());
            if (vehicle_id) {
                api_ptr->get_vehicle_velocity(vehicle_id, x, y, z);
            }
        }
        return std::sqrt(x*x + y*y + z*z) * coff;
    }
    return 0.0f;
}
