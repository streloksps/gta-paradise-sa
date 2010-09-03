#ifndef MESSAGES_PAGER_HPP
#define MESSAGES_PAGER_HPP
#include <string>
#include <map>
#include <vector>
#include "core/player/player_fwd.hpp"
#include "core/buffer/buffer_fwd.hpp"
#include "player_messages/chat_msg_item.hpp"

class messages_pager {
public:
    messages_pager(player_ptr_t const& player_ptr, buffer_ptr_c_t const& buffer_ptr_c);
    ~messages_pager();

    // ������������ ���������. ���� �������� ������ ������ - �� ������� ��������� ��������������� ������
    messages_pager& set_header_text(unsigned int level, std::string const& header_value = "");

    // �������� ��������� ������, � �������� ������� ����������
    messages_pager& operator()(std::string const& value);


    //������ � ���������� �� 1 ������
    messages_pager& items_add(std::string const& value);
    // ����� ������������ ������
    messages_pager& items_done();

    // ����������� ����� ��������� ���������� ��� ������ ����� ������ ��� �� ����� �������� �������
    void set_page_number(int page);
    bool set_page_number(std::string const& page_string);

    // ��������� ������, ���� ������� �������� ���������
    operator bool () const;
    
    // ��������� ����� ��������, ���� ������� ���������
    int get_page() const;

private:
    enum {max_lines_on_page = 10};
    typedef std::map<unsigned int, std::string> curr_headers_t;
    struct msg_t {
        curr_headers_t      headers;
        chat_msg_item_t       msg;
        msg_t(curr_headers_t const& headers, chat_msg_item_t const& msg): headers(headers), msg(msg) {}
    };
    typedef std::vector<msg_t> msgs_t;

    player_ptr_t player_ptr;
    buffer_ptr_c_t buffer_ptr_c;

    curr_headers_t curr_headers;

    msgs_t msgs;

    std::string work_item;

    int page;
    bool is_last_page_out_ok;
    void page_out();
    void clear();

    int process_headers(curr_headers_t const& header_old, curr_headers_t const& header_new, bool is_print) const;

    // ��������� ������, ���� �������� ������� ������ 1 �������� - ��� ���������
    bool is_can_page_single() const;
    int get_page_number() const;
    void print_footer_error() const;
    void print_footer_ok(int page_curr, int page_total) const;

    static int get_page_number_by_line(int line);

    void add_single_text(std::string const& text);
};
#endif // MESSAGES_PAGER_HPP
