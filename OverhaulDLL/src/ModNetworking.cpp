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
#include "PlayerVisualsValidationFix.h"

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
    bool SendP2PPacket_Replacement_injection_helper(CSteamID steamIDRemote, void *pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel);

    uint64_t CloseP2PSessionWithUser_Replacement_injection_return;
    void CloseP2PSessionWithUser_Replacement_injection();
    bool CloseP2PSessionWithUser_Replacement_injection_helper(CSteamID steamIDRemote);

    uint64_t Start_SessionDisconnect_Task_injection_return;
    void Start_SessionDisconnect_Task_injection();
    void Start_SessionDisconnect_Task_injection_helper(uint32_t);

    uint64_t Steam_LeaveLobby_injection_return;
    void Steam_LeaveLobby_injection();
    void Steam_LeaveLobby_injection_helper(uint64_t);

    uint64_t Steam_CreateLobby_injection_return;
    void Steam_CreateLobby_injection();
    void Steam_CreateLobby_injection_helper(void*);
}

typedef void* gfGetSteamInterface(int iSteamUser, int iUnkInt, const char* pcVersion, const char* pcInterface);
typedef ISteamClient* gfCreateSteamInterface(const char* pSteamClientVer, uint32_t iUnkZero);
ISteamUser* ModNetworking::SteamUser = nullptr;
ISteamFriends* ModNetworking::SteamFriends = nullptr;
ISteamMatchmaking* ModNetworking::SteamMatchmaking = nullptr;
ISteamNetworking* ModNetworking::SteamNetworking = nullptr;
ISteamNetworkingMessages* ModNetworking::SteamNetMessages = nullptr;
ISteamNetworkingUtils* ModNetworking::SteamNetUtils = nullptr;

//Connect to steam, and create a persistant lobby for the mod
bool Init_ISteam_Interfaces_And_Lobby(void* unused)
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

    ModNetworking::CreatePersistentLobby();

    return false;
}

//Create a lobby for this player.
//Normally the lobby is created right before another player connects, but there is no way to create a lobby with data,
//and setting the lobby data after creation causes a delay before its gets set which can cause the lobby to appear to have no data.
//This used to be handled with timeouts, but having an existing lobby that has been around long enough to get the data set is better.
CCallResult<ModNetworking, LobbyCreated_t> ModNetworking::LobbyCreatedCallResult;

void ModNetworking::CreatePersistentLobby()
{
    SteamAPICall_t lobby_created = ModNetworking::SteamMatchmaking->CreateLobby(ELobbyType::k_ELobbyTypePublic, 6);
    ModNetworking::LobbyCreatedCallResult.Set(lobby_created, &modnet, &ModNetworking::LobbyCreatedCallback);
}

//Await the result of creating the persistant lobby, and set it's data for the current mode
void ModNetworking::LobbyCreatedCallback(LobbyCreated_t* pCallback, bool bIOFailure)
{
    //if we're offline, just don't bother doing anything
    if (pCallback->m_eResult == k_EResultNoConnection)
    {
        return;
    }
    if (bIOFailure || pCallback->m_eResult != k_EResultOK)
    {
        FATALERROR("LobbyCreatedCallback failed");
    }
    ConsoleWrite("Created Persistent Lobby");
    ModNetworking::selfPersisantLobby.store(pCallback->m_ulSteamIDLobby);
    UpdatePersistentLobbyData(Mod::get_mode());
}

const char* MOD_LOBBY_DATA_KEY = "DarkSoulsOverhaulModData";
const char* MOD_LOBBY_USERAPPROVED_KEY = "DarkSoulsOverhaulModUserApproved";

//Set the persistant lobby data for what mode we are in
void ModNetworking::UpdatePersistentLobbyData(ModMode mode)
{
    ConsoleWrite("Set lobby data = %hhx", mode);
    uint64_t lobby = ModNetworking::selfPersisantLobby.load();
    ModNetworking::SteamMatchmaking->SetLobbyData(lobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(mode));
    ModNetworking::SteamMatchmaking->SetLobbyMemberData(lobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(mode));
}

