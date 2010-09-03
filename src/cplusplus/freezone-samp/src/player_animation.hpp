#ifndef PLAYER_ANIMATION_HPP
#define PLAYER_ANIMATION_HPP
#include "core/module_h.hpp"
#include "player_animation_t.hpp"

class player_animation
    :public application_item
    ,public createble_i
    ,public configuradable
    ,public players_events::on_connect_i
{
public:
    player_animation();
    virtual ~player_animation();

public: // createble_i
    virtual void create();

public: // configuradable
    virtual void configure_pre();
    virtual void configure(buffer::ptr const& buff, def_t const& def);
    virtual void configure_post();
public: // auto_name
    SERIALIZE_MAKE_CLASS_NAME(player_animation);

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);

private:
    command_arguments_rezult cmd_animation(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_animation_list(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    command_arguments_rezult cmd_special_dance(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_special_handsup(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_special_phone(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_special_pissing(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);

    // Начиная с 0.3
    command_arguments_rezult cmd_special_drink(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
    command_arguments_rezult cmd_special_smoke(player_ptr_t const& player_ptr, std::string const& arguments, messages_pager& pager, messages_sender const& sender, messages_params& params);
private:
    friend class player_animation_item;
    animation_libs_t animation_libs;
    int get_total_anims() const;

    bool is_block_on_jetpack;
public:
    void special_phone_start_impl(player_ptr_t const& player_ptr, messages_pager& pager, messages_sender const& sender);
    void special_phone_stop_impl(player_ptr_t const& player_ptr, messages_pager& pager, messages_sender const& sender);
};

class player_animation_item
    :public player_item
    ,public on_timer1000_i
    ,public player_events::on_request_class_i
    ,public player_events::on_keystate_change_i
    ,public player_events::on_update_i
{
public:
    typedef std::tr1::shared_ptr<player_animation_item> ptr;

protected:
    friend class player_animation;
    player_animation_item(player_animation& manager);
    virtual ~player_animation_item();

public: // player_events::*
    virtual void on_request_class(int class_id);
    virtual void on_keystate_change(int keys_new, int keys_old);
    virtual void on_timer1000();
    virtual bool on_update();

private:
    player_animation& manager;
    int is_can_preload;
    bool is_first;
    void preload_libs();
};

#endif // PLAYER_ANIMATION_HPP

