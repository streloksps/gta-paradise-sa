#ifndef IPL_LOADER_HPP
#define IPL_LOADER_HPP
#include "ipl_loader_t.hpp"
#include "objects_t.hpp"
#include <set>
#include <vector>
#include <boost/filesystem.hpp>

typedef std::vector<ipl_loader_item_t>       ipl_loader_items_t;

/*
typedef std::set<object_dynamic_t>  objects_dynamic_t;
typedef std::set<object_static_t>  objects_static_t;
*/

class ipl_loader {
public:
    ipl_loader(boost::filesystem::path const& root_path, bool is_dump);
    ~ipl_loader();

    void load_items(ipl_loader_items_t const& items, std::set<object_dynamic_t>& objects);
    void load_items(ipl_loader_items_t const& items, std::set<object_static_t>& objects);
private:
    boost::filesystem::path root_path;
    bool is_dump;
    template <typename object_t>
    inline void load_items_impl(ipl_loader_items_t const& items, std::set<object_t>& objects);
    template <typename object_t>
    inline void load_item(boost::filesystem::path const& file_name, int world, std::set<object_t>& objects);
};
#endif // IPL_LOADER_HPP
