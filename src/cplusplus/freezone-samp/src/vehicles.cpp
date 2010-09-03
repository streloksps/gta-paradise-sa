#include "config.hpp"
#include "vehicles.hpp"
#include <algorithm>
#include <functional>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>
#include "core/module_c.hpp"
#include "checkpoints.hpp"
#include "map_icons.hpp"
#include "mta10_loader.hpp"
#include "server_paths.hpp"
#include "server_configuration.hpp"

REGISTER_IN_APPLICATION(vehicles);

vehicles::ptr vehicles::instance() {
    return application::instance()->get_item<vehicles>();
}

vehicles::vehicles(): is_first_player_connected(true), is_dump_vehicles_on_config(false) {
}

vehicles::~vehicles() {
}

void vehicles::create() {
    command_add("vehicle_paintjob", std::tr1::bind(&vehicles::cmd_vehicle_paintjob, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&vehicles::cmd_vehicle_paintjob_access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("vehicle_color", std::tr1::bind(&vehicles::cmd_vehicle_color, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&vehicles::cmd_vehicle_color_access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("vehicle_modification", std::tr1::bind(&vehicles::cmd_vehicle_modification, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&vehicles::cmd_vehicle_modification_access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    command_add("vehicle_color_list", bind(&vehicles::cmd_vehicle_color_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only, std::tr1::bind(&vehicles::cmd_vehicle_color_access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("vehicle_modification_list", bind(&vehicles::cmd_vehicle_modification_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only, std::tr1::bind(&vehicles::cmd_vehicle_modification_access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

void vehicles::on_first_player_connected() {
    std::vector<int> vehicle_ids;
    vehiclesgame.get_keys(vehicle_ids);
    std::for_each(vehicle_ids.begin(), vehicle_ids.end(), 
        std::tr1::bind(&samp::api::set_vehicle_to_respawn, samp::api::instance(), std::tr1::placeholders::_1)
    );
}

void vehicles::configure_pre() {
    plate = "PLATE___";
    vehicle_defs.clear();
    vehicle_component_defs.clear();
    vehicle_component_ignore_duplicate.clear();
    vehiclesgame.update_begin();
    vehicles_static.clear();
    vehicle_colors.clear();

    vehicle_color_price = 150;
    vehicle_paintjob_price = 250;

    mod_garages_destroy();
    mod_garages.clear();

    repair_garages_destroy();
    repair_garages.clear();

    is_block_on_vehicle_mod = false;
    is_block_on_vehicle_paintjob = false;
    is_block_on_vehicle_respray = false;

    is_trace_enter_leave_mod = false;

    is_dump_vehicles_on_config = false;
}

void vehicles::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(plate);
    SERIALIZE_ITEM(vehicle_defs);
    SERIALIZE_ITEM(vehicle_component_defs);
    SERIALIZE_ITEM(vehicle_component_ignore_duplicate);
    SERIALIZE_NAMED_ITEM(vehiclesgame.update_get_data(), "vehicles");
    SERIALIZE_ITEM(vehicles_static);
    SERIALIZE_ITEM(vehicle_colors);
    SERIALIZE_ITEM(vehicle_color_price);
    SERIALIZE_ITEM(vehicle_paintjob_price);
    SERIALIZE_ITEM(mod_garages);
    SERIALIZE_ITEM(repair_garages);
    SERIALIZE_ITEM(is_block_on_vehicle_mod);
    SERIALIZE_ITEM(is_block_on_vehicle_paintjob);
    SERIALIZE_ITEM(is_block_on_vehicle_respray);

    SERIALIZE_ITEM(is_trace_enter_leave_mod);

    // Загружаем объекты из файлов MTA-SA 1.0
    mta10_loader loader(PATH_MTA10_DIR);
    mta10_loader_vehicles_t loader_mta10;
    SERIALIZE_ITEM(loader_mta10);
    loader.load_items(loader_mta10, vehiclesgame.update_get_data());

    SERIALIZE_ITEM(is_dump_vehicles_on_config);
}

void vehicles::configure_post() {
    if (is_dump_vehicles_on_config) {
        // Вызываем до vehiclesgame.update_end
        dump_vehicles(vehiclesgame.update_get_data());
    }
    vehiclesgame.update_end(std::tr1::bind(&vehicles::vehicle_create, this, std::tr1::placeholders::_1), std::tr1::bind(&vehicles::vehicle_destroy, this, std::tr1::placeholders::_1));
    mod_garages_create();
    repair_garages_create();
}

int vehicles::vehicle_create(pos_vehicle const& pos) {
    vehicle_t vehicle(*this, pos);
    if (vehicle.create()) {
        vehicles_state.insert(std::make_pair(vehicle.get_id(), vehicle));
    }
    return vehicle.get_id();

    /*
    samp::api::ptr api_ptr = samp::api::instance();
    vehicle_t state(pos.color1, pos.color2);
    if (state.is_need_change_color()) {
        // Пытаемся найти цвета - если нашли, то ставим наш
        vehicle_defs_t::const_iterator local_def_it = vehicle_defs.find(pos.model_id);
        if (vehicle_defs.end() != local_def_it && !local_def_it->second.color_items.empty()) {
            vehicle_def_t::color_item_t const& color_item = local_def_it->second.color_items[std::rand() % local_def_it->second.color_items.size()];
            if (vehicle_t::any_color == state.color1) state.color1 = color_item.color1;
            if (vehicle_t::any_color == state.color2) state.color2 = color_item.color2;
        }
    }
    int vehicle_id = api_ptr->create_vehicle(pos.model_id, pos.x, pos.y, pos.z, pos.rz, state.color1, state.color2, pos.respawn_delay);
    api_ptr->link_vehicle_to_interior(vehicle_id, pos.interior);
    api_ptr->set_vehicle_virtualworld(vehicle_id, pos.world);
    vehicles_state.insert(std::make_pair(vehicle_id, state));
    
    if (!is_first_player_connected) {
        api_ptr->set_vehicle_to_respawn(vehicle_id);
    }
    return vehicle_id;
    */
}

void vehicles::vehicle_destroy(int id) {
    samp::api::instance()->destroy_vehicle(id);
    vehicles_state.erase(id);
}

void vehicles::on_gamemode_init(AMX* amx, samp::server_ver ver) {
    BOOST_FOREACH(pos_vehicle const& vehicle_static, vehicles_static) {
        vehicle_t vehicle(*this, vehicle_static);
        if (vehicle.create_static()) {
            vehicles_state.insert(std::make_pair(vehicle.get_id(), vehicle));
        }
    }
}

void vehicles::on_vehicle_spawn(int vehicle_id) {
    samp::api::ptr api_ptr = samp::api::instance();

    
    api_ptr->set_vehicle_number_plate(vehicle_id, plate);


    vehicles_state_t::iterator vehicle_curr_it = vehicles_state.find(vehicle_id);
    if (vehicles_state.end() != vehicle_curr_it) {
        vehicle_curr_it->second.clear();
        int model_id = api_ptr->get_vehicle_model(vehicle_id);
        vehicle_defs_t::const_iterator local_def_it = vehicle_defs.find(model_id);
        if (vehicle_defs.end() != local_def_it) {
            // Может из-за этого вылетает
            if (vehicle_curr_it->second.is_need_change_color() &&!local_def_it->second.color_items.empty()) {
                //vehicle_def_t::color_item_t const& color_item = local_def_it->second.color_items[std::rand() % local_def_it->second.color_items.size()];
                //vehicle_curr_it->second.color1 = color_item.color1;
                //vehicle_curr_it->second.color2 = color_item.color2;
                //api_ptr->change_vehicle_color(vehicle_id, color_item.color1, color_item.color2);
            }
        }
    }
    pos_vehicle curr_v;
    if (vehiclesgame.get_obj_by_key(curr_v, vehicle_id)) {
        // Нашли координаты нашего транспорта
        //api_ptr->link_vehicle_to_interior(vehicle_id, curr_v.interior);
        //api_ptr->set_vehicle_virtualworld(vehicle_id, curr_v.world);
    }
}

void vehicles::on_player_connect(int player_id) {
    if (is_first_player_connected) {
        is_first_player_connected = false;
        on_first_player_connected();
    }
}

bool vehicles::get_component_from_id(int component_id, vehicle_component_defs_t::const_iterator& comp_it) const {
    for (vehicle_component_defs_t::const_iterator it = vehicle_component_defs.begin(), end = vehicle_component_defs.end(); end != it; ++it) {
        vehicle_component_def_t::ids_t const& ids = it->second.ids;
        if (ids.end() != std::find(ids.begin(), ids.end(), component_id)) {
            // Нашли наш ид
            comp_it = it;
            return true;
        }
    }
    // Ничего не нашли
    return false;
}

bool vehicles::on_vehicle_mod(int player_id, int vehicle_id, int component_id) {
    if (player::ptr player_ptr = players::instance()->get_player(player_id)) {
        if (player_ptr->block_get()) return false;

        int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);

        messages_item msg_item;
        msg_item.get_params()
            ("vehicle_id", vehicle_id)
            ("model_id", model_id)
            ("component_id", component_id)
            ("player_name", player_ptr->name_get_full())
            ;

        if (is_block_on_vehicle_mod) {
            player_ptr->block("vehicle/mod", msg_item.get_params().process_all_vars("$(vehicle_id) $(model_id) $(component_id)"));
            return false;
        }

        if (is_valid_component(model_id, component_id)) {
            msg_item.get_sender()("<log_section mod/debug/>$(player_name) $(vehicle_id) $(model_id) $(component_id)", msg_debug);
            return true;
        }
        else {
            msg_item.get_sender()("<log_section mod/error/>$(player_name) $(vehicle_id) $(model_id) $(component_id)", msg_debug);
            return false;
        }
    }
    return false;
}

bool vehicles::on_vehicle_paintjob(int player_id, int vehicle_id, int paintjob_id) {
    if (player::ptr player_ptr = players::instance()->get_player(player_id)) {
        if (player_ptr->block_get()) return false;

        int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);

        messages_item msg_item;
        msg_item.get_params()
            ("vehicle_id", vehicle_id)
            ("model_id", model_id)
            ("paintjob_id", paintjob_id)
            ("player_name", player_ptr->name_get_full())
            ;
        
        if (is_block_on_vehicle_paintjob) {
            player_ptr->block("vehicle/paintjob", msg_item.get_params().process_all_vars("$(vehicle_id) $(model_id) $(paintjob_id)"));
            return false;
        }

        if (is_valid_paintjob(model_id, paintjob_id)) {
            msg_item.get_sender()("<log_section paintjob/debug/>$(player_name) $(vehicle_id) $(model_id) $(paintjob_id)", msg_debug);
            return true;
        }
        else {
            msg_item.get_sender()("<log_section paintjob/error/>$(player_name) $(vehicle_id) $(model_id) $(paintjob_id)", msg_debug);
            return false;
        }
    }
    return false;
}

bool vehicles::on_vehicle_respray(int player_id, int vehicle_id, int color1, int color2) {
    if (player::ptr player_ptr = players::instance()->get_player(player_id)) {
        if (player_ptr->block_get()) return false;

        int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);
        messages_item msg_item;
        msg_item.get_params()
            ("vehicle_id", vehicle_id)
            ("model_id", model_id)
            ("color1", color1)
            ("color2", color2)
            ("player_name", player_ptr->name_get_full())
            ;

        if (is_block_on_vehicle_respray) {
            player_ptr->block("vehicle/respray", msg_item.get_params().process_all_vars("$(vehicle_id) $(model_id) $(color1) $(color2)"));
            return false;
        }

        if (is_valid_colors(model_id, color1, color2)) {
            msg_item.get_sender()("<log_section respray/debug/>$(player_name) $(vehicle_id) $(model_id) $(color1) $(color2)", msg_debug);
            return true;
        }
        else {
            msg_item.get_sender()("<log_section respray/error/>$(player_name) $(vehicle_id) $(model_id) $(color1) $(color2)", msg_debug);
            return false;
        }
    }
    return false;
}

bool vehicles::is_valid_component(int model_id, int component_id) {
    vehicle_defs_t::const_iterator def_it = vehicle_defs.find(model_id);
    if (vehicle_defs.end() != def_it) {
        vehicle_component_defs_t::const_iterator comp_it;
        if (get_component_from_id(component_id, comp_it)) {
            if (def_it->second.components.end() != def_it->second.components.find(comp_it->first)) {
                // Нашли среди разрешенных компонентов
                return true;
            }
        }
    }
    return false;
}

bool vehicles::is_valid_component(int model_id, std::string const& component_name) {
    vehicle_defs_t::const_iterator def_it = vehicle_defs.find(model_id);
    if (vehicle_defs.end() != def_it) {
        vehicle_component_defs_t::const_iterator comp_it = vehicle_component_defs.find(component_name);
        if (vehicle_component_defs.end() != comp_it) {
            if (def_it->second.components.end() != def_it->second.components.find(comp_it->first)) {
                // Нашли среди разрешенных компонентов
                return true;
            }
        }
    }
    return false;
}

bool vehicles::is_valid_paintjob(int model_id, int paintjob_id) {
    if (-1 == paintjob_id || 0 == paintjob_id || 1 == paintjob_id || 2 == paintjob_id) {
        return true;
    }
    return false;
}

bool vehicles::is_valid_colors(int model_id, int color1, int color2) {
    if (0 <= color1 && 127 >= color1 && 0 <= color2 && 127 >= color2) {
        return true;
    }
    return false;
}

bool vehicles::is_player_can_tune(player_ptr_t const& player_ptr) {
    garage_vals_t::const_iterator garage_vals_it = garage_vals.find(checkpoints::instance()->get_active_checkpoint(player_ptr));
    if (garage_vals.end() == garage_vals_it) {
        return false;
    }
    garage_val_t const& garage_val = garage_vals_it->second;

    int vehicle_id;
    vehicle_defs_t::const_iterator vehicle_def_it;
    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, vehicle_def_it)) {
        return false;
    }

    return garage_val.mod_garage_ptr->valid_vehicles.end() != garage_val.mod_garage_ptr->valid_vehicles.find(vehicle_def_it->second.sys_name);
}

bool vehicles::get_vehicle_def_by_player(player_ptr_t const& player_ptr, int& vehicle_id, vehicle_defs_t::const_iterator& vehicle_def_it) const {
    int local_vehicle_id;
    int model_id;
    bool is_driver;
    if (!player_ptr->get_vehicle(local_vehicle_id, model_id, is_driver)) return false;

    vehicle_defs_t::const_iterator local_def_it = vehicle_defs.find(model_id);
    if (vehicle_defs.end() == local_def_it) return false;
    
    vehicle_id = local_vehicle_id;
    vehicle_def_it = local_def_it;
    return true;
}


bool vehicles::get_trailer_def_by_player(player_ptr_t const& player_ptr, int& trailer_id, vehicle_defs_t::const_iterator& trailer_def_it) const {
    int local_vehicle_id;
    int model_id;
    bool is_driver;
    if (!player_ptr->get_vehicle(local_vehicle_id, model_id, is_driver)) return false;

    samp::api::ptr api_ptr = samp::api::instance();
    if (!api_ptr->is_trailer_attached_to_vehicle(local_vehicle_id)) return false;

    int local_trailer_id = api_ptr->get_vehicle_trailer(local_vehicle_id);
    int trailer_model_id = api_ptr->get_vehicle_model(local_trailer_id);

    vehicle_defs_t::const_iterator local_def_it = vehicle_defs.find(trailer_model_id);
    if (vehicle_defs.end() == local_def_it) return false;

    trailer_id = local_trailer_id;
    trailer_def_it = local_def_it;
    return true;
}

bool vehicles::cmd_vehicle_paintjob_access_checker_impl(player_ptr_t const& player_ptr) {
    int vehicle_id;
    vehicle_defs_t::const_iterator def_it;

    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, def_it)) return false;

    return !def_it->second.paintjobs.empty();
}

bool vehicles::cmd_vehicle_color_access_checker_impl(player_ptr_t const& player_ptr) {
    int vehicle_id;
    vehicle_defs_t::const_iterator def_it;

    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, def_it)) return false;

    return 0 != def_it->second.colors;
}

bool vehicles::cmd_vehicle_modification_access_checker_impl(player_ptr_t const& player_ptr) {
    int vehicle_id;
    vehicle_defs_t::const_iterator def_it;

    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, def_it)) return false;

    return !def_it->second.components.empty();
}

bool vehicles::cmd_vehicle_paintjob_access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name) {
    if (!is_player_can_tune(player_ptr)) return false;
    return cmd_vehicle_paintjob_access_checker_impl(player_ptr);
}

