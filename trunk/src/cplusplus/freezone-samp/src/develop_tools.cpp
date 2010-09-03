#include "config.hpp"
#include "develop_tools.hpp"
#include "core/module_c.hpp"
#include "server_paths.hpp"
#include "vehicles.hpp"
#include <map>
#include <fstream>
#include "core/samp/pawn/pawn_log.hpp"

REGISTER_IN_APPLICATION(develop_tools);

develop_tools::develop_tools(): is_enable(false) {
}

develop_tools::~develop_tools() {
}

inline void test_speed() {
    // ������������ ������������������ ������� �������. � ����� ������� ������ �������� ������� ������������������ 40%, �� ��������� � ������
    {
        time_base::millisecond_t first = time_base::tick_count_milliseconds();
        for (int i = 0; i < 1000000; ++i) {
            samp::api::instance()->set_game_mode_text("Some text");
        }
        time_base::millisecond_t last = time_base::tick_count_milliseconds();
        pawn::logprintf("C++ 1 Time: %d ms", (int)(last - first));
    }
    {
        time_base::millisecond_t first = time_base::tick_count_milliseconds();
        samp::api::ptr api_ptr = samp::api::instance();
        for (int i = 0; i < 1000000; ++i) {
            api_ptr->set_game_mode_text("Some text");
        }
        time_base::millisecond_t last = time_base::tick_count_milliseconds();
        pawn::logprintf("C++ 2 Time: %d ms", (int)(last - first));
    }
    /*
    {
        time_base::millisecond_t first = time_base::tick_count_milliseconds();
        samp::api::ptr api_ptr = samp::api::instance();
        for (int i = 0; i < 1000000; ++i) {
            api_ptr->set_game_mode_text2("Some text");
        }
        time_base::millisecond_t last = time_base::tick_count_milliseconds();
        pawn::logprintf("C++ 3 Time: %d ms", (int)(last - first));
    }
    */
}

