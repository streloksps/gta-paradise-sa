#include "config.hpp"
#include "player_name.hpp"
#include "core/module_c.hpp"
#include "core/serialization/serialization_regex_t.hpp"
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/foreach.hpp>

REGISTER_IN_APPLICATION(player_name);

player_name::ptr player_name::instance() {
    return application::instance()->get_item<player_name>();
}

player_name::player_name()
:asterisk_char('*')
,question_mark('?')
{
}

player_name::~player_name() {
}

void player_name::create() {
    command_add("find_player", std::tr1::bind(&player_name::cmd_find_player, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
}

void player_name::configure_pre() {
    name_valid_chars = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890[]_";
    name_length_min = 3;
    name_length_max = 19;
    name_check_regex.assign("^([A-Za-z0-9_@\\[\\]]+)$");
    name_check_regex_tag1_index = -1;
    name_check_regex_tag2_index = -1;
    name_check_regex_name_index = 0;
    name_tag_trim_chars = "";
    name_min_length = 3;
    tag_min_length = 1;
    name_chat = "$(name)[$(player_id)]";
    name_chat_close = "$(name)[$(player_id)]";
    name_full = "$(name)~[$(player_id)]";
    name_need_shield_for_regex_chars = "[]";
    replace_table.clear();
    transliteration.clear();
    find_player_count_max = 10;
}

void player_name::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(name_valid_chars);
    SERIALIZE_ITEM(name_length_min);
    SERIALIZE_ITEM(name_length_max);
    SERIALIZE_ITEM(name_check_regex);
    SERIALIZE_ITEM(name_check_regex_tag1_index);
    SERIALIZE_ITEM(name_check_regex_tag2_index);
    SERIALIZE_ITEM(name_check_regex_name_index);
    SERIALIZE_ITEM(name_tag_trim_chars);
    SERIALIZE_ITEM(name_min_length);
    SERIALIZE_ITEM(tag_min_length);
    SERIALIZE_ITEM(name_chat);
    SERIALIZE_ITEM(name_chat_close);
    SERIALIZE_ITEM(name_full);
    SERIALIZE_ITEM(replace_table);
    SERIALIZE_ITEM(transliteration);
    SERIALIZE_ITEM(name_need_shield_for_regex_chars);
    SERIALIZE_ITEM(find_player_count_max);
}

void player_name::configure_post() {
    // Заполняем replace_table из transliteration
    for (transliteration_t::const_iterator it = transliteration.begin(), end = transliteration.end(); it != end; ++it) {
        for (std::string::const_iterator str_it = it->first.begin(), str_end = it->first.end(); str_it != str_end; ++str_it) {
            replace_table.insert(std::make_pair(*str_it, it->second));
        }
    }
}

// Истина - разрешить подключение
// Если хотябы 1 метод вырнул ложь - игрок кикается, никакие другие события не вызываются
bool player_name::on_pre_rejectable_connect(player_ptr_t const& player_ptr) {
    samp::api::ptr api_ptr = samp::api::instance();
    int player_id = player_ptr->get_id();
    std::string const connect_name = api_ptr->get_player_name(player_id);

    messages_item msg_item;
    msg_item.get_params()
        ("player_id", player_id)
        ("connect_name", connect_name)
        ("connect_name_length", connect_name.length())
        ("player_geo_long_desc", player_ptr->geo_get_long_desc())
        ("name_length_min", name_length_min)
        ("name_length_max", name_length_max)
        ("name_valid_chars", name_valid_chars)
        ;

    if (connect_name.length() < static_cast<std::size_t>(name_length_min) || connect_name.length() > static_cast<std::size_t>(name_length_max)) {
        msg_item.get_sender()
            ("$(player_name_bad_name_length)", msg_player(player_ptr))
            ("$(player_name_bad_name_length_log)", msg_log);
        return false;
    }

    if (!is_valid_chars(connect_name)) {
        msg_item.get_sender()
            ("$(player_name_bad_name_validchars)", msg_player(player_ptr))
            ("$(player_name_bad_name_validchars_log)", msg_log);
        return false;
    }

    std::tr1::smatch name_matched;
    if (!regex_search(connect_name, name_matched, name_check_regex)) {
        msg_item.get_sender()
            ("$(player_name_bad_name_regex)", msg_player(player_ptr))
            ("$(player_name_bad_name_regex_log)", msg_log);
        return false;
    }

    std::string name = connect_name;
    
    std::string                 final_name = trim(name_matched[name_check_regex_name_index]);
    std::vector<std::string>    final_tags;
    if (-1 != name_check_regex_tag1_index && name_matched[name_check_regex_tag1_index].matched) {
        final_tags.push_back(trim(name_matched[name_check_regex_tag1_index]));
    }
    if (-1 != name_check_regex_tag2_index && name_matched[name_check_regex_tag2_index].matched) {
        final_tags.push_back(trim(name_matched[name_check_regex_tag2_index]));
    }

    msg_item.get_params()
        ("final_name", final_name)
        ("final_tags0", "#none")
        ("final_tags1", "#none")
        ;
    for (std::size_t i = 0, size = final_tags.size(); i < size; ++i) {
        msg_item.get_params()((boost::format("final_tags%1%") % i).str(), final_tags[i]);
    }


    // Проверяем распарсиненные ник и таги
    if (final_name.length() < static_cast<std::size_t>(name_min_length)) {
        msg_item.get_params()("name_min_length", name_min_length);
        msg_item.get_sender()
            ("$(player_name_bad_finalname)", msg_player(player_ptr))
            ("$(player_name_bad_finalname_log)", msg_log);
        return false;
    }
    for (std::size_t i = 0, size = final_tags.size(); i < size; ++i) {
        if (final_tags[i].length() < static_cast<std::size_t>(tag_min_length)) {
            msg_item.get_params()
                ("tag_min_length", tag_min_length)
                ("final_tag", final_tags[i]);
            msg_item.get_sender()
                ("$(player_name_bad_finaltag)", msg_player(player_ptr))
                ("$(player_name_bad_finaltag_log)", msg_log);
            return false;
        }
    }

    // Все проверки пройдены - устанавливаем имя
    msg_item.get_params()
        ("name", name)
        ("player_id", player_id);

    player_ptr->name = name;
    player_ptr->name_normalized = get_normalized_name(name);
    player_ptr->name_for_chat = msg_item.get_params().process_all_vars(name_chat);
    player_ptr->name_for_chat_close = msg_item.get_params().process_all_vars(name_chat_close);
    player_ptr->name_full = msg_item.get_params().process_all_vars(name_full);
    player_ptr->name_set(name);

    return true;
}

std::string player_name::get_normalized_name(std::string const& name) const {
    std::string rezult(name);
    for (std::string::iterator it = rezult.begin(), end = rezult.end(); end != it; ++it) {
        replace_table_t::const_iterator replace_it = replace_table.find(*it);
        if (replace_table.end() != replace_it) {
            // Нашли текущий символ в таблице замен
            *it = replace_it->second;
        }
    }
    return rezult;
}

command_arguments_rezult player_name::cmd_find_player(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    params
        ("pattern", arguments)
        ("valid_chars", name_valid_chars)
        ("asterisk_char", std::string(1, asterisk_char))
        ("question_mark", std::string(1, question_mark))
        ;

    { // Проверка на валидные симвлы
        std::string name = arguments;
        boost::find_format_all(name, boost::token_finder(boost::lambda::bind(boost::is_any_of(std::string(1, asterisk_char) + std::string(1, question_mark)), boost::lambda::_1)), boost::empty_formatter("")); // Оставляем только разрешенные символы
        if (!is_valid_chars(name)) {
            pager("$(find_player_error_invalid_chars)");
            return cmd_arg_process_error;
        }
    }
    
    players_t   finded_players;
    int         finded_players_count = 0;
    try { // ищем список игроков
        players_t players_list = players::instance()->get_players();
        std::tr1::regex regex(get_regex_by_name(arguments));
        std::tr1::smatch match;
        BOOST_FOREACH(player_ptr_t const& player_ptr, players_list) {
            if (std::tr1::regex_search(player_ptr->name_normalized_get(), match, regex)) {
                ++finded_players_count;
                if (static_cast<int>(finded_players.size()) < find_player_count_max) {
                    finded_players.insert(finded_players.end(), player_ptr);
                }
            }
        }
    }
    catch (std::exception const& err) { 
        assert(false);
        params("error", err.what());
        sender("<log_section find_player/exception/>$(error)", msg_log);
    }

    if (finded_players.empty()) {
        pager("$(find_player_not_found)");
        sender("$(find_player_not_found_log)", msg_log);
        return cmd_arg_ok;
    }

    params
        ("players", players::get_players_names(finded_players))
        ("players_count", finded_players_count)
        ("players_count_show", finded_players.size())
        ;
    if (finded_players.size() == finded_players_count) {
        pager("$(find_player_ok)");
    }
    else {
        pager("$(find_player_ok_part)");
    }
    sender("$(find_player_ok_log)", msg_log);
    return cmd_arg_ok;
}

bool player_name::is_valid_chars(std::string const& name) const {
    for (std::string::const_iterator it = name.begin(), end = name.end(); it != end; ++it) {
        if (std::string::npos == name_valid_chars.find(*it)) {
            return false;
        }
    }
    return true;
}

std::string player_name::get_regex_by_name(std::string const& name) const {
    std::string source = get_normalized_name(name);
    std::string rezult;

    for (std::string::const_iterator it = source.begin(), end = source.end(); end != it; ++it) {
        if (std::string::npos != name_need_shield_for_regex_chars.find(*it)) {
            rezult += std::string("\\") + std::string(1, *it);
        }
        else if (asterisk_char == *it) {
            rezult += ".*";
        }
        else if (question_mark == *it) {
            rezult += ".";
        }
        else {
            rezult += *it;
        }
    }

    return rezult;
}

std::string player_name::trim(std::string const& str) const {
    return boost::trim_copy_if(str, boost::is_any_of(name_tag_trim_chars));
}
