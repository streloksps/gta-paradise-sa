#ifndef TIME_COUNT_BLOCKER_HPP
#define TIME_COUNT_BLOCKER_HPP
#include <istream>
#include <ostream>
#include <string>
#include "core/time_outs.hpp"

struct time_count_blocker_params {
    time_base::millisecond_t    time_interval;

    unsigned int                attemp_count;
    unsigned int                critical_attemp_count;

    time_base::millisecond_t    penalty;
    time_base::millisecond_t    critical_penalty;
    time_count_blocker_params(): time_interval(60000), attemp_count(1000), critical_attemp_count(1000), penalty(0), critical_penalty(0) {}
    inline void set(time_base::millisecond_t time_interval_new, unsigned int attemp_count_new, unsigned int critical_attemp_count_new, time_base::millisecond_t penalty_new, time_base::millisecond_t critical_penalty_new) {
        time_interval = time_interval_new;
        attemp_count = attemp_count_new;
        critical_attemp_count = critical_attemp_count_new;
        penalty = penalty_new;
        critical_penalty = critical_penalty_new;
    }
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, time_count_blocker_params& params) {
    return is>>params.time_interval>>params.attemp_count>>params.critical_attemp_count>>params.penalty>>params.critical_penalty;
}

template <class char_t, class traits_t>
inline std::basic_ostream<char_t, traits_t>& operator<<(std::basic_ostream<char_t, traits_t>& os, time_count_blocker_params const& params_t) {
    return os<<params_t.time_interval<<" "<<params_t.attemp_count<<" "<<params_t.critical_attemp_count<<" "<<params_t.penalty<<" "<<params_t.critical_penalty;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read <time_count_blocker_params>: std::tr1::true_type {};
    template <> struct is_streameble_write<time_count_blocker_params>: std::tr1::true_type {};
} // namespace serialization {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename key_t>
class time_count_blocker {
public:
    time_count_blocker() {}

    inline time_count_blocker_params& get_params() {return params;}
    inline void increase(key_t const& key);
    inline bool is_blocked(key_t const& key) const;
    inline bool is_critical_blocked(key_t const& key) const;
    inline time_base::millisecond_t get_time_out(key_t const& key) const;
    inline void erase(key_t const& key);
    inline void erase_timeouts();
private:
    time_count_blocker_params params;
    time_outs<key_t, unsigned int> items;
};

template <typename key_t> void time_count_blocker<key_t>::increase(key_t const& key) {
    items.set_time_out(params.time_interval);
    if (items.is_exist(key)) {
        ++items.get(key);
        if (is_critical_blocked(key)) {
            items.change_last_update(key, params.critical_penalty);
        }
        else if (is_blocked(key)) {
            items.change_last_update(key, params.penalty);
        }
    }
    else {
        items.insert(key, 1);
    }
}

template <typename key_t> bool time_count_blocker<key_t>::is_blocked(key_t const& key) const {
    if (items.is_exist(key)) {
        if (items.get(key) >= params.attemp_count) {
            return true;
        }
    }
    return false;
}

template <typename key_t> bool time_count_blocker<key_t>::is_critical_blocked(key_t const& key) const {
    if (items.is_exist(key)) {
        if (items.get(key) >= params.critical_attemp_count) {
            return true;
        }
    }
    return false;
}

template <typename key_t> time_base::millisecond_t time_count_blocker<key_t>::get_time_out(key_t const& key) const {
    if (items.is_exist(key)) {
        return items.get_time_out(key);
    }
    return 0;
}

template <typename key_t> void time_count_blocker<key_t>::erase(key_t const& key) {
    items.erase(key);
}

template <typename key_t> void time_count_blocker<key_t>::erase_timeouts() {
    items.set_time_out(params.time_interval);
    items.erase_timeouts();
}
#endif // TIME_COUNT_BLOCKER_HPP
