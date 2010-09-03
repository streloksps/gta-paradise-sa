#include "config.hpp"
#include "geo_data_base.hpp"
#include <algorithm>
#include <functional>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>

#include "core/utility/directory.hpp"

#include "core/application.hpp"
#include "core/messages/messages_item.hpp"

#include "detail/geo_info_getter_db.hpp"
#include "detail/geo_info_getter_ip_check.hpp"
#include "detail/geo_info_getter_localization.hpp"

#include "server_paths.hpp"

namespace geo {
    REGISTER_IN_APPLICATION(data_base);

    data_base::ptr data_base::instance() {
        return application::instance()->get_item<data_base>();
    }

    data_base::data_base():is_loaded(false) {
        
    }

    data_base::~data_base() {
        unload();
    }

    void data_base::create() {
    }

    void data_base::configure_pre() {
        unload();
    }

    void data_base::configure(buffer::ptr const& buff, def_t const& def) {
        load();
    }

    void data_base::configure_post() {

    }

    void data_base::load() {
        if (is_loaded) {
            return;
        }
        messages_item msg_item;
        msg_item.get_sender()("$(geo_db_load_begin)", msg_debug);

        info_getters.push_back(info_getter::ptr(new info_getter_ip_check()));
        iterate_directory(PATH_GEO_DIR, PATH_GEO_REGEX, std::tr1::bind(&data_base::try_load_db, this, std::tr1::placeholders::_1));
        info_getters.push_back(info_getter::ptr(new info_getter_localization()));
        is_loaded = true;

        msg_item.get_sender()("$(geo_db_load_end)", msg_debug);
    }

    void data_base::unload() {
        if (!is_loaded) {
            return;
        }
        // Крашит сервер при анлоаде
        //messages_item msg_item;
        //msg_item.get_sender()("$(geo_db_unload)", msg_debug);
        info_getters.clear();
        is_loaded = false;
    }

    void data_base::try_load_db(boost::filesystem::path const& file) {
        info_getter::ptr db = info_getter_db::try_create_db(file.native_file_string().c_str());
        if (db) {
            info_getters.push_back(db);
        }
    }

    ip_info data_base::get_info(std::string const& ip_string) {
        ip_info rezult;
        rezult.ip_string = ip_string;
        std::for_each(info_getters.begin(), info_getters.end(), std::tr1::bind(&info_getter::process_ip_info, std::tr1::placeholders::_1, std::tr1::ref(rezult)));
        return rezult;
    }

    std::string data_base::get_dbs_info() const {
        std::string rezult;
        BOOST_FOREACH(info_getter::ptr const& getter, info_getters) {
            std::string info;
            if (getter->get_db_info(info)) {
                if (rezult.empty()) {
                    rezult = info;
                }
                else {
                    rezult += "; " + boost::replace_all_copy(info, " ", "~");
                }
            }
        }
        return rezult;
    }
} // namespace geo {
