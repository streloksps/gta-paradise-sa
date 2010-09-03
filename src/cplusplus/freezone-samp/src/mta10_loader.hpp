#ifndef MTA10_LOADER_HPP
#define MTA10_LOADER_HPP
#include "mta10_loader_t.hpp"
#include "objects_t.hpp"
#include "vehicles_t.hpp"
#include <set>
#include <vector>
#include <istream>
#include <boost/filesystem.hpp>

typedef std::vector<mta10_loader_item_t>       mta10_loader_items_t;
typedef std::vector<mta10_loader_vehicle_t>    mta10_loader_vehicles_t;

typedef std::set<object_dynamic_t>  objects_dynamic_t;
typedef std::set<pos_vehicle>       vehicles_dynamic_t;
typedef std::vector<pos4>           spawn_points_t;

class mta10_loader {
public:
    mta10_loader(boost::filesystem::path const& root_path);
    ~mta10_loader();

    void load_items(mta10_loader_items_t const& items, objects_dynamic_t& objects);
    void load_items(mta10_loader_items_t const& items, spawn_points_t& spawn_points);
    void load_items(mta10_loader_vehicles_t const& items, vehicles_dynamic_t& vehicles);
private:
    boost::filesystem::path root_path;
};
#endif // MTA10_LOADER_HPP
