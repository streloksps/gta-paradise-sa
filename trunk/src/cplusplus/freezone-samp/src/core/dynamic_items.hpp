#ifndef DYNAMIC_ITEMS_HPP
#define DYNAMIC_ITEMS_HPP
#include <set>
#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <boost/foreach.hpp>

// obj_t - должен реализовывать оператор <
struct dynamic_dummy {};
template <typename obj_t, typename key_t = dynamic_dummy>
class dynamic_items {
public:
    dynamic_items() {}
    ~dynamic_items() {}

public:
    typedef std::set<obj_t> update_t;
    typedef std::vector<key_t> keys_t;

private:
    struct data_holder {
        obj_t obj;
        key_t key;
        data_holder(obj_t const& obj, key_t key): obj(obj), key(key) {}
        data_holder(obj_t const& obj): obj(obj), key() {}
    };
    struct data_holder_obj_less: public std::binary_function<data_holder, data_holder, bool> {
        bool operator()(data_holder const& left, data_holder const& right) const {
            return left.obj < right.obj;
        }
    };
    typedef std::set<data_holder, data_holder_obj_less> data_t;
public:
    // Основные методы
    void        update_begin();
    update_t&   update_get_data();
    // Возращает истину, если были изменения
    template <typename creater_t, typename deleter_t>
    bool        update_end(creater_t creater, deleter_t deleter);
    
    // Сервисные методы
    template <typename deleter_t>
    void delete_obj_if_exist(obj_t const& obj, deleter_t deleter);
    template <typename deleter_t>
    void delete_all(deleter_t deleter);
    void get_keys(keys_t& keys) const;
    bool get_obj_by_key(obj_t& dest_obj, key_t source_key) const;
    int  get_size() const {return data.size();}
private:
    data_t      data;

    update_t    update;
};

template <typename obj_t, typename key_t>
bool dynamic_items<obj_t, key_t>::get_obj_by_key(obj_t& dest_obj, key_t source_key) const {
    BOOST_FOREACH(data_holder const& item, data) {
        if (source_key == item.key) {
            dest_obj = item.obj;
            return true;
        }
    }
    return false;
}
template <typename obj_t, typename key_t>
void dynamic_items<obj_t, key_t>::get_keys(keys_t& keys) const {
    keys.clear();
    std::transform(data.begin(), data.end(), std::back_inserter(keys), std::tr1::bind(&data_holder::key, std::tr1::placeholders::_1));
}

template <typename obj_t, typename key_t> template <typename deleter_t>
void dynamic_items<obj_t, key_t>::delete_obj_if_exist(obj_t const& obj, deleter_t deleter) {
    typename data_t::iterator it = data.find(obj);
    if (data.end() != it) {
        // Объект нашли - удаляем
        deleter(it->key);
        data.erase(it);
    }
}

template <typename obj_t, typename key_t> template <typename deleter_t>
void dynamic_items<obj_t, key_t>::delete_all(deleter_t deleter) {
    for (typename data_t::iterator it = data.begin(), end = data.end(); end != it; ++it) {
        deleter(it->key);
    }
    data.clear();
}

template <typename obj_t, typename key_t> template <typename creater_t, typename deleter_t>
bool dynamic_items<obj_t, key_t>::update_end(creater_t creater, deleter_t deleter) {
    // Синхронизируем новый список объектов и старый.
    // Вначале удаляем все удаленные объекты, потом добавляем новые
    //
    // Предпологаемые функторы:
    //   function<key_t (obj_t const&)> creater_t;
    //   function<void (key_t)> deleter_t;

    typedef std::vector<typename update_t::const_iterator> items_to_create_t;

    data_t new_data;
    items_to_create_t items_to_create;
    bool is_changed = false;

    typename update_t::const_iterator new_it = update.begin(), new_end = update.end();
    typename data_t::const_iterator old_it = data.begin(), old_end = data.end();
    for (;new_end != new_it && old_end != old_it;) {
        if (*new_it < old_it->obj) {
            // Добавляем итем
            items_to_create.push_back(new_it);
            is_changed = true;
            ++new_it;
        }
        else if (old_it->obj < *new_it) {
            // Удаляем итем
            deleter(old_it->key);
            is_changed = true;
            ++old_it;
        }
        else {
            // Равны
            new_data.insert(*old_it);
            ++new_it;
            ++old_it;
        }
    }
    for (;old_end != old_it; ++old_it) {
        // Удаляем конец
        deleter(old_it->key);
        is_changed = true;
    }
    for (;new_end != new_it; ++new_it) {
        // Добавляем конец
        items_to_create.push_back(new_it);
        is_changed = true;
    }

    // Производим фактическое добавление объектов
    for (typename items_to_create_t::const_iterator it = items_to_create.begin(), end = items_to_create.end(); end != it; ++it) {
        new_data.insert(data_holder(**it, creater(**it)));
    }

    if (is_changed) {
        data.swap(new_data);
    }
    update.clear();
    return is_changed;
}

template <typename obj_t, typename key_t>
typename dynamic_items<obj_t, key_t>::update_t& dynamic_items<obj_t, key_t>::update_get_data() {
    return update;
}

template <typename obj_t, typename key_t>
void dynamic_items<obj_t, key_t>::update_begin() {
    update.clear();
}
#endif // DYNAMIC_ITEMS_HPP
