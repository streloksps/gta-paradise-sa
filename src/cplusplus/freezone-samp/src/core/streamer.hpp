#ifndef STREAMER_HPP
#define STREAMER_HPP
#include "streamer_grid.hpp"
#include <map>
#include <set>
#include <vector>

namespace streamer {
    // Класс стримера
    template 
        <typename item_t
        ,typename item_grid_t = grid_t<item_t, int, 150>
        > 
    class streamer_t
        :private item_grid_t
    {

    public:
        typedef typename item_grid_t::key_type key_t;
        typedef std::tr1::function<void (key_t key)> delete_item_event_t;

        typedef std::multimap<typename coord_works<item_t>::metric_t, key_t> metrics_keys_t;
        typedef std::vector<key_t> item_kyes_t;
        typedef std::set<key_t> item_kyes_set_t;
        typedef std::vector<pos3> poses_t;

        streamer_t() {}
        ~streamer_t() {}

        key_t           item_add(item_t const& item);
        void            item_delete(key_t key);
        item_t const&   item_get(key_t key) const;
        void            clear();
        void            items_load(std::vector<item_t> const& items);

        // Возращает список итемов рядом с точкой
        void            items_get_by_pos(metrics_keys_t& rezult, pos3 const& pos);
        void            items_get_by_pos(item_kyes_set_t& rezult, pos3 const& pos, std::size_t max_rezult_count);
        bool            item_get_by_pos(key_t& key, pos3 const& pos); // Возращает истину, если нашел хотя бы 1 итем

        // Возращает список итемов рядом с вектором точек
        template<typename collate_item_t>
        void            items_get_by_poses(metrics_keys_t& rezult, poses_t const& poses, collate_item_t collate_item);
        template<typename collate_item_t>
        void            items_get_by_poses(item_kyes_set_t& rezult, poses_t const& poses, collate_item_t collate_item, std::size_t max_rezult_count);

        delete_item_event_t delete_item_event;
    private:
        typedef std::vector<item_t> storage_t;
        typedef std::vector<bool>   storage_usage_t;

        storage_t storage;
        storage_usage_t storage_usage;

        bool get_unused_key(key_t& key) const;

        // Объект сравнения
        template <typename collate_item_t>
        struct collate_traits: public std::binary_function<key_t, key_t, bool> {
            bool operator()(key_t left, key_t right) const {
                return collate_item(storage[left], storage[right]);
            }
            collate_traits(storage_t& storage, collate_item_t& collate_item): storage(storage), collate_item(collate_item) {}
        private:
            storage_t& storage;
            collate_item_t& collate_item;
        };
    };

    template <typename item_t, typename item_grid_t>
    bool streamer_t<item_t, item_grid_t>::item_get_by_pos(key_t& key, pos3 const& pos) {
        metrics_keys_t metrics_keys;
        items_get_by_pos(metrics_keys, pos);
        if (!metrics_keys.empty()) {
            key = metrics_keys.begin()->second;
            return true;
        }
        return false;
    }

    template <typename item_t, typename item_grid_t>
    void streamer_t<item_t, item_grid_t>::items_load(std::vector<item_t> const& items) {
        std::for_each(items.begin(), items.end(), std::tr1::bind(&streamer_t<item_t, item_grid_t>::item_add, this, std::tr1::placeholders::_1));
    }

    template <typename item_t, typename item_grid_t>
    void streamer_t<item_t, item_grid_t>::clear() {
        storage.clear();
        storage_usage.clear();
        item_grid_t::grid_clear();
    }

    template <typename item_t, typename item_grid_t>
    typename streamer_t<item_t, item_grid_t>::key_t streamer_t<item_t, item_grid_t>::item_add(item_t const& item) {
        key_t rezult;
        if (get_unused_key(rezult)) {
            assert((std::size_t)rezult < storage.size());
            storage[rezult] = item;
            storage_usage[rezult] = true;
        }
        else {
            rezult = storage.size();
            storage.push_back(item);
            storage_usage.push_back(true);
        }
        item_grid_t::grid_add(item, rezult);
        return rezult;
    }

    template <typename item_t, typename item_grid_t>
    void streamer_t<item_t, item_grid_t>::item_delete(key_t key) {
        assert((std::size_t)key < storage.size() && storage_usage[key]);
        item_grid_t::grid_remove(item_get(key), key);
        storage_usage[key] = false;
        if (delete_item_event) {
            delete_item_event(key);
        }
    }

    template <typename item_t, typename item_grid_t>
    item_t const& streamer_t<item_t, item_grid_t>::item_get(key_t key) const {
        assert((std::size_t)key < storage.size() && storage_usage[key]);
        return storage[key];
    }

