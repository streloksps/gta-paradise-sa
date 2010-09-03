#ifndef ACTIVE_POINTS_HPP
#define ACTIVE_POINTS_HPP
#include <functional>
#include <string>
#include <boost/optional.hpp>
#include "core/player/player_fwd.hpp"
#include "basic_types.hpp"
#include "limit.hpp"

typedef int active_point_handle_t;
typedef std::tr1::function<void (player_ptr_t const& player_ptr, active_point_handle_t handle)> active_point_event_t;
typedef std::tr1::function<std::string (active_point_handle_t handle, player_ptr_t const& player_ptr)> active_point_description_getter_t;

// Возращает истину, если можно использовать активную точку (если не задан - то использовать можно всегда)
typedef std::tr1::function<bool (player_ptr_t const& player_ptr, active_point_handle_t handle)> is_can_use_checker_t;

struct active_point_def_t {
    // Описания положения активной точки
    pos3                    pos;                        // Позиция активной точки
    int                     pickup_id;                  // Ид модели пикапа или указатель чтобы использовался чик поинт
    float                   visibility_radius;          // Радиус, при входе в который игрок начинает видеть активную точку
    float                   active_radius;              // Радиус, при попадании в который начинается взаимодействие игрока с активной точкой

    // Описание иконки на карте, обозначающей активную точку
    int                     mapicon_marker_type;        // Картинка иконки
    float                   mapicon_visibility_radius;  // Радиус, при попадании в который игрок видит иконку. Если = 0.0f, то иконка не создается


    // События входа/выхода в зону active_radius
    active_point_event_t    on_enter;
    active_point_event_t    on_leave;

    // Обработчик должен вернуть текст для отображения над данной точкой. Начинает запрашиваться при поподании игрока в visibility_radius
    active_point_description_getter_t description_getter;

    // Описание лимитов
    limit_def_t             limit_def;

};

// Ид модельки пикапа, который обозначает использовать вместо пикапа чикпоинт
enum {checkpoint_pickup_id = -1};

class active_points
{
public:
    active_points();
    virtual ~active_points();

public:
    active_point_handle_t add(pos3 const& pos, int pickup_id, float visibility_radius, float active_radius);
};
#endif // ACTIVE_POINTS_HPP
