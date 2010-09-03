#include "config.hpp"
#include <algorithm>
#include <functional>
#include <boost/algorithm/string.hpp>
#include "player_messages.hpp"
#include "core/application.hpp"
#include "core/player/player.hpp"
#include "core/players.hpp"

REGISTER_IN_APPLICATION(player_messages);

player_messages::ptr player_messages::instance() {
    return application::instance()->get_item<player_messages>();
}

player_messages::player_messages() {
}

player_messages::~player_messages() {
}

void player_messages::on_connect(player_ptr_t const& player_ptr) {
    player_messages_bubble_item::ptr item(new player_messages_bubble_item(), &player_item::do_delete);
    player_ptr->add_item(item);
}

void player_messages::send_message(player_ptr_t const& player_ptr, std::string const& msg) {
    chat_msg_items_t items = create_chat_items(msg);
    std::for_each(items.begin(), items.end(), std::tr1::bind(static_cast<void (*)(player_ptr_t const&, chat_msg_item_t const&)>(&player_messages::send_message), player_ptr, std::tr1::placeholders::_1));
}

void player_messages::send_message(player_ptr_t const& player_ptr, chat_msg_item_t const& msg_item) {
    if (msg_item.is_clear) {
        int max_clear_lines = 100;
        for (int i = 0; i < max_clear_lines; ++i) {
            player_ptr->send_client_message(msg_item.color << 8 , " ");
        }
    }
    else {
        std::string  msg_str = msg_item.msg;
        if (msg_str.empty()) {
            // ≈сли строка пуста€ - то сервер крашит
            msg_str = " ";
        }
        if (msg_str.length() > 128) {
            // ≈сли строка очень длинна€, то она просто не выводитс€ на экран
            msg_str = std::string(msg_str.begin(), msg_str.begin() + 128);
        }
        // ≈сли в строке будет символ процента - то сервер крашит
        boost::replace_all(msg_str, "%", "?");

        // “ильда у нас - неразрывный пробел :)
        boost::replace_all(msg_str, "~", " ");

        player_ptr->send_client_message(msg_item.color << 8 , msg_str);
    }
}

void player_messages::send_player_message(player_ptr_t const& player_ptr, player_ptr_t const& from_player, std::string const& msg, bool is_close_chat) {
    if (is_close_chat) {
        from_player->name_set_for_chat_close();
    }
    else {
        from_player->name_set_for_chat();
    }
    player_ptr->send_player_message(from_player, msg);
}

void player_messages::send_player_message_restore(player_ptr_t const& from_player) {
    from_player->name_reset();
}

void player_messages::set_chat_bubble(player_ptr_t const& player_ptr, std::string const& text) {
    chat_bubble_t chat_bubble = create_chat_bubble(boost::replace_all_copy(text, "~", " "));
    player_ptr->get_item<player_messages_bubble_item>()->set_chat_bubble(chat_bubble);
}


player_messages_bubble_item::player_messages_bubble_item()
:last_chat_bubble_id(0)
,max_expire_time(0)
{
}

player_messages_bubble_item::~player_messages_bubble_item() {
}

void player_messages_bubble_item::on_player_stream_in(player_ptr_t const& player_ptr) {
    player_messages_bubble_item::ptr player_messages_bubble_item_ptr = player_ptr->get_item<player_messages_bubble_item>();

    showed_ids_t::iterator ids_it = showed_ids.find(player_ptr);
    if (showed_ids.end() == ids_it || player_messages_bubble_item_ptr->last_chat_bubble_id != ids_it->second) {
        player_messages_bubble_item_ptr->update_chat_bubble();
        showed_ids[player_ptr] = player_messages_bubble_item_ptr->last_chat_bubble_id;
    }    
}

void player_messages_bubble_item::on_disconnect(samp::player_disconnect_reason reason) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        item_player_ptr->get_item<player_messages_bubble_item>()->showed_ids.erase(get_root());
    }
}

void player_messages_bubble_item::set_chat_bubble(chat_bubble_t const& chat_bubble) {
    set_chat_bubble_impl(chat_bubble, time_base::tick_count_milliseconds() + chat_bubble.expire_time);
}

void player_messages_bubble_item::set_chat_bubble_impl(chat_bubble_t const& chat_bubble, time_base::millisecond_t expire_time) {
    ++last_chat_bubble_id;
    last_chat_bubble = chat_bubble;
    last_chat_bubble_expire_time.reset(expire_time);
    if (max_expire_time < expire_time) {
        max_expire_time = expire_time;
    }

    // 
    players_t const& players = get_root()->stream_in_players_get();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        item_player_ptr->get_item<player_messages_bubble_item>()->showed_ids.insert(std::make_pair(get_root(), last_chat_bubble_id));
    }

    get_root()->set_chat_bubble(chat_bubble.color, chat_bubble.draw_distance, chat_bubble.expire_time, chat_bubble.text);
}

bool player_messages_bubble_item::is_chat_bubble_active(int& show_time) {
    if (!last_chat_bubble_expire_time) {
        return false;
    }
    time_base::millisecond_t curr_time = time_base::tick_count_milliseconds();
    if (last_chat_bubble_expire_time.get() <= curr_time) {
        last_chat_bubble_expire_time.reset();
        if (max_expire_time > curr_time) {
            // нужно еще стереть
            set_chat_bubble_impl(chat_bubble_t(static_cast<int>(max_expire_time - curr_time)), max_expire_time);
        }
        return false;
    }
    show_time = static_cast<int>(last_chat_bubble_expire_time.get() - curr_time);
    return true;
}

void player_messages_bubble_item::update_chat_bubble() {
    int show_time = 0;
    if (is_chat_bubble_active(show_time)) {
        // ќбновл€ем чат бабл, если мы вошли в зону видимости игрока (если мы не видели когда игрок устанавливал чат бабл, то мы этот чат бабл не увидим, вне зависимости от времени показа)
        get_root()->set_chat_bubble(last_chat_bubble.color, last_chat_bubble.draw_distance, show_time, last_chat_bubble.text);
    }
}
