#ifndef LOG_WRITER_HPP
#define LOG_WRITER_HPP
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>

class log_writer {
public:
    log_writer(std::string const& file_name_format);
    ~log_writer();

    //! Печатает сообщение в лог
    bool log(std::string const& section, std::string const& text);
    //! Закрывает файл лога - может быть полезно, при удалении лога
    void close();
private:
    boost::mutex                file_mutex;

    std::string                 file_name_format;

    // Текущий файл лога
    boost::filesystem::ofstream curr_file;
    boost::filesystem::path     curr_file_path;

    // Возращает истину, если удалось инициализировать поток лог файла
    bool init_curr_file(boost::posix_time::ptime const& time_utc);
};

#endif // LOG_WRITER_HPP
