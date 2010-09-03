#include "config.hpp"
#include "player_gamepause.hpp"
#include "core/module_c.hpp"
#include "core/utility/timestuff.hpp"
#include "player_spectate.hpp"

REGISTER_IN_APPLICATION(player_gamepause);

player_gamepause::ptr player_gamepause::instance() {
    return application::instance()->get_item<player_gamepause>();
}

player_gamepause::player_gamepause() {
}

player_gamepause::~player_gamepause() {
}

void player_gamepause::create() {
    command_add("player_sleep_list", bind(&player_gamepause::cmd_player_sleep_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
}

void player_gamepause::configure_pre() {
    is_active = false;
    min_detect_time = 10000;
    min_onleave_msg_time = 10000;
    prefixes.clear();
    progress_item = "-";
    progress_items_cout = 5;
    indicators.clear();
}

void player_gamepause::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(is_active);
    SERIALIZE_ITEM(min_detect_time);
    SERIALIZE_ITEM(min_onleave_msg_time);
    SERIALIZE_ITEM(prefixes);
    SERIALIZE_ITEM(progress_item);
    SERIALIZE_ITEM(progress_items_cout);
    SERIALIZE_ITEM(indicators);
}

void player_gamepause::configure_post() {
}

void player_gamepause::on_connect(player_ptr_t const& player_ptr) {
    player_gamepause_item::ptr item(new player_gamepause_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void player_gamepause::on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) {
    player_ptr->get_item<player_gamepause_item>()->process_exit();
}

command_arguments_rezult player_gamepause::cmd_player_sleep_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    typedef std::map<int, player::ptr> sleep_players_t;
    sleep_players_t sleep_players;

    { // Получаем сортированный список спящих игроков
        players_t players = players::instance()->get_players();
        BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
            player_gamepause_item::ptr player_gamepause_item_ptr = item_player_ptr->get_item<player_gamepause_item>();
            int sleep_time;
            if (player_gamepause_item_ptr->get_sleep_time(sleep_time)) {
                sleep_players.insert(std::make_pair(sleep_time, item_player_ptr));
            }
        }
    }

    // Список пуст
    if (sleep_players.empty()) {
        pager("$(cmd_player_sleep_list_empty)");
        return cmd_arg_auto;
    }

    params("count", sleep_players.size());
    pager.set_header_text(0, "$(cmd_player_sleep_list_header)");
    // Выводим спиоск игроку
    for (sleep_players_t::const_reverse_iterator it = sleep_players.rbegin(), end = sleep_players.rend(); end != it; ++it) {
        params
            ("player_name", it->second->name_get_full())
            ("time_text", get_time_text(it->first / 1000));
        pager.items_add("$(cmd_player_sleep_list_item)");
    }
    pager.items_done();

    return cmd_arg_auto;
}

player_gamepause_item::player_gamepause_item(player_gamepause& manager)
:manager(manager)
,progress_index(0)
{
}

player_gamepause_item::~player_gamepause_item() {
}

bool player_gamepause_item::on_update() {
    update_timer();
    return true;
}

void player_gamepause_item::on_spawn() {
    update_timer();
}

void player_gamepause_item::update_timer() {
    //time_update = boost::posix_time::microsec_clock::universal_time();
    time_update2 = time_base::tick_count_milliseconds();
}

void player_gamepause_item::on_timer500() {
    if (!manager.is_active) {
        return;
    }
    if (player_in_game == get_root()->get_state() && !get_root()->get_item<player_spectate_item>()->is_spectate()) {
        //int noupdate_time = static_cast<int>((boost::posix_time::microsec_clock::universal_time() - time_update).total_milliseconds());
        int noupdate_time = static_cast<int>(time_base::tick_count_milliseconds() - time_update2);
        if (noupdate_time >= manager.min_detect_time) {
            // Можем заходить/обнавлять спанье
            if (!sleep_start2) {
                do_sleep_enter();
            }
            do_sleep_update(noupdate_time);
        }
        else {
            if (sleep_start2) {
                // Если мы спим, то просыпаемся
                do_sleep_leave();
            }
        }
    }
}

void player_gamepause_item::do_sleep_enter() {
    messages_item msg_item;
    msg_item.get_params()("player_name", get_root()->name_get_full());
    msg_item.get_sender()("$(player_gamepause_enter_log)", msg_log);

    sleep_start2.reset(time_update2);
    progress_index = 0;
}

void player_gamepause_item::do_sleep_leave(bool is_exit) {
    int sleep_time;
    if (!get_sleep_time(sleep_time)) {
        return;
    }
    messages_item msg_item;
    msg_item.get_params()
        ("player_name", get_root()->name_get_full())
        ("time", sleep_time)
        ("time_text", get_time_text(sleep_time / 1000))
        ;

    if (is_exit) {
        msg_item.get_sender()("$(player_gamepause_leave_exit_log)", msg_log);
    }
    else {
        if (sleep_time > manager.min_onleave_msg_time) {
            msg_item.get_sender()
                ("$(player_gamepause_leave_bubble)", msg_player_bubble(get_root()))
                ("$(player_gamepause_leave_player)", msg_player(get_root()))
                ;
        }
        else {
            msg_item.get_sender()
                ("$(player_gamepause_leave_bubble_empty)", msg_player_bubble(get_root()))
                ;
        }
        msg_item.get_sender()("$(player_gamepause_leave_log)", msg_log);
    }
    sleep_start2.reset();
}

void player_gamepause_item::process_exit() {
    if (sleep_start2) {
        do_sleep_leave(true);
    }
}

void player_gamepause_item::do_sleep_update(int sleep_time) {
    assert(sleep_start2);
    if (manager.prefixes.empty()) {
        return;
    }
    messages_item msg_item;

    int time_min = 0;
    int time_max = 0x7FFFFFFF;
    std::string time_bubble;
    {
        bool is_find = false;
        for (player_gamepause::prefixes_t::const_iterator it = manager.prefixes.begin(), last = it, end = manager.prefixes.end(); end != it; ++it) {
            if (it->first > sleep_time) {
                if (last == it) {
                    time_min = 0;
                }
                else {
                    time_min = last->first;
                }
                time_max = it->first;
                time_bubble = it->second;
                is_find = true;
                break;
            }
            last = it;
        }
        if (!is_find) {
            player_gamepause::prefixes_t::const_iterator it = manager.prefixes.end();
            --it;
            time_min = it->first;
            time_bubble = it->second;
        }
    }
    
    std::string progress_str;
    {
        int progress_items_count = (manager.progress_items_cout * (sleep_time - time_min)) / (time_max - time_min);
        for (int i = 0; i < progress_items_count; ++i) {
            progress_str += manager.progress_item;
        }
    }

    std::string indicator_str;
    {
        if (!manager.indicators.empty()) {
            progress_index %= manager.indicators.size();
            indicator_str = manager.indicators[progress_index];
            ++progress_index;
        }
    }

    msg_item.get_params()
        ("bubble", time_bubble)
        ("progress", progress_str)
        ("indicator", indicator_str);
    
    msg_item.get_sender()("$(player_gamepause_bubble)", msg_player_bubble(get_root()));
}

bool player_gamepause_item::get_sleep_time(int& sleep_time) {
    if (sleep_start2) {
        //sleep_time = static_cast<int>((boost::posix_time::microsec_clock::universal_time() - sleep_start.get()).total_milliseconds());
        sleep_time = static_cast<int>(time_base::tick_count_milliseconds() - sleep_start2.get());
        return true;
    }
    return false;
}

bool player_gamepause_item::is_sleep() const {
    return sleep_start2;
}
