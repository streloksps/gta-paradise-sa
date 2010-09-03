#ifndef DISABLE_ONFOOT_ENTER_T_HPP
#define DISABLE_ONFOOT_ENTER_T_HPP
#include <string>
#include <istream>
#include <ostream>
#include "basic_types.hpp"

struct disabled_zone_t {
    float x;
    float y;
    float z;
    int interior;
    float radius;
    std::string name;
    std::string msg;
    disabled_zone_t()
        :x(0.0f)
        ,y(0.0f)
        ,z(0.0f)
        ,interior(0)
        ,radius(0.0f)
    {}
    bool operator==(disabled_zone_t const& right) const {
        return x == right.x
            && y == right.y
            && z == right.z
            && interior == right.interior
            && radius == right.radius
            && name == right.name
            && msg == right.msg
            ;
    }
    bool operator<(disabled_zone_t const& right) const {
        if (x == right.x) {
            if (y == right.y) {
                if (z == right.z) {
                    if (interior == right.interior) {
                        if (radius == right.radius) {
                            if (name == right.name) {
                                return msg < right.msg;
                            }
                            return name < right.name;
                        }
                        return radius < right.radius;
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
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, disabled_zone_t& zone) {
    return is>>zone.x>>zone.y>>zone.z>>zone.interior>>zone.radius>>zone.name>>zone.msg;
}
template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, disabled_zone_t const& zone) {
    return os<<zone.x<<" "<<zone.y<<" "<<zone.z<<" "<<zone.interior<<" "<<zone.radius<<" "<<zone.name<<" "<<zone.msg;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <disabled_zone_t>: std::tr1::true_type {};
    template <> struct is_streameble_write<disabled_zone_t>: std::tr1::true_type {};
} // namespace serialization {

#include "core/streamer_coord_works.hpp"
namespace streamer {
    template <>
    struct coord_works<disabled_zone_t> {
        // Методы, необходимые для грида.
        inline static bool get_boundary_box(disabled_zone_t const& item, float& x_min, float& y_min, float& z_min, float& x_max, float& y_max, float& z_max) {
            return create_sphere_boundary_box(item, item.radius, x_min, y_min, z_min, x_max, y_max, z_max);
        }
        inline static int get_interior(disabled_zone_t const& item) {return item.interior;}
        inline static int get_world(disabled_zone_t const& item) {return world_any;}

        // Методы, необходимые для упорядочивания результатов поиска
        typedef int metric_t;
        inline static metric_t get_metric(disabled_zone_t const& item, pos3 const& pos) {
            return 0;
        }

        // Методы, необходимые для детальной проверки
        inline static bool get_is_point_in(disabled_zone_t const& item, pos3 const& pos) {
            return true;
        }
    };
} // namespace streamer {

#endif // DISABLE_ONFOOT_ENTER_T_HPP
