#include "config.hpp"
#include "messages_item.hpp"
#include "core/buffer/buffer.hpp"
#include "messages_storage.hpp"

messages_item::messages_item()
:buffer_ptr(messages_storage::instance()->get_msg_buff()->children_create_connected())
,params(buffer_ptr)
,sender(buffer_ptr)
{
}

messages_item::~messages_item() {

}

messages_item_paged::messages_item_paged(player_ptr_t const& player_ptr)
:pager(player_ptr, buffer_ptr)
{
}

messages_item_paged::~messages_item_paged() {

}
