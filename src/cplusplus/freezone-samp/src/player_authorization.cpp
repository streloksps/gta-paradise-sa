#include "config.hpp"
#include "player_authorization.hpp"
#include "core/application.hpp"
#include "core/player/player.hpp"
#include "core/messages/messages_item.hpp"

REGISTER_IN_APPLICATION(player_authorization);

player_authorization::player_authorization() {
}

player_authorization::~player_authorization() {
}

void player_authorization::create() {
}

void player_authorization::on_connect(player_ptr_t const& player_ptr) {
    player_authorization_item::ptr item(new player_authorization_item(), &player_item::do_delete);
    player_ptr->add_item(item);
}

player_authorization_item::player_authorization_item(): is_first_request_class(true) {
}

player_authorization_item::~player_authorization_item() {
}

void player_authorization_item::on_request_class(int class_id) {
    if (is_first_request_class) {
        is_first_request_class = false;
        on_first_request_class();
    }
}

void player_authorization_item::on_connect() {
    messages_item msg_item;
    msg_item.get_sender()
        ("$(player_connect_begin_player)", msg_player(get_root()));
}

void player_authorization_item::on_first_request_class() {
    messages_item msg_item;
    msg_item.get_sender()
        ("$(player_connect_done_player)", msg_player(get_root()));
}
