#include "config.hpp"
#include "samp_hook_events.hpp"
#include "samp_hook_events_i.hpp"
#include <cassert>
#include "core/application.hpp"
#include "core/container/container_handlers.hpp"
#include "pawn/pawn_log.hpp"
#include <set>
#include "core/utility/path_works.hpp"
#include "hashlib2plus/hl_sha1wrapper.h"
#include "samp_hook_events_addresses.hpp"

// Platform-dependant includes
#ifdef WIN32
#   include <windows.h>
    enum {is_has_win32 = true};

    // Сокеты
#   define API_CALL __stdcall
    typedef int socklen_t;
#else // #ifdef WIN32
#   include <sys/mman.h>
#   include <limits.h>

#   ifndef PAGESIZE
#       define PAGESIZE 4096
#   endif // ifndef PAGESIZE

    typedef unsigned long DWORD;
    enum {is_has_win32 = false};

    // Сокеты
#   include <sys/types.h> 
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <unistd.h>
#   define API_CALL
    typedef int SOCKET;

#   ifndef SOCKET_ERROR
#       define SOCKET_ERROR (-1)
#   endif

#endif // #ifdef WIN32

#include "samp_hook_events_network.hpp"

namespace samp {
    REGISTER_IN_APPLICATION(hook_events);

    hook_events::ptr hook_events::instance() {
        return application::instance()->get_item<hook_events>();
    }
    namespace {
        //Заменяет вызов функции на наш. Возращает истину, если смог заменить
        bool overwrite_instruction(DWORD address, char const* prefix, size_t prefix_size, DWORD new_value) {
            bool rezult = false;
#           ifdef WIN32
            DWORD d, ds;
            VirtualProtect((LPVOID)address, sizeof(new_value) + prefix_size, PAGE_EXECUTE_READWRITE, &d);
#           else // ifdef WIN32
            // it's a bit tricky for linux
            int r = address/PAGESIZE;
            char* p = (char *)(r*PAGESIZE);
            if(!mprotect( p, PAGESIZE, PROT_WRITE | PROT_READ | PROT_EXEC ) ) {
#           endif // ifdef WIN32
                bool prefix_is_ok = true;
                for (size_t i = 0; i < prefix_size; ++i) {
                    if (prefix[i] != ((char const*)address)[i]) {
                        prefix_is_ok = false;
                    }
                }
                if (prefix_is_ok) {
                    *(DWORD*)(address + prefix_size) = new_value;
                    rezult = true;
                }
#           ifdef WIN32
                VirtualProtect((LPVOID)address, sizeof(new_value) + prefix_size, d, &ds);
#           else // ifdef WIN32
            }
#           endif // ifdef WIN32
            if (!rezult) {// Пишем диагностику в случае ошибки
                pawn::logprintf("Overwrite instruction error address: 0x%X, preffix_size: %d", address, prefix_size);
            }
            return rezult;
        }

        inline bool overwrite_E8(DWORD address, DWORD handler_address) {
            // E8 - вызов с относительным адресом
            return overwrite_instruction(address, "\xE8", 1, handler_address - address - 5);
        }

        inline bool overwrite_FF15(DWORD address, DWORD& buffer_with_handler_address) {
            // FF15 - косвенный вызов статически линкуемой функции приводит к тому, что первые 4 байта функции интерпретируются как указатель
            return overwrite_instruction(address, "\xFF\x15", 2, (DWORD)&buffer_with_handler_address);
        }

        inline bool overwrite_8B1D(DWORD address, DWORD& buffer_with_handler_address) {
            return overwrite_instruction(address, "\x8B\x1D", 2, (DWORD)&buffer_with_handler_address);
        }

