#ifndef PLAYER_GAMEPAUSE_HPP
#define PLAYER_GAMEPAUSE_HPP
#include "core/module_h.hpp"
//#include <boost/date_time/posix_time/posix_time.hpp>
#include "core/time_outs.hpp"
#include <boost/optional.hpp>
#include <map>
#include <string>
#include <vector>

class player_gamepause_item;
class player_gamepause
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public players_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<player_gamepause> ptr;
    static ptr instance();

public:
    player_gamepause();
    virtual ~player_gamepause();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();

public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_gamepause);

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);
    virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason);

private:
    friend class player_gamepause_item;
    typedef std::map<int, std::string> prefixes_t;
    typedef std::vector<std::string> indicators_t;

    bool            is_active;
    int             min_detect_time; // Минимальное время неактивность на которое срабатывает обнаружение паузы, милисекунды
    int             min_onleave_msg_time; // Минимальное время, которое игрок должен провести в паузе, чтобы было сообщение о выходе из паузы, милисекунды
    prefixes_t      prefixes;
    std::string     progress_item;
    int             progress_items_cout;
    indicators_t    indicators;

private:
    command_arguments_rezult cmd_player_sleep_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
};


class player_gamepause_item
    :public player_item
    ,public on_timer500_i
    ,public player_events::on_spawn_i
    ,public player_events::on_update_i
{
public:
    typedef std::tr1::shared_ptr<player_gamepause_item> ptr;

protected:
    friend class player_gamepause;
    player_gamepause_item(player_gamepause& manager);
    virtual ~player_gamepause_item();

private:
    player_gamepause& manager;

public: // player_events::*
    virtual bool on_update();
    virtual void on_spawn();
    virtual void on_timer500();

public: // Публичный интерфейс
    bool is_sleep() const; // Возращает истину, если игрок спит

private:
    void process_exit();
private:
    time_base::millisecond_t time_update2; // Время последнего апдейта
    boost::optional<time_base::millisecond_t> sleep_start2; // Время начала спанья

    //boost::posix_time::ptime time_update; // Время последнего апдейта
    //boost::optional<boost::posix_time::ptime> sleep_start; // Время начала спанья
    int progress_index;

private:
    void do_sleep_enter();
    void do_sleep_leave(bool is_exit = false);
    void do_sleep_update(int sleep_time);
    bool get_sleep_time(int& sleep_time);
    void update_timer();
};

#endif // PLAYER_GAMEPAUSE_HPP
