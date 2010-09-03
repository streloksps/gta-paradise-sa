#ifndef OBJECTS_T_HPP
#define OBJECTS_T_HPP
#include <istream>
#include <ostream>
#include "basic_types.hpp"

struct object_static_t {
    int model_id;
    float x;
    float y;
    float z;
    float rx;
    float ry;
    float rz;
    object_static_t(int model_id = 0, float x = .0f, float y = .0f, float z = .0f, float rx = .0f, float ry = .0f, float rz = .0f)
        :model_id(model_id)
        ,x(x)
        ,y(y)
        ,z(z)
        ,rx(rx)
        ,ry(ry)
        ,rz(rz)
        {}
    bool operator==(object_static_t const& right) const {
        return model_id == right.model_id 
            && x == right.x
            && y == right.y
            && z == right.z
            && rx == right.rx
            && ry == right.ry
            && rz == right.rz
            ;
    }
    bool operator<(object_static_t const& right) const {
        if (model_id == right.model_id) {
            if (x == right.x) {
                if (y == right.y) {
                    if (z == right.z) {
                        if (rx == right.rx) {
                            if (ry == right.ry) {
                                return rz < right.rz;
                            }
                            return ry < right.ry;
                        }
                        return rx < right.rx;
                    }
                    return z < right.z;
                }
                return y < right.y;
            }
            return x < right.x;
        }
        return model_id < right.model_id;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, object_static_t& obj) {
    return is>>obj.model_id>>obj.x>>obj.y>>obj.z>>obj.rx>>obj.ry>>obj.rz;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, object_static_t const& obj) {
    return os<<obj.model_id<<" "<<obj.x<<" "<<obj.y<<" "<<obj.z<<" "<<obj.rx<<" "<<obj.ry<<" "<<obj.rz;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <object_static_t>: std::tr1::true_type {};
    template <> struct is_streameble_write<object_static_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct object_def_t {
    float visibility;   // Как далеко видно объект
    float radius;       // Размер объекта - используется для сортировки
    object_def_t(): visibility(0.0f), radius(0.0f) {}
    object_def_t(float visibility, float radius): visibility(visibility), radius(radius) {}
    
    bool operator==(object_def_t const& right) const {
        return visibility == right.visibility
            && radius == right.radius;
    }
    bool operator<(object_def_t const& right) const {
        if (visibility == right.visibility) {
            return radius < right.radius;
        }
        return visibility < right.visibility;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, object_def_t& def) {
    return is>>def.visibility>>def.radius;
}
template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, object_def_t const& def) {
    return os<<def.visibility<<" "<<def.radius;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <object_def_t>: std::tr1::true_type {};
    template <> struct is_streameble_write<object_def_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct object_dynamic_t: public pos6, public object_def_t {
    int model_id;
    object_dynamic_t(): model_id(0) {}
    object_dynamic_t(int model_id, float x, float y, float z, int interior, int world, float rx, float ry, float rz)
        :model_id(model_id), pos6(x, y, z, interior, world, rx, ry, rz) {}

    bool operator==(object_dynamic_t const& right) const {
        return model_id == right.model_id
            && pos6::operator==(right)
            && object_def_t::operator==(right)
            ;
    }
    bool operator<(object_dynamic_t const& right) const {
        if (model_id == right.model_id) {
            if (pos6::operator==(right)) {
                return object_def_t::operator<(right);
            }
            return pos6::operator<(right);
        }
        return model_id < right.model_id;
    }
};

// Примечание: велечины visibility и radius не загружаются. Предпологается что будет отдельный проход, их заполняющий
template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, object_dynamic_t& object) {
    return is>>object.model_id>>object.x>>object.y>>object.z>>object.interior>>object.world>>object.rx>>object.ry>>object.rz;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, object_dynamic_t const& object) {
    return os<<object.model_id<<" "<<object.x<<" "<<object.y<<" "<<object.z<<" "<<object.interior<<" "<<object.world<<" "<<object.rx<<" "<<object.ry<<" "<<object.rz;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <object_dynamic_t>: std::tr1::true_type {};
    template <> struct is_streameble_write<object_dynamic_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "core/streamer_coord_works.hpp"
namespace streamer {
    template <>
    struct coord_works<object_dynamic_t> {
        // Методы, необходимые для грида.
        inline static bool get_boundary_box(object_dynamic_t const& item, float& x_min, float& y_min, float& z_min, float& x_max, float& y_max, float& z_max) {
            return create_sphere_boundary_box(item, item.visibility + item.radius, x_min, y_min, z_min, x_max, y_max, z_max);
        }
        inline static int get_interior(object_dynamic_t const& item) {return item.interior;}
        inline static int get_world(object_dynamic_t const& item) {return item.world;}

        // Методы, необходимые для упорядочивания результатов поиска
        typedef float metric_t;
        inline static metric_t get_metric(object_dynamic_t const& item, pos3 const& pos) {
            float rezult = get_points_metric_square_noiw(item, pos);
            rezult -= item.radius * item.radius;
            if (rezult < 0.0f) rezult = 0.0f;
            return rezult;
        }

        // Методы, необходимые для детальной проверки
        inline static bool get_is_point_in(object_dynamic_t const& item, pos3 const& pos) {
            float total_radius = item.visibility + item.radius;
            return get_points_metric_square_noiw(item, pos)
                <  total_radius * total_radius;
        }
    };
} // namespace streamer {

#endif // OBJECTS_T_HPP
