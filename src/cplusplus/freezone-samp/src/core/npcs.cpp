#include "config.hpp"
#include "npcs.hpp"
#include "core/module_c.hpp"
#include "npc.hpp"
#include "npc_i.hpp"
#include "npcs_i.hpp"

REGISTER_IN_APPLICATION(npcs);

npcs::ptr npcs::instance() {
    return application::instance()->get_item<npcs>();
}

npcs::npcs() {

}

npcs::~npcs() {

}

void npcs::create() {
    api_ptr = samp::api::instance();
}

void npcs::configure_pre() {
    allow_ips.clear();
}

void npcs::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(allow_ips);
}

void npcs::configure_post() {
    if (allow_ips.empty()) {
        std::string bind_ip = api_ptr->get_server_var_as_string("bind");
        if (bind_ip.empty()) {
            allow_ips.insert("127.0.0.1");
        }
        else {
            allow_ips.insert(bind_ip);
        }
    }
}

void npcs::on_player_connect(int player_id) {
    if (!api_ptr->is_has_030_features() || !api_ptr->is_player_npc(player_id)) {
        // Это не бот
        return;
    }
    std::string ip_string = api_ptr->get_player_ip(player_id);
    std::string name = api_ptr->get_player_name(player_id);

    messages_item msg_item;
    msg_item.get_params()
        ("id", player_id)
        ("name", name)
        ("ip", ip_string)
        ;

    if (allow_ips.end() == allow_ips.find(ip_string)) {
        // Бот подключился с неправильного адреса
        msg_item.get_sender()("$(npcs_reject_bad_ip_log)", msg_log);
        api_ptr->kick(player_id);
        return;
    }

    npc::ptr_t npc_ptr(new npc(player_id));
    std::pair<npcs_holder_t::iterator, bool> insert_rezult = npcs_holder.insert(std::make_pair(player_id, npc_ptr));
    assert(insert_rezult.second && "Ошибка добавление игрока");
    
    container_execute_handlers(application::instance(), &npcs_events::on_npc_connect_i::on_npc_connect, npc_ptr);
    container_execute_handlers(npc_ptr, &npc_events::on_connect_i::on_connect);
    
    msg_item.get_sender()("$(npcs_connect_log)", msg_log);
}

void npcs::on_player_request_class(int player_id, int class_id) {
    npcs_holder_t::iterator npc_ptr_it = npcs_holder.find(player_id);
    if (npcs_holder.end() != npc_ptr_it) {
        container_execute_handlers(npc_ptr_it->second, &npc_events::on_request_class_i::on_request_class);
    }
}

bool npcs::on_player_request_spawn(int player_id) {
    npcs_holder_t::iterator npc_ptr_it = npcs_holder.find(player_id);
    if (npcs_holder.end() != npc_ptr_it) {
        return container_execute_handlers(npc_ptr_it->second, &npc_events::on_request_spawn_i::on_request_spawn);
    }
    return true;
}

void npcs::on_player_spawn(int player_id) {
    npcs_holder_t::iterator npc_ptr_it = npcs_holder.find(player_id);
    if (npcs_holder.end() != npc_ptr_it) {
        container_execute_handlers(npc_ptr_it->second, &npc_events::on_spawn_i::on_spawn);
    }
}

void npcs::on_player_commandtext(int player_id, std::string const& cmd) {
    npcs_holder_t::iterator npc_ptr_it = npcs_holder.find(player_id);
    if (npcs_holder.end() != npc_ptr_it) {
        // Парсим rpc событие
        if ("{event} playback_end" == cmd) {
            container_execute_handlers(npc_ptr_it->second, &npc_events::on_playback_end_i::on_playback_end);
        }
        if ("{event} vehicle_enter" == cmd) {
            container_execute_handlers(npc_ptr_it->second, &npc_events::on_enter_vehicle_i::on_enter_vehicle, -1);
        }
    }
}

void npcs::on_player_keystate_change(int player_id, int keys_new, int keys_old) {
    npcs_holder_t::iterator npc_ptr_it = npcs_holder.find(player_id);
    if (npcs_holder.end() != npc_ptr_it) {
        container_execute_handlers(npc_ptr_it->second, &npc_events::on_keystate_change_i::on_keystate_change, keys_new, keys_old);
    }
}

void npcs::on_player_disconnect(int player_id, samp::player_disconnect_reason reason) {
    npcs_holder_t::iterator npc_ptr_it = npcs_holder.find(player_id);
    if (npcs_holder.end() != npc_ptr_it) {
        messages_item msg_item;
        msg_item.get_params()
            ("id", npc_ptr_it->second->id_get())
            ("name", npc_ptr_it->second->name_get())
            ("reason", static_cast<int>(reason))
            ;
        msg_item.get_sender()("$(npcs_disconnect_log)", msg_log);

        container_execute_handlers(npc_ptr_it->second, &npc_events::on_disconnect_i::on_disconnect, reason);
        container_execute_handlers(application::instance(), &npcs_events::on_npc_disconnect_i::on_npc_disconnect, npc_ptr_it->second, reason);
        npcs_holder.erase(npc_ptr_it);
    }
}

void npcs::on_post_gamemode_exit(AMX* amx) {

}

void npcs::add_npc(std::string const& name) {
    connect_queue.push_back(name);
}

void npcs::on_timer500() {
    // Отключаем всех, кто у нас был кикнут
    for (npcs_holder_t::iterator it = npcs_holder.begin(), end = npcs_holder.end(); end != it;) {
        npcs_holder_t::iterator curr_it = it;
        ++it;
        curr_it->second->do_kick();
    }

    // Подключаем тех, кто в очереди на подключение
    for (connect_queue_t::const_iterator it = connect_queue.begin(), end = connect_queue.end(); end != it; ++it) {
        api_ptr->connect_npc(*it, "freemode_v030a");
    }
    connect_queue.clear();
}
