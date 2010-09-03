#ifndef NPCS_HPP
#define NPCS_HPP
#include <map>
#include <set>
#include <vector>
#include <string>
#include "npc_fwd.hpp"
#include "core/module_h.hpp"
#include "core/samp/samp_events_i.hpp"
#include "core/samp/samp_api.hpp"

class npcs
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public on_timer500_i
    ,public samp::on_player_connect_i
    ,public samp::on_player_request_class_i
    ,public samp::on_player_request_spawn_i
    ,public samp::on_player_spawn_i
    ,public samp::on_player_commandtext_i
    ,public samp::on_player_disconnect_i
    ,public samp::on_player_keystate_change_i
    ,public samp::on_post_gamemode_exit_i
{
public:
    typedef std::tr1::shared_ptr<npcs> ptr;
    static ptr instance();

public: // Публичные методы
    void add_npc(std::string const& name); // Подключает новый NPC

public:
    npcs();
    virtual ~npcs();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(npcs);

public:
    virtual void on_timer500();

public: // samp::*
    virtual void on_player_connect(int player_id);
    virtual void on_player_request_class(int player_id, int class_id);
    virtual bool on_player_request_spawn(int player_id); // Истина = разрешить спавн
    virtual void on_player_spawn(int player_id);
    virtual void on_player_commandtext(int player_id, std::string const& cmd);
    virtual void on_player_disconnect(int player_id, samp::player_disconnect_reason reason);
    virtual void on_player_keystate_change(int player_id, int keys_new, int keys_old);
    virtual void on_post_gamemode_exit(AMX* amx);

private: // Настройка
    typedef std::set<std::string> allow_ips_t;
    allow_ips_t allow_ips;

public:
    typedef std::map<unsigned int, npc_ptr_t> npcs_holder_t;
    npcs_holder_t npcs_holder;

private:
    samp::api::ptr api_ptr;

private:
    typedef std::vector<std::string> connect_queue_t;
    connect_queue_t connect_queue;
};
#endif // NPCS_HPP
