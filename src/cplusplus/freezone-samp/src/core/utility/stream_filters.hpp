#ifndef STREAM_FILTERS_HPP
#define STREAM_FILTERS_HPP
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>
#include "core/utility/strings.hpp"
#include "core/utility/locale_ru.hpp"
#include "core/utility/integer_expression_calc.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename filter_next_t>
struct filter_streamreader_t {
    filter_next_t& filter_next;
    std::istream& file;
    filter_streamreader_t(filter_next_t& filter_next, std::istream& file): filter_next(filter_next), file(file) {
    }
    /* медленный вариант
    void filter_process() {
    if (file) {
    for (;!file.eof();) {
    std::string file_line;
    getline(file, file_line);
    filter_next.filter_addline(file_line);
    }
    }
    }
    */

    void filter_process() {
        if (file) {
            char file_buff[1024*10];
            for (;!file.eof();) {
                file.getline(file_buff, sizeof(file_buff) / sizeof(file_buff[0]));
                filter_next.filter_addline(std::string(file_buff));
            }
        }
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Пишет все, что проходит через него еще в поток, если он задан
template <typename filter_next_t>
struct filter_streamwriter_t {
    filter_next_t& filter_next;
    std::ostream* file_ptr;
    filter_streamwriter_t(filter_next_t& filter_next): filter_next(filter_next), file_ptr(0) {
    }
    ~filter_streamwriter_t() {
        if (file_ptr) {
            (*file_ptr) << std::endl;
        }
    }
    void attach_stream(std::ostream* file_ptr) {
        this->file_ptr = file_ptr;
    }
    void filter_addline(std::string const& line) {
        filter_next.filter_addline(line);
        if (file_ptr) {
            (*file_ptr) << line << std::endl;
        }
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename filter_next_t>
struct filter_dropcomments_t {
    filter_next_t& filter_next;
    filter_dropcomments_t(filter_next_t& filter_next): filter_next(filter_next) {}
    void filter_addline(std::string const& line) {
        std::string::size_type p = line.find("#");
        if (std::string::npos != p) {
            filter_next.filter_addline(line.substr(0, p));
        }
        else {
            filter_next.filter_addline(line);
        }
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename filter_next_t>
struct filter_trim_t {
    filter_next_t& filter_next;
    filter_trim_t(filter_next_t& filter_next): filter_next(filter_next) {}
    /* медленный вариант
    void filter_addline(std::string const& line) {
    std::string buff(line);
    boost::erase_all(buff, "\n");
    boost::erase_all(buff, "\r");
    boost::trim(buff);
    filter_next.filter_addline(buff);
    }
    */
    void filter_addline(std::string const& line) {
        std::string::const_iterator begin = line.begin(), end = line.end();
        for (; begin != end; ++begin) {
            char c = *begin;
            if (' ' != c && '\n' != c && '\r' != c && '\t' != c) {
                break;
            }
        }
        for (; begin != end; --end) {
            char c = *(end-1);
            if (' ' != c && '\n' != c && '\r' != c && '\t' != c) {
                break;
            }
        }

        std::string rezult_line(begin, end);

        // Заменяем начальные подчеркивания на пробелы
        for (std::string::iterator it = rezult_line.begin(), end = rezult_line.end(); it != end; ++it) {
            if ('_' == *it) {
                *it = ' ';
            }
            else {
                break;
            }
        }
        filter_next.filter_addline(rezult_line);
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename filter_next_t>
struct filter_multiline_t {
    filter_next_t& filter_next;
    std::string buff;
    filter_multiline_t(filter_next_t& filter_next): filter_next(filter_next) {}
    void filter_addline(std::string const& line) {
        if (boost::ends_with(line, "\\")) {
            if (boost::ends_with(line, "\\\\")) {
                // Символ слеша проэкронирован - заменяем на 1 символ
                buff += boost::erase_last_copy(line, "\\");
                filter_next.filter_addline(buff);
                buff.clear();
            }
            else {
                buff += boost::erase_last_copy(line, "\\");
            }
        }
        else {
            if (buff.empty()) {
                filter_next.filter_addline(line);
            }
            else {
                buff += line;
                filter_next.filter_addline(buff);
                buff.clear();
            }
        }        
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // STREAM_FILTERS_HPP
