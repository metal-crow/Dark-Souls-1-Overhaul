 #include "ModNetworking.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "AnimationEdits.h"
#include <wchar.h>
#include <mutex>
#include <unordered_map>
#include <string>
#include <queue>
#include "Rollback.h"
#include "ggponet.h"

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
 * ===========ISteamNetworkingMessages REPLACEMENT SECTION
 */

 //We need to keep track of who, specifically, is approved for the new API, even if the lobby is set up
 //This is because if a new non-mod user joins, we want to talk to them correctly and setup everything to use the old API
 //also keep track of if they have the mod installed or not as the value
static std::unordered_map<uint64_t, bool> SteamAPIStatusKnown_Users;

//At the point this is called, we should know the exact nature of the other user. So we should be all set up, mode-wise.
bool SteamNetworkingMessages_Supported(CSteamID steamIDRemote)
{
    //this should never happen, but just in case
    if (SteamAPIStatusKnown_Users.count(steamIDRemote.ConvertToUint64()) == 0)
    {
        //TODO this will trigger after a player leaves, haven't fixed yet
        ConsoleWrite("Called SteamNetworkingMessages_Supported for player %llx, but we don't know them", steamIDRemote.ConvertToUint64());
        return false;
    }
    return SteamAPIStatusKnown_Users[steamIDRemote.ConvertToUint64()];
}

class PacketStorageData
{
public:
    CSteamID steamIDRemote;
    void *pubData;
    uint32 cubData;
    EP2PSend eP2PSendType;
    int nChannel;

    PacketStorageData(CSteamID steamIDRemote_a, const void *pubData_a, uint32 cubData_a, EP2PSend eP2PSendType_a, int nChannel_a)
    {
        steamIDRemote = steamIDRemote_a;
        pubData = malloc(cubData_a);
        cubData = cubData_a;
        memcpy_s(pubData, cubData, pubData_a, cubData_a);
        eP2PSendType = eP2PSendType_a;
        nChannel = nChannel_a;
    };

    ~PacketStorageData()
    {
        free(pubData);
        pubData = nullptr;
    }
};

//Once the lobby setup is complete, we know what networking API is supported and can send over the packets queued during this setup period
//This is needed because if we try to talk to a user with the wrong API, steam internals can get fucked up and the connection can just randomly die.
static std::vector<PacketStorageData*> queued_packets;

void ClearQueuedPackets()
{
    ConsoleWrite("Cleared queued packets. Size=%d", queued_packets.size());
    for (auto elem : queued_packets)
    {
        delete elem;
    }
    queued_packets.clear();
}

void RemoveQueuedPackets(uint64_t steamid)
{
    ConsoleWrite("Removing queued packets for %llx. Size=%d", steamid, queued_packets.size());
    std::vector<PacketStorageData*>::iterator pkt_it = queued_packets.begin();
    while (pkt_it != queued_packets.end())
    {
        auto elem = *pkt_it;
        if (elem->steamIDRemote.ConvertToUint64() == steamid)
        {
            pkt_it = queued_packets.erase(pkt_it);
            delete elem;
        }
        else
        {
            pkt_it++;
        }
    }
    ConsoleWrite("Queued packets remaining: %d", queued_packets.size());
}

void SendQueuedPackets()
{
    //send out all the packets that were queued during this time, and remove them as we send them
    std::vector<PacketStorageData*>::iterator pkt_it = queued_packets.begin();
    while (pkt_it != queued_packets.end())
    {
        auto elem = *pkt_it;
        if (SteamAPIStatusKnown_Users.count(elem->steamIDRemote.ConvertToUint64()) != 0)
        {
            bool sent = SendP2PPacket_Replacement_injection_helper(elem->steamIDRemote, elem->pubData, elem->cubData, elem->eP2PSendType, elem->nChannel);
            ConsoleWrite("Sending queued packets: total %d was_sent=%d", queued_packets.size(), sent);
            pkt_it = queued_packets.erase(pkt_it);
            delete elem;
        }
        else
        {
            pkt_it++;
        }
    }
    ConsoleWrite("Queued packets remaining: %d", queued_packets.size());
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

    ConsoleWrite("AcceptSessionWithUser %llx", user.ConvertToUint64());
    ModNetworking::SteamNetMessages->AcceptSessionWithUser(pCallback->m_identityRemote);
}

static std::queue<SteamNetworkingMessage_t*> messages;

