#ifndef BUFFER_VARIABLES_HPP
#define BUFFER_VARIABLES_HPP
#include <boost/algorithm/string.hpp>
#include "buffer.hpp"

namespace buffer_variables {
    // Работа с подстановкой переменных
    inline bool find_val_by_name(buffer::ptr_c const& buff, std::string const& name, std::string& founded_val) {
        if (buff->value_is_exist(name)) {
            buffer::values_t::const_iterator it = buff->values.find(name);
            founded_val = it->second;
            return true;
        }
        else {
            buffer::ptr_c parent(buff->parent.lock());
            if (parent) {
                return find_val_by_name(parent, name, founded_val);
            }
            else {
                return false;
            }
        }
    }
    inline bool find_next_var(std::string const& str, std::string::size_type start, std::string::size_type& p1, std::string::size_type& p2, std::string& name) {
        std::string::size_type pos_start = str.find("$(", start);
        if (std::string::npos == pos_start) {
            return false;
        }
        std::string::size_type pos_end = str.find(")", pos_start + 2);
        if (std::string::npos == pos_end) {
            return false;
        }
        name = str.substr(pos_start + 2, pos_end - (pos_start + 2));
        name = boost::trim_copy(name);
        p1 = pos_start;
        p2 = pos_end + 1;
        return !name.empty();
    }
    inline bool process_vars(buffer::ptr_c const& buff, int ttl, std::string& string) {
        if (ttl < 0) {
            return false;
        }
        std::string::size_type pos = 0, p1, p2;
        std::string name, val;
        for (;find_next_var(string, pos, p1, p2, name);) {
            if (find_val_by_name(buff, name, val)) {
                if (process_vars(buff, ttl-1, val)) {
                    string.replace(p1, p2 - p1, val);
                    pos = p1 + val.length();
                    continue;
                }
            }
            pos = p2;
        }
        return true;
    }
} // namespace buffer_variables {
#endif // BUFFER_VARIABLES_HPP
