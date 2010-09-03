#include "config.hpp"
#include "player_animation.hpp"
#include "core/module_c.hpp"
#include "core/samp/samp_api.hpp"
#include "core/utility/locale_ru.hpp"
#include <boost/algorithm/string.hpp>

REGISTER_IN_APPLICATION(player_animation);

player_animation::player_animation() {
}

player_animation::~player_animation() {
}

void player_animation::create() {
    command_add("animation", std::tr1::bind(&player_animation::cmd_animation, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
    command_add("animation_list", std::tr1::bind(&player_animation::cmd_animation_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);

    command_add("special_dance", std::tr1::bind(&player_animation::cmd_special_dance, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
    command_add("special_handsup", std::tr1::bind(&player_animation::cmd_special_handsup, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
    command_add("special_phone", std::tr1::bind(&player_animation::cmd_special_phone, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
    command_add("special_pissing", std::tr1::bind(&player_animation::cmd_special_pissing, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));

    if (samp::api::instance()->is_has_030_features()) {
        command_add("special_drink", std::tr1::bind(&player_animation::cmd_special_drink, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
        command_add("special_smoke", std::tr1::bind(&player_animation::cmd_special_smoke, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
    }
}

void player_animation::configure_pre() {
    animation_libs.clear();
    is_block_on_jetpack = false;
}

void player_animation::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(animation_libs);
    SERIALIZE_ITEM(is_block_on_jetpack);
}

void player_animation::configure_post() {
}

void player_animation::on_connect(player_ptr_t const& player_ptr) {
    player_animation_item::ptr item(new player_animation_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

command_arguments_rezult player_animation::cmd_animation(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    std::string anim_lib_name;
    int         anim_index = -1;
    { // Парсинг
        std::istringstream iss(arguments);
        iss>>anim_lib_name>>anim_index;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    params
        ("anim_lib_name", anim_lib_name)
        ("anim_index", anim_index);

    animation_libs_t::const_iterator it = animation_libs.find(anim_lib_name);
    if (animation_libs.end() == it) {
        pager("$(animation_error_bad_lib_name)");
        return cmd_arg_process_error;
    }
    params("anim_lib_name", it->first);
    if ((int)it->second.anim_names.size() < anim_index || 1 > anim_index) {
        params("anim_index_min", 1)("anim_index_max", it->second.anim_names.size());
        pager("$(animation_error_bad_index)");
        return cmd_arg_process_error;
    }

    if (!player_ptr->group_is_in("admin")) { // Пока админам разрешаем все :)
        if (!it->second.is_use_in_vehicle && player_ptr->vehicle_is_in()) {
            pager("$(animation_error_not_for_vehicle)");
            return cmd_arg_process_error;
        }
    }

    {
        std::string anim_lib_name_raw = it->second.lib_name;
        std::string anim_name_raw = it->second.anim_names[anim_index - 1];

        samp::api::instance()->apply_animation(player_ptr->get_id(), anim_lib_name_raw, anim_name_raw, 4.1f, 0, 1, 1, 1, 1, 0);
        params
            ("anim_lib_name_raw", anim_lib_name_raw)
            ("anim_name_raw", anim_name_raw);
        
        pager("$(animation_done_player)");
        sender("$(animation_done_log)", msg_log);
    }

    return cmd_arg_ok;
}

command_arguments_rezult player_animation::cmd_animation_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (animation_libs.empty()) {
        pager("$(animation_list_empty)");
        return cmd_arg_auto;
    }

    params
        ("anim_libs_count", animation_libs.size())
        ("anim_count", get_total_anims());
    pager.set_header_text(0, "$(animation_list_header)");
    BOOST_FOREACH(animation_libs_t::value_type const& item, animation_libs) {
        params("anim_name", item.first)("anim_count", item.second.anim_names.size());
        pager.items_add("$(animation_list_item)");
    }
    pager.items_done();
    return cmd_arg_auto;
}

command_arguments_rezult player_animation::cmd_special_dance(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int id = -1;
    { // Парсинг
        std::istringstream iss(arguments);
        iss>>id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    if (player_ptr->vehicle_is_in()) {
        pager("$(player_not_on_foot)");
        return cmd_arg_process_error;
    }

    if (1 > id || 4 < id) {
        pager("$(special_dance_error_id)");
        return cmd_arg_process_error;
    }

    params("dance_id", id);
    static samp::api::special_action dances[] = {samp::api::SPECIAL_ACTION_DANCE1, samp::api::SPECIAL_ACTION_DANCE2, samp::api::SPECIAL_ACTION_DANCE3, samp::api::SPECIAL_ACTION_DANCE4};
    player_ptr->special_action_set(dances[id - 1]);

    pager("$(special_dance_done_player)");
    sender("$(special_dance_done_log)", msg_log);

    return cmd_arg_ok;
}

command_arguments_rezult player_animation::cmd_special_handsup(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    if (player_ptr->vehicle_is_in()) {
        pager("$(player_not_on_foot)");
        return cmd_arg_process_error;
    }

    if (samp::api::SPECIAL_ACTION_PISSING == player_ptr->special_action_get()) {
        pager("$(player_pissing_handsup)");
        return cmd_arg_process_error;
    }

    player_ptr->special_action_set(samp::api::SPECIAL_ACTION_HANDSUP);

    pager("$(special_handsup_done_player)");
    sender("$(special_handsup_done_log)", msg_log);

    return cmd_arg_ok;
}

command_arguments_rezult player_animation::cmd_special_phone(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    if (player_ptr->vehicle_is_in()) {
        pager("$(player_not_on_foot)");
        return cmd_arg_process_error;
    }

    if (samp::api::SPECIAL_ACTION_PISSING == player_ptr->special_action_get()) {
        pager("$(player_pissing_phone)");
        return cmd_arg_process_error;
    }

    special_phone_start_impl(player_ptr, pager, sender);

    return cmd_arg_ok;
}

command_arguments_rezult player_animation::cmd_special_pissing(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    if (player_ptr->vehicle_is_in()) {
        pager("$(player_not_on_foot)");
        return cmd_arg_process_error;
    }

    player_ptr->special_action_set(samp::api::SPECIAL_ACTION_PISSING);

    pager("$(special_pissing_done_player)");
    sender("$(special_pissing_done_log)", msg_log);

    return cmd_arg_ok;
}

int player_animation::get_total_anims() const {
    int rezult = 0;
    BOOST_FOREACH(animation_libs_t::value_type const& item, animation_libs) {
        rezult += item.second.anim_names.size();
    }
    return rezult;
}

void player_animation::special_phone_start_impl(player_ptr_t const& player_ptr, messages_pager& pager, messages_sender const& sender) {
    player_ptr->special_action_set(samp::api::SPECIAL_ACTION_USECELLPHONE);
    pager("$(special_phone_start_done_player)");
    sender("$(special_phone_start_done_log)", msg_log);
}

void player_animation::special_phone_stop_impl(player_ptr_t const& player_ptr, messages_pager& pager, messages_sender const& sender) {
    player_ptr->special_action_set(samp::api::SPECIAL_ACTION_STOPUSECELLPHONE);
    pager("$(special_phone_stop_done_player)");
    sender("$(special_phone_stop_done_log)", msg_log);
}

command_arguments_rezult player_animation::cmd_special_drink(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    samp::api::special_action action;
    if (boost::iequals(arguments, params.process_all_vars("$(cmd_special_drink_param_beer)"), locale_ru)) {
        action = samp::api::SPECIAL_ACTION_DRINK_BEER;
        params("action", "beer");
    }
    else if (boost::iequals(arguments, params.process_all_vars("$(cmd_special_drink_param_wine)"), locale_ru)) {
        action = samp::api::SPECIAL_ACTION_DRINK_WINE;
        params("action", "wine");
    }
    else if (boost::iequals(arguments, params.process_all_vars("$(cmd_special_drink_param_sprunk)"), locale_ru)) {
        action = samp::api::SPECIAL_ACTION_DRINK_SPRUNK;
        params("action", "sprunk");
    }
    else {
        return cmd_arg_syntax_error;
    }

    params("action_to_player", boost::to_lower_copy(arguments, locale_ru));

    if (player_ptr->vehicle_is_in()) {
        pager("$(player_not_on_foot)");
        return cmd_arg_process_error;
    }

    player_ptr->special_action_set(action);

    pager("$(cmd_special_drink_done_player)");
    sender("$(cmd_special_drink_done_log)", msg_log);

    return cmd_arg_ok;
}

command_arguments_rezult player_animation::cmd_special_smoke(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    if (player_ptr->vehicle_is_in()) {
        pager("$(player_not_on_foot)");
        return cmd_arg_process_error;
    }

    player_ptr->special_action_set(samp::api::SPECIAL_ACTION_SMOKE_CIGGY);

    pager("$(special_smoke_done_player)");
    sender("$(special_smoke_done_log)", msg_log);

    return cmd_arg_ok;
}

player_animation_item::player_animation_item(player_animation& manager): manager(manager), is_first(true), is_can_preload(0) {
}

player_animation_item::~player_animation_item() {
}

void player_animation_item::preload_libs() {
    // Делаем предзагрузку библиотек - чтобы анимации игрались сразу
    int id = get_root()->get_id();
    samp::api::ptr api_ptr = samp::api::instance();
    BOOST_FOREACH(animation_libs_t::value_type const& item, manager.animation_libs) {
        api_ptr->apply_animation(id, item.second.lib_name, "null", 0.0f, 0, 0, 0, 0, 0, 0);
    }
}

void player_animation_item::on_request_class(int class_id) {
    if (is_first) {
        is_can_preload = 1;
        is_first = false;
    }
}

void player_animation_item::on_timer1000() {
    if (0 != is_can_preload) {
        if (3 == is_can_preload) {
            preload_libs();
            is_can_preload = 0;
        }
        else {
            ++is_can_preload;
        }
    }
}

void player_animation_item::on_keystate_change(int keys_new, int keys_old) {
    if (is_key_just_down(mouse_lbutton, keys_new, keys_old)) {
        if (get_root()->animation_is_use_phone()) {
            messages_item_paged msg_item(get_root());
            msg_item.get_params()("cmd_player_name", get_root()->name_get_full());

            manager.special_phone_stop_impl(get_root(), msg_item.get_pager(), msg_item.get_sender());
        }
    }
}

bool player_animation_item::on_update() {
    if (manager.is_block_on_jetpack) {
        if (samp::api::SPECIAL_ACTION_USEJETPACK == get_root()->special_action_get()) {
            get_root()->block("jetpack", "");
            return false;
        }
    }
    return true;
}
