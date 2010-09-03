#ifndef TAGS_HPP
#define TAGS_HPP
#include <string>
#include <vector>
#include <map>
#include <set>

class tags {
    struct closed_tag_item {
        std::string val;
        size_t start;
        size_t end;
        closed_tag_item(std::string const& val, size_t start): val(val), start(start), end(-1) {}
    };
    typedef std::vector<closed_tag_item> closed_tag_items_t;
    typedef std::map<std::string, closed_tag_items_t> closed_tags_t;
    
    std::string src_str;
    std::string rezult_str;
    closed_tags_t closed_tags;
    void process_closed_tags();
public:
    typedef std::set<size_t> poses_t;

    tags(std::string const& str);

    std::string get_untaget_str() const;
    template<typename T>
    T get_closed_tag_val(std::string const& tag_name, size_t pos, T const& def_val) const;
    poses_t get_tags_poses() const;
};
#endif // TAGS_HPP
