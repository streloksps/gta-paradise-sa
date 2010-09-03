#ifndef MAP_ICONS_T_HPP
#define MAP_ICONS_T_HPP
#include "sphere_t.hpp"
#include <istream>

struct map_icon_t: sphere_t {
    int marker_type;
    // Цвет по моим тестам не работает
    map_icon_t(float x = .0f, float y = .0f, float z = .0f, int interior = 0, int world = 0, float radius = .0f, int marker_type = 0)
        :marker_type(marker_type)
        ,sphere_t(x, y, z, interior, world, radius)
    {}
    bool operator==(map_icon_t const& right) const {
        return marker_type == right.marker_type
            && sphere_t::operator==(right)
            ;
    }
    bool operator<(map_icon_t const& right) const {
        if (marker_type == right.marker_type) return sphere_t::operator<(right);
        return marker_type < right.marker_type;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, map_icon_t& icon) {
    return is>>icon.x>>icon.y>>icon.z>>icon.interior>>icon.world>>icon.radius>>icon.marker_type;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <map_icon_t>: std::tr1::true_type {};
} // namespace serialization {

#include <core/streamer_coord_works.hpp>
namespace streamer {
    template <> struct coord_works<map_icon_t>: coord_works<sphere_t> {};
} // namespace streamer {

#endif // MAP_ICONS_T_HPP
