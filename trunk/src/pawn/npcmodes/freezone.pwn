#include <a_npc>

main(){}

/*
// Отладка бота
event_impl(event_string[]) {
    new File:log_file = fopen("npc.log", io_append);
    fwrite(log_file, event_string);
    fwrite(log_file, "\r\n");
    fclose(log_file);
}
*/

// Отключаем отладку
#define event_impl(%1);

// we can printf like use
//stock event_fr(format_text[], {Float,_}:...);
#define event_fr(%1,%2); \
    {new txt_[256];format(txt_,256,%1,%2);event_impl(txt_);}
    
#define event event_impl

enum fm_rpc_e {
     fm_rpc_playback_start_none = 100
    ,fm_rpc_playback_start_driver
    ,fm_rpc_playback_start_onfoot
    ,fm_rpc_playback_stop
    ,fm_rpc_playback_pause
    ,fm_rpc_playback_resume    
};

fm_send_event(event_string[]) {
    new buff[256];
    format(buff, sizeof(buff), "{event} %s", event_string);
    SendCommand(buff);
}


public OnNPCModeInit() {
    event("OnNPCModeInit");
}

public OnNPCModeExit() {
    event("OnNPCModeExit");
}

public OnNPCConnect(myplayerid) {
    event_fr("OnNPCConnect myplayerid=%d", myplayerid);
    fm_send_event("connect");
}

public OnNPCDisconnect(reason[]) {
    event_fr("OnNPCDisconnect reason='%s'", reason);
}

public OnNPCSpawn() {
    event("OnNPCSpawn");
    fm_send_event("spawn");
}

public OnNPCEnterVehicle(vehicleid, seatid) {
    event_fr("OnNPCEnterVehicle vehicleid=%d seatid=%d", vehicleid, seatid);
    fm_send_event("vehicle_enter");
}

public OnNPCExitVehicle() {
    event("OnNPCExitVehicle");
    fm_send_event("vehicle_exit");
}

public OnClientMessage(color, text[]) {
    event_fr("OnClientMessage color=%d text='%s'", color, text);
    if (fm_rpc_playback_start_none == fm_rpc_e:color) {
        StartRecordingPlayback(PLAYER_RECORDING_TYPE_NONE, text);
    }
    else if (fm_rpc_playback_start_driver == fm_rpc_e:color) {
        StartRecordingPlayback(PLAYER_RECORDING_TYPE_DRIVER, text);
    }
    else if (fm_rpc_playback_start_onfoot == fm_rpc_e:color) {
        StartRecordingPlayback(PLAYER_RECORDING_TYPE_ONFOOT, text);
    }
    else if (fm_rpc_playback_stop == fm_rpc_e:color) {
        StopRecordingPlayback();
    }
    else if (fm_rpc_playback_pause == fm_rpc_e:color) {
        PauseRecordingPlayback();
    }
    else if (fm_rpc_playback_resume == fm_rpc_e:color) {
        ResumeRecordingPlayback();
    }
}

public OnPlayerDeath(playerid) {
    event_fr("OnPlayerDeath playerid=%d", playerid);
}

public OnPlayerText(playerid, text[]) {
    event_fr("OnPlayerText playerid=%d text='%s'", playerid, text);
}

public OnPlayerStreamIn(playerid) {
    event_fr("OnPlayerStreamIn playerid=%d", playerid);
}

public OnPlayerStreamOut(playerid) {
    event_fr("OnPlayerStreamOut playerid=%d", playerid);
}

public OnVehicleStreamIn(vehicleid) {
    event_fr("OnVehicleStreamIn vehicleid=%d", vehicleid);
}

public OnVehicleStreamOut(vehicleid) {
    event_fr("OnVehicleStreamOut vehicleid=%d", vehicleid);
}

public OnRecordingPlaybackEnd() {
    event("OnRecordingPlaybackEnd");
    fm_send_event("playback_end");
}