bool vehicles::cmd_vehicle_color_access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name) {
    if (!is_player_can_tune(player_ptr)) return false;
    return cmd_vehicle_color_access_checker_impl(player_ptr);
}

bool vehicles::cmd_vehicle_modification_access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name) {
    if (!is_player_can_tune(player_ptr)) return false;
    return cmd_vehicle_modification_access_checker_impl(player_ptr);
}

command_arguments_rezult vehicles::cmd_vehicle_paintjob(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int vehicle_id;
    vehicle_defs_t::const_iterator vehicle_def_it;
    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, vehicle_def_it)) return cmd_arg_process_error;

    params("v_id", vehicle_id); set_params(vehicle_def_it, params, "v_");
    int paintjobs_count = vehicle_def_it->second.paintjobs.size();
    vehicles_state_t::iterator vehicle_curr_it = vehicles_state.find(vehicle_id);

    garage_vals_t::const_iterator garage_vals_it = garage_vals.find(checkpoints::instance()->get_active_checkpoint(player_ptr));
    if (garage_vals.end() == garage_vals_it) {
        assert(false);
        return cmd_arg_process_error;
    }
    garage_val_t const& garage_val = garage_vals_it->second;

    params
        ("mod_garage_name", garage_val.mod_garage_ptr->name)
        ("mod_garage_sys_name", garage_val.mod_garage_ptr->sys_name)
        ("garage_sys_name", garage_val.garage_ptr->sys_name)
        ;

    int paintjob_id;

    { // Парсинг
        std::istringstream iss(arguments);
            iss>>paintjob_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    params
        ("money", vehicle_paintjob_price)
        ("id", paintjob_id)
        ("id_max", paintjobs_count);

    if (paintjob_id < 1 || paintjob_id > paintjobs_count) {
        pager("$(cmd_vehicle_paintjob_error_out_of_range)");
        return cmd_arg_process_error;
    }

    if (!player_ptr->get_item<player_money_item>()->can_take(vehicle_paintjob_price)) {
        pager("$(money_error_low_money)");
        return cmd_arg_process_error;
    }

    int paintjob_gta_id = vehicle_def_it->second.paintjobs[paintjob_id - 1];
    params("gta_id", paintjob_gta_id);

    if (vehicles_state.end() != vehicle_curr_it) {
        if (vehicle_curr_it->second.paintjob_id == paintjob_gta_id) {
            pager("$(cmd_vehicle_paintjob_error_duplicate)");
            return cmd_arg_process_error;
        }
        vehicle_curr_it->second.paintjob_id = paintjob_gta_id;
    }

    player_ptr->get_item<player_money_item>()->take(vehicle_paintjob_price);
    samp::api::instance()->change_vehicle_paintjob(vehicle_id, paintjob_gta_id);

    pager("$(cmd_vehicle_paintjob_done)");
    sender("$(cmd_vehicle_paintjob_done_log)", msg_log);

    return cmd_arg_ok;
}

