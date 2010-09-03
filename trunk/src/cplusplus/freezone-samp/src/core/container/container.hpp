#ifndef CONTAINER_HPP
#define CONTAINER_HPP
#include <vector>
#include <memory>
#include <boost/any.hpp>
#include "container_fwd.hpp"
#include "container_item_fwd.hpp"

class container: public std::tr1::enable_shared_from_this<container> {
public:
    typedef std::vector<container_item_ptr> items_t;
protected:
    container()
        :generation(0)
        ,cache(cache_max_size) {}
    virtual ~container() {}

public:
    inline void add_item(container_item_ptr item);

    //Возращает первый попавшийся итем, который приводится к заданному классу
    template<typename T> inline std::tr1::shared_ptr<T>& get_item();
    template<typename T> inline std::tr1::shared_ptr<T const>& get_item() const;
    
    // Возращает все итемы, которые приводятся к заданному типу
    template<typename T> inline std::vector<std::tr1::shared_ptr<T> >& get_items();
    template<typename T> inline std::vector<std::tr1::shared_ptr<T const> >& get_items() const;

private:
    items_t items;
    int generation;

private: // Кеширование
    typedef std::size_t type_id_t;
    
    struct cache_item_t {
        int generation;
        boost::any ptrs; // vector<shared_ptr<T> >
        boost::any cptrs; // vector<shared_ptr<T const> >
        cache_item_t(): generation(-1) {}
    };

    template<typename T> static inline type_id_t get_type_id() {
        static type_id_t rezult = type_id_max++;
        return rezult;
    }

    template<typename T> inline void refresh_cache() const;
    template<typename T> inline cache_item_t& get_cache_item() const;
    template<typename T> inline std::vector<std::tr1::shared_ptr<T> >& get_cache_items();
    template<typename T> inline std::vector<std::tr1::shared_ptr<T const> >& get_cache_items() const;

private:
    mutable std::vector<cache_item_t> cache; // ид в векторе совпадают с type_id
    static type_id_t type_id_max;
    enum {cache_max_size = 128};
};

#include <cassert>
#include "container_item.hpp"

template<typename T>
container::cache_item_t& container::get_cache_item() const {
    type_id_t type_id = get_type_id<T>();
    if (cache.size() <= type_id) {
        assert(cache_max_size <= type_id + 1 && "необходимо увеличить cache_max_size, чтобы обеспечить гарантию отсутствия перераспределения памяти у вектора");
        cache.resize(type_id + 1);
    }
    return cache[type_id];
}

template<typename T>
std::vector<std::tr1::shared_ptr<T> >& container::get_cache_items(){
    cache_item_t& cache_item = get_cache_item<T>();
    assert(!cache_item.ptrs.empty());
    return *boost::unsafe_any_cast<std::vector<std::tr1::shared_ptr<T> > >(&cache_item.ptrs);
}

template<typename T>
std::vector<std::tr1::shared_ptr<T const> >& container::get_cache_items() const {
    cache_item_t& cache_item = get_cache_item<T>();
    assert(!cache_item.ptrs.empty());
    return *boost::unsafe_any_cast<std::vector<std::tr1::shared_ptr<T const> > >(&cache_item.cptrs);
}

template<typename T>
void container::refresh_cache() const {
    cache_item_t& cache_item = get_cache_item<T>();
    if (generation != cache_item.generation) {
        cache_item.generation = generation;

        typedef std::tr1::shared_ptr<T>         ptr_t;
        typedef std::tr1::shared_ptr<T const>   cptr_t;
        typedef std::vector<ptr_t>              ptrs_t;
        typedef std::vector<cptr_t>             cptrs_t;
        
        ptrs_t ptrs;
        cptrs_t cptrs;
        for (items_t::const_iterator p = items.begin(), end = items.end(); p != end; ++p) {
            if (ptr_t ok_ptr = std::tr1::dynamic_pointer_cast<T>(*p)) {
                ptrs.push_back(ok_ptr);
                cptrs.push_back(ok_ptr);
            }
        }
        cache_item.ptrs = ptrs;
        cache_item.cptrs = cptrs;
    }
}

void container::add_item(container_item_ptr item) {
    items.push_back(item);
    item->root = container_wptr_t(shared_from_this());
    ++generation;
    //assert(0 == cache.size());
}

template<typename T>
std::tr1::shared_ptr<T>& container::get_item() {
    typedef std::tr1::shared_ptr<T> ptr_t;
    typedef std::vector<ptr_t>      ptrs_t;

    refresh_cache<T>();
    ptrs_t& cache_data = get_cache_items<T>();
    assert(1 == cache_data.size());
    return cache_data.front();
}

template<typename T>
std::tr1::shared_ptr<T const>& container::get_item() const {
    typedef std::tr1::shared_ptr<T const>   cptr_t;
    typedef std::vector<cptr_t>             cptrs_t;

    refresh_cache<T>();
    cptrs_t& cache_data = get_cache_items<T>();
    assert(1 == cache_data.size());
    return cache_data.front();
}

template<typename T>
std::vector<std::tr1::shared_ptr<T> >& container::get_items() {
    refresh_cache<T>();
    return get_cache_items<T>();
}

template<typename T>
std::vector<std::tr1::shared_ptr<T const> >& container::get_items() const {
    refresh_cache<T>();
    return get_cache_items<T>();
}

#endif // CONTAINER_HPP
