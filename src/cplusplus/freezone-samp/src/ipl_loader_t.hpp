#ifndef IPL_LOADER_T_HPP
#define IPL_LOADER_T_HPP
#include "basic_loader_t.hpp"

struct ipl_loader_item_t: basic_loader_item_t {
};

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <ipl_loader_item_t>: std::tr1::true_type {};
} // namespace serialization {
#endif // IPL_LOADER_T_HPP
