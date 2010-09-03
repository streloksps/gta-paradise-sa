#include "config.hpp"
#include "npc_taxi.hpp"
#include "core/module_c.hpp"
#include "core/npcs.hpp"
#include "vehicles.hpp"
#include "core/npcs.hpp"
#include "core/utility/math.hpp"
#include "checkpoints.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdlib>
#include <functional>
#include <algorithm>

REGISTER_IN_APPLICATION(npc_taxi);

npc_taxi::npc_taxi()
:route_last_id(0)
,route_npc_name_last_id(0)
{
}

npc_taxi::~npc_taxi() {

}

void npc_taxi::configure_pre() {
    routes.update_begin();
    vehicle_stops.clear();
    is_nitro_active = true;
    is_active = false;
}

void npc_taxi::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(is_active);

    if (is_active) {
        SERIALIZE_NAMED_ITEM(routes.update_get_data(), "routes");
        SERIALIZE_ITEM(vehicle_stops);
        SERIALIZE_ITEM(is_nitro_active);
    }
}

void npc_taxi::configure_post() {
    update_stops(vehicle_stops, routes);
    vehicle_stops.clear();

    routes.update_end(std::tr1::bind(&npc_taxi::route_create, this, std::tr1::placeholders::_1), std::tr1::bind(&npc_taxi::route_destroy, this, std::tr1::placeholders::_1));
}

void npc_taxi::on_pre_pre_gamemode_init(AMX*, samp::server_ver ver) {
    npc_taxi_route_section_t::truncate_rec_dir();
}

void npc_taxi::on_post_post_gamemode_exit(AMX* amx) {
    npc_taxi_route_section_t::truncate_rec_dir();
}

void npc_taxi::on_npc_connect(npc_ptr_t const& npc_ptr) {
    std::string name = npc_ptr->name_get();
    int route_npc_name_id = -1;
    if (parse_npc_name(name, route_npc_name_id)) {
        int route_id;
        int route_npc_id;

        if (get_npc_route_and_item_ids(route_npc_name_id, route_id, route_npc_id)) {
            // Подключился бот таксист
            npc_taxi_item::ptr item(new npc_taxi_item(*this, route_id, route_npc_id), &npc_item::do_delete);
            npc_ptr->add_item(item);

            route_datas_t::iterator route_datas_it = route_datas.find(route_id);
            if (route_datas.end() != route_datas_it) {
                route_datas_it->second.route_npcs[route_npc_id].npc_ptr = npc_ptr;
            }
        }
    }
}

void npc_taxi::on_npc_disconnect(npc_ptr_t const& npc_ptr, samp::player_disconnect_reason reason) {
    BOOST_FOREACH(route_datas_t::value_type& route_pair, route_datas) {
        BOOST_FOREACH(route_npc_t& route_npc, route_pair.second.route_npcs) {
            if (route_npc.npc_ptr == npc_ptr) {
                // Отключается бот, который находиться в активном маршруте
                route_npc.npc_ptr.reset();
                --route_pair.second.ready_count;

                if (route_npc.vehicle_id) {
                    // Удаляем машинку, отвалившегося бота
                    vehicles::instance()->vehicle_destroy(route_npc.vehicle_id);
                    route_npc.vehicle_id = 0;
                }

                messages_item msg_item;
                msg_item.get_params()
                    ("npc_name", npc_ptr->name_get())
                    ("reason", static_cast<int>(reason))
                    ("route_sys_name", route_pair.second.route.sys_name)
                    ;
                msg_item.get_sender()("$(npc_taxi_disconnect_inuse_log)", msg_log);
            }
        }
    }
}

