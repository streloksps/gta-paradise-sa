#ifndef CHECKPOINTS_HPP
#define CHECKPOINTS_HPP
#include "core/module_h.hpp"
#include "core/dynamic_items.hpp"
#include "core/streamer.hpp"
#include "checkpoints_t.hpp"
#include "pos_change_checker.hpp"
#include <vector>

class checkpoints_item;
class checkpoints
    :public application_item
    ,public players_events::on_connect_i
{
public:
    typedef std::tr1::shared_ptr<checkpoints> ptr;
    static ptr instance();

public:
    friend class checkpoints_item;
    checkpoints();
    virtual ~checkpoints();

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

public:
    enum {invalid_checkpoint_id = -1};

    // Добавляет чикпоинт
    int  add(checkpoint_t const& checkpoint, checkpoint_event_t const& on_enter, checkpoint_event_t const& on_leave);
    
    // Удаляет чикпоинт по ид
    void remove(int id);

    // Возращает ид чикпоинта, в котором находится игрок или invalid_checkpoint_id, если игрок не в чикпоинтах
    int get_active_checkpoint(player_ptr_t const& player_ptr) const;
private:
    typedef streamer::streamer_t<checkpoint_t> checkpoint_streamer_t;
    checkpoint_streamer_t checkpoint_streamer;

    typedef std::vector<checkpoint_data_t> checkpoints_data_t;
    checkpoints_data_t checkpoints_data;

    // Вызывается при изменении чикпоинтов
    void on_update();
};

class checkpoints_item
    :public player_item
    ,public on_timer250_i
    ,public player_events::on_enter_checkpoint_i
    ,public player_events::on_leave_checkpoint_i
    ,public player_events::on_spawn_i
    ,public player_events::on_health_suicide_i
    ,public player_events::on_health_kill_i
{
public:
    typedef std::tr1::shared_ptr<checkpoints_item> ptr;

private:
    friend class checkpoints;
    checkpoints_item(checkpoints& manager);
    virtual ~checkpoints_item();

public: // player_events::*
    virtual void on_timer250();
    virtual void on_enter_checkpoint();
    virtual void on_leave_checkpoint();
    virtual void on_spawn();
    virtual void on_health_suicide(int reason, bool is_by_script);
    virtual void on_health_kill(player_ptr_t const& killer_ptr, int reason);


private:
    checkpoints& manager;
    pos_change_checker checker;

private:
    void on_update();
    void process_update_pos();
    
    int active_checkpoint_id;
    bool is_in_checkpoint;
    bool is_ingame;

    void show(int id);  // Показываем заданный чикпоинт
    void hide();        // Скрываем заданный чикпоинт

    void on_death_impl();
public:
    // Возращает ид чикпоинта, в котором находится игрок или invalid_checkpoint_id, если игрок не в чикпоинтах
    int get_active_checkpoint() const;
};
#endif // CHECKPOINTS_HPP
