#include "config.hpp"
#include "players.hpp"
#include <functional>
#include <algorithm>
#include <memory>
#include <cassert>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "core/application.hpp"
#include "core/container/container_handlers.hpp"
#include "core/player/player.hpp"
#include "core/player/player_i.hpp"
#include "players_i.hpp"

REGISTER_IN_APPLICATION(players);

players::ptr players::instance() {
    return application::instance()->get_item<players>();
}

players::players() {
}

players::~players() {
}

bool players::is_valid_player_id(unsigned int id) {
    players_holder_t::iterator curr_player_ptr = players_holder.find(id);
    return players_holder.end() != curr_player_ptr;
}

player::ptr players::get_player(unsigned int id) {
    players_holder_t::iterator curr_player_ptr = players_holder.find(id);
    if (players_holder.end() != curr_player_ptr) {
        return curr_player_ptr->second;
    }
    return player::ptr();
}

players_t players::get_players() {
    players_t rezult;
    for (players_holder_t::iterator p = players_holder.begin(), end = players_holder.end(); p != end; ++p) {
        rezult.insert(rezult.end(), p->second);
    }
    return rezult;
}

std::size_t players::get_players_count() const {
    return players_holder.size();
}

std::size_t players::get_players_max() const {
    return samp::api::instance()->get_max_players();
}

void players::create() {
    api_ptr = samp::api::instance();
}

void players::on_player_connect(int player_id) {
    if (api_ptr->is_has_030_features() && api_ptr->is_player_npc(player_id)) {
        // Не создаем игрока для бота
        return;
    }
    player::ptr player_ptr(new player(player_id));
    std::pair<players_holder_t::iterator, bool> insert_rezult = players_holder.insert(std::make_pair(player_id, player_ptr));
    assert(insert_rezult.second && "Ошибка добавление игрока");
    if (container_execute_handlers(application::instance(), &players_events::on_pre_rejectable_connect_i::on_pre_rejectable_connect, player_ptr)
     && container_execute_handlers(application::instance(), &players_events::on_rejectable_connect_i::on_rejectable_connect, player_ptr)) {
        // Игрока разрешили создать
        container_execute_handlers(application::instance(), &players_events::on_pre_connect_i::on_pre_connect, player_ptr);
        container_execute_handlers(application::instance(), &players_events::on_connect_i::on_connect, player_ptr);
        container_execute_handlers(player_ptr, &player_events::on_connect_i::on_connect);
    }
    else {
        // Запрещено создание игрока - удаляем его из списка игроков. Больше никакие события на него не вызовутся
        players_holder.erase(insert_rezult.first);
        player_ptr->kick();
    }
}

void players::on_player_disconnect(int player_id, samp::player_disconnect_reason reason) {
    players_holder_t::iterator player_ptr_it = players_holder.find(player_id);
    if (players_holder.end() != player_ptr_it) {
        container_execute_handlers(player_ptr_it->second, &player_events::on_disconnect_i::on_disconnect, reason);
        container_execute_handlers(application::instance(), &players_events::on_disconnect_i::on_disconnect, player_ptr_it->second, reason);
        players_holder.erase(player_ptr_it);
    }
}

void players::on_player_spawn(int player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        if (player_ptr->block_get()) return;
        container_execute_handlers(player_ptr, &player_events::on_spawn_i::on_spawn);
    }
}

void players::on_player_death(int player_id, int killer_id, int reason) {
    if (player::ptr player_ptr = get_player(player_id)) {
        if (player_ptr->block_get()) return;
        container_execute_handlers(player_ptr, &player_events::on_death_i::on_death, get_player(killer_id), reason);
        container_execute_handlers(player_ptr, &player_events::on_death_post_i::on_death_post, get_player(killer_id), reason);
    }
}

void players::on_player_request_class(int player_id, int class_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_request_class_i::on_request_class, class_id);
    }
}

void players::on_player_enter_vehicle(int player_id, int vehicle_id, bool is_passenger) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_enter_vehicle_i::on_enter_vehicle, vehicle_id, is_passenger);
    }
}

void players::on_player_exit_vehicle(int player_id, int vehicle_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_exit_vehicle_i::on_exit_vehicle, vehicle_id);
    }
}

void players::on_player_state_change(int player_id, samp::player_state state_new, samp::player_state state_old) {
    if (player::ptr player_ptr = get_player(player_id)) {
        if (player_ptr->block_get()) return;
        if (samp::player_state_on_foot == state_new && player_in_game != player_ptr->get_state()) {
            // Игрок не был рожден, но состояние перешло в игровое - возможно небыл вызван OnSpawn
            container_execute_handlers(player_ptr, &player_events::on_spawn_i::on_spawn);
        }
        container_execute_handlers(player_ptr, &player_events::on_state_change_i::on_state_change, state_new, state_old);
    }
}

