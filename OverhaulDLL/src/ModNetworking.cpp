 #include "ModNetworking.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "AnimationEdits.h"
#include <wchar.h>

//This is needed for the steam callbacks to work
static ModNetworking modnet = ModNetworking();

extern "C" {
    uint64_t AcceptP2PSessionWithUser_injection_return;
    void AcceptP2PSessionWithUser_injection();
    bool AcceptP2PSessionWithUser_injection_helper(uint64_t incoming_steamid);

    uint64_t IsP2PPacketAvailable_1_Replacement_injection_return;
    void IsP2PPacketAvailable_1_Replacement_injection();
    uint64_t IsP2PPacketAvailable_2_Replacement_injection_return;
    void IsP2PPacketAvailable_2_Replacement_injection();
    bool IsP2PPacketAvailable_Replacement_injection_helper(uint32 *pcubMsgSize, int nChannel);

    uint64_t ReadP2PPacket_Replacement_injection_return;
    void ReadP2PPacket_Replacement_injection();
    bool ReadP2PPacket_Replacement_injection_helper(void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, CSteamID *psteamIDRemote, int nChannel);

    uint64_t SendP2PPacket_voice_Replacement_injection_return;
    void SendP2PPacket_voice_Replacement_injection();
    uint64_t SendP2PPacket_Replacement_injection_return;
    void SendP2PPacket_Replacement_injection();
    bool SendP2PPacket_Replacement_injection_helper(CSteamID steamIDRemote, const void *pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel);

    uint64_t CloseP2PSessionWithUser_Replacement_injection_return;
    void CloseP2PSessionWithUser_Replacement_injection();
    bool CloseP2PSessionWithUser_Replacement_injection_helper(CSteamID steamIDRemote);
}

typedef void* gfGetSteamInterface(int iSteamUser, int iUnkInt, const char* pcVersion, const char* pcInterface);
typedef ISteamClient* gfCreateSteamInterface(const char* pSteamClientVer, uint32_t iUnkZero);
ISteamUser* ModNetworking::SteamUser = nullptr;
ISteamFriends* ModNetworking::SteamFriends = nullptr;
ISteamMatchmaking* ModNetworking::SteamMatchmaking = nullptr;
ISteamNetworking* ModNetworking::SteamNetworking = nullptr;
ISteamNetworkingMessages* ModNetworking::SteamNetMessages = nullptr;
ISteamNetworkingUtils* ModNetworking::SteamNetUtils = nullptr;

