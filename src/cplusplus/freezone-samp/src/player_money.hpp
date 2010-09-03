#ifndef PLAYER_MONEY_HPP
#define PLAYER_MONEY_HPP
#include "core/module_h.hpp"
#include "core/samp/samp_api.hpp"

class player_money_item;
class player_money
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public samp::on_gamemode_init_i
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<player_money> ptr;

public:
    player_money();
    virtual ~player_money();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_money);

public: // samp::on_gamemode_init_i
    virtual void on_gamemode_init(AMX*, samp::server_ver ver);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

private:
    friend class player_money_item;
    int max_player_money;
    int send_money_min;
    int send_money_max;
    float send_distance_max;

    bool is_block_on_maxmoney;  // Разрешить блоичить за превышение максимального количества денег
    samp::api::ptr api_ptr;
private:
    command_arguments_rezult cmd_money_send(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
};


class player_money_item
    :public player_item
    ,public player_events::on_death_post_i
    ,public player_events::on_update_i
{
private:
    friend class player_money;
    typedef std::tr1::shared_ptr<player_money_item> ptr;
    player_money_item(player_money& manager);
    virtual ~player_money_item();

public:
    int  get() const; // Возращает сколько у игрока денег
    int  get_server() const; // Возращает только серверные деньги
    void take(int money); // Забирает заданную сумму у игрока
    void give(int money); // Дает заданную сумму игроку
    void reset(); // Обнуляет деньги игрока
    bool can_take(int money) const; // Истина, если у игрока можно забрать заданную сумму
    bool can_give(int money) const; // Истина, если игроку можно передать заданную сумму
private:
    player_money& manager;
    int server_money;
    int server_money_max;
public: // player_events::*
    virtual void on_death_post(player_ptr_t const& killer_ptr, int reason);
    virtual bool on_update();
};

#endif // PLAYER_MONEY_HPP
