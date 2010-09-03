#ifndef CONTAINER_ITEM_HPP
#define CONTAINER_ITEM_HPP
#include <memory>
#include <boost/noncopyable.hpp>
#include "container_fwd.hpp"

class container_item
    : public std::tr1::enable_shared_from_this<container_item> 
    , private boost::noncopyable
{
public:
    inline container_ptr_t get_root() {return root.lock();}
    inline container_ptr_c_t get_root() const {return root.lock();}
    static inline void do_delete(container_item* item) {delete item;}

protected:
    friend class container;
    container_wptr_t root;

protected:
    inline container_item() {} // Создает элемент в заданном контейнере
    virtual ~container_item() {}
};
#endif // CONTAINER_ITEM_HPP
