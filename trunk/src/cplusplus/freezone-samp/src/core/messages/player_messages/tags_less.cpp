#include "config.hpp"
#include "tags_less.hpp"
#include "tags.hpp"

bool tags_less::operator()(std::string const& left, std::string const& right) const {
    tags tag_left(left);
    tags tag_right(right);
    unsigned int sort_left = tag_left.get_closed_tag_val<unsigned int>("sort", 0, 0xFFFF);
    unsigned int sort_right = tag_right.get_closed_tag_val<unsigned int>("sort", 0, 0xFFFF);
    if (sort_left == sort_right) {
        return tag_left.get_untaget_str() < tag_right.get_untaget_str();
    }
    return sort_left < sort_right;
}
