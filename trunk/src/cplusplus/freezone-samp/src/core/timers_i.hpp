#ifndef TIMERS_I_HPP
#define TIMERS_I_HPP

struct on_timer250_i {
    virtual void on_timer250() = 0;
};
struct on_timer500_i {
    virtual void on_timer500() = 0;
};
struct on_timer1000_i {
    virtual void on_timer1000() = 0;
};
struct on_timer5000_i {
    virtual void on_timer5000() = 0;
};
struct on_timer15000_i {
    virtual void on_timer15000() = 0;
};
struct on_timer60000_i {
    virtual void on_timer60000() = 0;
};

#endif // TIMERS_I_HPP