void ModNetworking::start()
{
    ConsoleWrite("Enabling mod networking...");

    uint8_t *write_address;

    /*
     * Code to initalize the ISteamNetworkingMessages Interface
     */
    MainLoop::setup_mainloop_callback(Init_ISteam_Interfaces_And_Lobby, NULL, "Init_ISteam_Interfaces");

    //Inject code into when the game normally creates it's own lobby to instead use our persistant lobby
    //This ensures we don't have to worry about lobby data not being updated in time for other players
    //This injection makes it so that the function that normally creates a lobby instead sets the lobbyid to our persistant lobby, and returns true
    write_address = (uint8_t*)(ModNetworking::Steam_CreateLobby_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Steam_CreateLobby_injection_return, 0, &Steam_CreateLobby_injection);


    //Inject code into when the game leaves a lobby to re-create our persistant lobby
    //We can only be in one lobby at a time, so we have to re-create it
    write_address = (uint8_t*)(ModNetworking::Steam_LeaveLobby_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Steam_LeaveLobby_injection_return, 1, &Steam_LeaveLobby_injection);

    /*
     * Supporting code for the connection handshake section
     * Inject into the AcceptP2PSessionWithUser callback to be sure we d/c users
     */
    write_address = (uint8_t*)(ModNetworking::AcceptP2PSessionWithUser_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &AcceptP2PSessionWithUser_injection_return, 1, &AcceptP2PSessionWithUser_injection);

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

    /*
     * Prevent the type 34 steam network packet from ever informing the host that we don't see any guests
     * Normally this is a desync prevention measure but the timing is too strict plus it just d/cs the guest anyway
     * So lie and say always just say we aren't missing any guests by returning 0 in the packet
     */
    write_address = (uint8_t*)(ModNetworking::sendType34SteamMessage_data_offset + Game::ds1_base);
    uint8_t type34_data_patch[3] = { 0x45, 0x31, 0xc0 }; //xor r8d, r8d
    sp::mem::patch_bytes(write_address, type34_data_patch, sizeof(type34_data_patch));

    /*
     * Hook the Start_SessionDisconnect_Task function and log the reason a session is ended.
     * Useful for debugging
     */
    write_address = (uint8_t*)(ModNetworking::Start_SessionDisconnect_Task_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Start_SessionDisconnect_Task_injection_return, 1, &Start_SessionDisconnect_Task_injection);
}

void Start_SessionDisconnect_Task_injection_helper(uint32_t reason)
{
    ConsoleWrite("Session disconnection. Reason: %llx", reason);
}

/*
 * ===========ISteamNetworkingMessages REPLACEMENT SECTION
 */

 //We need to keep track of who, specifically, is approved for the new API, even if the lobby is set up
 //This is because if a new non-mod user joins, we want to talk to them correctly and setup everything to use the old API
 //also keep track of if they have the mod installed or not as the value
std::unordered_map<uint64_t, bool> ModNetworking::SteamAPIStatusKnown_Users;

//At the point this is called, we should know the exact nature of the other user. So we should be all set up, mode-wise.
bool SteamNetworkingMessages_Supported(CSteamID steamIDRemote)
{
    //this should never happen, but just in case
    if (!ModNetworking::SteamAPIStatusKnown_Users.contains(steamIDRemote.ConvertToUint64()))
    {
        //TODO this will trigger after a player leaves, haven't fixed yet
        ConsoleWrite("Called SteamNetworkingMessages_Supported for player %llx, but we don't know them", steamIDRemote.ConvertToUint64());
        return false;
    }
    return ModNetworking::SteamAPIStatusKnown_Users[steamIDRemote.ConvertToUint64()];
}

//Helper function for disconnecting guest
std::atomic<uint64_t> ModNetworking::incoming_guest_to_not_accept = 0;

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

