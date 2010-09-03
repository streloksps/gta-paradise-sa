#include "config.hpp"
#include "buffer_writer.hpp"
#include "buffer.hpp"
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>

buffer_writer::buffer_writer(buffer_ptr_c_t const& buff)
:buff(buff)
,indent_size(4)
{
}

buffer_writer::~buffer_writer() {
}

void buffer_writer::save_file(boost::filesystem::path const& file_name, bool process_all_vars) {
    try {
        boost::filesystem::ofstream file(file_name);
        if (file) {
            save_stream(file, process_all_vars);
        }
    }
    catch (std::exception const& /*err*/) {
        assert(false);
    }
}

void buffer_writer::save_stream(std::ostream& out, bool process_all_vars) {
    process_sections(out, process_all_vars, buff, 0);
}

void buffer_writer::process_sections(std::ostream& out, bool process_all_vars, buffer_ptr_c_t const& curr_buff, int indent) {
    std::string indent_str(indent * indent_size, ' ');

    BOOST_FOREACH(buffer::values_t::value_type const& values, curr_buff->values) {
        out << indent_str << values.first << '=';
        if (process_all_vars) {
            out << curr_buff->process_all_vars(values.second);
        }
        else {
            out << values.second;
        }
        out << std::endl;
    }

    BOOST_FOREACH(buffer::children_t::value_type const& children, curr_buff->children) {
        out << indent_str << '<' << children.first << '>' << std::endl;
        process_sections(out, process_all_vars, children.second, indent + 1);
        out << indent_str << "</" << children.first << '>' << std::endl;
    }
}
