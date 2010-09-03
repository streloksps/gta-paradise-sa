#ifndef SAMP_DEMARSHALING_HPP
#define SAMP_DEMARSHALING_HPP
#include <string>
#include <cassert>
#include "samp-plugin-sdk/amx/amx.h"

namespace pawn {
    //Демаршалинг pawn аргументов

    //Класс аргумента
    template<int param_id, typename T>
    class demarh_t {
    };

    template<int param_id>
    class demarh_t<param_id, int> {
        int val;
    public:
        demarh_t(AMX* amx, cell* params) {
            val = static_cast<int>(params[param_id + 1]);
        }
        int get() {
            return val;
        }
        static const int next_param_id = param_id + 1;
    };

    template<int param_id>
    class demarh_t<param_id, bool> {
        bool val;
    public:
        demarh_t(AMX* amx, cell* params) {
            val = 0 != params[param_id + 1];
        }
        bool get() {
            return val;
        }
        static const int next_param_id = param_id + 1;
    };

    template<int param_id>
    class demarh_t<param_id, std::string const&> {
        std::string val;
    public:
        demarh_t(AMX* amx, cell* params) {
            char buff[129];
            cell* cstr;

            amx_GetAddr(amx, params[param_id + 1], &cstr);
            amx_GetString(buff, cstr, 0, sizeof(buff)/sizeof(buff[0]));
            val = std::string(buff);
        }
        std::string const& get() {
            return val;
        }
        static const int next_param_id = param_id + 1;
    };

    template<int param_id>
    class demarh_t<param_id, AMX*> {
        AMX* amx;
    public:
        demarh_t(AMX* amx, cell* params): amx(amx) {
        }
        AMX* get() {
            return amx;
        }
        static const int next_param_id = param_id;
    };

    // Класс для проверки количества аргументов
    template<int param_id>
    class demarh_last_t {
    public:
        demarh_last_t(AMX* amx, cell* params) {
            assert(params[0] == param_id * sizeof(cell));
        }
    };

} // namespace pawn {

#endif // SAMP_DEMARSHALING_HPP
