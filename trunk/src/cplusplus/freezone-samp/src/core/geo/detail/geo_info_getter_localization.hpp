#ifndef GEO_INFO_GETTER_LOCALIZATION_HPP
#define GEO_INFO_GETTER_LOCALIZATION_HPP
#include "geo_info_getter.hpp"
#include <string>
#include <unordered_map>

namespace geo {
    class info_getter_localization: public info_getter {
    private:
        std::tr1::unordered_map<std::string, std::string> countries_ru;
    public:
        info_getter_localization();
        virtual ~info_getter_localization();
        virtual bool get_db_info(std::string& db_info_string) const;

    public: // info_getter
        virtual void process_ip_info(ip_info& info);
    };
} // namespace geo {
#endif // GEO_INFO_GETTER_LOCALIZATION_HPP
