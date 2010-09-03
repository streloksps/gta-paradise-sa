#include "config.hpp"
#include "player_health.hpp"
#include <functional>
#include "core/module_c.hpp"
#include "core/utility/locale_ru.hpp"
#include <boost/algorithm/string.hpp>
#include "player_spectate.hpp"
#include "core/utility/math.hpp"

REGISTER_IN_APPLICATION(player_health);

player_health::ptr player_health::instance() {
    return application::instance()->get_item<player_health>();
}

player_health::player_health() {
}

player_health::~player_health() {
}

void player_health::create() {
    command_add("suicide", &player_health::cmd_suicide);
    command_add("check", std::tr1::bind(&player_health::cmd_check, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
}

void player_health::configure_pre() {
    check_silent_vehicle = 300.0f;
    check_silent_player_h = 20.0f;
    check_silent_player_a = 20.0f;

    check_explode_player_h = 10.0f;

    check_random_vehicle_min = 10.0f;
    check_random_vehicle_max = 100.0f;

    check_random_player_min = 1.0f;
    check_random_player_max = 30.0f;

    counter.get_params().set(4000, 1, 1, 0, 0);
    script_death_reason = 255;
    script_death_player_count = 1;
}

void player_health::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(check_silent_vehicle);
    SERIALIZE_ITEM(check_silent_player_h);
    SERIALIZE_ITEM(check_silent_player_a);

    SERIALIZE_ITEM(check_explode_player_h);

    SERIALIZE_ITEM(check_random_vehicle_min);
    SERIALIZE_ITEM(check_random_vehicle_max);

    SERIALIZE_ITEM(check_random_player_min);
    SERIALIZE_ITEM(check_random_player_max);

    SERIALIZE_NAMED_ITEM(counter.get_params(), "counter_params");
    SERIALIZE_ITEM(script_death_reason);
    SERIALIZE_ITEM(script_death_player_count);
}

void player_health::configure_post() {
}

void player_health::on_connect(player_ptr_t const& player_ptr) {
    player_health_item::ptr item(new player_health_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void player_health::on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) {
    counter.erase(player_ptr);
}

command_arguments_rezult player_health::cmd_suicide(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    player_ptr->get_item<player_health_item>()->kill();
    pager("$(cmd_suicide_done_player)");
    sender("$(cmd_suicide_done_log)", msg_log);
    return cmd_arg_ok;
}

command_arguments_rezult player_health::cmd_check(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int check_player_id;
    check_type_e check_type = check_explode;
    { // Парсинг
        std::string flag;
        std::istringstream iss(arguments);
        iss>>flag>>check_player_id;
        if (!iss.fail() && iss.eof() && boost::iequals(flag, params.process_all_vars("$(cmd_check_param_silent)"), locale_ru)) {
            check_type = check_silent;
        }
        else if (!iss.fail() && iss.eof() && boost::iequals(flag, params.process_all_vars("$(cmd_check_param_random)"), locale_ru)) {
            check_type = check_random;
        }
        else {
            std::istringstream iss_2(arguments);
            iss_2>>check_player_id;
            if (iss_2.fail() || !iss_2.eof()) {
                return cmd_arg_syntax_error;
            }
        }
    }

    player::ptr check_player_ptr = players::instance()->get_player(check_player_id);

    if (!check_player_ptr) {
        params("player_id", check_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    if (cmd_check_impl(player_ptr, check_player_ptr, pager, sender, params, check_type)) {
        return cmd_arg_ok;
    }
    else {
        return cmd_arg_process_error;
    }
}

bool player_health::cmd_check_impl(player_ptr_t const& player_ptr, player_ptr_t const& explode_player_ptr, messages_pager& pager, messages_sender const& sender, messages_params& params, check_type_e check_type) {
    samp::api::ptr api_ptr = samp::api::instance();
    params
        ("admin_name", player_ptr->name_get_full())
        ("player_name", explode_player_ptr->name_get_full());

    if (counter.is_blocked(explode_player_ptr)) {
        pager("$(cmd_check_error_is_blocked)");
        return false;
    }

    if (explode_player_ptr->get_item<player_position_item>()->is_preload()) {
        pager("$(cmd_check_error_is_preload)");
        return false;
    }

    if (player_in_game != explode_player_ptr->get_state()) {
        pager("$(player_not_in_game_player)");
        return false;
    }

    if (explode_player_ptr->get_item<player_spectate_item>()->is_spectate()) {
        pager("$(player_is_spectate_player)");
        return false;
    }

    int vehicle_id = api_ptr->get_player_vehicle_id(explode_player_ptr->get_id());
    if (0 != vehicle_id) {
        // Вызрываем авто
        if (!explode_player_ptr->vehicle_is_driver()) {
            // Если игрок не водитель - то не взрываем его
            pager("$(cmd_check_error_not_driver)");
            return false;
        }

        if (check_explode == check_type) {
            api_ptr->set_vehicle_health(vehicle_id, 0.0f);
            sender
                ("$(cmd_check_explode_done_vehicle_admins)", msg_players_all_admins)
                ("$(cmd_check_explode_done_vehicle_log)", msg_log);

        }
        else {
            float decrease_health = check_silent_vehicle;
            if (check_random == check_type) {
                decrease_health = static_cast<float>(static_cast<int>(my_rand(check_random_vehicle_min, check_random_vehicle_max)*10.0f))/10.0f;
            }

            float vehicle_health = api_ptr->get_vehicle_health(vehicle_id);
            vehicle_health -= decrease_health;
            if (vehicle_health < 0.0f) vehicle_health = 0.0f;

            api_ptr->set_vehicle_health(vehicle_id, vehicle_health);

            params
                ("decrease_health", boost::format("%1$.1f") % decrease_health)
                ("vehicle_health", boost::format("%1$.1f") % vehicle_health);
            sender
                ("$(cmd_check_done_vehicle_admins)", msg_players_all_admins)
                ("$(cmd_check_done_vehicle_log)", msg_log);
        }
    }
    else {
        if (check_explode == check_type) {
            api_ptr->set_player_health(explode_player_ptr->get_id(), check_explode_player_h);
            sender
                ("$(cmd_check_explode_done_player_admins)", msg_players_all_admins)
                ("$(cmd_check_explode_done_player_log)", msg_log);
        }
        else {
            player_health_armour ha = explode_player_ptr->health_get();

            float decrease_health = check_silent_player_h;
            float decrease_armour = check_silent_player_a;

            if (check_random == check_type) {
                float decrease_val = static_cast<float>(static_cast<int>(my_rand(check_random_player_min, check_random_player_max)));
                if (0.0f == ha.armour) {
                    decrease_health = decrease_val;
                    decrease_armour = 0.0f;
                }
                else {
                    decrease_health = 0.0f;
                    decrease_armour = decrease_val;
                }
            }

            ha.health -= decrease_health;
            ha.armour -= decrease_armour;
            
            if (ha.health < 0.0f) ha.health = 0.0f;
            if (ha.armour < 0.0f) ha.armour = 0.0f;

            explode_player_ptr->health_set(ha);

            params
                ("decrease_health", boost::format("%1$.0f") % decrease_health)
                ("decrease_armour", boost::format("%1$.0f") % decrease_armour)
                ("health", boost::format("%1$.0f") % ha.health)
                ("armour", boost::format("%1$.0f") % ha.armour);

            sender
                ("$(cmd_check_done_player_admins)", msg_players_all_admins)
                ("$(cmd_check_done_player_log)", msg_log);
        }
    }

    // Если необходимо, делаем взрыв
    if (check_explode == check_type) {
        // Координаты для взрыва
        float x, y, z;
        api_ptr->get_player_pos(explode_player_ptr->get_id(), x, y, z);

        // Лечим всех, кто в спеке
        player_spectate::instance()->on_pre_explode(x, y, z);

        // Создаем взрыв
        api_ptr->create_explosion(x, y, z, 6, 2);
        api_ptr->create_explosion(x, y, z, 7, 2);
    }

    counter.increase(explode_player_ptr);
    return true;
}

player_health_item::player_health_item(player_health& manager)
:manager(manager)
,is_killed_by_script(false) 
,is_hide_death(false)
,is_spawned(false)
{
}

player_health_item::~player_health_item() {
}

void player_health_item::on_death(player_ptr_t const& killer_ptr, int reason) {
    if (!is_spawned) { // Игрок не соспавлен (умереть не может)
        messages_item msg_item;
        msg_item.get_params()
            ("player_name", get_root()->name_get_full())
            ("reason", reason)
            ;
        if (killer_ptr) {
            msg_item.get_params()("killer_name", killer_ptr->name_get_full());
            msg_item.get_sender()("$(on_death_error_killed_not_in_game)", msg_log);
        }
        else {
            msg_item.get_sender()("$(on_death_error_suicided_not_in_game)", msg_log);
        }
        return;
    }
    if (get_root()->get_item<player_spectate_item>()->is_spectate()) { // Игрок в режиме слежки - умереть не может
        messages_item msg_item;
        msg_item.get_params()
            ("player_name", get_root()->name_get_full())
            ("reason", reason)
            ;
        if (killer_ptr) {
            msg_item.get_params()("killer_name", killer_ptr->name_get_full());
            msg_item.get_sender()("$(on_death_error_killed_spectate)", msg_log);
        }
        else {
            msg_item.get_sender()("$(on_death_error_suicided_spectate)", msg_log);
        }
        return;
    }

    // Все проверки прошли
    is_spawned = false;

    if (is_killed_by_script) {
        if (is_hide_death) {
            // Не показываем смерить
        }
        else {
            process_suicide(reason, true);
        }
    }
    else {
        // Дописать проверку на расстояние между убитым и убийцей
        if (killer_ptr) {
            process_kill(killer_ptr, reason);
        }
        else {
            process_suicide(reason, false);
        }
    }
    is_killed_by_script = false;
}

void player_health_item::kill(bool is_hide_death) {
    player::ptr player_ptr = this->get_root();
    is_killed_by_script = true;
    this->is_hide_death = is_hide_death;
    player_ptr->kill();
}

void player_health_item::process_suicide(int reason, bool is_by_script) {
    // Заплатка с килл листом
    player::ptr player_ptr = this->get_root();

    // Посылаем сообщение в килллист
    if (is_by_script) {
        if (2 == manager.script_death_player_count) {
            player::send_death_message(player_ptr, player_ptr, manager.script_death_reason);
        }
        else {
            player::send_death_message(player_ptr, manager.script_death_reason);
        }
    }
    else {
        player::send_death_message(player_ptr, reason);
    }

    // Вызываем обработчики события суецида
    container_execute_handlers(player_ptr, &player_events::on_health_suicide_i::on_health_suicide, reason, is_by_script);

    { // Устанавливаем статус
        messages_item msg_item;
        msg_item.get_sender()("$(on_death_suicided_bubble)", msg_player_bubble(get_root()));
    }
}

void player_health_item::process_kill(player_ptr_t const& killer_ptr, int reason) {
    // Заплатка с килл листом
    player::ptr player_ptr = this->get_root();

    // Посылаем сообщение в килллист
    player::send_death_message(killer_ptr, player_ptr, reason);

    // Вызываем обработчики события суецида
    container_execute_handlers(player_ptr, &player_events::on_health_kill_i::on_health_kill, killer_ptr, reason);

    { // Устанавливаем статус
        messages_item msg_item;
        msg_item.get_params()("killer_name", killer_ptr->name_get_full());
        msg_item.get_sender()("$(on_death_killed_bubble)", msg_player_bubble(get_root()));
    }
}

void player_health_item::on_spawn() {
    if (!is_spawned) {
        { // Устанавливаем статус
            messages_item msg_item;
            msg_item.get_sender()("$(on_spawn_bubble)", msg_player_bubble(get_root()));
        }
        is_spawned = true;
    }
}
