#include "config.hpp"
#include "messages_pager.hpp"
#include <algorithm>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>

#include "core/buffer/buffer.hpp"
#include "messages_sender.hpp"
#include "messages_params.hpp"
#include "core/player/player.hpp"
#include "core/messages/player_messages/player_messages.hpp"

messages_pager::messages_pager(player_ptr_t const& player_ptr, buffer_ptr_c_t const& buffer_ptr_c)
:page(-1)
,player_ptr(player_ptr)
,buffer_ptr_c(buffer_ptr_c)
,is_last_page_out_ok(false)
{
}

messages_pager::~messages_pager() {
    if (!msgs.empty()) {
        page_out();
    }
}

void messages_pager::page_out() {
    is_last_page_out_ok = false;

    if (msgs.empty()) {
        return;
    }

    int page_number = get_page_number();

    if (0 == page_number && is_can_page_single()) {
        // Выводим одностранично
        is_last_page_out_ok = true;
        curr_headers_t last_headers;
        BOOST_FOREACH(msg_t const& msg, msgs) {
            process_headers(last_headers, msg.headers, true);
            last_headers = msg.headers;
            player_messages::send_message(player_ptr, msg.msg);
        }
    }
    else {
        curr_headers_t  last_headers;
        int             last_page = 0;

        int curr_line = 0;
        int curr_page = 0;
        BOOST_FOREACH(msg_t const& msg, msgs) {
            curr_page = get_page_number_by_line(curr_line);
            if (last_page != curr_page) {
                // Началась новая страница
                last_headers.clear();
                last_page = curr_page;
            }
            
            {
                int curr_item_lines = process_headers(last_headers, msg.headers, false);
                if (curr_page != get_page_number_by_line(curr_line + curr_item_lines)) {
                    // Данное сообщение не влезет на эту страницу - переходим на следующую страницу
                    ++curr_page;
                    curr_line = curr_page * (max_lines_on_page - 1);
                    last_headers.clear();
                    last_page = curr_page;
                }
            }

            bool is_print = (curr_page == page_number);
            is_last_page_out_ok |= is_print;

            {
                int header_count = process_headers(last_headers, msg.headers, is_print);
                if (0 != header_count) {
                    last_headers = msg.headers;
                }
                curr_line += (header_count + 1);
            }
            if (is_print) {
                player_messages::send_message(player_ptr, msg.msg);
            }
        }
        if (is_last_page_out_ok) {
            print_footer_ok(page_number + 1, curr_page + 1);
        }
        else {
            print_footer_error();
        }
    }
    clear();
}

void messages_pager::print_footer_error() const {
    buffer::ptr local_buff = buffer_ptr_c->children_create_connected();
    messages_params params(local_buff);
    messages_sender sender(local_buff);
    params
        ("pager_page_curr", boost::format("%1%") % page)
        ("pager_page_next", "1")
        ;
    sender("$(pager_error)", msg_player(player_ptr));
}

void messages_pager::print_footer_ok(int page_curr, int page_total) const {
    int page_next = page_curr != page_total ? page_curr + 1 : 1;

    buffer::ptr local_buff = buffer_ptr_c->children_create_connected();
    messages_params params(local_buff);
    messages_sender sender(local_buff);

    params
        ("pager_page_curr", boost::format("%1%") % page_curr)
        ("pager_page_total", boost::format("%1%") % page_total)
        ("pager_page_next", boost::format("%1%") % page_next)
        ;
    sender("$(pager_ok)", msg_player(player_ptr));
}

void messages_pager::set_page_number(int page) {
    is_last_page_out_ok = false;
    this->page = page;
    page_out();
}

bool messages_pager::set_page_number(std::string const& page_string) {
    is_last_page_out_ok = false;
    if (page_string.empty()) {
        page = 0;
        page_out();
        return true;
    }
    std::istringstream iss(page_string);
    iss>>page;
    if (!iss.fail() && iss.eof()) {
        page_out();
        return true;
    }
    clear();
    page = -1;
    return false;
}

messages_pager& messages_pager::operator()(std::string const& value) {
    add_single_text(buffer_ptr_c->process_all_vars(value));
    return *this;
}

messages_pager::operator bool() const {
    return is_last_page_out_ok;
}

messages_pager& messages_pager::set_header_text(unsigned int level, std::string const& header_value) {
    if (header_value.empty()) {
        // Удаляем заголовок из буфера
        curr_headers.erase(level);
    }
    else {
        curr_headers[level] = buffer_ptr_c->process_all_vars(header_value);
    }
    return *this;
}

int messages_pager::process_headers(curr_headers_t const& header_old, curr_headers_t const& header_new, bool is_print) const {
    int rezult = 0;
    BOOST_FOREACH(curr_headers_t::value_type const& heades_pair, header_new) {
        curr_headers_t::const_iterator p = header_old.find(heades_pair.first);
        if (header_old.end() == p || p->second != heades_pair.second) {
            ++rezult;
            if (is_print) {
                player_messages::send_message(player_ptr, heades_pair.second);
            }
        }
    }
    return rezult;
}

bool messages_pager::is_can_page_single() const {
    int local_line = 0;
    curr_headers_t last_headers;
    BOOST_FOREACH(msg_t const& msg, msgs) {
        local_line += process_headers(last_headers, msg.headers, false);
        last_headers = msg.headers;
        ++local_line;
        if (local_line > max_lines_on_page) {
            return false;
        }
    }
    return true;
}

int messages_pager::get_page_number() const {
    int rezult = 0;
    if (-1 != page) {
        rezult = page - 1;
        if (rezult < 0) {
            rezult = 0;
        }
    }
    return rezult;
}

int messages_pager::get_page_number_by_line(int line) {
    return line / (max_lines_on_page - 1);
}

void messages_pager::clear() {
    msgs.clear();
    curr_headers.clear();
    //page = -1;
}

messages_pager& messages_pager::items_add(std::string const& value) {
    work_item += buffer_ptr_c->process_all_vars(value);
    return *this;
}

messages_pager& messages_pager::items_done() {
    if (!work_item.empty()) {
        add_single_text(work_item);
        work_item.clear();
    }
    return *this;
}

void messages_pager::add_single_text(std::string const& text) {
    chat_msg_items_t msg_items = create_chat_items(text);
    std::transform(msg_items.begin(), msg_items.end(), std::back_inserter(msgs), boost::lambda::bind(boost::lambda::constructor<msg_t>(), boost::cref(curr_headers), boost::lambda::_1));
}

int messages_pager::get_page() const {
    int rezult = 1;
    if (-1 != page && page > 1) {
        rezult = page;
    }
    return rezult;
}
