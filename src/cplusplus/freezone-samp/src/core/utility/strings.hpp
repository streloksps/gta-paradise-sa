#ifndef UTILITY_STRINGS_HPP
#define UTILITY_STRINGS_HPP
#include <string>
#include <boost/format.hpp>

inline void process_starts_underline(std::string& text) {
    for (std::size_t pos = 0, len = text.length(); len > pos; ++pos) {
        if ('_' == text[pos]) {
            text[pos] = ' ';
        }
        else {
            break;
        }
    }
}

inline void process_ends_underline(std::string& text) {
    if (!text.empty()) {
        for (std::size_t pos = text.length() - 1; 0 <= pos; --pos) {
            if ('_' == text[pos]) {
                if (pos > 0 && '\\' == text[pos-1]) {
                    // Наше подчеркивание было экранировано
                    text = text.substr(0, pos - 1) + text.substr(pos);
                    break;
                }
                text[pos] = ' ';
            }
            else {
                break;
            }
        }
    }
}

template <int array_size, typename enum_t>
std::string get_enum_name(enum_t e, std::string const* names) {
    int index = static_cast<int>(e);
    if (0 > index || array_size <= index) {
        return (boost::format("unknown(%1%)") % index).str();
    }
    return names[index];
}

inline std::string str_clip(std::string const str, int max_len) {
    if (static_cast<int>(str.length()) > max_len) {
        std::string endind = "...";
        return str.substr(0, max_len - endind.length()) + endind;
    }
    return str;
}

#endif // UTILITY_STRINGS_HPP
