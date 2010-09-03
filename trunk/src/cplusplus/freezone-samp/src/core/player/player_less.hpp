#ifndef PLAYER_LESS_HPP
#define PLAYER_LESS_HPP
#include "player_fwd.hpp"
#include <functional>

struct player_less: public std::binary_function<player_ptr_t, player_ptr_t, bool> {
    bool operator()(player_ptr_t const& left, player_ptr_t const& right) const;
};

#endif // PLAYER_LESS_HPP
