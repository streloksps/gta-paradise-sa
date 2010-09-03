#ifndef BUFFER_FWD_HPP
#define BUFFER_FWD_HPP
#include <memory>

class buffer;
typedef std::tr1::shared_ptr<buffer> buffer_ptr_t;
typedef std::tr1::shared_ptr<buffer const> buffer_ptr_c_t;

#endif // BUFFER_FWD_HPP
