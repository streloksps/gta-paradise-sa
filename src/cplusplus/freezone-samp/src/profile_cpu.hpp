#ifndef PROFILE_CPU_HPP
#define PROFILE_CPU_HPP
#include <ctime>

class profile_cpu
{
public:
    profile_cpu();
    ~profile_cpu();

    bool    is_active;
    void    update();
    float   user_utility_get() const;
    float   kernel_utility_get() const;
    void    raw_get(std::clock_t& ticks_per_seconds, std::clock_t& clock_delta, std::clock_t& user_delta, std::clock_t& kernel_delta) const;

private: // Данные, необходимые между интерациями
    std::clock_t cpu_clock_last;
    std::clock_t cpu_user_last;
    std::clock_t cpu_kernel_last;
    void update_impl();

private: // Результаты замеров
    std::clock_t ticks_per_seconds;
    std::clock_t clock_delta;
    std::clock_t user_delta;
    std::clock_t kernel_delta;
};
#endif // PROFILE_CPU_HPP
