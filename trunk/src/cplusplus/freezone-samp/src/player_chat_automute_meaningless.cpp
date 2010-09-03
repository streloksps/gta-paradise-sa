#include "config.hpp"
#include "player_chat_automute_meaningless.hpp"
#include "player_chat.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <map>

player_chat_automute_meaningless::player_chat_automute_meaningless()
:is_active(false)
{
}

player_chat_automute_meaningless::~player_chat_automute_meaningless() {

}

void player_chat_automute_meaningless::configure_pre() {
    is_active = false;
    chars_types.clear();
    chars_type_precompile.clear();

    mute_frequency = 70;
    mute_min_metric = 15.0f;
    mute_min_len = 6;
    mute_metric_to_time = 0.2f;
    mute_time_min = 5;
    mute_time_max = 30;
}

void player_chat_automute_meaningless::configure(buffer::ptr const& buff, def_t const& def) {
    configure_pre();

    SERIALIZE_ITEM(is_active);
    SERIALIZE_ITEM(chars_types);
    SERIALIZE_ITEM(mute_frequency);
    SERIALIZE_ITEM(mute_min_metric);
    SERIALIZE_ITEM(mute_min_len);
    SERIALIZE_ITEM(mute_metric_to_time);
    SERIALIZE_ITEM(mute_time_min);
    SERIALIZE_ITEM(mute_time_max);

    configure_post();
}

void player_chat_automute_meaningless::configure_post() {
    for (chars_types_t::const_iterator it = chars_types.begin(), end = chars_types.end(); it != end; ++it) {
        player_chat_automute_meaningless_chars_type_t const* ptr = &(*it);
        BOOST_FOREACH(char c, ptr->chars) {
            chars_type_precompile[c] = ptr;
        }
    }

#   ifndef NDEBUG
    // В режиме отладки прогоняем список
    test();
#   endif //#ifndef NDEBUG
}

float player_chat_automute_meaningless::get_metric(std::string const& msg) const {
    float rezult = 0.0f;

    typedef std::map<char, std::size_t> chars_count_t;

    chars_count_t chars_count;
    BOOST_FOREACH(char c, msg) {
        ++chars_count[c];
    }

    BOOST_FOREACH(chars_count_t::value_type const& pair, chars_count) {
        player_chat_automute_meaningless_chars_type_t const* char_type_ptr = 0;
        int     char_count = pair.second;
        float   char_relative_count = static_cast<float>(char_count)/static_cast<float>(msg.length());
        {
            chars_type_precompile_t::const_iterator it = chars_type_precompile.find(pair.first);
            if (chars_type_precompile.end() != it) {
                char_type_ptr = it->second;
            }
            else {
                continue;
            }
        }
        if (-1 != char_type_ptr->min_count && char_count <= char_type_ptr->min_count) {
            continue;
        }
        if (-1.0f != char_type_ptr->min_relative_count && char_relative_count <= char_type_ptr->min_relative_count) {
            continue;
        }
        float metric = char_count * char_type_ptr->mentic_factor;
        rezult += metric;
    }
    return rezult;
}

void player_chat_automute_meaningless::automute_msg(player_ptr_t const& player_ptr, std::string const& msg) const {
    if (!is_active) {
        return;
    }
    if (static_cast<int>(msg.length()) <= mute_min_len) {
        // Недостаточная длина сообщения для автомута
        return;
    }
    float metric = get_metric(msg);
    if (metric <= mute_min_metric) {
        // Недостаточная метрика для мута
        return;
    }
    
    int mute_time = static_cast<int>(mute_metric_to_time * metric);
    if (mute_time_min > mute_time) {
        mute_time = mute_time_min;
    }
    if (mute_time_max < mute_time) {
        mute_time = mute_time_max;
    }

    { // Игрока можно мутить :)
        messages_item msg_item;
        msg_item.get_params()
            ("player_name", player_ptr->name_get_full())
            ("metric", (boost::format("%1$.1f") % metric).str())
            ("total_time", mute_time)
            ("msg", msg);

        if ((std::rand() % 100) < mute_frequency) {
            // Мутить будем
            int mute_in_time = player_ptr->get_item<player_chat_item>()->automute_set(mute_time);
            msg_item.get_params()("mute_in_time", mute_in_time);
            msg_item.get_sender()
                ("$(automute_meaningless_to_mute_admins)", msg_players_all_admins)
                ("$(automute_meaningless_to_mute_log)", msg_log);
        }
        else {
            // Мутить не будем - пусть живет
            msg_item.get_sender()
                ("$(automute_meaningless_no_mute_admins)", msg_players_all_admins)
                ("$(automute_meaningless_no_mute_log)", msg_log);
        }
    }
}

void player_chat_automute_meaningless::test() const {
    // Данные для тестирования по логам: cat /cygdrive/c/_logs/sa/log/2010/02/* | grep '<chat/msg>' | perl -pe 's/^.+\) '"'"'(.+)'"'"'$/${1}/' > /cygdrive/c/meaningless_in.txt
    std::ifstream in("c:\\meaningless_in.txt", std::ios_base::binary);
    if (in) {
        std::ofstream out("c:\\meaningless_out.txt", std::ios_base::binary);
        if (out) {
            for (;!in.eof();) {
                std::string file_line;
                getline(in, file_line);
                float metric = get_metric(file_line);
                out << metric << " " << file_line << std::endl;
            }
        }
    }
}
