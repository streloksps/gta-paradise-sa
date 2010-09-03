#ifndef VEHICLES_HPP
#define VEHICLES_HPP
#include "core/module_h.hpp"
#include "vehicle.hpp"
#include "vehicles_t.hpp"
#include <vector>
#include <string>
#include <map>
#include <set>
#include "basic_types.hpp"
#include "core/dynamic_items.hpp"
#include "core/utility/locale_ru.hpp"
#include "vehicle_fwd.hpp"

class vehicles
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public samp::on_gamemode_init_i
    ,public samp::on_vehicle_spawn_i
    ,public samp::on_vehicle_mod_i
    ,public samp::on_vehicle_paintjob_i
    ,public samp::on_vehicle_respray_i
    ,public samp::on_player_connect_i
{
    friend class vehicle_t;
public:
    typedef std::tr1::shared_ptr<vehicles> ptr;
    static ptr instance();

public:
    vehicles();
    virtual ~vehicles();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(vehicles);

public: // samp::*
    virtual void on_gamemode_init(AMX* amx, samp::server_ver ver);
    virtual void on_vehicle_spawn(int vehicle_id);
    virtual bool on_vehicle_mod(int player_id, int vehicle_id, int component_id);
    virtual bool on_vehicle_paintjob(int player_id, int vehicle_id, int paintjob_id);
    virtual bool on_vehicle_respray(int player_id, int vehicle_id, int color1, int color2);
    virtual void on_player_connect(int player_id);

private:
    void on_first_player_connected();

public:
    vehicle_ptr_t get_vehicle(int vehicle_id);
    vehicle_ref_t get_vehicle_throw(int vehicle_id);

private:
    typedef std::map<int, vehicle_def_t> vehicle_defs_t;
    vehicle_defs_t vehicle_defs;

    typedef std::map<std::string, vehicle_component_def_t> vehicle_component_defs_t;
    vehicle_component_defs_t vehicle_component_defs;

    // Список имен компонетов, которые не проверяются на дубликаты
    std::set<std::string> vehicle_component_ignore_duplicate;

    typedef dynamic_items<pos_vehicle, int> vehiclesgame_t;
    typedef std::vector<pos_vehicle> vehicles_static_t; // Транспорт, который будет создаваться только при загрузке игрового режима

    vehiclesgame_t vehiclesgame;
    vehicles_static_t vehicles_static;
    
    bool is_first_player_connected;

    std::string plate;

    typedef std::vector<vehicle_color_t> vehicle_colors_t;
    vehicle_colors_t vehicle_colors;

    bool is_dump_vehicles_on_config;
    void dump_vehicles(vehiclesgame_t::update_t& vehicles);

private:
    // Текущие параметры транспорта
    typedef std::map<int, vehicle_t> vehicles_state_t;
    vehicles_state_t vehicles_state;

public:
    int vehicle_create(pos_vehicle const& pos);
    void vehicle_destroy(int id);

public:
    bool get_component_from_id(int component_id, vehicle_component_defs_t::const_iterator& comp_it) const;

    bool is_valid_component(int model_id, int component_id);
    bool is_valid_component(int model_id, std::string const& component_name);
    bool is_valid_paintjob(int model_id, int paintjob_id);
    bool is_valid_colors(int model_id, int color1, int color2);
private:
    bool is_player_can_tune(player_ptr_t const& player_ptr);
    bool cmd_vehicle_paintjob_access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name);
    bool cmd_vehicle_color_access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name);
    bool cmd_vehicle_modification_access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name);
    bool cmd_vehicle_paintjob_access_checker_impl(player_ptr_t const& player_ptr);
    bool cmd_vehicle_color_access_checker_impl(player_ptr_t const& player_ptr);
    bool cmd_vehicle_modification_access_checker_impl(player_ptr_t const& player_ptr);

    bool get_vehicle_def_by_player(player_ptr_t const& player_ptr, int& vehicle_id, vehicle_defs_t::const_iterator& vehicle_def_it) const;
    bool get_trailer_def_by_player(player_ptr_t const& player_ptr, int& trailer_id, vehicle_defs_t::const_iterator& trailer_def_it) const;

    command_arguments_rezult cmd_vehicle_paintjob(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_vehicle_color(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_vehicle_modification(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_vehicle_modification_impl(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    command_arguments_rezult cmd_vehicle_color_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_vehicle_modification_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
private:
    void set_params(vehicle_defs_t::const_iterator const& def_it, messages_params& params, std::string const& prefix = "v_");
    void set_params_colors(int index, messages_params& params, std::string const& prefix);

public:
    std::string get_vehicle_info(player_ptr_t const& player_ptr);
    int get_max_vehicle_id() const;
    bool is_valid_vehicle_id(int vehicle_id) const;
    bool is_vehicle_created_by_me(int vehicle_id) const; // Возращает истину, если вихикл создан модом
    pos4 pos_get(int vehicle_id) const;
    bool get_model_id_by_name(std::string const& gta_model_name, int& model_id) const;
    bool get_component_id_by_name(std::string const& gta_component_name, int& first_component_id) const;
    std::string get_component_name_by_id(int component_id);

private:
    int vehicle_paintjob_price;
    int vehicle_color_price;

    bool is_block_on_vehicle_mod;
    bool is_block_on_vehicle_paintjob;
    bool is_block_on_vehicle_respray;

    bool is_trace_enter_leave_mod;
private:
    typedef std::pair<std::string /* gta name */, vehicle_component_def_t const*> mod_item_t;
    typedef std::multimap<std::string, mod_item_t> mods_group_item_t;
    typedef std::map<std::string, mods_group_item_t, ilexicographical_less> mods_groups_t;
    void get_mods_groups(vehicle_defs_t::const_iterator const& vehicle_def_it, mods_groups_t& mods_groups);
    void add_component(vehicle_component_def_t const& component_def, int vehicle_id);
    void remove_component(vehicle_component_def_t const& component_def, int vehicle_id);

private: // Мод гаражи
    typedef std::vector<mod_garage_t> mod_garages_t;
    typedef std::map<int, garage_val_t> garage_vals_t;
    mod_garages_t mod_garages;
    garage_vals_t garage_vals;

    void mod_garages_destroy();
    void mod_garages_create();
    void mod_garage_item_on_enter(player_ptr_t const& player_ptr, int id);
    void mod_garage_item_on_leave(player_ptr_t const& player_ptr, int id);
private: // Горажи починки
    typedef std::vector<repair_garage_t> repair_garages_t;
    repair_garages_t repair_garages;

    void repair_garages_destroy();
    void repair_garages_create();
    void repair_garage_item_on_enter(player_ptr_t const& player_ptr, int id);
    void repair_garage_item_on_leave(player_ptr_t const& player_ptr, int id);
};
#endif // VEHICLES_HPP
