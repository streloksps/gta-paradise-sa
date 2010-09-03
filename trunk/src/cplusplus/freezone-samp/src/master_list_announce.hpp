#ifndef MASTER_LIST_ANNOUNCE_HPP
#define MASTER_LIST_ANNOUNCE_HPP
#include "core/module_h.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

class master_list_announce
    :public application_item
    ,public configuradable
    ,public on_timer5000_i
{
public:
    typedef std::tr1::shared_ptr<master_list_announce> ptr;
    static ptr instance();

public:
    master_list_announce();
    virtual ~master_list_announce();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(master_list_announce);

public: // timer15000_i
    virtual void on_timer5000();

private: // Настройки
    bool        is_active;
    bool        is_trace;
    int         interval;           // Интервал между срабатываниями
    std::string http_bind_ip;
    std::string http_host;
    int         http_host_port;
    std::string http_url;
    std::string http_user_agent;
    std::string http_referer;

private: // Состояние
    boost::posix_time::ptime time_last;
private:
    void do_announce() const;
    struct http_request_t {
        boost::optional<std::string>    bind_ip; // локальный ip адрес, к котором биндим сокет для отсыла сообщения. Если не задан, то используется первый попавшийся
        std::string                     host;
        int                             host_port;
        std::string                     url;
        std::string                     user_agent;
        std::string                     referer;
    };
    static void do_announce_impl(http_request_t const& http_request, bool is_trace);
};
#endif // MASTER_LIST_ANNOUNCE_HPP
