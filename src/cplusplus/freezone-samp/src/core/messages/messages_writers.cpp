#include "config.hpp"
#include "messages_writers.hpp"
#include <algorithm>
#include <functional>
#include <cassert>
#include "core/logger/logger.hpp"
#include "core/player/player.hpp"
#include "core/players.hpp"
#include "core/application.hpp"
#include "core/messages/player_messages/tags.hpp"
#include "core/samp/pawn/pawn_log.hpp"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "core/messages/player_messages/player_messages.hpp"
#include "core/utility/erase_if.hpp"

// Врайтеры
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct message_writer_logger: messages_writer, private T {
    virtual void message_write(std::string const& msg) {
        logger::ptr logger_ptr = application::instance()->get_item<logger>();
        tags message(msg);
        std::string msg_section = message.get_closed_tag_val<std::string>("log_section", 0, "no_section");
        std::string msg_str = message.get_untaget_str();
        T::process_log(logger_ptr, msg_section, msg_str);
    }
};
struct message_writer_logger_log {
    static inline void process_log(logger::ptr& logger_ptr, std::string const& msg_section, std::string const& msg_str) {
        logger_ptr->log(msg_section, boost::replace_all_copy(msg_str, "~", " "));
    }
};
struct message_writer_logger_debug {
    static inline void process_log(logger::ptr& logger_ptr, std::string const& msg_section, std::string const& msg_str) {
        logger_ptr->debug(msg_section, boost::replace_all_copy(msg_str, "~", " "));
    }
};
typedef message_writer_logger<message_writer_logger_log> message_writer_log;
typedef message_writer_logger<message_writer_logger_debug> message_writer_debug;

struct message_writer_player: messages_writer {
    player::ptr player_ptr;
    message_writer_player(player::ptr player_ptr): player_ptr(player_ptr) {}
    virtual void message_write(std::string const& msg) {
        player_messages::send_message(player_ptr, msg);
    }
};

struct message_writer_player_bubble: messages_writer {
    player::ptr player_ptr;
    message_writer_player_bubble(player::ptr player_ptr): player_ptr(player_ptr) {}
    virtual void message_write(std::string const& msg) {
        player_messages::set_chat_bubble(player_ptr, msg);
    }
};

struct message_writer_server_log: messages_writer {
    message_writer_server_log() {}
    virtual void message_write(std::string const& msg) {
        pawn::logprintf("%s", msg.c_str());
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
msg_log_t msg_log;
msg_debug_t msg_debug;
msg_serverlog_t msg_serverlog;
msg_players_all_t msg_players_all;
msg_players_all_admins_t msg_players_all_admins;
msg_players_all_users_t msg_players_all_users;

// Манипуляторы лога
////////////////////////////////////////////////////////////////////////////////////////////////////
void msg_log_t::add(msg_dests& dests) const {
    dests.message_writers.push_back(messages_writer::ptr(new message_writer_log()));
}

void msg_debug_t::add(msg_dests& dests) const {
    dests.message_writers.push_back(messages_writer::ptr(new message_writer_debug()));
}

void msg_serverlog_t::add(msg_dests& dests) const {
    dests.message_writers.push_back(messages_writer::ptr(new message_writer_server_log()));
}

// Манипуляторы игрока
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace {
    inline void player_add(msg_dests& dests, player::ptr player_ptr) {
        BOOST_FOREACH(messages_writer::ptr ptr, dests.message_writers) {
            if (std::tr1::shared_ptr<message_writer_player> message_writer_player_ptr = std::tr1::dynamic_pointer_cast<message_writer_player>(ptr)) {
                if (player_ptr == message_writer_player_ptr->player_ptr) {
                    // Данные игрок уже есть в списке
                    return;
                }
            }
        }
        dests.message_writers.push_back(messages_writer::ptr(new message_writer_player(player_ptr)));
    }
    inline void player_remove(msg_dests& dests, player::ptr player_ptr) {
        //BOOST_FOREACH(message_writer::ptr ptr, dests.message_writers) {
        for (msg_dests::message_writers_t::iterator it = dests.message_writers.begin(), end = dests.message_writers.end(); it != end; ++it) {
            if (std::tr1::shared_ptr<message_writer_player> message_writer_player_ptr = std::tr1::dynamic_pointer_cast<message_writer_player>(*it)) {
                if (player_ptr == message_writer_player_ptr->player_ptr) {
                    // Данные игрок уже есть в списке - удаляем
                    dests.message_writers.erase(it);
                    return;
                }
            }
        }
    }
    inline bool is_player_can_receive_msg_for_all(player::ptr const& player_ptr) {
        player_state state = player_ptr ->get_state();
        return player_in_game == state || player_not_spawn == state;
    }
}
void msg_players_all_t::add(msg_dests& dests) const {
    players_t players_vector = players::instance()->get_players();
    erase_if(players_vector, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&is_player_can_receive_msg_for_all, std::tr1::placeholders::_1)));
    for_each(players_vector.begin(), players_vector.end(), std::tr1::bind(&player_add, std::tr1::ref(dests), std::tr1::placeholders::_1));
}

