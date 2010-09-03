#include "config.hpp"
#include "player_free_for_all.hpp"
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <boost/foreach.hpp>
#include <boost/config.hpp>
#include <boost/version.hpp>

#include "core/module_c.hpp"
#include "game.hpp"
#include "weapons.hpp"
#include "core/utility/timestuff.hpp"
#include "server.hpp"

#include "mta10_loader.hpp"
#include "server_paths.hpp"
#include "player_classes.hpp"


REGISTER_IN_APPLICATION(player_free_for_all);

player_free_for_all::player_free_for_all(): spawn_money(0) {
}

player_free_for_all::~player_free_for_all() {
}

void player_free_for_all::create() {
    start_time = boost::posix_time::second_clock::local_time();

    command_add("version", bind(&player_free_for_all::cmd_version, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
    command_add_text("rules");
    command_add_text("help");
    command_add_text("server_about");
}

void player_free_for_all::on_connect(player_ptr_t const& player_ptr) {
    player_free_for_all_item::ptr item(new player_free_for_all_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
    player_ptr->game_context_activate(item);
}

void player_free_for_all::configure_pre() {
    spawn_money = 5000;
    spawn_weapons.clear();
    spawn_weapons_by_skins.clear();
    spawn_poses.clear();
    spawn_world_bounds = world_bounds_t(-5000.f, -5000.0f, 5000.0f, 5000.0f);
}

void player_free_for_all::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(spawn_money);
    SERIALIZE_ITEM(spawn_weapons);
    SERIALIZE_ITEM(spawn_weapons_by_skins);
    SERIALIZE_ITEM(spawn_poses);
    SERIALIZE_ITEM(spawn_world_bounds);

    { // Загружаем объекты из файлов MTA-SA 1.0
        mta10_loader loader(PATH_MTA10_DIR);
        mta10_loader_items_t loader_mta10;
        SERIALIZE_ITEM(loader_mta10);
        loader.load_items(loader_mta10, spawn_poses);
    }
}

void player_free_for_all::configure_post() {
    if (spawn_poses.empty()) {
        spawn_poses.push_back(pos4(1958.3783f, 1343.1572f, 15.3746f, 0, 0, 270.0f));
    }
}

command_arguments_rezult player_free_for_all::cmd_version(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    boost::posix_time::ptime curr_time = boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration dur = curr_time - start_time;

    params
        ("ver_load_time", boost::format("%1%") % start_time)
        ("ver_work_time", get_time_text(dur.total_seconds()) /*time_base::tick_count_milliseconds()*/)
        ("ver_curr_time", boost::format("%1%") % curr_time)
        ("ver_platform", BOOST_PLATFORM)
        ("ver_compiler", BOOST_COMPILER)
        ("ver_stdlib", BOOST_STDLIB)
        ("ver_boost", boost::format("Boost v%1%.%2%.%3%") % (BOOST_VERSION / 100000) % (BOOST_VERSION / 100 % 1000) % (BOOST_VERSION % 100))
        ("ver_stat", server::instance()->stat_get_string());

    pager.set_header_text(0, "$(cmd_version_header)")("$(cmd_version_text)");
    return cmd_arg_auto;
}

player_free_for_all_item::player_free_for_all_item(player_free_for_all& manager): manager(manager) {

}

player_free_for_all_item::~player_free_for_all_item() {

}

void player_free_for_all_item::on_spawn() {
    if (get_gc_is_active()) {
        game::ptr game_ptr = game::instance();
        player::ptr player_ptr = get_root();
        player_ptr->get_item<player_position_item>()->pos_set_ex(manager.spawn_poses[std::rand() % manager.spawn_poses.size()]);
        player_ptr->get_item<player_position_item>()->world_bounds_set(manager.spawn_world_bounds);

        player_ptr->get_item<player_money_item>()->reset();
        player_ptr->get_item<player_money_item>()->give(manager.spawn_money);

        {
            weapons_item::ptr weapons_item_ptr = player_ptr->get_item<weapons_item>();
            weapons_item_ptr->weapon_reset();
            weapons_item_ptr->weapon_add(manager.spawn_weapons);
            { // Даем оружие, специфичное для данного вида персонажа
                int skin_id = player_ptr->get_item<player_classes_item>()->get_class_def().skin_id;
                BOOST_FOREACH(player_free_for_all_weapons_by_skin_t const& weapons_by_skin, manager.spawn_weapons_by_skins) {
                    player_free_for_all_weapons_by_skin_t::skin_ids_t::const_iterator skin_it = weapons_by_skin.skin_ids.find(skin_id);
                    if (weapons_by_skin.skin_ids.end() != skin_it) {
                        weapons_item_ptr->weapon_add(weapons_by_skin.weapons);
                    }
                }
            }
        }

        player_ptr->weather_set(game_ptr->get_weather());
        player_ptr->time_set(game_ptr->get_time());

        messages_item msg_item;
        msg_item.get_params()("player_name_full", player_ptr->name_get_full());
        msg_item.get_sender()("$(player_free_for_all_spawn)", msg_log);
    }
}

void player_free_for_all_item::on_health_suicide(int reason, bool is_by_script) {
    player::ptr player_ptr = get_root();

    player_ptr->set_score(player_ptr->get_score() - 1);

    if (!is_by_script) {
        // Не пишем в лог, если убили скриптом игрока - всеравно там уже это написано
        messages_item msg_item;

        msg_item.get_params()
            ("player_name_full", player_ptr->name_get_full())
            ("reason", reason);

        msg_item.get_sender()
            ("$(health_suicide_log)", msg_log);
    }
}

void player_free_for_all_item::on_health_kill(player_ptr_t const& killer_ptr, int reason) {
    player::ptr player_ptr = get_root();
    messages_item msg_item;

    msg_item.get_params()
        ("player_name_full", player_ptr->name_get_full())
        ("killer_name_full", killer_ptr->name_get_full())
        ("reason", reason);

    msg_item.get_sender()
        ("$(health_kill_log)", msg_log);

    killer_ptr->set_score(killer_ptr->get_score() + 1);

    //Отдаем убийце деньги убитого
    int money = player_ptr->get_item<player_money_item>()->get();
    if (money > 0) {
        killer_ptr->get_item<player_money_item>()->give(money);
    }
}

void player_free_for_all_item::on_gc_init() {
}

void player_free_for_all_item::on_gc_fini() {
}

void player_free_for_all_item::on_timer15000() {
    if (player_in_game == get_root()->get_state()) {
        get_root()->weather_set(game::instance()->get_weather());
        get_root()->time_set(game::instance()->get_time());
    }
}
