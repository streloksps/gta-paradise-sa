#ifndef CONTAINER_HANDLERS_HPP
#define CONTAINER_HANDLERS_HPP
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <type_traits>
#include <boost/call_traits.hpp> 
#include <boost/utility/enable_if.hpp>
#include "container.hpp"



// Вызываем указанный обработчик у всех элементов контейнера и у самого контейнера
template<typename container_t, typename event_class_t>
inline void container_execute_handlers_impl(typename std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)()) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1));
}
template<typename container_t, typename event_class_t, typename arg1_t>
inline void container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t), typename boost::call_traits<arg1_t>::param_type arg1) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1)));
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t>
inline void container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2)));
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t>
inline void container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3)));
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
inline void container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4)));
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
inline void container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5)));
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t>
inline void container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5), std::tr1::ref(arg6)));
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t, typename arg7_t>
inline void container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, typename boost::call_traits<arg7_t>::param_type arg7) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5), std::tr1::ref(arg6), std::tr1::ref(arg7)));
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t, typename arg7_t, typename arg8_t>
inline void container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, typename boost::call_traits<arg7_t>::param_type arg7, typename boost::call_traits<arg8_t>::param_type arg8) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    std::for_each(handlers.begin(), handlers.end(), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5), std::tr1::ref(arg6), std::tr1::ref(arg7), std::tr1::ref(arg8)));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename container_t, typename event_class_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(), typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event);
}
template<typename container_t, typename event_class_t, typename arg1_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event, arg1);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event, arg1, arg2);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, arg6);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t, typename arg7_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, typename boost::call_traits<arg7_t>::param_type arg7, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t, typename arg7_t, typename arg8_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, typename boost::call_traits<arg7_t>::param_type arg7, typename boost::call_traits<arg8_t>::param_type arg8, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename container_t, typename event_class_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(), typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1)(cont);
    container_execute_handlers_impl(cont, event);
}
template<typename container_t, typename event_class_t, typename arg1_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1))(cont);
    container_execute_handlers_impl(cont, event, arg1);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2))(cont);
    container_execute_handlers_impl(cont, event, arg1, arg2);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3))(cont);
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4))(cont);
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5))(cont);
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5), std::tr1::ref(arg6))(cont);
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, arg6);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t, typename arg7_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, typename boost::call_traits<arg7_t>::param_type arg7, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5), std::tr1::ref(arg6), std::tr1::ref(arg7))(cont);
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t, typename arg7_t, typename arg8_t>
inline void container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, void (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, typename boost::call_traits<arg7_t>::param_type arg7, typename boost::call_traits<arg8_t>::param_type arg8, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5), std::tr1::ref(arg6), std::tr1::ref(arg7), std::tr1::ref(arg8))(cont);
    container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}






enum container_execute_rezult_e {
    container_execute_rezult_false_if_have_one_false
    ,container_execute_rezult_true_if_have_one_true
};




