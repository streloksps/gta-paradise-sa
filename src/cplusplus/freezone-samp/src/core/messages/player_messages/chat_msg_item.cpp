#include "config.hpp"

#include "chat_msg_item.hpp"
#include "tags.hpp"

namespace {
    enum {chat_line_max = 115};
    enum {chat_line_min = 90};
    enum {chat_line_preffex_max = 64};

    std::string get_preffix(tags& tag, size_t pos) {
        std::string preffix = tag.get_closed_tag_val<std::string>("preffix", pos, "");
        if (preffix.length() > chat_line_preffex_max) {
            // Обрезаем слишком длинный префикс
            preffix = preffix.substr(0, chat_line_preffex_max);
        }
        return preffix;
    }

    void add_chat_item(chat_msg_items_t& items, unsigned int msg_color, std::string const& msg_text, size_t tag_pos, tags& tag) {
        if ("none" != tag.get_closed_tag_val<std::string>("nonewline", tag_pos, "none")) {
            // Не разбиваем на несколько строчек
            std::string preffix = get_preffix(tag, tag_pos);
            items.push_back(chat_msg_item_t(msg_color, preffix + msg_text));
        }
        else {
            std::size_t msg_len = msg_text.length();
            std::size_t pos_start = 0;
            // Разбиваем на несколько строк
            for (;;) {
                std::string preffix = get_preffix(tag, tag_pos + pos_start);
                if (msg_len + preffix.length() - pos_start < chat_line_max) {
                    // Оставшияся строка влезает на 1 строчку
                    if (msg_text.length() > pos_start) {
                        // Не добавляем пустых строк
                        items.push_back(chat_msg_item_t(msg_color, preffix + msg_text.substr(pos_start)));
                    }
                    break;
                }
                else {
                    // Строку необходимо разбивать
                    std::size_t space_pos = msg_text.rfind(' ', chat_line_max - preffix.length() + pos_start);
                    if (std::string::npos == space_pos || space_pos < chat_line_min - preffix.length() + pos_start) {
                        // Режем строчку жестко
                        items.push_back(chat_msg_item_t(msg_color, preffix + msg_text.substr(pos_start, chat_line_max - preffix.length())));
                        pos_start += chat_line_max - preffix.length();
                    }
                    else {
                        // Режем по пробелу
                        items.push_back(chat_msg_item_t(msg_color, preffix + msg_text.substr(pos_start, space_pos - pos_start)));
                        pos_start += space_pos - pos_start + 1;
                    }
                }
            }
        }
    }
}

chat_msg_item_t::chat_msg_item_t(unsigned int color, std::string const& msg): color(color), msg(msg), is_clear(false) {
    
}

chat_msg_item_t::chat_msg_item_t(): color(0xffffff), is_clear(true) {

}

chat_msg_item_t::~chat_msg_item_t() {

}

chat_msg_items_t create_chat_items(std::string const& msg) {
    chat_msg_items_t rezult;
    tags message(msg);
    if ("none" != message.get_closed_tag_val<std::string>("clear", 0, "none")) {
        rezult.push_back(chat_msg_item_t());
    }
    std::string msg_str = message.get_untaget_str();
    size_t pos_start = 0;
    size_t pos_curr;
    for (;;) {
        pos_curr = msg_str.find("\\n", pos_start);
        unsigned int msg_color = message.get_closed_tag_val<unsigned int>("color", pos_start, 0xFFFFFF);
        if (std::string::npos == pos_curr) {
            add_chat_item(rezult, msg_color, msg_str.substr(pos_start), pos_start, message);
            break;
        }
        else {
            add_chat_item(rezult, msg_color, msg_str.substr(pos_start, pos_curr - pos_start), pos_start, message);
            pos_start = pos_curr + 2;
        }
    }
    return rezult;
}

chat_bubble_t::chat_bubble_t()
:color(0xFFFFFFFF)
,draw_distance(0.0f)
,expire_time(1000)
{
}

chat_bubble_t::chat_bubble_t(int expire_time)
:color(0xFFFFFFFF)
,draw_distance(0.0f)
,expire_time(expire_time)
{
}


chat_bubble_t create_chat_bubble(std::string const& text) {
    chat_bubble_t rezult;
    tags tags_text(text);
    rezult.color = tags_text.get_closed_tag_val<unsigned int>("color", 0, 0xFFFFFF);
    rezult.draw_distance = tags_text.get_closed_tag_val<float>("distance", 0, 0.0f);
    rezult.expire_time = tags_text.get_closed_tag_val<unsigned int>("time", 0, 0);
    rezult.text = tags_text.get_untaget_str();
    return rezult;
}