command_arguments_rezult vehicles::cmd_vehicle_color(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int vehicle_id, trailer_id;
    vehicle_defs_t::const_iterator vehicle_def_it, trailer_def_it;
    vehicles_state_t::iterator vehicle_curr_it = vehicles_state.end(), trailer_curr_it = vehicles_state.end();
    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, vehicle_def_it)) return cmd_arg_process_error;
    bool is_has_trailer = get_trailer_def_by_player(player_ptr, trailer_id, trailer_def_it);

    params("v_id", vehicle_id); set_params(vehicle_def_it, params, "v_");
    int colors_count = vehicle_def_it->second.colors;
    vehicle_curr_it = vehicles_state.find(vehicle_id);

    if (is_has_trailer) {
        colors_count += trailer_def_it->second.colors;
        params("t_id", trailer_id); set_params(trailer_def_it, params, "t_");
        trailer_curr_it = vehicles_state.find(trailer_id);
    }

    garage_vals_t::const_iterator garage_vals_it = garage_vals.find(checkpoints::instance()->get_active_checkpoint(player_ptr));
    if (garage_vals.end() == garage_vals_it) {
        assert(false);
        return cmd_arg_process_error;
    }
    garage_val_t const& garage_val = garage_vals_it->second;

    params
        ("mod_garage_name", garage_val.mod_garage_ptr->name)
        ("mod_garage_sys_name", garage_val.mod_garage_ptr->sys_name)
        ("garage_sys_name", garage_val.garage_ptr->sys_name)
        ;

    enum {max_colors = 3};

    boost::optional<int> colors[max_colors];
    int colors_change_count = 0;

    { // Парсинг
        std::string not_change_string = params.process_all_vars("$(cmd_vehicle_color_param_notchange)");
        std::istringstream iss(arguments);
        std::string colors_text[max_colors];

        if (colors_count > max_colors) colors_count = max_colors;

        if (3 == colors_count) {
            iss>>colors_text[0]>>colors_text[1]>>colors_text[2];
        }
        else if (2 == colors_count) {
            iss>>colors_text[0]>>colors_text[1];
        }
        else if (1 == colors_count) {
            iss>>colors_text[0];
        }
        else {
            assert(false);
            return cmd_arg_process_error;
        }

        if (iss.fail() || !iss.eof()) {
            pager("$(cmd_vehicle_color_error_arg" + boost::lexical_cast<std::string>(colors_count) +")");
            return cmd_arg_process_error;
        }

        try {
            for (int i = 0; i < colors_count; ++i) {
                if (!boost::iequals(not_change_string, colors_text[i], locale_ru)) {
                    colors[i].reset(boost::lexical_cast<int>(colors_text[i]));
                    ++colors_change_count;
                }
            }
        }
        catch (boost::bad_lexical_cast &) {
            pager("$(cmd_vehicle_color_error_arg" + boost::lexical_cast<std::string>(colors_count) +")");
            return cmd_arg_process_error;
        }
    }

    if (0 == colors_change_count) {
        pager("$(cmd_vehicle_color_error_not_colors)");
        return cmd_arg_process_error;
    }

    for (int i = 0; i < colors_count; ++i) {
        if (colors[i] && (*colors[i] < 1 || *colors[i] > static_cast<int>(vehicle_colors.size()))) {
            params("id", *colors[i])("id_max", vehicle_colors.size());
            pager("$(cmd_vehicle_color_error_out_of_range)");
            return cmd_arg_process_error;
        }
    }

    params("money", vehicle_color_price * colors_change_count);
    params("money_item", vehicle_color_price);

    if (!player_ptr->get_item<player_money_item>()->can_take(vehicle_color_price * colors_change_count)) {
        pager("$(money_error_low_money)");
        return cmd_arg_process_error;
    }

    // Все проверки пройдены - перекрашиваем
    player_ptr->get_item<player_money_item>()->take(vehicle_color_price * colors_change_count);

    { // Перекрашиваем вихл
        int  gta_colors[2] = {0, 0};
        bool gta_colors_change[2] = {false, false};

        for (int i = 0; i < vehicle_def_it->second.colors; ++i) {
            if (colors[i]) {
                gta_colors_change[i] = true;
                gta_colors[i] = vehicle_colors[*colors[i]-1].id;
                set_params_colors(*colors[i], params, (boost::format("color%1%_") % (i + 1)).str());
                pager((boost::format("$(cmd_vehicle_color_done%1%)") % (i + 1)).str());
                sender((boost::format("$(cmd_vehicle_color_done%1%_log)") % (i + 1)).str(), msg_log);
            }
        }
        if (vehicles_state.end() != vehicle_curr_it) {
            if (0 < vehicle_def_it->second.colors) {
                if (colors[0]) {
                    vehicle_curr_it->second.color1 = gta_colors[0];
                }
                else {
                    gta_colors[0] = vehicle_curr_it->second.color1;
                }
            }
            if (1 < vehicle_def_it->second.colors) {
                if (colors[1]) {
                    vehicle_curr_it->second.color2 = gta_colors[1];
                }
                else {
                    gta_colors[1] = vehicle_curr_it->second.color2;
                }
            }
        }
        if (gta_colors_change[0] || gta_colors_change[1]) {
            samp::api::instance()->change_vehicle_color(vehicle_id, gta_colors[0], gta_colors[1]);
        }
    }

    if (is_has_trailer && 0 != trailer_def_it->second.colors) {
        // Перекрашиваем прицеп
        int index = vehicle_def_it->second.colors;
        if (colors[index]) {
            int gta_color = vehicle_colors[*colors[index]-1].id;
            set_params_colors(*colors[index], params, "color_trailer_");
            pager("$(cmd_vehicle_color_done_trailer)");
            sender("$(cmd_vehicle_color_done_trailer_log)", msg_log);
            if (vehicles_state.end() != trailer_curr_it) {
                trailer_curr_it->second.color1 = gta_color;
            }
            samp::api::instance()->change_vehicle_color(trailer_id, gta_color, gta_color);
        }
    }

    return cmd_arg_ok;
}

