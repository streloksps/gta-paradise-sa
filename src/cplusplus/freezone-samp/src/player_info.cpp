#include "config.hpp"
#include "player_info.hpp"
#include <cmath>
#include "core/module_c.hpp"
#include "core/utility/strings.hpp"
#include "player_color.hpp"
#include "player_money.hpp"
#include "player_position.hpp"
#include "player_gamepause.hpp"
#include "player_spectate.hpp"
#include "weapons.hpp"
#include "vehicles.hpp"
#include "core/samp/samp_hook_events_network.hpp"

REGISTER_IN_APPLICATION(player_info);

player_info::ptr player_info::instance() {
    return application::instance()->get_item<player_info>();
}

player_info::player_info() {

}

player_info::~player_info() {

}

void player_info::create() {
    server_ptr = server::instance();
    command_add("player_info", std::tr1::bind(&player_info::cmd_player_info, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
}

void player_info::configure_pre() {
    geo_max_str_len = 64;
}

void player_info::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(geo_max_str_len);
}

void player_info::configure_post() {
}

void player_info::on_connect(player_ptr_t const& player_ptr) {
    player_info_item::ptr item(new player_info_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void player_info::on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        if (item_player_ptr == player_ptr) {
            continue;
        }
        player_info_item::ptr player_info_item_ptr = item_player_ptr->get_item<player_info_item>();
        if (player_ptr == player_info_item_ptr->info_src && player_info_item_ptr->is_automanage) {
            player_info_item_ptr->do_stop();
        }
    }
}

command_arguments_rezult player_info::cmd_player_info(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    player_info_item::ptr player_info_item_ptr = player_ptr->get_item<player_info_item>();
    if (arguments.empty()) {
        if (player_info_item_ptr->is_active()) {
            player_info_item_ptr->do_stop();
            pager("$(cmd_player_info_stop_player)");
            sender("$(cmd_player_info_stop_log)", msg_log);
            return cmd_arg_ok;
        }
        else {
            player_info_item_ptr->do_start(player_ptr, true);
            pager("$(cmd_player_info_start_self_player)");
            sender("$(cmd_player_info_start_self_log)", msg_log);
            return cmd_arg_ok;
        }
    }
    unsigned int info_player_id;
    { // Парсинг
        std::istringstream iss(arguments);
        iss>>info_player_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    player::ptr info_player_ptr = players::instance()->get_player(info_player_id);
    if (!info_player_ptr) {
        params("player_id", info_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    player_info_item_ptr->do_start(info_player_ptr, true);
    params("info_player_name", info_player_ptr->name_get_full());
    pager("$(cmd_player_info_start_id_player)");
    sender("$(cmd_player_info_start_id_log)", msg_log);
    return cmd_arg_ok;
}

player_info_item::player_info_item(player_info& manager)
:manager(manager) 
,td(new text_draw::td_item("$(player_info_textdraw)"))
,is_automanage(false)
{
}

player_info_item::~player_info_item() {

}

void player_info_item::on_timer500() {
    if (!info_src) {
        return;
    }
    do_update();
}

void player_info_item::on_spectate_start(player_ptr_t const& spectated_player_ptr) {
    do_start(spectated_player_ptr, false);
}

void player_info_item::on_spectate_change(player_ptr_t const& new_spectated_player_ptr) {
    do_start(new_spectated_player_ptr, false);
}

void player_info_item::on_spectate_stop(std::time_t spectate_time) {
    do_stop();
}

void player_info_item::do_start(player_ptr_t const& player_ptr, bool is_manage_internal) {
    is_automanage = is_manage_internal;
    bool old_state = info_src;
    info_src = player_ptr;
    do_update();
    if (!old_state) {
        td->show_for_player(get_root());
    }
}

void player_info_item::do_stop() {
    if (info_src) {
        info_src.reset();
        td->hide_for_player(get_root());
    }
}

bool player_info_item::is_active() const {
    return info_src;
}

namespace {
    static std::string const weapon_names[] = {
        "Unarmed",                  // 0
        "Brass Knuckles",           // 1
        "Golf Club",                // 2
        "Night Stick",              // 3
        "Knife",                    // 4
        "Baseball Bat",             // 5
        "Shovel",                   // 6
        "Pool Cue",                 // 7
        "Katana",                   // 8
        "Chainsaw",                 // 9
        "Purple Dildo",             //10
        "White Dildo",              //11
        "Long White Dildo",         //12
        "White Dildo 2",            //13
        "Flowers",                  //14
        "Cane",                     //15
        "Grenades",                 //16
        "Tear Gas",                 //17
        "Molotovs",                 //18
        "-",                        //19
        "-",                        //20
        "-",                        //21
        "Pistol",                   //22
        "Silenced Pistol",          //23
        "Desert Eagle",             //24
        "Shotgun",                  //25
        "Sawn-Off Shotgun",         //26
        "Combat Shotgun",           //27
        "Micro Uzi",                //28
        "MP5",                      //29
        "AK47",                     //30
        "M4",                       //31
        "Tec9",                     //32
        "Rifle",                    //33
        "Sniper Rifle",             //34
        "RPG",                      //35
        "Missile Launch",           //36
        "Flame Thrower",            //37
        "Minigun",                  //38
        "Sachel Charges",           //39
        "Detonator",                //40
        "Spray Paint",              //41
        "Fire Exting",              //42
        "Camera",                   //43
        "NightvisionG",             //44
        "ThermalG",                 //45
        "Parachute"                 //46
    };

    static std::string const player_state_names[] = {
        "not_conected"
        ,"not_authorized"
        ,"not_in_game"
        ,"in_game"
        ,"not_spawn"
    };

    std::string get_weapons_str(weapons_t const& weapons, messages_params& params) {
        std::string rezult;
        BOOST_FOREACH(weapon_item_t const& item, weapons.items) {
            params
                ("weapon_name", get_enum_name<sizeof(weapon_names)/sizeof(weapon_names[0])>(item.id, weapon_names))
                ("weapon_count", item.ammo)
                ;
            if (weapons.active_id == item.id) {
                rezult += params.process_all_vars("$(player_info_weapon_active)");
            }
            else {
                rezult += params.process_all_vars("$(player_info_weapon_normal)");
            }
        }
        return rezult;
    }
} // namespace {

void player_info_item::do_update() {
    if (!info_src) {
        assert(false);
        return;
    }
    samp::api::ptr api_ptr = samp::api::instance();

    // Имя игрока
    td->get_params()
        ("player_name", info_src->name_get())
        ("player_id", info_src->get_id())
        ("player_color", info_src->get_item<player_color_item>()->get_color())
        ("player_color_index", info_src->get_item<player_color_item>()->get_color_index())
        ;


    // Жизнь игрока
    player_health_armour ha = info_src->health_get();
    td->get_params()
        ("player_health", ha.health)
        ("player_armour", ha.armour)
        ;

    // Счет игрока 
    td->get_params()
        ("player_score", info_src->get_score())
        ;
    
    // Транспорт
    int vehicle_id = api_ptr->get_player_vehicle_id(info_src->get_id());
    if (0 != vehicle_id && info_src->vehicle_is_driver()) {
        // Водитель
        float vehicle_health = api_ptr->get_vehicle_health(vehicle_id);
        td->get_params()
            ("player_info_vehicle_block")
            ("vehicle_health", boost::format("%1$.1f") % vehicle_health);
        
        td->get_params()
            ("vehicle_sys_name", "unknown")
            ("vehicle_name", "unknown")
            ;

        if (vehicle_ptr_t vehicle_ptr = vehicles::instance()->get_vehicle(vehicle_id)) {
            if (vehicle_def_cptr_t vehicle_def_cptr = vehicle_ptr->get_def()) {
                td->get_params()
                    ("vehicle_sys_name", vehicle_def_cptr->sys_name)
                    ("vehicle_name", vehicle_def_cptr->name)
                    ;
            }
        }
    }
    else {
        // Пешком или пасажир
        td->get_params()("player_info_vehicle_block", "");
    }

    { // Деньги
        int money_client = api_ptr->get_player_money(info_src->get_id());
        int money_server = info_src->get_item<player_money_item>()->get_server();

        td->get_params()
            ("money_client", money_client)
            ("money_server", money_server)
            ("player_info_money_sign_equal_block", "")
            ("player_info_money_sign_greater_block", "")
            ("player_info_money_sign_less_block", "")
            ;

        if (money_client > money_server) {
            td->get_params()("player_info_money_sign_greater_block");
        }
        else if (money_client < money_server) {
            td->get_params()("player_info_money_sign_less_block");
        }
        else {
            td->get_params()("player_info_money_sign_equal_block");
        }
    }

    {
        player_position_item::ptr player_position_item_ptr = info_src->get_item<player_position_item>();
        
        // Координата
        td->get_params()("position", boost::format("%1%") % player_position_item_ptr->pos_get());

        // Скорость пешком
        float speed = player_position_item_ptr->get_speed(true);
        if (0.0f != speed && !info_src->vehicle_is_in()) {
            td->get_params()
                ("player_info_speed_block")
                ("speed", boost::format("%1$.1f") % speed);
        }
        else {
            td->get_params()("player_info_speed_block", "");
        }
        
        // Скорость на транспорте
        float vehicle_speed = player_position_item_ptr->get_speed(false);
        if (0.0f != vehicle_speed && info_src->vehicle_is_in() && info_src->vehicle_is_driver()) {
            td->get_params()
                ("player_info_vehicle_speed_block")
                ("vehicle_speed", boost::format("%1$.1f") % vehicle_speed);
        }
        else {
            td->get_params()("player_info_vehicle_speed_block", "");
        }
    }

    // Сон
    if (info_src->get_item<player_gamepause_item>()->is_sleep()) {
        td->get_params()("player_info_sleep_block");
    }
    else {
        td->get_params()("player_info_sleep_block", "");
    }

    // Патроны
    weapons_t weapons = info_src->get_item<weapons_item>()->weapon_get();
    td->get_params()("weapons", get_weapons_str(weapons, td->get_params()));

    // Блок
    if (info_src->block_get()) {
        td->get_params()("player_info_is_block_block");
    }
    else {
        td->get_params()("player_info_is_block_block", "");
    }

    { // Слежка
        player_spectate_item::ptr player_spectate_item_ptr = info_src->get_item<player_spectate_item>();
        if (player_spectate_item_ptr->is_spectate()) {
            td->get_params()
                ("player_info_spectate_block")
                ("spec_player_name", player_spectate_item_ptr->get_spectate_to()->name_get())
                ("spec_player_id", player_spectate_item_ptr->get_spectate_to()->get_id());
        }
        else {
            td->get_params()("player_info_spectate_block", "");
        }
    }

    // Пинг
    td->get_params()("ping", api_ptr->get_player_ping(info_src->get_id()));

    { // Состояние игрока
        player_state state = info_src->get_state();
        if (player_in_game != state) {
            td->get_params()
                ("player_info_game_state_block")
                ("state", get_enum_name<sizeof(player_state_names)/sizeof(player_state_names[0])>(state, player_state_names));
        }
        else {
            td->get_params()("player_info_game_state_block", "");
        }
        
    }

    // Гео
    td->get_params()("geo", str_clip(text_draw::td_item::get_save_string(info_src->geo_get_long_desc()), manager.geo_max_str_len));

    // ркон админ
    if (info_src->is_rcon_admin()) {
        td->get_params()("player_info_is_rcon_admin_block");
    }
    else {
        td->get_params()("player_info_is_rcon_admin_block", "");
    }

    // Статистика
    {
        td->get_params()
            ("cpu_script_user", boost::format("%1$.1f") % (manager.server_ptr->stat_user_utility_get() * 100.0f))
            ("cpu_script_kernel", boost::format("%1$.1f") % (manager.server_ptr->stat_kernel_utility_get() * 100.0f))
            ("cpu_network_user", boost::format("%1$.1f") % (samp::profile_network_cpu.user_utility_get() * 100.0f))
            ("cpu_network_kernel", boost::format("%1$.1f") % (samp::profile_network_cpu.kernel_utility_get() * 100.0f))
            ("cpu_load_average1", boost::format("%1$.2f") % (manager.server_ptr->stat_load_average1_get()))
            ("cpu_load_average5", boost::format("%1$.2f") % (manager.server_ptr->stat_load_average5_get()))
            ("cpu_load_average15", boost::format("%1$.2f") % (manager.server_ptr->stat_load_average15_get()))
            ;
    }

    // Сериал
    td->get_params()("player_serial", info_src->get_serial());

    td->update();
}

void player_info_item::on_disconnect(samp::player_disconnect_reason reason) {
    if (info_src) {
        info_src.reset();
    }
}
