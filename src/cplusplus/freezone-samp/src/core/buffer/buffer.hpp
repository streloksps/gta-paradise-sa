#ifndef BUFFER_HPP
#define BUFFER_HPP
#include "buffer_fwd.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class buffer: public std::tr1::enable_shared_from_this<buffer> {
public:
    typedef std::tr1::shared_ptr<buffer> ptr;
    typedef std::tr1::shared_ptr<buffer const> ptr_c;
    typedef std::tr1::weak_ptr<buffer const> wptr; 
    typedef std::tr1::unordered_multimap<std::string, std::string> values_t;
    typedef std::tr1::unordered_multimap<std::string, ptr> children_t;
    
    typedef std::vector<std::string> values_vector_t;
    typedef std::vector<ptr>         children_vector_t;

    values_t values;

    children_t children;
    wptr parent;

    buffer();
    ~buffer();

    void clear();


    // Добавляет связный элемент, но не делает его потомком
    ptr children_create_connected() const;

    // Добавляет потомка, с заданным именем
    ptr children_add(std::string const& name);
    
    // Возращает потомка. Если потомка с заданным именем нет, то возращает нулевой указатель
    bool                children_is_exist(std::string const& name) const;
    ptr                 children_get(std::string const& name) const;
    children_vector_t   children_get_all(std::string const& name) const;


    void        value_add(std::string const& name, std::string const& value);
    void        value_replace(std::string const& name, std::string const& value);
    bool        value_is_exist(std::string const& name) const;
    std::string value_get(std::string const& name) const;
    values_vector_t value_get_all(std::string const& name) const;
    
    // Обрабатывает все замены переменных в строке, используя текущий буффер как базу
    std::string process_all_vars(std::string const& str) const;
};
#endif // BUFFER_HPP
