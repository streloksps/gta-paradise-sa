#ifndef SERVER_PROPERTIES_HPP
#define SERVER_PROPERTIES_HPP
#include <string>
#include <memory>
#include "core/container/application_item.hpp"
#include "core/createble_i.hpp"
#include "core/serialization/configuradable.hpp"
#include "core/samp/samp_events_i.hpp"
#include "core/timers_i.hpp"

class server_properties
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public samp::on_gamemode_init_i
    ,public on_timer60000_i
{
public:
    typedef std::tr1::shared_ptr<server_properties> ptr;
    static ptr instance();

private:
    std::string name;   // Имя сервера
    std::string site;   // Сайт сервера
    std::string mode;   // Имя мода сервера
    std::string map;    // Имя карты сервера
    long crc;           // Контрольная сумма вышеперечисленных параметров - пока не используется

    std::string address;    // Адрес сервера
    std::string name_suffix;
    std::string mode_suffix;
public:
    server_properties();
    virtual ~server_properties();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(server_properties);

public: // on_gamemode_init_i
    virtual void on_gamemode_init(AMX* amx, samp::server_ver ver);

public:
    virtual void on_timer60000();

public:
    std::string get_name() const;
    std::string get_name_suffix() const;
    std::string get_site() const;
    std::string get_site_full() const;
    std::string get_mode() const;
    std::string get_mode_name() const;
    std::string get_map() const;
    int         get_port() const;
    std::string get_address() const;
    bool is_valid() const; // Возращает истину, если данные коректны - для внутреней самопроверки

private:
    bool is_mode_loaded;
    void refresh_server();
};
#endif // SERVER_PROPERTIES_HPP
