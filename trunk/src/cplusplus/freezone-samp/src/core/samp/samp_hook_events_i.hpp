#ifndef SAMP_HOOK_EVENTS_I_HPP
#define SAMP_HOOK_EVENTS_I_HPP
#include "samp_hook_events_t.hpp"
#include <string>
#include <ctime>

namespace samp {
    // ��������������: ��� ������� ���������� � ������ �����
    // ��������� ������� ����� � ��������� �������
    struct on_bad_rcon_external_i {
        virtual void on_bad_rcon_external(std::string const& ip_string) = 0;
    };

    // ��������� ������� ����� � ���� � ����
    struct on_bad_rcon_local_i {
        virtual void on_bad_rcon_local(int player_id, std::string const& password) = 0;
    };

    // ��� ������� ������ ������� - �� ����� ������� ��� �� �������
    struct on_network_rcon_external_i {
        virtual void on_network_rcon_external(unsigned int ip, std::string const& password, std::string const& cmd) = 0;
    };

    // ��� ����������� ����������� ���� �������
    struct on_network_rcon_external_not_parse_i {
        virtual void on_network_rcon_external_not_parse(unsigned int ip, std::string const& raw_base64) = 0;
    };

    // ���������� ��� ����������� ������� �� ���� - ��� ����� �����������
    struct on_network_connect_i {
        virtual void on_network_connect(unsigned int ip, unsigned short port) = 0;
    };

    // ���������� ��� ���������� �������
    struct on_network_disconnect_i {
        virtual void on_network_disconnect(unsigned int ip, unsigned short port, std::time_t connection_time, unsigned int data_send, unsigned int data_recive) = 0;
    };

    // ���������� ��� ���������� �������
    struct on_network_statistics_i {
        virtual void on_network_statistics(unsigned int ip, std::time_t connection_time, unsigned int data_send, unsigned int data_recive, unsigned int count_send, unsigned int count_recive) = 0;
    };

    struct on_network_connection_request_i {
        virtual void on_network_connection_request(unsigned int ip, on_network_connection_request_action* action) = 0;
    };
} // namespace samp {
#endif // SAMP_HOOK_EVENTS_I_HPP
