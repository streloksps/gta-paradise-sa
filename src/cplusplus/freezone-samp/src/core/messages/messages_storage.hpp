#ifndef MESSAGES_STORAGE_HPP
#define MESSAGES_STORAGE_HPP

#include <memory>
#include "core/container/application_item.hpp"
#include "core/createble_i.hpp"
#include "core/buffer/buffer.hpp"
#include "core/serialization/configuradable.hpp"
#include "messages_sender.hpp"
#include "messages_params.hpp"

class server_configuration;
class messages_storage
    :public application_item
    ,public createble_i
    ,public configuradable
{
public:
    typedef std::tr1::shared_ptr<messages_storage> ptr;
    static ptr instance();

    messages_storage();
    ~messages_storage();

public:
    buffer::ptr_c get_msg_buff() const;

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(messages_storage);

private:
    buffer::ptr root_buff;
    buffer::ptr work_buff;

private:
    void init_server_properties();

private:
    friend class server_configuration;
    buffer::ptr const& get_root_buff() const;
};

#endif // MESSAGES_STORAGE_HPP
