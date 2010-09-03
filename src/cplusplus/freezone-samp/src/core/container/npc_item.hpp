#ifndef NPC_ITEM_HPP
#define NPC_ITEM_HPP
#include "container_item.hpp"
#include "core/npc_fwd.hpp"
#include "core/npc.hpp"

class npc_item: public container_item {
public:
    typedef std::tr1::shared_ptr<npc_item> ptr_t;

protected:
    inline npc_item() {}
    inline virtual ~npc_item() {}

public: // container_item_t
    inline npc_ptr_t get_root();

private:
    npc_wptr_t root;
};

npc_ptr_t npc_item::get_root() {
    if (root.expired()) {
        root = npc_wptr_t(std::tr1::dynamic_pointer_cast<npc>(container_item::get_root()));
    }
    return root.lock();
}

#endif // NPC_ITEM_HPP
