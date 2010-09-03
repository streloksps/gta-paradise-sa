#ifndef PLAYER_CHAT_HPP
#define PLAYER_CHAT_HPP
#include <memory>
#include <set>
#include <ctime>
#include <map>
#include "core/module_h.hpp"
#include "core/player/player_less.hpp"
#include "core/time_count_blocker.hpp"
#include "core/timers_i.hpp"
#include "core/utility/locale_ru.hpp"
#include "player_chat_regex_t.hpp"
#include "player_chat_automute_meaningless.hpp"

class player_chat_item;
class player_chat
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public players_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<player_chat> ptr;
    static ptr instance();

    player_chat();
    virtual ~player_chat();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_chat);

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);
    virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason);

private:
    command_arguments_rezult cmd_pm(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_ignore(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_ignore_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    command_arguments_rezult cmd_report(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    command_arguments_rezult cmd_admin_chat(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_admin_say(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    command_arguments_rezult cmd_mute(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    static command_arguments_rezult cmd_unmute(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    static command_arguments_rezult cmd_mute_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    command_arguments_rezult cmd_automute_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    command_arguments_rezult cmd_regexblock_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
private:
    friend class player_chat_item;
private:
    int         cmd_mute_def_time_minutes;
    std::string cmd_mute_def_reason;

public:
    unsigned int length_to_lower;
    unsigned int max_repeated_chars;

    time_count_blocker<player_ptr_t> antiflood;
    time_count_blocker<player_ptr_t> antiflood_shared; // Отдельный антифлуд на общий чат

    bool is_process_input_text;

    int mute_time_min; // Минимальное время мута, минуты
    int mute_time_max; // Максимальное время мута, минуты


    bool is_block_on_flood;         // Разрешить блоичить за флуд
    int  block_on_flood_max_time;   // Время штрафа в секундах, за которое блочим

    std::string valid_chat_chars;   // Строки содержит разрешенные в чате символы. Остальные будут удаляться из чата

    // Отправка личного сообщения
    bool do_send_pm_message(player_ptr_t const& player_source_ptr, player_ptr_t const& player_dest_ptr, std::string const& text, messages_sender const& sender, messages_params& params);

/************************************************************************/
/* Автомут по словарю                                                   */
/************************************************************************/
private:
    // Сделать мап, независящим от регистра
    typedef std::map<std::string, int, ilexicographical_less> auto_mute_words_t;
    
    auto_mute_words_t   auto_mute_words;            // Список слов для мута
    int                 auto_mute_exec_frequency;   // Проценты [0-100] - чем больше, тем больше вероятность срабатывания
    int                 auto_mute_delay_min;        // Минимальная задержка, секунды
    int                 auto_mute_delay_max;        // Максимальная задержка, секунды
    std::string         auto_mute_spliters;         // Содержит разделители
public:
    void process_auto_mute_msg(player_ptr_t const& player_ptr, std::string const& msg);

/************************************************************************/
/* Блокировка сообщений регексом                                        */
/************************************************************************/
private:
    typedef std::vector<string_regex_t> regexs_t;

    regexs_t regexs;
public:
    // Возращает истину, если сообщение игрока можно вывести всем игрокам, иначе сообщение выведется только тому, кто его набрал
    // + выводит сообщение в лог
    bool is_can_broadcast_msg(player_ptr_t const& player_ptr, std::string const& msg);

public:
    void process_chat_text(std::string& text) const;
    void process_chat_text_unticaps(std::string& text) const;
    void process_chat_text_delete_repeated_chars(std::string& text) const;

private:
    int group_chat_name_min;
    int group_chat_name_max;
    command_arguments_rezult cmd_group_chat_add(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_group_chat_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_group_chat_list_all(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_group_chat(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

public:
    static players_t group_chat_get_players_by_name(std::string const& group_chat_name);    

private:
    command_arguments_rezult cmd_chat_off(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

/************************************************************************/
/* Ближний чат                                                          */
/************************************************************************/
private:
    float close_chat_radius;
    command_arguments_rezult cmd_close_chat(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

private:
    player_chat_automute_meaningless automute_meaningless;

private: // Команды задания беседы с 1 игроком
    command_arguments_rezult cmd_pm_destination_set(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_pm_destination_chat(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
};

class player_chat_item
    :public player_item
    ,public on_timer1000_i
    ,public on_timer5000_i
    ,public player_events::on_disconnect_i
    ,public player_events::on_spawn_i
    ,public player_events::on_text_i
{
public:
    typedef std::tr1::shared_ptr<player_chat_item> ptr;

protected:
    friend class player_chat;
    player_chat_item(player_chat& manager);
    virtual ~player_chat_item();

public: // player_events::*
    virtual void on_disconnect(samp::player_disconnect_reason reason);
    virtual void on_spawn();
    virtual void on_text(std::string const& text);
    virtual void on_timer1000();
    virtual void on_timer5000();


private:
    typedef std::set<player_ptr_t, player_less> ignore_list_t;
    typedef std::vector<std::string> broadcast_lines_t;
    player_chat& manager;
    bool is_in_game;
    ignore_list_t ignore_list; // Список игроков, от которых данный игрок не хочет получать сообщения

    enum deny_reasons {
        all_ok
        ,deny_not_in_game
        ,deny_is_chat_off
        ,deny_is_ignored
        ,deny_is_spam
        ,deny_is_sleep
    };

    bool is_can_sent_msg(std::string const& type, std::string const& msg);
    bool is_can_receive_msg_from(player_ptr_t const& sender, bool is_broadcast_msg, deny_reasons& reason) const;
    static bool is_can_receive_msg_from_st(player_ptr_t const& destination, player_ptr_t const& sender, bool is_broadcast_msg, deny_reasons& reason);

    enum {broadcast_msg_max_len = 64};
    static broadcast_lines_t make_broadcast_lines(std::string const& msg);

    void send_broadcast_lines(broadcast_lines_t const& lines);  // Отсылает сообщение всем игрокам (разбито по строчно)
    void send_broadcast_text(std::string const& msg);           // Отсылает сообщение всем игрокам (сообщение целиком)
    void send_only_src_lines(broadcast_lines_t const& lines);   // Отсылает сообщение только отправителю

    void send_deny_to_msg(deny_reasons reason, player_ptr_t const& player_dest_ptr);

    void send_close_chat_lines(broadcast_lines_t const& lines);  // Отсылает сообщение заданным игрокам (разбито по строчно)
    void send_close_chat_text(std::string const& msg);           // Отсылает сообщение всем игрокам (сообщение целиком)
public:
    bool get_is_chat_off() const;
    int get_remain_chat_off_time() const;
private: // Работа с мутом
    bool is_chat_off;
    std::time_t chat_off_end_time;
    int chat_off_last_remain;

    void do_chat_off(int minutes);
    void do_chat_on();
    void do_recalc_chat();

    void process_auto_chat_on();
    void show_remain_time();

private:
    bool        automute_is_active;
    std::time_t automute_time;
    int         automute_minutes;
    void        automute_process();

public:
    int         automute_set(int mute_time); // Устанавливает или прибавляет время для автомута, возращает через сколько секунд будет мут

private:
    std::string group_chat_name;
    void        group_chat_on_enter();
    void        group_chat_on_leave(bool is_exit);
    void        group_chat_disconnect();

private:
    bool        is_player_self_chat_off;

public:
    bool        get_is_player_self_chat_off() const;

private: // Личная беседа с 1 игроком
    player_ptr_t pm_destination;
    void pm_destination_disconnect();
};

#endif // PLAYER_CHAT_HPP
