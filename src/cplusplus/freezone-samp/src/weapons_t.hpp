#ifndef WEAPONS_T_HPP
#define WEAPONS_T_HPP
#include <string>
#include <istream>
#include <ostream>
#include <boost/algorithm/string.hpp>
#include "basic_types.hpp"

struct weapon_def_t {
    int id;
    int slot;
    int pickup_id;
    int ammo_reload;        // Сколько патронов в обойме
    int max_reload;         // Сколько максимум обойм можно продать игроку
    int sell_reload;        // По сколько обойм продовать
    int price;              // Цена одного комплекта
    std::string descripion; // Игровое имя

    weapon_def_t(): id(0), slot(0), pickup_id(0), ammo_reload(0), max_reload(0), sell_reload(0), price(0) {}

    bool operator ==(weapon_def_t const& right) const {
        return id == right.id
            && slot == right.slot
            && pickup_id == right.pickup_id
            && ammo_reload == right.ammo_reload
            && max_reload == right.max_reload
            && sell_reload == right.sell_reload
            && price == right.price
            && descripion == right.descripion
            ;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, weapon_def_t& weapon_def) {
    is>>weapon_def.id>>weapon_def.slot>>weapon_def.pickup_id>>weapon_def.ammo_reload>>weapon_def.max_reload>>weapon_def.sell_reload>>weapon_def.price;
    std::getline(is, weapon_def.descripion);
    boost::trim(weapon_def.descripion);
    if (weapon_def.descripion.empty() && !is.fail()) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <weapon_def_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct weapon_buy_item_t: public pos3 {
    std::string def_name;   // Имя описания оружия
    int limit_count;        // Количество оружия, после которого итем блокируется на limit_time секунд. 0 = нет ограничений
    int limit_time;         // Время в течении которого ограничивается покупка оружия. 0 = нет ограничений
    weapon_buy_item_t(): limit_count(0), limit_time(0) {}

    bool operator==(weapon_buy_item_t const& right) const {
        return def_name == right.def_name
            && limit_count == limit_count
            && limit_time == limit_time
            && pos3::operator==(right)
            ;
    }
    bool operator<(weapon_buy_item_t const& right) const {
        if (def_name == right.def_name) {
            if (limit_count == right.limit_count) {
                if (limit_time == right.limit_time) {
                    return pos3::operator<(right);
                }
                return limit_time < right.limit_time;
            }
            return limit_count < right.limit_count;
        }
        return def_name < right.def_name;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, weapon_buy_item_t& weapon_buy_item) {
    return is>>weapon_buy_item.x>>weapon_buy_item.y>>weapon_buy_item.z>>weapon_buy_item.interior>>weapon_buy_item.world>>weapon_buy_item.def_name>>weapon_buy_item.limit_count>>weapon_buy_item.limit_time;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <weapon_buy_item_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "core/streamer_coord_works.hpp"
namespace streamer {
    template <> struct coord_works<weapon_buy_item_t> {
        // Методы, необходимые для грида.
        inline static bool get_boundary_box(weapon_buy_item_t const& item, float& x_min, float& y_min, float& z_min, float& x_max, float& y_max, float& z_max) {
            return create_sphere_boundary_box(item, 1.0f, x_min, y_min, z_min, x_max, y_max, z_max);
        }
        inline static int get_interior(weapon_buy_item_t const& item) {return item.interior;}
        inline static int get_world(weapon_buy_item_t const& item) {return item.world;}

        // Методы, необходимые для упорядочивания результатов поиска
        typedef float metric_t;
        inline static metric_t get_metric(weapon_buy_item_t const& item, pos3 const& pos) {
            return get_points_metric_square_noiw(item, pos);
        }

        // Методы, необходимые для детальной проверки
        inline static bool get_is_point_in(weapon_buy_item_t const& item, pos3 const& pos) {
            return is_points_in_sphere(item, pos, 1.0f);
        }
    };
} // namespace streamer {

struct weapon_skills_t {
    int pistol;
    int pistol_silenced;
    int desert_eagle;
    int shotgun;
    int sawnoff_shotgun;
    int spas12_shotgun;
    int micro_uzi;
    int mp5;
    int ak47;
    int m4;
    int sniper_rifle;
    weapon_skills_t(): pistol(200), pistol_silenced(200), desert_eagle(200), shotgun(200), sawnoff_shotgun(200), spas12_shotgun(200), micro_uzi(200), mp5(200), ak47(200), m4(200), sniper_rifle(200) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, weapon_skills_t& skills) {
    return is>>skills.pistol>>skills.pistol_silenced>>skills.desert_eagle>>skills.shotgun>>skills.sawnoff_shotgun>>skills.spas12_shotgun>>skills.micro_uzi>>skills.mp5>>skills.ak47>>skills.m4>>skills.sniper_rifle;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <weapon_skills_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // WEAPONS_T_HPP
