#ifndef VEHICLES_T_HPP
#define VEHICLES_T_HPP
#include <string>
#include <vector>
#include <set>
#include <istream>
#include <ostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "basic_types.hpp"
#include "checkpoints_t.hpp"
#include "core/serialization/configuradable.hpp"

struct vehicle_def_t {
    enum type_e {
        type_unknown = 0 //Неизвесные
        ,type_bike        //Мотоциклы
        ,type_bicycle     //Велосипеды
        ,type_airplane    //Самолеты
        ,type_helicopter  //Вертолеты
        ,type_boat        //Лодки
        ,type_service     //Служебные машины
        ,type_offroad     //Внедорожники
        ,type_train       //Поезда
        ,type_rc          //Радио управляемы модели
        ,type_lowrider    //Лоурайдеры
        ,type_trailer     //Прицепы
        ,type_industrial  //Промышленный транспорт
        ,type_sport       //Спортивные автомобили
        ,type_unique      //Редкое авто
        ,type_saloon      //Седаны
        ,type_cabriolet   //Кабриолеты
        ,type_estate      //Универсалы
    };

    enum modification_e {
        modification_none = 0              //не модифицируются
        ,modification_transfender          //Transfender
        ,modification_loco_low_co          //Loco Low Co
        ,modification_wheel_arch_angels    //Wheel Arch Angels
    };

    struct color_item_t {
        int color1;
        int color2;
        color_item_t(int color1, int color2): color1(color1), color2(color2) {}
    };

    std::string sys_name;
    std::string game_type;
    bool valid;
    std::string name;
    std::string real_name;
    type_e type;
    modification_e modification;
    int colors;
    int seats;
    float z_delta;
    std::string comment;
    std::vector<int> paintjobs;
    std::set<std::string> components;
    std::vector<color_item_t> color_items;
    bool can_attach_trailer;
    bool can_change_plate;

    vehicle_def_t()
        :valid(false)
        ,type(type_unknown)
        ,modification(modification_none)
        ,colors(0)
        ,seats(0)
        ,z_delta(0.0f)
        ,can_attach_trailer(false)
        ,can_change_plate(false)
    {}
};


