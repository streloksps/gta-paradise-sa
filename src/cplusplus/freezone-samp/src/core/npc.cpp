#include "config.hpp"
#include "npc.hpp"
#include "core/module_c.hpp"

enum fm_rpc_e {
     fm_rpc_playback_start_none = 100
    ,fm_rpc_playback_start_driver
    ,fm_rpc_playback_start_onfoot
    ,fm_rpc_playback_stop
    ,fm_rpc_playback_pause
    ,fm_rpc_playback_resume   
};

npc::npc(unsigned int id)
:id(id)
,api_ptr(samp::api::instance())
,spawn_skin_id(0)
,spawn_color(0)
,is_spawn_seted(false)
,is_spawned(false)
,is_kicked(false)
{
    name = api_ptr->get_player_name(id);
}

npc::~npc() {

}

void npc::on_request_class() {
    api_ptr->set_spawn_info(id, 69, spawn_skin_id, spawn_pos.x, spawn_pos.y, spawn_pos.z, spawn_pos.rz, -1, -1, -1, -1, -1, -1);
    is_spawn_seted = true;
}

void npc::on_spawn() {
    api_ptr->set_player_color(id, spawn_color);
    api_ptr->set_player_interior(id, spawn_pos.interior);
    api_ptr->set_player_virtual_world(id, spawn_pos.world);
    is_spawned = true;
}

void npc::set_spawn_info(int skin_id, pos4 const& pos) {
    spawn_skin_id = skin_id;
    spawn_pos = pos;
    assert(!is_spawn_seted && "Пока можно устанавливать параметры только до спавна");
}

void npc::set_color(unsigned int color) {
    spawn_color = color;
    if (is_spawned) {
        api_ptr->set_player_color(id, spawn_color);
    }
}

void npc::put_to_vehicle(int vehicle_id) {
    if (!is_spawned) {
        assert(false);
        return;
    }
    api_ptr->put_player_in_vehicle(id, vehicle_id, 0);
}

void npc::playback_start(playback_type_e playback_type, std::string const& recording_name) {
    if (!is_spawned) {
        assert(false);
        return;
    }
    if (playback_type_none == playback_type) {
        api_ptr->send_client_message(id, static_cast<unsigned int>(fm_rpc_playback_start_none), recording_name);
    }
    else if (playback_type_driver == playback_type) {
        api_ptr->send_client_message(id, static_cast<unsigned int>(fm_rpc_playback_start_driver), recording_name);
    }
    else if (playback_type_onfoot == playback_type) {
        api_ptr->send_client_message(id, static_cast<unsigned int>(fm_rpc_playback_start_onfoot), recording_name);
    }
}

void npc::playback_stop() {
    if (!is_spawned) {
        assert(false);
        return;
    }
    api_ptr->send_client_message(id, static_cast<unsigned int>(fm_rpc_playback_stop), "");
}

void npc::playback_pause() {
    if (!is_spawned) {
        assert(false);
        return;
    }
    api_ptr->send_client_message(id, static_cast<unsigned int>(fm_rpc_playback_pause), "");
}

void npc::playback_resume() {
    if (!is_spawned) {
        assert(false);
        return;
    }
    api_ptr->send_client_message(id, static_cast<unsigned int>(fm_rpc_playback_resume), "");
}

void npc::kick() {
    is_kicked = true;
}

void npc::do_kick() {
    if (is_kicked) {
        api_ptr->kick(id);
    }
}

bool npc::is_valid() const {
    return !is_kicked;
}
