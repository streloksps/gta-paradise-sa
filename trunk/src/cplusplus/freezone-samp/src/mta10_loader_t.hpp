#ifndef MTA10_LOADER_T_HPP
#define MTA10_LOADER_T_HPP
#include "basic_loader_t.hpp"

struct mta10_loader_item_t: basic_loader_item_t {
};

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <mta10_loader_item_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mta10_loader_vehicle_t: public mta10_loader_item_t {
    int respawn_delay;
    mta10_loader_vehicle_t():respawn_delay(0) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, mta10_loader_vehicle_t& item) {
    is>>item.world>>item.respawn_delay;
    if (is.eof()) {
        is.setstate(std::ios_base::failbit);
    }
    else {
        std::getline(is, item.file_name);
        boost::trim(item.file_name);
        if (item.file_name.empty()) {
            is.setstate(std::ios_base::failbit);
        }
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <mta10_loader_vehicle_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MTA10_LOADER_T_HPP
