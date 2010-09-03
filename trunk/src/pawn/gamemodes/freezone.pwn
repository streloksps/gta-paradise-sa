/* Русский
  Файл мода freezone
*/

// Устраняем предупреждения компилятора на несоответвие тегов
#define Text _
#define Menu _
#define Text3D _
#define PlayerText3D _

#include <a_samp>
#include "../includes/freezone"
#include "../../shared/freezone_ver"

main() {
}

stock test() {
	// Тестирование производительности
    new first = GetTickCount(), last = 0;
    for(new i = 0; i < 1000000; ++i) {
        SetGameModeText("Some text");
    }
    last = GetTickCount();
    printf("Pawn Time: %d ms", last - first);
}

public OnGameModeInit() {
	//test();
	
    anti_de_amx();
    print("freezone " VERSION_STR_DESC ", v0.3b gamemode load");
    return FZGameModeInit(server_ver_03b);
}

public OnGameModeExit() {
    print("freezone " VERSION_STR_DESC ", v0.3b gamemode unload");
    return FZGameModeExit();
}

public OnPlayerConnect(playerid) {
    return FZPlayerConnect(playerid);
}

public OnPlayerDisconnect(playerid, reason) {
    return FZPlayerDisconnect(playerid, reason);
}

public OnPlayerSpawn(playerid) {
    return FZPlayerSpawn(playerid);
}

public OnPlayerDeath(playerid, killerid, reason) {
    return FZPlayerDeath(playerid, killerid, reason);
}

public OnVehicleSpawn(vehicleid) {
    return FZVehicleSpawn(vehicleid);
}

public OnVehicleDeath(vehicleid, killerid) {
    return FZVehicleDeath(vehicleid, killerid);
}

public OnPlayerText(playerid, text[]) {
    return FZPlayerText(playerid, text);
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    return FZPlayerCommandText(playerid, cmdtext);
}

public OnPlayerRequestClass(playerid, classid) {
    return FZPlayerRequestClass(playerid, classid);
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger) {
    return FZPlayerEnterVehicle(playerid, vehicleid, ispassenger);
}

public OnPlayerExitVehicle(playerid, vehicleid) {
    return FZPlayerExitVehicle(playerid, vehicleid);
}

public OnPlayerStateChange(playerid, newstate, oldstate) {
    return FZPlayerStateChange(playerid, newstate, oldstate);
}

public OnPlayerEnterCheckpoint(playerid) {
    return FZPlayerEnterCheckpoint(playerid);
}

public OnPlayerLeaveCheckpoint(playerid) {
    return FZPlayerLeaveCheckpoint(playerid);
}

public OnPlayerEnterRaceCheckpoint(playerid) {
    return FZPlayerEnterRaceCheckpoint(playerid);
}

public OnPlayerLeaveRaceCheckpoint(playerid) {
    return FZPlayerLeaveRaceCheckpoint(playerid);
}

public OnRconCommand(cmd[]) {
    return FZRconCommand(cmd);
}

public OnPlayerRequestSpawn(playerid) {
    return FZPlayerRequestSpawn(playerid);
}

public OnObjectMoved(objectid) {
    return FZObjectMoved(objectid);
}

public OnPlayerObjectMoved(playerid, objectid) {
    return FZPlayerObjectMoved(playerid, objectid);
}

public OnPlayerPickUpPickup(playerid, pickupid) {
    return FZPlayerPickUpPickup(playerid, pickupid);
}

public OnVehicleMod(playerid, vehicleid, componentid) {
    return FZVehicleMod(playerid, vehicleid, componentid);
}

public OnEnterExitModShop(playerid, enterexit, interiorid) {
    return FZEnterExitModShop(playerid, enterexit, interiorid);
}

public OnVehiclePaintjob(playerid, vehicleid, paintjobid) {
    return FZVehiclePaintjob(playerid, vehicleid, paintjobid);
}

public OnVehicleRespray(playerid, vehicleid, color1, color2) {
    return FZVehicleRespray(playerid, vehicleid, color1, color2);
}

public OnVehicleDamageStatusUpdate(vehicleid, playerid) {
	return FZVehicleDamageStatusUpdate(vehicleid, playerid);
}

public OnPlayerSelectedMenuRow(playerid, row) {
    return FZPlayerSelectedMenuRow(playerid, row);
}

public OnPlayerExitedMenu(playerid) {
    return FZPlayerExitedMenu(playerid);
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid) {
    return FZPlayerInteriorChange(playerid, newinteriorid, oldinteriorid);
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    return FZPlayerKeyStateChange(playerid, newkeys, oldkeys);
}

public OnRconLoginAttempt(ip[], password[], success) {
    return FZRconLoginAttempt(ip, password, success);
}

public OnPlayerUpdate(playerid) {
    return FZPlayerUpdate(playerid);
}

