#include "config.hpp"
#include "timers.hpp"
#include <functional>
#include <boost/foreach.hpp>
#include "core/application.hpp"
#include "core/players.hpp"
#include "core/player/player.hpp"
#include "core/container/container_handlers.hpp"
#include "core/players_handlers.hpp"
#include "core/npcs_handlers.hpp"
#include "timers_i.hpp"

REGISTER_IN_APPLICATION(timers);

timers::ptr timers::instance() {
    return application::instance()->get_item<timers>();
}

timers::timers() {
}

timers::~timers() {
}

void timers::create() {
    boost::posix_time::ptime time_curr(boost::posix_time::microsec_clock::universal_time());

    time_last_250 = time_curr;
    time_last_500 = time_curr;
    time_last_1000 = time_curr;
    time_last_5000 = time_curr;
    time_last_15000 = time_curr;
    time_last_60000 = time_curr;
}

template <typename timer_i>
void timer_process_timer(boost::posix_time::ptime const& curr_time, boost::posix_time::ptime& last_time, unsigned int delta, void (timer_i::*event_timer_p)()) {
    boost::posix_time::time_duration diff = curr_time - last_time;
    if (diff.total_milliseconds() > delta) {
        { // Приложение
            std::vector<std::tr1::shared_ptr<timer_i> > is_ptr = application::instance()->get_items<timer_i>();
            std::for_each(is_ptr.begin(), is_ptr.end(), std::tr1::bind(event_timer_p, std::tr1::placeholders::_1));
        }
        
        // Игроки
        players_execute_handlers(event_timer_p);

        // Боты
        npcs_execute_handlers(event_timer_p);

        last_time = curr_time;
    }
}

void timers::plugin_on_tick() {
    boost::posix_time::ptime time_curr(boost::posix_time::microsec_clock::universal_time());

    timer_process_timer(time_curr, time_last_250,   250,    &on_timer250_i::on_timer250);
    timer_process_timer(time_curr, time_last_500,   500,    &on_timer500_i::on_timer500);
    timer_process_timer(time_curr, time_last_1000,  1000,   &on_timer1000_i::on_timer1000);
    timer_process_timer(time_curr, time_last_5000,  5000,   &on_timer5000_i::on_timer5000);
    timer_process_timer(time_curr, time_last_15000, 15000,  &on_timer15000_i::on_timer15000);
    timer_process_timer(time_curr, time_last_60000, 60000,  &on_timer60000_i::on_timer60000);
}
