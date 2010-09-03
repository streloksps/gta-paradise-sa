#include "config.hpp"
#include "player_less.hpp"
#include "player.hpp"

bool player_less::operator()(player_ptr_t const& left, player_ptr_t const& right) const {
    return left->get_id() < right->get_id();
}
