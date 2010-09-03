#ifndef PLAYER_FWD_HPP
#define PLAYER_FWD_HPP
#include <memory>
class player;

typedef std::tr1::shared_ptr<player> player_ptr_t;
typedef std::tr1::shared_ptr<player const> player_ptr_c_t;
typedef std::tr1::weak_ptr<player> player_wptr_t;
typedef std::tr1::weak_ptr<player const> player_wptr_c_t;
#endif // PLAYER_FWD_HPP
