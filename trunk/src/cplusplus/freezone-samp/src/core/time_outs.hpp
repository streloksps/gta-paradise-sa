#ifndef TIME_OUTS_HPP
#define TIME_OUTS_HPP
#include <map>
#include <functional>
#include <algorithm>
#include <iterator>
#include <cassert>
//#include <boost/date_time/posix_time/posix_time.hpp>

struct time_base {
    typedef int millisecond_t;

    // Может возращать отрицательные числа (использовать только разницы)
    static millisecond_t tick_count_milliseconds();

    //// Очень медленно
    //typedef boost::posix_time::time_duration::tick_type millisecond_t;
    //static inline millisecond_t epoch_milliseconds() {
    //    boost::posix_time::time_duration epoch = boost::posix_time::microsec_clock::universal_time() - boost::posix_time::ptime(boost::gregorian::date(1970,1,1));
    //    return epoch.total_milliseconds();
    //}
};

// Пустой класс
struct time_outs_dummy_val {};

// Класс тайм-аутов.
template <typename key_t, typename val_t = time_outs_dummy_val>
class time_outs: public time_base {
    
public:
    time_outs(millisecond_t time_out = 1000): time_out(time_out) {}
    ~time_outs() {}
    
    // Устанавливает значение тайм аута - перезатерает то, что было установлино при конструировании объекта
    inline void set_time_out(millisecond_t time_out_new);

    // Возращает истину, если значение с заданным ключом существует и для него не вышел тайм аут
    inline bool is_exist(key_t const& key) const;
    
    // Возращает значение по ключу. Перед вызовом обязательно проверить наличие значение методом is_exist
    inline val_t& get(key_t const& key);
    inline val_t const& get(key_t const& key) const;

    // Возращает значение тайм аута для заданного ключа. Перед вызовом обязательно проверить наличие значение методом is_exist
    inline millisecond_t get_time_out(key_t const& key) const;

    // Обновляет время последнего обновления по ключу. Итем должен существовать
    inline void refresh(key_t const& key);

    // Если итем есть - то рефрешим и возращаем истину, если нет - возращаем лож
    inline bool refresh_if_exist(key_t const& key);

    // Изменяет время последнего обновления на заданное число
    inline void change_last_update(key_t const& key, millisecond_t change_value);

    // Добовляет новый итем. Если итем с заданным ключем существует и(или) для него вышел тайм аут - то старый итем удаляется, при этом вызывается пользовательский обработчик этого события
    template <typename deleter_t> inline void insert(deleter_t deleter, key_t const& key, val_t const& val = val_t());
    inline void insert(key_t const& key, val_t const& val = val_t()) {insert(&dummy_deleter, key, val);}

    // Удаляет заданный итем
    template <typename deleter_t> inline void erase(deleter_t deleter, key_t const& key);
    inline void erase(key_t const& key) {erase(&dummy_deleter, key);}

    // Удаляет все итемы, у которых прошли тайм ауты. Для удалившихся итемов вызывается обработчик
    template <typename deleter_t> inline void erase_timeouts(deleter_t deleter);
    inline void erase_timeouts() {erase_timeouts(&dummy_deleter);}

    // Удаляет все итемы, вызывая обработчик для них
    template <typename deleter_t> inline void erase_all(deleter_t deleter);

    struct map_second {
        millisecond_t last_update;
        val_t         value;
        map_second(val_t const& value): value(value), last_update(tick_count_milliseconds()) {}
    };

    typedef std::map<key_t, map_second> items_t;

    items_t const& get_items() const {
        return items;
    }         

private:

    millisecond_t time_out;
    items_t items;

    // Возращает истину, если тайм-аут вышел
    inline bool is_item_timeout(typename items_t::value_type const& item, millisecond_t curr_time = tick_count_milliseconds()) const;

