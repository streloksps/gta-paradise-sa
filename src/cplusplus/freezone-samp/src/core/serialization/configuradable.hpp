#ifndef CONFIGURADABLE_HPP
#define CONFIGURADABLE_HPP
#include "serialization.hpp"
#include <memory>

// Интерфейс объекта, который может быть сконфигурирован
class configuradable: public serialization::auto_name {
public:
    typedef std::tr1::shared_ptr<configuradable> ptr;
    typedef serialization::def_t<configuradable> def_t;

    virtual void configure_pre() = 0;
    virtual void configure(buffer::ptr const& buff, def_t const& def) = 0;
    virtual void configure_post() = 0;
};
#endif // CONFIGURADABLE_HPP
