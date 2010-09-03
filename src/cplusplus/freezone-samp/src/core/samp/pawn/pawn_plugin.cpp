#include "config.hpp"
#include "pawn_plugin.hpp"
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "samp-plugin-sdk/amx/amx.h"
#include "samp-plugin-sdk/plugincommon.h"
#include "core/ver.hpp"
#include "core/application.hpp"
#include "pawn_log.hpp"
#include "pawn_plugin_i.hpp"
#include "core/container/container_handlers.hpp"
#include "core/logger/logger.hpp"


typedef std::list<AMX*> amxs_t;
static amxs_t amxs;

extern void *pAMXFunctions;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

//Загрузка плагина
PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData) {
    // Инициализация служебной переменной, необходимой для работы плагина
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

    // Сохраняем логер сервера
    pawn::logprintf = reinterpret_cast<pawn::logprintf_t>(reinterpret_cast<ptrdiff_t>(ppData[PLUGIN_DATA_LOGPRINTF]));

    pawn::logprintf("%s loaded", plugin_ver);

    { // Пишем в консоль сервера время запуска
        boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
        facet->format("%Y-%m-%d %H:%M:%S");
        std::cout.imbue(std::locale(std::cout.getloc(), facet));
        std::cout<<boost::posix_time::second_clock::local_time()<<" "<<plugin_ver<<" loaded.";
    }
    
    // Вызываем обработчик события
    container_execute_handlers(application::instance(), &pawn::plugin_on_pre_load_i::plugin_on_pre_load);
    container_execute_handlers(application::instance(), &pawn::plugin_on_load_i::plugin_on_load);

    return true;
}

//Выгрузка плагина
PLUGIN_EXPORT void PLUGIN_CALL Unload() {
    // Вызываем обработчик события
    container_execute_handlers(application::instance(), &pawn::plugin_on_unload_i::plugin_on_unload);
    container_execute_handlers(application::instance(), &pawn::plugin_on_post_unload_i::plugin_on_post_unload);

    pawn::logprintf("%s unloaded", plugin_ver);
    pawn::logprintf = 0;
}

// Подгрузка виртуальной машины
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
    // Вызываем обработчик события
    container_execute_handlers(application::instance(), &pawn::plugin_on_pre_amx_load_i::plugin_on_load, amx);
    container_execute_handlers(application::instance(), &pawn::plugin_on_amx_load_i::plugin_on_amx_load, amx);
    amxs.push_back(amx);
    return AMX_ERR_NONE;
}

// Выгрузка виртуальной машины
PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
    // Вызываем обработчик события
    container_execute_handlers(application::instance(), &pawn::plugin_on_amx_unload_i::plugin_on_amx_unload, amx);
    container_execute_handlers(application::instance(), &pawn::plugin_on_post_amx_unload_i::plugin_on_post_amx_unload, amx);
    amxs.remove(amx);
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
    static int tick_id = 0;
    tick_id = (tick_id + 1) % 20;
    if (0 == tick_id) {
        //logger::instance()->debug("ProcessTick", "hi");
        container_execute_handlers(application::instance(), &pawn::plugin_on_tick_i::plugin_on_tick);
    }
}

security_tokens_t get_security_tokens() {
    security_tokens_t rezult;
    for (amxs_t::const_iterator it = amxs.begin(), end = amxs.end(); end != it; ++it) {
        cell retval;
        int index;
        if (AMX_ERR_NONE == amx_FindPublic(*it, "check_freemode_module", &index)) {
            if (AMX_ERR_NONE == amx_Exec(*it, &retval, index)) {
                rezult.push_back(retval);
                continue;
            }
        }
        // Ошибочный код
        rezult.push_back(0);
    }
    return rezult;
}
