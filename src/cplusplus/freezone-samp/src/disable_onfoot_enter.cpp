#include "config.hpp"
#include "disable_onfoot_enter.hpp"
#include "core/module_c.hpp"
#include <vector>
#include <functional>
#include <algorithm>
#include "player_spectate.hpp"

REGISTER_IN_APPLICATION(disable_onfoot_enter);

disable_onfoot_enter::disable_onfoot_enter() {
}

disable_onfoot_enter::~disable_onfoot_enter() {
}

disable_onfoot_enter::ptr disable_onfoot_enter::instance() {
    return application::instance()->get_item<disable_onfoot_enter>();
}

void disable_onfoot_enter::configure_pre() {
}

void disable_onfoot_enter::configure(buffer::ptr const& buff, def_t const& def) {
    std::vector<disabled_zone_t> disabled_zones;
    SERIALIZE_ITEM(disabled_zones);

    disabled_zones_streamer.clear();
    disabled_zones_streamer.items_load(disabled_zones);
}

void disable_onfoot_enter::configure_post() {
}

void disable_onfoot_enter::on_connect(player_ptr_t const& player_ptr) {
    disable_onfoot_enter_item::ptr item(new disable_onfoot_enter_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

disable_onfoot_enter_item::disable_onfoot_enter_item(disable_onfoot_enter& manager):manager(manager) {
}

disable_onfoot_enter_item::~disable_onfoot_enter_item() {
}

void disable_onfoot_enter_item::on_keystate_change(int keys_new, int keys_old) {
    if (is_key_just_down(key_f, keys_new, keys_old)) {
        player_spectate_item::ptr player_spectate_item_ptr = get_root()->get_item<player_spectate_item>();
        if (player_spectate_item_ptr->is_spectate() || player_spectate_item_ptr->is_spectate_ending()) {
            // Не обрабатываем, если игрок следит или завершает слежение
            return;
        }

        // Нажали кнопку действия и не находиться в режиме слежки
        disable_onfoot_enter::disabled_zones_streamer_t::key_t key;
        if (manager.disabled_zones_streamer.item_get_by_pos(key, get_root()->get_item<player_position_item>()->pos_get())) {
            // Игрок делает что то запрещенное - убиваем его
            disabled_zone_t const& zone = manager.disabled_zones_streamer.item_get(key);
            messages_item msg_item;

            msg_item.get_params()
                ("player_name_full", get_root()->name_get_full())
                ("zone_name", zone.name)
                ("zone_msg", zone.msg)
                ;

            msg_item.get_sender()
                ("$(disable_onfoot_enter_done_player)", msg_player(get_root()))
                ("$(disable_onfoot_enter_done_log)", msg_log);

            get_root()->kill();
        }
    }
}
