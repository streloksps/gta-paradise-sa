#ifndef CONTAINER_FWD_HPP
#define CONTAINER_FWD_HPP
#include <memory>

class container;
typedef std::tr1::shared_ptr<container> container_ptr_t;
typedef std::tr1::shared_ptr<container const> container_ptr_c_t;
typedef std::tr1::weak_ptr<container> container_wptr_t;

#endif // CONTAINER_FWD_HPP
