#include "config.hpp"
#include <unordered_set>
#include "samp_hook_events_network.hpp"
#include "samp_hook_events_i.hpp"
#include "core/container/container_handlers.hpp"
#include "core/application.hpp"
#include "core/time_outs.hpp"
#include "utility/base64.hpp"
#include "core/utility/locale_ru.hpp"

namespace samp {
    namespace {
        // Ќастройка
        static const std::time_t network_timeout_check_interval = 2;    // »нтервал, через сколько провер€ть на тайм аут
        //static const std::time_t network_timeout_dump_stat = 3600;      // »нтервал, через сколько секунд необходимо сбрасывать статистику используемых соединений

        struct network_point_t {
            unsigned int ip;
            unsigned short port;

            network_point_t(unsigned int ip, unsigned short port): ip(ip), port(port) {}        
            bool operator<(network_point_t const& right) const {
                if (ip == right.ip) return port < right.port;
                return ip < right.ip;
            }
        };

        struct network_val_t {
            time_base::millisecond_t    connect_time;
            unsigned int                data_send;
            unsigned int                data_recive;
            unsigned int                count_send;
            unsigned int                count_recive;
            network_val_t(time_base::millisecond_t connect_time, unsigned int data_send, unsigned int data_recive, unsigned int count_send, unsigned int count_recive)
                :connect_time(connect_time)
                ,data_send(data_send)
                ,data_recive(data_recive)
                ,count_send(count_send)
                ,count_recive(count_recive)
            { }
            void increment_send(unsigned int data_send_size) {
                data_send += data_send_size;
                ++count_send;
            }
            void increment_recive(unsigned int data_recive_size) {
                data_recive += data_recive_size;
                ++count_recive;
            }
        };
        
        std::string const valid_rcon_chars = "јаЅб¬в√гƒд≈е®Є∆ж«з»и…й кЋлћмЌнќоѕп–р—с“т”у‘ф’х÷ц„чЎшўщЏъџы№ьЁэёюя€AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890~`.,:;{}!@#$%^*()-=_+? ";

        time_outs<network_point_t, network_val_t> points(10500); // ¬ рак-нет тайм аут 10 секунд
        std::time_t last_process_disconnect = 0;

        // ѕодсчет статистики
        time_outs<unsigned int, network_val_t>  networt_statistics(30000);
        static const std::time_t                networt_statistics_timeout_check_interval = 10;    // »нтервал, через сколько провер€ть на тайм аут
        std::time_t                             networt_statistics_last_process = 0;


    } // namespace {

    static const std::time_t    profile_network_cpu_timeout_check_interval = 15; // »нтервал, через сколько секунд обновл€ть статистику по нагрузке на проц
    std::time_t                 profile_network_cpu_last_process = 0;
    profile_cpu                 profile_network_cpu;

    void on_disconect(network_point_t const& point, time_base::millisecond_t const& last_update, network_val_t const& network_val) {
        container_execute_handlers(application::instance(), &on_network_disconnect_i::on_network_disconnect, point.ip, point.port, (last_update - network_val.connect_time) / 1000L, network_val.data_send, network_val.data_recive);
    }

    void on_networt_statistics(unsigned int const& ip, time_base::millisecond_t const& last_update, network_val_t const& network_val) {
        container_execute_handlers(application::instance(), &on_network_statistics_i::on_network_statistics, ip, (last_update - network_val.connect_time) / 1000L, network_val.data_send, network_val.data_recive, network_val.count_send, network_val.count_recive);
    }

    // ¬ызываетс€ при приеме покета по сети
    void on_recive_network_data_from(unsigned int ip, unsigned short port, unsigned int data_size) {
        network_point_t point(ip, port);

        if (points.refresh_if_exist(point)) {
            points.get(point).increment_recive(data_size);
        }
        else {
            points.insert(&on_disconect, point, network_val_t(time_base::tick_count_milliseconds(), 0, data_size, 0, 1));
            container_execute_handlers(application::instance(), &on_network_connect_i::on_network_connect, point.ip, point.port);
        }
    }

    void on_network_data_cleanup() {
        std::time_t curr_time = std::time(0);

        // ќбрабатываем отключившихс€
        if (curr_time - last_process_disconnect >= network_timeout_check_interval) {
            points.erase_timeouts(&on_disconect);
            last_process_disconnect = curr_time;
        }

        // ќбрабатываем отключившихс€
        if (curr_time - networt_statistics_last_process >= networt_statistics_timeout_check_interval) {
            networt_statistics.erase_timeouts(&on_networt_statistics);
            networt_statistics_last_process = curr_time;
        }

        // ќбрабатываем статистику
        if (curr_time - profile_network_cpu_last_process >= profile_network_cpu_timeout_check_interval) {
            profile_network_cpu.update();
            profile_network_cpu_last_process = curr_time;
        }
    }

    void on_send_network_data_to(unsigned int ip, unsigned short port, unsigned int data_size) {
        network_point_t point(ip, port);

        if (points.is_exist(point)) {
            points.get(point).increment_send(data_size);
        }
    }

    void on_recive_network_data_from_fast(unsigned int ip, unsigned int data_size) {
        if (networt_statistics.refresh_if_exist(ip)) {
            networt_statistics.get(ip).increment_recive(data_size);
        }
        else {
            networt_statistics.insert(&on_networt_statistics, ip, network_val_t(time_base::tick_count_milliseconds(), 0, data_size, 0, 1));
        }
    }