bool Init_ISteam_Interfaces(void* unused)
{
    if (!SteamAPI_GetHSteamUser() || !SteamAPI_GetHSteamPipe())
    {
        return true;
    }

    HMODULE hSteamClient64 = GetModuleHandleA("steamclient64.dll");
    if (!hSteamClient64)
    {
        return true;
    }

    gfCreateSteamInterface* fCreateSteamInterface = (gfCreateSteamInterface*)GetProcAddress(hSteamClient64, "CreateInterface");
    if (!fCreateSteamInterface)
    {
        return true;
    }

    ISteamClient* SteamClient = fCreateSteamInterface("SteamClient017", 0);
    if (!SteamClient)
    {
        return true;
    }

    ModNetworking::SteamUser = (ISteamUser*)SteamClient->GetISteamGenericInterface(1, 1, STEAMUSER_INTERFACE_VERSION);
    if (!ModNetworking::SteamUser)
    {
        return true;
    }
    ModNetworking::SteamFriends = (ISteamFriends*)SteamClient->GetISteamGenericInterface(1, 1, STEAMFRIENDS_INTERFACE_VERSION);
    if (!ModNetworking::SteamFriends)
    {
        return true;
    }
    ModNetworking::SteamMatchmaking = (ISteamMatchmaking*)SteamClient->GetISteamGenericInterface(1, 1, STEAMMATCHMAKING_INTERFACE_VERSION);
    if (!ModNetworking::SteamMatchmaking)
    {
        return true;
    }
    ModNetworking::SteamNetworking = (ISteamNetworking*)SteamClient->GetISteamGenericInterface(1, 1, STEAMNETWORKING_INTERFACE_VERSION);
    if (!ModNetworking::SteamNetworking)
    {
        return true;
    }
    ModNetworking::SteamNetMessages = (ISteamNetworkingMessages*)SteamClient->GetISteamGenericInterface(1, 1, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
    if (!ModNetworking::SteamNetMessages)
    {
        return true;
    }
    ModNetworking::SteamNetUtils = (ISteamNetworkingUtils*)SteamClient->GetISteamGenericInterface(1, 1, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
    if (!ModNetworking::SteamNetUtils)
    {
        return true;
    }

    ConsoleWrite("Steam Init_ISteam_Interfaces loaded");

    return false;
}

void ModNetworking::start()
{
    ConsoleWrite("Enabling mod networking...");

    uint8_t *write_address;

    /*
     * Code to initalize the ISteamNetworkingMessages Interface
     */
    MainLoop::setup_mainloop_callback(Init_ISteam_Interfaces, NULL, "Init_ISteam_Interfaces");

    /*
     * Supporting code for the connection handshake section
     * Inject into the AcceptP2PSessionWithUser callback to be sure we d/c users
     */
    write_address = (uint8_t*)(ModNetworking::AcceptP2PSessionWithUser_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &AcceptP2PSessionWithUser_injection_return, 0, &AcceptP2PSessionWithUser_injection);

    /*
     * Code to replace the IsP2PPacketAvailable call with a compatible ISteamNetworkingMessages API call
     * Nop out a big chunk since we want to replace the entire call
     */
    write_address = (uint8_t*)(ModNetworking::IsP2PPacketAvailable_1_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &IsP2PPacketAvailable_1_Replacement_injection_return, 17, &IsP2PPacketAvailable_1_Replacement_injection);
    write_address = (uint8_t*)(ModNetworking::IsP2PPacketAvailable_2_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &IsP2PPacketAvailable_2_Replacement_injection_return, 17, &IsP2PPacketAvailable_2_Replacement_injection);

    /*
     * Code to replace the ReadP2PPacket call with a compatible ISteamNetworkingMessages API call
     * Nop out a big chunk since we want to replace the entire call
     */
    write_address = (uint8_t*)(ModNetworking::ReadP2PPacket_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &ReadP2PPacket_Replacement_injection_return, 26, &ReadP2PPacket_Replacement_injection);

    /*
     * Code to replace the SendP2PPacket call with a compatible ISteamNetworkingMessages API call
     * Nop out a big chunk since we want to replace the entire call
     */
    write_address = (uint8_t*)(ModNetworking::SendP2PPacket_voice_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &SendP2PPacket_voice_Replacement_injection_return, 37, &SendP2PPacket_voice_Replacement_injection);
    write_address = (uint8_t*)(ModNetworking::SendP2PPacket_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &SendP2PPacket_Replacement_injection_return, 39, &SendP2PPacket_Replacement_injection);

    /*
     * Code to replace the CloseP2PSessionWithUser call with a compatible ISteamNetworkingMessages API call
     * Nop out a big chunk since we want to replace the entire call
     */
    write_address = (uint8_t*)(ModNetworking::CloseP2PSessionWithUser_Replacement_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &CloseP2PSessionWithUser_Replacement_injection_return, 9, &CloseP2PSessionWithUser_Replacement_injection);

}

/*
 * ===========ROLLBACK NETCODE SECTION
 */
int64_t ModNetworking::timer_offset = 0;


/*
 * ===========ISteamNetworkingMessages REPLACEMENT SECTION
 */

 //Only use the new steamapi if we have finished the handshake connection and all connected users support it
bool SteamNetworkingMessages_Supported()
{
    return false && ModNetworking::currentLobby != 0 && ModNetworking::lobby_setup_complete && Mod::get_mode() != ModMode::Compatability;
}

//Add a new callback for the NetMessages equivalent of AcceptP2PSessionWithUser
void ModNetworking::SteamNetworkingMessagesSessionRequestCallback(SteamNetworkingMessagesSessionRequest_t* pCallback)
{
    CSteamID user = pCallback->m_identityRemote.GetSteamID();

    //Check if we want to d/c them
    if (ModNetworking::incoming_guest_to_not_accept != 0 && user.ConvertToUint64() == ModNetworking::incoming_guest_to_not_accept)
    {
        return;
    }

    //Check they're not blocked
    EFriendRelationship relation = ModNetworking::SteamFriends->GetFriendRelationship(user);
    if (relation == EFriendRelationship::k_EFriendRelationshipIgnored)
    {
        return;
    }

    //Check they're in the lobby with us
    if (!ModNetworking::SteamFriends->IsUserInSource(user, ModNetworking::currentLobby))
    {
        return;
    }

    ModNetworking::SteamNetMessages->AcceptSessionWithUser(pCallback->m_identityRemote);
}

static SteamNetworkingMessage_t* message[1];
static bool have_message = false;
static int have_message_on_channel = 0; //in theory this should be redundant since DSR only uses channel 0

//IsP2PPacketAvailable
bool IsP2PPacketAvailable_Replacement_injection_helper(uint32 *pcubMsgSize, int nChannel)
{
    if (SteamNetworkingMessages_Supported())
    {
        /*
         * There is no equivilent in the NetworkingMessages API, so just grab a message if one is present and save it
         * Grab this saved message and return it in the upcoming ReadP2PPacket call
         */

        //the dark souls code can only handle 1 message at a time, and only reads them from 1 channel: channel 0
        if (have_message)
        {
            ConsoleWrite("WARNING: IsP2PPacketAvailable called but we had an unread message waiting. Dropping it.");
            have_message = false;
            message[0]->Release();
            message[0] = nullptr;
        }
        int num_messages = ModNetworking::SteamNetMessages->ReceiveMessagesOnChannel(nChannel, message, 1);

        if (num_messages == 1)
        {
            *pcubMsgSize = message[0]->m_cbSize;
            have_message = true;
            have_message_on_channel = nChannel;
        }
        return have_message;
    }
    else
    {
        return ModNetworking::SteamNetworking->IsP2PPacketAvailable(pcubMsgSize, nChannel);
    }
}

//ReadP2PPacket/ReceiveMessagesOnChannel
bool ReadP2PPacket_Replacement_injection_helper(void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, CSteamID *psteamIDRemote, int nChannel)
{
    if (SteamNetworkingMessages_Supported())
    {
        //fill out the given args from the saved network message struct
        if (have_message && message[0] != nullptr && have_message_on_channel == nChannel)
        {
            memcpy_s(pubDest, cubDest, message[0]->m_pData, message[0]->m_cbSize);
            *pcubMsgSize = message[0]->m_cbSize;
            *psteamIDRemote = message[0]->m_identityPeer.GetSteamID();

            message[0]->Release();
            message[0] = nullptr;
            have_message = false;
            return true;
        }
        else
        {
            ConsoleWrite("WARNING: ReadP2PPacket called but we didn't have a message to read or the channel was incorrect. have_message=%d, have_message_on_channel:%d!=%d", have_message, have_message_on_channel, nChannel);
            have_message = false;
            return false;
        }
    }
    else
    {
        return ModNetworking::SteamNetworking->ReadP2PPacket(pubDest, cubDest, pcubMsgSize, psteamIDRemote, nChannel);
    }
}

//SendP2PPacket/SendMessageToUser
bool SendP2PPacket_Replacement_injection_helper(CSteamID steamIDRemote, const void *pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel)
{
    // Handle disconnecting a user
    if (ModNetworking::incoming_guest_to_not_accept != 0 && ModNetworking::incoming_guest_to_not_accept == steamIDRemote.ConvertToUint64())
    {
        return false;
    }

    if (SteamNetworkingMessages_Supported())
    {
        SteamNetworkingIdentity target;
        target.SetSteamID(steamIDRemote);

        int nSendFlags;
        switch (eP2PSendType)
        {
            case k_EP2PSendUnreliable:
                nSendFlags = k_nSteamNetworkingSend_UnreliableNoNagle;
                break;
            case k_EP2PSendUnreliableNoDelay:
                nSendFlags = k_nSteamNetworkingSend_UnreliableNoDelay;
                break;
            case k_EP2PSendReliable:
                nSendFlags = k_nSteamNetworkingSend_ReliableNoNagle;
                break;
            case k_EP2PSendReliableWithBuffering:
                nSendFlags = k_nSteamNetworkingSend_Reliable;
                break;
        }

        EResult res = ModNetworking::SteamNetMessages->SendMessageToUser(target, pubData, cubData, nSendFlags, nChannel);
        return res == EResult::k_EResultOK;
    }
    else
    {
        return ModNetworking::SteamNetworking->SendP2PPacket(steamIDRemote, pubData, cubData, eP2PSendType, nChannel);
    }
}

//CloseP2PSessionWithUser/CloseSessionWithUser
bool CloseP2PSessionWithUser_Replacement_injection_helper(CSteamID steamIDRemote)
{
    if (SteamNetworkingMessages_Supported())
    {
        SteamNetworkingIdentity remote;
        remote.SetSteamID(steamIDRemote);
        return ModNetworking::SteamNetMessages->CloseSessionWithUser(remote);
    }
    else
    {
        return ModNetworking::SteamNetworking->CloseP2PSessionWithUser(steamIDRemote);
    }
}


/*
 * ===========CONNECTION HANDSHAKE SECTION
 */
const char* MOD_LOBBY_DATA_KEY = "DarkSoulsOverhaulModData";
const uint8_t MOD_ENABLED = 0x80; //it's encoded that the mod is active in the most significant bit
const uint8_t LEGACY_ENABLED = 0x40; //it's encoded that the mod is in legacy mode in the 2nd most significant bit

bool ModNetworking::allow_connect_with_non_mod_host = true;
bool ModNetworking::allow_connect_with_legacy_mod_host = true;
bool ModNetworking::allow_connect_with_overhaul_mod_host = true;
bool ModNetworking::allow_connect_with_non_mod_guest = true;

bool ModNetworking::host_got_info = false;
bool ModNetworking::host_mod_installed = false;
bool ModNetworking::host_legacy_enabled = false;
bool ModNetworking::incoming_guest_got_info = false;
bool ModNetworking::incoming_guest_mod_installed = false;
bool ModNetworking::incoming_guest_legacy_enabled = false;
uint64_t ModNetworking::incoming_guest_to_not_accept = 0;

uint64_t ModNetworking::currentLobby = 0;
bool ModNetworking::lobby_setup_complete = false;

bool AcceptP2PSessionWithUser_injection_helper(uint64_t incoming_steamid)
{
    if (ModNetworking::incoming_guest_to_not_accept != 0 && incoming_steamid == ModNetworking::incoming_guest_to_not_accept)
    {
        ConsoleWrite("Denying connection to d/c'd user %llx", incoming_steamid);
        return false;
    }
    return true;
}

void HostForceDisconnectGuest(uint64_t steamid, const wchar_t* dc_reason)
{
    //Ensure we don't implicitly accept the connection via calling SendP2PPacket to this user
    ModNetworking::incoming_guest_to_not_accept = steamid;

    //Disconnect any existing session we have via CloseP2PSessionWithUser / CloseSessionWithUser
    if (SteamNetworkingMessages_Supported())
    {
        SteamNetworkingIdentity remote;
        remote.SetSteamID(steamid);
        ModNetworking::SteamNetMessages->CloseSessionWithUser(remote);
    }
    else
    {
        ModNetworking::SteamNetworking->CloseP2PSessionWithUser(steamid);
    }

    //Tell the host why we're dcing this guest
    wchar_t dc_msg[300];
    swprintf(dc_msg, 300, L"Disconnecting incoming guest player.\n%s", dc_reason);
    Game::show_popup_message(dc_msg);
}

typedef struct
{
    uint64_t start_time;
} GuestAwaitIncomingLobbyData_Struct;

static const uint32_t MS_TO_WAIT_FOR_HOST_DATA = (uint32_t)(0.25 * 10000000);

bool GuestAwaitIncomingLobbyData(void* unused);

void ModNetworking::LobbyEnterCallback(LobbyEnter_t* pCallback)
{
    CSteamID lobbyowner = ModNetworking::SteamMatchmaking->GetLobbyOwner(pCallback->m_ulSteamIDLobby);
    CSteamID selfsteamid = ModNetworking::SteamUser->GetSteamID();
    ModNetworking::currentLobby = pCallback->m_ulSteamIDLobby;
    ModNetworking::lobby_setup_complete = false;
    ModNetworking::incoming_guest_to_not_accept = 0; //prevent any holdover data on new lobby

    // 1. As the host, upon lobby creation(and self entry into it) set the lobby data to inform connecting users of our mod status
    // Since lobby data is persistent, we don't have to worry about resending anything for new connections
    if (lobbyowner == selfsteamid)
    {
        char value = MOD_ENABLED;
        if (Mod::legacy_mode)
        {
            value |= LEGACY_ENABLED;
        }
        ConsoleWrite("1. Host set lobby data = %hhx", value);
        ModNetworking::SteamMatchmaking->SetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY, &value);
        return;
    }

    // 2. If we're the guest, upon lobby connection check for lobby data on mod status
    // This callback is only triggered when WE attempt to enter a lobby, so existing guests won't have this trigger
    // The lobby data may not arrive for a bit, so set up a callback to trigger when we do get the data (if we get it)
    if (lobbyowner != selfsteamid)
    {
        GuestAwaitIncomingLobbyData_Struct* data = (GuestAwaitIncomingLobbyData_Struct*)malloc(sizeof(GuestAwaitIncomingLobbyData_Struct));
        data->start_time = Game::get_accurate_time();
        MainLoop::setup_mainloop_callback(GuestAwaitIncomingLobbyData, nullptr, "GuestAwaitIncomingLobbyData");
        return;
    }
}

void ModNetworking::LobbyDataUpdateCallback(LobbyDataUpdate_t* pCallback)
{
    CSteamID lobbyowner = ModNetworking::SteamMatchmaking->GetLobbyOwner(pCallback->m_ulSteamIDLobby);
    CSteamID selfsteamid = ModNetworking::SteamUser->GetSteamID();

    // 2. If we're the guest, listen to the callback for lobby data updates
    // This will be triggered upon joining a lobby, and also upon reciving any lobby data updates
    if (lobbyowner != selfsteamid)
    {
        const char* mod_value_arry = ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY);
        char mod_value = mod_value_arry[0];
        ConsoleWrite("2. Guest get lobby data = %hhx", mod_value);

        if (mod_value == 0)
        {
            ModNetworking::host_mod_installed = false;
        }
        else
        {
            if ((mod_value & MOD_ENABLED) != 0)
            {
                ModNetworking::host_mod_installed = true;
            }
            else
            {
                ModNetworking::host_mod_installed = false;
            }

            if ((mod_value & LEGACY_ENABLED) != 0)
            {
                ModNetworking::host_legacy_enabled = true;
            }
            else
            {
                ModNetworking::host_legacy_enabled = false;
            }

            ModNetworking::host_got_info = true;
        }
    }
}

