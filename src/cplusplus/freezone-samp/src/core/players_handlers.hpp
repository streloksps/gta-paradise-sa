#ifndef PLAYERS_HANDLERS_HPP
#define PLAYERS_HANDLERS_HPP
#include <boost/foreach.hpp>
#include "players.hpp"
#include "container/container_handlers.hpp"
// «апускает метод у всех итемов всех игроков

template<typename event_class_t>
inline void players_execute_handlers(void (event_class_t::*event)()) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        container_execute_handlers(item_player_ptr, event);
    }
}

template<typename event_class_t, typename arg1_t>
inline void players_execute_handlers(void (event_class_t::*event)(arg1_t), typename boost::call_traits<arg1_t>::const_reference arg1) {
    players_t players = players::instance()->get_players();
    BOOST_FOREACH(player::ptr const& item_player_ptr, players) {
        container_execute_handlers(item_player_ptr, event, arg1);
    }
}

#endif // PLAYERS_HANDLERS_HPP
