#include "config.hpp"
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/foreach.hpp>
#include "command_manager.hpp"
#include "core/messages/messages_storage.hpp"
#include "core/utility/locale_ru.hpp"
#include "core/messages/player_messages/tags.hpp"
#include "core/messages/player_messages/tags_less.hpp"
#include "core/module_c.hpp"

command_t::command_t(std::string const& sys_name, std::string const& section, std::string const& name, std::string const& params, std::string const& short_desc, std::string const& full_desc, command_handler_t command, command_type type, command_access_checker_t const& access_checker, bool is_hidden)
:sys_name(sys_name)
,section(section)
,name(name)
,params(params)
,short_desc(short_desc)
,full_desc(full_desc)
,command(command)
,type(type)
,access_checker(access_checker)
,is_hidden(is_hidden)
{
}


REGISTER_IN_APPLICATION(command_manager);

command_manager::ptr command_manager::instance() {
    return application::instance()->get_item<command_manager>();
}

command_manager::command_manager() {
}

command_manager::~command_manager() {
}

void command_manager::create() {
    msg_buff = messages_storage::instance()->get_msg_buff();
    add_command(command_t("commands", "$(cmd_commands_section)", "$(cmd_commands_name)", "$(cmd_commands_params)", "$(cmd_commands_short)", "$(cmd_commands_full)", bind(&command_manager::cmd_commands, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only, command_access_checker_t(), false));
}

void command_manager::on_connect(player_ptr_t const& player_ptr) {
    command_manager_item::ptr item(new command_manager_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void command_manager::configure_pre() {
    admin_cmds.clear();
    short_cmds.clear();
    anti_flood.get_params().set(15000, 1, 8, 1500, 30000);
    valid_command_string_chars = "јаЅб¬в√гƒд≈е®Є∆ж«з»и…й кЋлћмЌнќоѕп–р—с“т”у‘ф’х÷ц„чЎшўщЏъџы№ьЁэёюя€AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890/.";
    is_nead_check_for_valid_command_string_chars = true;
}

void command_manager::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(admin_cmds);
    SERIALIZE_ITEM(short_cmds);
    SERIALIZE_ITEM(valid_command_string_chars);
    SERIALIZE_ITEM(is_nead_check_for_valid_command_string_chars);
    SERIALIZE_NAMED_ITEM(anti_flood.get_params(), "anti_flood_params");
}

void command_manager::configure_post() {
}

cmd_id_t command_manager::add_command(command_t const& command) {
    cmd_id_t rezult = cmds.size();
    cmds.push_back(command);
    return rezult;
}

void command_manager::process_cmd(player_ptr_t const& player_ptr, std::string const& text_raw) {
    std::string text = get_save_command_string(text_raw);
    if (!it_is_command(text)) {
        return;
    }

    if (player_ptr->block_get()) {
        return;
    }

    std::string cmd_name, cmd_arguments;
    parse_cmd_string(text, cmd_name, cmd_arguments);

    messages_item_paged msg_item_paged(player_ptr);
    msg_item_paged.get_params()
        ("cmd_full_text", text)
        ("cmd_raw_name", cmd_name)
        ("cmd_arguments", cmd_arguments)
        ("cmd_player_name", player_ptr->name_get_full());

    // ѕеред началом обработки команды отправл€ем в лог сообщение - чтобы диагностировать ошибку обработки
    msg_item_paged.get_sender()("$(cmd_begin_process_log)", msg_log);

    if (player_in_game != player_ptr->get_state() && player_not_spawn != player_ptr->get_state()) {
        // »грок еще не вошел в игру - команды выполн€ть нельз€
        msg_item_paged.get_pager()("$(cmd_not_login_to_game)");
        msg_item_paged.get_sender()("$(cmd_not_login_to_game_log)", msg_log);
        return;
    }

    commands_t::iterator it = std::find_if(cmds.begin(), cmds.end(), std::tr1::bind(&command_manager::is_command_equal, this, std::tr1::ref(player_ptr), std::tr1::cref(cmd_name), std::tr1::placeholders::_1));

    if (cmds.end() == it) {
        //  оманда не найдена
        msg_item_paged.get_pager()("$(cmd_not_found)");
        msg_item_paged.get_sender()("$(cmd_not_found_log)", msg_log);
        return;
    }

    msg_item_paged.get_params()
        ("cmd_sys_name", it->sys_name)
        ("cmd_section", cmd_get_section(*it))
        ("cmd_name", cmd_get_name(*it))
        ("cmd_params", it->params)
        ("cmd_short_desc", cmd_get_short_desc(*it))
        ("cmd_args", "");

    if (cmd_game == it->type || cmd_game_can_flood == it->type) {
        if (player_in_game != player_ptr->get_state()) {
            // »грок не в игре, а команда мен€ет параметры игры - нельз€ выполн€ть
            msg_item_paged.get_pager()("$(cmd_not_in_game)");
            msg_item_paged.get_sender()("$(cmd_not_in_game_log)", msg_log);
            return;
        }
        if (player_ptr->animation_is_use_phone()) {
            // »грок говорит по телефону
            msg_item_paged.get_pager()("$(cmd_use_phone)");
            msg_item_paged.get_sender()("$(cmd_use_phone_log)", msg_log);
            return;
        }
        if (player_ptr->get_item<player_position_item>()->is_preload()) {
            // ¬ыполн€етс€ програзка объектов
            msg_item_paged.get_pager()("$(cmd_loading_active)");
            msg_item_paged.get_sender()("$(cmd_loading_active_log)", msg_log);
            return;
        }
    }

    if (cmd_game_can_flood == it->type) {
        if (anti_flood.is_blocked(player_ptr)) {
            // »грок не может слишком часто использовать команды, которые пишут что то на экране других игроков
            anti_flood.increase(player_ptr);
            msg_item_paged.get_params()("time_out", (int)(anti_flood.get_time_out(player_ptr) / 1000));
            msg_item_paged.get_pager()("$(cmd_antiflood_block)");
            msg_item_paged.get_sender()("$(cmd_antiflood_block_log)", msg_log);
            return;
        }
    }

    command_arguments_rezult rezult = it->command(player_ptr, cmd_arguments, msg_item_paged.get_pager(), msg_item_paged.get_sender(), msg_item_paged.get_params());
    if (cmd_arg_auto == rezult) {
        // јвтоматическое определение результата - работа с пейджером
        if (msg_item_paged.get_pager().set_page_number(cmd_arguments)) {
            if (!msg_item_paged.get_pager()) {
                // ѕейджер отработал с ошибкой
                rezult = cmd_arg_process_error;
            }
        }
        else {
            rezult = cmd_arg_syntax_error;
        }
    }

    if (cmd_arg_auto == rezult) {
        // ѕейджер отработал коректно
        msg_item_paged.get_params()("cmd_pager_page", msg_item_paged.get_pager().get_page());
        msg_item_paged.get_sender()("$(cmd_auto_log)", msg_log);
    }
    else if (cmd_arg_ok == rezult) {
        //  оманда успешно выполнилась
        msg_item_paged.get_sender()("$(cmd_ok_log)", msg_log);
        if (cmd_game_can_flood == it->type) {
            anti_flood.increase(player_ptr);
        }
    }
    else if (cmd_arg_syntax_error == rezult) {
        // ќшибка синтаксиса вызова команды
        msg_item_paged.get_sender()("$(cmd_syntax_error)", msg_player(player_ptr));
        msg_item_paged.get_sender()("$(cmd_syntax_error_log)", msg_log);
    }
    else if (cmd_arg_process_error == rezult) {
        // ќшибка в работе команды
        msg_item_paged.get_sender()("$(cmd_process_error_log)", msg_log);
    }
}

bool command_manager::is_player_have_access(player_ptr_t const& player_ptr, command_t const& cmd) {
    bool rezult = true;

    // ѕровер€ем команды администрации
    admin_cmds_t::iterator it = std::find(admin_cmds.begin(), admin_cmds.end(), cmd.name);
    if (admin_cmds.end() != it && !player_ptr->group_is_in("admin")) {
        //  оманда только дл€ администраторов а у нас простой игрок
        rezult = false;
    }

    // ѕровер€ем пользовательским функциональным объектом
    command_access_checker_t access_checker = cmd.access_checker;
    if (access_checker && rezult) {
        // ≈сть метод проверки прав
        if (!access_checker(player_ptr, cmd.name)) {
            // »грок не может выполн€ть команду
            rezult = false;
        }
    }

    return rezult;
}

bool command_manager::it_is_command(std::string const& str) {
    if (boost::starts_with(str, "/") || boost::starts_with(str, ".")) {
        return true;
    }
    BOOST_FOREACH(std::string const& short_cmd, short_cmds) {
        if (boost::istarts_with(str, short_cmd, locale_ru)) {
            return true;
        }
    }
    return false;
}

void command_manager::parse_cmd_string(std::string const& cmd_string, std::string& cmd_name, std::string& cmd_arguments) {
    // »щем короткую команду
    BOOST_FOREACH(std::string const& short_cmd, short_cmds) {
        if (short_cmd.empty()) {
            assert(false);
            continue;
        }
        if (boost::istarts_with(cmd_string, short_cmd, locale_ru)) {
            cmd_name = cmd_string.substr(0, short_cmd.length());
            cmd_arguments = boost::trim_copy(cmd_string.substr(short_cmd.length()));
            return;
        }
    }

    std::string::size_type p = cmd_string.find(" ");
    if (std::string::npos != p) {
        cmd_name = boost::trim_copy(cmd_string.substr(0, p));
        cmd_arguments = boost::trim_copy(cmd_string.substr(p + 1));
    }
    else {
        cmd_name = boost::trim_copy(cmd_string);
        cmd_arguments = std::string();
    }
}

bool command_manager::is_command_equal(player_ptr_t const& player_ptr, std::string const& text, command_t const& cmd) {
    if (is_player_have_access(player_ptr, cmd)) {
        tags tag(msg_buff->process_all_vars(cmd.name));
        std::string real_cmd_name = tag.get_untaget_str();
        if (boost::starts_with(real_cmd_name, "/")) {
            return boost::iequals(real_cmd_name, text, locale_ru)
                || boost::iequals(boost::replace_first_copy(real_cmd_name, "/", "."), text, locale_ru)
                ;
        }
        else {
            // ѕровер€ем на короткое им€
            return boost::iequals(real_cmd_name, text, locale_ru);
        }
    }
    return false;
}

command_arguments_rezult command_manager::cmd_commands(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    typedef command_t const* cmd_ptr_t;
    typedef std::map<std::string, cmd_ptr_t, tags_less> name_cmd_ptr_t;
    typedef std::map<std::string, name_cmd_ptr_t, tags_less> section_name_cmd_ptr_t;

    std::list<cmd_ptr_t> cmd_ptrs;
    section_name_cmd_ptr_t section_name_cmd;

    std::transform(cmds.begin(), cmds.end(), std::back_inserter(cmd_ptrs), &boost::lambda::_1);
    // ѕровер€ем права доступа
    cmd_ptrs.remove_if(!boost::lambda::bind(&command_manager::is_player_have_access, this, boost::ref(player_ptr), *boost::lambda::_1));
    // ”бираем из списка скрытые команды
    cmd_ptrs.remove_if(std::tr1::bind(&command_t::is_hidden, std::tr1::placeholders::_1));

    // —ортируем по секци€м и именам
    BOOST_FOREACH(command_t const* cmd_p, cmd_ptrs) {
        section_name_cmd[msg_buff->process_all_vars(cmd_p->section)][msg_buff->process_all_vars(cmd_p->name)] = cmd_p;
    }

    pager.set_header_text(0, "$(cmd_commands_header1)");
    BOOST_FOREACH(section_name_cmd_ptr_t::value_type& name_cmd_ptr, section_name_cmd) {
        BOOST_FOREACH(name_cmd_ptr_t::value_type& cmd_ptr, name_cmd_ptr.second) {
            params
                ("cmd_item_section", cmd_get_section(*cmd_ptr.second))
                ("cmd_item_name", cmd_get_name(*cmd_ptr.second))
                ("cmd_item_short_desc", cmd_get_short_desc(*cmd_ptr.second))
                ;
            pager.set_header_text(1, "$(cmd_commands_header2)");
            pager("$(cmd_commands_item)");
        }
    }
    return cmd_arg_auto;
}

command_arguments_rezult command_manager::cmd_print_text(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params, std::string const& text) {
    pager(text);
    return cmd_arg_auto;
}

std::string command_manager::cmd_get_section(command_t const& cmd) {
    tags tag(msg_buff->process_all_vars(cmd.section));
    return tag.get_untaget_str();
}

std::string command_manager::cmd_get_name(command_t const& cmd) {
    tags tag(msg_buff->process_all_vars(cmd.name));
    return tag.get_untaget_str();
}

std::string command_manager::cmd_get_short_desc(command_t const& cmd) {
    tags tag_params(msg_buff->process_all_vars(cmd.params));
    tags tag_short_desc(msg_buff->process_all_vars(cmd.short_desc));
    messages_item msg_item;
    msg_item.get_params()
        ("params", tag_params.get_untaget_str())
        ("short_desc", tag_short_desc.get_untaget_str());

    if (tag_params.get_untaget_str().empty()) {
        return msg_item.get_params().process_all_vars("$(cmd_short_desc)");
    }
    return msg_item.get_params().process_all_vars("$(cmd_short_desc_with_params)");
}

void command_manager::on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) {
    anti_flood.erase(player_ptr);
}

std::string command_manager::get_save_command_string(std::string const& src_cmd_string) const {
    if (is_nead_check_for_valid_command_string_chars) {
        return boost::find_format_all_copy(src_cmd_string, boost::token_finder(boost::lambda::bind(std::logical_not<bool>(),boost::lambda::bind(boost::is_any_of(valid_command_string_chars), boost::lambda::_1))), boost::empty_formatter("")); // ќставл€ем только разрешенные символы
    }
    return src_cmd_string;
}
command_manager_item::command_manager_item(command_manager& manager): manager(manager) {

}

command_manager_item::~command_manager_item() {

}

void command_manager_item::on_commandtext(std::string const& cmd) {
    manager.process_cmd(get_root(), cmd);
}

void command_manager_item::on_text(std::string const& text) {
    manager.process_cmd(get_root(), text);
}

void command_manager_item::on_private_msg(player_ptr_t const& reciever_ptr, std::string const& text) {
    messages_item msg_item;
    msg_item.get_sender()("$(cmd_not_found)", msg_player(get_root()));
}
