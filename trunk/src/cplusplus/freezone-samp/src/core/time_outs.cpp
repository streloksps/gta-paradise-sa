#include "config.hpp"
#include "time_outs.hpp"

#ifdef LINUX
#include <sys/time.h>
#include <sys/times.h>
#else // LINUX
#include <Windows.h>
#include <Winbase.h>
#endif // LINUX

time_base::millisecond_t time_base::tick_count_milliseconds() {
    // Не работает в линуксе - хотя есть идея использовать для замера производительности
    //return std::clock() / (CLOCKS_PER_SEC/1000);

#ifdef LINUX
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else 
    return static_cast<millisecond_t>(GetTickCount());
#endif
}
