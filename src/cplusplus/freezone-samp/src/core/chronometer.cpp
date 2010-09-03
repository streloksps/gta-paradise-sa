#include "config.hpp"
#include "chronometer.hpp"
#include <boost/foreach.hpp>

chronometer_t::chronometer_t(std::string const& name): name(name), is_dumb(false) {
}

chronometer_t::chronometer_t(std::string const& name, chronometer_manager_t& manager): name(name), is_dumb(false) {
    manager.add(this);
}

chronometer_t::~chronometer_t() {
}

void chronometer_t::set_dumb(bool is_dumb) {
    this->is_dumb = is_dumb;
}

void chronometer_t::event_begin() {
}

void chronometer_t::event_end() {
}

void chronometer_t::event_dumb(boost::format const& params) {
    if (is_dumb) {
        if (!log_ptr) {
            log_ptr = logger::instance();
        }
        if (log_ptr) {
            log_ptr->debug("raw/begin/" + name, params.str());
        }
    }
}

std::string const& chronometer_t::get_name() {
    return name;
}

void chronometer_manager_t::add(chronometer_t* item) {
    items.push_back(item);
}

void chronometer_manager_t::dump_names_begin() {
    dump_names.clear();
}

chronometer_manager_t::dump_names_t& chronometer_manager_t::dump_names_get() {
    return dump_names;
}

void chronometer_manager_t::dump_names_end() {
    BOOST_FOREACH(chronometer_t* item, items) {
        item->set_dumb(dump_names.end() != dump_names.find(item->get_name()));
    }
}