//IsP2PPacketAvailable
bool IsP2PPacketAvailable_Replacement_injection_helper(uint32 *pcubMsgSize, int nChannel)
{
    bool is_available = false;
    if (nChannel != 0)
    {
        ConsoleWrite("WARNING: Why the fuck is Dark Souls not checking on channel 0. Please report me.");
        nChannel = 0;
    }

    /*
    * There is no equivilent in the NetworkingMessages API, so just grab a message if one is present and save it
    * Grab this saved message and return it in the upcoming ReadP2PPacket call
    */
    SteamNetworkingMessage_t* new_message = new SteamNetworkingMessage_t();
    int num_messages = ModNetworking::SteamNetMessages->ReceiveMessagesOnChannel(nChannel, &new_message, 1);

    if (num_messages == 1)
    {
        *pcubMsgSize = new_message->m_cbSize;
        messages.push(new_message);
        is_available |= true;
    }
    //Otherwise fall back to the Networking interface (this fallback order is important. NetworkingMessages, then old Networking)
    else
    {
        is_available |= ModNetworking::SteamNetworking->IsP2PPacketAvailable(pcubMsgSize, nChannel);
    }

    return is_available;
}

//ReadP2PPacket/ReceiveMessagesOnChannel
bool ReadP2PPacket_Replacement_injection_helper(void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, CSteamID *psteamIDRemote, int nChannel)
{
    if (nChannel != 0)
    {
        ConsoleWrite("WARNING: Why the fuck is Dark Souls not reading on channel 0. Please report me.");
    }

    //If we have a packet from the NetworkingMessages interface to return, return it
    if (messages.size() > 0 && nChannel == 0)
    {
        //fill out the given args from the saved network message struct
        SteamNetworkingMessage_t* cur_message = messages.front();
        messages.pop();

        memcpy_s(pubDest, cubDest, cur_message->m_pData, cur_message->m_cbSize);
        if (pcubMsgSize != nullptr)
        {
            *pcubMsgSize = cur_message->m_cbSize;
        }
        if (psteamIDRemote != nullptr)
        {
            *psteamIDRemote = cur_message->m_identityPeer.GetSteamID();
        }

        cur_message->Release();
        return true;
    }
    //otherwise see if there's a message from the Networking interface and return that
    else
    {
        return ModNetworking::SteamNetworking->ReadP2PPacket(pubDest, cubDest, pcubMsgSize, psteamIDRemote, nChannel);
    }
}