    void on_send_network_data_to_fast(unsigned int ip, unsigned int data_size) {
        if (networt_statistics.is_exist(ip)) {
            networt_statistics.get(ip).increment_send(data_size);
        }
    }

    void on_shutdown() {
        points.erase_all(&on_disconect);
        networt_statistics.erase_all(&on_networt_statistics);
    }

    // ¬ыпарсивает строку
    inline bool parse_string(char* buffer, int len, int& pos, std::string& str, int len_max, std::string const& valid_chars, bool is_conver_from_cp866_to_cp1251) {
        int length = buffer[pos] | static_cast<int>(static_cast<unsigned char>(buffer[pos + 1])) << 8;
        if (1 > length || len_max <= length) {
            return false;
        }
        int pos_begin = pos + 2;
        int pos_end = pos + 2 + length;
        if (pos_end > len || pos_end <= 0 || pos_begin > pos_end) {
            return false;
        }
        for (char* it = buffer + pos_begin; it < buffer + pos_end; ++it) {
            if (is_conver_from_cp866_to_cp1251) {
                *it = cp866_to_cp1251(*it);
            }
            if (std::string::npos == valid_chars.find(*it)) {
                return false;
            }
        }
        str = std::string(buffer + pos_begin, buffer + pos_end);
        pos += 2 + length;
        return true;
    }

    // »щем среди вход€щих пакетов команды дл€ ркона
    bool on_recive_network_hook_rcon_cmd(char* buffer, int len, unsigned int ip, unsigned short port) {
        if (13 < len && 'S' == buffer[0] && 'A' == buffer[1] && 'M' == buffer[2] && 'P' == buffer[3] && 'x' == buffer[10]) {
            // ¬озможно rcon команда
            int pos = 11;
            std::string password;
            std::string cmd;
            if (!parse_string(buffer, len, pos, password, 32, valid_rcon_chars, false)
                || !parse_string(buffer, len, pos, cmd, 144, valid_rcon_chars, true)) {
                    container_execute_handlers(application::instance(), &on_network_rcon_external_not_parse_i::on_network_rcon_external_not_parse, ip, base64_encode(reinterpret_cast<unsigned char const*>(buffer), len));
                    return false;
            }
            container_execute_handlers(application::instance(), &on_network_rcon_external_i::on_network_rcon_external, ip, password, cmd);
        }
        return true;
    }

    bool on_recive_network_is_brief_description(char* buffer, int len) {
        if (11 == len && 'S' == buffer[0] && 'A' == buffer[1] && 'M' == buffer[2] && 'P' == buffer[3] && 'c' == buffer[10]) {
            return true;
        }
        return false;
    }

    bool on_recive_network_is_connection_request(char const* buffer, int len) {
        return 3 == len && 0x07 == buffer[0] && 0x17 == buffer[1]/* && 0x61 == buffer[2] ѕоследний байт зависит от порта= (port & 0xff)*/;
    }

    on_network_connection_request_action on_recive_network_connection_request_get_response(unsigned int ip) {
        on_network_connection_request_action action = on_network_connection_request_action_common_process;
        container_execute_handlers(application::instance(), &on_network_connection_request_i::on_network_connection_request, ip, &action);        
        return action;
    }

    bool on_send_network_hook_rcon_cmd(char* buffer, int len, unsigned int ip, unsigned short port) {
        if (13 < len && 'S' == buffer[0] && 'A' == buffer[1] && 'M' == buffer[2] && 'P' == buffer[3] && 'x' == buffer[10]) {
            // ¬озможно rcon отклик
            int pos = 11;
            int length = buffer[pos] | static_cast<int>(static_cast<unsigned char>(buffer[pos + 1])) << 8;

            int pos_begin = pos + 2;
            int pos_end = pos + 2 + length;
            for (char* it = buffer + pos_begin; it < buffer + pos_end && it < buffer + len; ++it) {
                *it = cp1251_to_cp866(*it);
            }
        }
        return true;
    }
} // namespace samp {



#include <boost/thread/mutex.hpp>
namespace samp {
    // ћодуль блокировки сетевой активности по ип адресу
    namespace {
        typedef std::tr1::unordered_set<unsigned int> network_activity_blocks_t;
        
        network_activity_blocks_t network_activity_blocks;
        boost::mutex              network_activity_mutex;
    }
    bool on_network_activity(unsigned int ip) {
        boost::mutex::scoped_lock lock(network_activity_mutex);
        return network_activity_blocks.end() == network_activity_blocks.find(ip);
    }

    void network_activity_block(unsigned int ip) {
        boost::mutex::scoped_lock lock(network_activity_mutex);
        network_activity_blocks.insert(ip);
    }

    void network_activity_unblock(unsigned int ip) {
        boost::mutex::scoped_lock lock(network_activity_mutex);
        network_activity_blocks.erase(ip);
    }

} // namespace samp {

#include "core/logger/logger.hpp"
#include "core/geo/geo_ip_info.hpp"
#include <boost/format.hpp>
namespace samp {
    void on_network_three_chars(unsigned int ip, unsigned short port, char c1, char c2, char c3) {
        logger::instance()->debug("guard/network/three_chars", (boost::format("%1%:%2% %3% %4% %5%") % geo::get_ip_string(ip) % port % static_cast<int>(c1) % static_cast<int>(c2) % static_cast<int>(c3)).str());
    }
} // namespace samp {
/*
»нформаци€
≈сли изменить lan=1 на lan=0 то вход€щий/исход€щий трафик уменьшитьс€ в 1.6 раза

*/
