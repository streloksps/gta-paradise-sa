#include "config.hpp"
#include "objects.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include "core/module_c.hpp"
#include "core/samp/samp_api.hpp"
#include "mta10_loader.hpp"
#include "ipl_loader.hpp"

#include "server_paths.hpp"

REGISTER_IN_APPLICATION(objects);

objects::objects(): max_objects(0), max_objects_dynamic(0), is_debug(false) {
    streamer.delete_item_event = std::tr1::bind(&objects::streamer_item_deleter, this, std::tr1::placeholders::_1);
}

objects::~objects() {
}

void objects::configure_pre() {
    objects_static.update_begin();
    objects_dynamic.update_begin();
    object_def_default = object_def_t(500.0f, 0.0f);
    object_defs.clear();
    is_debug = false;
    is_debug_config = false;
    max_objects = 150;
}

void objects::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_NAMED_ITEM(objects_static.update_get_data(), "static_object");
    SERIALIZE_NAMED_ITEM(objects_dynamic.update_get_data(), "dynamic_object");
    SERIALIZE_ITEM(object_def_default);
    SERIALIZE_ITEM(object_defs);
    SERIALIZE_ITEM(is_debug);
    SERIALIZE_ITEM(is_debug_config);
    SERIALIZE_ITEM(max_objects);


    { // Загружаем объекты из файлов MTA-SA 1.0
        mta10_loader_items_t loader_mta10;
        mta10_loader_items_t loader_mta10_static;
        SERIALIZE_ITEM(loader_mta10);
        SERIALIZE_ITEM(loader_mta10_static);
        mta10_loader loader(PATH_MTA10_DIR);
        loader.load_items(loader_mta10, objects_dynamic.update_get_data());
        //loader.load_items(loader_mta10_static, objects_static.update_get_data());
    }

    { // Загружаем объекты из файлов GTA ipl
        bool loader_ipl_dump = false;
        ipl_loader_items_t loader_ipl;
        ipl_loader_items_t loader_ipl_static;
        SERIALIZE_ITEM(loader_ipl);
        SERIALIZE_ITEM(loader_ipl_static);
        SERIALIZE_ITEM(loader_ipl_dump);
        ipl_loader loader(PATH_IPL_DIR, loader_ipl_dump);
        loader.load_items(loader_ipl, objects_dynamic.update_get_data());
        loader.load_items(loader_ipl_static, objects_static.update_get_data());
    }
}

void objects::configure_post() {
    if (max_objects < 0) {
        max_objects = 0;
    }

    // Работа со статическими объектами
    if (objects_static.update_get_data().size() > static_cast<std::size_t>(max_objects)) {
        // Отсекаем лишние объекты
        objects_static_t::update_t::iterator it = objects_static.update_get_data().begin();
        for (int i = 0; i <= max_objects; ++i) ++it;
        objects_static.update_get_data().erase(it, objects_static.update_get_data().end());        
    }
    objects_static.update_end(std::tr1::bind(&objects::static_object_create, this, std::tr1::placeholders::_1), std::tr1::bind(&objects::static_object_destroy, this, std::tr1::placeholders::_1));
    max_objects_dynamic = max_objects - objects_static.update_get_data().size();

    // Работа с динамическими объектами
    {  // Заполняем определения объектов
        objects_dynamic_t::update_t buff;
        std::transform(objects_dynamic.update_get_data().begin(), objects_dynamic.update_get_data().end(), std::inserter(buff, buff.begin()), std::tr1::bind(&objects::fill_defs, this, std::tr1::placeholders::_1));
        objects_dynamic.update_get_data().swap(buff);
    }
    objects_dynamic.update_end(std::tr1::bind(&objects_streamer_t::item_add, std::tr1::ref(streamer), std::tr1::placeholders::_1), std::tr1::bind(&objects_streamer_t::item_delete, std::tr1::ref(streamer), std::tr1::placeholders::_1));

    players_execute_handlers(&objects_item::reconfig_done_event);

    if (is_debug_config) {
        messages_item msg_item;
        msg_item.get_params()
            ("static_count", objects_static.get_size())
            ("dynamic_count", objects_dynamic.get_size());
        msg_item.get_sender()("<log_section objects/debug/configdone/>static=$(static_count), dynamic=$(dynamic_count)", msg_debug);
    }
}