command_arguments_rezult vehicles::cmd_vehicle_modification(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    command_arguments_rezult rezult = cmd_arg_syntax_error;
    for (boost::split_iterator<std::string::const_iterator> args_it = boost::make_split_iterator(arguments, boost::token_finder(boost::is_any_of(params.process_all_vars("$(cmd_vehicle_modification_arg_delimiter)")), boost::token_compress_on)), end; args_it != end; ++args_it) {
        rezult = cmd_vehicle_modification_impl(player_ptr, boost::trim_copy(boost::copy_range<std::string>(*args_it)), pager, sender, params);
        if (cmd_arg_ok != rezult) {
            break;
        }
    }

    return rezult;
}

command_arguments_rezult vehicles::cmd_vehicle_modification_impl(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    std::string group_name;
    bool is_has_id;
    int id;

    { // Парсинг
        // Пробуем распарсить 2 параметра
        std::istringstream iss2(arguments);
        iss2>>group_name>>id;
        if (iss2.fail() || !iss2.eof()) {
            // Пробуем распарсить 1 параметр
            std::istringstream iss1(arguments);
            iss1>>group_name;
            if (iss1.fail() || !iss1.eof()) {
                return cmd_arg_syntax_error;
            }
            else {
                is_has_id = false;
                id = 1;
            }
        }
        else {
            is_has_id = true;
        }
    }

    params
        ("user_group_name", group_name)
        ("id", id);

    int vehicle_id, trailer_id;
    vehicle_defs_t::const_iterator vehicle_def_it, trailer_def_it;
    vehicles_state_t::iterator vehicle_curr_it = vehicles_state.end(), trailer_curr_it = vehicles_state.end();
    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, vehicle_def_it)) return cmd_arg_process_error;
    bool is_has_trailer = get_trailer_def_by_player(player_ptr, trailer_id, trailer_def_it);

    params("v_id", vehicle_id); set_params(vehicle_def_it, params, "v_");
    vehicle_curr_it = vehicles_state.find(vehicle_id);

    if (is_has_trailer) {
        params("t_id", trailer_id); set_params(trailer_def_it, params, "t_");
        trailer_curr_it = vehicles_state.find(trailer_id);
    }


    garage_vals_t::const_iterator garage_vals_it = garage_vals.find(checkpoints::instance()->get_active_checkpoint(player_ptr));
    if (garage_vals.end() == garage_vals_it) {
        assert(false);
        return cmd_arg_process_error;
    }
    garage_val_t const& garage_val = garage_vals_it->second;

    params
        ("mod_garage_name", garage_val.mod_garage_ptr->name)
        ("mod_garage_sys_name", garage_val.mod_garage_ptr->sys_name)
        ("garage_sys_name", garage_val.garage_ptr->sys_name)
        ;

    mods_groups_t mods_groups;
    get_mods_groups(vehicle_def_it, mods_groups);

    mods_groups_t::const_iterator mods_groups_it = mods_groups.find(group_name);
    if (mods_groups.end() == mods_groups_it) {
        pager("$(cmd_vehicle_modification_error_bad_group)");
        return cmd_arg_process_error;
    }
    params
        ("group_name", mods_groups_it->first)
        ("id_max", mods_groups_it->second.size());

    if (1 != mods_groups_it->second.size() && !is_has_id) {
        // Если у нас более 1 компонента в группе, то необходимо явное указание индекса
        pager("$(cmd_vehicle_modification_error_need_id)");
        return cmd_arg_process_error;
    }

    if ((int)mods_groups_it->second.size() < id || 1 > id) {
        pager("$(cmd_vehicle_modification_error_out_of_range)");
        return cmd_arg_process_error;
    }

    mod_item_t mod_item;
    {
        mods_group_item_t::const_iterator component_it = mods_groups_it->second.begin();
        for (int i = 1; i < id; ++i) {
            ++component_it;
        }
        mod_item = component_it->second;
    }

    params
        ("sys_name", mod_item.first)
        ("name", mod_item.second->name)
        ("money", mod_item.second->price)
        ("slot", mod_item.second->slot)
        ;

    if (!player_ptr->get_item<player_money_item>()->can_take(mod_item.second->price)) {
        pager("$(money_error_low_money)");
        return cmd_arg_process_error;
    }

    // Истина, если не нужно проверять на дубликаты
    bool is_ignore_duplicate = vehicle_component_ignore_duplicate.end() != vehicle_component_ignore_duplicate.find(mod_item.first);

    bool is_need_mod_vehicle = false;
    bool is_need_mod_trailer = false;

    if (is_ignore_duplicate || vehicles_state.end() == vehicle_curr_it) {
        is_need_mod_vehicle = true;
    }
    else {
        vehicle_t::mods_t::const_iterator curr_slot = vehicle_curr_it->second.mods.find(mod_item.second->slot);
        if (vehicle_curr_it->second.mods.end() == curr_slot || mod_item.first != curr_slot->second) {
            vehicle_curr_it->second.mods[mod_item.second->slot] = mod_item.first;
            is_need_mod_vehicle = true;
        }
    }

    if (is_has_trailer && trailer_def_it->second.components.end() != trailer_def_it->second.components.find(mod_item.first)) {
        // У нас есть прицеп, который может быть затюнены таким же компонетом
        if (is_ignore_duplicate || vehicles_state.end() == trailer_curr_it) {
            is_need_mod_trailer = true;
        }
        else {
            vehicle_t::mods_t::const_iterator curr_trailer_slot = trailer_curr_it->second.mods.find(mod_item.second->slot);
            if (trailer_curr_it->second.mods.end() == curr_trailer_slot || mod_item.first != curr_trailer_slot->second) {
                trailer_curr_it->second.mods[mod_item.second->slot] = mod_item.first;
                is_need_mod_trailer = true;
            }
        }
    }

    if (!is_need_mod_vehicle && !is_need_mod_trailer) {
        pager("$(cmd_vehicle_modification_error_duplicate)");
        return cmd_arg_process_error;
    }

    player_ptr->get_item<player_money_item>()->take(mod_item.second->price);

    if (is_need_mod_vehicle) {
        add_component(*mod_item.second, vehicle_id);

        pager("$(cmd_vehicle_modification_done)");
        sender("$(cmd_vehicle_modification_done_log)", msg_log);
    }

    if (is_need_mod_trailer) {
        add_component(*mod_item.second, trailer_id);

        pager("$(cmd_vehicle_modification_done_trailer)");
        sender("$(cmd_vehicle_modification_done_trailer_log)", msg_log);
    }

    return cmd_arg_ok;
}