void develop_tools::create() {
    command_add("���_�������������������",      "$(cmd_section_admin_develop)", "/���_�������������������",     "[��_��������]",                "������/������������� ����������� ��������", "", std::tr1::bind(&develop_tools::cmd_special_action, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_����������",               "$(cmd_section_admin_develop)", "/���_����������",              "[x y z interior world rz]",    "������/������������� ���������� ������", "", std::tr1::bind(&develop_tools::cmd_coordinate, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_������",                   "$(cmd_section_admin_develop)", "/���_������",                  "�����",                        "���� ������", "", std::tr1::bind(&develop_tools::cmd_money, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_������������",             "$(cmd_section_admin_develop)", "/���_������������",            "��_���������",                 "������������� paintjob", "", std::tr1::bind(&develop_tools::cmd_paintjob, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_������",                   "$(cmd_section_admin_develop)", "/���_������",                  "����1 ����2",                  "������������� ���� ����������", "", std::tr1::bind(&develop_tools::cmd_carcolor, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_�������������",            "$(cmd_section_admin_develop)", "/���_�������������",           "��_�����������|��������",      "������������� ����� ��������� ������� �� ������� ����", "", std::tr1::bind(&develop_tools::cmd_mod, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_�������",                  "$(cmd_section_admin_develop)", "/���_�������",                 "[��_����������]",              "������� � ��������/��������� ���������", "", std::tr1::bind(&develop_tools::cmd_sit, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_�������",                  "$(cmd_section_admin_develop)", "/���_�������",                 "",                             "����� �� �������� ����������", "", std::tr1::bind(&develop_tools::cmd_unsit, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_������������",             "$(cmd_section_admin_develop)", "/���_������������",            "",                             "������� ���������, ��������� �� �����", "", std::tr1::bind(&develop_tools::cmd_destroy_notmod_vehicles, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_����������",               "$(cmd_section_admin_develop)", "/���_����������",              "��_������|��������",           "������� ��������� ��������� ������", "", std::tr1::bind(&develop_tools::cmd_vehicle_create, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_���������������������",    "$(cmd_section_admin_develop)", "/���_���������������������",   "��_�����",                     "��������� �� ���������� �� ������ ����� � ������� ����", "", std::tr1::bind(&develop_tools::cmd_getcomponent, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_�����",                    "$(cmd_section_admin_develop)", "/���_�����",                   "[����� �����]",                "������/������������� ����� ������", "", std::tr1::bind(&develop_tools::cmd_health, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_���",                      "$(cmd_section_admin_develop)", "/���_���",                     "[�����_���]",                  "������/������������� ��� ������", "", std::tr1::bind(&develop_tools::cmd_player_name, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_���_�����_��������_�����", "$(cmd_section_admin_develop)", "/���_���_�����_��������_�����","",                             "�������� ��� ��������� �������� ������� ��� ������", "", std::tr1::bind(&develop_tools::cmd_player_name_find_valid_chars, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_�����",                    "$(cmd_section_admin_develop)", "/���_�����",                   "�����",                        "������������� ��� ����� �� ������", "", std::tr1::bind(&develop_tools::cmd_set_chat_bubble, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_������",                   "$(cmd_section_admin_develop)", "/���_������",                  "��",                           "�������� ��������� � ������ � ���� ����", "", std::tr1::bind(&develop_tools::cmd_send_death_message, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_������������",             "$(cmd_section_admin_develop)", "/���_������������",            "[x y z interior world rz]",    "������/������������� ���������� ����������", "", std::tr1::bind(&develop_tools::cmd_vehicle_coordinate, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    command_add("���_����",                     "$(cmd_section_admin_develop)", "/���_����",                    "��",                           "������ ���� (���������)", "", std::tr1::bind(&develop_tools::cmd_skin, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game, std::tr1::bind(&develop_tools::access_checker, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    //test_speed();
}

void develop_tools::configure_pre() {
    is_enable = false;
    destroy_notmod_vehicles_impl();
}

void develop_tools::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(is_enable);
}

void develop_tools::configure_post() {

}

bool develop_tools::access_checker(player_ptr_t const& player_ptr, std::string const& cmd_name) {
    return is_enable;
}

command_arguments_rezult develop_tools::cmd_special_action(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (arguments.empty()) {
        // ������� ������� ��������
        params("id", player_ptr->special_action_get());
        pager("$(color_cmd_text)������� ����������� ��������: $(id)");
        sender("<log_section develop_tools/special_action/get/>$(cmd_player_name) $(id)", msg_debug);
        return cmd_arg_ok;
    }

    int id = -1;
    { // �������
        std::istringstream iss(arguments);
        iss>>id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    params("id", id);
    pager("$(color_cmd_ok)����� ����������� ��������: $(id)");
    sender("<log_section develop_tools/special_action/set/>$(cmd_player_name) $(id)", msg_debug);
    player_ptr->special_action_set(static_cast<samp::api::special_action>(id));
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_coordinate(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (arguments.empty()) {
        pos4 pos = player_ptr->get_item<player_position_item>()->pos_get();
        params("coord", boost::format("%1%") % pos);
        pager("$(color_cmd_text)������� ����������: $(coord)");
        sender("<log_section develop_tools/coordinate/get/>$(cmd_player_name) $(coord)", msg_debug);
        return cmd_arg_ok;
    }
    pos4 pos;
    { // �������
        std::istringstream iss(arguments);
        iss>>pos;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    params("coord", boost::format("%1%") % pos);
    pager("$(color_cmd_ok)����������� ����� ����������: $(coord)");
    sender("<log_section develop_tools/coordinate/set/>$(cmd_player_name) $(coord)", msg_debug);
    player_ptr->get_item<player_position_item>()->pos_set_ex(pos);
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_paintjob(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int paintjob_id;
    { // �������
        std::istringstream iss(arguments);
        iss>>paintjob_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    samp::api::ptr api_ptr = samp::api::instance();
    int vehicle_id = api_ptr->get_player_vehicle_id(player_ptr->get_id());
    int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);
    
    if (!model_id) {
        pager("$(color_cmd_error)�� �� � ����������");
        return cmd_arg_process_error;
    }

    params
        ("vehicle_id", vehicle_id)
        ("model_id", model_id)
        ("paintjob_id", paintjob_id)
        ;
    pager("$(color_cmd_ok)�������� ���������� � ����������:$(vehicle_id) ������:$(model_id) ��:$(paintjob_id)");
    sender("<log_section develop_tools/paintjob/>$(cmd_player_name) $(vehicle_id) $(model_id) $(paintjob_id)", msg_debug);

    api_ptr->change_vehicle_paintjob(vehicle_id, paintjob_id);

    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_carcolor(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int color1, color2;
    { // �������
        std::istringstream iss(arguments);
        iss>>color1>>color2;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    samp::api::ptr api_ptr = samp::api::instance();
    int vehicle_id = api_ptr->get_player_vehicle_id(player_ptr->get_id());
    int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);

    if (!model_id) {
        pager("$(color_cmd_error)�� �� � ����������");
        return cmd_arg_process_error;
    }

    params
        ("vehicle_id", vehicle_id)
        ("model_id", model_id)
        ("color1", color1)
        ("color2", color2)
        ;
    pager("$(color_cmd_ok)������� ���� � ����������:$(vehicle_id) ������:$(model_id) $(color1) $(color2)");
    sender("<log_section develop_tools/carcolor/>$(cmd_player_name) $(vehicle_id) $(model_id) $(color1) $(color2)", msg_debug);

    api_ptr->change_vehicle_color(vehicle_id, color1, color2);

    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_mod(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int component_id;
    { // �������
        if (!vehicles::instance()->get_component_id_by_name(arguments, component_id)) {
            std::istringstream iss(arguments);
            iss>>component_id;
            if (iss.fail() || !iss.eof()) {
                return cmd_arg_syntax_error;
            }
        }
    }

    samp::api::ptr api_ptr = samp::api::instance();
    int vehicle_id = api_ptr->get_player_vehicle_id(player_ptr->get_id());
    int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);

    if (!model_id) {
        pager("$(color_cmd_error)�� �� � ����������");
        return cmd_arg_process_error;
    }

    params
        ("vehicle_id", vehicle_id)
        ("model_id", model_id)
        ("component_id", component_id)
        ("component_name", vehicles::instance()->get_component_name_by_id(component_id))
        ;
    pager("$(color_cmd_ok)���������� ��������� ������ �� ���������:$(vehicle_id) ������:$(model_id) ��:$(component_id) $(component_name)");
    sender("<log_section develop_tools/mod/>$(cmd_player_name) $(vehicle_id) $(model_id) $(component_id) $(component_name)", msg_debug);

    api_ptr->add_vehicle_component(vehicle_id, component_id);

    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_money(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int money;
    { // �������
        std::istringstream iss(arguments);
        iss>>money;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }
    player_ptr->get_item<player_money_item>()->give(money);

    params("money", money);
    pager("$(color_cmd_ok)�� �������� $$(money)");
    sender("<log_section develop_tools/money/>$(cmd_player_name) $(money)", msg_debug);

    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_sit(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int vehicle_id;
    vehicles::ptr vehicles_ptr = vehicles::instance();
    if (!arguments.empty()) {
        std::istringstream iss(arguments);
        iss>>vehicle_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }
    else {
        // ���� ��������� ����
        typedef std::multimap<float, int> vmetrics_t;
        vmetrics_t vmetrics;
        pos4 player_pos = player_ptr->get_item<player_position_item>()->pos_get();
        for (int i = 1, max = vehicles_ptr->get_max_vehicle_id(); i <= max; ++i) {
            if (vehicles_ptr->is_valid_vehicle_id(i)) {
                vmetrics.insert(std::make_pair(get_points_metric_square(player_pos, vehicles_ptr->pos_get(i)), i));
            }
        }
        if (vmetrics.empty()) {
            pager("$(color_cmd_error)��������� �� ������");
            return cmd_arg_process_error;
        }
        vehicle_id = vmetrics.begin()->second;
    }

    if (!vehicles_ptr->is_valid_vehicle_id(vehicle_id)) {
        pager("$(color_cmd_error)�� �� � ����������");
        return cmd_arg_process_error;
    }

    int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);
    samp::api::instance()->put_player_in_vehicle(player_ptr->get_id(), vehicle_id, 0);
    
    params
        ("vehicle_id", vehicle_id)
        ("model_id", model_id);
    pager("$(color_cmd_ok)�� ���� � $(vehicle_id) ������: $(model_id)");
    sender("<log_section develop_tools/vehicles/sit/>$(cmd_player_name) $(vehicle_id) $(model_id)", msg_debug);
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_unsit(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (!arguments.empty()) {
        return cmd_arg_syntax_error;
    }

    samp::api::ptr api_ptr = samp::api::instance();

    int vehicle_id = api_ptr->get_player_vehicle_id(player_ptr->get_id());
    int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);

    if (!model_id) {
        pager("$(color_cmd_error)�� �� � ����������");
        return cmd_arg_process_error;
    }

    float x, y, z;
    api_ptr->get_vehicle_pos(vehicle_id, x, y, z);
    pos4 pos(x, y, z, api_ptr->get_player_interior(player_ptr->get_id()), api_ptr->get_vehicle_virtual_world(vehicle_id), api_ptr->get_vehicle_zangle(vehicle_id));
    
    pos.z += 2.0f;
    player_ptr->get_item<player_position_item>()->pos_set(pos);

    // �� �������� �� ������������
    //api_ptr->remove_player_from_vehicle(player_ptr->get_id());

    params
        ("vehicle_id", vehicle_id)
        ("model_id", model_id);
    pager("$(color_cmd_ok)�� ����� �� $(vehicle_id) ������: $(model_id)");
    sender("<log_section develop_tools/vehicles/unsit/>$(cmd_player_name) $(vehicle_id) $(model_id)", msg_debug);
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_destroy_notmod_vehicles(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int count = destroy_notmod_vehicles_impl();
    params("count", count);
    pager("$(color_cmd_ok)������� ����������: $(count)");
    sender("<log_section develop_tools/vehicles/destroy/all/>$(cmd_player_name) $(count)", msg_debug);
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_vehicle_create(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int model_id;
    { // �������
        if (!vehicles::instance()->get_model_id_by_name(arguments, model_id)) {
            std::istringstream iss(arguments);
            iss>>model_id;
            if (iss.fail() || !iss.eof()) {
                return cmd_arg_syntax_error;
            }
        }
    }

    pos4 pos = player_ptr->get_item<player_position_item>()->pos_get();

    { // ������� ������� ����
        int vehicle_id = samp::api::instance()->get_player_vehicle_id(player_ptr->get_id());
        int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);
        if (0 != model_id) {
            pos.rz = samp::api::instance()->get_vehicle_zangle(vehicle_id);
            if (!vehicles::instance()->is_vehicle_created_by_me(vehicle_id)) {
                samp::api::instance()->destroy_vehicle(vehicle_id);
            }
        }
    }

    int vehicle_id = samp::api::instance()->create_vehicle(model_id, pos.x, pos.y, pos.z + 1.5f, pos.rz, -1, -1, 3600);
    samp::api::instance()->set_vehicle_virtual_world(vehicle_id, pos.world);
    samp::api::instance()->link_vehicle_to_interior(vehicle_id, pos.interior);
    samp::api::instance()->put_player_in_vehicle(player_ptr->get_id(), vehicle_id, 0);

    params
        ("model_id", model_id)
        ("vehicle_id", vehicle_id)
        ;
    pager("$(color_cmd_ok)�� ������� $(vehicle_id) ������: $(model_id)");
    sender("<log_section develop_tools/vehicles/create/>$(cmd_player_name) $(vehicle_id) $(model_id)", msg_debug);

    return cmd_arg_ok;
}

int develop_tools::destroy_notmod_vehicles_impl() {
    int count = 0;
    vehicles::ptr vehicles_ptr = vehicles::instance();
    for (int i = 1, max = vehicles_ptr->get_max_vehicle_id(); i <= max; ++i) {
        if (vehicles_ptr->is_valid_vehicle_id(i)) {
            if (!vehicles_ptr->is_vehicle_created_by_me(i)) {
                samp::api::instance()->destroy_vehicle(i);
                ++count;
            }
        }
    }
    return count;
}

command_arguments_rezult develop_tools::cmd_getcomponent(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int slot_id;
    { // �������
        std::istringstream iss(arguments);
        iss>>slot_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    samp::api::ptr api_ptr = samp::api::instance();

    if (!api_ptr->is_has_030_features()) {
        // �������� ������ � 0.3
        return cmd_arg_process_error;
    }

    int vehicle_id = api_ptr->get_player_vehicle_id(player_ptr->get_id());
    int model_id = samp::api::instance()->get_vehicle_model(vehicle_id);

    if (!model_id) {
        pager("$(color_cmd_error)�� �� � ����������");
        return cmd_arg_process_error;
    }

    int component_id = api_ptr->get_vehicle_component_in_slot(vehicle_id, (samp::api::carmod_type)slot_id);
    params
        ("vehicle_id", vehicle_id)
        ("model_id", model_id)
        ("slot_id", slot_id)
        ("component_id", component_id)
        ("component_name", vehicles::instance()->get_component_name_by_id(component_id))
        ;
    pager("$(color_cmd_ok)�� ����������:$(vehicle_id) ������:$(model_id) � �����: $(slot_id) ��:$(component_id) $(component_name)");
    sender("<log_section develop_tools/getmod/>$(cmd_player_name) $(vehicle_id) $(model_id) $(slot_id) $(component_id) $(component_name)", msg_debug);

    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_health(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    player_health_armour curr;
    if (arguments.empty()) {
        curr = player_ptr->health_get();
        params
            ("health", boost::format("%1%") % curr.health)
            ("armour", boost::format("%1%") % curr.armour)
            ;
        pager("$(color_cmd_text)������� �����: $(health), �����: $(armour)");
        sender("<log_section develop_tools/health/get/>$(cmd_player_name) $(health) $(armour)", msg_debug);
        return cmd_arg_ok;
    }
    { // �������
        std::istringstream iss(arguments);
        iss>>curr.health>>curr.armour;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    params
        ("health", boost::format("%1%") % curr.health)
        ("armour", boost::format("%1%") % curr.armour)
        ;
    pager("$(color_cmd_ok)����������� ����� �����: $(health), �����: $(armour)");
    sender("<log_section develop_tools/health/set/>$(cmd_player_name) $(health) $(armour)", msg_debug);
    player_ptr->health_set(curr);
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_player_name(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    if (arguments.empty()) {
        std::string name = samp::api::instance()->get_player_name(player_ptr->get_id());
        params
            ("name", name)
            ;
        pager("$(color_cmd_text)������� ���: '$(name)'");
        sender("<log_section develop_tools/name/get/>$(cmd_player_name) '$(name)'", msg_debug);
        return cmd_arg_ok;
    }
    std::string new_name = arguments;
    int rezult = samp::api::instance()->set_player_name(player_ptr->get_id(), new_name);
    std::string name = samp::api::instance()->get_player_name(player_ptr->get_id());

    params
        ("new_name", new_name)
        ("name", name)
        ("rezult", rezult)
        ;
    if (new_name == name) {
        pager("$(color_cmd_ok)����������� ����� ���: '$(name)' ���: $(rezult)");
        sender("<log_section develop_tools/name/set/>$(cmd_player_name) '$(name)' $(rezult)", msg_debug);
    }
    else {
        pager("$(color_cmd_error)�� ������� ���������� ���: '$(new_name)' ���: $(rezult)");
        sender("<log_section develop_tools/name/error/>$(cmd_player_name) '$(new_name)' $(rezult)", msg_debug);
    }
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_player_name_find_valid_chars(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int min_nick_name = 3;
    std::string valid_chars;
    for (unsigned char ch = 1; ch != 0; ++ch) {
        std::string test_str(min_nick_name, static_cast<char>(ch));
        samp::api::instance()->set_player_name(player_ptr->get_id(), test_str);
        if (samp::api::instance()->get_player_name(player_ptr->get_id()) == test_str) {
            valid_chars += static_cast<char>(ch);
        }
    }
    params("valid_chars", valid_chars);
    pager("$(color_cmd_ok)�������� �������: '$(valid_chars)'");
    sender("<log_section develop_tools/player_name_find_valid_chars/>$(cmd_player_name) '$(valid_chars)'", msg_debug);
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_set_chat_bubble(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    samp::api::instance()->set_player_chat_bubble(player_ptr->get_id(), arguments, 0x0000FF00, 100.0f, 60000);
    params("text", arguments);
    sender("<log_section develop_tools/set_chat_bubble/>$(cmd_player_name) '$(text)'", msg_debug);
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_send_death_message(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int reason;
    { // �������
        std::istringstream iss(arguments);
        iss>>reason;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    params("reason", reason);
    pager("$(color_cmd_ok)������� ��������� � ������: $(reason)");
    sender("<log_section develop_tools/send_death_message/>$(cmd_player_name) $(reason)", msg_debug);
    player::send_death_message(player_ptr, reason);
    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_vehicle_coordinate(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    samp::api::ptr api_ptr = samp::api::instance();
    int vehicle_id = api_ptr->get_player_vehicle_id(player_ptr->get_id());
    int model_id = api_ptr->get_vehicle_model(vehicle_id);

    if (!model_id) {
        pager("$(color_cmd_error)�� �� � ����������");
        return cmd_arg_process_error;
    }

    params
        ("vehicle_id", vehicle_id)
        ("model_id", model_id)
        ;

    if (arguments.empty()) {
        float x, y, z;
        api_ptr->get_vehicle_pos(vehicle_id, x, y, z);
        pos4 pos(x, y, z, api_ptr->get_player_interior(player_ptr->get_id()), api_ptr->get_vehicle_virtual_world(vehicle_id), api_ptr->get_vehicle_zangle(vehicle_id));

        params("coord", boost::format("%1%") % pos);
        pager("$(color_cmd_text)������� ���������� ���������� $(vehicle_id) ������ $(model_id): $(coord)");
        sender("<log_section develop_tools/vcoordinate/get/>$(cmd_player_name) $(vehicle_id) $(model_id) $(coord)", msg_debug);
        { // ����� ��� � ������ �������
            std::ofstream file(PATH_DEV_LOG_FILENAME, std::ios_base::app);
            file << boost::format("# ���������� ���������� �������� %1% (%2%)") % player_ptr->name_get() % player_ptr->get_id() << std::endl;
            file << boost::format("vehicles=%1% %2$.4f %3$.4f %4$.4f %5% %6% %7$.4f -1 -1 600") % model_id % pos.x % pos.y % pos.z % pos.interior % pos.world % pos.rz << std::endl;
        }
        
        return cmd_arg_ok;
    }
    
    pos4 pos;
    { // �������
        std::istringstream iss(arguments);
        iss>>pos;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    params("coord", boost::format("%1%") % pos);
    pager("$(color_cmd_ok)����������� ����� ���������� ���������� $(vehicle_id) ������ $(model_id): $(coord)");
    sender("<log_section develop_tools/vcoordinate/set/>$(cmd_player_name) $(vehicle_id) $(model_id) $(coord)", msg_debug);
    
    api_ptr->set_vehicle_pos(vehicle_id, pos.x, pos.y, pos.z);
    api_ptr->set_vehicle_zangle(vehicle_id, pos.rz);
    api_ptr->link_vehicle_to_interior(vehicle_id, pos.interior);
    api_ptr->set_vehicle_virtual_world(vehicle_id, pos.world);
    api_ptr->set_player_interior(player_ptr->get_id(), pos.interior);
    api_ptr->set_player_virtual_world(player_ptr->get_id(), pos.world);

    return cmd_arg_ok;
}

command_arguments_rezult develop_tools::cmd_skin(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int skin_id;
    { // �������
        std::istringstream iss(arguments);
        iss>>skin_id;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    params("skin_id", skin_id);
    pager("$(color_cmd_ok)�� ������� ���� ��������� ��: $(skin_id)");
    sender("<log_section develop_tools/skin/set/>$(cmd_player_name) $(skin_id)", msg_debug);
    samp::api::instance()->set_player_skin(player_ptr->get_id(), skin_id);
    return cmd_arg_ok;
}
