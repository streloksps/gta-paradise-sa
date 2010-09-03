#ifndef MESSAGES_ITEM_HPP
#define MESSAGES_ITEM_HPP
#include "core/buffer/buffer_fwd.hpp"
#include "messages_params.hpp"
#include "messages_sender.hpp"
#include "messages_pager.hpp"
#include "core/player/player_fwd.hpp"

class messages_item {
public:
    messages_item();
    ~messages_item();
    
    inline messages_params& get_params() {return params;}
    inline messages_sender& get_sender() {return sender;}
protected:
    buffer_ptr_t buffer_ptr;
    messages_params params;
    messages_sender sender;
};

class messages_item_paged: public messages_item {
public:
    messages_item_paged(player_ptr_t const& player_ptr);
    ~messages_item_paged();

    inline messages_pager& get_pager() {return pager;}
protected:
    messages_pager pager;
};

#endif // MESSAGES_ITEM_HPP
