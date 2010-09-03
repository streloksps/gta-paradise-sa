#include "config.hpp"
#include "geo_info_getter_db.hpp"
#include <assert.h>
#include "geo_info_getter_asnum.hpp"
#include "geo_info_getter_city.hpp"
#include "geo_info_getter_country.hpp"
#include "geo_info_getter_org.hpp"
#include "maxmind-geoip/GeoIP.h"
#include "core/application.hpp"
#include "core/messages/messages_item.hpp"


namespace geo {
    std::string info_getter_db::get_info_copy(std::string const& src_info) {
        std::size_t pos = src_info.find(" Copyright");
        if (std::string::npos == pos) {
            return src_info;
        }
        else {
            return src_info.substr(0, pos);
        }
    }

    info_getter_db::info_getter_db(GeoIPTag* db):db(db) {
        assert(db);
        info = "none";

        if (char* db_info_char = GeoIP_database_info(db)) {
            info = get_info_copy(db_info_char);
            free(db_info_char);
        }
        messages_item msg_item;
        msg_item.get_params()
            ("geo_db_filename", db->file_path)
            ("geo_db_type", GeoIPDBDescription[GeoIP_database_edition(db)])
            ("geo_db_info", info);
        msg_item.get_sender()
            ("$(geo_db_load_item)", msg_debug);
    }

    info_getter_db::~info_getter_db() {
        if (db) {
            GeoIP_delete(db);
            db = 0;
        }
    }

    info_getter::ptr info_getter_db::try_create_db(char const * filename) {
        // GEOIP_MEMORY_CACHE - «агружает всю базу в пам€ть - много пам€ти потребл€ет
        // GEOIP_STANDARD - »спользует поиск по диску
        GeoIP* geo_ptr = GeoIP_open(filename, GEOIP_STANDARD);
        if (geo_ptr) {
            if (GEOIP_COUNTRY_EDITION == geo_ptr->databaseType) {
                return info_getter::ptr(new info_getter_country(geo_ptr));
            }
            else if (GEOIP_ASNUM_EDITION == geo_ptr->databaseType) {
                return info_getter::ptr(new info_getter_asnum(geo_ptr));
            }
            else if (GEOIP_CITY_EDITION_REV1 == geo_ptr->databaseType) {
                return info_getter::ptr(new info_getter_city(geo_ptr));
            }
            else if (GEOIP_ORG_EDITION == geo_ptr->databaseType) {
                return info_getter::ptr(new info_getter_org(geo_ptr));
            }
            GeoIP_delete(geo_ptr);
        }

        return info_getter::ptr();
    }

    bool info_getter_db::get_db_info(std::string& db_info_string) const {
        db_info_string = get_db_info_prefix() + " - " + info;
        return true;
    }
} // namespace geo {
