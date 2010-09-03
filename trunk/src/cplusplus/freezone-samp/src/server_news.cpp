#include "config.hpp"
#include "server_news.hpp"
#include "core/application.hpp"
#include "core/command/command_adder.hpp"
#include "core/player/player.hpp"
#include <algorithm>
#include <iterator>
#include <functional>

REGISTER_IN_APPLICATION(server_news);

server_news::ptr server_news::instance() {
    return application::instance()->get_item<server_news>();
}

server_news::server_news() {
}

server_news::~server_news() {
}

void server_news::create() {
    command_add("news", std::tr1::bind(&server_news::cmd_news, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_info_only);
}

void server_news::configure_pre() {
    news.clear();
}

void server_news::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(news);
}

void server_news::configure_post() {

}

command_arguments_rezult server_news::cmd_news(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    pager.set_header_text(0, "$(cmd_news_header)");
    news_t local_news;
    std::remove_copy_if(news.rbegin(), news.rend(), std::back_inserter(local_news), std::tr1::bind(std::logical_not<bool>(), std::tr1::bind(&server_news::is_player_can_see, std::tr1::ref(player_ptr), std::tr1::placeholders::_1)));
    for (std::size_t i = 0, size = local_news.size(); i < size; ++i) {
        params("nn", size - i)("text", local_news[i].text);
        if (local_news[i].is_public) {
            pager("$(cmd_news_item_user)");
        }
        else {
            pager("$(cmd_news_item_admin)");
        }
    }
    return cmd_arg_auto;
}

bool server_news::is_player_can_see(player_ptr_t const& player_ptr, server_news_item_t const& item) {
    if (item.is_public) {
        return true;
    }
    return player_ptr->group_is_in("admin");
}