template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, vehicle_def_t& def) {
    try {
        std::string valid_str;
        std::string type_str;
        std::string modification_str;
        std::string colors_str;
        std::string seats_str;
        std::string z_delta_str;
        std::string paintjobs_str;
        std::string components_str;
        std::string color_items_str;
        std::string can_attach_trailer_str;
        std::string can_change_plate_str;


        std::getline(is, def.sys_name, ';');            boost::trim(def.sys_name);
        std::getline(is, def.game_type, ';');           boost::trim(def.game_type);
        std::getline(is, valid_str, ';');               boost::trim(valid_str);
        std::getline(is, def.name, ';');                boost::trim(def.name);
        std::getline(is, def.real_name, ';');           boost::trim(def.real_name);
        std::getline(is, type_str, ';');                boost::trim(type_str);
        std::getline(is, modification_str, ';');        boost::trim(modification_str);
        std::getline(is, colors_str, ';');              boost::trim(colors_str);
        std::getline(is, seats_str, ';');               boost::trim(seats_str);
        std::getline(is, z_delta_str, ';');             boost::trim(z_delta_str);
        std::getline(is, def.comment, ';');             boost::trim(def.comment);
        std::getline(is, paintjobs_str, ';');           boost::trim(paintjobs_str);
        std::getline(is, components_str, ';');          boost::trim(components_str);
        std::getline(is, color_items_str, ';');         boost::trim(color_items_str);
        std::getline(is, can_attach_trailer_str, ';');  boost::trim(can_attach_trailer_str);
        std::getline(is, can_change_plate_str);         boost::trim(can_change_plate_str);

        if (is.eof() && !is.fail()) {
            def.valid = boost::iequals(valid_str, "true");

            if (boost::iequals(type_str, "bike"))
                def.type = vehicle_def_t::type_bike;
            else if (boost::iequals(type_str, "bicycle")) 
                def.type = vehicle_def_t::type_bicycle;
            else if (boost::iequals(type_str, "airplane")) 
                def.type = vehicle_def_t::type_airplane;
            else if (boost::iequals(type_str, "helicopter")) 
                def.type = vehicle_def_t::type_helicopter;
            else if (boost::iequals(type_str, "boat")) 
                def.type = vehicle_def_t::type_boat;
            else if (boost::iequals(type_str, "service")) 
                def.type = vehicle_def_t::type_service;
            else if (boost::iequals(type_str, "offroad")) 
                def.type = vehicle_def_t::type_offroad;
            else if (boost::iequals(type_str, "train")) 
                def.type = vehicle_def_t::type_train;
            else if (boost::iequals(type_str, "rc")) 
                def.type = vehicle_def_t::type_rc;
            else if (boost::iequals(type_str, "lowrider")) 
                def.type = vehicle_def_t::type_lowrider;
            else if (boost::iequals(type_str, "trailer")) 
                def.type = vehicle_def_t::type_trailer;
            else if (boost::iequals(type_str, "industrial")) 
                def.type = vehicle_def_t::type_industrial;
            else if (boost::iequals(type_str, "sport"))
                def.type = vehicle_def_t::type_sport;
            else if (boost::iequals(type_str, "unique"))
                def.type = vehicle_def_t::type_unique;
            else if (boost::iequals(type_str, "saloon"))
                def.type = vehicle_def_t::type_saloon;
            else if (boost::iequals(type_str, "cabriolet"))
                def.type = vehicle_def_t::type_cabriolet;
            else if (boost::iequals(type_str, "estate"))
                def.type = vehicle_def_t::type_estate;
            else 
                def.type = vehicle_def_t::type_unknown;

            
            if (boost::iequals(modification_str, "transfender"))
                def.modification = vehicle_def_t::modification_transfender;
            else if (boost::iequals(modification_str, "loco_low_co"))
                def.modification = vehicle_def_t::modification_loco_low_co;
            else if (boost::iequals(modification_str, "wheel_arch_angels"))
                def.modification = vehicle_def_t::modification_wheel_arch_angels;
            else 
                def.modification = vehicle_def_t::modification_none;

            def.colors = boost::lexical_cast<int>(colors_str);
            if (def.colors > 2 || def.colors < 0) def.colors = 0;

            def.seats = boost::lexical_cast<int>(seats_str);
            if (def.seats > 10 || def.seats < 0) def.seats = 0;

            def.z_delta = boost::lexical_cast<float>(z_delta_str);

            def.can_attach_trailer = boost::iequals(can_attach_trailer_str, "true");

            def.can_change_plate = boost::iequals(can_change_plate_str, "true");

            for (boost::split_iterator<std::string::iterator> paintjobs_it = boost::make_split_iterator(paintjobs_str, boost::token_finder(boost::is_any_of(", "), boost::token_compress_on)), end; paintjobs_it != end; ++paintjobs_it) {
                std::string paintjob = boost::copy_range<std::string>(*paintjobs_it);
                if (!paintjob.empty()) {
                    def.paintjobs.push_back(boost::lexical_cast<int>(paintjob));
                }
            }

            for (boost::split_iterator<std::string::iterator> components_it = boost::make_split_iterator(components_str, boost::token_finder(boost::is_any_of(", "), boost::token_compress_on)), end; components_it != end; ++components_it) {
                std::string component = boost::copy_range<std::string>(*components_it);
                if (!component.empty()) {
                    def.components.insert(component);
                }
            }

            for (boost::split_iterator<std::string::iterator> color_items_it = boost::make_split_iterator(color_items_str, boost::token_finder(boost::is_any_of(", "), boost::token_compress_on)), end;;) {
                if (color_items_it == end) break;
                std::string color1_str = boost::copy_range<std::string>(*color_items_it);
                ++color_items_it;
                if (color_items_it == end) break;
                std::string color2_str = boost::copy_range<std::string>(*color_items_it);
                ++color_items_it;

                try {
                    def.color_items.push_back(vehicle_def_t::color_item_t(boost::lexical_cast<int>(color1_str), boost::lexical_cast<int>(color2_str)));
                }
                catch (boost::bad_lexical_cast &) {
                    assert(false);
                }
            }
        }
    }
    catch (boost::bad_lexical_cast &) {
        assert(false);
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <vehicle_def_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct vehicle_component_def_t {
    typedef std::vector<int> ids_t;
    int         slot;
    int         price;
    ids_t       ids;
    std::string group;
    std::string name;
    vehicle_component_def_t(): slot(0), price(0) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, vehicle_component_def_t& def) {
    std::string slot_str;
    std::string price_str;
    std::string ids_str;
    std::getline(is, slot_str, ';');        boost::trim(slot_str);
    std::getline(is, price_str, ';');       boost::trim(price_str);
    std::getline(is, ids_str, ';');         boost::trim(ids_str);
    std::getline(is, def.group, ';');       boost::trim(def.group);
    std::getline(is, def.name);             boost::trim(def.name);

    try {
        if (is.eof() && !is.fail()) {
            def.slot = boost::lexical_cast<int>(slot_str);
            def.price = boost::lexical_cast<int>(price_str);
            for (boost::split_iterator<std::string::iterator> ids_it = boost::make_split_iterator(ids_str, boost::token_finder(boost::is_any_of(", "), boost::token_compress_on)), end; ids_it != end; ++ids_it) {
                def.ids.push_back(boost::lexical_cast<int>(boost::copy_range<std::string>(*ids_it)));
            }
        }
    }
    catch (boost::bad_lexical_cast &) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}
#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <vehicle_component_def_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct pos_vehicle: public pos4 {
    int model_id;
    int color1;
    int color2;
    int respawn_delay; // Время респавна, секунды
    pos_vehicle(int model_id = 0, float x = .0f, float y = .0f, float z = .0f, int interior = 0, int world = 0, float rz =.0f, int color1 = 0, int color2 = 0, int respawn_delay = 10 * 60)
        :model_id(model_id), color1(color1), color2(color2), respawn_delay(respawn_delay)
        ,pos4(x, y, z, interior, world, rz) {}

    bool operator==(pos_vehicle const& right) const {
        return model_id == right.model_id
            && color1 == right.color1
            && color2 == right.color2
            && respawn_delay == right.respawn_delay
            && pos4::operator==(right)
            ;
    }
    bool operator<(pos_vehicle const& right) const {
        if (model_id == right.model_id) {
            // Цвета и время респавна не учитываем при сверке
            return pos4::operator<(right);
        }
        return model_id < right.model_id;
    }
};
template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, pos_vehicle& pos) {
    return is>>pos.model_id>>pos.x>>pos.y>>pos.z>>pos.interior>>pos.world>>pos.rz>>pos.color1>>pos.color2>>pos.respawn_delay;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, pos_vehicle const& pos) {
    return os<<pos.model_id<<" "<<pos.x<<" "<<pos.y<<" "<<pos.z<<" "<<pos.interior<<" "<<pos.world<<" "<<pos.rz<<" "<<pos.color1<<" "<<pos.color2<<" "<<pos.respawn_delay;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <pos_vehicle>: std::tr1::true_type {};
    template <> struct is_streameble_write<pos_vehicle>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct vehicle_color_t {
    int id;
    int color;
    std::string name;
    vehicle_color_t(): id(0), color(0) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, vehicle_color_t& color) {
    is>>color.id>>color.color;
    std::getline(is, color.name);
    boost::trim(color.name);
    return is;
}
template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, vehicle_color_t const& color) {
    return os<<color.id<<" "<<color.color;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <vehicle_color_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mod_garage_item_t: checkpoint_t {
    std::string     sys_name; // Служебное название, для логов
    float           mapicon_radius; // Радиус отрисовки иконки на радаре

    mod_garage_item_t(): mapicon_radius(0.0f) {}

    bool operator==(mod_garage_item_t const& right) const {
        return sys_name == right.sys_name
            && mapicon_radius == right.mapicon_radius
            && checkpoint_t::operator==(right)
            ;
    }
    bool operator<(mod_garage_item_t const& right) const {
        if (sys_name == right.sys_name) {
            if (mapicon_radius == right.mapicon_radius) {
                return checkpoint_t::operator<(right);
            }
            return mapicon_radius < right.mapicon_radius;
        }
        return sys_name < right.sys_name;
    }
};
template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, mod_garage_item_t& garage) {
    return is>>garage.x>>garage.y>>garage.z>>garage.interior>>garage.world>>garage.radius>>garage.size>>garage.sys_name>>garage.mapicon_radius;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <mod_garage_item_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct repair_garage_item_t: checkpoint_t {
    std::string     sys_name;           // Служебное название, для логов
    float           mapicon_radius;     // Радиус отрисовки иконки на радаре
    int             limit_player_count; // Сколько может воспользоваться один игрок. Если = 0 - то без лимита
    int             limit_player_time;  // Время действия ограничения, секунды
    int             limit_global_count; // Если = 0 - то без лимита
    int             limit_global_time;  //

    repair_garage_item_t(): mapicon_radius(0.0f), limit_player_count(0), limit_player_time(0), limit_global_count(0), limit_global_time(0) {}

    bool operator==(repair_garage_item_t const& right) const {
        return sys_name == right.sys_name
            && mapicon_radius == right.mapicon_radius
            && limit_player_count == right.limit_player_count
            && limit_player_time == right.limit_player_time
            && limit_global_count == right.limit_global_count
            && limit_global_time == right.limit_global_time
            && checkpoint_t::operator==(right)
            ;
    }
    bool operator<(repair_garage_item_t const& right) const {
        if (sys_name == right.sys_name) {
            if (mapicon_radius == right.mapicon_radius) {
                if (limit_player_count == right.limit_player_count) {
                    if (limit_player_time == right.limit_player_time) {
                        if (limit_global_count == right.limit_global_count) {
                            if (limit_global_time == right.limit_global_time) {
                                return checkpoint_t::operator<(right);
                            }
                            return limit_global_time < right.limit_global_time;
                        }
                        return limit_global_count < right.limit_global_count;
                    }
                    return limit_player_time < right.limit_player_time;
                }
                return limit_player_count < right.limit_player_count;
            }
            return mapicon_radius < right.mapicon_radius;
        }
        return sys_name < right.sys_name;
    }
};
template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, repair_garage_item_t& repair_item) {
    return is>>repair_item.x>>repair_item.y>>repair_item.z>>repair_item.interior>>repair_item.world>>repair_item.radius>>repair_item.size>>repair_item.sys_name>>repair_item.mapicon_radius>>repair_item.limit_player_count>>repair_item.limit_player_time>>repair_item.limit_global_count>>repair_item.limit_global_time;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <repair_garage_item_t>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct mod_garage_t: public configuradable {
public: // configuradable
    virtual void configure_pre() {}
    virtual void configure(buffer::ptr const& buff, def_t const& def) {
        SERIALIZE_ITEM(name);
        SERIALIZE_ITEM(sys_name);
        SERIALIZE_ITEM(mapicon_marker_type);
        SERIALIZE_DELIMITED_ITEM(valid_vehicles);
        SERIALIZE_ITEM(items);
    }
    virtual void configure_post() {}
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(mod_garage);

public:
    mod_garage_t():mapicon_marker_type(0) {}

public:
    typedef std::set<std::string> valid_vehicles_t;
    typedef std::vector<mod_garage_item_t> mod_garage_items_t;
    
    std::string         name;
    std::string         sys_name;
    int                 mapicon_marker_type;
    valid_vehicles_t    valid_vehicles;
    mod_garage_items_t  items;

    bool operator==(mod_garage_t const& right) const {
        return name == right.name
            && sys_name == right.sys_name
            && mapicon_marker_type == right.mapicon_marker_type
            && valid_vehicles == right.valid_vehicles
            && items == right.items
            ;
    }
    bool operator<(mod_garage_t const& right) const {
        if (name == right.name) {
            if (sys_name == right.sys_name) {
                if (mapicon_marker_type == right.mapicon_marker_type) {
                    if (valid_vehicles == right.valid_vehicles) {
                        return items < right.items;
                    }
                    return valid_vehicles < right.valid_vehicles;
                }
                return mapicon_marker_type < right.mapicon_marker_type;
            }
            return sys_name < right.sys_name;
        }
        return name < right.name;
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct garage_val_t {
    int checkpoint_id;
    mod_garage_t const* mod_garage_ptr;
    mod_garage_item_t const* garage_ptr;
    int mapicon_id;
    garage_val_t(): mod_garage_ptr(0), garage_ptr(0), mapicon_id(-1), checkpoint_id(-1) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct repair_garage_t: public configuradable {
public: // configuradable
    virtual void configure_pre() {}
    virtual void configure(buffer::ptr const& buff, def_t const& def) {
        SERIALIZE_ITEM(name);
        SERIALIZE_ITEM(sys_name);
        SERIALIZE_ITEM(mapicon_marker_type);
        SERIALIZE_ITEM(valid_vehicles);
        SERIALIZE_ITEM(price);
        SERIALIZE_ITEM(items);
    }
    virtual void configure_post() {}
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(repair_garage);

public:
    repair_garage_t(): mapicon_marker_type(0), price(0) {}
public:
    typedef std::set<std::string> valid_vehicles_t;
    typedef std::vector<repair_garage_item_t> repair_garage_items_t;

    std::string             name;
    std::string             sys_name;
    int                     mapicon_marker_type;
    valid_vehicles_t        valid_vehicles;
    int                     price;
    repair_garage_items_t   items;
};

typedef vehicle_def_t const* vehicle_def_cptr_t;
typedef vehicle_def_t const& vehicle_def_cref_t;

#endif // VEHICLES_T_HPP