void ProcessQueuedPackets(uint64_t steamid)
{
    //send out all the packets for this user that were queued during this time, and remove them as we send them
    ConsoleWrite("Sending queued packets for %llx. Total %d", steamid, queued_packets.size());
    std::vector<PacketStorageData*>::iterator pkt_it = queued_packets.begin();
    while (pkt_it != queued_packets.end())
    {
        auto elem = *pkt_it;
        //Only send packets if it's for this specific user and we know the user's api status
        if (elem->steamIDRemote.ConvertToUint64() == steamid && ModNetworking::SteamAPIStatusKnown_Users.contains(steamid))
        {
            //If their api status is non-mod, since we would have already sent the packet then just delete it
            if (ModNetworking::SteamAPIStatusKnown_Users[steamid] != false)
            {
                bool sent = SendP2PPacket_Replacement_injection_helper(elem->steamIDRemote, elem->pubData, elem->cubData, elem->eP2PSendType, elem->nChannel);
                if (!sent)
                {
                    ConsoleWrite("WARNING: failed sending queued packet type %d", ((uint8_t*)elem->pubData)[1]);
                }
            }
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
    uint64_t incoming_guest_to_not_accept = ModNetworking::incoming_guest_to_not_accept.load();
    if (incoming_guest_to_not_accept != 0 && user.ConvertToUint64() == incoming_guest_to_not_accept)
    {
        ConsoleWrite("Denying connection to d/c'd user %llx", user.ConvertToUint64());
        return;
    }

    //Check they're not blocked
    EFriendRelationship relation = ModNetworking::SteamFriends->GetFriendRelationship(user);
    if (relation == EFriendRelationship::k_EFriendRelationshipIgnored)
    {
        return;
    }

    //Check they're in the lobby with us
    if (!ModNetworking::SteamFriends->IsUserInSource(user, ModNetworking::currentLobby.load()))
    {
        return;
    }

    ConsoleWrite("AcceptSessionWithUser %llx", user.ConvertToUint64());
    ModNetworking::SteamNetMessages->AcceptSessionWithUser(pCallback->m_identityRemote);
}

bool AcceptP2PSessionWithUser_injection_helper(uint64_t incoming_steamid)
{
    //Check if we want to d/c them
    uint64_t incoming_guest_to_not_accept = ModNetworking::incoming_guest_to_not_accept.load();
    if (incoming_guest_to_not_accept != 0 && incoming_steamid == incoming_guest_to_not_accept)
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
    if (!ModNetworking::SteamFriends->IsUserInSource(id, ModNetworking::currentLobby.load()))
    {
        return false;
    }

    ConsoleWrite("AcceptP2PSessionWithUser %llx", incoming_steamid);
    return ModNetworking::SteamNetworking->AcceptP2PSessionWithUser(id);
}

static std::queue<SteamNetworkingMessage_t*> messages;

//IsP2PPacketAvailable
bool IsP2PPacketAvailable_Replacement_injection_helper(uint32 *pcubMsgSize, int nChannel)
{
    bool is_available = false;
    if (nChannel != 0)
    {
        FATALERROR("WARNING: Why the fuck is Dark Souls not checking on channel 0. Please report me.");
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
        FATALERROR("Why the fuck is Dark Souls not reading on channel 0. Please report me.");
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
        bool read_packet_old = ModNetworking::SteamNetworking->ReadP2PPacket(pubDest, cubDest, pcubMsgSize, psteamIDRemote, nChannel);

        //If we're getting this old networking response, they're trying to connect with the Networking api, so we should just accept them using that
        //It also means we can immediatly tell they're non-mod unless we already know otherwise from them (they might be a mod user testing us)
        uint64_t curLobby = ModNetworking::currentLobby.load();
        if (read_packet_old && !ModNetworking::SteamAPIStatusKnown_Users.contains(psteamIDRemote->ConvertToUint64()))
        {
            //As the host, we only change our settings if the connecting user is non-mod and we allow non-mod connections
            if (curLobby == ModNetworking::selfPersisantLobby)
            {
                ConsoleWrite("Host detects incoming guest is non-mod user.");
                if (ModNetworking::allow_connect_with_non_mod_guest == true)
                {
                    if (Mod::get_mode() != ModMode::Compatability)
                    {
                        Mod::set_mode(ModMode::Compatability);
                    }
                    //Let guests know the host approves this user
                    std::string approveduser = std::to_string(psteamIDRemote->ConvertToUint64());
                    ModNetworking::SteamMatchmaking->SetLobbyData(curLobby, MOD_LOBBY_USERAPPROVED_KEY, approveduser.c_str());
                    //We now know this user's API, and we already sent their queued packets, so clear them
                    ModNetworking::SteamAPIStatusKnown_Users.insert_or_assign(psteamIDRemote->ConvertToUint64(), false);
                    RemoveQueuedPackets(psteamIDRemote->ConvertToUint64());
                }
                // If specified in options, we must disconnect the non-mod player, since they won't on their own
                else if (ModNetworking::allow_connect_with_non_mod_guest == false)
                {
                    ConsoleWrite("Incoming guest is non-mod user, and host do not allow connections with non-mod users.");
                    HostForceDisconnectGuest(psteamIDRemote->ConvertToUint64(), L"Incoming guest is non-mod user, and you do not allow connections with non-mod users.", true);
                    RemoveQueuedPackets(psteamIDRemote->ConvertToUint64());
                    return false;
                }
            }
            //As a guest, only make this approval decision ourselves if the lobby is hosted by non-mod user
            if (curLobby != ModNetworking::selfPersisantLobby && Mod::get_mode() == ModMode::Compatability)
            {
                ConsoleWrite("Guest auto-approves incoming guest in non-mod host world.");
                ModNetworking::SteamAPIStatusKnown_Users.insert_or_assign(psteamIDRemote->ConvertToUint64(), false);
                RemoveQueuedPackets(psteamIDRemote->ConvertToUint64());
            }
        }

        return read_packet_old;
    }
}

//SendP2PPacket/SendMessageToUser
bool SendP2PPacket_Replacement_injection_helper(CSteamID steamIDRemote, void *pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel)
{
    if (nChannel != 0)
    {
        FATALERROR("Why the fuck is Dark Souls not sending on channel 0. Please report me.");
    }

    // Handle disconnecting a user
    uint64_t incoming_guest_to_not_accept = ModNetworking::incoming_guest_to_not_accept.load();
    if (incoming_guest_to_not_accept != 0 && incoming_guest_to_not_accept == steamIDRemote.ConvertToUint64())
    {
        ConsoleWrite("Forcing d/c on user %lld", steamIDRemote.ConvertToUint64());
        return false;
    }

    //Any packet sent during this period where we don't know the other user's API nature, send it on the old api and save to send later if they use the new api.
    if (!ModNetworking::SteamAPIStatusKnown_Users.contains(steamIDRemote.ConvertToUint64()))
    {
        uint64_t currentLobby = ModNetworking::currentLobby.load();
        //need to verify that they're in the session, since sometimes the game sends packets even after they've left
        int numlobbymembers = ModNetworking::SteamMatchmaking->GetNumLobbyMembers(currentLobby);
        for (int i = 0; i < numlobbymembers; i++)
        {
            CSteamID member = ModNetworking::SteamMatchmaking->GetLobbyMemberByIndex(currentLobby, i);
            if (member == steamIDRemote)
            {
                queued_packets.push_back(new PacketStorageData(steamIDRemote, pubData, cubData, eP2PSendType, nChannel));
                ConsoleWrite("Saving queued packet type %d for %llx: total %d", ((uint8_t*)pubData)[1], steamIDRemote.ConvertToUint64(), queued_packets.size());
                break;
            }
        }

        //However, if we don't know them, we need to send the packet on the old API, as a bait for non-mod users
        //(need to do this as both host and guest)
        //If they respond (which mod users will not since they instantly on join know we are mod), we know they are non-mod
        PlayerVisualsValidationFix::sanitizePacketData((uint8_t*)pubData, cubData);
        return ModNetworking::SteamNetworking->SendP2PPacket(steamIDRemote, pubData, cubData, eP2PSendType, nChannel);
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
        PlayerVisualsValidationFix::sanitizePacketData((uint8_t*)pubData, cubData); //since the other player doesn't have the mod, need to prevent them from seeing invalid player visuals
        return ModNetworking::SteamNetworking->SendP2PPacket(steamIDRemote, pubData, cubData, eP2PSendType, nChannel);
    }
}

//CloseP2PSessionWithUser/CloseSessionWithUser
bool CloseP2PSessionWithUser_Replacement_injection_helper(CSteamID steamIDRemote)
{
    //In theory we should never still have the user saved when the game hits this, so just return that we already closed them
    if (!ModNetworking::SteamAPIStatusKnown_Users.contains(steamIDRemote.ConvertToUint64()))
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

//LeaveLobby
void Steam_LeaveLobby_injection_helper(uint64_t lobbyid)
{
    //We are injecting at the function start, which is why we call LeaveLobby here and then the game will call it again
    ModNetworking::SteamMatchmaking->LeaveLobby(lobbyid);
    //Re-create the persistent lobby now that we are not a guest anymore
    ModNetworking::CreatePersistentLobby();
}

//CreateLobby
void Steam_CreateLobby_injection_helper(void* SteamSessionLight)
{
    //tell the game the lobby was created, and here's it's id
    LobbyCreated_t lobbyCreatedt;
    lobbyCreatedt.m_eResult = k_EResultOK;
    lobbyCreatedt.m_ulSteamIDLobby = ModNetworking::selfPersisantLobby.load();
    Steam_CreateLobby_APICallResult(SteamSessionLight, &lobbyCreatedt);
}

/*
 * ===========CONNECTION HANDSHAKE SECTION
 */
bool ModNetworking::allow_connect_with_non_mod_host = true;
bool ModNetworking::allow_connect_with_legacy_mod_host = true;
bool ModNetworking::allow_connect_with_overhaul_mod_host = true;
bool ModNetworking::allow_connect_with_non_mod_guest = true;

std::atomic<uint64_t> ModNetworking::selfPersisantLobby = 0;
std::atomic<uint64_t> ModNetworking::currentLobby = 0;

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
    ModNetworking::incoming_guest_to_not_accept = 0;
    ModNetworking::SteamAPIStatusKnown_Users.clear();
    ClearQueuedPackets(); //the game may still send packets to users after they D/C, so clean any leftovers out

    // Guest: upon lobby connection check for lobby data on mod status
    // This callback is only triggered when WE attempt to enter a lobby, so existing guests won't have this trigger
    if (lobbyowner != selfsteamid)
    {
        //According to SteamAPI documentation, lobby metadata is available to use immediately after reciving this callback
        const char* mod_str = ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY);
        ModMode mod_value = ModMode::InvalidMode;
        if (String_To_ModModes.count(mod_str) != 0)
        {
            mod_value = String_To_ModModes.at(mod_str);
        }
        ModNetworking::SteamAPIStatusKnown_Users.insert_or_assign(lobbyowner.ConvertToUint64(), mod_value != ModMode::InvalidMode);
        ConsoleWrite("Guest got lobby data = %s/%hhx", mod_str, mod_value);

        // As the guest, we must change our settings to match the host (based on how we've configured our options), or else disconnect.
        if (mod_value == ModMode::InvalidMode && ModNetworking::allow_connect_with_non_mod_host == true)
        {
            //connecting to non-mod
            Mod::set_mode(ModMode::Compatability);
        }
        else if (mod_value == ModMode::Compatability && ModNetworking::allow_connect_with_non_mod_host == true)
        {
            //connecting to (basically) non-mod
            Mod::set_mode(ModMode::Compatability);
        }
        else if (mod_value == ModMode::Overhaul && ModNetworking::allow_connect_with_overhaul_mod_host == true)
        {
            //connecting to overhaul
            Mod::set_mode(ModMode::Overhaul);
        }
        else if (mod_value == ModMode::Legacy && ModNetworking::allow_connect_with_legacy_mod_host == true)
        {
            //connecting to legacy
            Mod::set_mode(ModMode::Legacy);
        }
        else
        {
            //Tell the player why we're dcing
            wchar_t dc_msg[300];
            wcscpy_s(dc_msg, L"Unable to connect to host player.\n");
            if (mod_value == ModMode::InvalidMode && ModNetworking::allow_connect_with_non_mod_host == false)
            {
                wcscat_s(dc_msg, L"Host does not have mod and you do not allow connections with non-mod users.");
            }
            else if (mod_value == ModMode::Compatability && ModNetworking::allow_connect_with_non_mod_host == false)
            {
                wcscat_s(dc_msg, L"Host has a non-mod user connected and you do not allow connections with non-mod users.");
            }
            else if (mod_value == ModMode::Overhaul && ModNetworking::allow_connect_with_overhaul_mod_host == false)
            {
                wcscat_s(dc_msg, L"Host is in overhaul mode and you do not allow connections with overhaul mode users.");
            }
            else if (mod_value == ModMode::Legacy && ModNetworking::allow_connect_with_legacy_mod_host == false)
            {
                wcscat_s(dc_msg, L"Host is in legacy mode and you do not allow connections with legacy mode users.");
            }
            Game::show_popup_message(dc_msg);
            ConsoleWrite("Guest disconnecting due to settings");

            //disconnect
            ModNetworking::SteamMatchmaking->LeaveLobby(pCallback->m_ulSteamIDLobby);
            ModNetworking::currentLobby = 0;
            ModNetworking::SteamAPIStatusKnown_Users.clear();
            ClearQueuedPackets();
            ModNetworking::CreatePersistentLobby();
            return;
        }

        //set our lobby member data confirming we also respect these settings and have the mod
        ModNetworking::SteamMatchmaking->SetLobbyMemberData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY, ModModes_To_String.at(Mod::get_mode()));
        ConsoleWrite("Guest set member data = %hhx", Mod::get_mode());

        //now that we know the host, we can send the queued packets (only if they are a mod user, otherwise we already sent them)
        ProcessQueuedPackets(lobbyowner.ConvertToUint64());

        //we now lookup the status of the guest and any connected hosts
        //we can safely assume their lobby member data is set before we get there
        //This is only really useful if >1 mod users are connected in a non-mod lobby
        int numlobbymembers = ModNetworking::SteamMatchmaking->GetNumLobbyMembers(pCallback->m_ulSteamIDLobby);
        for (int i = 0; i < numlobbymembers; i++)
        {
            CSteamID member = ModNetworking::SteamMatchmaking->GetLobbyMemberByIndex(pCallback->m_ulSteamIDLobby, i);
            //Exclude lobby owner (we just read them), and ourselves
            if (member.ConvertToUint64() != lobbyowner && member.ConvertToUint64() != selfsteamid)
            {
                const char* user_mod_value_str = ModNetworking::SteamMatchmaking->GetLobbyMemberData(pCallback->m_ulSteamIDLobby, member, MOD_LOBBY_DATA_KEY);
                ModMode user_mod_value = ModMode::InvalidMode;
                if (user_mod_value_str != NULL && String_To_ModModes.count(user_mod_value_str) != 0)
                {
                    user_mod_value = String_To_ModModes.at(user_mod_value_str);
                }
                ModNetworking::SteamAPIStatusKnown_Users.insert_or_assign(member.ConvertToUint64(), user_mod_value != ModMode::InvalidMode);
                ProcessQueuedPackets(member.ConvertToUint64());
                ConsoleWrite("Guest read user %llx = %hhx", member.ConvertToUint64(), user_mod_value);
            }
        }

        //Kick off ggpo
        Rollback::rollback_start_session(ModNetworking::SteamNetMessages);

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

    // Guest: If we do get more data later, after we're connected, then the host is telling us that we need to change modes, or is triggering the approval of the new guest
    if (pCallback->m_ulSteamIDMember == pCallback->m_ulSteamIDLobby && lobbyowner != selfsteamid && ModNetworking::SteamAPIStatusKnown_Users.contains(lobbyowner.ConvertToUint64()))
    {
        const char* mod_str = ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_DATA_KEY);
        ModMode mod_value = ModMode::InvalidMode;
        if (String_To_ModModes.count(mod_str) != 0)
        {
            mod_value = String_To_ModModes.at(mod_str);
        }

        const char* approve_user_str = ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_USERAPPROVED_KEY);
        uint64_t approve_user_value = strtoull(approve_user_str, NULL, 10);

        //host changed mode
        if (mod_value != ModMode::InvalidMode && Mod::get_mode() != mod_value)
        {
            ConsoleWrite("Guest got updated lobby data = %s/%hhx", mod_str, mod_value);
            Mod::set_mode(mod_value);
        }

        //host approved incoming user
        if (approve_user_value != 0 && approve_user_value != ULLONG_MAX && !ModNetworking::SteamAPIStatusKnown_Users.contains(approve_user_value) && selfsteamid != approve_user_value)
        {
            //verify that they're in the session and grab their lobby data
            //by this point, their member data should have been sent (since the host got it), so we can also use it
            int numlobbymembers = ModNetworking::SteamMatchmaking->GetNumLobbyMembers(pCallback->m_ulSteamIDLobby);
            bool approved_in_session = false;
            ModMode user_mode = ModMode::InvalidMode;
            for (int i = 0; i < numlobbymembers; i++)
            {
                CSteamID member = ModNetworking::SteamMatchmaking->GetLobbyMemberByIndex(pCallback->m_ulSteamIDLobby, i);
                if (member.ConvertToUint64() == approve_user_value)
                {
                    //Confirm this user is in the lobby. This might be a stale value we're reading
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
                ConsoleWrite("Guest approves user = \"%s\"/%llx", approve_user_str, approve_user_value);
                ModNetworking::SteamAPIStatusKnown_Users.insert_or_assign(approve_user_value, user_mode != ModMode::InvalidMode);
                //now that we know the user, we can send the queued packets (only if they are a mod user, otherwise we already sent them)
                ProcessQueuedPackets(approve_user_value);
            }
        }
    }

    // Guest: If we're in a non-mod host lobby, can check the incoming guest's API by if they set their data
    // If we don't get this, we can use the API they send messages on to id them.
    if (pCallback->m_ulSteamIDMember != pCallback->m_ulSteamIDLobby && pCallback->m_ulSteamIDMember != selfsteamid.ConvertToUint64() && lobbyowner != selfsteamid && Mod::get_mode() == ModMode::Compatability)
    {
        const char* mod_guest_value_str = ModNetworking::SteamMatchmaking->GetLobbyMemberData(pCallback->m_ulSteamIDLobby, pCallback->m_ulSteamIDMember, MOD_LOBBY_DATA_KEY);
        if (mod_guest_value_str != NULL && String_To_ModModes.count(mod_guest_value_str) != 0)
        {
            ConsoleWrite("Guest read user %llx is mod user", pCallback->m_ulSteamIDMember);
            ModNetworking::SteamAPIStatusKnown_Users.insert_or_assign(pCallback->m_ulSteamIDMember, true);
            ProcessQueuedPackets(pCallback->m_ulSteamIDMember);
        }
    }

    // Host: got the response member data info from the guest saying they respect these settings
    if (pCallback->m_ulSteamIDMember != pCallback->m_ulSteamIDLobby && pCallback->m_ulSteamIDMember != selfsteamid.ConvertToUint64() && lobbyowner == selfsteamid)
    {
        const char* mod_guest_value_str = ModNetworking::SteamMatchmaking->GetLobbyMemberData(pCallback->m_ulSteamIDLobby, pCallback->m_ulSteamIDMember, MOD_LOBBY_DATA_KEY);
        ModMode mod_guest_value = ModMode::InvalidMode;
        if (mod_guest_value_str != NULL && String_To_ModModes.count(mod_guest_value_str) != 0)
        {
            mod_guest_value = String_To_ModModes.at(mod_guest_value_str);
        }
        ConsoleWrite("Host got guest data = %s/%hhx", mod_guest_value_str, mod_guest_value);

        // If the guest isn't respecting our settings for some reason (can't hurt to check), disconnect them right away
        if (mod_guest_value != Mod::get_mode())
        {
            ConsoleWrite("Host detects incoming guest is a mod user, but isn't matching the legacy/overhaul mode. Host=%d, guest=%d.", static_cast<int>(Mod::get_mode()), static_cast<int>(mod_guest_value));
            HostForceDisconnectGuest(pCallback->m_ulSteamIDMember, L"Incoming guest is a mod user, but isn't matching your legacy/overhaul mode.");
            RemoveQueuedPackets(pCallback->m_ulSteamIDMember); //get rid of the queued packets
        }
        //Success state for a new guest
        else if (!ModNetworking::SteamAPIStatusKnown_Users.contains(pCallback->m_ulSteamIDMember))
        {
            //Let guests know the host approves this user
            std::string approveduser = std::to_string(pCallback->m_ulSteamIDMember);
            ConsoleWrite("Host update lobby user approval = %llx", pCallback->m_ulSteamIDMember);
            ModNetworking::SteamMatchmaking->SetLobbyData(pCallback->m_ulSteamIDLobby, MOD_LOBBY_USERAPPROVED_KEY, approveduser.c_str());

            //We now know this user's API, so we can send the queued packets
            ModNetworking::SteamAPIStatusKnown_Users.insert_or_assign(pCallback->m_ulSteamIDMember, true);
            ProcessQueuedPackets(pCallback->m_ulSteamIDMember);

            //Start GGPO for this session (new session or re-create with new players)
            Rollback::rollback_end_session();
            Rollback::rollback_start_session(ModNetworking::SteamNetMessages);
        }
    }
}