        // Устанавливаем хуки, в случае успеха возращаем истину
        bool setup_hooks(samp_hook_t const& samp_hook) {
            if (reinterpret_cast<address_t>(pawn::logprintf) != samp_hook.printf) {
                pawn::logprintf("printf address error");
                return false;
            }

            for (replace_pairs_t::const_iterator e8_it = samp_hook.E8.begin(), e8_end = samp_hook.E8.end(); e8_it != e8_end; ++e8_it) {
                if (!overwrite_E8(e8_it->from, *e8_it->to)) {
                    return false;
                }
            }
            for (replace_pairs_t::const_iterator ff15_it = samp_hook.FF15.begin(), ff15_end = samp_hook.FF15.end(); ff15_it != ff15_end; ++ff15_it) {
                if (!overwrite_FF15(ff15_it->from, *ff15_it->to)) {
                    return false;
                }
            }
            for (replace_pairs_t::const_iterator _8b1d_it = samp_hook._8B1D.begin(), _8b1d_end = samp_hook._8B1D.end(); _8b1d_it != _8b1d_end; ++_8b1d_it) {
                if (!overwrite_8B1D(_8b1d_it->from, *_8b1d_it->to)) {
                    return false;
                }
            }
            return true;
        }
    }

    namespace handlers { // Новые обработчики
        // Вызывается при неудачном вводе пароля в удаленную консоль
        void on_bad_rcon_external(char const* format, char const* ip) {
            if (pawn::logprintf) {
                //Пишем лог - делаем то, что должна данная функция
                pawn::logprintf(format, ip);

                //Вызываем наш обработчик
                container_execute_handlers(application::instance(), &on_bad_rcon_external_i::on_bad_rcon_external, std::string(ip));
            }
        }

        // Вызывается при неудачной авторизации в локальном рконе
        void on_bad_rcon_local(char const* format, int player_id, char const* name, char const* password) {
            if (pawn::logprintf) {
                //Пишем лог - делаем то, что должна данная функция
                pawn::logprintf(format, player_id, name, password);

                //Вызываем наш обработчик
                container_execute_handlers(application::instance(), &on_bad_rcon_local_i::on_bad_rcon_local, player_id, std::string(password));
            }
        }

        // Вызывается при переименовании игрока
        void on_player_rename(char const* format, char const* name_old, char const* name_new) {
            // Просто блокируем флуд в лог о переименовании
        }

        // Вызывается при вводе сообщения в чат
        void on_player_chat(char const* format, char const* player_nick, char const* message) {
            // Просто блокируем флуд в лог о переименовании
        }

        // Вызывается когда самп хочет проверить ник на валидность
        bool is_bad_nick(char const* nick) {
            return false;
        }

        int API_CALL on_recvfrom(SOCKET s, char* buf, int len, int flags, sockaddr_in* from, socklen_t* fromlen) {
            int recv_size = recvfrom(s, buf, len, flags, (sockaddr*)from, fromlen);
            if (!hook_events::is_hook_network) {
                // Запрещена обработка
                return recv_size;
            }
            
            { // Вызываем очистку
                static int counter = 0;
                ++counter;
                if (500 == counter) {
                    on_network_data_cleanup();
                    counter = 0;
                }
            }
            
            if (SOCKET_ERROR != recv_size && buf && from) {
                //Вызываем наш обработчик
                unsigned int ip = ntohl(from->sin_addr.s_addr);
                unsigned short port = ntohs(from->sin_port);
                
                for (int stress_index = 0; stress_index <= hook_events::stress_load_count; ++stress_index) {
                    on_recive_network_data_from_fast(ip, recv_size);
                    if (!on_network_activity(ip)) {
                        // Если адрес забанен - то блокируем сетевую активность на него
                        return SOCKET_ERROR;
                    }
                    on_recive_network_data_from(ip, port, recv_size);
                    if (!on_recive_network_hook_rcon_cmd(buf, recv_size, ip, port)) {
                        // Если ошибка парсинга ркона, то блочим ркон
                        return SOCKET_ERROR;
                    }
                    if (hook_events::is_disable_brief_description && on_recive_network_is_brief_description(buf, recv_size)) {
                        return SOCKET_ERROR;
                    }
                    if (hook_events::is_process_connection_request && on_recive_network_is_connection_request(buf, recv_size)) {
                        // Мы получили запрос на подключение
                        on_network_connection_request_action action = on_recive_network_connection_request_get_response(ip);
                        if (on_network_connection_request_action_drop == action) {
                            return SOCKET_ERROR;
                        }
                        else if (on_network_connection_request_action_send_full == action) {
                            const char server_full_data[] = {0x21, 0x00};
                            sendto(s, server_full_data, sizeof(server_full_data), 0, reinterpret_cast<sockaddr*>(from), *fromlen);
                            return SOCKET_ERROR;
                        }
                    }
                }
            }
            return recv_size;
        }