void msg_players_all_admins_t::add(msg_dests& dests) const {
    players_t players_vector = players::instance()->get_players();
    erase_if(players_vector, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&is_player_can_receive_msg_for_all, std::tr1::placeholders::_1)));
    erase_if(players_vector, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&player::group_is_in, std::tr1::placeholders::_1, std::tr1::cref(std::string("admin")))));
    for_each(players_vector.begin(), players_vector.end(), std::tr1::bind(&player_add, std::tr1::ref(dests), std::tr1::placeholders::_1));
}

void msg_players_all_admins_t::remove(msg_dests& dests) const {
    players_t players_vector = players::instance()->get_players();
    erase_if(players_vector, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&player::group_is_in, std::tr1::placeholders::_1, std::tr1::cref(std::string("admin")))));
    for_each(players_vector.begin(), players_vector.end(), std::tr1::bind(&player_remove, std::tr1::ref(dests), std::tr1::placeholders::_1));
}

void msg_players_all_users_t::add(msg_dests& dests) const {
    players_t players_vector = players::instance()->get_players();
    erase_if(players_vector, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&is_player_can_receive_msg_for_all, std::tr1::placeholders::_1)));
    erase_if(players_vector, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&player::group_is_in, std::tr1::placeholders::_1, std::tr1::cref(std::string("user")))));
    for_each(players_vector.begin(), players_vector.end(), std::tr1::bind(&player_add, std::tr1::ref(dests), std::tr1::placeholders::_1));
}

void msg_players_all_users_t::remove(msg_dests& dests) const {
    players_t players_vector = players::instance()->get_players();
    erase_if(players_vector, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&player::group_is_in, std::tr1::placeholders::_1, std::tr1::cref(std::string("user")))));
    for_each(players_vector.begin(), players_vector.end(), std::tr1::bind(&player_remove, std::tr1::ref(dests), std::tr1::placeholders::_1));
}

msg_players::msg_players(players_t const& players_): players_(players_) {
}

void msg_players::add(msg_dests& dests) const {
    players_t players_vector = players_;
    erase_if(players_vector, std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&is_player_can_receive_msg_for_all, std::tr1::placeholders::_1)));
    for_each(players_vector.begin(), players_vector.end(), std::tr1::bind(&player_add, std::tr1::ref(dests), std::tr1::placeholders::_1));
}

void msg_players::remove(msg_dests& dests) const {
    players_t players_vector = players_;
    for_each(players_vector.begin(), players_vector.end(), std::tr1::bind(&player_remove, std::tr1::ref(dests), std::tr1::placeholders::_1));
}

msg_player::msg_player(player_ptr_t player_ptr): player_ptr(player_ptr) {
    assert(player_ptr);
}

void msg_player::add(msg_dests& dests) const {
    player_add(dests, player_ptr);
}

void msg_player::remove(msg_dests& dests) const {
    player_remove(dests, player_ptr);
}

msg_player_bubble::msg_player_bubble(player_ptr_t player_ptr): player_ptr(player_ptr) {
}

void msg_player_bubble::add(msg_dests& dests) const {
    dests.message_writers.push_back(messages_writer::ptr(new message_writer_player_bubble(player_ptr)));
}