void vehicles::set_params(vehicle_defs_t::const_iterator const& def_it, messages_params& params, std::string const& prefix) {
    params
        (prefix + "model_id", def_it->first)
        (prefix + "sys_name", def_it->second.sys_name)
        (prefix + "name", def_it->second.name)
        (prefix + "real_name", def_it->second.real_name)
        ;
}

void vehicles::set_params_colors(int index, messages_params& params, std::string const& prefix) {
    vehicle_color_t& vehicle_color = vehicle_colors[index - 1];
    params
        (prefix + "id", index)
        (prefix + "gtaid", vehicle_color.id)
        (prefix + "name", vehicle_color.name)
        (prefix + "raw", vehicle_color.color)
        ;
}

command_arguments_rezult vehicles::cmd_vehicle_modification_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int vehicle_id;
    vehicle_defs_t::const_iterator vehicle_def_it;
    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, vehicle_def_it)) return cmd_arg_process_error;

    params("v_id", vehicle_id); set_params(vehicle_def_it, params, "v_");
    mods_groups_t mods_groups;
    get_mods_groups(vehicle_def_it, mods_groups);

    pager.set_header_text(0, "$(cmd_vehicle_modification_list_header)");
    BOOST_FOREACH(mods_groups_t::value_type const& mods_group, mods_groups) {
        params("group_name", mods_group.first);
        if (1 == mods_group.second.size()) {
            // В группе у нас 1 элемент - обрабатывать его отдельно
            params
                ("name", mods_group.second.begin()->first)
                ("price", mods_group.second.begin()->second.second->price)
                ;
            pager("$(cmd_vehicle_modification_list_gp_single)");
        }
        else {
            // В группе несколько элементов
            pager.items_add("$(cmd_vehicle_modification_list_gp_header)");
            int id = 1;
            BOOST_FOREACH(mods_group_item_t::value_type const& mods_group_item, mods_group.second) {
                params
                    ("id", id++)
                    ("name", mods_group_item.first)
                    ("price", mods_group_item.second.second->price)
                    ;
                pager.items_add("$(cmd_vehicle_modification_list_item)");
            }
            pager.items_done();
        }
    }
    return cmd_arg_auto;
}

