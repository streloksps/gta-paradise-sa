#include "config.hpp"
#include "weapons.hpp"
#include <iterator>
#include "core/module_c.hpp"
#include "core/samp/samp_api.hpp"
#include "pickups.hpp"
#include <boost/format.hpp>
#include <boost/foreach.hpp>

REGISTER_IN_APPLICATION(weapons);

weapons::ptr weapons::instance() {
    return application::instance()->get_item<weapons>();
}

weapons::weapons()
:pickups_visibility_radius(150.0f)
,invalid_id(-1, -1)
{
}

weapons::~weapons() {
    streamer.delete_item_event = std::tr1::bind(&weapons::streamer_item_deleter, this, std::tr1::placeholders::_1);
}

void weapons::create() {
    buy_text.reset(new text_draw::td_item("$(weapons_buy_textdraw)"));
}

void weapons::configure_pre() {
    weapons_can_get_ingame.clear();
    weapon_defs_pre_config.clear();
    weapon_defs_pre_config.swap(weapon_defs);
    buffer.update_begin();
    is_block_on_weapon_id = false;
}

void weapons::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(weapons_can_get_ingame);
    SERIALIZE_ITEM(weapon_defs);
    SERIALIZE_ITEM(is_block_on_weapon_id);
    SERIALIZE_NAMED_ITEM(buffer.update_get_data(), "buy_items");
}

void weapons::configure_post() {
    if (weapon_defs_pre_config != weapon_defs) {
        buffer.delete_all(std::tr1::bind(&weapons::buy_item_destroy, this, std::tr1::placeholders::_1));
    }
    weapon_defs_pre_config.clear();
    buffer.update_end(std::tr1::bind(&weapons::buy_item_create, this, std::tr1::placeholders::_1), std::tr1::bind(&weapons::buy_item_destroy, this, std::tr1::placeholders::_1));

    buy_text->update();
}

