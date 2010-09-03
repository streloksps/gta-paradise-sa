#include "config.hpp"
#include "player_spectate.hpp"
#include "player_spectate_i.hpp"
#include "core/module_c.hpp"
#include "core/container/container_handlers.hpp"
#include "game.hpp"
#include "player_connection.hpp"
#include "player_health.hpp"
#include <cassert>
#include <algorithm>
#include "weapons.hpp"

REGISTER_IN_APPLICATION(player_spectate);

player_spectate::ptr player_spectate::instance() {
    return application::instance()->get_item<player_spectate>();
}

player_spectate::player_spectate()
{
}

player_spectate::~player_spectate() {
}

void player_spectate::create() {
    keys_info.reset(new text_draw::td_item("$(spectate_keys_info_textdraw)"));
    command_add("spectate", std::tr1::bind(&player_spectate::cmd_spectate, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
}

void player_spectate::configure_pre() {
    keys_info_timeout = 5000;
}

void player_spectate::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(keys_info_timeout);
}

void player_spectate::configure_post() {
    keys_info->update();
}

void player_spectate::on_connect(player_ptr_t const& player_ptr) {
    player_spectate_item::ptr item(new player_spectate_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void player_spectate::on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        player_spectate_item::ptr item_player_spectate_ptr = item_player_ptr->get_item<player_spectate_item>();
        if (item_player_spectate_ptr->is_spectate_to(player_ptr)) {
            item_player_spectate_ptr->do_shift();
        }
    }
}

void player_spectate::refresh_spectate_by_dest(player_ptr_t const& changed_player_ptr) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        player_spectate_item::ptr item_player_spectate_ptr = item_player_ptr->get_item<player_spectate_item>();
        if (item_player_spectate_ptr->is_spectate_to(changed_player_ptr)) {
            item_player_spectate_ptr->do_refresh();
        }
    }
}


command_arguments_rezult player_spectate::cmd_spectate(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (arguments.empty()) {
        player::ptr spec_player_ptr = get_player_to_spectate(player_ptr);

        if (!spec_player_ptr) {
            pager("$(spectate_error_not_found)");
            return cmd_arg_process_error;
        }

        if (cmd_spectate_impl(player_ptr, spec_player_ptr, pager, sender, params)) {
            return cmd_arg_ok;
        }
    }
    else {
        int spec_player_id = -1;
        {
            std::istringstream iss(arguments);
            iss>>spec_player_id;
            if (iss.fail() || !iss.eof()) {
                return cmd_arg_syntax_error;
            }
        }

        player::ptr spec_player_ptr = players::instance()->get_player(spec_player_id);
        if (!spec_player_ptr) {
            params("player_id", spec_player_id);
            pager("$(player_bad_id)");
            return cmd_arg_process_error;
        }

        if (cmd_spectate_impl(player_ptr, spec_player_ptr, pager, sender, params)) {
            return cmd_arg_ok;
        }
    }
    return cmd_arg_process_error;
}

player_ptr_t player_spectate::get_player_to_spectate(player_ptr_t const& player_ptr) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        if (item_player_ptr != player_ptr) {
            return item_player_ptr;
        }
    }
    // Ничего не нашли
    return player_ptr_t();
}

bool player_spectate::cmd_spectate_impl(player_ptr_t const& player_ptr, player_ptr_t const& spec_player_ptr, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (spec_player_ptr == player_ptr) {
        pager("$(player_not_same)");
        return false;
    }

    if (player_ptr->vehicle_is_in()) {
        pager("$(player_not_on_foot)");
        return false;
    }

    player_ptr->get_item<player_spectate_item>()->do_spectate(spec_player_ptr);
    return true;
}

void player_spectate::on_pre_explode(float x, float y, float z) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        player_spectate_item::ptr item_player_spectate_ptr = item_player_ptr->get_item<player_spectate_item>();
        if (item_player_spectate_ptr->is_spectate()) {
            item_player_ptr->health_set_inf();
        }
    }
}

player_spectate_item::player_spectate_item(player_spectate& manager)
:manager(manager)
,is_ending(false)
{
}

player_spectate_item::~player_spectate_item() {
}

void player_spectate_item::do_spectate(player_ptr_t const& spec_player_ptr) {
    if (!curr_spec_player_ptr) {
        on_spectate_start(spec_player_ptr);
        get_root()->health_set_inf();
    }
    else if (curr_spec_player_ptr != spec_player_ptr) {
        on_spectate_change(spec_player_ptr);
        get_root()->health_set_inf();
    }

    is_ending = false;
    curr_spec_player_ptr = spec_player_ptr;
    get_root()->spectate_do(spec_player_ptr);
}

void player_spectate_item::on_gc_init() {
    /*
    messages_item msg_item;
    msg_item.get_params()("player_name_full", get_root()->name_get_full());
    msg_item.get_sender()("<log_section spectate/gc_init/>$(player_name_full)", msg_debug);
    */
}

