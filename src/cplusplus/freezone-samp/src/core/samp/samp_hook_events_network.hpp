#ifndef SAMP_HOOK_EVENTS_NETWORK_HPP
#define SAMP_HOOK_EVENTS_NETWORK_HPP
#include "profile_cpu.hpp"
#include "samp_hook_events_t.hpp"

namespace samp {
    void on_recive_network_data_from(unsigned int ip, unsigned short port, unsigned int data_size);
    void on_send_network_data_to(unsigned int ip, unsigned short port, unsigned int data_size);

    void on_recive_network_data_from_fast(unsigned int ip, unsigned int data_size);
    void on_send_network_data_to_fast(unsigned int ip, unsigned int data_size);

    void on_network_data_cleanup(); // Метод для обновления состояния, вызывается редко

    // Вернуть истину = не блокировать пакет
    bool on_recive_network_hook_rcon_cmd(char* buffer, int len, unsigned int ip, unsigned short port);
    bool on_send_network_hook_rcon_cmd(char* buffer, int len, unsigned int ip, unsigned short port);

    // Возращает истину, если у нас запрос на список игроков
    bool on_recive_network_is_brief_description(char* buffer, int len);

    // Возращает истину, если у нас запрос на подключение
    bool on_recive_network_is_connection_request(char const* buffer, int len);
    on_network_connection_request_action on_recive_network_connection_request_get_response(unsigned int ip);

    void on_network_three_chars(unsigned int ip, unsigned short port, char c1, char c2, char c3);
        
    bool on_network_activity(unsigned int ip); // Истина не блокировать: используется для бана ип адресов
    void network_activity_block(unsigned int ip);
    void network_activity_unblock(unsigned int ip);

    // Вызывается при остановке приложения
    void on_shutdown();

    extern profile_cpu profile_network_cpu;
} // namespace samp {

#endif // SAMP_HOOK_EVENTS_NETWORK_HPP
