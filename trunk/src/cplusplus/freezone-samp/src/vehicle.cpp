#include "config.hpp"
#include "vehicle.hpp"
#include "core/module_c.hpp"
#include "vehicles.hpp"

vehicle_t::vehicle_t(vehicles& manager, pos_vehicle const& pos)
:manager(manager)
,pos_spawn(pos)
,id(0)
,paintjob_id(paintjob_default)
,color1(0)
,color2(0)
,pos_interior(0)
,pos_world(0)
,is_static(false)
{
}

bool vehicle_t::create() {
    pos_vehicle pos = pos_spawn;
    create_impl_pre(pos);
    id = samp::api::instance()->create_vehicle(pos_spawn.model_id, pos_spawn.x, pos_spawn.y, pos_spawn.z, pos_spawn.rz, pos.color1, pos.color2, pos_spawn.respawn_delay);
    return create_impl_post(pos);
}

bool vehicle_t::create_static() {
    pos_vehicle pos = pos_spawn;
    create_impl_pre(pos);
    id = samp::api::instance()->add_static_vehicle_ex(pos_spawn.model_id, pos_spawn.x, pos_spawn.y, pos_spawn.z, pos_spawn.rz, pos.color1, pos.color2, pos_spawn.respawn_delay);
    return create_impl_post(pos);
}

void vehicle_t::create_impl_pre(pos_vehicle& pos) const {
    if (vehicle_def_cptr_t def_cptr = get_def()) {
        if (is_need_change_color() && !def_cptr->color_items.empty()) {
            // Пытаемся найти цвета - если нашли, то ставим наш
            vehicle_def_t::color_item_t const& color_item = def_cptr->color_items[std::rand() % def_cptr->color_items.size()];
            if (any_color == pos.color1) pos.color1 = color_item.color1;
            if (any_color == pos.color2) pos.color2 = color_item.color2;
        }
    }
}

bool vehicle_t::create_impl_post(pos_vehicle& pos) {
    if (!is_valid_id(id)) { // Не удалось создать транспорт
        assert(false);
        return false;
    }
    samp::api::ptr api_ptr = samp::api::instance();

    color1 = pos.color1;
    color2 = pos.color2;
    pos_interior = pos.interior;
    pos_world = pos.world;
    api_ptr->link_vehicle_to_interior(id, pos_interior);
    api_ptr->set_vehicle_virtual_world(id, pos_world);

    return true;
}

int vehicle_t::get_id() const {
    return id;
}

bool vehicle_t::get_is_static() const {
    return is_static;
}

bool vehicle_t::is_need_change_color() const {
    return any_color == pos_spawn.color1 || any_color == pos_spawn.color2;
}

void vehicle_t::clear() {
    mods.clear();
    paintjob_id = paintjob_default;
}

pos4 vehicle_t::pos_get() {
    float x, y, z;
    samp::api::ptr api_ptr = samp::api::instance();
    api_ptr->get_vehicle_pos(id, x, y, z);
    return pos4(x, y, z, pos_interior, pos_world, api_ptr->get_vehicle_zangle(id));
}

void vehicle_t::pos_set(pos4 const& pos) {
    samp::api::ptr api_ptr = samp::api::instance();
    api_ptr->set_vehicle_pos(id, pos.x, pos.y, pos.z);
    api_ptr->link_vehicle_to_interior(id, pos.interior);
    api_ptr->set_vehicle_virtual_world(id, pos.world);
    api_ptr->set_vehicle_zangle(id, pos.rz);
    pos_interior = pos.interior;
    pos_world = pos.world;
}

bool vehicle_t::is_valid_id(int id) {
    if (65535 == id) {
        return false;
    }
    if (0 == id) {
        return false;
    }
    return true;
}

int vehicle_t::get_model() const {
    return pos_spawn.model_id;
}

vehicle_def_cptr_t vehicle_t::get_def() const {
    vehicles::vehicle_defs_t::const_iterator def_it = manager.vehicle_defs.find(get_model());
    if (manager.vehicle_defs.end() != def_it) {
        return &(def_it->second);
    }
    return 0;    
}

bool vehicle_t::is_valid_component(int component_id) const {
    return manager.is_valid_component(get_model(), component_id);
}

bool vehicle_t::is_valid_component(std::string const& component_name) const {
    return manager.is_valid_component(get_model(), component_name);
}

void vehicle_t::add_component(vehicle_component_def_t const& component_def) {
    manager.add_component(component_def, id);
}

void vehicle_t::add_component(std::string const& component_name) {
    vehicles::vehicle_component_defs_t::const_iterator comp_it = manager.vehicle_component_defs.find(component_name);
    if (manager.vehicle_component_defs.end() != comp_it) {
        add_component(comp_it->second);
    }
}

void vehicle_t::remove_component(vehicle_component_def_t const& component_def) {
    manager.remove_component(component_def, id);
}

void vehicle_t::remove_component(std::string const& component_name) {
    vehicles::vehicle_component_defs_t::const_iterator comp_it = manager.vehicle_component_defs.find(component_name);
    if (manager.vehicle_component_defs.end() != comp_it) {
        remove_component(comp_it->second);
    }
}
