#ifndef PLAYER_ITEM_HPP
#define PLAYER_ITEM_HPP
#include "container_item.hpp"
#include "core/player/player_fwd.hpp"
#include "core/player/player.hpp"

class player_item: public container_item {
public:
    typedef std::tr1::shared_ptr<player_item> ptr;

protected:
    inline player_item() {}
    inline virtual ~player_item() {}

public: // container_item_t
    inline player_ptr_t get_root();
    inline player_ptr_c_t get_root() const;

private:
    player_wptr_t root;
    mutable player_wptr_c_t root_c;
};

player_ptr_t player_item::get_root() {
    if (root.expired()) {
        root = player_wptr_t(std::tr1::dynamic_pointer_cast<player>(container_item::get_root()));
    }
    return root.lock();
}

player_ptr_c_t player_item::get_root() const {
    if (root_c.expired()) {
        root_c = player_wptr_c_t(std::tr1::dynamic_pointer_cast<player const>(container_item::get_root()));
    }
    return root_c.lock();
}

#endif // PLAYER_ITEM_HPP
