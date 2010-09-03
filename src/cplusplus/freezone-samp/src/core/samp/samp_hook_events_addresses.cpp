#include "config.hpp"
#include "samp_hook_events_addresses.hpp"

namespace samp {
    address_t samp_on_bad_rcon_external = 0;
    address_t samp_on_bad_rcon_local = 0;
    address_t samp_on_player_rename = 0;
    address_t samp_on_player_chat = 0;
    address_t samp_on_recvfrom = 0;
    address_t samp_on_sendto = 0;
    address_t samp_is_bad_nick = 0;

    samp_hooks_t init() {
        samp_hooks_t rezult;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /*
        {
            samp_hook_t hook(0x00476D90, "Windows 0.2.2 United");
            hook.E8.push_back(replace_pair_t(0x0048235B, &samp_on_bad_rcon_external));
            hook.E8.push_back(replace_pair_t(0x0047A98D, &samp_on_bad_rcon_local));
            hook.FF15.push_back(replace_pair_t(0x0045E154, &samp_on_recvfrom));
            hook.FF15.push_back(replace_pair_t(0x0047450A, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x00481C3E, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x00481D0F, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x00481EFA, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x00482182, &samp_on_sendto));
            rezult.insert(std::make_pair("07fd932f7dd11c9b4f0e85e445c9b41f4614e37b", hook));
        }

        {
            samp_hook_t hook(0x0046A590, "Windows 0.2X");
            hook.E8.push_back(replace_pair_t(0x00475E31, &samp_on_bad_rcon_external));
            hook.E8.push_back(replace_pair_t(0x0046E1EF, &samp_on_bad_rcon_local));
            hook.FF15.push_back(replace_pair_t(0x00451744, &samp_on_recvfrom));
            hook.FF15.push_back(replace_pair_t(0x00467BFA, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x004756FE, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x004757BF, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x004759AA, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x00475C32, &samp_on_sendto));
            rezult.insert(std::make_pair("2b15c53b93a379629a59a96339baffcf9ebf3bcf", hook));
        }
        {
            samp_hook_t hook(0x00477A30, "Windows 0.3a");
            hook.E8.push_back(replace_pair_t(0x0047EB05, &samp_on_bad_rcon_external));
            hook.E8.push_back(replace_pair_t(0x0047B51C, &samp_on_bad_rcon_local));
            hook.E8.push_back(replace_pair_t(0x004676A9, &samp_on_player_rename));
            hook.E8.push_back(replace_pair_t(0x0047C48C, &samp_on_player_chat));
            hook.FF15.push_back(replace_pair_t(0x0045D8F2, &samp_on_recvfrom));
            hook.FF15.push_back(replace_pair_t(0x004764BA, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x0047E3EE, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x0047E4AF, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x0047E68F, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x0047E914, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x004675A9, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x0047BE6B, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x0047C1FD, &samp_is_bad_nick));
            rezult.insert(std::make_pair("e00401cc4d357c4017f178c588dcdc80230705f2", hook));
        }
        */
        {
            samp_hook_t hook(0x00478910, "Windows 0.3a R7");
            hook.E8.push_back(replace_pair_t(0x0047FBA5, &samp_on_bad_rcon_external));  // "BAD RCON ATTEMPT BY: %s"
            hook.E8.push_back(replace_pair_t(0x0047C5BE, &samp_on_bad_rcon_local));     // "RCON (In-Game): Player #%d (%s) <%s> failed login."
            hook.E8.push_back(replace_pair_t(0x0047D54C, &samp_on_player_chat));        // "[chat] [%s]: %s"
            hook.E8.push_back(replace_pair_t(0x00467E49, &samp_on_player_rename));      // "[nick] %s nick changed to %s"
            hook.E8.push_back(replace_pair_t(0x00467D49, &samp_is_bad_nick));           // "SetPlayerName", см samp_on_player_rename, 6 блок с начала функции
            hook.E8.push_back(replace_pair_t(0x0047CF2B, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x0047D2BD, &samp_is_bad_nick));
            hook.FF15.push_back(replace_pair_t(0x0045D322, &samp_on_recvfrom)); // recvfrom
            hook._8B1D.push_back(replace_pair_t(0x0045D190, &samp_on_sendto));   // __imp_sendto
            hook.FF15.push_back(replace_pair_t(0x0047739A, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x0047F48E, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x0047F54F, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x0047F72F, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x0047F9B4, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x00404E45, &samp_on_sendto)); // sendto
            rezult.insert(std::make_pair("dc384103bd9eb23080de94756cab993e63f3d8d9", hook)); // Нормальная версия самп сервера R7
            rezult.insert(std::make_pair("7ad254501e77ea8c8358ed3fdb705895c05e7db9", hook)); // Версия самп сервера для Андреуса a1a R7
            rezult.insert(std::make_pair("db16383f2327f935d40e85ee23dbbd2ab5a17a44", hook)); // Версия самп сервера для Гостауна g6a R7
            rezult.insert(std::make_pair("68b181214e379b3e112f541131e44ef83c80c3d0", hook)); // Версия самп сервера для Юнайтеда u1a R7
        }
        {
            samp_hook_t hook(0x00497140, "Windows 0.3b R2");
            hook.E8.push_back(replace_pair_t(0x004A07A5, &samp_on_bad_rcon_external));  // sequence of bytes, "BAD RCON ATTEMPT BY: %s"
            hook.E8.push_back(replace_pair_t(0x0049AF4E, &samp_on_bad_rcon_local));     // sequence of bytes, "RCON (In-Game): Player #%d (%s) <%s> failed login."
            hook.E8.push_back(replace_pair_t(0x0049BB82, &samp_on_player_chat));        // sequence of bytes, "[chat] [%s]: %s"
            hook.E8.push_back(replace_pair_t(0x0048E919, &samp_on_player_rename));      // sequence of bytes, "[nick] %s nick changed to %s"
            hook.E8.push_back(replace_pair_t(0x0048E819, &samp_is_bad_nick));           // sequence of bytes, "SetPlayerName", 6 блок с начала функции, см samp_on_player_rename
            hook.E8.push_back(replace_pair_t(0x0049B8ED, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x0049F84A, &samp_is_bad_nick));
            hook.FF15.push_back(replace_pair_t(0x00479F22, &samp_on_recvfrom)); // смотрим таблицу импорта, recvfrom из WSOCK32
            hook._8B1D.push_back(replace_pair_t(0x00479D90, &samp_on_sendto));  // __imp_sendto
            hook.FF15.push_back(replace_pair_t(0x00495BCA, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x004A008E, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x004A014F, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x004A032F, &samp_on_sendto));
            hook.FF15.push_back(replace_pair_t(0x004A05B4, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x00404E45, &samp_on_sendto)); // sendto
            rezult.insert(std::make_pair("da39a3ee5e6b4b0d3255bfef95601890afd80709", hook)); // Нормальная версия самп сервера
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /*
        {
            samp_hook_t hook(0x8087B76, "Linux 0.2.2 United");
            hook.E8.push_back(replace_pair_t(0x809E7E2, &samp_on_bad_rcon_external));
            hook.E8.push_back(replace_pair_t(0x808DC0E, &samp_on_bad_rcon_local));
            hook.E8.push_back(replace_pair_t(0x804D5C1, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x80E341A, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x804D57A, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8060CF4, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x809DE0D, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x809E0E5, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x809E2EA, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x809E562, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x809E923, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x80E35C2, &samp_on_sendto));
            rezult.insert(std::make_pair("6bf4de0c60c944e11c26f61b670ee6d50f571370", hook));
        }
        {
            samp_hook_t hook(0x807D760, "Linux 0.2X");
            hook.E8.push_back(replace_pair_t(0x8091785, &samp_on_bad_rcon_external));
            hook.E8.push_back(replace_pair_t(0x80827C8, &samp_on_bad_rcon_local));
            hook.E8.push_back(replace_pair_t(0x804D381, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x80CD002, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x804CEFE, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x805CCEE, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8090F0A, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x80911FE, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8091412, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x80915EC, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8091762, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x80CD1B1, &samp_on_sendto));
            rezult.insert(std::make_pair("57cf13f09f9f6d31465753346078322c3bd88450", hook));
        }
        {
            samp_hook_t hook(0x8074F24, "Linux 0.3a R2");
            hook.E8.push_back(replace_pair_t(0x8089060, &samp_on_bad_rcon_external));
            hook.E8.push_back(replace_pair_t(0x8079C14, &samp_on_bad_rcon_local));
            hook.E8.push_back(replace_pair_t(0x804DC60, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x80C1399, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x804DC02, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x805CD20, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8088865, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8088A80, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8088C33, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8088E3B, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x808919C, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x80C14FE, &samp_on_sendto));
            rezult.insert(std::make_pair("8587d1fc896abf977f61f4dd158cc0ed0bd677f7", hook));
        }
        {
            samp_hook_t hook(0x8074F24, "Linux 0.3a R3");
            hook.E8.push_back(replace_pair_t(0x8089060, &samp_on_bad_rcon_external));
            hook.E8.push_back(replace_pair_t(0x8079C14, &samp_on_bad_rcon_local));
            hook.E8.push_back(replace_pair_t(0x80AFF48, &samp_on_player_rename));            
            hook.E8.push_back(replace_pair_t(0x804DC60, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x80C0B69, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x804DC02, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x805CD20, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8088865, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8088A80, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8088C33, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x8088E3B, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x808919C, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x80C0CCE, &samp_on_sendto));
            rezult.insert(std::make_pair("bcc7a4cd8e29dc886875f00537a62b104113df0a", hook));
        }
        {
            samp_hook_t hook(0x8074F58, "Linux 0.3a R4");
            hook.E8.push_back(replace_pair_t(0x080891F8, &samp_on_bad_rcon_external));
            hook.E8.push_back(replace_pair_t(0x08079C9A, &samp_on_bad_rcon_local));
            hook.E8.push_back(replace_pair_t(0x0807BA0B, &samp_on_player_chat));
            hook.E8.push_back(replace_pair_t(0x080B0108, &samp_on_player_rename));
            hook.E8.push_back(replace_pair_t(0x0804DC74, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x080C0F69, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x0804DC16, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0805CD0C, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x080889FD, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x08088C18, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x08088DCB, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x08088FD3, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x08089334, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x080C10CE, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0807B04A, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x0807B6F5, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x080AFF97, &samp_is_bad_nick));
            rezult.insert(std::make_pair("27f7cda5f4af9564af2369dc68aa9be370a2bbc7", hook)); // Нормальная версия самп сервера
            rezult.insert(std::make_pair("b1c4cd4ecb90985c4be97b94858aee574470800a", hook)); // Версия самп сервера для Гостауна g6a (версия = 59421)
            rezult.insert(std::make_pair("2b1be8b67303f74c3abeef41ec1638c6a1a624c3", hook)); // Версия самп сервера для Гостауна g6a R2
            rezult.insert(std::make_pair("4b67f9acea723d11a596db08ec80519d33f66f76", hook)); // Версия самп сервера для Юнайтеда u1a R2
            rezult.insert(std::make_pair("9511a53b037cff7a45607cee82bf0dbf1fa57639", hook)); // Версия самп сервера для Андреуса a1a R2
        }

        {
            samp_hook_t hook(0x80763B0, "Linux 0.3a R5");
            hook.E8.push_back(replace_pair_t(0x0808C99E, &samp_on_bad_rcon_external));  // "BAD RCON ATTEMPT BY: %s"
            hook.E8.push_back(replace_pair_t(0x0807B903, &samp_on_bad_rcon_local));     // "RCON (In-Game): Player #%d (%s) <%s> failed login."
            hook.E8.push_back(replace_pair_t(0x0807E0EE, &samp_on_player_chat));        // "[chat] [%s]: %s"
            hook.E8.push_back(replace_pair_t(0x080B4F8A, &samp_on_player_rename));      // "[nick] %s nick changed to %s"
            hook.E8.push_back(replace_pair_t(0x0804E07E, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x080C78D5, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x0804DBEE, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0805CC6E, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C12A, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C404, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C610, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C7F0, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C97E, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x080C7731, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0807DA5F, &samp_is_bad_nick));           // "SetPlayerName", см samp_on_player_rename, 6 блок с начала функции
            hook.E8.push_back(replace_pair_t(0x0807DED7, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x080B4E1D, &samp_is_bad_nick));
            rezult.insert(std::make_pair("3cfdc3aa8dfc58085484641348b7df7f80fe28af", hook)); // Нормальная версия самп сервера
            rezult.insert(std::make_pair("ef4f27d90e2573ebbae7f2d22ac8fbb9cc396d2d", hook)); // Версия самп сервера для Андреуса a1a R4
            rezult.insert(std::make_pair("97757dd3c0497cf28e6654f583876295c65012af", hook)); // Версия самп сервера для Гостауна g6a R4
            rezult.insert(std::make_pair("beefe84853c509953106db12444fd094aa2283e8", hook)); // Версия самп сервера для Юнайтеда u1a R4
        }

        {
            samp_hook_t hook(0x080763B0, "Linux 0.3a R6");
            hook.E8.push_back(replace_pair_t(0x0808C78E, &samp_on_bad_rcon_external));  // "BAD RCON ATTEMPT BY: %s"
            hook.E8.push_back(replace_pair_t(0x0807B903, &samp_on_bad_rcon_local));     // "RCON (In-Game): Player #%d (%s) <%s> failed login."
            hook.E8.push_back(replace_pair_t(0x0807E0EE, &samp_on_player_chat));        // "[chat] [%s]: %s"
            hook.E8.push_back(replace_pair_t(0x080B4D7A, &samp_on_player_rename));      // "[nick] %s nick changed to %s"
            hook.E8.push_back(replace_pair_t(0x0804E07E, &samp_on_recvfrom));           // objdump -D samp03svr | grep 'recvfrom@plt', берем адреса из первого столбика
            hook.E8.push_back(replace_pair_t(0x080C7775, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x0804DBEE, &samp_on_sendto));             // objdump -D samp03svr | grep 'sendto@plt', берем адреса из первого столбика
            hook.E8.push_back(replace_pair_t(0x0805CC6E, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808BF1A, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C1F4, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C400, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C5E0, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C76E, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x080C75D1, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0807DA5F, &samp_is_bad_nick));           // "SetPlayerName", см samp_on_player_rename, 6 блок с начала функции
            hook.E8.push_back(replace_pair_t(0x0807DED7, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x080B4C0D, &samp_is_bad_nick));
            rezult.insert(std::make_pair("4ed67e0d2699db78096f11e0e31427a3ae079ed1", hook)); // Нормальная версия самп сервера R5
            rezult.insert(std::make_pair("099d9a5722001607d0a269a941846a8e60921223", hook)); // Версия самп сервера для Андреуса a1a R5
            rezult.insert(std::make_pair("706fcb622e9bfe1a418b16558542e46dfa488b5a", hook)); // Версия самп сервера для Гостауна g6a R5
            rezult.insert(std::make_pair("a627d0d7ce9a53a7af50d02f9f70368090f51578", hook)); // Версия самп сервера для Юнайтеда u1a R5
        }
        */

        {
            samp_hook_t hook(0x080763B0, "Linux 0.3a R7");
            hook.E8.push_back(replace_pair_t(0x0808C79E, &samp_on_bad_rcon_external));  // "BAD RCON ATTEMPT BY: %s"
            hook.E8.push_back(replace_pair_t(0x0807B903, &samp_on_bad_rcon_local));     // "RCON (In-Game): Player #%d (%s) <%s> failed login."
            hook.E8.push_back(replace_pair_t(0x0807E0EE, &samp_on_player_chat));        // "[chat] [%s]: %s"
            hook.E8.push_back(replace_pair_t(0x080B4D8A, &samp_on_player_rename));      // "[nick] %s nick changed to %s"
            hook.E8.push_back(replace_pair_t(0x0807DA5F, &samp_is_bad_nick));           // "SetPlayerName", см samp_on_player_rename, 6 блок с начала функции
            hook.E8.push_back(replace_pair_t(0x0807DED7, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x080B4C1D, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x0804E07E, &samp_on_recvfrom));           // objdump -D samp03svr | grep 'recvfrom@plt', берем адреса из первого столбика
            hook.E8.push_back(replace_pair_t(0x080C7785, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x0804DBEE, &samp_on_sendto));             // objdump -D samp03svr | grep 'sendto@plt', берем адреса из первого столбика
            hook.E8.push_back(replace_pair_t(0x0805CC6E, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808BF2A, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C204, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C410, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C5F0, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0808C77E, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x080C75E1, &samp_on_sendto));
            rezult.insert(std::make_pair("ef98b2427052df1ea654b8575a2a1cea042a318a", hook)); // Нормальная версия самп сервера R7
            rezult.insert(std::make_pair("eaa290a39eecd37a99d189eaaaeb593dace2229b", hook)); // Версия самп сервера для Андреуса a1a R7
            rezult.insert(std::make_pair("99212b20cc05f3cd558a2340aa3d55e97a60f7f9", hook)); // Версия самп сервера для Гостауна g6a R7
            rezult.insert(std::make_pair("125c66bf2fdd3ff31fed302dcf74c186a83d913d", hook)); // Версия самп сервера для Юнайтеда u1a R7
        }
        {
            samp_hook_t hook(0x08076B10, "Linux 0.3b R2");
            hook.E8.push_back(replace_pair_t(0x0809FDCE, &samp_on_bad_rcon_external));  // "BAD RCON ATTEMPT BY: %s"
            hook.E8.push_back(replace_pair_t(0x0807C2E3, &samp_on_bad_rcon_local));     // "RCON (In-Game): Player #%d (%s) <%s> failed login."
            hook.E8.push_back(replace_pair_t(0x0807E731, &samp_on_player_chat));        // "[chat] [%s]: %s"
            hook.E8.push_back(replace_pair_t(0x080C842A, &samp_on_player_rename));      // "[nick] %s nick changed to %s"
            hook.E8.push_back(replace_pair_t(0x0807E507, &samp_is_bad_nick));           // "SetPlayerName", см samp_on_player_rename, 6 блок с начала функции
            hook.E8.push_back(replace_pair_t(0x080812DB, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x080C82BD, &samp_is_bad_nick));
            hook.E8.push_back(replace_pair_t(0x0804E63E, &samp_on_recvfrom));           // objdump -D samp03svr | grep 'recvfrom@plt', берем адреса из первого столбика
            hook.E8.push_back(replace_pair_t(0x080DBAD5, &samp_on_recvfrom));
            hook.E8.push_back(replace_pair_t(0x0804E1AE, &samp_on_sendto));             // objdump -D samp03svr | grep 'sendto@plt', берем адреса из первого столбика
            hook.E8.push_back(replace_pair_t(0x0805D2FE, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0809F55A, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0809F834, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0809FA40, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0809FC20, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x0809FDAE, &samp_on_sendto));
            hook.E8.push_back(replace_pair_t(0x080DB931, &samp_on_sendto));
            rezult.insert(std::make_pair("ddac809ea7faab82eb10358ee1600e8401dae067", hook)); // Нормальная версия самп сервера
        }

        return rezult;
    }

    samp_hooks_t samp_hooks = init();
} // namespace samp {

/*
Нахожение адресов под windows:
Находим название методов:
dumpbin /all c:\WINDOWS\system32\ws2_32.dll | find /i "sendto"
dumpbin /all c:\WINDOWS\system32\ws2_32.dll | find /i "recvfrom"

Ставим точку останова на
{,,wsock32.dll}_sendto@24
{,,wsock32.dll}_recvfrom@24
{,,ws2_32.dll}_sendto@24
{,,ws2_32.dll}_recvfrom@24

Нахождение адресов по принтфам:
ставим точку остонова на принтф, в watch пишем:
*((char**)esp+0) - первый текстовой аргумент
*((char**)esp+1) - второй текстовой аргумент
*((char**)esp+2) или сместить +1 :)


Получить дизасемблер:
dumpbin /disasm C:\_dev\local_samp_server\samp-server.exe > C:\_dev\local_samp_server\samp-server.dumpbin
objdump.exe -Dlx samp-server.exe > samp-server.objdump

Линукс
Получить дамп:
objdump -D samp02Xsvr > samp02Xsvr.dmb

Получить адреса:
objdump -D samp02Xsvr | grep 'recvfrom@plt'
objdump -D samp02Xsvr | grep 'sendto@plt'
берем адреса из первого столбика


samp_is_bad_nick находиться через строку в лог о смене ника через иду



Отладка:
gdb ./samp03svr
b *0x8074F24 - точна останова на адрес (нужно на адрес принтфа)
r - запустить прогу
c - продолжить выполнение
bt - показать стек
q - выйди

display *((char**)$sp + 1) - чтобы всегда выводил первый строковой аргумент
print *((char**)$sp + 1) - печать первого строкового аргумента
print *((char**)$sp + 2) - печать второго строкового аргумента



(gdb) bt
#0  0x08074f24 in putchar ()
#1  0x08089065 in putchar ()
берем вторую строчку и отнимаем 5: 0x08089065 - 5 = 0x08089065

для получения брейк поинта в игре нужно забить 2 c перед подключением
*/