void player_spectate_item::on_gc_fini() {
    is_ending = false;
    /*
    messages_item msg_item;
    msg_item.get_params()("player_name_full", get_root()->name_get_full());
    msg_item.get_sender()("<log_section spectate/gc_fini/>$(player_name_full)", msg_debug);
    */
}

bool player_spectate_item::is_spectate() const {
    return curr_spec_player_ptr;
}

player_ptr_t player_spectate_item::get_spectate_to() {
    return curr_spec_player_ptr;
}

bool player_spectate_item::is_spectate_ending() const {
    return is_ending;
}

bool player_spectate_item::is_spectate_to(player_ptr_t const& spec_player_ptr) const {
    return is_spectate() && curr_spec_player_ptr == spec_player_ptr;
}

void player_spectate_item::state_save() {
    // Сохраняем параметры, чтобы после респа после слежения полностью востановить состояние игрока
    player_ptr_t player_ptr = get_root();
	state_context = player_ptr->game_context_activate(std::tr1::dynamic_pointer_cast<game_context>(shared_from_this()));
	state_healht = player_ptr->health_get();
    state_pos = player_ptr->get_item<player_position_item>()->pos_get();
    state_money = player_ptr->get_item<player_money_item>()->get();
    state_weapons = player_ptr->get_item<weapons_item>()->weapon_get();
}

void player_spectate_item::state_preload() {
    // Предворительно востанавливаем состояние игрока
    //player::ptr player_ptr = get_root();

    //player_ptr->health_set(state_healht);
    //player_ptr->get_item<player_position_item>()->pos_set_ex(state_pos);
}

void player_spectate_item::state_load() {
    // Востанавливаем состояние игрока
    game::ptr game_ptr = game::instance();
    player::ptr player_ptr = get_root();

    player_ptr->health_set(state_healht);
    player_ptr->get_item<player_position_item>()->pos_set_ex(state_pos);
    
    player_ptr->get_item<player_money_item>()->reset();
    player_ptr->get_item<player_money_item>()->give(state_money);

    player_ptr->get_item<weapons_item>()->weapon_reset();
    player_ptr->get_item<weapons_item>()->weapon_add(state_weapons);

    player_ptr->weather_set(game_ptr->get_weather());
    player_ptr->time_set(game_ptr->get_time());

    player_ptr->game_context_activate_async(state_context);
    state_context.reset();
}


void player_spectate_item::on_disconnect(samp::player_disconnect_reason reason) {
    // Отключился следящий игрок
    if (is_spectate()) {
        do_stop_spec();
    }
}

void player_spectate_item::on_spawn() {
    if (get_gc_is_active() && state_context) {
        if (is_spectate()) {
            // Аварийно выходим из спека
            //do_stop_spec(true);
            // Обновляем слежку
            do_refresh();
        }
        else {
            state_load();
            on_spectate_spawn_after();
            is_ending = false;
        }
    }
}

void player_spectate_item::on_state_change(samp::player_state state_new, samp::player_state state_old) {
    if (state_old != state_new) {
        manager.refresh_spectate_by_dest(get_root());
    }
}

void player_spectate_item::on_interior_change(int interior_id_new, int interior_id_old) {
    if (interior_id_new != interior_id_old) {
        manager.refresh_spectate_by_dest(get_root());
    }
}

void player_spectate_item::on_keystate_change(int keys_new, int keys_old) {
    if (is_spectate()) {
        // Обрабатываем кнопки только тех, кто следит
        if (is_key_just_down(key_f, keys_new, keys_old)) {
            // Выходим из спека
            do_stop_spec();
        }
        else if (is_key_just_down(mouse_rbutton, keys_new, keys_old)) {
            // Переходим к следующему игроку
            do_shift(true);
        }
        else if (is_key_just_down(mouse_lbutton, keys_new, keys_old)) {
            // Переходим к предыдущему игроку
            do_shift(false);
        }
        //else if (is_key_just_down(key_alt, keys_new, keys_old)) {
        //    // Показываем гео по текущему игроку
        //    messages_item_paged msg_item(get_root());
        //    player_connection::cmd_geo_player_impl(get_root(), curr_spec_player_ptr, msg_item.get_pager(), msg_item.get_sender(), msg_item.get_params());
        //}
        else if (is_key_just_down(key_space, keys_new, keys_old)) {
            // Взрываем игрока
            messages_item_paged msg_item(get_root());
            player_health::instance()->cmd_check_impl(get_root(), curr_spec_player_ptr, msg_item.get_pager(), msg_item.get_sender(), msg_item.get_params(), player_health::check_explode);
        }
        else if (is_key_just_down(key_shift, keys_new, keys_old)) {
            // Взрываем игрока тихо
            messages_item_paged msg_item(get_root());
            player_health::instance()->cmd_check_impl(get_root(), curr_spec_player_ptr, msg_item.get_pager(), msg_item.get_sender(), msg_item.get_params(), player_health::check_silent);
        }
    }
}

