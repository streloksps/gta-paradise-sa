#ifndef PLAYER_ANIMATION_T_HPP
#define PLAYER_ANIMATION_T_HPP
#include "core/serialization/configuradable.hpp"
#include <vector>
#include <map>
#include "core/utility/locale_ru.hpp"

struct animation_lib_val: public configuradable {
public: // configuradable
    virtual void configure_pre() {}
    virtual void configure(buffer::ptr const& buff, def_t const& def) {
        SERIALIZE_ITEM(lib_name);
        SERIALIZE_ITEM(anim_names);
        SERIALIZE_ITEM(is_use_in_vehicle);
    }
    virtual void configure_post() {}
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(animation_lib_val);

public:
    animation_lib_val(): is_use_in_vehicle(false) {}

public:
    std::string                 lib_name;
    std::vector<std::string>    anim_names;
    bool                        is_use_in_vehicle;
};

typedef std::map<std::string, animation_lib_val, ilexicographical_less> animation_libs_t;

#endif // PLAYER_ANIMATION_T_HPP
