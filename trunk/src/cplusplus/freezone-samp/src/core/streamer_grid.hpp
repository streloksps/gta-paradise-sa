#ifndef STREAMER_GRID_HPP
#define STREAMER_GRID_HPP
#include "streamer_coord_works.hpp"
#include "basic_types.hpp"
#include <boost/functional/hash.hpp>
#include <algorithm>
#include <iterator>
#include <list>
#include <functional>
#include <unordered_map>

namespace streamer {
    // Класс грида - аптимизация по скорости работы стримера
    template <typename item_t, typename key_t, int grid_step>
    class grid_t {
    public:
        typedef key_t key_type;
        typedef std::list<key_t> keys_t;

        void grid_add(item_t const& item, key_t key);
        void grid_remove(item_t const& item, key_t key);
        void grid_get(keys_t& keys, pos3 const& pos);
        void grid_clear();
    private:
        inline int get_grided_coord(float coord) {return static_cast<int>(coord)/grid_step;};

        /************************************************************************/
        struct grid_key_t {
            int x;
            int y;
            int z;
            bool is_has_xyz;
            int interior;
            int world;
            grid_key_t(int x, int y, int z, int interior, int world): x(x), y(y), z(z), is_has_xyz(true), interior(interior), world(world) {}
            grid_key_t(int interior, int world): x(0), y(0), z(0), is_has_xyz(false), interior(interior), world(world) {}
            bool operator==(grid_key_t const& right) const {
                return x == right.x
                    && y == right.y
                    && z == right.z
                    && is_has_xyz == right.is_has_xyz
                    && interior == right.interior
                    && world == right.world
                    ;
            }
        };
        struct grid_key_t_hasher: std::unary_function<grid_key_t, std::size_t> {
            std::size_t operator()(grid_key_t const& key) const {
                std::size_t seed = 0;
                boost::hash_combine(seed, key.x);
                boost::hash_combine(seed, key.y);
                boost::hash_combine(seed, key.z);
                boost::hash_combine(seed, key.is_has_xyz);
                boost::hash_combine(seed, key.interior);
                boost::hash_combine(seed, key.world);
                return seed;
            }
        };

        typedef std::tr1::unordered_map<grid_key_t, keys_t, grid_key_t_hasher> grid_data_t;
        grid_data_t grid_data;

        void grid_add_handler(grid_key_t const& item, key_t key);
        void grid_remove_handler(grid_key_t const& item, key_t key);


        typedef void (grid_t<item_t, key_t, grid_step>::*handler_t)(grid_key_t const& item, key_t key);
        template <handler_t handler> void enumarate_item_key(item_t const& item, key_t key) {
            int interior = coord_works<item_t>::get_interior(item);
            int world = coord_works<item_t>::get_world(item);
            int x_min, y_min, z_min, x_max, y_max, z_max;
            bool is_has_xyz;
            {
                float coord_x_min, coord_y_min, coord_z_min, coord_x_max, coord_y_max, coord_z_max;
                if (is_has_xyz = coord_works<item_t>::get_boundary_box(item, coord_x_min, coord_y_min, coord_z_min, coord_x_max, coord_y_max, coord_z_max)) {
                    x_min = get_grided_coord(coord_x_min);
                    y_min = get_grided_coord(coord_y_min);
                    z_min = get_grided_coord(coord_z_min);
                    x_max = get_grided_coord(coord_x_max);
                    y_max = get_grided_coord(coord_y_max);
                    z_max = get_grided_coord(coord_z_max);
                }
            }

            if (is_has_xyz) {
                for (int x = x_min; x <= x_max; ++x) {
                    for (int y = y_min; y <= y_max; ++y) {
                        for (int z = z_min; z <= z_max; ++z) {
                            grid_key_t grid_item(x, y, z, interior, world);
                            (this->*handler)(grid_item, key);
                        }
                    }
                }
            }
            else {
                grid_key_t grid_item(interior, world);
                (this->*handler)(grid_item, key);
            }
        }
    };

    template <typename item_t, typename key_t, int grid_step>
    void grid_t<item_t, key_t, grid_step>::grid_add(item_t const& item, key_t key) {
        enumarate_item_key<&grid_t<item_t, key_t, grid_step>::grid_add_handler>(item, key);
    }

    template <typename item_t, typename key_t, int grid_step>
    void streamer::grid_t<item_t, key_t, grid_step>::grid_add_handler(grid_key_t const& item, key_t key) {
        keys_t& keys = grid_data[item];
        keys.push_back(key);
    }

    template <typename item_t, typename key_t, int grid_step>
    void grid_t<item_t, key_t, grid_step>::grid_remove(item_t const& item, key_t key) {
        enumarate_item_key<&grid_t<item_t, key_t, grid_step>::grid_remove_handler>(item, key);
    }

    template <typename item_t, typename key_t, int grid_step>
    void streamer::grid_t<item_t, key_t, grid_step>::grid_remove_handler(grid_key_t const& item, key_t key) {
        typename grid_data_t::iterator it = grid_data.find(item);
        if (grid_data.end() != it) {
            it->second.remove(key);
            if (it->second.empty()) {
                // Удаляем еще сам итем грида
                grid_data.erase(it);
            }
        }
        else {
            assert(false);
        }
    }

    template <typename item_t, typename key_t, int grid_step>
    void grid_t<item_t, key_t, grid_step>::grid_get(keys_t& keys, pos3 const& pos) {
        assert(interior_any != pos.interior && world_any != pos.world);
        keys.clear();

        typename grid_data_t::const_iterator it;
        int x = get_grided_coord(pos.x);
        int y = get_grided_coord(pos.y);
        int z = get_grided_coord(pos.z);
        
        it = grid_data.find(grid_key_t(x, y, z, pos.interior, pos.world));
        if (grid_data.end() != it) {
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(keys));
        }

        it = grid_data.find(grid_key_t(pos.interior, pos.world));
        if (grid_data.end() != it) {
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(keys));
        }

        it = grid_data.find(grid_key_t(x, y, z, interior_any, pos.world));
        if (grid_data.end() != it) {
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(keys));
        }

        it = grid_data.find(grid_key_t(interior_any, pos.world));
        if (grid_data.end() != it) {
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(keys));
        }

        it = grid_data.find(grid_key_t(x, y, z, pos.interior, world_any));
        if (grid_data.end() != it) {
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(keys));
        }

        it = grid_data.find(grid_key_t(pos.interior, world_any));
        if (grid_data.end() != it) {
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(keys));
        }

        it = grid_data.find(grid_key_t(x, y, z, interior_any, world_any));
        if (grid_data.end() != it) {
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(keys));
        }

        it = grid_data.find(grid_key_t(interior_any, world_any));
        if (grid_data.end() != it) {
            std::copy(it->second.begin(), it->second.end(), std::back_inserter(keys));
        }
    }

    template <typename item_t, typename key_t, int grid_step>
    void streamer::grid_t<item_t, key_t, grid_step>::grid_clear() {
        grid_data.clear();
    }

} // namespace streamer {

#endif // STREAMER_GRID_HPP
