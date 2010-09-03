#ifndef NPC_TAXI_T_HPP
#define NPC_TAXI_T_HPP
#include <istream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "basic_types.hpp"
#include "server_paths.hpp"
#include "core/npc_t.hpp"
#include "core/serialization/configuradable.hpp"

// Описание смещения координат остановки (в цилиндрической системе координат) относительно координаты транспорта
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct npc_taxi_route_vehicle_stop_t {
    float shift_radius;             // Радиус смещения остановки в плоскости xy относительно позиции транспорта
    float shift_angle;              // Угол, по которому отсчитывается shift_radius
    float shift_z;                  // Смещение остановки по координате z, относительно центра транспорта
    float cp_visibility_radius;     // Радиус видимости чикпоинта
    float cp_size;                  // Радиус чикпоинта
    npc_taxi_route_vehicle_stop_t(): shift_radius(0.0f), shift_angle(0.0f), shift_z(0.0f), cp_visibility_radius(50.0f), cp_size(1.0f) {}

    bool operator==(npc_taxi_route_vehicle_stop_t const& right) const {
        return shift_radius == right.shift_radius
            && shift_angle == right.shift_angle
            && shift_z == right.shift_z
            && cp_visibility_radius == right.cp_visibility_radius
            && cp_size == right.cp_size
            ;
    }
    bool operator<(npc_taxi_route_vehicle_stop_t const& right) const {
        if (shift_radius == right.shift_radius) {
            if (shift_angle == right.shift_angle) {
                if (shift_z == right.shift_z) {
                    if (cp_visibility_radius == right.cp_visibility_radius) {
                        return cp_size < right.cp_size;
                    }
                    return cp_visibility_radius < right.cp_visibility_radius;
                }
                return shift_z < right.shift_z;
            }
            return shift_angle < right.shift_angle;
        }
        return shift_radius < right.shift_radius;
    }
    operator bool() const {
        return 0.0f != shift_radius
            && 0.0f != shift_angle
            && 0.0f != shift_z
            ;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, npc_taxi_route_vehicle_stop_t& vehicle_stop) {
    return is>>vehicle_stop.shift_radius>>vehicle_stop.shift_angle>>vehicle_stop.shift_z>>vehicle_stop.cp_visibility_radius>>vehicle_stop.cp_size;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <npc_taxi_route_vehicle_stop_t>: std::tr1::true_type {};
} // namespace serialization {

// Описание возможных моделей транспорта для маршрута такси
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct npc_taxi_route_vehicle_t {
    std::string                     model_name;
    int                             skin_id;
    npc_taxi_route_vehicle_stop_t   stop;
    npc_taxi_route_vehicle_t(): model_name("none"), skin_id(0) {}
    npc_taxi_route_vehicle_t(std::string const& model_name, int skin_id): model_name(model_name), skin_id(skin_id) {}

    bool operator==(npc_taxi_route_vehicle_t const& right) const {
        return model_name == right.model_name
            && skin_id == right.skin_id
            && stop == right.stop;
            ;
    }
    bool operator<(npc_taxi_route_vehicle_t const& right) const {
        if (model_name == right.model_name) {
            if (skin_id == right.skin_id) {
                return stop < right.stop;
            }
            return skin_id < right.skin_id;
        }
        return model_name < right.model_name;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, npc_taxi_route_vehicle_t& route_vehicle) {
    is>>route_vehicle.model_name>>route_vehicle.skin_id;
    if (!is.eof() && !is.fail()) {
        is >> route_vehicle.stop;
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <npc_taxi_route_vehicle_t>: std::tr1::true_type {};
} // namespace serialization {

// Описание отрезков маршрута
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct npc_taxi_route_section_t {
    std::string             file_path;
    int                     stop_time;
    std::string             stop_sys_name;
    std::string             stop_name;
    npc_recording_desc_t    desc;               // Описание маршрута, берется из метафайла

    npc_taxi_route_section_t(): stop_time(0) {}

    inline std::string get_src_filename_rec() const {
        return std::string(PATH_REC_DIR) + "/taxi/" + file_path + ".rec";
    }
    inline std::string get_src_filename_meta() const {
        return std::string(PATH_REC_DIR) + "/taxi/" + file_path + ".meta";
    }
    inline std::string get_rec_name() const {
        return "taxi/" + file_path;
    }

    inline boost::filesystem::path get_filename_rec() const {
        return boost::filesystem::path("npcmodes/recordings") / (get_rec_name() + ".rec");
    }

    // Удаляет рабочую директорию с записями. Рекомендуется вызывать при старте сервера
    inline static void truncate_rec_dir() {
        remove_all(boost::filesystem::path("npcmodes/recordings/taxi"));
    }

    inline void init_rec() const {
        boost::filesystem::path rec_filename(get_filename_rec());
        create_directories(rec_filename.parent_path());
        boost::filesystem::copy_file(boost::filesystem::path(get_src_filename_rec()), rec_filename, boost::filesystem::copy_option::overwrite_if_exists);
    }

    bool operator==(npc_taxi_route_section_t const& right) const {
        return file_path == right.file_path
            && stop_time == right.stop_time
            && stop_sys_name == right.stop_sys_name
            && stop_name == right.stop_name
            && desc == right.desc
            ;
    }

    bool operator<(npc_taxi_route_section_t const& right) const {
        if (file_path == right.file_path) {
            if (stop_time == right.stop_time) {
                if (stop_sys_name == right.stop_sys_name) {
                    if (stop_name == right.stop_name) {
                        return desc < right.desc;
                    }
                    return stop_name < right.stop_name;
                }
                return stop_sys_name < right.stop_sys_name;
            }
            return stop_time < right.stop_time;
        }
        return file_path < right.file_path;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, npc_taxi_route_section_t& section) {
    std::string stop_time_str;

    std::getline(is, section.file_path, ';');       boost::trim(section.file_path);
    std::getline(is, stop_time_str, ';');           boost::trim(stop_time_str);
    std::getline(is, section.stop_sys_name, ';');   boost::trim(section.stop_sys_name);
    std::getline(is, section.stop_name);            boost::trim(section.stop_name);

    if (is.eof() && !is.fail()) {
        {
            std::string::iterator stop_time_str_it = stop_time_str.begin();
            if (!boost::spirit::qi::parse(stop_time_str_it, stop_time_str.end(), boost::spirit::qi::int_, section.stop_time)) {
                is.setstate(std::ios_base::failbit);
                return is;
            }
        }
        { // Читаем файл с методанными
            std::ifstream in(section.get_src_filename_meta().c_str());
            if (!in.is_open()) {
                is.setstate(std::ios_base::failbit);
                return is;
            }
            in >> section.desc;
            if (!in.eof() || is.fail()) {
                is.setstate(std::ios_base::failbit);
                return is;
            }
        }
        if (!boost::filesystem::exists(section.get_src_filename_rec())) {
            is.setstate(std::ios_base::failbit);
            return is;
        }
    }

    return is;
}

// Описание маршрута
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <npc_taxi_route_section_t>: std::tr1::true_type {};
} // namespace serialization {

struct npc_taxi_route_t: public configuradable {
public: // configuradable
    virtual void configure_pre() {}
    virtual void configure(buffer::ptr const& buff, def_t const& def) {
        SERIALIZE_ITEM(sys_name);
        SERIALIZE_ITEM(name);
        SERIALIZE_ITEM(price);
        SERIALIZE_ITEM(npc_color);
        SERIALIZE_ITEM(interior);
        SERIALIZE_ITEM(world);
        SERIALIZE_ITEM(count);
        SERIALIZE_ITEM(route_vehicles);
        SERIALIZE_ITEM(route_sections);
        full_route_vehicles();
    }
    virtual void configure_post() {}
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(npc_taxi_route);

public:
    typedef std::vector<npc_taxi_route_vehicle_t> route_vehicles_t;
    typedef std::vector<npc_taxi_route_section_t> route_sections_t;

    std::string         sys_name;       // Системное название маршрута, пишется в логи
    std::string         name;           // Название маршрута, выводится игрокам
    int                 price;          // Цена, чтобы сесть в такси
    unsigned int        npc_color;      // Цвет бота, для отображения его на радаре
    int                 interior;       // Номер интрьера, где проходит маршрут
    int                 world;          // Номер мира маршрута
    int                 count;          // Количество транспорта на маршруте
    route_vehicles_t    route_vehicles; // Перечень возможного транспорта на маршруте (транспорт выбирается случайным образом)
    route_sections_t    route_sections; // Описание маршрута

    bool operator==(npc_taxi_route_t const& right) const {
        return sys_name == right.sys_name
            && name == right.name
            && price == right.price
            && npc_color == right.npc_color
            && interior == right.interior
            && world == right.world
            && route_vehicles == right.route_vehicles
            && route_sections == right.route_sections
            && count == right.count
            ;
    }

    bool operator<(npc_taxi_route_t const& right) const {
        if (sys_name == right.sys_name) {
            if (name == right.name) {
                if (price == right.price) {
                    if (npc_color == right.npc_color) {
                        if (interior == right.interior) {
                            if (world == right.world) {
                                if (route_vehicles == right.route_vehicles) {
                                    if (route_sections == right.route_sections) {
                                        return count < right.count;
                                    }
                                    return route_sections < right.route_sections;
                                }
                                return route_vehicles < right.route_vehicles;
                            }
                            return world < right.world;
                        }
                        return interior < right.interior;
                    }
                    return npc_color < right.npc_color;
                }
                return price < right.price;
            }
            return name < right.name;
        }
        return sys_name < right.sys_name;
    }
    void full_route_vehicles() {
        if (route_vehicles.empty() && !route_sections.empty()) {
            // Если список транспорта пустой, то добавляем 1 модель из мето данных
            route_vehicles.push_back(npc_taxi_route_vehicle_t(route_sections.front().desc.model_name, route_sections.front().desc.skin_id));
        }
    }
};

#endif // NPC_TAXI_T_HPP