    template <typename item_t, typename item_grid_t>
    void streamer_t<item_t, item_grid_t>::items_get_by_pos(metrics_keys_t& rezult, pos3 const& pos) {
        rezult.clear();

        typename item_grid_t::keys_t curr_keys;

        // Получаем спиоск возможных ид, для дальнейшего иследования
        item_grid_t::grid_get(curr_keys, pos);

        // Формируем мапу из подходящих нам итемов
        for (typename item_grid_t::keys_t::const_iterator it = curr_keys.begin(), end = curr_keys.end(); end != it; ++it) {
            item_t const& item = item_get(*it);
            if (coord_works<item_t>::get_is_point_in(item, pos)) {
                // Наша координата попала в сферу элемента
                rezult.insert(std::make_pair(coord_works<item_t>::get_metric(item, pos), *it));
            }
        }
    }

    template <typename item_t, typename item_grid_t> template<typename collate_item_t>
    void streamer_t<item_t, item_grid_t>::items_get_by_poses(metrics_keys_t& rezult, poses_t const& poses, collate_item_t collate_item) {
        typedef collate_traits<collate_item_t> collate_t;

        typedef std::map<key_t, typename coord_works<item_t>::metric_t, collate_t> pre_collected_t;
        collate_t collate(storage, collate_item);
        pre_collected_t pre_collected(collate);

        typename item_grid_t::keys_t curr_keys;

        for (typename poses_t::const_iterator popes_it = poses.begin(), poses_end = poses.end(); poses_end != popes_it; ++popes_it) {
            // Получаем спиоск возможных ид, для дальнейшего иследования
            item_grid_t::grid_get(curr_keys, *popes_it);

            // Формируем мапу из подходящих нам итемов
            for (typename item_grid_t::keys_t::const_iterator it = curr_keys.begin(), end = curr_keys.end(); end != it; ++it) {
                item_t const& item = item_get(*it);
                if (coord_works<item_t>::get_is_point_in(item, *popes_it)) {
                    // Наша координата попала в сферу элемента
                    typename coord_works<item_t>::metric_t metric = coord_works<item_t>::get_metric(item, *popes_it);
                    typename pre_collected_t::iterator pre_collected_it = pre_collected.find(*it);
                    if (pre_collected.end() == pre_collected_it) {
                        // новый итем
                        pre_collected.insert(std::make_pair(*it, metric));
                    }
                    else {
                        // Итем уже есть в буфере - меняем метрику на меньщую, если нужно
                        if (pre_collected_it->second > metric) {
                            pre_collected_it->second = metric;
                        }
                    }
                }
            }
        }

        rezult.clear();
        // Сортируем по метрике
        for (typename pre_collected_t::const_iterator it = pre_collected.begin(), end = pre_collected.end(); end != it; ++it) {
            rezult.insert(std::make_pair(it->second, it->first));
        }
    }

    template <typename item_t, typename item_grid_t>
    void streamer_t<item_t, item_grid_t>::items_get_by_pos(item_kyes_set_t& rezult, pos3 const& pos, std::size_t max_rezult_count) {
        rezult.clear();

        metrics_keys_t metrics_keys;

        items_get_by_pos(metrics_keys, pos);

        // Заполняем массив результата - только заданное число элементов (возможный хвост отсикаем)
        std::size_t count = 0;
        for (typename metrics_keys_t::const_iterator it = metrics_keys.begin(), end = metrics_keys.end(); end != it && count < max_rezult_count; ++it, ++count) {
            rezult.insert(it->second);
        }
    }

    template <typename item_t, typename item_grid_t> template<typename collate_item_t>
    void streamer_t<item_t, item_grid_t>::items_get_by_poses(item_kyes_set_t& rezult, poses_t const& poses, collate_item_t collate_item, std::size_t max_rezult_count) {
        rezult.clear();

        metrics_keys_t metrics_keys;

        items_get_by_poses(metrics_keys, poses, collate_item);

        // Заполняем массив результата - только заданное число элементов (возможный хвост отсикаем)
        std::size_t count = 0;
        for (typename metrics_keys_t::const_iterator it = metrics_keys.begin(), end = metrics_keys.end(); end != it && count < max_rezult_count; ++it, ++count) {
            rezult.insert(it->second);
        }
    }

    template <typename item_t, typename item_grid_t>
    bool streamer_t<item_t, item_grid_t>::get_unused_key(key_t& key) const {
        typename storage_usage_t::const_iterator it = std::find(storage_usage.begin(), storage_usage.end(), false);
        if (storage_usage.end() == it) {
            // Не нашли свободных ячеек
            return false;
        }
        else {
            key = it - storage_usage.begin();
            return true;
        }
    }

} // namespace streamer {

#endif // STREAMER_HPP
