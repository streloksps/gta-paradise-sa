#ifndef MESSAGES_PARAMS_HPP
#define MESSAGES_PARAMS_HPP
#include <string>
#include <boost/format.hpp>
#include "core/buffer/buffer_fwd.hpp"

class messages_params {
    buffer_ptr_t buffer_ptr;
public:
    messages_params(buffer_ptr_t const& buffer_ptr);
    ~messages_params();

    // Обновляет запись
    messages_params& operator()(std::string const& name, std::string const& value);
    messages_params& operator()(std::string const& name, boost::format const& formated_value);
    messages_params& operator()(std::string const& name, int value);
    messages_params& operator()(std::string const& name, unsigned int value);
    messages_params& operator()(std::string const& name, float value, int precision = 0);

    // Удаляет запись
    messages_params& operator()(std::string const& name);
    messages_params& operator()(char const* name);

    // Дампит класс со специальным методом
    template <typename T>
    messages_params& operator()(T const& item);
    
    std::string process_all_vars(std::string const& str) const;
};

template <typename T>
messages_params& messages_params::operator()(T const& item) {
    item.dump_to_params(*this);
    return *this;
}

#endif // MESSAGES_PARAMS_HPP
