#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <regex>

template <typename func_t>
inline void iterate_directory_impl(boost::filesystem::path const& work, std::tr1::regex const& regex, bool is_recurse, func_t func) {
    if (exists(work)) {
        for (boost::filesystem::directory_iterator it(work), end; it != end; ++it) {
            if (is_directory(it->status()) && is_recurse) {
                iterate_directory_impl(it->path(), regex, is_recurse, func);
            }
            else if (is_regular_file(it->status())) {
                if (regex_match(it->leaf(), regex)) {
                    func(it->path());
                }
            }
        }
    }
}

// Использовать например:
//   iterate_directory("freemode/etc", ".*\\.conf", tt, true);
template <typename func_t>
inline void iterate_directory(boost::filesystem::path root, std::string const& math_regex, func_t func, bool is_recurse = false) {
    std::tr1::regex regex(math_regex);
    iterate_directory_impl(root, regex, is_recurse, func);
}

#endif // DIRECTORY_HPP
