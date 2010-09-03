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

    //! �������� ��������� � ���
    bool log(std::string const& section, std::string const& text);
    //! ��������� ���� ���� - ����� ���� �������, ��� �������� ����
    void close();
private:
    boost::mutex                file_mutex;

    std::string                 file_name_format;

    // ������� ���� ����
    boost::filesystem::ofstream curr_file;
    boost::filesystem::path     curr_file_path;

    // ��������� ������, ���� ������� ���������������� ����� ��� �����
    bool init_curr_file(boost::posix_time::ptime const& time_utc);
};

#endif // LOG_WRITER_HPP