void player_spectate_item::on_timer5000() {
    if (is_spectate()) {
        get_root()->health_set_inf();
    }
}

void player_spectate_item::do_stop_spec(bool is_error) {
    is_ending = true;
    get_root()->spectate_stop();
    curr_spec_player_ptr.reset();
    on_spectate_stop(is_error);
    state_preload();
}

void player_spectate_item::on_spectate_start(player_ptr_t const& spectated_player_ptr) {
    state_save();
    state_start = std::time(0);
    is_ending = false;

    messages_item msg_item;
    msg_item.get_params()
        ("player_name_full", get_root()->name_get_full())
        ("spectated_player_name_full", spectated_player_ptr->name_get_full());
    msg_item.get_sender()
        ("$(spectate_start_admins)", msg_players_all_admins)
        ("$(spectate_start_log)", msg_log);

    manager.keys_info->show_for_player(get_root(), manager.keys_info_timeout);
    container_execute_handlers(get_root(), &player_events::on_spectate_start_i::on_spectate_start, spectated_player_ptr);
}

void player_spectate_item::on_spectate_change(player_ptr_t const& new_spectated_player_ptr) {
    messages_item msg_item;
    msg_item.get_params()
        ("player_name_full", get_root()->name_get_full())
        ("spectated_player_name_full", new_spectated_player_ptr->name_get_full());
    msg_item.get_sender()
        ("$(spectate_change_log)", msg_log);

    container_execute_handlers(get_root(), &player_events::on_spectate_change_i::on_spectate_change, new_spectated_player_ptr);
}

void player_spectate_item::on_spectate_stop(bool is_error) {
    std::time_t spectate_time = std::time(0) - state_start;

    messages_item msg_item;
    msg_item.get_params()
        ("player_name_full", get_root()->name_get_full())
        ("spectate_time", (int)spectate_time);
    if (is_error) {
        msg_item.get_sender()
            ("$(spectate_stop_error_admins)", msg_players_all_admins)
            ("$(spectate_stop_error_log)", msg_log);
    }
    else {
        msg_item.get_sender()
            ("$(spectate_stop_ok_admins)", msg_players_all_admins)
            ("$(spectate_stop_ok_log)", msg_log);
    }

    manager.keys_info->hide_for_player(get_root());
    container_execute_handlers(get_root(), &player_events::on_spectate_stop_i::on_spectate_stop, spectate_time);
}

void player_spectate_item::on_spectate_spawn_after() {
    messages_item msg_item;
    msg_item.get_params()("player_name_full", get_root()->name_get_full());
    msg_item.get_sender()("$(spectate_spawn_after_log)", msg_log);
}

void player_spectate_item::do_refresh() {
    get_root()->spectate_do(curr_spec_player_ptr);
}

void player_spectate_item::do_shift(bool is_next) {
    // Переключаемся к следующему игроку, либо выходим из спека, если это не возможно
    players_t players = players::instance()->get_players();
    players_t::iterator it = std::find(players.begin(), players.end(), curr_spec_player_ptr);
    if (players.end() == it) {
        // Ошибка
        assert(false);
        do_stop_spec(true);
        return;
    }
    for (shift_it(it, players, is_next); *it != curr_spec_player_ptr; shift_it(it, players, is_next)) {
        if (get_root() != *it) {
            // Пропускаем себя
            do_spectate(*it);
            return;
        }
    }
    // Ничего не нашли - останавливаем спек
    do_stop_spec();
}

void player_spectate_item::shift_it(players_t::iterator& it, players_t& players, bool is_next) {
    if (1 >= players.size()) {
        return;
    }
    players_t::iterator last = players.end();
    --last;
    if (is_next) {
        if (last == it) {
            // it сейчас указывает на последний элемент
            it = players.begin();
        }
        else {
            ++it;
        }
    }
    else {
        if (players.begin() == it) {
            it = last;
        }
        else {
            --it;
        }
    }
}

void player_spectate_item::on_click_player(player_ptr_t const& clicked_player, samp::click_source source) {
    if (is_can_spectate()) {
        // Если мы можем за кем то следить и по нему кликнули - то переключаемся на него
        messages_item_paged msg_item(get_root());
        manager.cmd_spectate_impl(get_root(), clicked_player, msg_item.get_pager(), msg_item.get_sender(), msg_item.get_params());
    }
}

bool player_spectate_item::is_can_spectate() const {
    return get_root()->group_is_in("admin");
}

void player_spectate_item::on_player_stream_out(player_ptr_t const& player_ptr) {
    if (is_spectate_to(player_ptr)) {
        // Если игрок выподает из зоны видимости следящего, то слежка сбивается
        do_refresh();
    }
}