    // Пустой обработчик, если клиенский код не хочет обрабатывать ситуацию удаления итема
    inline static void dummy_deleter(key_t const& point, time_base::millisecond_t const& last_update, val_t const& connect_time) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename key_t, typename val_t> void time_outs<key_t, val_t>::set_time_out(millisecond_t time_out_new) {
    time_out = time_out_new;
}

template <typename key_t, typename val_t> bool time_outs<key_t, val_t>::is_exist(key_t const& key) const {
    typename items_t::const_iterator it = items.find(key);
    if (items.end() != it) {
        // Нашли итем
        if (!is_item_timeout(*it)) {
            // Тайм аут по нему еще не вышел
            return true;
        }        
    }
    return false;
}

template <typename key_t, typename val_t> val_t& time_outs<key_t, val_t>::get(key_t const& key) {
    assert(is_exist(key) && "ключа нет");
    typename items_t::iterator it = items.find(key);
    return it->second.value;
}

template <typename key_t, typename val_t> val_t const& time_outs<key_t, val_t>::get(key_t const& key) const {
    assert(is_exist(key) && "ключа нет");
    typename items_t::const_iterator it = items.find(key);
    return it->second.value;
}

template <typename key_t, typename val_t> time_base::millisecond_t time_outs<key_t, val_t>::get_time_out(key_t const& key) const {
    assert(is_exist(key) && "ключа нет");
    typename items_t::const_iterator it = items.find(key);
    return time_out - (tick_count_milliseconds() - it->second.last_update);
}

template <typename key_t, typename val_t> void time_outs<key_t, val_t>::refresh(key_t const& key) {
    assert(is_exist(key) && "ключа нет");
    typename items_t::iterator it = items.find(key);
    it->second.last_update = tick_count_milliseconds();
}

template <typename key_t, typename val_t>
bool time_outs<key_t, val_t>::refresh_if_exist(key_t const& key) {
    typename items_t::iterator it = items.find(key);
    time_base::millisecond_t curr_time = tick_count_milliseconds();
    if (items.end() != it) {
        // Нашли итем
        if (!is_item_timeout(*it, curr_time)) {
            // Тайм аут по нему еще не вышел
            it->second.last_update = curr_time;
            return true;
        }        
    }
    return false;
}

template <typename key_t, typename val_t> void time_outs<key_t, val_t>::change_last_update(key_t const& key, millisecond_t change_value) {
    assert(is_exist(key) && "ключа нет");
    typename items_t::iterator it = items.find(key);
    it->second.last_update += change_value;
}

template <typename key_t, typename val_t> template <typename deleter_t> void time_outs<key_t, val_t>::insert(deleter_t deleter, key_t const& key, val_t const& val) {
    erase(deleter, key);
    items.insert(std::make_pair(key, map_second(val)));
}

template <typename key_t, typename val_t> template <typename deleter_t> void time_outs<key_t, val_t>::erase(deleter_t deleter, key_t const& key) {
    typename items_t::iterator it = items.find(key);
    if (items.end() != it) {
        // Если такой итем уже есть - удаляем его
        deleter(it->first, it->second.last_update, it->second.value);
        items.erase(it);
    }
}

template <typename key_t, typename val_t> template <typename deleter_t> void time_outs<key_t, val_t>::erase_timeouts(deleter_t deleter) {
    millisecond_t curr_time = tick_count_milliseconds();
    for (typename items_t::iterator it = items.begin(), end = items.end(); it != end;) {
        typename items_t::iterator work_it = it++;
        if (is_item_timeout(*work_it, curr_time)) {
            // Вышел тайм-аут
            deleter(work_it->first, work_it->second.last_update, work_it->second.value);
            items.erase(work_it);
        }
    }
}

template <typename key_t, typename val_t> template <typename deleter_t> void time_outs<key_t, val_t>::erase_all(deleter_t deleter) {
    for (typename items_t::iterator it = items.begin(), end = items.end(); it != end; ++it) {
        deleter(it->first, it->second.last_update, it->second.value);
    }
    items.clear();
}

template <typename key_t, typename val_t> bool time_outs<key_t, val_t>::is_item_timeout(typename items_t::value_type const& item, millisecond_t curr_time) const {
    return curr_time - item.second.last_update > time_out;
}

#endif // TIME_OUTS_HPP