bool GuestAwaitIncomingLobbyData(void* data_a)
{
    GuestAwaitIncomingLobbyData_Struct* data = (GuestAwaitIncomingLobbyData_Struct*)data_a;

    // Wait till we either recieve data from the host about mod status, or we timeout
    // If we timeout, then we know they aren't a mod user

    //No timeout yet, back to sleep
    if (ModNetworking::host_got_info == false && !(Game::get_accurate_time() > data->start_time + MS_TO_WAIT_FOR_HOST_DATA))
    {
        return true;
    }
    //Timeout, host is non-mod user
    if (ModNetworking::host_got_info == false && (Game::get_accurate_time() > data->start_time + MS_TO_WAIT_FOR_HOST_DATA))
    {
        ModNetworking::host_mod_installed = false;
    }

    // 3. As the guest, we must change our settings to match the host (based on how we've configured our options), or else disconnect.
    if (ModNetworking::host_mod_installed == false && ModNetworking::allow_connect_with_non_mod_host == true)
    {
        //connecting to non-mod
        Mod::set_mode(true, ModNetworking::host_mod_installed);
    }
    else if (ModNetworking::host_mod_installed == true && ModNetworking::host_legacy_enabled == false && ModNetworking::allow_connect_with_overhaul_mod_host == true)
    {
        //connecting to overhaul
        Mod::set_mode(ModNetworking::host_legacy_enabled, ModNetworking::host_mod_installed);
    }
    else if (ModNetworking::host_mod_installed == true && ModNetworking::host_legacy_enabled == true && ModNetworking::allow_connect_with_legacy_mod_host == true)
    {
        //connecting to legacy
        Mod::set_mode(ModNetworking::host_legacy_enabled, ModNetworking::host_mod_installed);
    }
    else
    {
        //Tell the player why we're dcing
        wchar_t dc_msg[300];
        wcscpy_s(dc_msg, L"Unable to connect to host player.\n");
        if (ModNetworking::host_mod_installed == false && ModNetworking::allow_connect_with_non_mod_host == false)
        {
            wcscat_s(dc_msg, L"Host does not have mod and you do not allow connections with non-mod users.");
        }
        else if (ModNetworking::host_mod_installed && ModNetworking::host_legacy_enabled == false && ModNetworking::allow_connect_with_overhaul_mod_host == false)
        {
            wcscat_s(dc_msg, L"Host is in overhaul mode and you do not allow connections with overhaul mode users.");
        }
        else if (ModNetworking::host_mod_installed && ModNetworking::host_legacy_enabled == true && ModNetworking::allow_connect_with_legacy_mod_host == false)
        {
            wcscat_s(dc_msg, L"Host is in legacy mode and you do not allow connections with legacy mode users.");
        }
        Game::show_popup_message(dc_msg);
        ConsoleWrite("3. Guest disconnecting due to settings");

        //disconnect
        ModNetworking::SteamMatchmaking->LeaveLobby(ModNetworking::currentLobby);
        ModNetworking::currentLobby = 0;
        ModNetworking::lobby_setup_complete = false;
        ModNetworking::host_got_info = false;
        free(data);
        return false;
    }

    //send a chat message confirming we also respect these settings and have the mod
    char value = MOD_ENABLED;
    if (Mod::legacy_mode)
    {
        value |= LEGACY_ENABLED;
    }
    ModNetworking::SteamMatchmaking->SendLobbyChatMsg(ModNetworking::currentLobby, &value, sizeof(value));
    ConsoleWrite("3. Guest send chat msg = %hhx", value);
    ModNetworking::lobby_setup_complete = true;

    //reset the incoming guest info for next host
    ModNetworking::host_got_info = false;
    free(data);
    return false;
}

