#ifndef COMMAND_TYPE_HPP
#define COMMAND_TYPE_HPP
#include "core/messages/messages_item.hpp"
#include "core/player/player_fwd.hpp"
#include <functional>

// результат обработки аргументов
enum command_arguments_rezult {
     cmd_arg_syntax_error
    ,cmd_arg_process_error
    ,cmd_arg_ok
    ,cmd_arg_auto
};

enum command_type {
     cmd_info_only      // Информационная команда - только выводит текст игроку
    ,cmd_chat           // Команда чата
    ,cmd_game           // Команда, которая меняет игровые параметры
    ,cmd_game_can_flood // Команда, которая меняет игровые параметры и может флудить
};

// Тип идентификатора команды
typedef int cmd_id_t;

typedef std::tr1::function<command_arguments_rezult (player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params)> command_handler_t;

// Возращает истину, если данный игрок может выполнить данную команду
typedef std::tr1::function<bool (player_ptr_t const& player_ptr, std::string const& cmd_name)> command_access_checker_t;

#endif // COMMAND_TYPE_HPP
