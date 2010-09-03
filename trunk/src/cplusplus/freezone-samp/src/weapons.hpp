#ifndef WEAPONS_HPP
#define WEAPONS_HPP
#include <map>
#include <vector>
#include <set>
#include <boost/optional.hpp>
#include "core/module_h.hpp"
#include "core/dynamic_items.hpp"
#include "core/streamer.hpp"
#include "weapons_t.hpp"

class weapons_item;
class weapons
    :public application_item
    ,public createble_i 
    ,public configuradable
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<weapons> ptr;
    static ptr instance();

public:
    friend class weapons_item;
    weapons();
    virtual ~weapons();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(weapons);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

private:
    std::set<int>       weapons_can_get_ingame;
    bool                is_block_on_weapon_id;

private:
    typedef std::map<std::string, weapon_def_t> weapon_defs_t;
    weapon_defs_t weapon_defs;
    weapon_defs_t weapon_defs_pre_config;

private:
    struct weapon_buy_key_t {
        int pickup_id;
        int streamer_id;
        weapon_buy_key_t(int pickup_id, int streamer_id): pickup_id(pickup_id), streamer_id(streamer_id) {}
        bool operator==(weapon_buy_key_t const& right) const {
            return pickup_id == right.pickup_id
                && streamer_id == right.streamer_id
                ;
        }
    };
    typedef dynamic_items<weapon_buy_item_t, weapon_buy_key_t> weapon_buy_items_t;
    weapon_buy_items_t  buffer;
    weapon_buy_key_t    buy_item_create(weapon_buy_item_t const& weapon_buy_item);
    void                buy_item_destroy(weapon_buy_key_t id);

private:
    float pickups_visibility_radius;
    weapon_buy_key_t const invalid_id;

private:
    typedef streamer::streamer_t<weapon_buy_item_t> weapon_buy_streamer_t;
    weapon_buy_streamer_t streamer;
    void streamer_item_deleter(int key);

private:
    text_draw::td_item::ptr buy_text;
};

class weapons_item
    :public player_item
    ,public on_timer250_i
    ,public player_events::on_keystate_change_i
    ,public player_events::on_update_i
{
public:
    typedef std::tr1::shared_ptr<weapons_item> ptr;

public: // player_events::
    virtual void on_keystate_change(int keys_new, int keys_old);
    virtual void on_timer250();
    virtual bool on_update(); //Истина = разрешить передавать изменения на других игроков

private:
    friend class weapons;
    weapons_item(weapons& manager);
    virtual ~weapons_item();

private:
    weapons& manager;

private:
    typedef dynamic_items<int /* icon_key */> weapon_buy_player_t;
    weapon_buy_player_t weapon_buy_player;

public:
    void delete_event(int icon_key);

private:
    dynamic_dummy on_enter(int key_id);
    void          on_exit(dynamic_dummy);
    boost::optional<int> active_key_id;
    bool is_msg_sended;

public:
    void            set_skills(weapon_skills_t& skills);

    void            weapon_reset(); // Обнуляет оружие у игрока
    void            weapon_add(weapon_item_t const& weapon); // Добавляет оружие игроку
    void            weapon_add(weapons_t const& weapons); // Добавляет список оружия игроку
    weapons_t       weapon_get(); // Возращает массив оружия игрока. Последним идет активное оружие

private:
    typedef std::set<int> player_can_have_weapons_t;
    player_can_have_weapons_t player_can_have_weapons;
};

#endif // WEAPONS_HPP