//SendP2PPacket/SendMessageToUser
bool SendP2PPacket_Replacement_injection_helper(CSteamID steamIDRemote, const void *pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel)
{
    if (nChannel != 0)
    {
        ConsoleWrite("WARNING: Why the fuck is Dark Souls not sending on channel 0. Please report me.");
    }

    // Handle disconnecting a user
    if (ModNetworking::incoming_guest_to_not_accept != 0 && ModNetworking::incoming_guest_to_not_accept == steamIDRemote.ConvertToUint64())
    {
        //ConsoleWrite("Forcing d/c on user %lld", steamIDRemote.ConvertToUint64());
        return false;
    }

    //Any packet sent during this period where we don't know the other user's API nature, save them to send later.
    if (SteamAPIStatusKnown_Users.count(steamIDRemote.ConvertToUint64()) == 0)
    {
        //need to verify that they're in the session, since sometimes the game sends packets even after they've left
        int numlobbymembers = ModNetworking::SteamMatchmaking->GetNumLobbyMembers(ModNetworking::currentLobby);
        for (int i = 0; i < numlobbymembers; i++)
        {
            CSteamID member = ModNetworking::SteamMatchmaking->GetLobbyMemberByIndex(ModNetworking::currentLobby, i);
            if (member == steamIDRemote)
            {
                queued_packets.push_back(new PacketStorageData(steamIDRemote, pubData, cubData, eP2PSendType, nChannel));
                ConsoleWrite("Saving queued packets for %llx: total %d", steamIDRemote.ConvertToUint64(), queued_packets.size());
                return true;
            }
        }
    }

    if (SteamNetworkingMessages_Supported(steamIDRemote))
    {
        SteamNetworkingIdentity target;
        target.SetSteamID(steamIDRemote);

        int nSendFlags = k_nSteamNetworkingSend_AutoRestartBrokenSession; //default behavior in original netcode
        switch (eP2PSendType)
        {
            case k_EP2PSendUnreliable:
                nSendFlags |= k_nSteamNetworkingSend_UnreliableNoNagle;
                break;
            case k_EP2PSendUnreliableNoDelay:
                nSendFlags |= k_nSteamNetworkingSend_UnreliableNoDelay;
                break;
            case k_EP2PSendReliable:
                nSendFlags |= k_nSteamNetworkingSend_ReliableNoNagle;
                break;
            case k_EP2PSendReliableWithBuffering:
                nSendFlags |= k_nSteamNetworkingSend_Reliable;
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
    //In theory we should never still have the user saved when the game hits this, so just return that we already closed them
    if (SteamAPIStatusKnown_Users.count(steamIDRemote.ConvertToUint64()) == 0)
    {
        return true;
    }

    //Have this just in case
    ConsoleWrite("Dark Souls manually closing session with %llx", steamIDRemote.ConvertToUint64());
    if (SteamNetworkingMessages_Supported(steamIDRemote))
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
const char* MOD_LOBBY_USERAPPROVED_KEY = "DarkSoulsOverhaulModUserApproved";

bool ModNetworking::allow_connect_with_non_mod_host = true;
bool ModNetworking::allow_connect_with_legacy_mod_host = true;
bool ModNetworking::allow_connect_with_overhaul_mod_host = true;
bool ModNetworking::allow_connect_with_non_mod_guest = true;

bool ModNetworking::host_got_info = false;
bool ModNetworking::host_mod_installed = false;
ModMode ModNetworking::host_mod_mode;
bool ModNetworking::incoming_guest_got_info = false;
bool ModNetworking::incoming_guest_mod_installed = false;
ModMode ModNetworking::incoming_guest_mod_mode;
uint64_t ModNetworking::incoming_guest_to_not_accept = 0; //The steamid of the incoming user if we want to d/c them. When AcceptSessionWithUser/AcceptP2PSessionWithUser is called with this id, decline the session

uint64_t ModNetworking::currentLobby = 0;

//Used to prevent LobbyDataUpdateCallback (run from a seperate thread) from interrupting and corrupting data GuestAwaitIncomingLobbyData is using
static std::mutex guest_get_host_info_mtx;

//If we know an incoming guest is on the way (they've connected to the lobby)
bool ModNetworking::new_guest_incoming = false;

//Used to prevent the callbacks (run from a seperate thread) from interrupting and corrupting data HostAwaitIncomingGuestMemberData is using
static std::mutex host_get_guest_response_mtx;


bool AcceptP2PSessionWithUser_injection_helper(uint64_t incoming_steamid)
{
    //Check if we want to d/c them
    if (ModNetworking::incoming_guest_to_not_accept != 0 && incoming_steamid == ModNetworking::incoming_guest_to_not_accept)
    {
        ConsoleWrite("Denying connection to d/c'd user %llx", incoming_steamid);
        return false;
    }

    CSteamID id;
    id.SetFromUint64(incoming_steamid);

    //Check they're not blocked
    EFriendRelationship relation = ModNetworking::SteamFriends->GetFriendRelationship(id);
    if (relation == EFriendRelationship::k_EFriendRelationshipIgnored)
    {
        return false;
    }

    //Check they're in the lobby with us
    if (!ModNetworking::SteamFriends->IsUserInSource(id, ModNetworking::currentLobby))
    {
        return false;
    }

    //If we're getting this callback, they're trying to connect with the Networking api, so we should just accept them using that
    //It also means we can immediatly tell they're non-mod, but since this callback doesn't get hit anyway not going to bother with that
    ConsoleWrite("AcceptP2PSessionWithUser %llx", incoming_steamid);
    return ModNetworking::SteamNetworking->AcceptP2PSessionWithUser(id);
}

void HostForceDisconnectGuest(uint64_t steamid, const wchar_t* dc_reason, bool use_old_api = false)
{
    //Ensure we don't implicitly accept the connection via calling SendP2PPacket to this user
    ModNetworking::incoming_guest_to_not_accept = steamid;

    //Disconnect any existing session we have via CloseP2PSessionWithUser / CloseSessionWithUser
    if (SteamNetworkingMessages_Supported(CSteamID(steamid)) && !use_old_api)
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
    Game::show_banner_message(dc_msg);
}

typedef struct
{
    uint64_t start_time;
} GuestAwaitIncomingLobbyData_Struct;

static const uint32_t MS_TO_WAIT_FOR_HOST_DATA = (uint32_t)(0.25 * 10000000);

bool GuestAwaitIncomingLobbyData(void* unused);

//This is called only when the local user joins a lobby. Not when anyone else we're connected to does.
void ModNetworking::LobbyEnterCallback(LobbyEnter_t* pCallback)
{
    //Painted Worlds fix
    if (strlen(ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, "PaintedWorlds")) != 0)
    {
        ConsoleWrite("Ignore PaintedWorlds prejoin lobby");
        return;
    }

    CSteamID lobbyowner = ModNetworking::SteamMatchmaking->GetLobbyOwner(pCallback->m_ulSteamIDLobby);
    CSteamID selfsteamid = ModNetworking::SteamUser->GetSteamID();
    ModNetworking::currentLobby = pCallback->m_ulSteamIDLobby;
    ModNetworking::incoming_guest_to_not_accept = 0; //prevent any holdover data on new lobby
    SteamAPIStatusKnown_Users.clear();
    ClearQueuedPackets(); //the game may still send packets to users after they D/C, so clean any leftovers out

    // H1. As the host, upon lobby creation(and self entry into it) set the lobby data to inform connecting users of our mod status
    // Since lobby data is persistent, we don't have to worry about resending anything for new connections
    if (lobbyowner == selfsteamid)
    {
        ConsoleWrite("H1. Host set lobby data = %hhx", Mod::get_mode());
        ModNetworking::SteamMatchmaking->SetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(Mod::get_mode()));
        ModNetworking::SteamMatchmaking->SetLobbyMemberData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(Mod::get_mode()));
        return;
    }

    // G1. If we're the guest, upon lobby connection check for lobby data on mod status
    // This callback is only triggered when WE attempt to enter a lobby, so existing guests won't have this trigger
    // The lobby data may not arrive for a bit (since the host JITs the lobby for a new connection and we then add the lobby data right after), so set up a callback to trigger when we do get the data (if we get it)
    // Waiting until we get this data before sending our confirmation as guest we also have the mod does eat into the HostAwaitIncomingGuestMemberData timeout a bit, but probably not by any more then a 1/2 of the ping
    if (lobbyowner != selfsteamid)
    {
        ConsoleWrite("G1. Guest joined lobby. Start waiting for incoming lobby data");
        GuestAwaitIncomingLobbyData_Struct* data = (GuestAwaitIncomingLobbyData_Struct*)malloc(sizeof(GuestAwaitIncomingLobbyData_Struct));
        data->start_time = Game::get_accurate_time();
        MainLoop::setup_mainloop_callback(GuestAwaitIncomingLobbyData, data, "GuestAwaitIncomingLobbyData");
        return;
    }
}

