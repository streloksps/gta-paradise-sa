/*
  Файл описания плагина freezone
*/

#if defined FREEZONE_INC
	#endinput
#endif
#define FREEZONE_INC
#pragma library freezone

#tryinclude <freemodetoken.inc>
#if !defined freemode_token
#tryinclude "../includes/freemodetoken.inc"
#if !defined freemode_token
#define freemode_token 0
#endif
#endif

enum server_ver {
    server_ver_022 = 1
   ,server_ver_02X = 2
   ,server_ver_03a = 3
   ,server_ver_03b = 4
};

native FZGameModeInit(server_ver:ver);
native FZGameModeExit();
native FZPlayerConnect(playerid);
native FZPlayerDisconnect(playerid, reason);
native FZPlayerSpawn(playerid);
native FZPlayerDeath(playerid, killerid, reason);
native FZVehicleSpawn(vehicleid);
native FZVehicleDeath(vehicleid, killerid);
native FZPlayerText(playerid, text[]);
native FZPlayerCommandText(playerid, cmdtext[]);
native FZPlayerRequestClass(playerid, classid);
native FZPlayerEnterVehicle(playerid, vehicleid, ispassenger);
native FZPlayerExitVehicle(playerid, vehicleid);
native FZPlayerStateChange(playerid, newstate, oldstate);
native FZPlayerEnterCheckpoint(playerid);
native FZPlayerLeaveCheckpoint(playerid);
native FZPlayerEnterRaceCheckpoint(playerid);
native FZPlayerLeaveRaceCheckpoint(playerid);
native FZRconCommand(cmd[]);
native FZPlayerRequestSpawn(playerid);
native FZObjectMoved(objectid);
native FZPlayerObjectMoved(playerid, objectid);
native FZPlayerPickUpPickup(playerid, pickupid);
native FZVehicleMod(playerid, vehicleid, componentid);
native FZEnterExitModShop(playerid, enterexit, interiorid);
native FZVehiclePaintjob(playerid, vehicleid, paintjobid);
native FZVehicleRespray(playerid, vehicleid, color1, color2);
native FZVehicleDamageStatusUpdate(vehicleid, playerid);
native FZPlayerSelectedMenuRow(playerid, row);
native FZPlayerExitedMenu(playerid);
native FZPlayerInteriorChange(playerid, newinteriorid, oldinteriorid);
native FZPlayerKeyStateChange(playerid, newkeys, oldkeys);
native FZRconLoginAttempt( ip[], password[], success );
native FZPlayerUpdate(playerid);
native FZPlayerStreamIn(playerid, forplayerid);
native FZPlayerStreamOut(playerid, forplayerid);
native FZVehicleStreamIn(vehicleid, forplayerid);
native FZVehicleStreamOut(vehicleid, forplayerid);
native FZDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
native FZPlayerClickPlayer(playerid, clickedplayerid, source);

#define iterate_players(%1) for (new %1 = 0, max_players = GetMaxPlayers(); %1 < max_players; ++%1) if (IsPlayerConnected(%1))

//Возращает ид игрока, который ведет авто, или -1, если таких нет
stock vehicle_get_driver(vehicle_id) {
    iterate_players(i) {
        if (GetPlayerVehicleID(i) == vehicle_id && PLAYER_STATE_DRIVER == GetPlayerState(i)) {
            return i;
        }
    }
    return -1;
}

//Вызвать в любой публичной функции
stock anti_de_amx() {
    new a[][] = {
        "Unarmed (Fist)",
        "Brass K"
    };
    #pragma unused a
}

forward check_freemode_module();
