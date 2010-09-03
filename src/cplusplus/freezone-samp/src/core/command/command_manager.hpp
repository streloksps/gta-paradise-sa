#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP
#include <memory>
#include <string>
#include <vector>
#include "core/module_h.hpp"
#include "core/time_count_blocker.hpp"

struct command_t {
    std::string                 sys_name;
    std::string                 section;
    std::string                 name;
    std::string                 params;
    std::string                 short_desc;
    std::string                 full_desc;
    command_handler_t           command;
    command_type                type;
    command_access_checker_t    access_checker;
    bool                        is_hidden;
public:
    command_t(std::string const& sys_name, std::string const& section, std::string const& name, std::string const& params, std::string const& short_desc, std::string const& full_desc, command_handler_t command, command_type type, command_access_checker_t const& access_checker, bool is_hidden);
};
typedef std::vector<command_t> commands_t;


class command_manager_item;
class command_manager
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public players_events::on_disconnect_i
{
    friend class command_manager_item;
public:
    typedef std::tr1::shared_ptr<command_manager> ptr;
    static ptr instance();

    command_manager();
    virtual ~command_manager();

public: // Публичный интерфейс
    // Добавляет команду
    cmd_id_t add_command(command_t const& command);
    // Возращает истину, если данная строка может быть командой
    bool it_is_command(std::string const& str);
    // Команда печатает заданный текст
    command_arguments_rezult cmd_print_text(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params, std::string const& text);

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(command_manager);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);
    virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason);

private:
    // Печатает список доступных команд
    command_arguments_rezult cmd_commands(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

private:
    typedef std::vector<std::string> admin_cmds_t;
    admin_cmds_t admin_cmds;

    commands_t cmds;
    buffer_ptr_c_t msg_buff;
    bool is_player_have_access(player_ptr_t const& player_ptr, command_t const& cmd);
    void process_cmd(player_ptr_t const& player_ptr, std::string const& text);
    bool is_command_equal(player_ptr_t const& player_ptr, std::string const& text, command_t const& cmd);
    void parse_cmd_string(std::string const& cmd_string, std::string& cmd_name, std::string& cmd_arguments);

    std::string cmd_get_section(command_t const& cmd);
    std::string cmd_get_name(command_t const& cmd);
    std::string cmd_get_short_desc(command_t const& cmd);

    std::string valid_command_string_chars;
    bool        is_nead_check_for_valid_command_string_chars;
    std::string get_save_command_string(std::string const& src_cmd_string) const;

private:
    time_count_blocker<player_ptr_t>    anti_flood;
    std::vector<std::string>            short_cmds;
};

class command_manager_item
    :public player_item
    ,public player_events::on_commandtext_i
    ,public player_events::on_text_i
    ,public player_events::on_private_msg_i
{
    friend class command_manager;
    command_manager_item(command_manager& manager);
    virtual ~command_manager_item();
public:
    typedef std::tr1::shared_ptr<command_manager_item> ptr;

public: // player_events::*
    virtual void on_commandtext(std::string const& cmd);
    virtual void on_text(std::string const& text);
    virtual void on_private_msg(player_ptr_t const& reciever_ptr, std::string const& text);

private:
    command_manager& manager;
};
#endif // COMMAND_MANAGER_HPP
