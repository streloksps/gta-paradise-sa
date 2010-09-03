#ifndef PLAYER_COLOR_HPP
#define PLAYER_COLOR_HPP
#include "player_color_t.hpp"
#include "core/module_h.hpp"

class player_color;
class player_color_item;

class player_color 
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
    ,public players_events::on_disconnect_i
{
public:
    typedef std::tr1::shared_ptr<player_color> ptr;
    static ptr instance();

public:
    player_color();
    virtual ~player_color();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();

public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_color);

public: // players_events::*
    virtual void on_connect(player_ptr_t const& player_ptr);
    virtual void on_disconnect(player_ptr_t const& player_ptr, samp::player_disconnect_reason reason);

private:
    friend class player_color_item;
    typedef std::vector<player_color_t> color_defs_t;

    color_defs_t    colors;
    int             max_players_for_stream_player_markers;
private:
    command_arguments_rezult cmd_color(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_color_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

private:
    bool last_is_stream;
    void update_stream_player_markers(int count_delta = 0);
};

class player_color_item
    :public player_item 
    ,public player_events::on_spawn_i
    ,public player_events::on_interior_change_i
    ,public player_events::on_request_class_i
{
public:
    typedef std::tr1::shared_ptr<player_color_item> ptr;

protected:
    friend class player_color;
    player_color_item(player_color& manager);
    virtual ~player_color_item();

private:
    player_color& manager;
    int color;
    std::size_t color_index;
    //bool color_is_custom;
    bool is_first_class;

    void set_color(int color);
    void set_color_index(std::size_t color_index);

public: // player_events::*
    virtual void on_interior_change(int interior_id_new, int interior_id_old);
    virtual void on_spawn();
    virtual void on_request_class(int class_id);

public:
    void show_marker();
    void hide_marker();

public:
    void init_color();
    unsigned int get_color() const;
    std::size_t get_color_index() const;
};

#endif // PLAYER_COLOR_HPP
