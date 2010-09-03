#ifndef COMMAND_ADDER_HPP
#define COMMAND_ADDER_HPP
#include "command_type.hpp"

cmd_id_t command_add(std::string const& sys_name, std::string const& section, std::string const& name, std::string const& params, std::string const& short_desc, std::string const& full_desc, command_handler_t command, command_type type = cmd_game, command_access_checker_t const& access_checker = command_access_checker_t(), bool is_hidden = false);
cmd_id_t command_add(std::string const& sys_name, command_handler_t command, command_type type = cmd_game, command_access_checker_t const& access_checker = command_access_checker_t(), bool is_hidden = false); // Использует ресурсы ио имени команды
cmd_id_t command_add_text(std::string const& sys_name, std::string const& section, std::string const& name, std::string const& params, std::string const& short_desc, std::string const& text, command_access_checker_t const& access_checker = command_access_checker_t(), bool is_hidden = false);
cmd_id_t command_add_text(std::string const& sys_name, command_access_checker_t const& access_checker = command_access_checker_t(), bool is_hidden = false);

#endif // COMMAND_ADDER_HPP
