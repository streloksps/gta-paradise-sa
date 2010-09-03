#ifndef CONTAINER_ITEM_FWD_HPP
#define CONTAINER_ITEM_FWD_HPP
#include <memory>

class container_item;
typedef std::tr1::shared_ptr<container_item> container_item_ptr;
typedef std::tr1::weak_ptr<container_item> container_item_wptr;
#endif // CONTAINER_ITEM_FWD_HPP
