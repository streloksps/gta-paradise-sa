#ifndef TIMESTUFF_HPP
#define TIMESTUFF_HPP
#include <string>

std::string get_var_by_number(std::string const& var_name, int number);

// Возращает строку с текстовым представлением промежутка времени в секундах
std::string get_time_text(int delta);

#endif // TIMESTUFF_HPP
