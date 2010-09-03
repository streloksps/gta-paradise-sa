#ifndef SERVER_ANNOUNCEMENTS_HPP
#define SERVER_ANNOUNCEMENTS_HPP
#include "core/module_h.hpp"
#include <vector>
#include <string>
#include <ctime>

class server_announcements
    :public application_item
    ,public configuradable
    ,public on_timer5000_i
{
public:
    server_announcements();
    virtual ~server_announcements();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(server_announcements);

public:
    virtual void on_timer5000();

private:
    int delay; // Интервал, между сообщениями, секунды
    std::vector<std::string> messages; // Список сообщений
    std::time_t last_message_show;
};
#endif // SERVER_ANNOUNCEMENTS_HPP
