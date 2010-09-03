#include "config.hpp"
#include "command_adder.hpp"
#include "command_manager.hpp"
#include <functional>

cmd_id_t command_add(std::string const& sys_name, std::string const& section, std::string const& name, std::string const& params, std::string const& short_desc, std::string const& full_desc, command_handler_t command, command_type type /*= cmd_game*/, command_access_checker_t const& access_checker, bool is_hidden) {
    return command_manager::instance()->add_command(command_t(sys_name, section, name, params, short_desc, full_desc, command, type, access_checker, is_hidden));
}

cmd_id_t command_add(std::string const& sys_name, command_handler_t command, command_type type, command_access_checker_t const& access_checker, bool is_hidden) {
    return command_manager::instance()->add_command(command_t(sys_name, "$(cmd_" + sys_name + "_section)", "$(cmd_" + sys_name + "_name)", "$(cmd_" + sys_name + "_params)", "$(cmd_" + sys_name + "_short)", "$(cmd_" + sys_name + "_full)", command, type, access_checker, is_hidden));
}

cmd_id_t command_add_text(std::string const& sys_name, std::string const& section, std::string const& name, std::string const& params, std::string const& short_desc, std::string const& text, command_access_checker_t const& access_checker, bool is_hidden) {
    return command_add(sys_name, section, name, params, short_desc, "", bind(&command_manager::cmd_print_text, command_manager::instance().get(), std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5, text), cmd_info_only, access_checker, is_hidden);
}

cmd_id_t command_add_text(std::string const& sys_name, command_access_checker_t const& access_checker, bool is_hidden) {
    return command_add(sys_name, "$(cmd_" + sys_name + "_section)", "$(cmd_" + sys_name + "_name)", "$(cmd_" + sys_name + "_params)", "$(cmd_" + sys_name + "_short)", "", bind(&command_manager::cmd_print_text, command_manager::instance().get(), std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5, "$(cmd_" + sys_name + "_text)"), cmd_info_only, access_checker, is_hidden);
}