void players::on_player_enter_checkpoint(int player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_enter_checkpoint_i::on_enter_checkpoint);
    }
}

void players::on_player_leave_checkpoint(int player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_leave_checkpoint_i::on_leave_checkpoint);
    }
}

void players::on_player_enter_racecheckpoint(int player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_enter_racecheckpoint_i::on_enter_racecheckpoint);
    }
}

void players::on_player_leave_racecheckpoint(int player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_leave_racecheckpoint_i::on_leave_racecheckpoint);
    }
}

bool players::on_player_request_spawn(int player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        return container_execute_handlers(player_ptr, &player_events::on_request_spawn_i::on_request_spawn);
    }
    // Может быть ботом
    return true;
}

void players::on_player_pickuppickup(int player_id, int pickup_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_pickuppickup_i::on_pickuppickup, pickup_id);
    }
}

void players::on_player_interior_change(int player_id, int interior_id_new, int interior_id_old) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_interior_change_i::on_interior_change, interior_id_new, interior_id_old);
    }
}

void players::on_player_keystate_change(int player_id, int keys_new, int keys_old) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_keystate_change_i::on_keystate_change, keys_new, keys_old);
    }
}

bool players::on_player_update(int player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        if (player_ptr->block_get()) return false;
        return container_execute_handlers(player_ptr, &player_events::on_update_i::on_update);
    }
    // Может быть ботом
    return true;
}

void players::on_player_commandtext(int player_id, std::string const& cmd) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_commandtext_i::on_commandtext, cmd);
    }
}

void players::on_player_text(int player_id, std::string const& text) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_text_i::on_text, text);
    }
}

void players::on_player_private_msg(int player_id, int reciever_id, std::string const& text) {
    if (player::ptr player_ptr = get_player(player_id)) {
        if (player::ptr reciever_ptr = get_player(reciever_id)) {
            container_execute_handlers(player_ptr, &player_events::on_private_msg_i::on_private_msg, reciever_ptr, text);
        }
    }
}

void players::on_player_selected_menurow(int player_id, int row) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_selected_menurow_i::on_selected_menurow, row);
    }
}

void players::on_player_exited_menu(int player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        container_execute_handlers(player_ptr, &player_events::on_exited_menu_i::on_exited_menu);
    }
}

void players::on_player_click_player(int player_id, int clicked_player_id, samp::click_source source) {
    if (player::ptr player_ptr = get_player(player_id)) {
        if (player::ptr clicked_player_ptr = get_player(clicked_player_id)) {
            container_execute_handlers(player_ptr, &player_events::on_click_player_i::on_click_player, clicked_player_ptr, source);
        }
    }
}

void players::on_post_gamemode_exit(AMX* amx) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        on_player_disconnect(item_player_ptr->get_id(), samp::player_disconnect_reason_server_shutdown);
    }
}

void players::on_player_stream_in(int player_id, int for_player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        if (player::ptr for_player_ptr = get_player(for_player_id)) {
            container_execute_handlers(for_player_ptr, &player_events::on_player_stream_in_i::on_player_stream_in, player_ptr);
        }
    }
}

void players::on_player_stream_out(int player_id, int for_player_id) {
    if (player::ptr player_ptr = get_player(player_id)) {
        if (player::ptr for_player_ptr = get_player(for_player_id)) {
            container_execute_handlers(for_player_ptr, &player_events::on_player_stream_out_i::on_player_stream_out, player_ptr);
        }
    }
}

void players::on_vehicle_stream_in(int vehicle_id, int for_player_id) {
    if (player::ptr for_player_ptr = get_player(for_player_id)) {
        container_execute_handlers(for_player_ptr, &player_events::on_vehicle_stream_in_i::on_vehicle_stream_in, vehicle_id);
    }
}

void players::on_vehicle_stream_out(int vehicle_id, int for_player_id) {
    if (player::ptr for_player_ptr = get_player(for_player_id)) {
        container_execute_handlers(for_player_ptr, &player_events::on_vehicle_stream_out_i::on_vehicle_stream_out, vehicle_id);
    }
}

std::string players::get_players_names(players_t const& players_list) {
    if (players_list.empty()) {
        return "empty";
    }
    std::string rezult;
    BOOST_FOREACH(player::ptr const& player_ptr, players_list) {
        if (rezult.empty()) {
            rezult = (boost::format("%1%;") % player_ptr->name_get_full()).str();
        }
        else {
            rezult += (boost::format(" %1%;") % player_ptr->name_get_full()).str();
        }
    }
    return rezult;
}
