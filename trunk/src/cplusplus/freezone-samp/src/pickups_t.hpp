#ifndef PICKUPS_T_HPP
#define PICKUPS_T_HPP
#include "sphere_t.hpp"
#include <istream>
#include <functional>

struct pickup_t: sphere_t {
    int model;
    int type;
    // Цвет по моим тестам не работает
    pickup_t(float x = .0f, float y = .0f, float z = .0f, int interior = 0, int world = 0, float radius = .0f, int model = 0, int type = 0)
        :model(model)
        ,type(type)
        ,sphere_t(x, y, z, interior, world, radius)
    {}
    bool operator==(pickup_t const& right) const {
        return model == right.model
            && type == type
            && sphere_t::operator==(right)
            ;
    }
    bool operator<(pickup_t const& right) const {
        if (model == right.model) {
            if (type == right.type) {
                return sphere_t::operator<(right);
            }
            return type < right.type;
        }
        return model < right.model;
    }
};

struct pickup_only_coords_less: public std::binary_function<pickup_t, pickup_t, bool> {
    bool operator()(pickup_t const& left, pickup_t const& right) const {
        if (left.model == right.model) {
            if (left.type == right.type) {
                if (left.x == right.x) {
                    if (left.y == right.y) {
                        return left.z < right.z;
                    }
                    return left.y < right.y;
                }
                return left.x < right.x;
            }
            return left.type < right.type;
        }
        return left.model < right.model;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, pickup_t& pickup) {
    return is>>pickup.x>>pickup.y>>pickup.z>>pickup.interior>>pickup.world>>pickup.radius>>pickup.model>>pickup.type;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <pickup_t>: std::tr1::true_type {};
} // namespace serialization {

#include <core/streamer_coord_works.hpp>
namespace streamer {
    template <> struct coord_works<pickup_t>: coord_works<sphere_t> {};
} // namespace streamer {
#endif // PICKUPS_T_HPP
