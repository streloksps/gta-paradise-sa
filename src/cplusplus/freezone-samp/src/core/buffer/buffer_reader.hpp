#ifndef BUFFER_READER_HPP
#define BUFFER_READER_HPP
#include <string>
#include <istream>
#include <ostream>
#include <stack>
#include <utility>
#include <vector>
#include <boost/filesystem/path.hpp>
#include "buffer_fwd.hpp"

class buffer_reader {
public:
    buffer_reader(buffer_ptr_t const& buff, buffer_ptr_c_t const& preprocess_vars);
    ~buffer_reader();

    void load_file(boost::filesystem::path const& file_name);
    void load_stream(std::istream& file, boost::filesystem::path root_directory);
    void load_stream_only_condition(std::istream& file);

public: // Сервисные функции
    typedef std::vector<std::string> expressions_t;

    // Присоединяем поток, в который будет писаться почти обработанный конфиг (до обработки условий). Объект потока должен быть жив до окончания жизни класса
    void attach_preprocessed_stream(std::ostream& preprocessed_out);
    void attach_expressions_buff(expressions_t& expressions);

    // Для внутреннего использования
    void filter_addline(std::string const& line);

private:
    std::ostream* preprocessed_out_ptr;
    expressions_t* expressions_ptr;

private:
    typedef std::stack<std::pair<buffer_ptr_t, std::string> > process_level_t;

    buffer_ptr_t buff;
    buffer_ptr_c_t preprocess_vars;
    process_level_t process_level;

    static bool get_section(std::string const& line, std::string& name, bool& is_open);
    static bool get_param(std::string const& line, std::string& name, std::string& val);
    void process_level_clear();
};
#endif // BUFFER_READER_HPP