command_arguments_rezult vehicles::cmd_vehicle_color_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int vehicle_id, trailer_id;
    vehicle_defs_t::const_iterator vehicle_def_it, trailer_def_it;
    if (!get_vehicle_def_by_player(player_ptr, vehicle_id, vehicle_def_it)) return cmd_arg_process_error;
    bool is_has_trailer = get_trailer_def_by_player(player_ptr, trailer_id, trailer_def_it);

    int colors = vehicle_def_it->second.colors;
    if (is_has_trailer) {
        colors += trailer_def_it->second.colors;
    }

    params("v_id", vehicle_id); set_params(vehicle_def_it, params);

    pager.set_header_text(0, "$(cmd_vehicle_color_list_header" + boost::lexical_cast<std::string>(colors) + ")");

    for (std::size_t i = 0, size = vehicle_colors.size(); i < size; ++i) {
        params
            ("id", i + 1)
            ("color", vehicle_colors[i].color)
            ("name", vehicle_colors[i].name);
        pager("$(cmd_vehicle_color_list_item)");
    }
    return cmd_arg_auto;
}

void vehicles::get_mods_groups(vehicle_defs_t::const_iterator const& vehicle_def_it, mods_groups_t& mods_groups) {
    BOOST_FOREACH(std::string const& mod_name, vehicle_def_it->second.components) {
        vehicle_component_defs_t::const_iterator cmp_it = vehicle_component_defs.find(mod_name);
        if (vehicle_component_defs.end() != cmp_it) {
            mods_groups[cmp_it->second.group].insert(std::make_pair(cmp_it->second.name, std::make_pair(cmp_it->first, &cmp_it->second)));
        }
    }
}

