#ifndef PAWN_MARSHALING_AMX_HPP
#define PAWN_MARSHALING_AMX_HPP
#include "samp-plugin-sdk/amx/amx.h"
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>

namespace pawn {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Классы описания методов
    class marh_amx_t;

    // Колекция описателей. Используется для инициализации/финализации группы
    class marh_collection_t {
    public:
        typedef std::vector<marh_amx_t*> marhs_t;
        marhs_t marhs;

        inline void add(marh_amx_t* item);
        inline std::vector<std::string> get_pawn_calls() const;
        inline void fini();
    };

    class marh_amx_t {
    private:
        std::string const name;
        std::string const pawn_call_args;
    protected:
        AMX* amx;
        AMX_NATIVE fn;

        inline marh_amx_t(std::string const& name, std::string const& pawn_call_args, marh_collection_t& marh_collection);
        inline marh_amx_t(std::string const& name, std::string const& pawn_call_args);

    public:
        // В случае ошибки возращает false
        inline bool init(AMX* new_amx);
        inline void fini();
        inline std::string get_pawn_call() const;

    private:
        //Возращает адрес метода павн по его имени или 0 в случае неудачи
        inline static AMX_NATIVE marh_find_function(AMX* amx, std::string const& name);
    };

    // Реализация
    ////////////////////////////////////////////////////////////////////////////////////////////////

    marh_amx_t::marh_amx_t(std::string const& name, std::string const& pawn_call_args, marh_collection_t& marh_collection): name(name), pawn_call_args(pawn_call_args), amx(0), fn(0) {
        marh_collection.add(this);
    }

    marh_amx_t::marh_amx_t(std::string const& name, std::string const& pawn_call_args): name(name), pawn_call_args(pawn_call_args), amx(0), fn(0) {
    }

    bool marh_amx_t::init(AMX* new_amx) {
        if (new_amx) {
            fn = marh_find_function(new_amx, name);
            if (fn) {
                amx = new_amx;
                return true;
            }
        }
        return false;
    }

    void marh_amx_t::fini() {
        fn = 0;
        amx = 0;
    }

    std::string marh_amx_t::get_pawn_call() const {
        return "    " + name + "(" + pawn_call_args + ");";
    }

    AMX_NATIVE marh_amx_t::marh_find_function(AMX* amx, std::string const& name) {
        if(!amx) {
            return 0;
        }

        int index;
        amx_FindNative(amx, name.c_str(), &index);

        if(0x7FFFFFFF == index) {
            //The command cannot be found.
            return 0;
        }

        // Proceed with locating the memory address for this function;
        AMX_HEADER *hdr = (AMX_HEADER *)amx->base;
        unsigned int call_addr = (unsigned int)((AMX_FUNCSTUB *)
            ((char *)(hdr) + (hdr)->natives + hdr->defsize * index))->address;

        if (call_addr == 0) {
            //Could not locate the function's address.
            return 0;
        }

        return (AMX_NATIVE)call_addr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void marh_collection_t::add(marh_amx_t* item) {
        marhs.push_back(item);
    }

    std::vector<std::string> marh_collection_t::get_pawn_calls() const {
        std::vector<std::string> rezult;
        rezult.reserve(marhs.size());
        std::transform(marhs.begin(), marhs.end(), std::back_inserter(rezult), std::tr1::bind(&marh_amx_t::get_pawn_call, std::tr1::placeholders::_1));
        return rezult;
    }

    void marh_collection_t::fini() {
        std::for_each(marhs.begin(), marhs.end(), std::tr1::bind(&marh_amx_t::fini, std::tr1::placeholders::_1));
    }

} // namespace pawn {

#endif // PAWN_MARSHALING_AMX_HPP
