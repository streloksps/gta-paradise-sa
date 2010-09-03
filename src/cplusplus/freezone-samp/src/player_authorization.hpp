#ifndef PLAYER_AUTHORIZATION_HPP
#define PLAYER_AUTHORIZATION_HPP
#include "core/module_h.hpp"

class player_authorization
    :public application_item
    ,public createble_i
    ,public players_events::on_connect_i
{
public:
    player_authorization();
    virtual ~player_authorization();

public: // createble_i
    virtual void create();

public: // players_events::on_connect_i
    virtual void on_connect(player_ptr_t const& player_ptr);
};

class player_authorization_item
    :public player_item
    ,public player_events::on_connect_i
    ,public player_events::on_request_class_i
{
public:
    typedef std::tr1::shared_ptr<player_authorization_item> ptr;

private:
    friend class player_authorization;
    player_authorization_item();
    virtual ~player_authorization_item();

private:
    bool is_first_request_class;

public: // player_events::*
    virtual void on_connect();
    virtual void on_request_class(int class_id);

private:
    void on_first_request_class();
};

#endif // PLAYER_AUTHORIZATION_HPP
