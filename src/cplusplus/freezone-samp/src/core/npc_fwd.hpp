#ifndef NPC_FWD_HPP
#define NPC_FWD_HPP
#include <memory>
class npc;

typedef std::tr1::shared_ptr<npc> npc_ptr_t;
typedef std::tr1::shared_ptr<npc const> npc_ptr_c_t;
typedef std::tr1::weak_ptr<npc> npc_wptr_t;
typedef std::tr1::weak_ptr<npc const> npc_wptr_c_t;
#endif // NPC_FWD_HPP
