#ifndef BASIC_TYPES_HPP
#define BASIC_TYPES_HPP
#include <istream>
#include <ostream>
#include <vector>
#include <limits>
#include <boost/format.hpp>

// »д, которые значат любой интерьер/мир
enum {interior_any = -1};
enum {world_any = -1};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct pos3 {
    float x;
    float y;
    float z;
    int interior;
    int world;
    pos3(float x = .0f, float y = .0f, float z = .0f, int interior = 0, int world = 0)
        :x(x), y(y), z(z), interior(interior), world(world) {
    }

    bool operator==(pos3 const& right) const {
        return x == right.x
            && y == right.y
            && z == right.z
            && interior == right.interior
            && world == right.world
            ;
    }
    bool operator<(pos3 const& right) const {
        if (x == right.x) {
            if (y == right.y) {
                if (z == right.z) {
                    if (interior == right.interior) {
                        return world < right.world;
                    }
                    return interior < right.interior;
                }
                return z < right.z;
            }
            return y < right.y;
        }
        return x < right.x;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, pos3& pos) {
    return is>>pos.x>>pos.y>>pos.z>>pos.interior>>pos.world;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, pos3 const& pos) {
    return os << boost::basic_format<char_t>("%1$0.3f %2$0.3f %3$0.3f %4% %5%") % pos.x % pos.y % pos.z % pos.interior % pos.world;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <pos3>: std::tr1::true_type {};
    template <> struct is_streameble_write<pos3>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct pos_camera {
    float x;
    float y;
    float z;
    float lock_x;
    float lock_y;
    float lock_z;
    pos_camera(float x = .0f, float y = .0f, float z = .0f, float lock_x = .0f, float lock_y = .0f, float lock_z = .0f)
        :x(x), y(y), z(z), lock_x(lock_x), lock_y(lock_y), lock_z(lock_z) {
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, pos_camera& pos) {
    return is>>pos.x>>pos.y>>pos.z>>pos.lock_x>>pos.lock_y>>pos.lock_z;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, pos_camera const& pos) {
    return os<<pos.x<<" "<<pos.y<<" "<<pos.z<<" "<<pos.lock_x<<" "<<pos.lock_y<<" "<<pos.lock_z;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <pos_camera>: std::tr1::true_type {};
    template <> struct is_streameble_write<pos_camera>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct pos4: pos3 {
    float rz;
    pos4(float x = .0f, float y = .0f, float z = .0f, int interior = 0, int world = 0, float rz =.0f)
        :pos3(x, y, z, interior, world), rz(rz) {
    }
    bool operator==(pos4 const& right) const {
        return rz == right.rz
            && pos3::operator==(right)
            ;
    }
    bool operator<(pos4 const& right) const {
        if (pos3::operator==(right)) {
            return rz < right.rz;
        }
        return pos3::operator<(right);
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, pos4& pos) {
    return is>>pos.x>>pos.y>>pos.z>>pos.interior>>pos.world>>pos.rz;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, pos4 const& pos) {
    return os << boost::basic_format<char_t>("%1$0.3f %2$0.3f %3$0.3f %4% %5% %6$0.1f") % pos.x % pos.y % pos.z % pos.interior % pos.world % pos.rz;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <pos4>: std::tr1::true_type {};
    template <> struct is_streameble_write<pos4>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct pos6: pos3 {
    float rx;
    float ry;
    float rz;
    pos6(float x = .0f, float y = .0f, float z = .0f, int interior = 0, int world = 0, float rx =.0f, float ry =.0f, float rz =.0f)
        :pos3(x, y, z, interior, world), rx(rx), ry(ry), rz(rz) {
    }
    bool operator==(pos6 const& right) const {
        return rx == right.rx
            && ry == right.ry
            && rz == right.rz
            && pos3::operator==(right)
            ;
    }
    bool operator<(pos6 const& right) const {
        if (pos3::operator==(right)) {
            if (rx == right.rx) {
                if (ry == right.ry) {
                    return rz < right.rz;
                }
                return ry < right.ry;
            }
            return rx < right.rx;
        }
        return pos3::operator<(right);
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, pos6& pos) {
    return is>>pos.x>>pos.y>>pos.z>>pos.interior>>pos.world>>pos.rx>>pos.ry>>pos.rz;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, pos6 const& pos) {
    return os<<pos.x<<" "<<pos.y<<" "<<pos.z<<" "<<pos.interior<<" "<<pos.world<<" "<<pos.rx<<" "<<pos.ry<<" "<<pos.rz;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <pos6>: std::tr1::true_type {};
    template <> struct is_streameble_write<pos6>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct player_time {
    int hour;
    int minute;
    player_time(int hour = 12, int minute = 0): hour(hour), minute(minute) {
    }
};
template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, player_time& time) {
    return is>>time.hour>>time.minute;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, player_time const& time) {
    return os<<time.hour<<" "<<time.minute;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <player_time>: std::tr1::true_type {};
    template <> struct is_streameble_write<player_time>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct weapon_item_t {
    int id;
    int ammo;
    weapon_item_t(int id = 0, int ammo = 0): id(id), ammo(ammo) {}
};
template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, weapon_item_t& weapon) {
    return is>>weapon.id>>weapon.ammo;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, weapon_item_t& weapon) {
    return os<<weapon.id<<" "<<weapon.ammo;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <weapon_item_t>: std::tr1::true_type {};
    template <> struct is_streameble_write<weapon_item_t>: std::tr1::true_type {};
} // namespace serialization {

struct weapons_t {
    typedef std::vector<weapon_item_t> items_t;

    items_t items;
    int     active_id;
    weapons_t(): active_id(-1) {}
    weapons_t(items_t const& items): items(items), active_id(-1) {
        if (!items.empty()) {
            // јктивным делаем последний итем
            active_id = items.back().id;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct player_health_armour {
    float health;
    float armour;
    player_health_armour(float health = 0.0f, float armour = 0.0f): health(health), armour(armour) {}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct world_bounds_t {
    float x_min;
    float y_min;
    float x_max;
    float y_max;
    world_bounds_t(): x_min(.0f), y_min(.0f), x_max(.0f), y_max(.0f) {}
    world_bounds_t(float x_min, float y_min, float x_max, float y_max): x_min(x_min), y_min(y_min), x_max(x_max), y_max(y_max) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, world_bounds_t& world_bounds) {
    return is>>world_bounds.x_min>>world_bounds.y_min>>world_bounds.x_max>>world_bounds.y_max;
}
template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, world_bounds_t const& world_bounds) {
    return os<<world_bounds.x_min<<" "<<world_bounds.y_min<<" "<<world_bounds.x_max<<" "<<world_bounds.y_max;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <world_bounds_t>: std::tr1::true_type {};
    template <> struct is_streameble_write<world_bounds_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ¬озращает истину, если между точками рассто€ние меньше заданного
bool inline is_points_in_sphere(pos3 const& pos1, pos3 const& pos2, float radius) {
    if (pos1.interior != pos2.interior || pos1.world != pos2.world) {
        return false;
    }
    return (pos1.x - pos2.x)*(pos1.x - pos2.x)
        +  (pos1.y - pos2.y)*(pos1.y - pos2.y)
        +  (pos1.z - pos2.z)*(pos1.z - pos2.z)
        <  radius * radius
        ;
}

// ¬озращает квадрат расто€ни€ между точками
float inline get_points_metric_square(pos3 const& pos1, pos3 const& pos2) {
    if ((pos1.interior != pos2.interior && interior_any != pos1.interior && interior_any != pos2.interior)
      ||(pos1.world != pos2.world && world_any != pos1.world && world_any != pos2.world)) {
        return std::numeric_limits<float>::max();
    }
    return (pos1.x - pos2.x)*(pos1.x - pos2.x)
        +  (pos1.y - pos2.y)*(pos1.y - pos2.y)
        +  (pos1.z - pos2.z)*(pos1.z - pos2.z)
        ;
}

// ¬озращает квадрат расто€ни€ между точками, не учитыва€ интерьер и мир
float inline get_points_metric_square_noiw(pos3 const& pos1, pos3 const& pos2) {
    return (pos1.x - pos2.x)*(pos1.x - pos2.x)
        +  (pos1.y - pos2.y)*(pos1.y - pos2.y)
        +  (pos1.z - pos2.z)*(pos1.z - pos2.z)
        ;
}

#endif // BASIC_TYPES_HPP