void weapons::on_connect(player_ptr_t const& player_ptr) {
    weapons_item::ptr item(new weapons_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

weapons::weapon_buy_key_t weapons::buy_item_create(weapon_buy_item_t const& weapon_buy_item) {
    weapon_defs_t::const_iterator def_it = weapon_defs.find(weapon_buy_item.def_name);
    if (weapon_defs.end() == def_it) {
        assert(false && "Не найдено описание оружия");
        return invalid_id;
    }
    return weapon_buy_key_t(
        pickups::instance()->streamer.item_add(pickup_t(weapon_buy_item.x,weapon_buy_item.y, weapon_buy_item.z, weapon_buy_item.interior, weapon_buy_item.world, pickups_visibility_radius, def_it->second.pickup_id, 1))
        ,streamer.item_add(weapon_buy_item)
        );
}

void weapons::buy_item_destroy(weapon_buy_key_t id) {
    if (invalid_id == id) {
        return;
    }
    pickups::instance()->streamer.item_delete(id.pickup_id);
    streamer.item_delete(id.streamer_id);
}

void weapons::streamer_item_deleter(int key) {
    players_execute_handlers(&weapons_item::delete_event, key);
}

weapons_item::weapons_item(weapons& manager): manager(manager) {
    player_can_have_weapons = manager.weapons_can_get_ingame;
}

weapons_item::~weapons_item() {

}

void weapons_item::on_timer250() {
    weapon_buy_player.update_begin();
    manager.streamer.items_get_by_pos(weapon_buy_player.update_get_data(), get_root()->pos_get(), 1);
    weapon_buy_player.update_end(std::tr1::bind(&weapons_item::on_enter, this, std::tr1::placeholders::_1), std::tr1::bind(&weapons_item::on_exit, this, std::tr1::placeholders::_1));
}

void weapons_item::delete_event(int icon_key) {
    weapon_buy_player.delete_obj_if_exist(icon_key, std::tr1::bind(&weapons_item::on_exit, this, std::tr1::placeholders::_1));
}

dynamic_dummy weapons_item::on_enter(int key_id) {
    weapon_buy_item_t const& item = manager.streamer.item_get(key_id);
    weapons::weapon_defs_t::const_iterator def_it = manager.weapon_defs.find(item.def_name);
    assert(manager.weapon_defs.end() != def_it);
    weapon_def_t const& def = def_it->second;
    messages_item msg_item;

    msg_item.get_params()
        ("ammo_count", def.sell_reload * def.ammo_reload)
        ("price", def.price)
        ("weapon_name", def.descripion)
        ;

    ;
    if (!get_root()->get_item<player_money_item>()->can_take(def.price)) {
        // У игрока нет денег
        msg_item.get_sender()("$(weapons_buy_msg_error_nomoney)", msg_player(get_root()));
    }
    else {
        // У игрока есть деньги
        msg_item.get_sender()("$(weapons_buy_msg_can)", msg_player(get_root()));
        manager.buy_text->show_for_player(get_root());
    }

    active_key_id.reset(key_id);
    is_msg_sended = false;
    return dynamic_dummy();
}

void weapons_item::on_exit(dynamic_dummy) {
    active_key_id.reset();
    manager.buy_text->hide_for_player(get_root());
}

void weapons_item::on_keystate_change(int keys_new, int keys_old) {
    if (is_key_just_down(KEY_LOOK_BEHIND, keys_new, keys_old) && active_key_id) {
        weapon_buy_item_t const& item = manager.streamer.item_get(active_key_id.get());
        weapons::weapon_defs_t::const_iterator def_it = manager.weapon_defs.find(item.def_name);
        assert(manager.weapon_defs.end() != def_it);
        weapon_def_t const& def = def_it->second;
        messages_item msg_item;

        msg_item.get_params()
            ("ammo_count", def.sell_reload * def.ammo_reload)
            ("price", def.price)
            ("weapon_def_name", item.def_name)
            ("weapon_name", def.descripion)
            ("player_name", get_root()->name_get_full())
            ;

        if (!get_root()->get_item<player_money_item>()->can_take(def.price)) {
            // У игрока нет денег
            msg_item.get_sender()("$(weapons_buy_error_nomoney)", msg_player(get_root()));
            samp::api::instance()->player_play_sound(get_root()->get_id(), sound_ammunation_buy_weapon_denied, 0.0f, 0.0f, 0.0f);
            manager.buy_text->hide_for_player(get_root());
            return;
        }

        // Продаем игроку оружие
        get_root()->get_item<player_money_item>()->take(def.price);
        weapon_add(weapon_item_t(def.id, def.sell_reload * def.ammo_reload));
        samp::api::instance()->player_play_sound(get_root()->get_id(), sound_ammunation_buy_weapon, 0.0f, 0.0f, 0.0f);
        
        if (!is_msg_sended) {
            // Чтобы небыло флуда в чате печатаем сообщение только 1 раз
            msg_item.get_sender()("$(weapons_buy_done)", msg_player(get_root()));
            is_msg_sended = true;
        }
        msg_item.get_sender()("$(weapons_buy_done_log)", msg_log);
    }
}

bool weapons_item::on_update() {
    if (manager.is_block_on_weapon_id) {
        if (player_in_game == get_root()->get_state()) {
            weapon_item_t curr_weapon_item = get_root()->weapon_get_current();
            if (player_can_have_weapons.end() == player_can_have_weapons.find(curr_weapon_item.id)) {
                // Нашли запрещенное оружие
                get_root()->block("weapon/id", (boost::format("%1%") % curr_weapon_item.id).str());
                return false;
            }
        }
    }
    return true;
}

void weapons_item::set_skills(weapon_skills_t& skills) {
    samp::api::ptr api_ptr = samp::api::instance();
    if (api_ptr->is_has_030_features()) {
        int player_id = get_root()->get_id();
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_PISTOL, skills.pistol);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_PISTOL_SILENCED, skills.pistol_silenced);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_DESERT_EAGLE, skills.desert_eagle);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_SHOTGUN, skills.shotgun);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_SAWNOFF_SHOTGUN, skills.sawnoff_shotgun);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_SPAS12_SHOTGUN, skills.spas12_shotgun);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_MICRO_UZI, skills.micro_uzi);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_MP5, skills.mp5);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_AK47, skills.ak47);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_M4, skills.m4);
        api_ptr->set_player_skill_level(player_id, samp::api::WEAPONSKILL_SNIPERRIFLE, skills.sniper_rifle);
    }
}

void weapons_item::weapon_reset() {
    get_root()->weapon_reset();
    player_can_have_weapons = manager.weapons_can_get_ingame;
}

void weapons_item::weapon_add(weapon_item_t const& weapon) {
    player_can_have_weapons.insert(weapon.id);
    get_root()->weapon_add(weapon);
}

weapons_t weapons_item::weapon_get() {
    return get_root()->weapon_get();
}

void weapons_item::weapon_add(weapons_t const& weapons) {
    std::transform(weapons.items.begin(), weapons.items.end(), std::inserter(player_can_have_weapons, player_can_have_weapons.end()), std::tr1::bind(&weapon_item_t::id, std::tr1::placeholders::_1));
    int active_weapon_ammo = -1;
    BOOST_FOREACH(weapon_item_t const& item, weapons.items) {
        if (weapons.active_id == item.id) {
            active_weapon_ammo = item.ammo;
        }
        else {
            get_root()->weapon_add(item);
        }
    }
    if (-1 != active_weapon_ammo) {
        get_root()->weapon_add(weapon_item_t(weapons.active_id, active_weapon_ammo));
    }
    //std::for_each(weapons.begin(), weapons.end(), std::tr1::bind(static_cast<void (weapons_item::*)(weapon_item_t const&)>(&weapons_item::weapon_add), this, std::tr1::placeholders::_1));
}
