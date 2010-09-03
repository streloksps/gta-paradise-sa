#include "config.hpp"
#include "messages_params.hpp"
#include "core/buffer/buffer.hpp"

messages_params::messages_params(buffer_ptr_t const& buffer_ptr): buffer_ptr(buffer_ptr) {
}

messages_params::~messages_params() {
}

messages_params& messages_params::operator()(std::string const& name, std::string const& value) {
    buffer_ptr->value_replace(name, value);
    return *this;
}

messages_params& messages_params::operator()(std::string const& name, boost::format const& formated_value) {
    buffer_ptr->value_replace(name, formated_value.str());
    return *this;
}

messages_params& messages_params::operator()(std::string const& name, int value) {
    buffer_ptr->value_replace(name, (boost::format("%1%") % value).str());
    return *this;
}

messages_params& messages_params::operator()(std::string const& name, unsigned int value) {
    buffer_ptr->value_replace(name, (boost::format("%1%") % value).str());
    return *this;
}

messages_params& messages_params::operator()(std::string const& name, float value, int precision) {
    buffer_ptr->value_replace(name, (boost::format("%1%") % value /*boost::io::group(std::setprecision(precision), value)*/).str());
    return *this;
}

messages_params& messages_params::operator()(std::string const& name) {
    buffer_ptr->values.erase(name);
    return *this;
}

messages_params& messages_params::operator()(char const* name) {
    buffer_ptr->values.erase(name);
    return *this;
}

std::string messages_params::process_all_vars(std::string const& str) const {
    return buffer_ptr->process_all_vars(str);
}
