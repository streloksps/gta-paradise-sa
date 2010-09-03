#ifndef MESSAGES_SENDER_HPP
#define MESSAGES_SENDER_HPP
#include <string>
#include "core/buffer/buffer_fwd.hpp"
#include "messages_writers.hpp"

class messages_sender {
    buffer_ptr_c_t buffer_ptr_c;
public:
    messages_sender(buffer_ptr_c_t const& buffer_ptr_c);
    ~messages_sender();

    messages_sender const& operator()(std::string const& value, msg_dests const& dests) const;

    template <typename manipulator_t>
    inline messages_sender const& operator()(std::string const& value, manipulator_t const& manipulator) const {
        return operator()(value, msg_dests() + manipulator);
    }
};
/*
    Можно использовать так:
    todo: написать :)
*/
#endif // MESSAGES_SENDER_HPP
