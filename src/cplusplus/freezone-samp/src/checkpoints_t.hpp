#ifndef CHECKPOINTS_T_HPP
#define CHECKPOINTS_T_HPP
#include "sphere_t.hpp"
#include <functional>
#include "core/player/player_fwd.hpp"

struct checkpoint_t: sphere_t {
    float size;
    // Цвет по моим тестам не работает
    checkpoint_t()
        :size(0.0f)
    {}
    checkpoint_t(float x, float y, float z, int interior, int world, float visibility_radius, float size)
        :size(size)
        ,sphere_t(x, y, z, interior, world, visibility_radius)
    {}

    bool operator==(checkpoint_t const& right) const {
        return size == right.size
            && sphere_t::operator==(right)
            ;
    }
    bool operator<(checkpoint_t const& right) const {
        if (size == right.size) return sphere_t::operator<(right);
        return size < right.size;
    }
};

typedef std::tr1::function<void (player_ptr_t const& player_ptr, int id)> checkpoint_event_t;

struct checkpoint_data_t {
    checkpoint_event_t on_enter;
    checkpoint_event_t on_leave;
    checkpoint_data_t(checkpoint_event_t const& on_enter, checkpoint_event_t const& on_leave): on_enter(on_enter), on_leave(on_leave) {}
    checkpoint_data_t() {}
};

#include <core/streamer_coord_works.hpp>
namespace streamer {
    template <> struct coord_works<checkpoint_t>: coord_works<sphere_t> {};
} // namespace streamer {

#endif // CHECKPOINTS_T_HPP
