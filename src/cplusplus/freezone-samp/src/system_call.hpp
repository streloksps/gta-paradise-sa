#ifndef SYSTEM_CALL_HPP
#define SYSTEM_CALL_HPP
#include <string>
#include <vector>
#include <map>
#include <boost/noncopyable.hpp>
#include "core/module_h.hpp"


class system_call_t
    :boost::noncopyable
    ,public configuradable
{
public:
    system_call_t(std::string const& config_name);
    virtual ~system_call_t();

public:
    int call(messages_params const& params);
    operator bool() const;

private: // configuradable
    virtual void configure_pre();
    virtual void configure_post();
public: // configuradable
    virtual void configure(buffer::ptr const& buff, def_t const& def);
public: // auto_name
    virtual char const* get_auto_name() const;

private:
    std::string config_name;

    bool enable;
    std::string enable_if_file_exist;
    std::string command;
};
#endif // SYSTEM_CALL_HPP
