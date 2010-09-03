#ifndef SAMP_HOOK_EVENTS_ADDRESSES_HPP
#define SAMP_HOOK_EVENTS_ADDRESSES_HPP
#include <map>
#include <string>
#include <vector>

namespace samp {
    typedef unsigned long address_t;

    struct replace_pair_t {
        address_t from;
        address_t* to;
        replace_pair_t(address_t from, address_t* to): from(from), to(to) {}
    };

    typedef std::vector<replace_pair_t> replace_pairs_t;

    struct samp_hook_t {
        address_t printf;
        std::string name;
        replace_pairs_t E8;
        replace_pairs_t FF15;
        replace_pairs_t _8B1D;
        samp_hook_t(address_t printf, std::string const& name): printf(printf), name(name) {}
    };

    typedef std::map<std::string, samp_hook_t> samp_hooks_t;

    
    extern address_t samp_on_bad_rcon_external;
    extern address_t samp_on_bad_rcon_local;
    extern address_t samp_on_player_rename;
    extern address_t samp_on_player_chat;
    extern address_t samp_on_recvfrom;
    extern address_t samp_on_sendto;
    extern address_t samp_is_bad_nick;

    extern samp_hooks_t samp_hooks;
} // namespace samp {
#endif // SAMP_HOOK_EVENTS_ADDRESSES_HPP
