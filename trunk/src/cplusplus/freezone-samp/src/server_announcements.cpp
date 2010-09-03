#include "config.hpp"
#include "server_announcements.hpp"
#include "core/module_c.hpp"

REGISTER_IN_APPLICATION(server_announcements);

server_announcements::server_announcements(): last_message_show(0), delay(0) {
}

server_announcements::~server_announcements() {
}

void server_announcements::configure_pre() {
    delay = 900;
    messages.clear();
}

void server_announcements::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(delay);
    SERIALIZE_ITEM(messages);
}

void server_announcements::configure_post() {
    if (delay <= 0) {
        delay = 900;
    }
}

void server_announcements::on_timer5000() {
    if (std::time(0) - last_message_show > delay) {
        last_message_show = std::time(0);
        if (!messages.empty()) {
            messages_item msg_item;
            msg_item.get_sender()(messages[std::rand() % messages.size()], msg_players_all);
        }
    }
}