void ModNetworking::LobbyChatMsgCallback(LobbyChatMsg_t* pCallback)
{
    CSteamID lobbyowner = ModNetworking::SteamMatchmaking->GetLobbyOwner(pCallback->m_ulSteamIDLobby);
    CSteamID selfsteamid = ModNetworking::SteamUser->GetSteamID();

    if (pCallback->m_ulSteamIDLobby != currentLobby)
    {
        return;
    }
    if (pCallback->m_ulSteamIDUser == selfsteamid.ConvertToUint64())
    {
        return;
    }

    // 4. If we're the host, listen for the response chat message from the guest saying they respect these settings
    if (lobbyowner == selfsteamid)
    {
        char value;
        int iMsgSize = ModNetworking::SteamMatchmaking->GetLobbyChatEntry(pCallback->m_ulSteamIDLobby, pCallback->m_iChatID, nullptr, &value, sizeof(value), nullptr);

        if (iMsgSize >= 1)
        {
            ConsoleWrite("4. Host got chat msg = %hhx", value);

            if ((value & MOD_ENABLED) != 0)
            {
                ModNetworking::incoming_guest_mod_installed = true;
            }
            else
            {
                ModNetworking::incoming_guest_mod_installed = false;
            }

            if ((value & LEGACY_ENABLED) != 0)
            {
                ModNetworking::incoming_guest_legacy_enabled = true;
            }
            else
            {
                ModNetworking::incoming_guest_legacy_enabled = false;
            }

            ModNetworking::incoming_guest_got_info = true; //Wait until we're all done before setting this, so we don't get interrupted
        }
    }
}

