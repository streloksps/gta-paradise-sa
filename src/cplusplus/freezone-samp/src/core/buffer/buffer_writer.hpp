#ifndef BUFFER_WRITER_HPP
#define BUFFER_WRITER_HPP
#include <string>
#include <ostream>
#include <boost/filesystem/path.hpp>
#include "buffer_fwd.hpp"

class buffer_writer
{
public:
    buffer_writer(buffer_ptr_c_t const& buff);
    ~buffer_writer();
    void save_file(boost::filesystem::path const& file_name, bool process_all_vars = false);
    void save_stream(std::ostream& out, bool process_all_vars = false);
private:
    int indent_size;
    buffer_ptr_c_t buff;
    void process_sections(std::ostream& out, bool process_all_vars, buffer_ptr_c_t const& curr_buff, int indent);
};
#endif // BUFFER_WRITER_HPP
