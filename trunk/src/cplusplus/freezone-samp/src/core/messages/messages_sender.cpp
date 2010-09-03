#include "config.hpp"
#include "messages_sender.hpp"
#include <algorithm>
#include <functional>
#include "core/buffer/buffer.hpp"

messages_sender::messages_sender(buffer_ptr_c_t const& buffer_ptr_c): buffer_ptr_c(buffer_ptr_c) {
}
messages_sender::~messages_sender() {
}

messages_sender const& messages_sender::operator()(std::string const& value, msg_dests const& dests) const {
    if (!dests.message_writers.empty()) {
        std::string msg_to_send = buffer_ptr_c->process_all_vars(value);
        for_each(dests.message_writers.begin(), dests.message_writers.end(), bind(&messages_writer::message_write, std::tr1::placeholders::_1, std::tr1::cref(msg_to_send)));
    }
    return *this;
}
