#ifndef MESSAGES_WRITERS_HPP
#define MESSAGES_WRITERS_HPP
#include <string>
#include <memory>
#include <vector>
#include "core/player/player_fwd.hpp"
#include "core/player/player_t.hpp"
#include "core/npc_fwd.hpp"

struct messages_writer {
public:
    typedef std::tr1::shared_ptr<messages_writer> ptr;
    messages_writer() {}
    virtual ~messages_writer() {}
    virtual void message_write(std::string const& msg) = 0;
};

struct msg_dests {
    typedef std::vector<messages_writer::ptr> message_writers_t;

    message_writers_t message_writers;
    msg_dests() {}

    template <typename manipulator_t>
    msg_dests& operator+(manipulator_t const& manipulator) {
        manipulator.add(*this);
        return *this;
    }
    template <typename manipulator_t>
    msg_dests& operator-(manipulator_t const& manipulator) {
        manipulator.remove(*this);
        return *this;
    }
};

template <typename T> struct is_msg_manipulator_has_add {};
template <typename manipulator1_t, typename manipulator2_t>
typename is_msg_manipulator_has_add<manipulator2_t>::type operator+(manipulator1_t const& manipulator1, manipulator2_t const& manipulator2) {
    return msg_dests() + manipulator1 + manipulator2;
}

template <typename T> struct is_msg_manipulator_has_remove {};
template <typename manipulator1_t, typename manipulator2_t>
typename is_msg_manipulator_has_remove<manipulator2_t>::type operator-(manipulator1_t const& manipulator1, manipulator2_t const& manipulator2) {
    return msg_dests() + manipulator1 - manipulator2;
}

struct msg_log_t {
    void add(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_log_t> {typedef msg_dests type;};

struct msg_debug_t {
    void add(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_debug_t> {typedef msg_dests type;};

struct msg_serverlog_t {
    void add(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_serverlog_t> {typedef msg_dests type;};

struct msg_players_all_t {
    void add(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_players_all_t> {typedef msg_dests type;};

struct msg_players_all_admins_t {
    void add(msg_dests& dests) const;
    void remove(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_players_all_admins_t> {typedef msg_dests type;};
template <> struct is_msg_manipulator_has_remove<msg_players_all_admins_t> {typedef msg_dests type;};

struct msg_players_all_users_t {
    void add(msg_dests& dests) const;
    void remove(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_players_all_users_t> {typedef msg_dests type;};
template <> struct is_msg_manipulator_has_remove<msg_players_all_users_t> {typedef msg_dests type;};

struct msg_players {
    players_t players_;
    msg_players(players_t const& players_);
    void add(msg_dests& dests) const;
    void remove(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_players> {typedef msg_dests type;};
template <> struct is_msg_manipulator_has_remove<msg_players> {typedef msg_dests type;};

struct msg_player {
    player_ptr_t player_ptr;
    msg_player(player_ptr_t player_ptr);
    void add(msg_dests& dests) const;
    void remove(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_player> {typedef msg_dests type;};
template <> struct is_msg_manipulator_has_remove<msg_player> {typedef msg_dests type;};

struct msg_player_bubble {
    player_ptr_t player_ptr;
    msg_player_bubble(player_ptr_t player_ptr);
    void add(msg_dests& dests) const;
};
template <> struct is_msg_manipulator_has_add<msg_player_bubble> {typedef msg_dests type;};

extern msg_log_t msg_log;
extern msg_debug_t msg_debug;
extern msg_serverlog_t msg_serverlog;
extern msg_players_all_t msg_players_all;
extern msg_players_all_admins_t msg_players_all_admins;
extern msg_players_all_users_t msg_players_all_users;

#endif // MESSAGES_WRITERS_HPP