typedef struct
{
    uint64_t start_time;
    uint64_t steamid;
} AwaitIncomingUserChatMessage_Struct;

static const uint32_t MS_TO_WAIT_FOR_GUEST_MSG = (uint32_t)(0.75 * 10000000);

bool HostAwaitIncomingGuestChatMessage(void* data_a);

void ModNetworking::LobbyChatUpdateCallback(LobbyChatUpdate_t* pCallback)
{
    CSteamID lobbyowner = ModNetworking::SteamMatchmaking->GetLobbyOwner(pCallback->m_ulSteamIDLobby);
    CSteamID selfsteamid = ModNetworking::SteamUser->GetSteamID();
    ModNetworking::incoming_guest_to_not_accept = 0; //make sure a reconnecting user has another chance (this is reset on both leave and join)

    // 4. As the host, when a new user connects to the lobby set a timer to wait for their response chat message
    // Need to handle the case where they don't send a chat message, and thus are a non-mod user and we must disconnect them from this lobby
    // UNLESS: we allow non-mod connections, and we don't have any other non-mod phantoms in here
    if (lobbyowner == selfsteamid && pCallback->m_rgfChatMemberStateChange == EChatMemberStateChange::k_EChatMemberStateChangeEntered && pCallback->m_ulSteamIDUserChanged != selfsteamid.ConvertToUint64())
    {
        AwaitIncomingUserChatMessage_Struct* data = (AwaitIncomingUserChatMessage_Struct*)malloc(sizeof(AwaitIncomingUserChatMessage_Struct));
        data->start_time = Game::get_accurate_time();
        data->steamid = pCallback->m_ulSteamIDUserChanged;
        MainLoop::setup_mainloop_callback(HostAwaitIncomingGuestChatMessage, data, "HostAwaitIncomingGuestChatMessage");
    }
}

