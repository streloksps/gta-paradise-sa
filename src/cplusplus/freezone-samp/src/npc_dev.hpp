#ifndef NPC_DEV_HPP
#define NPC_DEV_HPP
#include "core/module_h.hpp"
#include "core/npc_t.hpp"
#include "core/time_outs.hpp"

class npc_dev_player_item;
class npc_dev_npc_item;
class npc_dev
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public npcs_events::on_npc_connect_i
    ,public players_events::on_connect_i
{
public:
    npc_dev();
    virtual ~npc_dev();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(npc_dev);

public: // samp::*
    virtual void on_connect(player_ptr_t const& player_ptr);

public: // npcs_events::*
    virtual void on_npc_connect(npc_ptr_t const& npc_ptr);

private:
    friend class npc_dev_player_item;
    friend class npc_dev_npc_item;

private:
    bool is_enable;
    bool access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name);
    float recording_vehicle_health;
    float acceleration_multiplier;
    float deceleration_multiplier;

    std::string const play_npc_name;

private:
    command_arguments_rezult cmd_record(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_play(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_create_vehicle(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    std::string play_record_name;
    int         play_record_vehicle_id;
};

class npc_dev_player_item
    :public player_item
    ,public on_timer250_i
    ,public player_events::on_keystate_change_i
    ,public player_events::on_update_i
{
public:
    typedef std::tr1::shared_ptr<npc_dev_player_item> ptr;

private:
    friend class npc_dev;

    npc_dev_player_item(npc_dev& manager);
    virtual ~npc_dev_player_item();

public:
    virtual void on_timer250();

public: // player_events::
    virtual void on_keystate_change(int keys_new, int keys_old);
    virtual bool on_update();

private:
    npc_dev& manager;

private:
    bool is_wait_recording;
    std::string recording_prefix;
    bool is_accelerate;
    void recording_start(bool is_vehicle);
    void recording_stop();
    std::string get_next_recording_filename() const;

private: // Описыватель текущего рекординга
    bool                        is_recording;
    int                         recording_vehicle_id;
    std::string                 recording_name;
    npc_recording_desc_t        recording_desc;
    time_base::millisecond_t    recording_time_start;
    bool                        recording_is_can_use_nitro;
    static std::string          recording_get_rec_filename(std::string const& recording_name);
    static std::string          recording_get_meta_filename(std::string const& recording_name);

private:
    float accelerate(float val) const;
    float decelerate(float val) const;
};

class npc_dev_npc_item
    :public npc_item
    ,public npc_events::on_connect_i
    ,public npc_events::on_spawn_i
    ,public npc_events::on_enter_vehicle_i
    ,public npc_events::on_playback_end_i
    ,public npc_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<npc_dev_npc_item> ptr;

private:
    friend class npc_dev;

    npc_dev_npc_item(npc_dev& manager);
    virtual ~npc_dev_npc_item();

private:
    npc_dev& manager;

public: // npc_events::*
    virtual void on_connect();
    virtual void on_spawn();
    virtual void on_enter_vehicle(int vehicle_id);
    virtual void on_playback_end();
    virtual void on_disconnect(samp::player_disconnect_reason reason);
};

#endif // NPC_DEV_HPP