void ModNetworking::LobbyDataUpdateCallback(LobbyDataUpdate_t* pCallback)
{
    //Painted Worlds fix
    if (strlen(ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, "PaintedWorlds")) != 0)
    {
        ConsoleWrite("Ignore PaintedWorlds prejoin lobby");
        return;
    }

    CSteamID lobbyowner = ModNetworking::SteamMatchmaking->GetLobbyOwner(pCallback->m_ulSteamIDLobby);
    CSteamID selfsteamid = ModNetworking::SteamUser->GetSteamID();

    // G2. If we're the guest, listen to the callback for lobby data updates
    // This will be triggered upon joining a lobby, and also upon reciving any lobby data updates
    // Make sure if for some reason we get more data later, after we're connected, we ignore it.
    // Otherwise it might set flags in a bad state and corrupt the next host we join
    if (pCallback->m_ulSteamIDMember == pCallback->m_ulSteamIDLobby && lobbyowner != selfsteamid && SteamAPIStatusKnown_Users.count(lobbyowner.ConvertToUint64()) == 0)
    {
        guest_get_host_info_mtx.lock();

        const char* mod_str = ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY);
        ModMode mod_value = ModMode::InvalidMode;
        if (String_To_ModModes.count(mod_str) != 0)
        {
            mod_value = String_To_ModModes.at(mod_str);
        }
        ConsoleWrite("G2. Guest get lobby data = %s/%hhx", mod_str, mod_value);

        if (mod_value == ModMode::InvalidMode)
        {
            ModNetworking::host_mod_installed = false;
        }
        else
        {
            ModNetworking::host_mod_installed = true;
            ModNetworking::host_mod_mode = mod_value;
            ModNetworking::host_got_info = true;
        }

        guest_get_host_info_mtx.unlock();
    }
    // G5. If we do get more data later, after we're connected, then the host is telling us that we need to change modes, or is triggering the approval of the new guest
    if (pCallback->m_ulSteamIDMember == pCallback->m_ulSteamIDLobby && lobbyowner != selfsteamid && SteamAPIStatusKnown_Users.count(lobbyowner.ConvertToUint64()) != 0)
    {
        const char* mod_str = ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY);
        ModMode mod_value = ModMode::InvalidMode;
        if (String_To_ModModes.count(mod_str) != 0)
        {
            mod_value = String_To_ModModes.at(mod_str);
        }

        const char* approve_user_str = ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_USERAPPROVED_KEY);
        uint64_t approve_user_value = strtoull(approve_user_str, NULL, 10);

        //change mode
        if (mod_value != ModMode::InvalidMode && mod_value != ModNetworking::host_mod_mode)
        {
            ConsoleWrite("G5. Guest get updated lobby data = %s/%hhx", mod_str, mod_value);

            Mod::set_mode(mod_value);
            ModNetworking::host_mod_mode = mod_value;

            //updating our lobby member data isn't needed for now, but do just in case
            ModNetworking::SteamMatchmaking->SetLobbyMemberData(ModNetworking::currentLobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(Mod::get_mode()));

            ConsoleWrite("G5. Guest set updated member data = %hhx", mod_value);
        }

        //approve host allowed incoming user
        if (approve_user_value != 0 && approve_user_value != ULLONG_MAX && SteamAPIStatusKnown_Users.count(approve_user_value) == 0 && selfsteamid != approve_user_value)
        {
            //need to verify that they're in the session, since if they left and the host changes modes we might approve someone not in the session
            //by this point, their member data should have been sent (since the host got it), so we can also use it
            int numlobbymembers = ModNetworking::SteamMatchmaking->GetNumLobbyMembers(ModNetworking::currentLobby);
            bool approved_in_session = false;
            ModMode user_mode = ModMode::InvalidMode;
            for (int i = 0; i < numlobbymembers; i++)
            {
                CSteamID member = ModNetworking::SteamMatchmaking->GetLobbyMemberByIndex(ModNetworking::currentLobby, i);
                if (member.ConvertToUint64() == approve_user_value)
                {
                    approved_in_session = true;
                    const char* mod_guest_value_str = ModNetworking::SteamMatchmaking->GetLobbyMemberData(pCallback->m_ulSteamIDLobby, member, MOD_LOBBY_DATA_KEY);
                    if (mod_guest_value_str != NULL && String_To_ModModes.count(mod_guest_value_str) != 0)
                    {
                        user_mode = String_To_ModModes.at(mod_guest_value_str);
                    }
                    break;
                }
            }

            if (approved_in_session)
            {
                ConsoleWrite("G5. Guest approves user = \"%s\"/%llx", approve_user_str, approve_user_value);
                SteamAPIStatusKnown_Users.insert_or_assign(approve_user_value, user_mode != ModMode::InvalidMode);
                //now that we know the user, we can send the queued packets
                SendQueuedPackets();
            }
        }
    }

    // H3. If we're the host, listen for the response member data info from the guest saying they respect these settings
    // Make sure we stop listening once we've finalized this guest.
    // Otherwise, this data could arrive late and when a new guest arrives flags might apply to them incorrectly
    if (pCallback->m_ulSteamIDMember != pCallback->m_ulSteamIDLobby && pCallback->m_ulSteamIDMember != selfsteamid.ConvertToUint64() && lobbyowner == selfsteamid && ModNetworking::new_guest_incoming)
    {
        host_get_guest_response_mtx.lock();

        const char* mod_guest_value_str = ModNetworking::SteamMatchmaking->GetLobbyMemberData(pCallback->m_ulSteamIDLobby, pCallback->m_ulSteamIDMember, MOD_LOBBY_DATA_KEY);
        ModMode mod_guest_value = ModMode::InvalidMode;
        if (mod_guest_value_str != NULL && String_To_ModModes.count(mod_guest_value_str) != 0)
        {
            mod_guest_value = String_To_ModModes.at(mod_guest_value_str);
        }
        ConsoleWrite("H3. Host got guest data = %s/%hhx", mod_guest_value_str, mod_guest_value);

        if (mod_guest_value == ModMode::InvalidMode)
        {
            ModNetworking::incoming_guest_mod_installed = false;
        }
        else
        {
            ModNetworking::incoming_guest_mod_installed = true;
            ModNetworking::incoming_guest_mod_mode = mod_guest_value;
            ModNetworking::incoming_guest_got_info = true;
        }

        host_get_guest_response_mtx.unlock();
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

    //try and lock here. If we can't, don't block (since that blocks DSR's main thread). We'll just run next frame anyway
    bool locked = guest_get_host_info_mtx.try_lock();
    if (!locked)
    {
        return true;
    }

    //Timeout, host is non-mod user
    if (ModNetworking::host_got_info == false && (Game::get_accurate_time() > data->start_time + MS_TO_WAIT_FOR_HOST_DATA))
    {
        ConsoleWrite("G3. Guest timeout getting host info");
        ModNetworking::host_mod_installed = false;
    }

    // 3. As the guest, we must change our settings to match the host (based on how we've configured our options), or else disconnect.
    if (ModNetworking::host_mod_installed == false && ModNetworking::allow_connect_with_non_mod_host == true)
    {
        //connecting to non-mod
        Mod::set_mode(ModMode::Compatability);
    }
    else if (ModNetworking::host_mod_installed == true && ModNetworking::host_mod_mode == ModMode::Compatability && ModNetworking::allow_connect_with_non_mod_host == true)
    {
        //connecting to (basically) non-mod
        Mod::set_mode(ModMode::Compatability);
    }
    else if (ModNetworking::host_mod_installed == true && ModNetworking::host_mod_mode == ModMode::Overhaul && ModNetworking::allow_connect_with_overhaul_mod_host == true)
    {
        //connecting to overhaul
        Mod::set_mode(ModMode::Overhaul);
    }
    else if (ModNetworking::host_mod_installed == true && ModNetworking::host_mod_mode == ModMode::Legacy && ModNetworking::allow_connect_with_legacy_mod_host == true)
    {
        //connecting to legacy
        Mod::set_mode(ModMode::Legacy);
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
        else if (ModNetworking::host_mod_installed && ModNetworking::host_mod_mode == ModMode::Compatability && ModNetworking::allow_connect_with_non_mod_host == false)
        {
            wcscat_s(dc_msg, L"Host has a non-mod user connected and you do not allow connections with non-mod users.");
        }
        else if (ModNetworking::host_mod_installed && ModNetworking::host_mod_mode == ModMode::Overhaul && ModNetworking::allow_connect_with_overhaul_mod_host == false)
        {
            wcscat_s(dc_msg, L"Host is in overhaul mode and you do not allow connections with overhaul mode users.");
        }
        else if (ModNetworking::host_mod_installed && ModNetworking::host_mod_mode == ModMode::Legacy && ModNetworking::allow_connect_with_legacy_mod_host == false)
        {
            wcscat_s(dc_msg, L"Host is in legacy mode and you do not allow connections with legacy mode users.");
        }
        Game::show_popup_message(dc_msg);
        ConsoleWrite("G3. Guest disconnecting due to settings");

        //disconnect
        ModNetworking::SteamMatchmaking->LeaveLobby(ModNetworking::currentLobby);
        ModNetworking::currentLobby = 0;
        SteamAPIStatusKnown_Users.clear();
        ModNetworking::host_got_info = false;
        guest_get_host_info_mtx.unlock();
        ClearQueuedPackets();
        free(data);
        return false;
    }

    //set our lobby member data confirming we also respect these settings and have the mod
    ModNetworking::SteamMatchmaking->SetLobbyMemberData(ModNetworking::currentLobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(Mod::get_mode()));
    ConsoleWrite("G3. Guest set member data = %hhx", Mod::get_mode());

    //we now lookup the status of the guest and any connected hosts
    //we can safely assume their lobby member data is set before we get there
    int numlobbymembers = ModNetworking::SteamMatchmaking->GetNumLobbyMembers(ModNetworking::currentLobby);
    for (int i = 0; i < numlobbymembers; i++)
    {
        CSteamID member = ModNetworking::SteamMatchmaking->GetLobbyMemberByIndex(ModNetworking::currentLobby, i);
        const char* user_mod_value_str = ModNetworking::SteamMatchmaking->GetLobbyMemberData(ModNetworking::currentLobby, member, MOD_LOBBY_DATA_KEY);
        ModMode user_mod_value = ModMode::InvalidMode;
        if (user_mod_value_str != NULL && String_To_ModModes.count(user_mod_value_str) != 0)
        {
            user_mod_value = String_To_ModModes.at(user_mod_value_str);
        }
        SteamAPIStatusKnown_Users.insert_or_assign(member.ConvertToUint64(), user_mod_value != ModMode::InvalidMode);
    }
    //this also means it's safe to send out the queued packets for everyone
    SendQueuedPackets();

    //this flag is finished, we don't need it anymore. Reset for next time we connect to a host.
    ModNetworking::host_got_info = false;
    guest_get_host_info_mtx.unlock();
    free(data);
    return false;
}

