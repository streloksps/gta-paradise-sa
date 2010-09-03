#ifndef SAMP_LOG_HPP
#define SAMP_LOG_HPP
#include <string>

namespace pawn {
    typedef void (*logprintf_t)(char const* format, ...);
    extern logprintf_t logprintf;

    // Безопасная версия принта
    void logprint(std::string const& log_str);
} // namespace pawn {
#endif // SAMP_LOG_HPP
