#include "config.hpp"
#include "rules.hpp"
#include "core/messages/messages_storage.hpp"
#include "core/utility/locale_ru.hpp"
#include <boost/algorithm/string.hpp>

std::string get_rules_string(std::string const& src) {
    buffer::ptr buff = messages_storage::instance()->get_msg_buff()->children_create_connected();
    std::string prefix = buff->process_all_vars("$(comment_rules_prefix)");
    if (boost::istarts_with(src, prefix, locale_ru)) {
        std::string index = src.substr(prefix.length());
        std::string test_rule = "$(rule" + index + ")";
        std::string rule_text = buff->process_all_vars(test_rule);
        if (rule_text != test_rule) {
            // Такое правило найдено
            buff->value_replace("rule_nn", index);
            buff->value_replace("rule", rule_text);
            return buff->process_all_vars("$(comment_rules_text)");
        }
    }
    return src;
}
