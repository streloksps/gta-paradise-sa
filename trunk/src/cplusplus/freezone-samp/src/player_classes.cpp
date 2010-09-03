#include "config.hpp"
#include "player_classes.hpp"
#include <string>
#include <boost/foreach.hpp>
#include "core/module_c.hpp"

REGISTER_IN_APPLICATION(player_classes);

player_classes::player_classes() {
}

player_classes::~player_classes() {
}

void player_classes::create() {
}

void player_classes::on_connect(player_ptr_t const& player_ptr) {
    player_classes_item::ptr item(new player_classes_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void player_classes::on_gamemode_init(AMX*, samp::server_ver ver) {
    samp::api::ptr api_ptr = samp::api::instance();
    BOOST_FOREACH(player_class_def_t const& def, class_defs) {
        api_ptr->add_player_class(def.skin_id, class_spawn_def_pos.x, class_spawn_def_pos.y, class_spawn_def_pos.z, class_spawn_def_pos.rz, 0, 0, 0, 0, 0, 0);
    }
    api_ptr->use_player_ped_anims();
}

//bool player_classes::on_request_spawn(player::ptr player_ptr) {
//    if (player_free_for_all::ptr ffa = get_root()->get_item<player_free_for_all>()) {
//        player_ptr->activate_context(ffa);
//    }
//
//    return true;
//}

void player_classes::configure_pre() {
    class_spawn_def_pos = pos4(1958.3783f, 1343.1572f, 15.3746f, 0, 0, 270.0f);
    class_select_pos = pos4(1941.2f, 949.5f, 59.5f, 0, 255, 330.0f);
    class_select_camera = pos_camera(1942.4708f, 952.1133f, 59.5812f, 1939.8826f, 942.4541f, 60.2788f);
    class_select_time = player_time(3);
    class_select_weather_id = 10;
    class_defs.clear();
}

void player_classes::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(class_spawn_def_pos);
    SERIALIZE_ITEM(class_select_pos);
    SERIALIZE_ITEM(class_select_camera);
    SERIALIZE_ITEM(class_select_time);
    SERIALIZE_ITEM(class_select_weather_id);
    SERIALIZE_ITEM(class_defs);
}

void player_classes::configure_post() {
    if (class_defs.empty()) {
        // Добавляем хотя бы 1 класс, чтобы самп не сглючило
        class_defs.push_back(player_class_def_t());
    }
}

player_classes_item::player_classes_item(player_classes& manager):manager(manager) {
}

player_classes_item::~player_classes_item() {
}

void player_classes_item::on_connect() {
    //setup_for_select_class();
}

void player_classes_item::on_request_class(int class_id) {
    player::ptr player_ptr = get_root();
    if (class_id >= 0 && static_cast<std::size_t>(class_id) < manager.class_defs.size()) {
        player_class_def = manager.class_defs[class_id];
    }
    else {
        player_class_def = player_class_def_t();
    }

    setup_for_select_class();
}

void player_classes_item::setup_for_select_class() {
    player::ptr player_ptr = get_root();
    player_ptr->get_item<player_position_item>()->pos_set(manager.class_select_pos, manager.class_select_camera);
    player_ptr->time_set(manager.class_select_time);
    player_ptr->weather_set(manager.class_select_weather_id);
}

player_class_def_t const& player_classes_item::get_class_def() const {
    return player_class_def;
}
