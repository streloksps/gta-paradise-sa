#include "config.hpp"
#include "checkpoints.hpp"
#include "core/module_c.hpp"
#include "core/samp/samp_api.hpp"
#include "player_spectate.hpp"
#include <functional>
#include <algorithm>

REGISTER_IN_APPLICATION(checkpoints);

checkpoints::ptr checkpoints::instance() {
    return application::instance()->get_item<checkpoints>();
}

checkpoints::checkpoints() {
}

checkpoints::~checkpoints() {
}

void checkpoints::on_connect(player_ptr_t const& player_ptr) {
    checkpoints_item::ptr item(new checkpoints_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

int checkpoints::add(checkpoint_t const& checkpoint, checkpoint_event_t const& on_enter, checkpoint_event_t const& on_leave) {
    int id = checkpoint_streamer.item_add(checkpoint);
    if (static_cast<std::size_t>(id) >= checkpoints_data.size()) {
        checkpoints_data.resize(id + 1);
    }
    checkpoints_data[id] = checkpoint_data_t(on_enter, on_leave);
    on_update();
    return id;
}

void checkpoints::remove(int id) {
    checkpoint_streamer.item_delete(id);
    on_update();
}

int checkpoints::get_active_checkpoint(player_ptr_t const& player_ptr) const {
    return player_ptr->get_item<checkpoints_item>()->get_active_checkpoint();
}

void checkpoints::on_update() {
    players_execute_handlers(&checkpoints_item::on_update);
}

checkpoints_item::checkpoints_item(checkpoints& manager)
:manager(manager)
,checker(1.0f)
,active_checkpoint_id(checkpoints::invalid_checkpoint_id)
,is_in_checkpoint(false)
,is_ingame(false)
{
}

checkpoints_item::~checkpoints_item() {
}

void checkpoints_item::on_timer250() {
    process_update_pos();
}

void checkpoints_item::on_update() {
    checker.reset();
    process_update_pos();
}

void checkpoints_item::process_update_pos() {
    if (!is_ingame) {
        return;
    }
    pos3 pos = get_root()->get_item<player_position_item>()->pos_get();
    if (checker(pos)) {
        checkpoints::checkpoint_streamer_t::item_kyes_set_t keys;
        manager.checkpoint_streamer.items_get_by_pos(keys, pos, 1);
        int curr_checkpoint_id = keys.empty() ? checkpoints::invalid_checkpoint_id : *keys.begin();
        if (curr_checkpoint_id != active_checkpoint_id) {
            // Изменилась зона чикпоинта
            if (checkpoints::invalid_checkpoint_id == curr_checkpoint_id || checkpoints::invalid_checkpoint_id != active_checkpoint_id) {
                hide();
            }
            else {
                show(curr_checkpoint_id);
            }
        }
    }
}

void checkpoints_item::show(int id) {
    if (checkpoints::invalid_checkpoint_id == active_checkpoint_id) {
        active_checkpoint_id = id;
        checkpoint_t const& checkpoint = manager.checkpoint_streamer.item_get(active_checkpoint_id);
        samp::api::instance()->set_player_checkpoint(get_root()->get_id(), checkpoint.x, checkpoint.y, checkpoint.z, checkpoint.size);
    }
}

void checkpoints_item::hide() {
    if (checkpoints::invalid_checkpoint_id != active_checkpoint_id) {
        on_leave_checkpoint();
        active_checkpoint_id = checkpoints::invalid_checkpoint_id;
        samp::api::instance()->disable_player_checkpoint(get_root()->get_id());
        checker.reset();
    }
}

void checkpoints_item::on_enter_checkpoint() {
    if (checkpoints::invalid_checkpoint_id != active_checkpoint_id && !is_in_checkpoint && is_ingame && !get_root()->get_item<player_spectate_item>()->is_spectate()) {
        is_in_checkpoint = true;
        checkpoint_data_t const& checkpoint_data = manager.checkpoints_data[active_checkpoint_id];
        if (checkpoint_data.on_enter) {
            checkpoint_data.on_enter(std::tr1::cref(get_root()), active_checkpoint_id);
        }
    }
}

void checkpoints_item::on_leave_checkpoint() {
    if (checkpoints::invalid_checkpoint_id != active_checkpoint_id && is_in_checkpoint && is_ingame) {
        is_in_checkpoint = false;
        checkpoint_data_t const& checkpoint_data = manager.checkpoints_data[active_checkpoint_id];
        if (checkpoint_data.on_leave) {
            checkpoint_data.on_leave(std::tr1::cref(get_root()), active_checkpoint_id);
        }
    }
}

int checkpoints_item::get_active_checkpoint() const {
    if (is_in_checkpoint) {
        return active_checkpoint_id;
    }
    return checkpoints::invalid_checkpoint_id;
}

void checkpoints_item::on_spawn() {
    is_ingame = true;
}

void checkpoints_item::on_death_impl() {
    hide();
    is_ingame = false;
}

void checkpoints_item::on_health_suicide(int reason, bool is_by_script) {
    on_death_impl();
}

void checkpoints_item::on_health_kill(player_ptr_t const& killer_ptr, int reason) {
    on_death_impl();
}