typedef struct
{
    uint64_t start_time;
    uint64_t steamid;
} AwaitIncomingUserMemberData_Struct;

static const uint32_t MS_TO_WAIT_FOR_GUEST_MSG = (uint32_t)(1.0 * 10000000);

bool HostAwaitIncomingGuestMemberData(void* data_a);
bool GuestAwaitIncomingGuestMemberData(void* data_a);

//This is called when another user has joined/left the lobby.
void ModNetworking::LobbyChatUpdateCallback(LobbyChatUpdate_t* pCallback)
{
    //Painted Worlds fix
    if (strlen(ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, "PaintedWorlds")) != 0)
    {
        ConsoleWrite("Ignore PaintedWorlds prejoin lobby");
        return;
    }

    CSteamID lobbyowner = ModNetworking::SteamMatchmaking->GetLobbyOwner(pCallback->m_ulSteamIDLobby);
    CSteamID selfsteamid = ModNetworking::SteamUser->GetSteamID();
    ModNetworking::incoming_guest_to_not_accept = 0; //make sure a reconnecting user has another chance (this is reset on both leave and join)
    host_get_guest_response_mtx.lock();
    ModNetworking::new_guest_incoming = true;
    host_get_guest_response_mtx.unlock();

    // H2. As the host, when a new user connects to the lobby set a timer to wait for their response member data setting
    // Need to handle the case where they don't set any member data, and thus are a non-mod user and we must disconnect them from this lobby
    if (lobbyowner == selfsteamid && pCallback->m_rgfChatMemberStateChange == EChatMemberStateChange::k_EChatMemberStateChangeEntered && pCallback->m_ulSteamIDUserChanged != selfsteamid.ConvertToUint64())
    {
        ConsoleWrite("H2. Host saw user %llx connect. Start waiting for incoming guest member data", pCallback->m_ulSteamIDUserChanged);
        AwaitIncomingUserMemberData_Struct* data = (AwaitIncomingUserMemberData_Struct*)malloc(sizeof(AwaitIncomingUserMemberData_Struct));
        data->start_time = Game::get_accurate_time();
        data->steamid = pCallback->m_ulSteamIDUserChanged;
        MainLoop::setup_mainloop_callback(HostAwaitIncomingGuestMemberData, data, "HostAwaitIncomingGuestMemberData");
    }

    // H4. Host: Handle the case where the guest disconnects on their own
    if (lobbyowner == selfsteamid && pCallback->m_rgfChatMemberStateChange == EChatMemberStateChange::k_EChatMemberStateChangeLeft && pCallback->m_ulSteamIDUserChanged != selfsteamid.ConvertToUint64())
    {
        ConsoleWrite("H4. Guest %llx d/c'd on their own.", pCallback->m_ulSteamIDUserChanged);

        host_get_guest_response_mtx.lock();
        ModNetworking::new_guest_incoming = false;
        host_get_guest_response_mtx.unlock();

        //Close the session before we erase the user so we do the right closure
        if (SteamNetworkingMessages_Supported(pCallback->m_ulSteamIDUserChanged))
        {
            SteamNetworkingIdentity remote;
            remote.SetSteamID64(pCallback->m_ulSteamIDUserChanged);
            ModNetworking::SteamNetMessages->CloseSessionWithUser(remote);
        }
        else
        {
            CSteamID remote;
            remote.SetFromUint64(pCallback->m_ulSteamIDUserChanged);
            ModNetworking::SteamNetworking->CloseP2PSessionWithUser(remote);
        }

        SteamAPIStatusKnown_Users.erase(pCallback->m_ulSteamIDUserChanged); //remove the user from the known list
        RemoveQueuedPackets(pCallback->m_ulSteamIDUserChanged); //get rid of the queued packets for this user since we don't need them anymore

        // We can switch modes back to non-compatability if this list is empty now
        bool all_mod_users = true;
        for (const auto &[user, is_mod] : SteamAPIStatusKnown_Users)
        {
            if (!is_mod)
            {
                all_mod_users = false;
                break;
            }
        }
        if (all_mod_users && Mod::get_mode() == ModMode::Compatability)
        {
            Mod::set_mode(Mod::user_selected_default_mode);
            ConsoleWrite("H1. Host reset lobby data = %hhx", Mod::get_mode());
            ModNetworking::SteamMatchmaking->SetLobbyData(ModNetworking::currentLobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(Mod::get_mode()));
        }
    }

    // G4. If we're a guest, and a new user is joining, we need to queue and send our packets only after we know what API they use
    // (Guest packets are required for a new guest to connect)
    // We won't know the API until the host tells up the new mode, however. So we have to save this user and watch for that mode change, and only approve them then
    // If we don't get a mode change, and they D/C (are forced to by the host), then we can stop waiting to approve them
    if (lobbyowner != selfsteamid && pCallback->m_rgfChatMemberStateChange == EChatMemberStateChange::k_EChatMemberStateChangeEntered && pCallback->m_ulSteamIDUserChanged != selfsteamid.ConvertToUint64())
    {
        //if the host has already changed to Compatability or is a non-mod user, this waiting isn't needed and we can skip right to approval
        if (ModNetworking::host_mod_installed == false || ModNetworking::host_mod_mode == ModMode::Compatability)
        {
            ConsoleWrite("G4. Guest detected new guest is trying to join. Auto-approved");
            //check in case this connecting user also has the mod, even if the host doesn't
            AwaitIncomingUserMemberData_Struct* data = (AwaitIncomingUserMemberData_Struct*)malloc(sizeof(AwaitIncomingUserMemberData_Struct));
            data->start_time = Game::get_accurate_time();
            data->steamid = pCallback->m_ulSteamIDUserChanged;
            MainLoop::setup_mainloop_callback(GuestAwaitIncomingGuestMemberData, data, "GuestAwaitIncomingGuestMemberData");
        }
        else
        {
            ConsoleWrite("G4. Guest detected new guest is trying to join. Waiting for host approval.");
        }
    }

    // G4. Guest: Handle the case where the guest disconnects
    if (lobbyowner != selfsteamid && pCallback->m_rgfChatMemberStateChange == EChatMemberStateChange::k_EChatMemberStateChangeLeft && pCallback->m_ulSteamIDUserChanged != selfsteamid.ConvertToUint64())
    {
        ConsoleWrite("G4. Guest detected another guest has D/C'd.");

        //this shouldn't be needed for the guest, but just in case
        host_get_guest_response_mtx.lock();
        ModNetworking::new_guest_incoming = false;
        host_get_guest_response_mtx.unlock();

        SteamAPIStatusKnown_Users.erase(pCallback->m_ulSteamIDUserChanged); //remove the user from the known list
        RemoveQueuedPackets(pCallback->m_ulSteamIDUserChanged);
    }
}

