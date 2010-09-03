#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cassert>
#include <type_traits>
#include <memory>
#include <sstream>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include "core/buffer/buffer.hpp"
#include "is_streameble.hpp"

namespace serialization {
    /************************************************************************/
    /* Работа с именами                                                     */
    /************************************************************************/
    struct auto_name {
        virtual char const* get_auto_name() const = 0;
    };
    template <typename T>
    struct nvp {
        nvp(char const* name, T& value): name(name), value(value) {}
        std::string name;
        T& value;
    };
    

    template <typename T>
    nvp<T> make_nvp(std::tr1::shared_ptr<T>& value, char const* name = "") {
        return make_nvp(*value.get(), name);
    }

    template <typename T>
    nvp<T> make_nvp(T& value, char const* name, typename boost::enable_if<boost::mpl::and_<boost::mpl::not_<std::tr1::is_base_of<auto_name, T> >, boost::mpl::not_<std::tr1::is_polymorphic<T> > > >::type* = 0) {
        return nvp<T>(name, value);
    }
    
    template <typename T>
    nvp<T> make_nvp(T& value, char const* name = "", typename boost::enable_if<boost::mpl::and_<std::tr1::is_polymorphic<T>,boost::mpl::not_<std::tr1::is_base_of<auto_name, T> > > >::type* = 0) {
        if (auto_name* auto_name_ptr = dynamic_cast<auto_name*>(&value)) {
            nvp<T>(auto_name_ptr->get_auto_name(), value);
        }
        assert(!std::string(name).empty() && "Имя не задано");
        return nvp<T>(name, value);
    }
    template <typename T>
    nvp<T> make_nvp(T& value, char const* name = "", typename boost::enable_if<std::tr1::is_base_of<auto_name, T> >::type* = 0) {
        return nvp<T>(value.get_auto_name(), value);
    }


    /************************************************************************/
    /* Работа с определениями                                               */
    /************************************************************************/
    enum def_type_e {
         def_reading = 0
        ,def_writing
    };

    template <typename T> struct is_streameble_tmp: boost::mpl::or_<is_streameble_read<T>, is_streameble_write<T> > {};

    template <typename interface_type>
    struct def_t {
        typedef void (interface_type::*member_t)(buffer::ptr const& buff, def_t<interface_type> const& def);
        def_t(member_t member, def_type_e def_type): member(member), def_type(def_type) {}

        def_type_e def_type;
        member_t member;
    };

    template <typename interface_type>
    def_t<interface_type> make_def(void (interface_type::*member)(buffer::ptr const& buff, def_t<interface_type> const& def), def_type_e def_type) {
        return def_t<interface_type>(member, def_type);
    }

} // namespace serialization {

/************************************************************************/
/* Мокросы для упращения жизни                                          */
/************************************************************************/
#   define SERIALIZE_MAKE_NAME2(ITEM) #ITEM
#   define SERIALIZE_MAKE_NAME(ITEM) SERIALIZE_MAKE_NAME2(ITEM)
#   define SERIALIZE_MAKE_NVP(ITEM) serialization::make_nvp(ITEM, SERIALIZE_MAKE_NAME(ITEM))

#   define SERIALIZE_ITEM(ITEM) serialize(buff, SERIALIZE_MAKE_NVP(ITEM), def);
#   define SERIALIZE_NAMED_ITEM(ITEM,NAME) serialize(buff, serialization::make_nvp(ITEM, NAME), def);

#   define SERIALIZE_DELIMITED_ITEM(ITEM) serialize_delimited(buff, SERIALIZE_MAKE_NVP(ITEM), def);
#   define SERIALIZE_DELIMITED_NAMED_ITEM(ITEM,NAME) serialize_delimited(buff, serialization::make_nvp(ITEM, NAME), def);

#   define SERIALIZE_MAKE_CLASS_NAME(CLASS) virtual char const* get_auto_name() const { return #CLASS;}

