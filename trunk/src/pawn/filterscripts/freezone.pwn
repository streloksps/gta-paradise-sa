/*
  Файл фильтр-скрипта freezone
*/
#include <a_samp>
#include "../includes/freezone"
#include "../../shared/freezone_ver"

public OnFilterScriptInit() {
    anti_de_amx();
    print("freezone " VERSION_STR_DESC " filterscript load");
	return true;
}

public OnFilterScriptExit() {
    print("freezone " VERSION_STR_DESC " filterscript unload");
	return true;
}

public OnRconCommand(cmd[]) {
    // Дает возможность обрабатывать rcon команды в гейм моде
    return 0;
}

public check_freemode_module() {
    return (VERSION_BUILD_ID | 0x0C000000) ^ freemode_token;
}

