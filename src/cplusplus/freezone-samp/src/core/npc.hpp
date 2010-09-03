#ifndef NPC_HPP
#define NPC_HPP
#include "npc_fwd.hpp"
#include "npc_i.hpp"
#include "core/module_h.hpp"
#include "basic_types.hpp"

class npc
    :public container
    ,public npc_events::on_request_class_i
    ,public npc_events::on_spawn_i
{
public:
    typedef npc_ptr_t ptr_t;
    enum playback_type_e {
         playback_type_none
        ,playback_type_driver
        ,playback_type_onfoot
    };

public:
    npc(unsigned int id);
    virtual ~npc();

public:
    unsigned int id_get() const {return id;}
    std::string const& name_get() const {return name;}

public:
    virtual void on_request_class();
    virtual void on_spawn();

public: // Методы работы с npc
    // Желательно вызвать во время конекта
    void set_spawn_info(int skin_id, pos4 const& pos = pos4());
    void set_color(unsigned int color);

    void put_to_vehicle(int vehicle_id);
    void playback_start(playback_type_e playback_type, std::string const& recording_name);
    void playback_stop();
    void playback_pause();
    void playback_resume();

    void kick();
    bool is_valid() const; // Возращает истину, если бот не был кикнут
    
public: // Внутренние метода
    void do_kick(); // Кикает бота, если в этом есть необходимость

private:
    samp::api::ptr api_ptr;
    unsigned int id;
    std::string name;
    bool is_spawn_seted;
    bool is_spawned;

private:
    int             spawn_skin_id;
    pos4            spawn_pos;
    unsigned int    spawn_color;
    bool            is_kicked; // Если истина, то нужно выгнать бота с сервера
};
#endif // NPC_HPP
