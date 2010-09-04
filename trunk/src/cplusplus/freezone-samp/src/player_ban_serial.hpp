#ifndef PLAYER_BAN_SERIAL_HPP
#define PLAYER_BAN_SERIAL_HPP
#include "core/module_h.hpp"
#include "player_ban_serial_item.hpp"
#include <string>
#include <set>
#include <map>
#include <bitset>

struct ban_reason_t {
    enum ban_reason_e {
        reason_none = 0,
        reason_ip = 1 << 0,
        reason_serial = 1 << 1,
        reason_as_num = 1 << 2,
        reason_ban_item = 1 << 3,
        reason_ban_item_dynamic = 1 << 4
    };
    static inline std::string get_reason_string(ban_reason_e reason) {
        std::bitset<5> b(reason);
        return b.to_string();
    }
};

inline ban_reason_t::ban_reason_e operator +=(ban_reason_t::ban_reason_e& a, ban_reason_t::ban_reason_e b) {
    a = static_cast<ban_reason_t::ban_reason_e>(a + b);
    return a;
}

class player_ban_serial
    :public application_item
    ,public configuradable
    ,public createble_i
    ,public players_events::on_rejectable_connect_i
    ,public on_timer15000_i
{
public:
    typedef std::tr1::shared_ptr<player_ban_serial> ptr;
    static ptr instance();

public:
    player_ban_serial();
    virtual ~player_ban_serial();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_ban_serial);

private:
    command_arguments_rezult cmd_ban_serial_unban(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_ban_serial_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

public: // players_events::on_rejectable_connect_i
    virtual bool on_rejectable_connect(player_ptr_t const& player_ptr);
    virtual void on_timer15000();

public:
    void ban_serial(player_ptr_t const& player_ptr);    

private:
    typedef time_outs<player_ban_serial_item_t, int> prebans_t;
    typedef time_outs<player_ban_serial_item_t> bans_t;
    
    typedef std::set<std::string>               ips_t;
    typedef std::set<std::string>               serials_t;
    typedef std::set<int>                       as_nums_t;
    typedef std::set<player_ban_serial_item_t>  ban_items_t;

    bool is_serial_bans_active;
    int preban_min_count;
    int preban_timeout;
    int bans_timeout;
    
    // Список белых листов
    ips_t       whitelist_ips;
    serials_t   whitelist_serials;
    as_nums_t   whitelist_as_nums;
    ban_items_t whitelist_ban_items;

    // Список черных листов
    ips_t       blacklist_ips;
    serials_t   blacklist_serials;
    as_nums_t   blacklist_as_nums;
    ban_items_t blacklist_ban_items;

    prebans_t prebans;
    bans_t bans;

private:
    void on_unban_serial(player_ban_serial_item_t const& item, time_base::millisecond_t const& last_update, time_outs_dummy_val const& dummy);
    bool access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name);
    bool is_banned(player_ban_serial_item_t const& item, std::string const& ip_string, ban_reason_t::ban_reason_e& reason) const;
};

#endif // PLAYER_BAN_SERIAL_HPP