// Создаем маршрут
int npc_taxi::route_create(npc_taxi_route_t const& route) {
    if (0 == route.route_vehicles.size()) {
        assert(false);
        return -1;
    }

    vehicles::ptr       vehicles_ptr = vehicles::instance();
    npcs::ptr           npcs_ptr = npcs::instance();
    checkpoints::ptr    checkpoints_ptr = checkpoints::instance();

    int             id = route_last_id++;
    route_data_t    route_data(route);
    
    for (int i = 0; i < route.count; ++i) {
        // Создаем таксистов маршрута

        npc_taxi_route_vehicle_t const& route_vehicle = route.route_vehicles[std::rand() % route.route_vehicles.size()];
        int npc_name_id = route_npc_name_last_id++;

        // Создаем транспорт
        int model_id;
        if (!vehicles_ptr->get_model_id_by_name(route_vehicle.model_name, model_id)) {
            assert(false);
            continue;
        }
        int vehicle_id = vehicles_ptr->vehicle_create(pos_vehicle(model_id, 0.0f, 0.0f, 0.0f, route.interior, route.world, 0.0f, -1, -1));
        
        // Подключаем бота
        npcs_ptr->add_npc(make_npc_name(npc_name_id));

        route_data.route_npcs.push_back(route_npc_t(vehicle_id, npc_name_id, route_vehicle.skin_id, route_vehicle.stop));
    }

    npc_taxi_route_vehicle_t const& npc_taxi_route_vehicle = route.route_vehicles.front();
    BOOST_FOREACH(npc_taxi_route_section_t const& section, route_data.route.route_sections) {
        // Создаем секции маршрута

        section.init_rec();
        pos3 stop_pos = get_stop_pos(section, npc_taxi_route_vehicle.stop);

        // Создаем чикпоинт
        int checkpoint_id = checkpoints_ptr->add(checkpoint_t(stop_pos.x, stop_pos.y, stop_pos.z, route.interior, route.world, npc_taxi_route_vehicle.stop.cp_visibility_radius, npc_taxi_route_vehicle.stop.cp_size), std::tr1::bind(&npc_taxi::stop_on_enter, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2), std::tr1::bind(&npc_taxi::stop_on_leave, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

        route_data.route_sections.push_back(route_section_t(checkpoint_id));
    }

    route_datas.insert(std::make_pair(id, route_data));    
    return id;
}

// Удаляем маршрут
void npc_taxi::route_destroy(int route_id) {
    vehicles::ptr       vehicles_ptr = vehicles::instance();
    checkpoints::ptr    checkpoints_ptr = checkpoints::instance();

    route_datas_t::iterator route_datas_it = route_datas.find(route_id);

    if (route_datas.end() == route_datas_it) {
        assert(false);
        return;
    }
    BOOST_FOREACH(route_npc_t& route_npc, route_datas_it->second.route_npcs) {
        // Удаляем бота
        if (route_npc.npc_ptr) {
            route_npc.npc_ptr->kick();
            route_npc.npc_ptr.reset();
        }

        if (route_npc.vehicle_id) {
            // Удаляем транспорт
            vehicles_ptr->vehicle_destroy(route_npc.vehicle_id);
        }
    }
    BOOST_FOREACH(route_section_t& section, route_datas_it->second.route_sections) {
        // Удаляем чикпоинт
        checkpoints_ptr->remove(section.checkpoint_id);
        section.checkpoint_id = checkpoints::invalid_checkpoint_id;
    }

    route_datas.erase(route_datas_it);
}

pos3 npc_taxi::get_stop_pos(npc_taxi_route_section_t const& section, npc_taxi_route_vehicle_stop_t const& stop) {
    float x = section.desc.point_begin.x;
    float y = section.desc.point_begin.y;
    float angle = section.desc.point_begin.rz + stop.shift_angle;
    get_rotated_xy(x, y, stop.shift_radius, angle);
    return pos3(x, y, section.desc.point_begin.z + stop.shift_z, 0, 0);
}

void npc_taxi::update_stops(vehicle_stops_t const& vehicle_stops, routes_t& routes) {
    vehicles::ptr vehicles_ptr = vehicles::instance();
    BOOST_FOREACH(vehicle_stops_t::value_type const& stop_pair, vehicle_stops) {
        int model_id;
        if (vehicles_ptr->get_model_id_by_name(stop_pair.first, model_id)) {
            for (std::set<npc_taxi_route_t>::iterator route_it = routes.update_get_data().begin(); routes.update_get_data().end() != route_it;) {
                bool is_changed = false;
                npc_taxi_route_t route = *route_it;
                BOOST_FOREACH(npc_taxi_route_vehicle_t& route_vehicle, route.route_vehicles) {
                    if (!route_vehicle.stop) {
                        // Описание остановок еще не инициализированно
                        if (stop_pair.first == route_vehicle.model_name) {
                            route_vehicle.stop = stop_pair.second;
                            is_changed = true;
                        }
                    }
                }
                if (is_changed) {
                    std::set<npc_taxi_route_t>::iterator route_curr = route_it;
                    ++route_it;
                    routes.update_get_data().erase(route_curr);
                    routes.update_get_data().insert(route);
                }
                else {
                    ++route_it;
                }
            }
        }
        else {
            //assert(false);
        }
    }
}

std::string npc_taxi::make_npc_name(int route_item_id) {
    return (boost::format("taxi[%1%]") % route_item_id).str();
}

bool npc_taxi::parse_npc_name(std::string const& name, int& route_npc_name_id) {
    if (boost::starts_with(name, "taxi[") && boost::ends_with(name, "]")) {
        std::string id_str = name.substr(5, name.length() - 6);
        try {
            route_npc_name_id = boost::lexical_cast<int>(id_str);
            return true;
        }
        catch (boost::bad_lexical_cast const&) {
        }
    }
    return false;
}

bool npc_taxi::get_npc_route_and_item_ids(int route_npc_name_id, int& route_id, int& route_npc_id) const {
    BOOST_FOREACH(route_datas_t::value_type const& route_pair, route_datas) {
        for (std::size_t i = 0; i < route_pair.second.route_npcs.size(); ++i) {
            if (route_pair.second.route_npcs[i].npc_name_id == route_npc_name_id) {
                route_id = route_pair.first;
                route_npc_id = i;
                return true;
            }
        }
    }
    return false;
}

void npc_taxi::on_timer1000() {
    BOOST_FOREACH(route_datas_t::value_type& route_pair, route_datas) {
        if (route_pair.second.ready_count == route_pair.second.route_npcs.size() && 0 == route_pair.second.start_time) {
            // Запускаем наш маршрут
            route_pair.second.start_time = time_base::tick_count_milliseconds();
        }
    }
}

void npc_taxi::stop_on_enter(player_ptr_t const& player_ptr, int id) {

}

void npc_taxi::stop_on_leave(player_ptr_t const& player_ptr, int id) {

}

npc_taxi_item::npc_taxi_item(npc_taxi& manager, int route_id, int route_npc_id)
:manager(manager)
,route_id(route_id)
,route_npc_id(route_npc_id)
,is_ready(false)
,route_data_ptr(0)
,route_item_ptr(0)
,state(state_t::oninit)
,is_can_use_nitro(false)
{
}

npc_taxi_item::~npc_taxi_item() {

}

void npc_taxi_item::on_connect() {
    if (!get_root()->is_valid() || !is_valid_data()) {
        assert(false);
        return;
    }

    get_root()->set_spawn_info(route_item_ptr->skin_id, pos4(0.0f, 0.0f, 0.0f, route_data_ptr->route.interior, route_data_ptr->route.world, 0.0f));
    get_root()->set_color(route_data_ptr->route.npc_color);
}

void npc_taxi_item::on_spawn() {
    if (!get_root()->is_valid() || !is_valid_data()) {
        assert(false);
        return;
    }
    get_root()->put_to_vehicle(route_item_ptr->vehicle_id);
    if (vehicle_ptr_t vehicle_ptr = vehicles::instance()->get_vehicle(route_item_ptr->vehicle_id)) {
        is_can_use_nitro = vehicle_ptr->is_valid_component("nto_b_tw");
    }
}

void npc_taxi_item::on_enter_vehicle(int vehicle_id) {
    if (!get_root()->is_valid() || !is_valid_data()) {
        assert(false);
        return;
    }
    if (route_data_ptr->route.route_sections.size() < 1) {
        assert(false);
        return;
    }
    if (is_ready) {
        return;
    }
    is_ready = true;
    ++route_data_ptr->ready_count;
}

void npc_taxi_item::on_playback_end() {
    if (!get_root()->is_valid() || !is_valid_data()) {
        assert(false);
        return;
    }
    if (!is_ready || 0 == route_data_ptr->start_time) {
        // Маршрут еще не активирован
        assert(false);
        return;
    }
    time_on_route_t time_on_route = get_time_on_route();
    assert(state_t::onroute == state);

    if (time_on_route.is_stop) {
        start_stop(time_on_route.section_id);
    }
    else {
        start_wait_stop();
    }
}

void npc_taxi_item::on_disconnect(samp::player_disconnect_reason reason) {

}

void npc_taxi_item::on_keystate_change(int keys_new, int keys_old) {
    if (is_can_use_nitro && manager.is_nitro_active) {
        if (is_key_just_down(mouse_lbutton, keys_new, keys_old)) {
            if (!get_root()->is_valid() || !is_valid_data()) {
                assert(false);
                return;
            }
            if (vehicle_ptr_t vehicle_ptr = vehicles::instance()->get_vehicle(route_item_ptr->vehicle_id)) {
                // Ставим нитро
                vehicle_ptr->add_component("nto_b_tw");
            }
        }
        else if (is_key_just_down(KEY_SUBMISSION, keys_new, keys_old)) {
            if (!get_root()->is_valid() || !is_valid_data()) {
                assert(false);
                return;
            }
            if (vehicle_ptr_t vehicle_ptr = vehicles::instance()->get_vehicle(route_item_ptr->vehicle_id)) {
                // Убираем нитро
                vehicle_ptr->remove_component("nto_b_tw");
            }
        }
    }
}

bool npc_taxi_item::is_valid_data() {
    npc_taxi::route_datas_t::iterator route_it = manager.route_datas.find(route_id);
    if (manager.route_datas.end() == route_it) {
        return false;
    }

    route_data_ptr = &route_it->second;

    if (route_data_ptr->route_npcs.size() < static_cast<std::size_t>(route_npc_id)) {
        return false;
    }
    route_item_ptr = &route_data_ptr->route_npcs[route_npc_id];
    return true;
}

void npc_taxi_item::on_timer250() {
    if (!get_root()->is_valid() || !is_valid_data()) {
        return;
    }
    if (!is_ready || 0 == route_data_ptr->start_time) {
        // Маршрут еще не активирован
        return;
    }
    time_on_route_t time_on_route = get_time_on_route();
    
    // Меняем состояние
    if (state_t::oninit == state || state_t::onroute_end == state) {
        if (time_on_route.is_stop) {
            start_stop(time_on_route.section_id);
        }
    }
    else if (state_t::onstop == state) {
        if (!time_on_route.is_stop) {
            start_route(time_on_route.section_id);
        }
    }

}

npc_taxi_item::time_on_route_t npc_taxi_item::get_time_on_route() const {
    int route_time_total = 0;
    BOOST_FOREACH(npc_taxi_route_section_t const& section, route_data_ptr->route.route_sections) {
        route_time_total += section.stop_time + section.desc.duration;
    }
    // Смещение по временной оси для данного бота
    int route_time_shift = route_time_total * route_npc_id / route_data_ptr->route_npcs.size();
    int route_time_on_circle = (time_base::tick_count_milliseconds() - route_data_ptr->start_time + static_cast<time_base::millisecond_t>(route_time_shift)) % route_time_total;

    {
        int time_section_start = 0;
        for (std::size_t i = 0, size = route_data_ptr->route.route_sections.size(); i < size; ++i) {
            npc_taxi_route_section_t const& section = route_data_ptr->route.route_sections[i];
            if (route_time_on_circle >= time_section_start && route_time_on_circle <= time_section_start + section.stop_time) {
                // Мы находимся на остановке
                return time_on_route_t(true, i, route_time_on_circle - time_section_start, section.stop_time);
            }
            else if (route_time_on_circle > time_section_start + section.stop_time && route_time_on_circle <= time_section_start + section.stop_time + section.desc.duration) {
                // Мы находимся на маршруте
                return time_on_route_t(false, i, route_time_on_circle - time_section_start - section.stop_time, section.desc.duration);
            }
            time_section_start += section.stop_time + section.desc.duration;
        }
    }
    throw std::runtime_error("get_time_on_route error");
}

void npc_taxi_item::start_stop(int section_id) {
    get_root()->playback_start(npc::playback_type_driver, route_data_ptr->route.route_sections[section_id].get_rec_name());
    get_root()->playback_pause();
    state = state_t::onstop;
}

void npc_taxi_item::start_route(int section_id) {
    get_root()->playback_start(npc::playback_type_driver, route_data_ptr->route.route_sections[section_id].get_rec_name());
    state = state_t::onroute;
}

void npc_taxi_item::start_wait_stop() {
    state = state_t::onroute_end;
}
