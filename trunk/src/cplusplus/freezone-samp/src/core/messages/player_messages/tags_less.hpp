#ifndef TAGS_LESS_HPP
#define TAGS_LESS_HPP
#include <string>
#include <functional>

struct tags_less: public std::binary_function<std::string, std::string, bool> {
    bool operator()(std::string const& left, std::string const& right) const;
};

#endif // TAGS_LESS_HPP
