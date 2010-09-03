#ifndef CHAT_MSG_ITEM_HPP
#define CHAT_MSG_ITEM_HPP
#include <string>
#include <vector>

struct chat_msg_item_t {
    unsigned int color;
    std::string msg;
    bool is_clear;
    
    // Сообщение
    chat_msg_item_t(unsigned int color, std::string const& msg);
    // Очистка чата
    chat_msg_item_t();

    ~chat_msg_item_t();
};

typedef std::vector<chat_msg_item_t> chat_msg_items_t;

struct chat_bubble_t {
    unsigned color;
    float draw_distance;
    int expire_time;
    std::string text;
    chat_bubble_t();
    chat_bubble_t(int expire_time);
};

chat_msg_items_t create_chat_items(std::string const& msg);

chat_bubble_t create_chat_bubble(std::string const& text);
#endif // CHAT_MSG_ITEM_HPP
