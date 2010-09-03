#ifndef VEHICLE_HPP
#define VEHICLE_HPP
#include "basic_types.hpp"
#include "core/module_h.hpp"
#include "vehicles_t.hpp"
#include "vehicle_fwd.hpp"

class vehicles;
class vehicle_t {
    friend class vehicles;
private:
    vehicle_t(vehicles& manager, pos_vehicle const& pos);
    bool create();
    bool create_static();

    void create_impl_pre(pos_vehicle& pos) const;
    bool create_impl_post(pos_vehicle& pos);

public:
    int get_id() const;
    bool get_is_static() const;
    static bool is_valid_id(int id);

private: // Постоянные параметры
    vehicles& manager;
    int id;
    bool is_static;
    pos_vehicle pos_spawn;

public: // Текущие параметры 
    typedef std::map<int /* slot */, std::string /* component name */> mods_t;
    enum {paintjob_default = 3};
    enum {any_color = -1};

    mods_t mods;
    int paintjob_id;
    int color1;
    int color2;
    int pos_interior;
    int pos_world;

    void clear();
    bool is_need_change_color() const;

public: // Координаты
    pos4 pos_get();
    void pos_set(pos4 const& pos);

public: // Получение постоянной инфы
    int                 get_model() const;
    vehicle_def_cptr_t  get_def() const;
    vehicle_def_cref_t  get_def_throw() const;
    
    bool is_valid_component(int component_id) const;                    // Возращает истину, если можно установить указанный компонент на данный транспорт
    bool is_valid_component(std::string const& component_name) const;   // Возращает истину, если можно установить указанный компонент на данный транспорт

    void add_component(vehicle_component_def_t const& component_def);
    void add_component(std::string const& component_name);
    void remove_component(vehicle_component_def_t const& component_def);
    void remove_component(std::string const& component_name);

};
#endif // VEHICLE_HPP
