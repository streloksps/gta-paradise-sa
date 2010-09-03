#ifndef PLAYER_NAME_HPP
#define PLAYER_NAME_HPP
#include "core/module_h.hpp"
#include <string>
#include <map>
#include <regex>

class player_name
    :public application_item
    ,public configuradable
    ,public createble_i
    ,public players_events::on_pre_rejectable_connect_i
{
public:
    typedef std::tr1::shared_ptr<player_name> ptr;
    static ptr instance();

public:
    player_name();
    virtual ~player_name();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_name);

public: // players_events::on_connect_i
    virtual bool on_pre_rejectable_connect(player_ptr_t const& player_ptr);

private:
    command_arguments_rezult cmd_find_player(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

private:
    typedef std::map<char, char> replace_table_t;
    typedef std::map<std::string, char> transliteration_t;

    std::string         name_valid_chars;
    int                 name_length_min;
    int                 name_length_max;
    std::tr1::regex     name_check_regex;
    int                 name_check_regex_tag1_index;
    int                 name_check_regex_tag2_index;
    int                 name_check_regex_name_index;
    std::string         name_tag_trim_chars;
    int                 name_min_length;        // ћинимальна€ длина ника, после его парсинга из имени и трима
    int                 tag_min_length;         // ћинимальна€ длина имени клана, после его парсинга из имени и трима
    std::string         name_chat;
    std::string         name_chat_close;
    std::string         name_full;
    std::string         name_need_shield_for_regex_chars;
    transliteration_t   transliteration;
    int                 find_player_count_max;

    // ¬нутренне использование
    replace_table_t     replace_table;
private:
    std::string get_normalized_name(std::string const& name) const;
    bool is_valid_chars(std::string const& name) const;
    std::string get_regex_by_name(std::string const& name) const;
    std::string trim(std::string const& str) const;

private:
    char const asterisk_char;
    char const question_mark;
};
#endif // PLAYER_NAME_HPP
