#include "config.hpp"
#include "buffer.hpp"
#include <cassert>
#include <algorithm>
#include <functional>
#include "buffer_variables.hpp"

buffer::buffer() {
}

buffer::~buffer() {
}

void buffer::clear() {
    children.clear();
    values.clear();
}

buffer::ptr buffer::children_create_connected() const {
    buffer::ptr rezult(new buffer);
    rezult->parent = shared_from_this();
    return rezult;
}

buffer::ptr buffer::children_add(std::string const& name) {
    buffer::ptr rezult(new buffer);
    rezult->parent = shared_from_this();
    children.insert(std::make_pair(name, rezult));
    return rezult;
}

bool buffer::children_is_exist(std::string const& name) const {
    return children.end() != children.find(name);
}

buffer::ptr buffer::children_get(std::string const& name) const {
    children_t::const_iterator it = children.find(name);
    if (children.end() == it) {
        // Потомок не найден
        return buffer::ptr();
    }
    else {
        return it->second;
    }
}

buffer::children_vector_t buffer::children_get_all(std::string const& name) const {
    children_vector_t rezult;
    std::pair<children_t::const_iterator, children_t::const_iterator> founded = children.equal_range(name);
    std::transform(founded.first, founded.second, std::back_inserter(rezult), std::tr1::bind(&children_t::value_type::second, std::tr1::placeholders::_1));
    return rezult;
}

void buffer::value_add(std::string const& name, std::string const& value) {
    values.insert(std::make_pair(name, value));
}

void buffer::value_replace(std::string const& name, std::string const& value) {
    values.erase(name);
    values.insert(std::make_pair(name, value));
}

bool buffer::value_is_exist(std::string const& name) const {
    return values.end() != values.find(name);
}

std::string buffer::value_get(std::string const& name) const {
    values_t::const_iterator it = values.find(name);
    if (values.end() == it) {
        assert(false && "not found");
        return std::string();
    }
    return process_all_vars(it->second);
}

buffer::values_vector_t buffer::value_get_all(std::string const& name) const {
    values_vector_t rezult;
    std::pair<values_t::const_iterator, values_t::const_iterator> founded = values.equal_range(name);
    for (values_t::const_iterator it = founded.first; it != founded.second; ++it) {
        rezult.push_back(process_all_vars(it->second));
    }
    // В VC 2010 что-то перестало компилироваться
    //std::transform(founded.first, founded.second, std::back_inserter(rezult), std::tr1::bind(&buffer::process_all_vars, this, std::tr1::bind(&values_t::value_type::second, std::tr1::placeholders::_1)));
    return rezult;
}

std::string buffer::process_all_vars(std::string const& str) const {
    std::string rezult = str;
    buffer_variables::process_vars(shared_from_this(), 16, rezult);
    return rezult;
}
