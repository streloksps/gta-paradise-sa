#include "config.hpp"
#include "map_icons.hpp"
#include "core/module_c.hpp"
#include "core/samp/samp_api.hpp"
#include <functional>
#include <algorithm>

REGISTER_IN_APPLICATION(map_icons);

map_icons::ptr map_icons::instance() {
    return application::instance()->get_item<map_icons>();
}

map_icons::map_icons() {
    streamer.delete_item_event = std::tr1::bind(&map_icons::streamer_item_deleter, this, std::tr1::placeholders::_1);
}

map_icons::~map_icons() {
}

void map_icons::configure_pre() {
    buffer.update_begin();
}

void map_icons::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_NAMED_ITEM(buffer.update_get_data(), "map_icons");
}

void map_icons::configure_post() {
    buffer.update_end(std::tr1::bind(&map_icons_streamer_t::item_add, std::tr1::ref(streamer), std::tr1::placeholders::_1), std::tr1::bind(&map_icons_streamer_t::item_delete, std::tr1::ref(streamer), std::tr1::placeholders::_1));
    players_execute_handlers(&map_icons_item::reconfig_done_event);
}

void map_icons::on_connect(player_ptr_t const& player_ptr) {
    map_icons_item::ptr item(new map_icons_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void map_icons::streamer_item_deleter(int key) {
    players_execute_handlers(&map_icons_item::delete_event, key);
}

int map_icons::add(float x, float y, float z, int interior, int world, float radius, int marker_type) {
    if (0.0f == radius) {
        // Если указан нулевой радиус видимости, то иконку не создаем
        return invalid_icon_id;
    }
    return streamer.item_add(map_icon_t(x, y, z, interior, world, radius, marker_type));
}

void map_icons::remove(int id) {
    if (invalid_icon_id != id) {
        streamer.item_delete(id);
    }
}

map_icons_item::map_icons_item(map_icons& manager)
:manager(manager)
,mapicons_in_use(map_icons::max_icons, false)
,checker(10.0f)
{
}

map_icons_item::~map_icons_item() {

}

void map_icons_item::on_timer250() {
    pos3 pos = get_root()->get_item<player_position_item>()->pos_get();
    if (checker(pos)) {
        mapicons_player.update_begin();
        manager.streamer.items_get_by_pos(mapicons_player.update_get_data(), pos, map_icons::max_icons);
        mapicons_player.update_end(std::tr1::bind(&map_icons_item::mapicon_create, this, std::tr1::placeholders::_1), std::tr1::bind(&map_icons_item::mapicon_destroy, this, std::tr1::placeholders::_1));
    }
}

int map_icons_item::mapicon_create(int icon_key) {
    mapicons_in_use_t::iterator it = std::find(mapicons_in_use.begin(), mapicons_in_use.end(), false);
    if (mapicons_in_use.end() == it) {
        assert(false && "все ид заняты - ошибка");
        return map_icons::invalid_icon_id;
    }

    int rezult = it - mapicons_in_use.begin();
    map_icon_t const& icon = manager.streamer.item_get(icon_key);
    
    *it = true;
    samp::api::instance()->set_player_mapicon(get_root()->get_id(), rezult, icon.x, icon.y, icon.z, icon.marker_type, 0);

    return rezult;
}

void map_icons_item::mapicon_destroy(int id) {
    if (map_icons::invalid_icon_id == id) {
        return;
    }
    assert((std::size_t)id < mapicons_in_use.size());

    mapicons_in_use[id] = false;
    samp::api::instance()->remove_player_mapicon(get_root()->get_id(), id);
}

void map_icons_item::delete_event(int icon_key) {
    mapicons_player.delete_obj_if_exist(icon_key, std::tr1::bind(&map_icons_item::mapicon_destroy, this, std::tr1::placeholders::_1));
}

void map_icons_item::reconfig_done_event() {
    checker.reset();
}