bool HostAwaitIncomingGuestChatMessage(void* data_a)
{
    AwaitIncomingUserChatMessage_Struct* data = (AwaitIncomingUserChatMessage_Struct*)data_a;

    //No timeout yet, back to sleep
    if (ModNetworking::incoming_guest_got_info == false && !(Game::get_accurate_time() > data->start_time + MS_TO_WAIT_FOR_GUEST_MSG))
    {
        return true;
    }

    //Timed out waiting for a message from the incoming guest, so they're a non-mod user
    //Also handle the (impossible?) case where we got the response from the guest but they say they don't have the mod
    else if ((ModNetworking::incoming_guest_got_info == false && Game::get_accurate_time() > data->start_time + MS_TO_WAIT_FOR_GUEST_MSG) ||
        (ModNetworking::incoming_guest_got_info == true && ModNetworking::incoming_guest_mod_installed == false))
    {
        ConsoleWrite("4. Host detects incoming guest is non-mod user. d/cing due to settings.");

        //As the host, we only change our settings if the connecting user is non-mod, and we allow non-mod connections, and we don't have any other non-mod phantoms in here
        if (ModNetworking::allow_connect_with_non_mod_guest == true)
        {
            // first non-mod user, change settings
            if (ModNetworking::SteamMatchmaking->GetNumLobbyMembers(ModNetworking::currentLobby) == 2)
            {
                Mod::set_mode(true, false);
            }
            // not first non-mod user and the other guests are not in a non-mod compatible mode
            else if (ModNetworking::SteamMatchmaking->GetNumLobbyMembers(ModNetworking::currentLobby) > 2 && Mod::get_mode() != Compatability)
            {
                HostForceDisconnectGuest(data->steamid, L"Incoming guest is a non-mod user, but mod user is already connected.");
            }

            else
            {
                FATALERROR("Number of lobby members is %d. Impossible.", ModNetworking::SteamMatchmaking->GetNumLobbyMembers(ModNetworking::currentLobby));
            }
        }
        // If specified in options, we must disconnect the non-mod player, since they won't on their own
        else if (ModNetworking::allow_connect_with_non_mod_guest == false)
        {
            HostForceDisconnectGuest(data->steamid, L"Incoming guest is non-mod user, and you do not allow connections with non-mod users.");
        }
    }

    // If the guest isn't respecting our settings for some reason, disconnect them right away
    else if (ModNetworking::incoming_guest_mod_installed == true && ModNetworking::incoming_guest_legacy_enabled != Mod::legacy_mode)
    {
        ConsoleWrite("4. Host detects incoming guest has wrong settings");

        HostForceDisconnectGuest(data->steamid, L"Incoming guest is mod-user, but isn't matching your legacy/overhaul mode.");
    }

    //Success state for a new guest
    else
    {
        ConsoleWrite("4. Host allows guest in!");

        ModNetworking::lobby_setup_complete = true;

        // As the host, start up the clock syncronization function
        //MainLoop::setup_mainloop_callback(HostTimerSync, NULL, "HostTimerSync");
    }

    //reset the incoming guest info for next guest
    ModNetworking::incoming_guest_got_info = false;
    free(data);
    return false;
}
