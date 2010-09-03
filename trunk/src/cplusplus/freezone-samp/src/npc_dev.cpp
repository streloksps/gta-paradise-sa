#include "config.hpp"
#include "npc_dev.hpp"
#include "core/module_c.hpp"
#include "core/npcs.hpp"
#include "vehicles.hpp"
#include "weapons.hpp"
#include "player_money.hpp"
#include <fstream>
#include <boost/filesystem.hpp>

REGISTER_IN_APPLICATION(npc_dev);

npc_dev::npc_dev()
:play_npc_name("npc_dev_play")
,play_record_vehicle_id(0)
{
}

npc_dev::~npc_dev() {
}

void npc_dev::create() {
    command_add("npc_dev_record", std::tr1::bind(&npc_dev::cmd_record, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&npc_dev::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("npc_dev_play", std::tr1::bind(&npc_dev::cmd_play, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&npc_dev::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("npc_dev_create_vehicle", std::tr1::bind(&npc_dev::cmd_create_vehicle, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&npc_dev::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
}

void npc_dev::configure_pre() {
    is_enable = false;
    recording_vehicle_health = 1000.0f;
    acceleration_multiplier = 1.2f;
    deceleration_multiplier = 0.8f;
}

void npc_dev::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(is_enable);
    SERIALIZE_ITEM(recording_vehicle_health);
    SERIALIZE_ITEM(acceleration_multiplier);
    SERIALIZE_ITEM(deceleration_multiplier);
}

void npc_dev::configure_post() {

}

void npc_dev::on_connect(player_ptr_t const& player_ptr) {
    npc_dev_player_item::ptr item(new npc_dev_player_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
}

void npc_dev::on_npc_connect(npc_ptr_t const& npc_ptr) {
    if (play_npc_name == npc_ptr->name_get()) {
        // Это наш бот для проигрования :)
        npc_dev_npc_item::ptr item(new npc_dev_npc_item(*this), &npc_item::do_delete);
        npc_ptr->add_item(item);
    }
}

bool npc_dev::access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name) {
    return is_enable;
}

command_arguments_rezult npc_dev::cmd_record(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    npc_dev_player_item::ptr npc_dev_player_item_ptr = player_ptr->get_item<npc_dev_player_item>();
    npc_dev_player_item_ptr->is_wait_recording = !npc_dev_player_item_ptr->is_wait_recording;
    if (npc_dev_player_item_ptr->is_wait_recording) {
        if (arguments.empty()) {
            npc_dev_player_item_ptr->recording_prefix = "record";
        }
        else {
            npc_dev_player_item_ptr->recording_prefix = arguments;
        }
        player_ptr->get_item<weapons_item>()->weapon_reset();
        player_ptr->get_item<player_money_item>()->reset();
        params("prefix", npc_dev_player_item_ptr->recording_prefix);
        pager("$(cmd_npc_dev_record_on_player)");
        sender("$(cmd_npc_dev_record_on_log)", msg_log);
    }
    else {
        if (npc_dev_player_item_ptr->is_recording) {
            npc_dev_player_item_ptr->recording_stop();
        }
        pager("$(cmd_npc_dev_record_off_player)");
        sender("$(cmd_npc_dev_record_off_log)", msg_log);
    }
    return cmd_arg_ok;
}

command_arguments_rezult npc_dev::cmd_play(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    {
        int vehicle_id;
        int model_id;
        bool is_driver;
        if (player_ptr->get_vehicle(vehicle_id, model_id, is_driver)) {
            play_record_vehicle_id = vehicle_id;
        }
        else {
            play_record_vehicle_id = 0;
        }
    }

    play_record_name = arguments;

    npcs::instance()->add_npc(play_npc_name);

    params("record_name", play_record_name);
    pager("$(cmd_npc_dev_play_player)");
    sender("$(cmd_npc_dev_play_log)", msg_log);
    return cmd_arg_ok;
}

command_arguments_rezult npc_dev::cmd_create_vehicle(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int model_id;
    { // Парсинг
        if (!vehicles::instance()->get_model_id_by_name(arguments, model_id)) {
            std::istringstream iss(arguments);
            iss>>model_id;
            if (iss.fail() || !iss.eof()) {
                return cmd_arg_syntax_error;
            }
        }
    }

    { // Проверяем, находится ли игрок в транспорте
        int vehicle_id = samp::api::instance()->get_player_vehicle_id(player_ptr->get_id());
        int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);
        if (0 != model_id) {
            pager("$(npc_dev_create_vehicle_error_invehicle)");
            return cmd_arg_process_error;
        }
    }

    pos4 pos = player_ptr->get_item<player_position_item>()->pos_get();

    int vehicle_id = vehicles::instance()->vehicle_create(pos_vehicle(model_id, pos.x, pos.y, pos.z + 1.5f, pos.interior, pos.world, pos.rz, -1, -1));
    samp::api::instance()->put_player_in_vehicle(player_ptr->get_id(), vehicle_id, 0);

    params
        ("model_id", model_id)
        ("vehicle_id", vehicle_id)
        ;
    pager("$(npc_dev_create_vehicle_done_player)");
    sender("$(npc_dev_create_vehicle_done_log)", msg_log);

    return cmd_arg_ok;
}

npc_dev_player_item::npc_dev_player_item(npc_dev& manager)
:manager(manager)
,is_wait_recording(false)
,is_recording(false)
,recording_vehicle_id(-1)
,recording_time_start(0)
,is_accelerate(false)
{

}

npc_dev_player_item::~npc_dev_player_item() {

}

void npc_dev_player_item::on_timer250() {
    if (!is_recording || 0 == recording_vehicle_id || !is_accelerate) {
        return;
    }

    int vehicle_id;
    int model_id;
    bool is_driver;
    if (get_root()->get_vehicle(vehicle_id, model_id, is_driver) && is_driver) {
        samp::api::ptr api_ptr = samp::api::instance();
        float x, y, z;
        int keys;
        int updown;
        int leftright;

        api_ptr->get_vehicle_velocity(vehicle_id, x, y, z);
        api_ptr->get_player_keys(get_root()->get_id(), keys, updown, leftright);
        if (keys & vehicle_accelerate) {
            api_ptr->set_vehicle_velocity(vehicle_id, accelerate(x), accelerate(y), accelerate(z));
        }
        else if (keys & vehicle_decelerate) {
            api_ptr->set_vehicle_velocity(vehicle_id, decelerate(x), decelerate(y), decelerate(z));
        }
    }
}

void npc_dev_player_item::on_keystate_change(int keys_new, int keys_old) {
    if (!is_wait_recording) {
        return;
    }
    if (is_recording && 0 != recording_vehicle_id) {
        if (recording_is_can_use_nitro) {
            if (is_key_just_down(mouse_lbutton, keys_new, keys_old)) {
                if (vehicle_ptr_t vehicle_ptr = vehicles::instance()->get_vehicle(recording_vehicle_id)) {
                    // Можем поставить нитро :)
                    vehicle_ptr->add_component("nto_b_tw");
                }
            }
            else if (is_key_just_down(KEY_SUBMISSION, keys_new, keys_old)) {
                if (vehicle_ptr_t vehicle_ptr = vehicles::instance()->get_vehicle(recording_vehicle_id)) {
                    // Убираем нитро
                    vehicle_ptr->remove_component("nto_b_tw");
                }
            }
        }
        if (is_key_just_down(KEY_ACTION, keys_new, keys_old)) {
            is_accelerate = true;
        }
        else if (is_key_just_up(KEY_ACTION, keys_new, keys_old)) {
            is_accelerate = false;
        }
    }
    if (is_key_just_down(KEY_LOOK_BEHIND, keys_new, keys_old) && !get_root()->vehicle_is_driver()) {
        // Запуск/остановка записи пешком
        if (is_recording) {
            recording_stop();
        }
        else {
            recording_start(false);
        }
    }
    else if (is_key_just_down(KEY_LOOK_RIGHT | KEY_LOOK_LEFT, keys_new, keys_old) && get_root()->vehicle_is_driver()) {
        // Запуск/остановка записи на транспорте
        if (is_recording) {
            recording_stop();
        }
        else {
            recording_start(true);
        }
    }
}

bool npc_dev_player_item::on_update() {
    if (is_recording) {
        samp::api::ptr api_ptr = samp::api::instance();
        int vehicle_id;
        int model_id;
        bool is_driver;
        if (get_root()->get_vehicle(vehicle_id, model_id, is_driver) && is_driver) {
            if (manager.recording_vehicle_health != api_ptr->get_vehicle_health(vehicle_id)) {
                //api_ptr->set_vehicle_health(vehicle_id, manager.recording_vehicle_health);
                api_ptr->repair_vehicle(vehicle_id);
            }
        }
        else {
        }
    }
    return true;
}

void npc_dev_player_item::recording_start(bool is_vehicle) {
    if (is_recording) {
        assert(false);
        return;
    }
    recording_name = get_next_recording_filename();
    recording_desc = npc_recording_desc_t();
    recording_vehicle_id = 0;
    recording_is_can_use_nitro = false;
    is_accelerate = false;
    
    samp::api::ptr api_ptr = samp::api::instance();

    recording_desc.skin_id = api_ptr->get_player_skin(get_root()->get_id());
    if (is_vehicle) {
        bool is_driver;
        int model_id;
        get_root()->get_vehicle(recording_vehicle_id, model_id, is_driver);
        api_ptr->get_vehicle_pos(recording_vehicle_id, recording_desc.point_begin.x, recording_desc.point_begin.y, recording_desc.point_begin.z);
        recording_desc.point_begin.rz = api_ptr->get_vehicle_zangle(recording_vehicle_id);
        if (vehicle_ptr_t vehicle_ptr = vehicles::instance()->get_vehicle(recording_vehicle_id)) {
            if (vehicle_def_cptr_t vehicle_def_cptr = vehicle_ptr->get_def()) {
                recording_desc.model_name = vehicle_def_cptr->sys_name;
            }
            recording_is_can_use_nitro = vehicle_ptr->is_valid_component("nto_b_tw");
        }
        api_ptr->repair_vehicle(recording_vehicle_id);
    }
    else {
        int player_id = get_root()->get_id();
        api_ptr->get_player_pos(player_id, recording_desc.point_begin.x, recording_desc.point_begin.y, recording_desc.point_begin.z);
        recording_desc.point_begin.rz = api_ptr->get_player_facing_angle(player_id);
    }
    recording_desc.point_begin.interior = api_ptr->get_player_interior(get_root()->get_id());

    messages_item msg_item;

    msg_item.get_params()
        ("player_name", get_root()->name_get_full())
        ("recording_name", recording_name)
        ;

    if (is_vehicle) {
        if (recording_is_can_use_nitro) {
            msg_item.get_sender()
                ("$(npc_dev_recordind_vehicle_nitro_start_player)", msg_player(get_root()))
                ("$(npc_dev_recordind_vehicle_nitro_start_log)", msg_log);
        }
        else {
            msg_item.get_sender()
                ("$(npc_dev_recordind_vehicle_start_player)", msg_player(get_root()))
                ("$(npc_dev_recordind_vehicle_start_log)", msg_log);
        }
        api_ptr->start_recording_player_data(get_root()->get_id(), samp::api::PLAYER_RECORDING_TYPE_DRIVER, recording_name);
    }
    else {
        msg_item.get_sender()
            ("$(npc_dev_recordind_onfoot_start_player)", msg_player(get_root()))
            ("$(npc_dev_recordind_onfoot_start_log)", msg_log);
        api_ptr->start_recording_player_data(get_root()->get_id(), samp::api::PLAYER_RECORDING_TYPE_ONFOOT, recording_name);
    }
    recording_time_start = time_base::tick_count_milliseconds();
    is_recording = true;
}

void npc_dev_player_item::recording_stop() {
    if (!is_recording) {
        assert(false);
        return;
    }

    samp::api::ptr api_ptr = samp::api::instance();

    time_base::millisecond_t recording_time_stop = time_base::tick_count_milliseconds();
    api_ptr->stop_recording_player_data(get_root()->get_id());
    is_recording = false;

    if (recording_desc.is_on_vehicle()) {
        api_ptr->get_vehicle_pos(recording_vehicle_id, recording_desc.point_end.x, recording_desc.point_end.y, recording_desc.point_end.z);
        recording_desc.point_end.rz = api_ptr->get_vehicle_zangle(recording_vehicle_id);
    }
    else {
        int player_id = get_root()->get_id();
        api_ptr->get_player_pos(player_id, recording_desc.point_end.x, recording_desc.point_end.y, recording_desc.point_end.z);
        recording_desc.point_end.rz = api_ptr->get_player_facing_angle(player_id);
    }
    recording_desc.duration = (recording_time_stop - recording_time_start);

    { // Сохраняем файл с методанными
        std::ofstream meta_file(recording_get_meta_filename(recording_name).c_str());
        if (meta_file) {
            meta_file << recording_desc;
        }
    }

    messages_item msg_item;
    msg_item.get_params()
        ("player_name", get_root()->name_get_full())
        ("recording_name", recording_name)
        ("recording_desc", boost::format("%1%") % recording_desc)
        ;

    msg_item.get_sender()
        ("$(npc_dev_recordind_stop_player)", msg_player(get_root()))
        ("$(npc_dev_recordind_stop_log)", msg_log);
}

std::string npc_dev_player_item::get_next_recording_filename() const {
    /*
    if (!boost::filesystem::exists(recording_get_rec_filename(recording_prefix))) {
        // В начале пытаемся имя без цифрового суфикса
        return recording_prefix;
    }
    */
    for (int i = 0; i < 1000; ++i) {
        std::string name = (boost::format("%1%%2$02d") % recording_prefix % i).str();
        if (!boost::filesystem::exists(recording_get_rec_filename(name))) {
            return name;
        }
    }
    assert(false);
    return "fuck";
}

std::string npc_dev_player_item::recording_get_meta_filename(std::string const& recording_name) {
    return "scriptfiles/" + recording_name + ".meta";
}
std::string npc_dev_player_item::recording_get_rec_filename(std::string const& recording_name) {
    return "scriptfiles/" + recording_name + ".rec";
}

float npc_dev_player_item::accelerate(float val) const {
    return val * manager.acceleration_multiplier;
}

float npc_dev_player_item::decelerate(float val) const {
    return val * manager.deceleration_multiplier;
}

npc_dev_npc_item::npc_dev_npc_item(npc_dev& manager)
:manager(manager)
{

}

npc_dev_npc_item::~npc_dev_npc_item() {

}

void npc_dev_npc_item::on_connect() {
    get_root()->set_spawn_info(11, pos4(1958.3783f, 1343.1572f, 15.3746f, 0, 0, 90.0f));
    get_root()->set_color(0xff000020);
}

void npc_dev_npc_item::on_spawn() {
    if (manager.play_record_vehicle_id) {
        // Сажаем в транспорт
        get_root()->put_to_vehicle(manager.play_record_vehicle_id);
    }
    else {
        // Включаем запись просто
        get_root()->playback_start(npc::playback_type_onfoot, manager.play_record_name);
    }
}

void npc_dev_npc_item::on_enter_vehicle(int vehicle_id) {
    if (manager.play_record_vehicle_id) {
        get_root()->playback_start(npc::playback_type_driver, manager.play_record_name);
    }
}

void npc_dev_npc_item::on_playback_end() {
    get_root()->kick();
}

void npc_dev_npc_item::on_disconnect(samp::player_disconnect_reason reason) {

}