void objects::on_gamemode_init(AMX* amx, samp::server_ver ver) {

}

void objects::on_connect(player_ptr_t const& player_ptr) {
    objects_item::ptr item(new objects_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

int objects::static_object_create(object_static_t const& object_static) {
    return samp::api::instance()->create_object(object_static.model_id, object_static.x, object_static.y, object_static.z, object_static.rx, object_static.ry, object_static.rz, 250.0f);
}

void objects::static_object_destroy(int id) {
    samp::api::instance()->destroy_object(id);
}

object_dynamic_t objects::fill_defs(object_dynamic_t const& obj_src) const {
    object_dynamic_t obj(obj_src);
    object_defs_t::const_iterator it = object_defs.find(obj.model_id);
    if (object_defs.end() == it) {
        // Не нашли в мапе кастомных описаний - берем стандартные
        obj.visibility = object_def_default.visibility;
        obj.radius = object_def_default.radius;
    }
    else {
        // Нашли и их используем
        obj.visibility = it->second.visibility;
        obj.radius = it->second.radius;
    }
    return obj;
}

void objects::streamer_item_deleter(int key) {
    players_execute_handlers(&objects_item::delete_event, key);
}

int objects::dynamic_object_create(object_dynamic_t const& object_dynamic) {
    return streamer.item_add(fill_defs(object_dynamic));
}

void objects::dynamic_object_destroy(int id) {
    streamer.item_delete(id);
}

objects_item::objects_item(objects& manager)
:manager(manager)
,samp_api(samp::api::instance()) 
,checker(5.0f)
{
}

objects_item::~objects_item() {
}

void objects_item::on_timer500() {
    pos4 pos = get_root()->get_item<player_position_item>()->pos_get();
    if (manager.is_debug) {
        // Если отладка, то отключаем оптимизацию детектора перемещения
        checker.reset();
    }
    update_player_pos(pos);
}

void objects_item::update_player_pos(pos3 const& pos) {
    if (checker(pos)) {
        objects_player.update_begin();
        manager.streamer.items_get_by_pos(objects_player.update_get_data(), pos, manager.max_objects_dynamic);
        objects_player.update_end(std::tr1::bind(&objects_item::object_create, this, std::tr1::placeholders::_1), std::tr1::bind(&objects_item::object_destroy, this, std::tr1::placeholders::_1));
    }
}

int objects_item::object_create(int object_key) {
    object_dynamic_t const& obj = manager.streamer.item_get(object_key);
    int id = samp_api->create_player_object(get_root()->get_id(), obj.model_id, obj.x, obj.y, obj.z, obj.rx, obj.ry, obj.rz, obj.visibility);
    if (manager.is_debug) {
        messages_item msg_item;
        msg_item.get_params()
            ("obj", boost::format("%1%") % obj)
            ("id", id)
            ("player_name", get_root()->name_get_full());
        msg_item.get_sender()("<log_section objects/debug/player_create/>$(player_name) $(id) $(obj)", msg_debug);
    }
    return id;
}

void objects_item::object_destroy(int id) {
    samp_api->destroy_player_object(get_root()->get_id(), id);
    if (manager.is_debug) {
        messages_item msg_item;
        msg_item.get_params()
            ("id", id)
            ("player_name", get_root()->name_get_full());
        msg_item.get_sender()("<log_section objects/debug/player_destroy/>$(player_name) $(id)", msg_debug);
    }
}

void objects_item::delete_event(int object_key) {
    objects_player.delete_obj_if_exist(object_key, std::tr1::bind(&objects_item::object_destroy, this, std::tr1::placeholders::_1));
}

void objects_item::reconfig_done_event() {
    checker.reset();
}