bool HostAwaitIncomingGuestMemberData(void* data_a)
{
    AwaitIncomingUserMemberData_Struct* data = (AwaitIncomingUserMemberData_Struct*)data_a;
    std::string approveduser;

    //No timeout yet, back to sleep
    if (ModNetworking::incoming_guest_got_info == false && !(Game::get_accurate_time() > data->start_time + MS_TO_WAIT_FOR_GUEST_MSG))
    {
        return true;
    }

    //try and lock here. If we can't, don't block (since that blocks DSR's main thread). We'll just run next frame anyway
    bool locked = host_get_guest_response_mtx.try_lock();
    if (!locked)
    {
        return true;
    }

    // If the guest disconnected on their own
    if (ModNetworking::new_guest_incoming == false)
    {
        ConsoleWrite("H4. Host detects that guest d/c'd on their own.");
        Game::show_banner_message(L"Guest declined to connect to your world due to their settings.");
        goto exit;
    }

    //Timed out waiting for a message from the incoming guest, so they're a non-mod user
    else if (ModNetworking::incoming_guest_got_info == false && Game::get_accurate_time() > data->start_time + MS_TO_WAIT_FOR_GUEST_MSG)
    {
        ConsoleWrite("H4. Host detects incoming guest is non-mod user.");
        ModNetworking::incoming_guest_mod_installed = false;

        //As the host, we only change our settings if the connecting user is non-mod and we allow non-mod connections
        if (ModNetworking::allow_connect_with_non_mod_guest == true && Mod::get_mode() != ModMode::Compatability)
        {
            Mod::set_mode(ModMode::Compatability);
        }
        // If specified in options, we must disconnect the non-mod player, since they won't on their own
        else if (ModNetworking::allow_connect_with_non_mod_guest == false)
        {
            ConsoleWrite("H4. Incoming guest is non-mod user, and host do not allow connections with non-mod users.");
            HostForceDisconnectGuest(data->steamid, L"Incoming guest is non-mod user, and you do not allow connections with non-mod users.");
            goto exit;
        }
    }

    // If the guest isn't respecting our settings for some reason (can't hurt to check), disconnect them right away
    else if (ModNetworking::incoming_guest_got_info == true && ModNetworking::incoming_guest_mod_installed == true && ModNetworking::incoming_guest_mod_mode != Mod::get_mode())
    {
        ConsoleWrite("H4. Host detects incoming guest is mod-user, but isn't matching the legacy/overhaul mode. Host=%d, guest=%d.", static_cast<int>(Mod::get_mode()), static_cast<int>(ModNetworking::incoming_guest_mod_mode));
        HostForceDisconnectGuest(data->steamid, L"Incoming guest is mod-user, but isn't matching your legacy/overhaul mode.");
        goto exit;
    }

    //Success state for a new guest
    ConsoleWrite("H4. Host allows guest in!");

    //Trigger a change to the lobby data, even if not needed.
    //If it's unchanged, steam just won't resend it
    ConsoleWrite("H4. Host update lobby data = %hhx", Mod::get_mode());
    ModNetworking::SteamMatchmaking->SetLobbyData(ModNetworking::currentLobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(Mod::get_mode()));

    //Let guests know the host approves this user
    approveduser = std::to_string(data->steamid);
    ConsoleWrite("H4. Host update lobby user approval = %llx", data->steamid);
    ModNetworking::SteamMatchmaking->SetLobbyData(ModNetworking::currentLobby, MOD_LOBBY_USERAPPROVED_KEY, approveduser.c_str());

    //We now know this user's API, so we can send the queued packets
    SteamAPIStatusKnown_Users.insert_or_assign(data->steamid, ModNetworking::incoming_guest_mod_installed);
    SendQueuedPackets();


    //reset for next guest
    exit:
    ModNetworking::incoming_guest_got_info = false;
    ModNetworking::new_guest_incoming = false;
    host_get_guest_response_mtx.unlock();
    RemoveQueuedPackets(data->steamid); //get rid of the queued packets if we ended up D/Cing the user (no-op if we accepted them)
    free(data);
    return false;
}

bool GuestAwaitIncomingGuestMemberData(void* data_a)
{
    AwaitIncomingUserMemberData_Struct* data = (AwaitIncomingUserMemberData_Struct*)data_a;

    const char* mod_guest_value_str = ModNetworking::SteamMatchmaking->GetLobbyMemberData(ModNetworking::currentLobby, data->steamid, MOD_LOBBY_DATA_KEY);
    ModMode mod_guest_value = ModMode::InvalidMode;
    if (mod_guest_value_str != NULL && String_To_ModModes.count(mod_guest_value_str) != 0)
    {
        mod_guest_value = String_To_ModModes.at(mod_guest_value_str);
    }

    //No timeout yet, back to sleep
    if (mod_guest_value == ModMode::InvalidMode && !(Game::get_accurate_time() > data->start_time + MS_TO_WAIT_FOR_GUEST_MSG))
    {
        return true;
    }

    //either we timed out and they're non-mod, or we got their status
    SteamAPIStatusKnown_Users.insert_or_assign(data->steamid, mod_guest_value != ModMode::InvalidMode);
    SendQueuedPackets();

    return false;
}
