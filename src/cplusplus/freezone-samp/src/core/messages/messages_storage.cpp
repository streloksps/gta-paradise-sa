#include "config.hpp"
#include "messages_storage.hpp"
#include <boost/lexical_cast.hpp>
#include "core/buffer/buffer_reader.hpp"
#include "core/utility/directory.hpp"
#include "core/application.hpp"
#include "core/geo/geo_data_base.hpp"
#include "core/samp/samp_api.hpp"
#include "server_properties.hpp"
#include "core/ver.hpp"

REGISTER_IN_APPLICATION(messages_storage);

messages_storage::messages_storage()
:root_buff(new buffer())
,work_buff(root_buff->children_create_connected())
{
}

messages_storage::~messages_storage() {
}

void messages_storage::create() {

}

void messages_storage::configure_pre() {
    work_buff->clear();
}

void messages_storage::configure(buffer::ptr const& buff, def_t const& def) {
}

void messages_storage::configure_post() {
    init_server_properties();
}

void messages_storage::init_server_properties() {
    server_properties::ptr props = server_properties::instance();
    
    work_buff->value_add("server_name", props->get_name());
    work_buff->value_add("server_site", props->get_site());
    work_buff->value_add("server_site_full", props->get_site_full());
    work_buff->value_add("server_mode", props->get_mode());
    work_buff->value_add("server_mode_full", props->get_mode_name() + " " + mode_only_ver);
    work_buff->value_add("server_map", props->get_map());
    work_buff->value_add("server_port", boost::lexical_cast<std::string>(props->get_port()));
    work_buff->value_add("server_address", props->get_address());
    work_buff->value_add("server_version", application::instance()->get_server_version());
    work_buff->value_add("server_bind_ip", samp::api::instance()->get_server_var_as_string("bind"));
    work_buff->value_add("geo_dbs", geo::data_base::instance()->get_dbs_info());
}

buffer::ptr_c messages_storage::get_msg_buff() const {
    return work_buff;
}

messages_storage::ptr messages_storage::instance() {
    return application::instance()->get_item<messages_storage>();
}

buffer::ptr const& messages_storage::get_root_buff() const {
    return root_buff;
}
