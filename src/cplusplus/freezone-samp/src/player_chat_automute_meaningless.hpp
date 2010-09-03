#ifndef PLAYER_CHAT_AUTOMUTE_MEANINGLESS_HPP
#define PLAYER_CHAT_AUTOMUTE_MEANINGLESS_HPP
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include "core/module_h.hpp"
#include "player_chat_automute_meaningless_t.hpp"
#include <map>

class player_chat_item;
class player_chat_automute_meaningless
    :boost::noncopyable
    ,public configuradable
{
public:
    player_chat_automute_meaningless();
    virtual ~player_chat_automute_meaningless();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(automute_meaningless);

private: // Параметры настройки
    typedef std::vector<player_chat_automute_meaningless_chars_type_t> chars_types_t;

    bool            is_active;
    chars_types_t   chars_types;
    int             mute_frequency;
    float           mute_min_metric;
    int             mute_min_len;
    float           mute_metric_to_time;
    int             mute_time_min;
    int             mute_time_max;
public:
    void automute_msg(player_ptr_t const& player_ptr, std::string const& msg) const;
    float get_metric(std::string const& msg) const;

private:
    typedef std::map<char, player_chat_automute_meaningless_chars_type_t const*> chars_type_precompile_t;
    chars_type_precompile_t chars_type_precompile; // Прекомпиляция для ускорения поиска
private:
    void test() const;
};
#endif // PLAYER_CHAT_AUTOMUTE_MEANINGLESS_HPP
