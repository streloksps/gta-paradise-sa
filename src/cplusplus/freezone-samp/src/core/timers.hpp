#ifndef TIMERS_HPP
#define TIMERS_HPP
#include "core/container/application_item.hpp"
#include "core/createble_i.hpp"
#include "core/samp/pawn/pawn_plugin_i.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

class timers 
    : public application_item
    , public createble_i
    , public pawn::plugin_on_tick_i
{
public:
    typedef std::tr1::shared_ptr<timers> ptr;
    static ptr instance();

    timers();
    virtual ~timers();
public: // createble_i
    virtual void create();

public: // plugin_on_tick_i
    virtual void plugin_on_tick();

private:
    boost::posix_time::ptime time_last_250;
    boost::posix_time::ptime time_last_500;
    boost::posix_time::ptime time_last_1000;
    boost::posix_time::ptime time_last_5000;
    boost::posix_time::ptime time_last_15000;
    boost::posix_time::ptime time_last_60000;
};
#endif // TIMERS_HPP