void vehicles::add_component(vehicle_component_def_t const& component_def, int vehicle_id) {
    std::for_each(component_def.ids.begin(), component_def.ids.end(), std::tr1::bind(&samp::api::add_vehicle_component, samp::api::instance(), vehicle_id, std::tr1::placeholders::_1));
}

void vehicles::remove_component(vehicle_component_def_t const& component_def, int vehicle_id) {
    std::for_each(component_def.ids.begin(), component_def.ids.end(), std::tr1::bind(&samp::api::remove_vehicle_component, samp::api::instance(), vehicle_id, std::tr1::placeholders::_1));
}

void vehicles::mod_garages_create() {
    BOOST_FOREACH(mod_garage_t const& mod_garage, mod_garages) {
        BOOST_FOREACH(mod_garage_item_t const& garage, mod_garage.items) {
            garage_val_t garage_val;
            garage_val.mod_garage_ptr = &mod_garage;
            garage_val.garage_ptr = &garage;
            garage_val.mapicon_id = map_icons::instance()->add(garage.x, garage.y, garage.z, garage.interior, garage.world, garage.mapicon_radius, mod_garage.mapicon_marker_type);
            garage_val.checkpoint_id = checkpoints::instance()->add(garage, std::tr1::bind(&vehicles::mod_garage_item_on_enter, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2), std::tr1::bind(&vehicles::mod_garage_item_on_leave, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
            garage_vals.insert(std::make_pair(garage_val.checkpoint_id, garage_val));
        }
    }
}

void vehicles::mod_garages_destroy() {
    BOOST_FOREACH(garage_vals_t::value_type const& garage_val_pair, garage_vals) {
        map_icons::instance()->remove(garage_val_pair.second.mapicon_id);
        checkpoints::instance()->remove(garage_val_pair.second.checkpoint_id);
    }
    garage_vals.clear();
}

void vehicles::mod_garage_item_on_enter(player_ptr_t const& player_ptr, int id) {
    messages_item msg_item;
    msg_item.get_params()
        ("checkpoint_id", id)
        ("player_name", player_ptr->name_get_full());

    garage_vals_t::const_iterator garage_vals_it = garage_vals.find(id);
    if (garage_vals.end() == garage_vals_it) {
        assert(false);
        return;
    }
    garage_val_t const& garage_val = garage_vals_it->second;

    msg_item.get_params()
        ("mod_garage_name", garage_val.mod_garage_ptr->name)
        ("mod_garage_sys_name", garage_val.mod_garage_ptr->sys_name)
        ("garage_sys_name", garage_val.garage_ptr->sys_name)
    ;

    int vehicle_id;
    vehicle_defs_t::const_iterator vehicle_def_it;
    bool vehicle_def_exhist = get_vehicle_def_by_player(player_ptr, vehicle_id, vehicle_def_it);
    bool is_valid_vehicle = false; 
    if (vehicle_def_exhist) {
        msg_item.get_params()("v_id", vehicle_id); set_params(vehicle_def_it, msg_item.get_params(), "v_");
        is_valid_vehicle = garage_val.mod_garage_ptr->valid_vehicles.end() != garage_val.mod_garage_ptr->valid_vehicles.find(vehicle_def_it->second.sys_name);
    }
    std::string valid_cmd_list;
    int cmd_count = 0;
    if (is_valid_vehicle) {
        if (cmd_vehicle_paintjob_access_checker_impl(player_ptr)) {
            valid_cmd_list += "$(cmd_vehicle_paintjob_name) ";
            ++cmd_count;
        }
        if (cmd_vehicle_color_access_checker_impl(player_ptr)) {
            valid_cmd_list += "$(cmd_vehicle_color_name) ";
            ++cmd_count;
        }
        if (cmd_vehicle_modification_access_checker_impl(player_ptr)) {
            valid_cmd_list += "$(cmd_vehicle_modification_name) ";
            ++cmd_count;
        }
        boost::trim(valid_cmd_list);
    }

    msg_item.get_params()("valid_cmd_list", valid_cmd_list);

    if (!vehicle_def_exhist) {
        msg_item.get_sender()("$(on_modgarage_enter_foot)", msg_player(player_ptr));
        if (is_trace_enter_leave_mod) {
            msg_item.get_sender()("$(on_modgarage_enter_foot_log)", msg_debug);
        }
    }
    else if (is_valid_vehicle && 0 != cmd_count) {
        msg_item.get_sender()("$(on_modgarage_enter_valid)", msg_player(player_ptr));
        if (is_trace_enter_leave_mod) {
            msg_item.get_sender()("$(on_modgarage_enter_valid_log)", msg_debug);
        }
    }
    else {
        msg_item.get_sender()("$(on_modgarage_enter_invalid)", msg_player(player_ptr));
        if (is_trace_enter_leave_mod) {
            msg_item.get_sender()("$(on_modgarage_enter_invalid_log)", msg_debug);
        }
    }
}

void vehicles::mod_garage_item_on_leave(player_ptr_t const& player_ptr, int id) {
    if (is_trace_enter_leave_mod) {
        messages_item msg_item;
        msg_item.get_params()
            ("checkpoint_id", id)
            ("player_name", player_ptr->name_get_full());

        garage_vals_t::const_iterator garage_vals_it = garage_vals.find(id);
        if (garage_vals.end() == garage_vals_it) {
            assert(false);
            return;
        }
        garage_val_t const& garage_val = garage_vals_it->second;

        msg_item.get_params()
            ("mod_garage_name", garage_val.mod_garage_ptr->name)
            ("mod_garage_sys_name", garage_val.mod_garage_ptr->sys_name)
            ("garage_sys_name", garage_val.garage_ptr->sys_name)
            ;

        msg_item.get_sender()("$(on_modgarage_leave_log)", msg_debug);
    }
}

int vehicles::get_max_vehicle_id() const {
    if (samp::server_ver_03a == samp::api::instance()->get_ver()) {
        return samp::api::MAX_VEHICLES_v030;
    }
    return samp::api::MAX_VEHICLES;
}

pos4 vehicles::pos_get(int vehicle_id) const {
    pos4 rezult;
    samp::api::ptr api_ptr = samp::api::instance();
    if (is_valid_vehicle_id(vehicle_id)) {
        // Дописать интерьер
        api_ptr->get_vehicle_pos(vehicle_id, rezult.x, rezult.y, rezult.z);
        rezult.rz = api_ptr->get_vehicle_zangle(vehicle_id);
        rezult.world = api_ptr->get_vehicle_virtual_world(vehicle_id);
    }
    return rezult;
}

bool vehicles::is_valid_vehicle_id(int vehicle_id) const {
    return 0 != samp::api::instance()->get_vehicle_model(vehicle_id);
}

bool vehicles::is_vehicle_created_by_me(int vehicle_id) const {
    return vehicles_state.end() != vehicles_state.find(vehicle_id);
}

bool vehicles::get_model_id_by_name(std::string const& gta_model_name, int& model_id) const {
    BOOST_FOREACH(vehicle_defs_t::value_type const& vehicle_def, vehicle_defs) {
        if (boost::iequals(gta_model_name, vehicle_def.second.sys_name)) {
            model_id = vehicle_def.first;
            return true;
        }
    }
    return false;
}

bool vehicles::get_component_id_by_name(std::string const& gta_component_name, int& first_component_id) const {
    vehicle_component_defs_t::const_iterator it = vehicle_component_defs.find(gta_component_name);
    if (vehicle_component_defs.end() != it) {
        if (!it->second.ids.empty()) {
            first_component_id = it->second.ids[0];
            return true;
        }
    }
    return false;
}

std::string vehicles::get_component_name_by_id(int component_id) {
    BOOST_FOREACH(vehicle_component_defs_t::value_type const& vehicle_component_def, vehicle_component_defs) {
        if (vehicle_component_def.second.ids.end() != std::find(vehicle_component_def.second.ids.begin(), vehicle_component_def.second.ids.end(), component_id)) {
            return vehicle_component_def.first;
        }
    }
    return "no_name";
}

void vehicles::repair_garages_destroy() {

}

void vehicles::repair_garages_create() {

}

void vehicles::repair_garage_item_on_enter(player_ptr_t const& player_ptr, int id) {

}

void vehicles::repair_garage_item_on_leave(player_ptr_t const& player_ptr, int id) {

}

vehicle_ptr_t vehicles::get_vehicle(int vehicle_id) {
    vehicles_state_t::iterator vehicle_curr_it = vehicles_state.find(vehicle_id);
    if (vehicles_state.end() != vehicle_curr_it) {
        return &vehicle_curr_it->second;
    }
    return 0;
}

void vehicles::dump_vehicles(vehiclesgame_t::update_t& vehicles) {
    buffer::ptr buff(new buffer());
    serialize(buff, serialization::make_nvp(vehicles, "vehicles"), serialization::make_def(&configuradable::configure, serialization::def_writing));
    server_configuration::configure_use_buffer_debug_save(buff, PATH_VEHICLES_DUMP_FILENAME);
}
