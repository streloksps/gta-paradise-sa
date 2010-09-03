#include <a_samp>
#define new_adress "83.222.115.139:7777"

/*
В конце конфига дописать:
plugins moveto
filterscripts moveto
gamemode0 moveto
password moveto

*/

main() {
}

public OnGameModeInit() {
	new buff_hostname[256];
	new buff_mode[256];
	new buff_map[256];
	new buff_weburl[256];
	
	format(buff_hostname, sizeof(buff_hostname), "hostname moved to: %s", new_adress);
	format(buff_mode, sizeof(buff_mode),         "gamemodetext FreeZone moved to: %s", new_adress);
	format(buff_map, sizeof(buff_map),           "mapname Russia");
	format(buff_weburl, sizeof(buff_weburl),     "weburl www.gta-paradise.ru");
	

	SendRconCommand(buff_hostname);
	SendRconCommand(buff_mode);
	SendRconCommand(buff_map);
	SendRconCommand(buff_weburl);

	AddPlayerClass(0, 1958.3783, 1343.1572, 15.3746, 269.1425, 0, 0, 0, 0, 0, 0);
	print("Moveto init done");
	return 1;
}

