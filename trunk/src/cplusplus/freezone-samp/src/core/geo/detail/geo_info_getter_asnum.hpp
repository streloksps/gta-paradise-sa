#ifndef GEO_INFO_GETTER_ASNUM_HPP
#define GEO_INFO_GETTER_ASNUM_HPP
#include "geo_info_getter_db.hpp"

namespace geo {
    class info_getter_asnum: public info_getter_db {
    public:
        info_getter_asnum(GeoIPTag* gi);
        virtual ~info_getter_asnum();
    public: // info_getter
        virtual void process_ip_info(ip_info& info);
    protected:
        virtual std::string get_db_info_prefix() const;
    };
} // namespace geo {
#endif // GEO_INFO_GETTER_ASNUM_HPP
