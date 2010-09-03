#include "config.hpp"
#include "player_color.hpp"
#include <functional>
#include <cstdlib>
#include <list>
#include <sstream>
#include "core/module_c.hpp"

REGISTER_IN_APPLICATION(player_color);

player_color::ptr player_color::instance() {
    return application::instance()->get_item<player_color>();
}

player_color::player_color()
:last_is_stream(false)
{
}

player_color::~player_color() {

}

void player_color::create() {
    last_is_stream = false;
    samp::api::instance()->show_player_markers(samp::api::PLAYER_MARKERS_MODE_GLOBAL);

    command_add("player_color", bind(&player_color::cmd_color, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5));
    command_add("player_color_list", bind(&player_color::cmd_color_list, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
}

void player_color::configure_pre() {
    colors.clear();
    max_players_for_stream_player_markers = 200;
}

void player_color::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(colors);
    SERIALIZE_ITEM(max_players_for_stream_player_markers);
}

void player_color::configure_post() {
    if (max_players_for_stream_player_markers < 0) {
        max_players_for_stream_player_markers = 0;
    }
    update_stream_player_markers();
}

void player_color::on_connect(player_ptr_t const& player_ptr) {
    player_color_item::ptr item(new player_color_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
    update_stream_player_markers();
}

void player_color::on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason) {
    update_stream_player_markers(-1);
}

command_arguments_rezult player_color::cmd_color(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int index = -1;
    {
        std::istringstream iss(arguments);
        iss>>index;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    if (0 > index || (int)colors.size() <= index) {
        params("max_id", colors.size() - 1);
        pager("$(cmd_player_color_error_id)");
        return cmd_arg_process_error;
    }

    player_color_t& color = colors[index];
    player_color_item::ptr player_color_item_ptr = player_ptr->get_item<player_color_item>();

    player_color_item_ptr->set_color_index(index);
    player_color_item_ptr->set_color(color.color);

    params
        ("id", index)
        ("color", color.color)
        ("color_hex", boost::format("%1$06X") % color.color)
        ("name", color.name)
        ;

    pager("$(cmd_player_color_done)");
    sender("$(cmd_player_color_done_log)", msg_log);

    return cmd_arg_ok;
}

command_arguments_rezult player_color::cmd_color_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    pager.set_header_text(0, "$(cmd_player_color_list_header)");
    for (std::size_t i = 0, size = colors.size(); i < size; ++i) {
        params
            ("id", i)
            ("color", colors[i].color)
            ("name", colors[i].name);
        pager("$(cmd_player_color_list_item)");
    }
    return cmd_arg_auto;
}

void player_color::update_stream_player_markers(int count_delta) {
    // Начиная с 0.3a можно включить стрим маркеров игроков
    int curr_players_count = players::instance()->get_players_count();
    bool curr_is_stream = curr_players_count + count_delta >= max_players_for_stream_player_markers;

    if (curr_is_stream != last_is_stream) {
        if (curr_is_stream) {
            samp::api::instance()->show_player_markers(samp::api::PLAYER_MARKERS_MODE_STREAMED);
        }
        else {
            samp::api::instance()->show_player_markers(samp::api::PLAYER_MARKERS_MODE_GLOBAL);
        }
        last_is_stream = curr_is_stream;
    }
}

player_color_item::player_color_item(player_color& manager)
:color(0xFFFFFF)
,color_index(-1)
//,color_is_custom(false)
,manager(manager)
,is_first_class(true)
{
}

player_color_item::~player_color_item() {
}

void player_color_item::set_color(int new_color) {
    color = new_color;
    show_marker();
}

void player_color_item::on_interior_change(int interior_id_new, int interior_id_old) {
    if (0 == interior_id_new && 0 != interior_id_old) {
        show_marker();
    }
    else if (0 != interior_id_new && 0 == interior_id_old) {
        hide_marker();
    }
}

void player_color_item::show_marker() {
    unsigned int samp_color = ((unsigned int)color) << 8;
    samp_color |= 0xFF;
    get_root()->set_color(samp_color);
}

void player_color_item::hide_marker() {
    unsigned int samp_color = ((unsigned int)color) << 8;
    get_root()->set_color(samp_color);
}

void player_color_item::on_spawn() {
    show_marker();
}

void player_color_item::on_request_class(int class_id) {
    if (is_first_class) {
        is_first_class = false;
        init_color();
        show_marker();
    }
}

void player_color_item::init_color() {
    if (manager.colors.empty()) {
        return;
    }
    int index = std::rand() % manager.colors.size();
    player_color_t& color = manager.colors[index];

    set_color_index(index);
    set_color(color.color);

    messages_item msg_item;

    msg_item.get_params()
        ("player_name", get_root()->name_get_full())
        ("id", index)
        ("color", color.color)
        ("color_hex", boost::format("%1$06X") % color.color)
        ("name", color.name)
        ;
    msg_item.get_sender()("$(player_color_init_log)", msg_log);
}

unsigned int player_color_item::get_color() const {
    unsigned int samp_color = ((unsigned int)color) << 8;
    samp_color |= 0xFF;
    return samp_color;
}

void player_color_item::set_color_index(std::size_t color_index) {
    this->color_index = color_index;
}

std::size_t player_color_item::get_color_index() const {
    return color_index;
}
