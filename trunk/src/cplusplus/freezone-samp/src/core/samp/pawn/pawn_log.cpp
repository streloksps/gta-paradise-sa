#include "config.hpp"
#include "pawn_log.hpp"
#include <boost/algorithm/string.hpp>

namespace pawn {
    logprintf_t logprintf = 0;
    void logprint(std::string const& log_str) {
        std::string line = boost::replace_all_copy(log_str, "~", " ");
        if (line.length() > 127) {
            line = line.substr(0, 127-3) + "...";
        }
        logprintf("%s", line.c_str());
    }
} // namespace pawn {
