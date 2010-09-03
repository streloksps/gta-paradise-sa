#ifndef PLAYER_BAN_SERIAL_HPP
#define PLAYER_BAN_SERIAL_HPP
#include "core/module_h.hpp"
#include <string>
#include <set>
#include <map>

struct player_ban_serial_item_t {
    int as_num;
    std::string serial;

    player_ban_serial_item_t();
    player_ban_serial_item_t(int as_num, std::string const& serial);
    player_ban_serial_item_t(player_ptr_t const& player_ptr);
    bool operator<(player_ban_serial_item_t const& right) const {
        if (as_num == right.as_num) return serial < right.serial;
        return as_num < right.as_num;
    }
    void dump_to_params(messages_params& params) const;
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, player_ban_serial_item_t& item) {
    is>>item.as_num>>item.serial;
    if (!is.eof()) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <player_ban_serial_item_t>: std::tr1::true_type {};
} // namespace serialization {


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
    typedef std::set<std::string> serials_t;
    typedef std::set<int> as_nums_t;
    typedef std::set<player_ban_serial_item_t> ban_items_t;

    bool is_serial_bans_active;
    int preban_min_count;
    int preban_timeout;
    int bans_timeout;
    
    // Список пермонентных банов
    serials_t perma_serials;
    as_nums_t perma_as_nums;
    ban_items_t perma_ban_items;

    prebans_t prebans;
    bans_t bans;

private:
    void on_unban_serial(player_ban_serial_item_t const& item, time_base::millisecond_t const& last_update, time_outs_dummy_val const& dummy);
    bool access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name);
    bool is_banned(player_ban_serial_item_t const& item) const;
};

#endif // PLAYER_BAN_SERIAL_HPP