//This is called when another user has joined/left the lobby.
void ModNetworking::LobbyChatUpdateCallback(LobbyChatUpdate_t* pCallback)
{
    //Painted Worlds fix
    if (strlen(ModNetworking::SteamMatchmaking->GetLobbyData(pCallback->m_ulSteamIDLobby, "PaintedWorlds")) != 0)
    {
        ConsoleWrite("Ignore PaintedWorlds prejoin lobby");
        return;
    }

    ModNetworking::incoming_guest_to_not_accept = 0;
    CSteamID lobbyowner = ModNetworking::SteamMatchmaking->GetLobbyOwner(pCallback->m_ulSteamIDLobby);
    CSteamID selfsteamid = ModNetworking::SteamUser->GetSteamID();

    // Host: Handle the case where the guest disconnects on their own
    if (lobbyowner == selfsteamid && pCallback->m_rgfChatMemberStateChange & EChatMemberStateChange::k_EChatMemberStateChangeLeft && pCallback->m_ulSteamIDUserChanged != selfsteamid.ConvertToUint64())
    {
        ConsoleWrite("Guest %llx d/c'd on their own.", pCallback->m_ulSteamIDUserChanged);

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

        ModNetworking::SteamAPIStatusKnown_Users.erase(pCallback->m_ulSteamIDUserChanged); //remove the user from the known list
        RemoveQueuedPackets(pCallback->m_ulSteamIDUserChanged); //get rid of the queued packets for this user since we don't need them anymore

        //end GGPO, and recreate for this session if needed
        Rollback::rollback_end_session();
        if (ModNetworking::SteamMatchmaking->GetNumLobbyMembers(pCallback->m_ulSteamIDLobby) > 1)
        {
            Rollback::rollback_start_session(ModNetworking::SteamNetMessages);
        }

        // We can switch modes back to non-compatability if this list is empty now
        bool all_mod_users = true;
        for (const auto &[user, is_mod] : ModNetworking::SteamAPIStatusKnown_Users)
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
            ConsoleWrite("Host reset lobby data = %hhx", Mod::get_mode());
        }
    }

    // Guest: a new user is joining, we need to queue and send our packets only after we know what API they use
    // (Guest packets are required for a new guest to connect)
    // We won't know if they use the API until either the host approves them, or they set their lobby data, however.
    // So we do nothing for now and pretend they are non-mod.
    if (lobbyowner != selfsteamid && pCallback->m_rgfChatMemberStateChange & EChatMemberStateChange::k_EChatMemberStateChangeEntered && pCallback->m_ulSteamIDUserChanged != selfsteamid.ConvertToUint64())
    {
        ConsoleWrite("Guest detected new guest is trying to join.");
    }

    // Guest: Handle the case where the guest disconnects
    if (lobbyowner != selfsteamid && pCallback->m_rgfChatMemberStateChange & EChatMemberStateChange::k_EChatMemberStateChangeLeft && pCallback->m_ulSteamIDUserChanged != selfsteamid.ConvertToUint64())
    {
        ConsoleWrite("Guest detected another guest has D/C'd.");

        ModNetworking::SteamAPIStatusKnown_Users.erase(pCallback->m_ulSteamIDUserChanged); //remove the user from the known list
        RemoveQueuedPackets(pCallback->m_ulSteamIDUserChanged);

        //end GGPO, and recreate for this session if needed
        Rollback::rollback_end_session();
        if (ModNetworking::SteamMatchmaking->GetNumLobbyMembers(pCallback->m_ulSteamIDLobby) > 1)
        {
            Rollback::rollback_start_session(ModNetworking::SteamNetMessages);
        }
    }
}
