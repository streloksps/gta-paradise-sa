#ifndef PLAYER_MESSAGES_HPP
#define PLAYER_MESSAGES_HPP
#include "core/module_h.hpp"
#include "core/time_outs.hpp"
#include "chat_msg_item.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <map>

class player_messages
    :public application_item
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<player_messages> ptr;
    static ptr instance();

    player_messages();
    virtual ~player_messages();

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);

public:
    // Методы отсылки простых сообщений в чат. Не требуют восстановления состояния
    static void send_message(player_ptr_t const& player_ptr, std::string const& msg);
    static void send_message(player_ptr_t const& player_ptr, chat_msg_item_t const& msg_item);

    static void set_chat_bubble(player_ptr_t const& player_ptr, std::string const& text);

    // Метод отсылки сообщения от имени игрока
    static void send_player_message(player_ptr_t const& player_ptr, player_ptr_t const& from_player, std::string const& msg, bool is_close_chat = false);
    // Восстанавливает состояние игрока - должно быть вызвано после всех вызовов send_player_message для данного игрока
    static void send_player_message_restore(player_ptr_t const& from_player);
};

class player_messages_bubble_item
    :public player_item
    ,public player_events::on_player_stream_in_i
    ,public player_events::on_disconnect_i
{
    friend class player_messages;
    player_messages_bubble_item();
    virtual ~player_messages_bubble_item();
public:
    typedef std::tr1::shared_ptr<player_messages_bubble_item> ptr;

public: // player_events::*
    virtual void on_player_stream_in(player_ptr_t const& player_ptr);
    virtual void on_disconnect(samp::player_disconnect_reason reason);

private: // Внешний интерфейс
    // Устанвливаем новый чат буббл
    void set_chat_bubble(chat_bubble_t const& chat_bubble);

private:
    boost::optional<time_base::millisecond_t>   last_chat_bubble_expire_time;
    chat_bubble_t                               last_chat_bubble;
    int                                         last_chat_bubble_id;

    time_base::millisecond_t                    max_expire_time;

    typedef std::map<player_ptr_t, int>         showed_ids_t;
    showed_ids_t                                showed_ids;     // Список игроков, чьи бублы видел данный игрок

    void set_chat_bubble_impl(chat_bubble_t const& chat_bubble, time_base::millisecond_t expire_time);

    bool is_chat_bubble_active(int& show_time);

public:
    void update_chat_bubble();
};

#endif // PLAYER_MESSAGES_HPP
