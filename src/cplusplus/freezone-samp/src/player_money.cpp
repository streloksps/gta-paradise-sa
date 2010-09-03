#include "config.hpp"
#include "player_money.hpp"
#include <algorithm>
#include <boost/format.hpp>
#include "core/module_c.hpp"
#include "player_spectate.hpp"

REGISTER_IN_APPLICATION(player_money);

player_money::player_money() {

}

player_money::~player_money() {

}

void player_money::create() {
    command_add("money_send", std::tr1::bind(&player_money::cmd_money_send, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3, std::tr1::placeholders::_4, std::tr1::placeholders::_5), cmd_game_can_flood);
    api_ptr = samp::api::instance();
}

void player_money::configure_pre() {
    max_player_money = 1000000;
    send_money_min = 1;
    send_money_max = 100000;
    send_distance_max = 50.0f;
    is_block_on_maxmoney = false;
}

void player_money::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(max_player_money);
    SERIALIZE_ITEM(send_money_min);
    SERIALIZE_ITEM(send_money_max);
    SERIALIZE_ITEM(send_distance_max);
    SERIALIZE_ITEM(is_block_on_maxmoney);
}

void player_money::configure_post() {
    if (max_player_money < 100000) {
        max_player_money = 100000;
    }
    if (1 > send_money_min) {
        send_money_min = 1;
    }
    if (send_money_max < send_money_min) {
        send_money_max = send_money_min;
    }
    if (send_distance_max < 5.0f) {
        send_distance_max = 5.0f;
    }
}

void player_money::on_gamemode_init(AMX*, samp::server_ver ver) {
    samp::api::instance()->enable_stunt_bonus_for_all(false);
}

void player_money::on_connect(player_ptr_t const& player_ptr) {
    player_money_item::ptr item(new player_money_item(*this), &player_item::do_delete);
    player_ptr->add_item(item);
    samp::api::instance()->reset_player_money(player_ptr->get_id());
}

command_arguments_rezult player_money::cmd_money_send(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params) {
    int receive_player_id;
    int money;
    { // Парсинг
        std::istringstream iss(arguments);
        iss>>receive_player_id>>money;
        if (iss.fail() || !iss.eof()) {
            return cmd_arg_syntax_error;
        }
    }

    player_ptr_t receive_player_ptr = players::instance()->get_player(receive_player_id);
    if (!receive_player_ptr) {
        params("player_id", receive_player_id);
        pager("$(player_bad_id)");
        return cmd_arg_process_error;
    }

    if (receive_player_ptr == player_ptr) {
        pager("$(player_not_same)");
        return cmd_arg_process_error;
    }

    params
        ("money_min", send_money_min)
        ("money_max", send_money_max)
        ("player_name", player_ptr->name_get_full())
        ("receive_player_name", receive_player_ptr->name_get_full())
        ("money", money)
        ;

    if (player_in_game != receive_player_ptr->get_state()) {
        params("player_name", receive_player_ptr->name_get_full());
        pager("$(player_not_in_game_player)");
        return cmd_arg_process_error;
    }

    if (send_money_min > money || send_money_max < money) {
        pager("$(cmd_money_send_error_ban_money)");
        return cmd_arg_process_error;
    }

    if (!player_ptr->get_item<player_money_item>()->can_take(money)) {
        params("money", money);
        pager("$(money_error_low_money)");
        return cmd_arg_process_error;
    }

    if (!receive_player_ptr->get_item<player_money_item>()->can_give(money)) {
        params("player_name", receive_player_ptr->name_get_full());
        pager("$(money_error_many_money_player)");
        return cmd_arg_process_error;
    }

    if (!is_points_in_sphere(player_ptr->get_item<player_position_item>()->pos_get(), receive_player_ptr->get_item<player_position_item>()->pos_get(), send_distance_max)
        || receive_player_ptr->get_item<player_spectate_item>()->is_spectate()) {
        pager("$(cmd_money_send_error_to_far)");
        return cmd_arg_process_error;
    }

    if (player_ptr->get_item<player_spectate_item>()->is_spectate()) {
        pager("$(cmd_money_send_error_spectate)");
        return cmd_arg_process_error;
    }

    player_ptr->get_item<player_money_item>()->take(money);
    receive_player_ptr->get_item<player_money_item>()->give(money);

    pager("$(cmd_money_send_done)");
    sender("$(cmd_money_send_done_receive)", msg_player(receive_player_ptr));
    sender("$(cmd_money_send_done_log)", msg_log);

    return cmd_arg_ok;
}

player_money_item::player_money_item(player_money& manager)
:manager(manager)
,server_money(0)
,server_money_max(0)
{
}

player_money_item::~player_money_item() {
}

int player_money_item::get() const {
    return std::min(server_money, samp::api::instance()->get_player_money(get_root()->get_id()));
}

int player_money_item::get_server() const {
    return server_money;
}

void player_money_item::take(int money) {
    if (money < 0) money = 0;
    int test_money = server_money - money;
    if (test_money < 0) test_money = 0;
    if (test_money > manager.max_player_money) test_money = manager.max_player_money;
    int delta = test_money - server_money;
    samp::api::instance()->give_player_money(get_root()->get_id(), delta);
    server_money = test_money;
}

void player_money_item::give(int money) {
    if (money < 0) money = 0;
    int test_money = server_money + money;
    if (test_money < 0) test_money = 0;
    if (test_money > manager.max_player_money) test_money = manager.max_player_money;
    int delta = test_money - server_money;
    samp::api::instance()->give_player_money(get_root()->get_id(), delta);
    server_money = test_money;
    if (server_money > server_money_max) {
        server_money_max = server_money;
    }
}

void player_money_item::reset() {
    samp::api::instance()->reset_player_money(get_root()->get_id());
    server_money = 0;
}

bool player_money_item::can_take(int money) const {
    if (money < 0) return false;
    if (money > get()) return false;
    return true;
}

bool player_money_item::can_give(int money) const {
    if (money < 0) return false;
    int test_money = server_money + money;
    if (test_money < 0) return false;
    if (test_money > manager.max_player_money) return false;
    return true;
}

void player_money_item::on_death_post(player_ptr_t const& killer_ptr, int reason) {
    reset();
    server_money_max = 0;
}

bool player_money_item::on_update() {
    if (manager.is_block_on_maxmoney) {
        if (player_in_game == get_root()->get_state()
            && (!get_root()->get_item<player_spectate_item>()->is_spectate() && !get_root()->get_item<player_spectate_item>()->is_spectate_ending())
            ) {
            int player_money = manager.api_ptr->get_player_money(get_root()->get_id());
            if (player_money > server_money_max) {
                get_root()->block("money/max", (boost::format("%1% %2%") % server_money_max % player_money).str());
                return false;
            }
        }
    }
    return true;
}
