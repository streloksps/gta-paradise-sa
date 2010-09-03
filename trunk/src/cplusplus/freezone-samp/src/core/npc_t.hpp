#ifndef NPC_T_HPP
#define NPC_T_HPP
#include "basic_types.hpp"

// Описатель файла записи бота
struct npc_recording_desc_t {
    std::string     model_name; // Название модели транспорта, в котором производится запись, либо onfoot, если пешком
    int             skin_id;    // Ид скина, которым записывалось
    int             duration;   // Продолжительность записи, милллисекунды
    pos4            point_begin;// Позиция начала записи
    pos4            point_end;  // Позиция конца записи
    
    bool is_on_vehicle() const {
        return "onfoot" != model_name;
    }
    npc_recording_desc_t(): model_name("onfoot"), skin_id(0), duration(0) {}

    bool operator==(npc_recording_desc_t const& right) const {
        return model_name == right.model_name
            && skin_id == right.skin_id
            && duration == right.duration
            && point_begin == right.point_begin
            && point_end == right.point_end
            ;
    }

    bool operator<(npc_recording_desc_t const& right) const {
        if (model_name == right.model_name) {
            if (skin_id == right.skin_id) {
                if (duration == right.duration) {
                    if (point_begin == right.point_begin) {
                        return point_end < right.point_end;
                    }
                    return point_begin < right.point_begin;
                }
                return duration < right.duration;
            }
            return skin_id < right.skin_id;
        }
        return model_name < right.model_name;
    }

};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, npc_recording_desc_t& desc) {
    return is>>desc.model_name>>desc.skin_id>>desc.duration>>desc.point_begin.interior
        >>desc.point_begin.x>>desc.point_begin.y>>desc.point_begin.z>>desc.point_begin.rz
        >>  desc.point_end.x>>  desc.point_end.y>>  desc.point_end.z>>  desc.point_end.rz
        ;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, npc_recording_desc_t const& desc) {
    return os << boost::basic_format<char_t>("%1% %2% %3% %4% %5$0.4f %6$0.4f %7$0.4f %8$0.3f %9$0.4f %10$0.4f %11$0.4f %12$0.3f") 
        % desc.model_name % desc.skin_id % desc.duration % desc.point_begin.interior
        % desc.point_begin.x % desc.point_begin.y % desc.point_begin.z % desc.point_begin.rz
        %   desc.point_end.x %   desc.point_end.y %   desc.point_end.z %   desc.point_end.rz
        ;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <npc_recording_desc_t>: std::tr1::true_type {};
    template <> struct is_streameble_write<npc_recording_desc_t>: std::tr1::true_type {};
} // namespace serialization {

#endif // NPC_T_HPP
