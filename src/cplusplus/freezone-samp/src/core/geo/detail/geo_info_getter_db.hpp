#ifndef GEO_INFO_GETTER_DB_HPP
#define GEO_INFO_GETTER_DB_HPP
#include <boost/utility.hpp>
#include "geo_info_getter.hpp"
#include "maxmind-geoip/GeoIP_fwd.h"

namespace geo {
    class info_getter_db: public info_getter, private boost::noncopyable {
    private:
        static std::string get_info_copy(std::string const& src_info);
    protected:
        GeoIPTag* db;
        std::string info;
        virtual std::string get_db_info_prefix() const = 0;
    public:
        info_getter_db(GeoIPTag* db);
        virtual ~info_getter_db();
        virtual bool get_db_info(std::string& db_info_string) const;

        static ptr try_create_db(char const * filename);
    };
} // namespace geo {
#endif // GEO_INFO_GETTER_DB_HPP
