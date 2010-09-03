#ifndef SPHERE_T_HPP
#define SPHERE_T_HPP
#include "basic_types.hpp"

struct sphere_t: pos3 {
    float radius;
    sphere_t(float x = .0f, float y = .0f, float z = .0f, int interior = 0, int world = 0, float radius = .0f)
        :radius(radius)
        ,pos3(x, y, z, interior, world)
    {}
    bool operator==(sphere_t const& right) const {
        return radius == right.radius
            && pos3::operator==(right)
            ;
    }
    bool operator<(sphere_t const& right) const {
        if (radius == right.radius) {
            return pos3::operator<(right);
        }
        return radius < right.radius;
    }
};

#include "core/streamer_coord_works.hpp"
namespace streamer {
    template <> struct coord_works<sphere_t> {
        // Методы, необходимые для грида.
        inline static bool get_boundary_box(sphere_t const& item, float& x_min, float& y_min, float& z_min, float& x_max, float& y_max, float& z_max) {
            return create_sphere_boundary_box(item, item.radius, x_min, y_min, z_min, x_max, y_max, z_max);
        }
        inline static int get_interior(sphere_t const& item) {return item.interior;}
        inline static int get_world(sphere_t const& item) {return item.world;}

        // Методы, необходимые для упорядочивания результатов поиска
        typedef float metric_t;
        inline static metric_t get_metric(sphere_t const& item, pos3 const& pos) {
            return get_points_metric_square_noiw(item, pos);
        }

        // Методы, необходимые для детальной проверки
        inline static bool get_is_point_in(sphere_t const& item, pos3 const& pos) {
            return is_points_in_sphere(item, pos, item.radius);
        }
    };
} // namespace streamer {

#endif // SPHERE_T_HPP
