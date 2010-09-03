#include "config.hpp"
#include "timestuff.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

std::string get_var_by_number(std::string const& var_name, int number) {
    int index = 0;
    { // Поиск индекса
        int dec = number % 10;
        if (number >=10 && number <= 19) {
            index = 0;
        }
        else if (1 == dec) {
            index = 1;
        }
        else if (2 == dec || 3 == dec || 4 ==dec) {
            index = 2;
        }
    }
    return (boost::format("$(%1%%2%)") % var_name % index).str();
}

std::string get_time_text(int delta) {
    if (0 == delta) {
        return "$(time_zero)";
    }

    int s =  delta                         % 60;   delta -= s;
    int m = (delta / (60))                 % 60;   delta -= m * 60;
    int h = (delta / (60 * 60))            % 24;   delta -= h * 60 * 60;
    int d = (delta / (60 * 60 * 24))       % 7;    delta -= d * 60 * 60 * 24;
    int w = (delta / (60 * 60 * 24 * 7));

    std::string rezult;
    if (w) {
        rezult += (boost::format("%1% %2% ") % w % get_var_by_number("time_weeks", w)).str();
    }
    if (w || d) {
        rezult += (boost::format("%1% %2% ") % d % get_var_by_number("time_days", d)).str();
    }
    if (w || d || h) {
        rezult += (boost::format("%1% %2% ") % h % get_var_by_number("time_hours", h)).str();
    }
    if (w || d || h || m) {
        rezult += (boost::format("%1% %2% ") % m % get_var_by_number("time_minuts", m)).str();
    }
    if (w || d || h || m || s) {
        rezult += (boost::format("%1% %2% ") % s % get_var_by_number("time_seconds", s)).str();
    }
    return boost::trim_copy(rezult);
}