public OnPlayerStreamIn(playerid, forplayerid) {
    return FZPlayerStreamIn(playerid, forplayerid);
}

public OnPlayerStreamOut(playerid, forplayerid) {
    return FZPlayerStreamOut(playerid, forplayerid);
}

public OnVehicleStreamIn(vehicleid, forplayerid) {
    return FZVehicleStreamIn(vehicleid, forplayerid);
}

public OnVehicleStreamOut(vehicleid, forplayerid) {
    return FZVehicleStreamOut(vehicleid, forplayerid);
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    return FZDialogResponse(playerid, dialogid, response, listitem, inputtext);
}

public OnPlayerClickPlayer(playerid, clickedplayerid, source) {
    return FZPlayerClickPlayer(playerid, clickedplayerid, source);
}


//Получено:
//cat freemode.inc | grep Pl | perl -pe 's/forward (Pl([^\(]+))(\(.*\)).*/public On${2}${3} {\n    return ${1}${3};\n}\n/'


// Метод, вызывающий все метода сампа - чтобы заставить самп подгрузить соответствующие библиотеки и чтобы плагин мог их использовать.
// Метод должен быть публичным. Вызывать его не нужно
native gpci(playerid, serial[], len);

forward sys_all_call();
public sys_all_call() {
    new vi;
    new vs[16];
    new Float:vf;

    CreateObject(vi, vf, vf, vf, vf, vf, vf, vf);
    SetObjectPos(vi, vf, vf, vf);
    GetObjectPos(vi, vf, vf, vf);
    SetObjectRot(vi, vf, vf, vf);
    GetObjectRot(vi, vf, vf, vf);
    IsValidObject(vi);
    DestroyObject(vi);
    MoveObject(vi, vf, vf, vf, vf);
    StopObject(vi);
    CreatePlayerObject(vi, vi, vf, vf, vf, vf, vf, vf, vf);
    SetPlayerObjectPos(vi, vi, vf, vf, vf);
    GetPlayerObjectPos(vi, vi, vf, vf, vf);
    SetPlayerObjectRot(vi, vi, vf, vf, vf);
    GetPlayerObjectRot(vi, vi, vf, vf, vf);
    IsValidPlayerObject(vi, vi);
    DestroyPlayerObject(vi, vi);
    MovePlayerObject(vi, vi, vf, vf, vf, vf);
    StopPlayerObject(vi, vi);
    AttachObjectToPlayer(vi, vi, vf, vf, vf, vf, vf, vf);
    AttachPlayerObjectToPlayer(vi, vi, vi, vf, vf, vf, vf, vf, vf);
    SetSpawnInfo(vi, vi, vi, vf, vf, vf, vf, vi, vi, vi, vi, vi, vi);
    SpawnPlayer(vi);
    SetPlayerPos(vi, vf, vf, vf);
    SetPlayerPosFindZ(vi, vf, vf, vf);
    GetPlayerPos(vi, vf, vf, vf);
    SetPlayerFacingAngle(vi, vf);
    GetPlayerFacingAngle(vi, vf);
    IsPlayerInRangeOfPoint(vi, vf, vf, vf, vf);
    IsPlayerStreamedIn(vi, vi);
    SetPlayerInterior(vi, vi);
    GetPlayerInterior(vi);
    SetPlayerHealth(vi, vf);
    GetPlayerHealth(vi, vf);
    SetPlayerArmour(vi, vf);
    GetPlayerArmour(vi, vf);
    SetPlayerAmmo(vi, vi, vi);
    GetPlayerAmmo(vi);
    GetPlayerWeaponState(vi);
    SetPlayerTeam(vi, vi);
    GetPlayerTeam(vi);
    SetPlayerScore(vi, vi);
    GetPlayerScore(vi);
    GetPlayerDrunkLevel(vi);
    SetPlayerDrunkLevel(vi, vi);
    SetPlayerColor(vi, vi);
    GetPlayerColor(vi);
    SetPlayerSkin(vi, vi);
    GetPlayerSkin(vi);
    GivePlayerWeapon(vi, vi, vi);
    ResetPlayerWeapons(vi);
    SetPlayerArmedWeapon(vi, vi);
    GetPlayerWeaponData(vi, vi, vi, vi);
    GivePlayerMoney(vi, vi);
    ResetPlayerMoney(vi);
    SetPlayerName(vi, vs);
    GetPlayerMoney(vi);
    GetPlayerState(vi);
    GetPlayerIp(vi, vs, vi);
    GetPlayerPing(vi);
    GetPlayerWeapon(vi);
    GetPlayerKeys(vi, vi, vi, vi);
    GetPlayerName(vi, vs, vi);
    SetPlayerTime(vi, vi, vi);
    GetPlayerTime(vi, vi, vi);
    TogglePlayerClock(vi, vi);
    SetPlayerWeather(vi, vi);
    ForceClassSelection(vi);
    SetPlayerWantedLevel(vi, vi);
    GetPlayerWantedLevel(vi);
    SetPlayerFightingStyle(vi, vi);
    GetPlayerFightingStyle(vi);
    SetPlayerVelocity(vi, vf, vf, vf);
    GetPlayerVelocity(vi, vf, vf, vf);
    PlayCrimeReportForPlayer(vi, vi, vi);
    SetPlayerShopName(vi, vs);
    SetPlayerSkillLevel(vi, vi, vi);
    GetPlayerSurfingVehicleID(vi);
    SetPlayerHoldingObject(vi, vi, vi, vf, vf, vf, vf, vf, vf);
    StopPlayerHoldingObject(vi);
    IsPlayerHoldingObject(vi);
    SetPlayerChatBubble(vi, vs, vi, vf, vi);
    PutPlayerInVehicle(vi, vi, vi);
    GetPlayerVehicleID(vi);
    GetPlayerVehicleSeat(vi);
    RemovePlayerFromVehicle(vi);
    TogglePlayerControllable(vi, vi);
    PlayerPlaySound(vi, vi, vf, vf, vf);
    ApplyAnimation(vi, vs, vs, vf, vi, vi, vi, vi, vi, vi);
    ClearAnimations(vi, vi);
    GetPlayerAnimationIndex(vi);
    GetAnimationName(vi, vs, vi, vs, vi);
    GetPlayerSpecialAction(vi);
    SetPlayerSpecialAction(vi, vi);
    SetPlayerCheckpoint(vi, vf, vf, vf, vf);
    DisablePlayerCheckpoint(vi);
    SetPlayerRaceCheckpoint(vi, vi, vf, vf, vf, vf, vf, vf, vf);
    DisablePlayerRaceCheckpoint(vi);
    SetPlayerWorldBounds(vi, vf, vf, vf, vf);
    SetPlayerMarkerForPlayer(vi, vi, vi);
    ShowPlayerNameTagForPlayer(vi, vi, vi);
    SetPlayerMapIcon(vi, vi, vf, vf, vf, vi, vi);
    RemovePlayerMapIcon(vi, vi);
    AllowPlayerTeleport(vi, vi);
    SetPlayerCameraPos(vi, vf, vf, vf);
    SetPlayerCameraLookAt(vi, vf, vf, vf);
    SetCameraBehindPlayer(vi);
    GetPlayerCameraPos(vi, vf, vf, vf);
    GetPlayerCameraFrontVector(vi, vf, vf, vf);
    IsPlayerConnected(vi);
    IsPlayerInVehicle(vi, vi);
    IsPlayerInAnyVehicle(vi);
    IsPlayerInCheckpoint(vi);
    IsPlayerInRaceCheckpoint(vi);
    SetPlayerVirtualWorld(vi, vi);
    GetPlayerVirtualWorld(vi);
    EnableStuntBonusForPlayer(vi, vi);
    EnableStuntBonusForAll(vi);
    TogglePlayerSpectating(vi, vi);
    PlayerSpectatePlayer(vi, vi, vi);
    PlayerSpectateVehicle(vi, vi, vi);
    StartRecordingPlayerData(vi, vi, vs);
    StopRecordingPlayerData(vi);
    SendClientMessage(vi, vi, vs);
    SendPlayerMessageToPlayer(vi, vi, vs);
    SendDeathMessage(vi, vi, vi);
    GameTextForPlayer(vi, vs, vi, vi);
    GetMaxPlayers();
    SetGameModeText(vs);
    SetTeamCount(vi);
    AddPlayerClass(vi, vf, vf, vf, vf, vi, vi, vi, vi, vi, vi);
    AddPlayerClassEx(vi, vi, vf, vf, vf, vf, vi, vi, vi, vi, vi, vi);
    AddStaticVehicle(vi, vf, vf, vf, vf, vi, vi);
    AddStaticVehicleEx(vi, vf, vf, vf, vf, vi, vi, vi);
    AddStaticPickup(vi, vi, vf, vf, vf, vi);
    CreatePickup(vi, vi, vf, vf, vf, vi);
    DestroyPickup(vi);
    ShowNameTags(vi);
    ShowPlayerMarkers(vi);
    GameModeExit();
    SetWorldTime(vi);
    GetWeaponName(vi, vs, vi);
    EnableTirePopping(vi);
    AllowInteriorWeapons(vi);
    SetWeather(vi);
    SetGravity(vf);
    AllowAdminTeleport(vi);
    SetDeathDropAmount(vi);
    CreateExplosion(vf, vf, vf, vi, vf);
    EnableZoneNames(vi);
    UsePlayerPedAnims();
    DisableInteriorEnterExits();
    SetNameTagDrawDistance(vf);
    DisableNameTagLOS();
    LimitGlobalChatRadius(vf);
    LimitPlayerMarkerRadius(vf);
    ConnectNPC(vs, vs);
    IsPlayerNPC(vi);
    IsPlayerAdmin(vi);
    Kick(vi);
    Ban(vi);
    BanEx(vi, vs);
    SendRconCommand(vs);
    GetServerVarAsString(vs, vs, vi);
    GetServerVarAsInt(vs);
    GetServerVarAsBool(vs);
    CreateMenu(vs, vi, vf, vf, vf, vf);
    DestroyMenu(vi);
    AddMenuItem(vi, vi, vs);
    SetMenuColumnHeader(vi, vi, vs);
    ShowMenuForPlayer(vi, vi);
    HideMenuForPlayer(vi, vi);
    IsValidMenu(vi);
    DisableMenu(vi);
    DisableMenuRow(vi, vi);
    GetPlayerMenu(vi);
    TextDrawCreate(vf, vf, vs);
    TextDrawDestroy(vi);
    TextDrawLetterSize(vi, vf, vf);
    TextDrawTextSize(vi, vf, vf);
    TextDrawAlignment(vi, vi);
    TextDrawColor(vi, vi);
    TextDrawUseBox(vi, vi);
    TextDrawBoxColor(vi, vi);
    TextDrawSetShadow(vi, vi);
    TextDrawSetOutline(vi, vi);
    TextDrawBackgroundColor(vi, vi);
    TextDrawFont(vi, vi);
    TextDrawSetProportional(vi, vi);
    TextDrawShowForPlayer(vi, vi);
    TextDrawHideForPlayer(vi, vi);
    TextDrawSetString(vi, vs);
    GangZoneCreate(vf, vf, vf, vf);
    GangZoneDestroy(vi);
    GangZoneShowForPlayer(vi, vi, vi);
    GangZoneHideForPlayer(vi, vi);
    GangZoneFlashForPlayer(vi, vi, vi);
    GangZoneStopFlashForPlayer(vi, vi);
    Create3DTextLabel(vs, vi, vf, vf, vf, vf, vi, vi);
    Delete3DTextLabel(vi);
    Attach3DTextLabelToPlayer(vi, vi, vf, vf, vf);
    Attach3DTextLabelToVehicle(vi, vi, vf, vf, vf);
    Update3DTextLabelText(vi, vi, vs);
    CreatePlayer3DTextLabel(vi, vs, vi, vf, vf, vf, vf, vi, vi, vi);
    DeletePlayer3DTextLabel(vi, vi);
    UpdatePlayer3DTextLabelText(vi, vi, vi, vs);
    ShowPlayerDialog(vi, vi, vi, vs, vs, vs, vs);
    CreateVehicle(vi, vf, vf, vf, vf, vi, vi, vi);
    DestroyVehicle(vi);
    IsVehicleStreamedIn(vi, vi);
    GetVehiclePos(vi, vf, vf, vf);
    SetVehiclePos(vi, vf, vf, vf);
    GetVehicleZAngle(vi, vf);
    GetVehicleRotationQuat(vi, vf, vf, vf, vf);
    SetVehicleZAngle(vi, vf);
    SetVehicleParamsForPlayer(vi, vi, vi, vi);
    SetVehicleToRespawn(vi);
    LinkVehicleToInterior(vi, vi);
    AddVehicleComponent(vi, vi);
    RemoveVehicleComponent(vi, vi);
    ChangeVehicleColor(vi, vi, vi);
    ChangeVehiclePaintjob(vi, vi);
    SetVehicleHealth(vi, vf);
    GetVehicleHealth(vi, vf);
    AttachTrailerToVehicle(vi, vi);
    DetachTrailerFromVehicle(vi);
    IsTrailerAttachedToVehicle(vi);
    GetVehicleTrailer(vi);
    SetVehicleNumberPlate(vi, vs);
    GetVehicleModel(vi);
    GetVehicleComponentInSlot(vi, vi);
    GetVehicleComponentType(vi);
    RepairVehicle(vi);
    GetVehicleVelocity(vi, vf, vf, vf);
    SetVehicleVelocity(vi, vf, vf, vf);
    SetVehicleAngularVelocity(vi, vf, vf, vf);
    GetVehicleDamageStatus(vi, vi, vi, vi, vi);
    UpdateVehicleDamageStatus(vi, vi, vi, vi, vi);
    SetVehicleVirtualWorld(vi, vi);
    GetVehicleVirtualWorld(vi);
    gpci(vi, vs, vi);
}

public check_freemode_module() {
    return (VERSION_BUILD_ID | 0x0D000000) ^ freemode_token;
}

