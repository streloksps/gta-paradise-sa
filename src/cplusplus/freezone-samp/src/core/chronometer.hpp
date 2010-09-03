#ifndef CHRONOMETER_HPP
#define CHRONOMETER_HPP
#include <string>
#include <vector>
#include <set>
#include <boost/format.hpp>
#include "core/logger/logger.hpp"

class chronometer_manager_t;
class chronometer_t {
public:
    chronometer_t(std::string const& name);
    chronometer_t(std::string const& name, chronometer_manager_t& manager); // еще регистрируемся в менеджере
    ~chronometer_t();

    void set_dumb(bool is_dumb);
    std::string const& get_name();         

    void event_begin();
    void event_end();

    inline bool get_is_dumb() const {return is_dumb;}
    void event_dumb(boost::format const& params);
private:
    std::string name;
    bool is_dumb;
    logger::ptr log_ptr;
};

class chronometer_manager_t {
public:
    typedef std::set<std::string> dump_names_t;
    void add(chronometer_t* item);

public:
    void            dump_names_begin();
    dump_names_t&   dump_names_get();
    void            dump_names_end();
private:
    typedef std::vector<chronometer_t*> items_t;
    items_t items;
    dump_names_t dump_names;
};
#endif // CHRONOMETER_HPP
