#ifndef GAME_CONTEXT_HPP
#define GAME_CONTEXT_HPP
#include <memory>

class game_context {
public:
    typedef std::tr1::shared_ptr<game_context> ptr;

protected:
    game_context(): gc_is_active(false) {}
    virtual ~game_context() {} 
    virtual void on_gc_init() = 0;
    virtual void on_gc_fini() = 0;
    bool get_gc_is_active() const {return gc_is_active;}
private:
    friend class player;
    bool gc_is_active;
};
#endif // GAME_CONTEXT_HPP
