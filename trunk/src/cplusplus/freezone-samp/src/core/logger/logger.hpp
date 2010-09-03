#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <memory>

#include "core/serialization/configuradable.hpp"
#include "core/createble_i.hpp"
#include "core/container/application_item.hpp"

#include "logger_writer.hpp"

class logger
    :public application_item
    ,public configuradable
    ,public createble_i
{
public:
    typedef std::tr1::shared_ptr<logger> ptr;
    static ptr instance();

public:
    logger();
    virtual ~logger();

public:
    //! Печатает сообщение в лог
    void log(std::string const& section, std::string const& text);
    //! Печатает сообщение в дебаг лог
    void debug(std::string const& section, std::string const& text);
    //! Закрывает файл лога - может быть полезно, при удалении лога
    void close();

private:
    bool is_duplicate_logs_in_server_log;
    bool is_duplicate_debugs_in_server_log;
    log_writer log_item;
    log_writer debug_item;

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(logger);
};
#endif // LOGGER_HPP
