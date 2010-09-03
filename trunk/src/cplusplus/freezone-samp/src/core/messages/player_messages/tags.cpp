#include "config.hpp"
#include "tags.hpp"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "core/utility/strings.hpp"

namespace {
    char close_tag_prefix[] = "<";
    char close_tag_siffix[] = "/>";

    bool find_close_tag(std::string const& str, std::string& content, size_t& pos_open, size_t& pos_close) {
        size_t p_open = str.find(close_tag_prefix);
        if (std::string::npos == p_open) {
            return false;
        }
        size_t p_close = str.find(close_tag_siffix, p_open);
        if (std::string::npos == p_close) {
            return false;
        }
        pos_open = p_open;
        pos_close = p_close + sizeof(close_tag_siffix)/sizeof(close_tag_siffix[0]) - 1;
        content = str.substr(p_open + sizeof(close_tag_prefix)/sizeof(close_tag_prefix[0]) - 1, p_close - (p_open + sizeof(close_tag_prefix)/sizeof(close_tag_prefix[0]) - 1));
        return true;
    }

    void parse_content(std::string const& content, std::string& name, std::string& val) {
        size_t pos = content.find(' ');
        if (std::string::npos == pos) {
            name = boost::trim_copy(content);
            val = "";
        }
        else {
            name = boost::trim_copy(content.substr(0, pos));
            val = boost::trim_copy(content.substr(pos + 1));
            process_starts_underline(val);
            process_ends_underline(val);
        }
    }
}

tags::tags(std::string const& str):src_str(str), rezult_str(str) {
    process_closed_tags();
}

std::string tags::get_untaget_str() const {
    return rezult_str;
}

void tags::process_closed_tags() {
    size_t work_pos = 0;
    for(;;) {
        std::string content;
        size_t p_open, p_close;
        if (find_close_tag(rezult_str, content, p_open, p_close)) {
            std::string name, val;
            parse_content(content, name, val);
            closed_tag_items_t& items = closed_tags[name];
            if (!items.empty()) {
                items.back().end = p_open;
            }
            if ("end" != val) {
                // Если таг не закрывающийся - то дабвляем его
                items.push_back(closed_tag_item(val, p_open));
            }
            rezult_str = rezult_str.substr(0, p_open) + rezult_str.substr(p_close);
        }
        else {
            break;
        }
    }
}

template<>
std::string tags::get_closed_tag_val(std::string const& tag_name, size_t pos, std::string const& def_val) const {
    closed_tags_t::const_iterator p = closed_tags.find(tag_name);
    if (closed_tags.end() != p) {
        // нашли таг
        for (closed_tag_items_t::const_iterator item_p = p->second.begin(), item_end = p->second.end(); item_p != item_end; ++item_p) {
            if (item_p->start <= pos && item_p->end > pos) {
                return item_p->val;
            }
        }
    }
    return def_val;
}

template<>
unsigned int tags::get_closed_tag_val(std::string const& tag_name, size_t pos, unsigned int const& def_val) const {
    std::ostringstream ostr;
    ostr << def_val;
    std::string rezult_str = get_closed_tag_val<std::string>(tag_name, pos, ostr.str());
    std::istringstream istr(rezult_str);
    istr.unsetf(std::ios_base::dec);
    unsigned int rezult(0);
    istr >> rezult;
    return rezult;
}

template<>
int tags::get_closed_tag_val(std::string const& tag_name, size_t pos, int const& def_val) const {
    std::ostringstream ostr;
    ostr << def_val;
    std::string rezult_str = get_closed_tag_val<std::string>(tag_name, pos, ostr.str());
    std::istringstream istr(rezult_str);
    istr.unsetf(std::ios_base::dec);
    int rezult(0);
    istr >> rezult;
    return rezult;
}

template<>
bool tags::get_closed_tag_val(std::string const& tag_name, size_t pos, bool const& def_val) const {
    std::ostringstream ostr;
    ostr << def_val;
    std::string rezult_str = get_closed_tag_val<std::string>(tag_name, pos, ostr.str());
    std::istringstream istr(rezult_str);
    istr.setf(std::ios_base::boolalpha);
    bool rezult(false);
    istr >> rezult;
    return rezult;
}

template<>
float tags::get_closed_tag_val(std::string const& tag_name, size_t pos, float const& def_val) const {
    std::ostringstream ostr;
    ostr << def_val;
    std::string rezult_str = get_closed_tag_val<std::string>(tag_name, pos, ostr.str());

    std::istringstream istr(rezult_str);
    float rezult;
    istr>>rezult;
    return rezult;
}

tags::poses_t tags::get_tags_poses() const {
    poses_t rezult;
    for (closed_tags_t::const_iterator closed_tags_it = closed_tags.begin(); closed_tags.end() != closed_tags_it; ++closed_tags_it ) {
        for (closed_tag_items_t::const_iterator closed_tag_items_it = closed_tags_it->second.begin(), closed_tag_items_end = closed_tags_it->second.end(); closed_tag_items_end != closed_tag_items_it; ++closed_tag_items_it) {
            rezult.insert(closed_tag_items_it->start);
        }
    }
    return rezult;
}