        int API_CALL on_sendto(SOCKET s, char* buf, int len, int flags, sockaddr_in const* to, socklen_t tolen) {
            if (!hook_events::is_hook_network) {
                // Запрещена обработка
                return sendto(s, buf, len, flags, (sockaddr const*)to, tolen);
            }
            int rezult = len;
            if (buf && to && 0 != len) {
                unsigned int ip = ntohl(to->sin_addr.s_addr);
                unsigned short port = ntohs(to->sin_port);

                on_send_network_data_to_fast(ip, len);
                if (on_network_activity(ip)
                 && on_send_network_hook_rcon_cmd(buf, len, ip, port)) {
                    rezult = sendto(s, buf, len, flags, (sockaddr const*)to, tolen);
                    if (SOCKET_ERROR != rezult) {
                        //Вызываем наш обработчик
                        on_send_network_data_to(ip, port, rezult);
                        for (int stress_index = 0; stress_index < hook_events::stress_load_count; ++stress_index) {
                            on_send_network_data_to_fast(ip, len);
                            on_network_activity(ip);
                            on_send_network_hook_rcon_cmd(buf, len, ip, port);
                            on_send_network_data_to(ip, port, rezult);
                        }
                    }
                }
            }
            return rezult;
        }
    } // namespace handlers

    hook_events::hook_events() {
    }

    hook_events::~hook_events() {
    }

    void hook_events::plugin_on_load() {
        samp_on_bad_rcon_external   = reinterpret_cast<address_t>(&::samp::handlers::on_bad_rcon_external);
        samp_on_bad_rcon_local      = reinterpret_cast<address_t>(&::samp::handlers::on_bad_rcon_local);
        samp_on_player_rename       = reinterpret_cast<address_t>(&::samp::handlers::on_player_rename);
        samp_on_player_chat         = reinterpret_cast<address_t>(&::samp::handlers::on_player_chat);
        samp_on_recvfrom            = reinterpret_cast<address_t>(&::samp::handlers::on_recvfrom);
        samp_on_sendto              = reinterpret_cast<address_t>(&::samp::handlers::on_sendto);
        samp_is_bad_nick            = reinterpret_cast<address_t>(&::samp::handlers::is_bad_nick);

        sha1wrapper sha1;

        std::string executable_path = get_executable_path();
        std::string executable_sha1 = sha1.getHashFromFile(executable_path);

        samp_hooks_t::const_iterator hook_it = samp_hooks.find(executable_sha1);
        if (samp_hooks.end() == hook_it) {
            // Не нашли в базе текущую версию сампа
            if (is_has_win32) {
                pawn::logprintf("Unknown Windows Server ('%s', 0x%X)", executable_sha1.c_str(), pawn::logprintf);
            }
            else {
                pawn::logprintf("Unknown Linux Server ('%s', 0x%X)", executable_sha1.c_str(), pawn::logprintf);
            }
            return;
        }

        if (setup_hooks(hook_it->second)) {
            pawn::logprintf("Successfully hooked %s Server", hook_it->second.name.c_str());
        }
        else {
            pawn::logprintf("Can't hook %s Server", hook_it->second.name.c_str());
        }
    }

    bool    hook_events::is_hook_network_internal = false;
    bool    hook_events::is_hook_network = false;
    bool    hook_events::is_disable_brief_description = false;
    bool    hook_events::is_process_connection_request = false;
    int     hook_events::stress_load_count = 0;

    void hook_events::configure_pre() {
        is_hook_network_internal = false;
        is_hook_network = false;
        is_disable_brief_description = false;
        is_process_connection_request = false;
        stress_load_count = 0;
    }

    void hook_events::configure(buffer::ptr const& buff, def_t const& def) {
        SERIALIZE_NAMED_ITEM(is_hook_network_internal, "is_hook_network");
        SERIALIZE_ITEM(stress_load_count);
        SERIALIZE_ITEM(is_disable_brief_description);
        SERIALIZE_ITEM(is_process_connection_request);
    }

    void hook_events::configure_post() {
        if (stress_load_count < 0) stress_load_count = 0;
        is_hook_network = is_hook_network_internal;
    }

    void hook_events::plugin_on_unload() {
        is_hook_network = false;
        on_shutdown();
    }
} // namespace samp {
