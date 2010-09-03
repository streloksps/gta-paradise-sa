#ifndef PLAYER_FREE_FOR_ALL_T_HPP
#define PLAYER_FREE_FOR_ALL_T_HPP
#include <string>
#include <set>
#include "basic_types.hpp"
#include "core/serialization/configuradable.hpp"

struct player_free_for_all_weapons_by_skin_t: public configuradable {
public: // configuradable
    virtual void configure_pre() {}
    virtual void configure(buffer::ptr const& buff, def_t const& def) {
        SERIALIZE_DELIMITED_ITEM(skin_ids);
        SERIALIZE_ITEM(weapons);
    }
    virtual void configure_post() {}
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(weapons_by_skin);

public:
    typedef std::set<int> skin_ids_t;

    skin_ids_t          skin_ids;
    weapons_t::items_t  weapons;
};


#endif // PLAYER_FREE_FOR_ALL_T_HPP