/************************************************************************/
/* Сериализации                                                         */
/************************************************************************/
namespace serialization {
    // Обобщенная сериализация
    template <typename T, typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<T> const& nvp_item, def_t<interface_type> const& def, typename boost::disable_if<is_streameble_tmp<T> >::type* = 0) {
        if (def_reading == def.def_type) {
            buffer::ptr work_buff;
            if (buff) {
                work_buff = buff->children_get(nvp_item.name);
            }
            (nvp_item.value.*def.member)(work_buff, def);
            return work_buff;
        }
        return false;
    }

    // Сериализция через потоки
    template <typename T, typename interface_type>
    inline bool serialize_read(buffer::ptr const& buff, nvp<T> const& nvp_val, def_t<interface_type> const& def, typename boost::enable_if<is_streameble_read<T> >::type* = 0) {
        if (!buff->value_is_exist(nvp_val.name)) {
            return false;
        }
        std::istringstream is(buff->value_get(nvp_val.name));
        is.setf(std::ios_base::boolalpha);
        is.unsetf(std::ios_base::dec);
        T val;
        is>>val;
        if (!is.fail() && is.eof()) {
            // Успешно смогли распарсить итем
            nvp_val.value = val;
            return true;
        }
        return false;
    }
    template <typename T, typename interface_type>
    inline bool serialize_read(buffer::ptr const& buff, nvp<T> const& nvp_val, def_t<interface_type> const& def, typename boost::disable_if<is_streameble_read<T> >::type* = 0) {
        assert(false && "Не реализовано");
        return false;
    }

    template <typename T, typename interface_type>
    inline bool serialize_write(buffer::ptr const& buff, nvp<T> const& nvp_val, def_t<interface_type> const& def, typename boost::enable_if<is_streameble_write<T> >::type* = 0) {
        std::ostringstream os;
        os.setf(std::ios_base::boolalpha);
        os<<nvp_val.value;
        buff->value_add(nvp_val.name, os.str());
        return true;
    }
    template <typename T, typename interface_type>
    inline bool serialize_write(buffer::ptr const& buff, nvp<T> const& nvp_val, def_t<interface_type> const& def, typename boost::disable_if<is_streameble_write<T> >::type* = 0) {
        assert(false && "Не реализовано");
        return false;
    }
    template <typename T, typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<T> const& nvp_val, def_t<interface_type> const& def, typename boost::enable_if<is_streameble_tmp<T> >::type* = 0) {
        if (def_reading == def.def_type && buff) {
            return serialize_read(buff, nvp_val, def);
        }
        else if (def_writing == def.def_type && buff) {
            return serialize_write(buff, nvp_val, def);
        }
        return false;
    }

    // Сериализция строки
    template <typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<std::string> const& nvp_val, def_t<interface_type> const& def) {
        if (def_reading == def.def_type && buff && buff->value_is_exist(nvp_val.name)) {
            nvp_val.value = buff->value_get(nvp_val.name);
            return true;
        }
        else if (def_writing == def.def_type && buff) {
            buff->value_add(nvp_val.name, nvp_val.value);
            return true;
        }
        return false;
    }

    // Сериализция 1 символа
    template <typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<char> const& nvp_val, def_t<interface_type> const& def) {
        if (def_reading == def.def_type && buff && buff->value_is_exist(nvp_val.name)) {
            std::string tmp_val = buff->value_get(nvp_val.name);
            std::istringstream is(tmp_val);
            is.unsetf(std::ios_base::dec);
            char val;
            is>>val;
            if (!is.fail() && 1 == tmp_val.length()) {
                // Успешно смогли распарсить итем
                nvp_val.value = val;
                return true;
            }
        }
        else if (def_writing == def.def_type && buff) {
            assert(false);
        }
        return false;
    }

    // Сериализация вектора через потоки
    template <typename T, typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<std::vector<T> > const& nvp_vector, def_t<interface_type> const& def, typename boost::enable_if<is_streameble_tmp<T> >::type* = 0) {
        if (def_reading == def.def_type && buff && buff->value_is_exist(nvp_vector.name)) {
            std::vector<T> rezult;
            buffer::values_vector_t vector = buff->value_get_all(nvp_vector.name);
            buffer::ptr tmp_buff(new buffer());
            BOOST_FOREACH(std::string& val, vector) {
                tmp_buff->value_add("vector_item", val);
                T rezult_item;
                if (serialize(tmp_buff, make_nvp(rezult_item, "vector_item"), def)) {
                    rezult.push_back(rezult_item);
                }
                tmp_buff->clear();
            }
            if (!rezult.empty()) {
                nvp_vector.value.swap(rezult);
                return true;
            }
        }
        else if (def_writing == def.def_type && buff) {
            assert(false);
        }
        return false;
    }

    // Сериализация сета списком через потоки
    template <typename T, typename interface_type>
    inline bool serialize_delimited(buffer::ptr const& buff, nvp<std::set<T> > const& nvp_set, def_t<interface_type> const& def, std::string const& delimiter_chars = ",", std::string const& trim_chars = "\t ", typename boost::enable_if<is_streameble_tmp<T> >::type* = 0) {
        if (def_reading == def.def_type && buff) {
            std::string values_str;
            if (serialize(buff, make_nvp(values_str, nvp_set.name.c_str()), def)) {
                boost::trim_if(values_str, boost::is_any_of(trim_chars));
                buffer::ptr tmp_buff(new buffer());
                for (boost::split_iterator<std::string::iterator> split_it = boost::make_split_iterator(values_str, boost::token_finder(boost::is_any_of(delimiter_chars), boost::token_compress_on)), end; split_it != end; ++split_it) {
                    std::string val_str = boost::copy_range<std::string>(*split_it);
                    if (!val_str.empty()) {
                        tmp_buff->value_add("set_item", val_str);
                        T rezult_item;
                        if (serialize(tmp_buff, make_nvp(rezult_item, "set_item"), def)) {
                            nvp_set.value.insert(rezult_item);
                        }
                        tmp_buff->clear();
                    }
                }
            }
        }
        else if (def_writing == def.def_type) {
            assert(false);
            return true;
        }
        return false;
    }

    // Сериализация сета через потоки
    template <typename T, typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<std::set<T> > const& nvp_set, def_t<interface_type> const& def, typename boost::enable_if<is_streameble_tmp<T> >::type* = 0) {
        if (def_reading == def.def_type && buff && buff->value_is_exist(nvp_set.name)) {
            std::set<T> rezult;
            buffer::values_vector_t vector = buff->value_get_all(nvp_set.name);
            buffer::ptr tmp_buff(new buffer());
            BOOST_FOREACH(std::string& val, vector) {
                tmp_buff->value_add("set_item", val);
                T rezult_item;
                if (serialize(tmp_buff, make_nvp(rezult_item, "set_item"), def)) {
                    rezult.insert(rezult_item);
                }
                tmp_buff->clear();
            }
            if (!rezult.empty()) {
                nvp_set.value.swap(rezult);
                return true;
            }
        }
        else if (def_writing == def.def_type) {
            BOOST_FOREACH(T val, nvp_set.value) {
                serialize(buff, make_nvp(val, nvp_set.name.c_str()), def);
            }
            return true;
        }
        return false;
    }

    // Сериализация мапа через потоки
    template <typename key_t, typename val_t, typename traits_t, typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<std::map<key_t, val_t, traits_t> > const& nvp_map, def_t<interface_type> const& def, typename boost::enable_if<is_streameble_tmp<key_t> >::type* = 0, typename boost::enable_if<is_streameble_tmp<val_t> >::type* = 0) {
        if (def_reading == def.def_type && buff && buff->value_is_exist(nvp_map.name)) {
            std::map<key_t, val_t, traits_t> rezult;
            buffer::values_vector_t vector = buff->value_get_all(nvp_map.name);
            buffer::ptr tmp_buff(new buffer());
            BOOST_FOREACH(std::string& val, vector) {
                std::size_t delim_pos = val.find(';');
                if (std::string::npos != delim_pos) {
                    tmp_buff->value_add("map_key", val.substr(0, delim_pos));
                    tmp_buff->value_add("map_val", val.substr(delim_pos+1));
                    key_t key;
                    val_t val;
                    if (serialize(tmp_buff, make_nvp(key, "map_key"), def) && serialize(tmp_buff, make_nvp(val, "map_val"), def)) {
                        rezult.insert(std::make_pair(key, val));
                    }
                    tmp_buff->clear();
                }
            }
            if (!rezult.empty()) {
                nvp_map.value.swap(rezult);
                return true;
            }
        }
        else if (def_writing == def.def_type && buff) {
            assert(false);
        }
        return false;
    }

    // Сериализация мапа через потоки, у которого значение - костомный объект
    template <typename key_t, typename val_t, typename traits_t, typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<std::map<key_t, val_t, traits_t> > const& nvp_map, def_t<interface_type> const& def, typename boost::enable_if<is_streameble_tmp<key_t> >::type* = 0, typename boost::disable_if<is_streameble_tmp<val_t> >::type* = 0) {
        if (def_reading == def.def_type && buff && buff->children_is_exist(nvp_map.name)) {
            std::map<key_t, val_t, traits_t> rezult;
            buffer::children_vector_t children_vector = buff->children_get_all(nvp_map.name);
            BOOST_FOREACH(buffer::ptr const& map_item_buff_ptr, children_vector) {
                key_t key;
                val_t val;
                if (serialize(map_item_buff_ptr, make_nvp(key, "key"), def) && serialize(map_item_buff_ptr, make_nvp(val, "fuck"), def)) {
                    rezult.insert(std::make_pair(key, val));
                }
            }
            if (!rezult.empty()) {
                nvp_map.value.swap(rezult);
                return true;
            }
        }
        else if (def_writing == def.def_type && buff) {
            assert(false);
        }
        return false;
    }

    // Сериализация вектора через потоки, у которого значение - костомный объект
    template <typename T, typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<std::vector<T> > const& nvp_vector, def_t<interface_type> const& def, typename boost::disable_if<is_streameble_tmp<T> >::type* = 0) {
        if (def_reading == def.def_type && buff && buff->children_is_exist(nvp_vector.name)) {
            std::vector<T> rezult;
            buffer::children_vector_t children_vector = buff->children_get_all(nvp_vector.name);
            buffer::ptr tmp_buff_root(new buffer());
            T tmp_val;
            nvp<T> tmp_nvp = make_nvp(tmp_val, "dummy");
            buffer::ptr tmp_buff = tmp_buff_root->children_add(tmp_nvp.name);
            BOOST_FOREACH(buffer::ptr const& vector_item_buff_ptr, children_vector) {
                tmp_buff->values = vector_item_buff_ptr->values;
                tmp_buff->children = vector_item_buff_ptr->children;
                tmp_buff->parent = vector_item_buff_ptr->parent;
                T val;
                if (serialize(tmp_buff_root, make_nvp(val, "dummy"), def)) {
                    rezult.push_back(val);
                }
                tmp_buff->clear();
            }
            if (!rezult.empty()) {
                nvp_vector.value.swap(rezult);
                return true;
            }
        }
        else if (def_writing == def.def_type && buff) {
            assert(false);
        }
        return false;
    }

    // Сериализация сета через потоки, у которого значение - костомный объект
    template <typename T, typename interface_type>
    inline bool serialize(buffer::ptr const& buff, nvp<std::set<T> > const& nvp_set, def_t<interface_type> const& def, typename boost::disable_if<is_streameble_tmp<T> >::type* = 0) {
        if (def_reading == def.def_type && buff && buff->children_is_exist(nvp_set.name)) {
            std::set<T> rezult;
            buffer::children_vector_t children_vector = buff->children_get_all(nvp_set.name);
            buffer::ptr tmp_buff_root(new buffer());
            T tmp_val;
            nvp<T> tmp_nvp = make_nvp(tmp_val, "dummy");
            buffer::ptr tmp_buff = tmp_buff_root->children_add(tmp_nvp.name);
            BOOST_FOREACH(buffer::ptr const& vector_item_buff_ptr, children_vector) {
                tmp_buff->values = vector_item_buff_ptr->values;
                tmp_buff->children = vector_item_buff_ptr->children;
                tmp_buff->parent = vector_item_buff_ptr->parent;
                T val;
                if (serialize(tmp_buff_root, make_nvp(val, "dummy"), def)) {
                    rezult.insert(val);
                }
                tmp_buff->clear();
            }
            if (!rezult.empty()) {
                nvp_set.value.swap(rezult);
                return true;
            }
        }
        else if (def_writing == def.def_type && buff) {
            assert(false);
        }
        return false;
    }

} // namespace serialization {

#endif // SERIALIZATION_HPP
