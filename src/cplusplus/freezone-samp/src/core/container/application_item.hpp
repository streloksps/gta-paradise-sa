#ifndef APPLICATION_ITEM_HPP
#define APPLICATION_ITEM_HPP
#include "container_item.hpp"
#include <memory>

class application_item: public container_item {
public:
    typedef std::tr1::shared_ptr<application_item> ptr;
};
#endif // APPLICATION_ITEM_HPP
