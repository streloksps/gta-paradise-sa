#include "config.hpp"
#include "logger_writer.hpp"
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

namespace {
    template <class char_t, class traits_t>
    inline void write_time_stamp(std::basic_ostream<char_t, traits_t>& stream, boost::posix_time::ptime const& time_utc, boost::posix_time::ptime const& time_local) {
        boost::posix_time::time_duration epoch_diff = time_utc - boost::posix_time::ptime(boost::gregorian::date(1970,1,1));
        boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
        facet->format("%Y%m%dT%H%M%S"); //facet->format("%H:%M:%S");
        stream.imbue(std::locale(stream.getloc(), facet));

        //Печатаем штам времени - милисекунды с начала эпохи + локальное время
        stream << epoch_diff.total_milliseconds() << " " << time_local << " ";
    }
    template <class char_t, class traits_t>
    inline void write_message(std::basic_ostream<char_t, traits_t>& stream, std::string const& section, std::string const& text) {
        stream << "<" << section << "> " << text << std::endl;
    }
    boost::filesystem::path create_path(boost::posix_time::ptime const& time, std::string format) {
        std::ostringstream file_name_generator;
        boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
        facet->format(format.c_str());
        file_name_generator.imbue(std::locale(file_name_generator.getloc(), facet));
        file_name_generator<<time;
        return boost::filesystem::path(file_name_generator.str());
    }
} // namespace {

log_writer::log_writer(std::string const& file_name_format):file_name_format(file_name_format) {
}

log_writer::~log_writer() {
}

bool log_writer::log(std::string const& section, std::string const& text) {
    boost::posix_time::ptime msg_time_utc(boost::posix_time::microsec_clock::universal_time());
    boost::posix_time::ptime msg_time_local = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(msg_time_utc);

    {
        boost::mutex::scoped_lock lock(file_mutex);
        if (init_curr_file(msg_time_local)) {
            //Печатаем временной штамп 
            write_time_stamp(curr_file, msg_time_utc, msg_time_local);
            //Печатаем сообщение
            write_message(curr_file, section, text);
            return true;
        }
    }
    return false;
}

void log_writer::close() {
    if (curr_file.is_open()) {
        curr_file.close();
        curr_file_path = boost::filesystem::path();
    }
}

bool log_writer::init_curr_file(boost::posix_time::ptime const& time) {
    boost::filesystem::path file_path = create_path(time, file_name_format);

    if (!curr_file.is_open() || curr_file_path != file_path) {
        // Открываем файл лога
        close();
        create_directories(file_path.parent_path());
        curr_file.open(file_path, std::ios_base::app);
        if (curr_file.is_open()) {
            curr_file_path = file_path;
        }
    }
    return curr_file.is_open();
}
