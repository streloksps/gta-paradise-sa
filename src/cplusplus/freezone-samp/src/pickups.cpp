#include "config.hpp"
#include "pickups.hpp"
#include "core/module_c.hpp"
#include "core/samp/samp_api.hpp"
#include <functional>

REGISTER_IN_APPLICATION(pickups);

pickups::ptr pickups::instance() {
    return application::instance()->get_item<pickups>();
}

pickups::pickups() {
    streamer.delete_item_event = std::tr1::bind(&pickups::pickup_delete_event, this, std::tr1::placeholders::_1);
}

pickups::~pickups() {
}

void pickups::configure_pre() {
    buffer.update_begin();
}

void pickups::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_NAMED_ITEM(buffer.update_get_data(), "pickups");
}

void pickups::configure_post() {
    buffer.update_end(std::tr1::bind(&pickups_streamer_t::item_add, std::tr1::ref(streamer), std::tr1::placeholders::_1), std::tr1::bind(&pickups_streamer_t::item_delete, std::tr1::ref(streamer), std::tr1::placeholders::_1));
}

void pickups::on_connect(player_ptr_t const& player_ptr) {

}

void pickups::on_timer500() {
    // Получаем координаты всех игроков
    std::vector<pos3> players_poses;
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        players_poses.push_back(item_player_ptr->get_item<player_position_item>()->pos_get());
    }

    // Получаем список ближайших пикапов
    work_pickups.update_begin();
    streamer.items_get_by_poses(work_pickups.update_get_data(), players_poses, pickup_only_coords_less(), max_pickups);
    work_pickups.update_end(std::tr1::bind(&pickups::pickup_create, this, std::tr1::placeholders::_1), std::tr1::bind(&pickups::pickup_destroy, this, std::tr1::placeholders::_1));
}

int pickups::pickup_create(int pickup_key) {
    pickup_t const& pickup = streamer.item_get(pickup_key);
    return samp::api::instance()->create_pickup(pickup.model, pickup.type, pickup.x, pickup.y, pickup.z, pickup.world);
}

void pickups::pickup_destroy(int id) {
    samp::api::instance()->destroy_pickup(id);
}

void pickups::pickup_delete_event(int pickup_key) {
    work_pickups.delete_obj_if_exist(pickup_key, std::tr1::bind(&pickups::pickup_destroy, this, std::tr1::placeholders::_1));
}
