#include "config.hpp"
#include "buffer_reader.hpp"
#include "buffer_filters.hpp"

buffer_reader::buffer_reader(buffer_ptr_t const& buff, buffer_ptr_c_t const& preprocess_vars)
:buff(buff)
,preprocess_vars(preprocess_vars)
,preprocessed_out_ptr(0)
,expressions_ptr(0)
{
}

buffer_reader::~buffer_reader() {
}

void buffer_reader::load_file(boost::filesystem::path const& file_name) {
    boost::filesystem::ifstream file(file_name, std::ios_base::binary);
    if (file) {
        load_stream(file, file_name.parent_path());
    }
}

void buffer_reader::load_stream(std::istream& file, boost::filesystem::path root_directory) {
    // Задаем цепочки обработки
    typedef buffer_reader filter07_t;

    typedef filter_preprocessor_condition_t <filter07_t>    filter06_t;
    typedef filter_streamwriter_t           <filter06_t>    filter05_t;
    typedef filter_preprocessor_include_t   <filter05_t>    filter04_t;
    typedef filter_multiline_t              <filter04_t>    filter03_t;
    typedef filter_trim_t                   <filter03_t>    filter02_t;
    typedef filter_dropcomments_t           <filter02_t>    filter01_t;
    typedef filter_streamreader_t           <filter01_t>    filter00_t;

    filter06_t filter06(*this, preprocess_vars, expressions_ptr);
    filter05_t filter05(filter06);
    filter04_t filter04(filter05, root_directory);
    filter03_t filter03(filter04);
    filter02_t filter02(filter03);
    filter01_t filter01(filter02);
    filter00_t filter00(filter01, file);

    process_level_clear();
    process_level.push(std::make_pair(buff, ""));

    // Обрабатываем файл
    if (preprocessed_out_ptr) {
        filter05.attach_stream(preprocessed_out_ptr);
    }
    filter00.filter_process();

    assert("" == process_level.top().second && "eof not in root section");
    process_level_clear();
}


void buffer_reader::load_stream_only_condition(std::istream& file) {
    // Задаем цепочки обработки
    typedef buffer_reader filter02_t;

    typedef filter_preprocessor_condition_t <filter02_t>    filter01_t;
    typedef filter_streamreader_t           <filter01_t>    filter00_t;

    filter01_t filter01(*this, preprocess_vars, 0);
    filter00_t filter00(filter01, file);

    process_level_clear();
    process_level.push(std::make_pair(buff, ""));

    filter00.filter_process();

    assert("" == process_level.top().second && "eof not in root section");
    process_level_clear();
}

void buffer_reader::process_level_clear() {
    for (;!process_level.empty();) {
        process_level.pop();
    }
}

void buffer_reader::filter_addline(std::string const& line) {
    if (line.empty()) {
        return;
    }
    std::string name;
    std::string val;
    bool section_is_open;
    if (get_section(line, name, section_is_open)) {
        if (section_is_open) {
            buffer::ptr section = process_level.top().first->children_add(name);
            process_level.push(std::make_pair(section, name));
        }
        else {
            assert(process_level.top().second == name && "open/close sections not coincide");
            if (process_level.top().second == name) {
                // Обработали текущую секцию
                process_level.pop();
            }
        }
    }
    else if (get_param(line, name, val)) {
        process_level.top().first->value_add(name, val);
    }
}

bool buffer_reader::get_section(std::string const& line, std::string& name, bool& is_open) {
    bool is_found = false;
    if (boost::starts_with(line, "<") && boost::ends_with(line, ">")) {
        std::string item = line.substr(1, line.length() - 2);
        boost::trim(item);
        if (boost::starts_with(item, "/")) {
            // Закрывающийся тег
            name = boost::trim_copy(item.substr(1));
            is_open = false;
            is_found = true;
        }
        else {
            // Открывающийся тег
            name = boost::trim_copy(item);
            is_open = true;
            is_found = true;
        }
    }
    return is_found && !name.empty();
}

bool buffer_reader::get_param(std::string const& line, std::string& name, std::string& val) {
    std::string::size_type p = line.find('=');
    if (std::string::npos != p) {
        name = boost::trim_copy(line.substr(0, p));
        val = boost::trim_copy(line.substr(p + 1));
        process_starts_underline(val);
        process_ends_underline(val);
        // Требуем чтобы имя параметра было не пусто - значение может быть любым
        return !name.empty();
    }
    return false;
}

void buffer_reader::attach_preprocessed_stream(std::ostream& preprocessed_out) {
    preprocessed_out_ptr = &preprocessed_out;
}

void buffer_reader::attach_expressions_buff(expressions_t& expressions) {
    expressions_ptr = &expressions;
}
