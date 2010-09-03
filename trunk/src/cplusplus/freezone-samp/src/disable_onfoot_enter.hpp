#ifndef DISABLE_ONFOOT_ENTER_HPP
#define DISABLE_ONFOOT_ENTER_HPP
#include "core/module_h.hpp"
#include "disable_onfoot_enter_t.hpp"
#include "core/streamer.hpp"

class disable_onfoot_enter
    :public application_item
    ,public configuradable
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<disable_onfoot_enter> ptr;
    static ptr instance();
public:
    disable_onfoot_enter();
    virtual ~disable_onfoot_enter();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(disable_onfoot_enter);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

public:
    typedef streamer::streamer_t<disabled_zone_t> disabled_zones_streamer_t;
    disabled_zones_streamer_t disabled_zones_streamer;
};

class disable_onfoot_enter_item
    :public player_item
    ,public player_events::on_keystate_change_i
{
public:
    typedef std::tr1::shared_ptr<disable_onfoot_enter_item> ptr;

protected:
    friend class disable_onfoot_enter;
    disable_onfoot_enter_item(disable_onfoot_enter& manager);
    virtual ~disable_onfoot_enter_item();

private:
    disable_onfoot_enter& manager;

public: // player_events::*
    virtual void on_keystate_change(int keys_new, int keys_old);
};

#endif // DISABLE_ONFOOT_ENTER_HPP
