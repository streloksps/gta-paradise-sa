#include "config.hpp"
#include "logger.hpp"
#include "core/application.hpp"
#include "core/samp/pawn/pawn_log.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "server_paths.hpp"

REGISTER_IN_APPLICATION(logger);

logger::ptr logger::instance() {
    return application::instance()->get_item<logger>();
}

logger::logger()
:is_duplicate_logs_in_server_log(false)
,is_duplicate_debugs_in_server_log(false)
,log_item(PATH_VAR_LOG_FILENAME)
,debug_item(PATH_VAR_DEBUG_FILENAME)
{
}

logger::~logger() {
}

void logger::log(std::string const& section, std::string const& text) {
    bool is_log_writed = log_item.log(section, text);
    if (is_duplicate_logs_in_server_log || !is_log_writed) {
        // Ќе удалось инициализировать файл лога или нужно дублировать - пишем в лог сервера 
        pawn::logprint((boost::format("<%1%> %2%") % section % text).str());
    }
}

void logger::debug(std::string const& section, std::string const& text) {
    bool is_log_writed = debug_item.log(section, text);
    if (is_duplicate_debugs_in_server_log || !is_log_writed) {
        // Ќе удалось инициализировать файл лога или нужно дублировать - пишем в лог сервера 
        pawn::logprint((boost::format("debug <%1%> %2%") % section % text).str());
    }
}

void logger::close() {
    log_item.close();
    debug_item.close();
}

void logger::create() {
}

void logger::configure_pre() {
    is_duplicate_logs_in_server_log = false;
    is_duplicate_debugs_in_server_log = false;
}

void logger::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(is_duplicate_logs_in_server_log);
    SERIALIZE_ITEM(is_duplicate_debugs_in_server_log);
}

void logger::configure_post() {
}
