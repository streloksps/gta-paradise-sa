#include "config.hpp"
#include "profile_cpu.hpp"

#ifdef LINUX
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#endif // LINUX

profile_cpu::profile_cpu()
:is_active(false)
,cpu_clock_last(0)
,cpu_user_last(0)
,cpu_kernel_last(0)
,ticks_per_seconds(0)
,clock_delta(0)
,user_delta(0)
,kernel_delta(0)
{
#ifdef LINUX
    is_active = true;
#endif // LINUX
}

profile_cpu::~profile_cpu() {
}

void profile_cpu::update() {
    if (is_active) {
        update_impl();
    }
    else {
        cpu_clock_last = 0;
        clock_delta = 0;
    }
}

#ifdef LINUX
void profile_cpu::update_impl() {
    tms ti;
    ticks_per_seconds = sysconf(_SC_CLK_TCK);
    std::clock_t real_time = times(&ti);

    if (cpu_clock_last) {
        clock_delta = real_time - cpu_clock_last;
        user_delta = ti.tms_utime - cpu_user_last;
        kernel_delta = ti.tms_stime - cpu_kernel_last;
    }

    cpu_clock_last = real_time;
    cpu_user_last = ti.tms_utime;
    cpu_kernel_last = ti.tms_stime;
}
#else // LINUX
void profile_cpu::update_impl() {
}
#endif // LINUX

float profile_cpu::user_utility_get() const {
    if (clock_delta) {
        return static_cast<float>(user_delta) / static_cast<float>(clock_delta);
    }
    return 0.0f;
}

float profile_cpu::kernel_utility_get() const {
    if (clock_delta) {
        return static_cast<float>(kernel_delta) / static_cast<float>(clock_delta);
    }
    return 0.0f;
}

void profile_cpu::raw_get(std::clock_t& ticks_per_seconds, std::clock_t& clock_delta, std::clock_t& user_delta, std::clock_t& kernel_delta) const {
    if (clock_delta) {
        ticks_per_seconds = this->ticks_per_seconds;
        clock_delta = this->clock_delta;
        user_delta = this->user_delta;
        kernel_delta = this->kernel_delta;
    }
    else {
        ticks_per_seconds = 0;
        clock_delta = 0;
        user_delta = 0;
        kernel_delta = 0;
    }
}
