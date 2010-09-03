#include "config.hpp"
#include "player_chat.hpp"
#include <functional>
#include <cstdlib>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/bind.hpp>
#include "core/application.hpp"
#include "core/command/command_adder.hpp"
#include "core/command/command_manager.hpp"
#include "core/utility/locale_ru.hpp"
#include "core/utility/erase_if.hpp"
#include "core/player/player.hpp"
#include "core/players.hpp"
#include "core/messages/messages_item.hpp"
#include "core/messages/player_messages/player_messages.hpp"
#include "rules.hpp"
#include "player_gamepause.hpp"
#include "player_position.hpp"

REGISTER_IN_APPLICATION(player_chat);

player_chat::ptr player_chat::instance() {
    return application::instance()->get_item<player_chat>();
}

player_chat::player_chat(): mute_time_max(0), mute_time_min(0), close_chat_radius(0.0f) {
}

player_chat::~player_chat() {
}

void player_chat::create() {
    command_add("pm", std::tr1::bind(&player_chat::cmd_pm, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);
    command_add("ignore", std::tr1::bind(&player_chat::cmd_ignore, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);
    command_add("ignore_list", std::tr1::bind(&player_chat::cmd_ignore_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);

    command_add("report", std::tr1::bind(&player_chat::cmd_report, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5),cmd_chat);

    command_add("admin_chat", std::tr1::bind(&player_chat::cmd_admin_chat, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);
    command_add("admin_say", std::tr1::bind(&player_chat::cmd_admin_say, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);

    command_add("mute", std::tr1::bind(&player_chat::cmd_mute, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);
    command_add("unmute", &player_chat::cmd_unmute, cmd_chat);
    command_add("mute_list", &player_chat::cmd_mute_list, cmd_info_only);

    command_add_text("comment_info");

    command_add("automute_list", std::tr1::bind(&player_chat::cmd_automute_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
    command_add("regexblock_list", std::tr1::bind(&player_chat::cmd_regexblock_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);

    command_add("group_chat_add", std::tr1::bind(&player_chat::cmd_group_chat_add, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);
    command_add("group_chat_list", std::tr1::bind(&player_chat::cmd_group_chat_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
    command_add("group_chat_list_all", std::tr1::bind(&player_chat::cmd_group_chat_list_all, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
    command_add("group_chat", std::tr1::bind(&player_chat::cmd_group_chat, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);

    command_add("chat_off", std::tr1::bind(&player_chat::cmd_chat_off, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);

    command_add("close_chat", std::tr1::bind(&player_chat::cmd_close_chat, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);

    command_add("pm_destination_set", std::tr1::bind(&player_chat::cmd_pm_destination_set, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);
    command_add("pm_destination_chat", std::tr1::bind(&player_chat::cmd_pm_destination_chat, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_chat);
}

void player_chat::configure_pre() {
    length_to_lower = 0;
    max_repeated_chars = 4;
    antiflood.get_params().set(3000, 1, 8, 1500, 30000);
    antiflood_shared.get_params().set(2000, 1, 8, 1500, 30000);

    is_process_input_text = true;

    mute_time_min = 5;
    mute_time_max = 3 * 60;
    
    auto_mute_words.clear();
    auto_mute_exec_frequency = 70;
    auto_mute_delay_min = 3;
    auto_mute_delay_max = 13;
    auto_mute_spliters.clear();
    
    regexs.clear();

    cmd_mute_def_time_minutes = 15;
    cmd_mute_def_reason = "п5";

    is_block_on_flood = false;
    block_on_flood_max_time = 1000;

    valid_chat_chars = "јаЅб¬в√гƒд≈е®Є∆ж«з»и…й кЋлћмЌнќоѕп–р—с“т”у‘ф’х÷ц„чЎшўщЏъџы№ьЁэёюя€AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890/.";

    group_chat_name_min = 3;
    group_chat_name_max = 16;

    close_chat_radius = 300.0f;
}

void player_chat::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(length_to_lower);
    SERIALIZE_ITEM(max_repeated_chars);
    SERIALIZE_NAMED_ITEM(antiflood.get_params(), "antiflood_params");
    SERIALIZE_NAMED_ITEM(antiflood_shared.get_params(), "antiflood_shared_params");    
    SERIALIZE_ITEM(is_process_input_text);

    SERIALIZE_ITEM(mute_time_min);
    SERIALIZE_ITEM(mute_time_max);

    SERIALIZE_ITEM(auto_mute_words);
    SERIALIZE_ITEM(auto_mute_exec_frequency);
    SERIALIZE_ITEM(auto_mute_delay_min);
    SERIALIZE_ITEM(auto_mute_delay_max);
    SERIALIZE_ITEM(auto_mute_spliters);

    SERIALIZE_ITEM(regexs);

    SERIALIZE_ITEM(cmd_mute_def_time_minutes);
    SERIALIZE_ITEM(cmd_mute_def_reason);

    SERIALIZE_ITEM(is_block_on_flood);
    SERIALIZE_ITEM(block_on_flood_max_time);

    SERIALIZE_ITEM(valid_chat_chars);

    SERIALIZE_ITEM(group_chat_name_min);
    SERIALIZE_ITEM(group_chat_name_max);

    SERIALIZE_ITEM(close_chat_radius);

    SERIALIZE_ITEM(automute_meaningless);
}

void player_chat::configure_post() {
    if (auto_mute_exec_frequency < 0 || auto_mute_exec_frequency > 100) {
        auto_mute_exec_frequency = 70;
    }
    if (mute_time_min <= 0) {
        mute_time_min = 5;
    }
    if (mute_time_max <= mute_time_min) {
        mute_time_max = mute_time_min + 60;
    }
    if (auto_mute_delay_min <= 0) {
        auto_mute_delay_min = 3;
    }
    if (auto_mute_delay_max <= auto_mute_delay_min) {
        auto_mute_delay_max = auto_mute_delay_min + 10;
    }
    if (auto_mute_spliters.empty()) {
        auto_mute_spliters = " ";
    }
    if (block_on_flood_max_time < 10) {
        block_on_flood_max_time = 10;
    }
}

void player_chat::on_connect(player_ptr_t const& player_ptr) {
    player_chat_item::ptr item(new player_chat_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void player_chat::on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) {
    antiflood.erase(player_ptr);
    antiflood_shared.erase(player_ptr);
}

command_arguments_rezult player_chat::cmd_pm(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    unsigned int dest_player_id;
    std::string text;
    { // ѕарсинг
        std::istringstream iss(arguments);
        iss>>dest_player_id;
        if (iss.fail() || iss.eof()) {
            return cmd_arg_syntax_error;
        }
        std::getline(iss, text);
        boost::trim(text);
        if (text.empty()) {
            return cmd_arg_syntax_error;
        }
    }
    
    player::ptr dest_player_ptr = players::instance()->get_player(dest_player_id);
    if (!dest_player_ptr) {
        params("player_id", dest_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    if (dest_player_ptr == player_ptr) {
        pager("$(player_not_same)");
        return cmd_arg_process_error;
    }
    
    if (do_send_pm_message(player_ptr, dest_player_ptr, text, sender, params)) {
        return cmd_arg_ok;
    }
    return cmd_arg_process_error;
}

command_arguments_rezult player_chat::cmd_pm_destination_set(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    unsigned int pm_destination_player_id;
    { // ѕарсинг
        std::istringstream iss(arguments);
        iss>>pm_destination_player_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    player::ptr pm_destination_player_ptr = players::instance()->get_player(pm_destination_player_id);
    if (!pm_destination_player_ptr) {
        params("player_id", pm_destination_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    if (pm_destination_player_ptr == player_ptr) {
        pager("$(player_not_same)");
        return cmd_arg_process_error;
    }

    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();
    player_chat_item_ptr->pm_destination = pm_destination_player_ptr;

    params("pm_destination_player", pm_destination_player_ptr->name_get_full());
    pager("$(cmd_pm_destination_set_done)");
    sender("$(cmd_pm_destination_set_done_log)", msg_log);

    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_pm_destination_chat(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();

    if (!player_chat_item_ptr->pm_destination) {
        pager("$(cmd_pm_destination_chat_error_no)");
        return cmd_arg_process_error;
    }

    if (do_send_pm_message(player_ptr, player_chat_item_ptr->pm_destination, arguments, sender, params)) {
        return cmd_arg_ok;
    }
    return cmd_arg_process_error;
}

command_arguments_rezult player_chat::cmd_report(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    unsigned int report_to_player_id;
    std::string text;
    { // ѕарсинг
        std::istringstream iss(arguments);
        iss>>report_to_player_id;
        if (iss.fail() || iss.eof()) {
            return cmd_arg_syntax_error;
        }
        std::getline(iss, text);
        boost::trim(text);
        if (text.empty()) {
            return cmd_arg_syntax_error;
        }
    }

    if (player_ptr->group_is_in("admin")) {
        pager("$(player_not_for_admin)");
        return cmd_arg_process_error;
    }

    player::ptr report_to_player_ptr = players::instance()->get_player(report_to_player_id);
    if (!report_to_player_ptr) {
        params("player_id", report_to_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    if (report_to_player_ptr == player_ptr) {
        pager("$(player_not_same)");
        return cmd_arg_process_error;
    }

    player_chat_item::ptr player_item_ptr = player_ptr->get_item<player_chat_item>();

    process_chat_text(text);

    if (text.empty()) {
        return cmd_arg_syntax_error;
    }

    if (!player_item_ptr->is_can_sent_msg("report", text)) {
        return cmd_arg_process_error;
    }

    params
        ("report_from_name", player_ptr->name_get_full())
        ("report_to_name", report_to_player_ptr->name_get_full())
        ("text", text);
    sender
        ("$(cmd_report_done_admins)", msg_players_all_admins)
        ("$(cmd_report_done_log)", msg_log);
    pager("$(cmd_report_done_player)");
    
    antiflood.increase(player_ptr);
    process_auto_mute_msg(player_ptr, text);
    automute_meaningless.automute_msg(player_ptr, text);

    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_ignore(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    unsigned int ignor_player_id;
    { // ѕарсинг
        std::istringstream iss(arguments);
        iss>>ignor_player_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    player::ptr ignor_player_ptr = players::instance()->get_player(ignor_player_id);
    if (!ignor_player_ptr) {
        params("player_id", ignor_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    if (ignor_player_ptr == player_ptr) {
        pager("$(player_not_same)");
        return cmd_arg_process_error;
    }

    // ¬ыполн€ем команду
    params("ignor_player_name", ignor_player_ptr->name_get_full());

    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();
    if (player_chat_item_ptr->ignore_list.end() == player_chat_item_ptr->ignore_list.find(ignor_player_ptr)) {
        // »грока пока нет в списке игнора
        player_chat_item_ptr->ignore_list.insert(ignor_player_ptr);
        pager("$(cmd_ignore_add)");
        sender("$(cmd_ignore_add_log)", msg_log);
    }
    else {
        // »грок уже в списке игнора
        player_chat_item_ptr->ignore_list.erase(ignor_player_ptr);
        pager("$(cmd_ignore_remove)");
        sender("$(cmd_ignore_remove_log)", msg_log);
    }

    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_ignore_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();
    pager.set_header_text(0, "$(cmd_ignore_list_header)");
    if (player_chat_item_ptr->ignore_list.empty()) {
        pager("$(cmd_ignore_list_empty)");
    }
    else {
        BOOST_FOREACH(player::ptr const& item_player_ptr, player_chat_item_ptr->ignore_list) {
            params("item_player_name", item_player_ptr->name_get_full());
            pager.items_add("$(cmd_ignore_list_item)");
        }
        pager.items_done();
    }
    return cmd_arg_auto;
}

bool player_chat::do_send_pm_message(player_ptr_t const& player_source_ptr, player_ptr_t const& player_dest_ptr, std::string const& text, messages_sender const& sender, messages_params& params) {
    player_chat_item::ptr player_chat_item_source_ptr = player_source_ptr->get_item<player_chat_item>();
    player_chat_item::ptr player_chat_item_dest_ptr = player_dest_ptr->get_item<player_chat_item>();
    std::string msg = text;
    process_chat_text(msg);

    if (player_chat_item_source_ptr->is_can_sent_msg("pm", msg)) {
        player_chat_item::deny_reasons reason;
        if (player_chat_item_dest_ptr->is_can_receive_msg_from(player_source_ptr, false, reason)) {
            params
                ("pm_msg", msg)
                ("pm_player_name_sender", player_source_ptr->name_get_full())
                ("pm_player_name_dest", player_dest_ptr->name_get_full());
            sender
                ("$(cmd_pm_msg_player)", msg_player(player_source_ptr))
                ("$(cmd_pm_msg)", msg_player(player_dest_ptr))
                ("$(cmd_pm_msg_log)", msg_log)
                ("$(cmd_pm_msg_admins)", msg_players_all_admins - msg_player(player_source_ptr) - msg_player(player_dest_ptr));
            antiflood.increase(player_source_ptr);
            return true;
        }
        else {
            player_chat_item_source_ptr->send_deny_to_msg(reason, player_dest_ptr);
        }
    }

    return false;
}

command_arguments_rezult player_chat::cmd_admin_chat(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    std::string message = arguments;
    process_chat_text(message);
    if (message.empty()) {
        return cmd_arg_syntax_error;
    }

    params
        ("admin_chat_msg", message)
        ("admin_chat_name_sender", player_ptr->name_get_full());
    sender
        ("$(cmd_admin_chat_admins)", msg_players_all_admins)
        ("$(cmd_admin_chat_log)", msg_log);
    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_admin_say(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    std::string message = arguments;
    process_chat_text(message);
    if (message.empty()) {
        return cmd_arg_syntax_error;
    }

    player_chat_item::ptr player_item_ptr = player_ptr->get_item<player_chat_item>();
    //if (!player_item_ptr->is_can_sent_msg()) {
    //    return cmd_arg_process_error;
    //}

    params
        ("admin_say_msg", message)
        ("admin_say_name_sender", player_ptr->name_get_full());
    sender
        ("$(cmd_admin_say_users)", msg_players_all_users)
        ("$(cmd_admin_say_admins)", msg_players_all_admins)
        ("$(cmd_admin_say_log)", msg_log);
    antiflood.increase(player_ptr);
    process_auto_mute_msg(player_ptr, message);
    automute_meaningless.automute_msg(player_ptr, message);

    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_mute(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int mute_player_id;
    int mute_time_minutes;
    std::string mute_reason;
    { // ѕарсинг
        std::istringstream iss(arguments);
        iss>>mute_player_id;
        if (iss.fail()) {
            return cmd_arg_syntax_error;
        }
        if (iss.eof() && 0 != cmd_mute_def_time_minutes) {
            mute_time_minutes = cmd_mute_def_time_minutes;
        }
        else {
            iss>>mute_time_minutes;
            if (iss.fail()) {
                return cmd_arg_syntax_error;
            }
        }

        if (iss.eof() && !cmd_mute_def_reason.empty()) {
            mute_reason = cmd_mute_def_reason;
        }
        else {
            std::getline(iss, mute_reason);
            boost::trim(mute_reason);
            if (mute_reason.empty()) {
                return cmd_arg_syntax_error;
            }
        }
    }
    process_chat_text(mute_reason);

    player::ptr mute_player_ptr = players::instance()->get_player(mute_player_id);
    if (!mute_player_ptr) {
        params("player_id", mute_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    params
        ("mute_admin_name", player_ptr->name_get_full())
        ("mute_player_name", mute_player_ptr->name_get_full())
        ("mute_time_minutes", mute_time_minutes)
        ("mute_reason", get_rules_string(mute_reason))
        ("mute_reason_raw", mute_reason);

    if (mute_time_min > mute_time_minutes || mute_time_max < mute_time_minutes) {
        params
            ("mute_time_min", mute_time_min)
            ("mute_time_max", mute_time_max);
        pager("$(cmd_mute_error_max_time)");
        return cmd_arg_process_error;
    }

    player_chat_item::ptr mute_player_ptr_item = mute_player_ptr->get_item<player_chat_item>();
    if (mute_player_ptr_item->get_is_chat_off()) {
        pager("$(cmd_mute_error_muted)");
        return cmd_arg_process_error;
    }

    sender
        ("$(cmd_mute_done_players)", msg_players_all_users)
        ("$(cmd_mute_done_admins)", msg_players_all_admins)
        ("$(cmd_mute_done_log)", msg_log);
    mute_player_ptr_item->do_chat_off(mute_time_minutes);
    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_unmute(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int unmute_player_id;
    { // ѕарсинг
        std::istringstream iss(arguments);
        iss>>unmute_player_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    player::ptr unmute_player_ptr = players::instance()->get_player(unmute_player_id);

    if (!unmute_player_ptr) {
        params("player_id", unmute_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    params
        ("unmute_admin_name", player_ptr->name_get_full())
        ("unmute_player_name", unmute_player_ptr->name_get_full());

    player_chat_item::ptr unmute_player_ptr_item = unmute_player_ptr->get_item<player_chat_item>();
    if (!unmute_player_ptr_item->get_is_chat_off()) {
        pager("$(cmd_unmute_error_unmuted)");
        return cmd_arg_process_error;
    }

    sender
        ("$(cmd_unmute_done_players)", msg_players_all_users)
        ("$(cmd_unmute_done_admins)", msg_players_all_admins)
        ("$(cmd_unmute_done_log)", msg_log);
    unmute_player_ptr_item->do_chat_on();

    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_mute_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int count = 0;

    pager.set_header_text(0, "$(cmd_mute_list_header)");
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        player_chat_item::ptr item_player_chat_ptr = item_player_ptr->get_item<player_chat_item>();
        if (item_player_chat_ptr->get_is_chat_off()) {
            ++count;
            params
                ("item_player_name", item_player_ptr->name_get_full())
                ("time_remain", item_player_chat_ptr->get_remain_chat_off_time());
            pager.items_add("$(cmd_mute_list_item)");
        }
    }
    if (0 == count) {
        pager.items_add("$(cmd_mute_list_empty)");
    }
    pager.items_done();

    return cmd_arg_auto;
}

command_arguments_rezult player_chat::cmd_automute_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    params
        ("items_count", auto_mute_words.size())
        ("auto_mute_exec_frequency", auto_mute_exec_frequency)
        ("auto_mute_delay_min", auto_mute_delay_min)
        ("auto_mute_delay_max", auto_mute_delay_max)
        ("auto_mute_spliters", auto_mute_spliters);

    pager.set_header_text(0, "$(cmd_automute_list_header)");
    BOOST_FOREACH(auto_mute_words_t::value_type const& item, auto_mute_words) {
        params("item_val", item.first)("item_penalty", item.second);
        pager.items_add("$(cmd_automute_list_item)");
    }
    pager.items_done();
    return cmd_arg_auto;
}

command_arguments_rezult player_chat::cmd_regexblock_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    pager.set_header_text(0, "$(cmd_regexblock_list_header)");
    for (int i = 0, size = regexs.size(); i < size; ++i) {
        params
            ("item_nn", i + 1)
            ("item_name", regexs[i].name)
            ("item_value", regexs[i].regex_str);
        pager("$(cmd_regexblock_list_item)");
    }
    return cmd_arg_auto;
}

void player_chat::process_auto_mute_msg(player_ptr_t const& player_ptr, std::string const& msg) {
    std::string mute_worlds;
    int mute_time = 0;
    for (boost::split_iterator<std::string::const_iterator> words_it = boost::make_split_iterator(msg, boost::token_finder(boost::is_any_of(auto_mute_spliters), boost::token_compress_on)), end; words_it != end; ++words_it) {
        auto_mute_words_t::iterator mute_it = auto_mute_words.find(boost::copy_range<std::string>(*words_it));
        if (auto_mute_words.end() != mute_it) {
            // Ќашли слово
            mute_time += mute_it->second;
            if (mute_worlds.empty()) {
                mute_worlds = mute_it->first;
            }
            else {
                mute_worlds += ", " + mute_it->first;
            }
        }
    }
    if (0 != mute_time) {
        // »грока можно мутить :)
        messages_item msg_item;
        msg_item.get_params()
            ("player_name", player_ptr->name_get_full())
            ("worlds", mute_worlds)
            ("total_time", mute_time)
            ("msg", msg);

        if ((std::rand() % 100) < auto_mute_exec_frequency) {
            // ћутить будем
            int mute_in_time = player_ptr->get_item<player_chat_item>()->automute_set(mute_time);
            msg_item.get_params()("mute_in_time", mute_in_time);
            msg_item.get_sender()
                ("$(automute_dictionary_to_mute_admins)", msg_players_all_admins)
                ("$(automute_dictionary_to_mute_log)", msg_log);
        }
        else {
            // ћутить не будем - пусть живет
            msg_item.get_sender()
                ("$(automute_dictionary_no_mute_admins)", msg_players_all_admins)
                ("$(automute_dictionary_no_mute_log)", msg_log);
        }
    }
}

bool player_chat::is_can_broadcast_msg(player_ptr_t const& player_ptr, std::string const& msg) {
    messages_item msg_item;
    bool rezult = true;

    msg_item.get_params()
        ("player_name_sender", player_ptr->name_get_full())
        ("msg", msg);

    if (player_ptr->block_get()) {
        msg_item.get_sender()("$(player_blocked_chat_log)", msg_log);
        return false;
    }

    std::tr1::smatch match;
    BOOST_FOREACH(string_regex_t const& regex, regexs) {
        if (std::tr1::regex_search(msg, match, regex.get_regex())) {
            rezult = false;
            msg_item.get_params()
                ("regex_name", regex.name)
                ("regex_match", match.str());
            break;
        }
    }

    if (!rezult) {
        msg_item.get_sender()
        ("$(player_chat_block_admins)", msg_players_all_admins)
        ("$(player_chat_block_log)", msg_log);
    }
    return rezult;
}

void player_chat::process_chat_text(std::string& text) const {
    if (is_process_input_text) {
        process_chat_text_unticaps(text);
        boost::find_format_all(text, boost::token_finder(boost::lambda::bind(std::logical_not<bool>(),boost::lambda::bind(boost::is_any_of(valid_chat_chars), boost::lambda::_1))), boost::empty_formatter("")); // ќставл€ем только разрешенные символы
        process_chat_text_delete_repeated_chars(text);
        boost::trim(text);
    }
}

void player_chat::process_chat_text_unticaps(std::string& text) const {
    if (text.length() > length_to_lower) {
        boost::to_lower(text, locale_ru);
    }
}

void player_chat::process_chat_text_delete_repeated_chars(std::string& text) const {
    char last_char = 0;
    std::size_t last_char_start_pos = 0;
    for (std::size_t pos = 0; pos < text.length(); ++pos) {
        if (text[pos] != last_char) {
            if (pos - last_char_start_pos > max_repeated_chars) {
                // ”дал€ем символы
                text.erase(last_char_start_pos, pos - last_char_start_pos - 1);
                pos -= pos - last_char_start_pos - 1;
            }
            last_char_start_pos = pos;
            last_char = text[pos];
        }
    }
    if (text.length() - last_char_start_pos > max_repeated_chars) {
        // ”дал€ем символы
        text.erase(last_char_start_pos + 1);
    }
}

command_arguments_rezult player_chat::cmd_group_chat_add(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();
    if (arguments.empty()) {
        // Ќет аргументов - выход из группы
        if (player_chat_item_ptr->group_chat_name.empty()) {
            pager("$(cmd_group_chat_error_empty)");
            return cmd_arg_process_error;
        }
        player_chat_item_ptr->group_chat_on_leave(false);
        player_chat_item_ptr->group_chat_name.clear();
        return cmd_arg_ok;
    }
    std::string group_chat_name = boost::to_lower_copy(arguments, locale_ru);
    params
        ("group_chat_name", group_chat_name)
        ("group_chat_name_min", group_chat_name_min)
        ("group_chat_name_max", group_chat_name_max)
        ;

    if (static_cast<int>(group_chat_name.length()) < group_chat_name_min || static_cast<int>(group_chat_name.length()) > group_chat_name_max) {
        pager("$(group_chat_add_error_length)");
        return cmd_arg_process_error;
    }

    if (group_chat_name == player_chat_item_ptr->group_chat_name) {
        pager("$(group_chat_add_error_in)");
        return cmd_arg_process_error;
    }

    if (!player_chat_item_ptr->group_chat_name.empty()) {
        player_chat_item_ptr->group_chat_on_leave(false);
    }
    player_chat_item_ptr->group_chat_name = group_chat_name;
    player_chat_item_ptr->group_chat_on_enter();
    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_group_chat_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();
    std::string group_chat_name = player_chat_item_ptr->group_chat_name;

    if (group_chat_name.empty()) {
        pager("$(cmd_group_chat_error_empty)");
        return cmd_arg_process_error;
    }

    players_t group_players = player_chat::group_chat_get_players_by_name(group_chat_name);
    params
        ("group_chat_name", group_chat_name)
        ("group_chat_count", group_players.size())
        ("group_chat_names", players::get_players_names(group_players));
    pager("$(cmd_group_chat_list_text)");

    return cmd_arg_auto;
}

command_arguments_rezult player_chat::cmd_group_chat_list_all(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    std::set<std::string, ilexicographical_less> group_chats;

    players_t players_list = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& player_ptr, players_list) {
        std::string group_chat_name = player_ptr->get_item<player_chat_item>()->group_chat_name;
        if (!group_chat_name.empty()) {
            group_chats.insert(group_chat_name);
        }
    }

    if (group_chats.empty()) {
        pager("$(cmd_group_chat_list_all_empty)");
        return cmd_arg_process_error;
    }

    params("chats_count", group_chats.size());
    pager.set_header_text(0, "$(cmd_group_chat_list_all_header)");
    BOOST_FOREACH(std::string const& group_chat_name, group_chats) {
        players_t group_players = player_chat::group_chat_get_players_by_name(group_chat_name);
        params
            ("group_chat_name", group_chat_name)
            ("group_chat_count", group_players.size())
            ("group_chat_names", players::get_players_names(group_players));
        pager("$(cmd_group_chat_list_all_item)");
    }
    return cmd_arg_auto;
}

command_arguments_rezult player_chat::cmd_group_chat(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();
    std::string group_chat_name = player_chat_item_ptr->group_chat_name;
    if (group_chat_name.empty()) {
        pager("$(cmd_group_chat_error_empty)");
        return cmd_arg_process_error;
    }
    params("group_chat_name", group_chat_name);

    players_t group_players = player_chat::group_chat_get_players_by_name(group_chat_name);

    if (group_players.size() < 2) {
        pager("$(cmd_group_chat_error_low)");
        return cmd_arg_process_error;
    }

    std::string message = arguments;
    process_chat_text(message);

    if (message.empty()) {
        return cmd_arg_syntax_error;
    }

    if (!player_chat_item_ptr->is_can_sent_msg("group", message)) {
        return cmd_arg_process_error;
    }

    // ”бираем из списка назначени€ игроков, которые не могут получать сообщени€
    player_chat_item::deny_reasons reason;
    erase_if(group_players, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&player_chat_item::is_can_receive_msg_from_st, std::tr1::placeholders::_1, player_ptr, true, std::tr1::ref(reason))));

    params
        ("msg", message);

    sender
        ("$(cmd_group_chat_msg)", msg_players(group_players))
        ("$(cmd_group_chat_msg_admins)", msg_players_all_admins - msg_players(group_players))
        ("$(cmd_group_chat_msg_log)", msg_log);

    antiflood.increase(player_ptr);
    process_auto_mute_msg(player_ptr, message);
    automute_meaningless.automute_msg(player_ptr, message);
    return cmd_arg_ok;
}

players_t player_chat::group_chat_get_players_by_name(std::string const& group_chat_name) {
    players_t rezult;
    players_t players_list = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& player_ptr, players_list) {
        if (group_chat_name == player_ptr->get_item<player_chat_item>()->group_chat_name) {
            rezult.insert(player_ptr);
        }
    }
    return rezult;
}

command_arguments_rezult player_chat::cmd_chat_off(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    if (player_ptr->group_is_in("admin")) {
        pager("$(cmd_chat_off_error_admin)");
        return cmd_arg_process_error;
    }

    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();
    if (player_chat_item_ptr->get_is_chat_off()) {
        pager("$(player_chat_deny_is_chat_off)");
        return cmd_arg_process_error;
    }

    player_chat_item_ptr->is_player_self_chat_off = !player_chat_item_ptr->is_player_self_chat_off;
    
    if (player_chat_item_ptr->is_player_self_chat_off) {
        // »грок отключил себе чат
        pager("$(cmd_chat_off_trun_off)");
        sender("$(cmd_chat_off_trun_off_log)", msg_log);
    }
    else {
        // »грок включил себе чат
        pager("$(cmd_chat_off_trun_on)");
        sender("$(cmd_chat_off_trun_on_log)", msg_log);
    }
    return cmd_arg_ok;
}

command_arguments_rezult player_chat::cmd_close_chat(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    player_chat_item::ptr player_chat_item_ptr = player_ptr->get_item<player_chat_item>();

    std::string message = arguments;
    process_chat_text(message);

    if (message.empty()) {
        return cmd_arg_syntax_error;
    }

    if (!player_chat_item_ptr->is_can_sent_msg("close", message)) {
        return cmd_arg_process_error;
    }

    player_chat_item::broadcast_lines_t lines = player_chat_item_ptr->make_broadcast_lines(message);

    if (lines.empty()) {
        return cmd_arg_syntax_error;
    }

    if (is_can_broadcast_msg(player_ptr, message)) {
        player_chat_item_ptr->send_close_chat_lines(lines);
        player_chat_item_ptr->send_close_chat_text(message);

        {
            messages_item msg_item;
            msg_item.get_params()
                ("player_name_sender", player_ptr->name_get_full())
                ("msg", message);
            msg_item.get_sender()("$(player_close_chat_log)", msg_log);
        }

    }
    else {
        // ќтсылыем сообщение только отправителю
        player_chat_item_ptr->send_only_src_lines(lines);
    }

    antiflood.increase(player_ptr);
    if (lines.size() > 1) {
        // «а вторую строчку даем пинальти автоматом
        antiflood.increase(player_ptr);
    }
    process_auto_mute_msg(player_ptr, message);
    automute_meaningless.automute_msg(player_ptr, message);
    return cmd_arg_ok;
}

player_chat_item::player_chat_item(player_chat& manager)
:manager(manager)
,is_in_game(false)
,is_chat_off(false)
,automute_is_active(false)
,automute_time(0)
,automute_minutes(0)
,is_player_self_chat_off(false)
{
}

player_chat_item::~player_chat_item() {
}

void player_chat_item::on_spawn() {
    if (!is_in_game) {
        // ѕервый спавн - игрок вошел в игру - теперь может получать сообщени€
        is_in_game = true;
    }
}

void player_chat_item::on_text(std::string const& text) {
    if (command_manager::instance()->it_is_command(text)) {
        // Ќе обрабатываем команды
        return;
    }
    player_ptr_t player_ptr = get_root();

    std::string msg = text;
    manager.process_chat_text(msg);

    if (is_can_sent_msg("shared", msg)) {
        broadcast_lines_t lines = make_broadcast_lines(msg);
        if (!lines.empty()) {
            if (get_is_player_self_chat_off()) {
                // »грок отключил себе чат и не может писать в общий чат
                messages_item msg_item;

                msg_item.get_params()
                    ("player_name_sender", player_ptr->name_get_full())
                    ("msg", msg);

                msg_item.get_sender()
                    ("$(cmd_chat_off_error_msg)", msg_player(player_ptr))
                    ("$(cmd_chat_off_error_msg_log)", msg_log);
                return;
            }

            { // –аботаем с антифлудом на общий чат
                if (manager.antiflood_shared.is_blocked(player_ptr)) {
                    // —читаем еще пинальти
                    manager.antiflood_shared.increase(player_ptr);
                    int flood_time_out = (int)(manager.antiflood_shared.get_time_out(player_ptr) / 1000);
                    if (manager.is_block_on_flood && !player_ptr->block_get()) {
                        if (flood_time_out > manager.block_on_flood_max_time) {
                            player_ptr->block("flood");
                        }
                    }

                    messages_item msg_item;
                    msg_item.get_params()
                        ("player_name", player_ptr->name_get_full())
                        ("msg", msg)
                        ("time_out", flood_time_out)
                        ;

                    msg_item.get_sender()
                        ("$(player_chat_deny_shared_is_flood)", msg_player(player_ptr))
                        ("$(player_chat_deny_shared_is_flood_log)", msg_log);
                    return;
                }
            }

            if (manager.is_can_broadcast_msg(player_ptr, msg)) {
                // ќтсылаем сообщение всем
                send_broadcast_lines(lines);
                send_broadcast_text(msg);

                {
                    messages_item msg_item;
                    msg_item.get_params()
                        ("player_name_sender", player_ptr->name_get_full())
                        ("msg", msg);
                    msg_item.get_sender()("$(player_chat_log)", msg_log);
                }
            }
            else {
                // ќтсылыем сообщение только отправителю
                send_only_src_lines(lines);
            }

            manager.antiflood_shared.increase(player_ptr);
            manager.antiflood.increase(player_ptr);
            if (lines.size() > 1) {
                // «а вторую строчку даем пинальти автоматом
                manager.antiflood_shared.increase(player_ptr);
                manager.antiflood.increase(player_ptr);
            }
            manager.process_auto_mute_msg(player_ptr, msg);
            manager.automute_meaningless.automute_msg(player_ptr, msg);
        }
    }
}

void player_chat_item::on_timer1000() {
    automute_process();
}

void player_chat_item::on_timer5000() {
    do_recalc_chat();
}

player_chat_item::broadcast_lines_t player_chat_item::make_broadcast_lines(std::string const& msg) {
    broadcast_lines_t rezult;
    if (msg.length() <= broadcast_msg_max_len) {
        // —ообщение влезает на 1 строчку
        if (!msg.empty()) {
            rezult.push_back(msg);
        }
    }
    else {
        // —ообщение на одну строчку невлезает
        std::size_t pos = msg.rfind(' ', broadcast_msg_max_len);
        if (std::string::npos != pos && msg.length() - pos - 1 <= broadcast_msg_max_len) {
            // оставша€с€ часть влазиет на 1 строчку - режем по пробелу
            rezult.push_back(msg.substr(0, pos));
            rezult.push_back(msg.substr(pos+1));
        }
        else {
            rezult.push_back(msg.substr(0, broadcast_msg_max_len));
            rezult.push_back(msg.substr(broadcast_msg_max_len));
        }
    }
    return rezult;
}

void player_chat_item::send_broadcast_lines(broadcast_lines_t const& lines) {
    players_t players = players::instance()->get_players();
    player::ptr const me = get_root();
    deny_reasons reason;

    BOOST_FOREACH(player::ptr const& player_ptr, players) {
        ptr player_chat_ptr = player_ptr->get_item<player_chat_item>();
        if (player_chat_ptr->is_can_receive_msg_from(me, true, reason) && !player_chat_ptr->get_is_player_self_chat_off()) {
            // ћы можем отослать сообщение игроку
            BOOST_FOREACH(std::string const& text, lines) {
                player_messages::send_player_message(player_ptr, me, text);
            }
        }
    }
    player_messages::send_player_message_restore(me);
}

// ќтсылаем всем близлежащим игрокам и админам
void player_chat_item::send_close_chat_lines(broadcast_lines_t const& lines) {
    players_t players = players::instance()->get_players();
    player::ptr const me = get_root();
    deny_reasons reason;
    pos4 me_pos = me->get_item<player_position_item>()->pos_get();

    BOOST_FOREACH(player::ptr const& player_ptr, players) {
        ptr player_chat_ptr = player_ptr->get_item<player_chat_item>();
        if (player_chat_ptr->is_can_receive_msg_from(me, true, reason)) {
            if (player_ptr->group_is_in("admin")
                || (me == player_ptr || is_points_in_sphere(player_ptr->get_item<player_position_item>()->pos_get(), me_pos, manager.close_chat_radius))) {
                // ≈сли игрок админ или он близко - получает сообщение
                BOOST_FOREACH(std::string const& text, lines) {
                    player_messages::send_player_message(player_ptr, me, text, true);
                }
            }
        }
    }
    player_messages::send_player_message_restore(me);
}

// ѕоказывает чат баблу чата
void player_chat_item::send_broadcast_text(std::string const& msg) {
    if (samp::api::instance()->is_has_030_features()) {
        // ¬ сампе 0.3 отсылаем еще чат баблу
        messages_item msg_item;
        msg_item.get_params()("msg", msg);
        msg_item.get_sender()("$(player_chat_bubble)", msg_player_bubble(get_root()));
    }
}

// ѕоказывает чат баблу дл€ ближнего чата
void player_chat_item::send_close_chat_text(std::string const& msg) {
    if (samp::api::instance()->is_has_030_features()) {
        // ¬ сампе 0.3 отсылаем еще чат баблу
        messages_item msg_item;
        msg_item.get_params()("msg", msg);
        msg_item.get_sender()("$(player_close_chat_bubble)", msg_player_bubble(get_root()));
    }
}

void player_chat_item::send_only_src_lines(broadcast_lines_t const& lines) {
    deny_reasons reason;
    player::ptr me = get_root();
    if (is_can_receive_msg_from(me, true, reason)) {
        // ћы можем отослать сообщение игроку
        BOOST_FOREACH(std::string const& text, lines) {
            player_messages::send_player_message(me, me, text);
        }
    }
    player_messages::send_player_message_restore(me);
}

void player_chat_item::send_deny_to_msg(deny_reasons reason, player_ptr_t const& player_dest_ptr) {
    messages_item msg_item;
    msg_item.get_params()("player_name", get_root()->name_get_full());
    msg_item.get_params()("pm_to_player_name", player_dest_ptr->name_get_full());
    if (deny_not_in_game == reason) {
        msg_item.get_sender()
            ("$(player_chat_deny_to_not_in_game)", msg_player(get_root()))
            ("$(player_chat_deny_to_not_in_game_log)", msg_log);
    }
    else if (deny_is_chat_off == reason) {
        msg_item.get_sender()
            ("$(player_chat_deny_to_is_chat_off)", msg_player(get_root()))
            ("$(player_chat_deny_to_is_chat_off_log)", msg_log);
    }
    else if (deny_is_ignored == reason) {
        msg_item.get_sender()
            ("$(player_chat_deny_to_is_ignored)", msg_player(get_root()))
            ("$(player_chat_deny_to_is_ignored_log)", msg_log);
    }
    else if (deny_is_sleep == reason) {
        msg_item.get_sender()
            ("$(player_chat_deny_to_sleep)", msg_player(get_root()))
            ("$(player_chat_deny_to_sleep_log)", msg_log);
    }
}

bool player_chat_item::is_can_sent_msg(std::string const& type, std::string const& msg) {
    messages_item msg_item;
    msg_item.get_params()
        ("player_name", get_root()->name_get_full())
        ("type", type)
        ("msg", msg);

    if (!is_in_game) {
        msg_item.get_sender()
            ("$(player_chat_deny_not_in_game)", msg_player(get_root()))
            ("$(player_chat_deny_not_in_game_log)", msg_log);
        return false;
    }
    else if (is_chat_off) {
        msg_item.get_sender()
            ("$(player_chat_deny_is_chat_off)", msg_player(get_root()))
            ("$(player_chat_deny_is_chat_off_log)", msg_log);
        return false;
    }
    else if (manager.antiflood.is_blocked(get_root())) {
        // —читаем еще пинальти
        manager.antiflood.increase(get_root());
        int flood_time_out = (int)(manager.antiflood.get_time_out(get_root()) / 1000);
        if (manager.is_block_on_flood && !get_root()->block_get()) {
            if (flood_time_out > manager.block_on_flood_max_time) {
                get_root()->block("flood");
            }
        }

        msg_item.get_params()("time_out", flood_time_out);
        msg_item.get_sender()
            ("$(player_chat_deny_is_flood)", msg_player(get_root()))
            ("$(player_chat_deny_is_flood_log)", msg_log);
        return false;
    }

    return true;
}

bool player_chat_item::is_can_receive_msg_from(player_ptr_t const& sender, bool is_broadcast_msg, deny_reasons& reason) const {
    if (!is_in_game) {
        reason = deny_not_in_game;
        return false;
    }
    else if (is_chat_off) {
        reason = deny_is_chat_off;
        return false;
    }
    else if (ignore_list.end() != ignore_list.find(sender)
        // јдмины должны получать все общие сообщени€ чата
        && (!is_broadcast_msg || !get_root()->group_is_in("admin"))
        ) {
        reason = deny_is_ignored;
        return false;
    }
    else if (!is_broadcast_msg && get_root()->get_item<player_gamepause_item>()->is_sleep()) {
        // ≈сли личка и игрок спит - то тоже не даем отослать
        reason = deny_is_sleep;
        return false;
    }

    reason = all_ok;
    return true;
}

bool player_chat_item::is_can_receive_msg_from_st(player_ptr_t const& destination, player_ptr_t const& sender, bool is_broadcast_msg, deny_reasons& reason) {
    return destination->get_item<player_chat_item>()->is_can_receive_msg_from(sender, is_broadcast_msg, reason);
}

void player_chat_item::on_disconnect(samp::player_disconnect_reason reason) {
    player_ptr_t player_ptr = get_root();
    players_t players = players::instance()->get_players();

    BOOST_FOREACH(player::ptr const& listed_player_ptr, players) {
        player_chat_item::ptr player_chat_ptr = listed_player_ptr->get_item<player_chat_item>();
        player_chat_ptr->ignore_list.erase(player_ptr);
    }

    group_chat_disconnect();
    pm_destination_disconnect();
}

bool player_chat_item::get_is_chat_off() const {
    return is_chat_off;
}

void player_chat_item::do_chat_off(int minutes) {
    automute_is_active = false;
    is_chat_off = true;
    chat_off_end_time = std::time(0) + (minutes * 60);
    chat_off_last_remain = get_remain_chat_off_time();
}

void player_chat_item::do_chat_on() {
    is_chat_off = false;
}

void player_chat_item::do_recalc_chat() {
    if (is_chat_off) {
        if (chat_off_end_time - std::time(0) <= 0) {
            process_auto_chat_on();
        }
        else if (get_remain_chat_off_time() != chat_off_last_remain) {
            chat_off_last_remain = get_remain_chat_off_time();
            show_remain_time();
        }
    }
}

int player_chat_item::get_remain_chat_off_time() const {
    int rezult = 0;
    if (is_chat_off) {
        std::time_t curr = std::time(0);
        rezult = (int)((chat_off_end_time - curr) / 60);
        if (0 != (chat_off_end_time - curr) % 60) {
            ++rezult;
        }
    }
    if (rezult < 0) {
        rezult = 0;
    }
    return rezult;
}

void player_chat_item::process_auto_chat_on() {
    messages_item msg_item;
    msg_item.get_params()("autounmute_player_name", get_root()->name_get_full());
    msg_item.get_sender()
        ("$(auto_unmute_done_all)", msg_players_all)
        ("$(auto_unmute_done_log)", msg_log);
   do_chat_on();
}

void player_chat_item::show_remain_time() {
    messages_item msg_item;
    msg_item.get_params()("time_remain", get_remain_chat_off_time());
    msg_item.get_sender()("$(mute_show_remain_time)", msg_player(get_root()));
}

void player_chat_item::automute_process() {
    if (automute_is_active && automute_time <= std::time(0)) {
        // јвтомутим
        messages_item msg_item;
        msg_item.get_params()
            ("player_name", get_root()->name_get_full())
            ("mute_time", automute_minutes);
        msg_item.get_sender()
            ("$(automute_done_players)", msg_players_all_users)
            ("$(automute_done_admins)", msg_players_all_admins)
            ("$(automute_done_log)", msg_log);

        do_chat_off(automute_minutes);
    }
}

int player_chat_item::automute_set(int mute_time) {
    if (automute_is_active) {
        // ”же автомутим - прибавл€ем врем€ мута
        automute_minutes += mute_time;
    }
    else {
        automute_minutes = mute_time;
        { // ”станавливаем задержку срабатывани€ автомута
            int time_delta = (std::rand() % (manager.auto_mute_delay_max - manager.auto_mute_delay_min)) + manager.auto_mute_delay_min;
            automute_time = std::time(0) + time_delta;
        }
        automute_is_active = true;
    }
    if (automute_minutes > manager.mute_time_max) {
        automute_minutes = manager.mute_time_max;
    }

    return static_cast<int>(static_cast<std::time_t>(automute_time) - std::time(0));
}

void player_chat_item::group_chat_on_enter() {
    messages_item msg_item;
    msg_item.get_params()
        ("group_chat_name", group_chat_name)
        ("player_name", get_root()->name_get_full());
    players_t group_players = player_chat::group_chat_get_players_by_name(group_chat_name);
    group_players.erase(get_root());
    if (group_players.empty()) {
        // —оздал группу
        msg_item.get_sender()
            ("$(group_chat_enter_new_player)", msg_player(get_root()))
            ("$(group_chat_enter_new_log)", msg_log);
    }
    else {
        // ¬ошел в группу
        msg_item.get_params()
            ("group_chat_count", group_players.size())
            ("group_chat_names", players::get_players_names(group_players));
        msg_item.get_sender()
            ("$(group_chat_enter_player)", msg_player(get_root()))
            ("$(group_chat_enter_players)", msg_players(group_players))
            ("$(group_chat_enter_log)", msg_log);
    }
}

void player_chat_item::group_chat_on_leave(bool is_exit) {
    messages_item msg_item;
    msg_item.get_params()
        ("group_chat_name", group_chat_name)
        ("player_name", get_root()->name_get_full());
    players_t group_players = player_chat::group_chat_get_players_by_name(group_chat_name);
    group_players.erase(get_root());
    if (group_players.empty()) {
        // ”далил группу
        msg_item.get_sender()
            ("$(group_chat_leave_last_player)", msg_player(get_root()))
            ("$(group_chat_leave_last_log)", msg_log);
    }
    else {
        // ”шел из группы
        msg_item.get_params()
            ("group_chat_count", group_players.size());
        msg_item.get_sender()
            ("$(group_chat_leave_player)", msg_player(get_root()))
            ("$(group_chat_leave_players)", msg_players(group_players))
            ("$(group_chat_leave_log)", msg_log);
    }
}

void player_chat_item::group_chat_disconnect() {
    if (!group_chat_name.empty()) {
        group_chat_on_leave(true);
        group_chat_name.clear();
    }
}

bool player_chat_item::get_is_player_self_chat_off() const {
    if (get_root()->group_is_in("admin")) {
        return false;
    }
    return is_player_self_chat_off;
}

void player_chat_item::pm_destination_disconnect() {
    player_ptr_t const player_ptr = get_root();
    players_t players = players::instance()->get_players();

    BOOST_FOREACH(player::ptr const& listed_player_ptr, players) {
        player_chat_item::ptr player_chat_ptr = listed_player_ptr->get_item<player_chat_item>();
        if (player_ptr == player_chat_ptr->pm_destination) {
            player_chat_ptr->pm_destination.reset();
        }
    }
}