//Возращает ложь, если хотя бы один вызов вернул ложь. Вызываются все обработчики
template<typename container_t, typename event_class_t>
inline bool container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(), container_execute_rezult_e rezult_type) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    if (!handlers.empty()) {
        std::vector<bool> rezults;
        rezults.reserve(handlers.size());
        std::transform(handlers.begin(), handlers.end(), std::back_inserter(rezults), std::tr1::bind(event, std::tr1::placeholders::_1));
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return *std::min_element(rezults.begin(), rezults.end());
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return *std::max_element(rezults.begin(), rezults.end());
        }
    }
    else {
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return true;
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return false;
        }
    }
    throw std::runtime_error("container_execute_handlers_impl error");
}
template<typename container_t, typename event_class_t, typename arg1_t>
inline bool container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t), typename boost::call_traits<arg1_t>::param_type arg1, container_execute_rezult_e rezult_type) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    if (!handlers.empty()) {
        std::vector<bool> rezults;
        rezults.reserve(handlers.size());
        std::transform(handlers.begin(), handlers.end(), std::back_inserter(rezults), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1)));
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return *std::min_element(rezults.begin(), rezults.end());
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return *std::max_element(rezults.begin(), rezults.end());
        }
    }
    else {
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return true;
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return false;
        }
    }
    throw std::runtime_error("container_execute_handlers_impl error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t>
inline bool container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, container_execute_rezult_e rezult_type) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    if (!handlers.empty()) {
        std::vector<bool> rezults;
        rezults.reserve(handlers.size());
        std::transform(handlers.begin(), handlers.end(), std::back_inserter(rezults), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2)));
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return *std::min_element(rezults.begin(), rezults.end());
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return *std::max_element(rezults.begin(), rezults.end());
        }
    }
    else {
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return true;
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return false;
        }
    }
    throw std::runtime_error("container_execute_handlers_impl error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t>
inline bool container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, container_execute_rezult_e rezult_type) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    if (!handlers.empty()) {
        std::vector<bool> rezults;
        rezults.reserve(handlers.size());
        std::transform(handlers.begin(), handlers.end(), std::back_inserter(rezults), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3)));
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return *std::min_element(rezults.begin(), rezults.end());
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return *std::max_element(rezults.begin(), rezults.end());
        }
    }
    else {
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return true;
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return false;
        }
    }
    throw std::runtime_error("container_execute_handlers_impl error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
inline bool container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, container_execute_rezult_e rezult_type) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    if (!handlers.empty()) {
        std::vector<bool> rezults;
        rezults.reserve(handlers.size());
        std::transform(handlers.begin(), handlers.end(), std::back_inserter(rezults), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4)));
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return *std::min_element(rezults.begin(), rezults.end());
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return *std::max_element(rezults.begin(), rezults.end());
        }
    }
    else {
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return true;
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return false;
        }
    }
    throw std::runtime_error("container_execute_handlers_impl error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
inline bool container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, container_execute_rezult_e rezult_type) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    if (!handlers.empty()) {
        std::vector<bool> rezults;
        rezults.reserve(handlers.size());
        std::transform(handlers.begin(), handlers.end(), std::back_inserter(rezults), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5)));
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return *std::min_element(rezults.begin(), rezults.end());
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return *std::max_element(rezults.begin(), rezults.end());
        }
    }
    else {
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return true;
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return false;
        }
    }
    throw std::runtime_error("container_execute_handlers_impl error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t>
inline bool container_execute_handlers_impl(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, container_execute_rezult_e rezult_type) {
    std::vector<std::tr1::shared_ptr<event_class_t> >& handlers = cont->template get_items<event_class_t>();
    if (!handlers.empty()) {
        std::vector<bool> rezults;
        rezults.reserve(handlers.size());
        std::transform(handlers.begin(), handlers.end(), std::back_inserter(rezults), std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5), std::tr1::ref(arg6)));
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return *std::min_element(rezults.begin(), rezults.end());
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return *std::max_element(rezults.begin(), rezults.end());
        }
    }
    else {
        if (container_execute_rezult_false_if_have_one_false == rezult_type) {
            return true;
        }
        else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
            return false;
        }
    }
    throw std::runtime_error("container_execute_handlers_impl error");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename container_t, typename event_class_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(), container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    return container_execute_handlers_impl(cont, event, rezult_type);
}
template<typename container_t, typename event_class_t, typename arg1_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t), typename boost::call_traits<arg1_t>::param_type arg1, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    return container_execute_handlers_impl(cont, event, arg1, rezult_type);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    return container_execute_handlers_impl(cont, event, arg1, arg2, rezult_type);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    return container_execute_handlers_impl(cont, event, arg1, arg2, arg3, rezult_type);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    return container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, rezult_type);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    return container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, rezult_type);
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::disable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    return container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, arg6, rezult_type);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename container_t, typename event_class_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(), container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    bool rezult1 = std::tr1::bind(event, std::tr1::placeholders::_1)(cont);
    bool rezult2 = container_execute_handlers_impl(cont, event, rezult_type);
    if (container_execute_rezult_false_if_have_one_false == rezult_type) {
        return std::min(rezult1, rezult2);
    }
    else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
        return std::max(rezult1, rezult2);
    }
    throw std::runtime_error("bool container_execute_handlers error");
}
template<typename container_t, typename event_class_t, typename arg1_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t), typename boost::call_traits<arg1_t>::param_type arg1, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    bool rezult1 = std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1))(cont);
    bool rezult2 = container_execute_handlers_impl(cont, event, arg1, rezult_type);
    if (container_execute_rezult_false_if_have_one_false == rezult_type) {
        return std::min(rezult1, rezult2);
    }
    else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
        return std::max(rezult1, rezult2);
    }
    throw std::runtime_error("bool container_execute_handlers error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    bool rezult1 = std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2))(cont);
    bool rezult2 = container_execute_handlers_impl(cont, event, arg1, arg2, rezult_type);
    if (container_execute_rezult_false_if_have_one_false == rezult_type) {
        return std::min(rezult1, rezult2);
    }
    else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
        return std::max(rezult1, rezult2);
    }
    throw std::runtime_error("bool container_execute_handlers error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    bool rezult1 = std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3))(cont);
    bool rezult2 = container_execute_handlers_impl(cont, event, arg1, arg2, arg3, rezult_type);
    if (container_execute_rezult_false_if_have_one_false == rezult_type) {
        return std::min(rezult1, rezult2);
    }
    else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
        return std::max(rezult1, rezult2);
    }
    throw std::runtime_error("bool container_execute_handlers error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    bool rezult1 = std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4))(cont);
    bool rezult2 = container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, rezult_type);
    if (container_execute_rezult_false_if_have_one_false == rezult_type) {
        return std::min(rezult1, rezult2);
    }
    else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
        return std::max(rezult1, rezult2);
    }
    throw std::runtime_error("bool container_execute_handlers error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    bool rezult1 = std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5))(cont);
    bool rezult2 = container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, rezult_type);
    if (container_execute_rezult_false_if_have_one_false == rezult_type) {
        return std::min(rezult1, rezult2);
    }
    else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
        return std::max(rezult1, rezult2);
    }
    throw std::runtime_error("bool container_execute_handlers error");
}
template<typename container_t, typename event_class_t, typename arg1_t, typename arg2_t, typename arg3_t, typename arg4_t, typename arg5_t, typename arg6_t>
inline bool container_execute_handlers(std::tr1::shared_ptr<container_t> const& cont, bool (event_class_t::*event)(arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t), typename boost::call_traits<arg1_t>::param_type arg1, typename boost::call_traits<arg2_t>::param_type arg2, typename boost::call_traits<arg3_t>::param_type arg3, typename boost::call_traits<arg4_t>::param_type arg4, typename boost::call_traits<arg5_t>::param_type arg5, typename boost::call_traits<arg6_t>::param_type arg6, container_execute_rezult_e rezult_type = container_execute_rezult_false_if_have_one_false, typename boost::enable_if<std::tr1::is_base_of<event_class_t, container_t> >::type* = 0) {
    bool rezult1 = std::tr1::bind(event, std::tr1::placeholders::_1, std::tr1::ref(arg1), std::tr1::ref(arg2), std::tr1::ref(arg3), std::tr1::ref(arg4), std::tr1::ref(arg5), std::tr1::ref(arg6))(cont);
    bool rezult2 = container_execute_handlers_impl(cont, event, arg1, arg2, arg3, arg4, arg5, arg6, rezult_type);
    if (container_execute_rezult_false_if_have_one_false == rezult_type) {
        return std::min(rezult1, rezult2);
    }
    else if (container_execute_rezult_true_if_have_one_true == rezult_type) {
        return std::max(rezult1, rezult2);
    }
    throw std::runtime_error("bool container_execute_handlers error");
}

#endif // CONTAINER_HANDLERS_HPP
