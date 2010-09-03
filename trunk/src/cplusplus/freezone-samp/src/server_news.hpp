#ifndef SERVER_NEWS_HPP
#define SERVER_NEWS_HPP
#include "core/module_h.hpp"
#include <istream>
#include <ostream>
#include <vector>
#include <boost/algorithm/string.hpp>

struct server_news_item_t {
    bool is_public;
    std::string text;
    server_news_item_t(): is_public(true) {}
};

template <class char_t, class traits_t>
inline std::basic_istream<char_t, traits_t>& operator>>(std::basic_istream<char_t, traits_t>& is, server_news_item_t& item) {
    is>>item.is_public;

    if (is.fail() || is.eof()) {
        is.setstate(std::ios_base::failbit);
    }
    else {
        std::getline(is, item.text);
        boost::trim(item.text);
        if (item.text.empty()) {
            is.setstate(std::ios_base::failbit);
        }
    }
    return is;
}

#include "core/serialization/is_streameble.hpp"
namespace serialization {
    template <> struct is_streameble_read<server_news_item_t>: std::tr1::true_type {};
} // namespace serialization {

class server_news
    :public application_item
    ,public createble_i
    ,public configuradable
{
public:
    typedef std::tr1::shared_ptr<server_news> ptr;
    static ptr instance();

    server_news();
    virtual ~server_news();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(server_news);

private:
    typedef std::vector<server_news_item_t> news_t;
    news_t news;

    command_arguments_rezult cmd_news(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    static bool is_player_can_see(player_ptr_t const& player_ptr, server_news_item_t const& item);
};

#endif // SERVER_NEWS_HPP
