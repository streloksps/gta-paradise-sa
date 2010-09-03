#ifndef NPCS_HANDLERS_HPP
#define NPCS_HANDLERS_HPP
#include <boost/foreach.hpp>
#include "core/npcs.hpp"
#include "container/container_handlers.hpp"
// «апускает метод у всех итемов всех ботов

template<typename event_class_t>
inline void npcs_execute_handlers(void (event_class_t::*event)()) {
    npcs::npcs_holder_t& npcs_holder = npcs::instance()->npcs_holder;
    BOOST_FOREACH(npcs::npcs_holder_t::value_type const& item_pair, npcs_holder) {
        container_execute_handlers(item_pair.second, event);
    }
}

template<typename event_class_t, typename arg1_t>
inline void npcs_execute_handlers(void (event_class_t::*event)(arg1_t), typename boost::call_traits<arg1_t>::param_type arg1) {
    npcs::npcs_holder_t& npcs_holder = npcs::instance()->npcs_holder;
    BOOST_FOREACH(npcs::npcs_holder_t::value_type const& item_pair, npcs_holder) {
        container_execute_handlers(item_pair.second, event, arg1);
    }
}
#endif // NPCS_HANDLERS_HPP
