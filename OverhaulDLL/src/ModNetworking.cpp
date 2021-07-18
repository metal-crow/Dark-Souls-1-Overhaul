#include "ModNetworking.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "AnimationEdits.h"
#include <wchar.h>

bool ModNetworking::allow_connect_with_non_mod_host = true;
bool ModNetworking::allow_connect_with_legacy_mod_host = true;
bool ModNetworking::allow_connect_with_overhaul_mod_host = true;
bool ModNetworking::allow_connect_with_non_mod_guest = true;

bool ModNetworking::host_mod_installed = false;
bool ModNetworking::host_legacy_enabled = false;
bool ModNetworking::guest_mod_installed = false;
bool ModNetworking::guest_legacy_enabled = false;

int64_t ModNetworking::timer_offset = 0;

const uint8_t MOD_ENABLED = 0x80; //it's encoded that the mod is active in the most significant bit
const uint8_t LEGACY_ENABLED = 0x40; //it's encoded that the mod is in legacy mode in the 2nd most significant bit
const uint8_t REMOVE_FLAGS = ~(MOD_ENABLED | LEGACY_ENABLED);

typedef void* SteamInternal_ContextInit_FUNC(uint64_t Init_SteamInternal_FUNCPTR);
SteamInternal_ContextInit_FUNC** SteamInternal_ContextInit = (SteamInternal_ContextInit_FUNC**)0x1420B6738; //the address of the location for the imported function address
uint64_t Init_SteamInternal_FUNCPTR = 0x141AC1020;
typedef void* SteamInternal_SteamNetworkingSend_FUNC(void* SteamNetworking, uint64_t steamid, const uint8_t* packet_data, uint32_t packet_len, uint32_t eP2PSendType, uint32_t nChannel);

typedef uint64_t getNetMessage_Typedef(void* session_man, void* player, uint32_t type, byte *store_data_here, uint32_t max_message_size);
getNetMessage_Typedef* getNetMessage = (getNetMessage_Typedef*)0x140509560;

typedef uint64_t getPlayerInsForConnectedPlayerData_Typedef(void *worldchrman, void *player);
getPlayerInsForConnectedPlayerData_Typedef* getPlayerInsForConnectedPlayerData = (getPlayerInsForConnectedPlayerData_Typedef*)0x140372710;

typedef uint64_t getSteamIDForConnectedPlayerData_Typedef(void *player);
getSteamIDForConnectedPlayerData_Typedef* getSteamIDForConnectedPlayerData = (getSteamIDForConnectedPlayerData_Typedef*)0x141062d40;

typedef uint64_t sendNetMessageToAllPlayers_Typedef(void* sessionMan, uint32_t type, byte * data, int size);
sendNetMessageToAllPlayers_Typedef* sendNetMessageToAllPlayers = (sendNetMessageToAllPlayers_Typedef*)0x1405098a0;

extern "C" {
    uint64_t sendPacket_injection_return;
    void sendPacket_injection();
    void sendPacket_injection_helper(uint8_t* data, uint32_t size, uint32_t type);

    uint64_t GetSteamData_Packet_injection_return;
    void GetSteamData_Packet_injection();
    uint32_t GetSteamData_Packet_injection_helper(void* data, uint32_t type, void* SteamSessionMemberLight);

    uint64_t getNetMessage_injection_return;
    void getNetMessage_injection();
    void getNetMessage_injection_helper(uint8_t* data, uint32_t size, uint32_t type);

    uint64_t SendRawP2PPacket_injection_return;
    void SendRawP2PPacket_injection();
    uint64_t SendRawP2PPacket_injection_helper(uint8_t* data, uint64_t size, uint32_t type);

    uint64_t ParseRawP2PPacketType_injection_return;
    void ParseRawP2PPacketType_injection();
    void ParseRawP2PPacketType_injection_helper(uint8_t* data, uint64_t steamId_remote);

    uint64_t type1_32byte_p2pPacket_parse_rollback_injection_return;
    uint64_t type1_40byte_p2pPacket_parse_rollback_injection_return;
    void type1_32byte_p2pPacket_parse_rollback_injection();
    void type1_40byte_p2pPacket_parse_rollback_injection();
    uint64_t type1_32byte_p2pPacket_parse_rollback_injection_helper(void* session_man, void* player, uint32_t type, byte *store_data_here);
    uint64_t type1_40byte_p2pPacket_parse_rollback_injection_helper(void* session_man, void* player, uint32_t type, byte *store_data_here);

    uint64_t type1_p2pPacket_sending_rollback_injection_return;
    void type1_p2pPacket_sending_rollback_injection();
    void type1_p2pPacket_send_rollback_injection_helper(void* sessionMan, uint32_t type, byte * data, int size);
}


void ModNetworking::start()
{
    ConsoleWrite("Enabling mod networking...");

    uint8_t *write_address;

    /*
     * Inject code that will piggypack on the packet sent upon the start of an initial connection with a person, and will provide data on the mod.
     * This will specify if the mod is installed, and what mode it is in (legacy/non-legacy)
     */
    //handle reading from GameData packets
    //write_address = (uint8_t*)(ModNetworking::sendPacket_injection_offset + Game::ds1_base);
    //sp::mem::code::x64::inject_jmp_14b(write_address, &sendPacket_injection_return, 1, &sendPacket_injection);

    //handle reading the handshake from SteamData packets
    write_address = (uint8_t*)(ModNetworking::GetSteamData_Packet_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &GetSteamData_Packet_injection_return, 2, &GetSteamData_Packet_injection);

    //handle writing to GameData packets
    //write_address = (uint8_t*)(ModNetworking::getNetMessage_injection_offset + Game::ds1_base);
    //sp::mem::code::x64::inject_jmp_14b(write_address, &getNetMessage_injection_return, 4, &getNetMessage_injection);

    //handle setting the handshake in SteamData packets
    write_address = (uint8_t*)(ModNetworking::SendRawP2PPacket_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &SendRawP2PPacket_injection_return, 0, &SendRawP2PPacket_injection);

    /*
     * Code to support rollback netcode functionality.
     * A callback function which has the host syncronize the time with the clients.
     * And an injection for the clients to get that sync packet
     */

    //inject into ParseRawP2PPacketType to have a handler for our custom encapsulated packet type
    write_address = (uint8_t*)(ModNetworking::ParseRawP2PPacketType_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &ParseRawP2PPacketType_injection_return, 1, &ParseRawP2PPacketType_injection);

    //inject into the type1 packet sending function and include the current time
    write_address = (uint8_t*)(ModNetworking::type1_p2pPacket_sending_rollback_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &type1_p2pPacket_sending_rollback_injection_return, 1, &type1_p2pPacket_sending_rollback_injection);

    //inject into the type1 packet parsing function and read the given time and adjust the animation starting offset
    write_address = (uint8_t*)(ModNetworking::type1_32byte_p2pPacket_parsing_rollback_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &type1_32byte_p2pPacket_parse_rollback_injection_return, 1, &type1_32byte_p2pPacket_parse_rollback_injection);
    //same as above but for the other type of type1
    write_address = (uint8_t*)(ModNetworking::type1_40byte_p2pPacket_parsing_rollback_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &type1_40byte_p2pPacket_parse_rollback_injection_return, 3, &type1_40byte_p2pPacket_parse_rollback_injection);

}

/*
 * ===========ROLLBACK NETCODE SECTION
 */

enum TimestampSyncPacketTypes
{
    DelayCalc = 0,
    UpdateTime = 1,
};

const uint32_t ResyncPeriod = 15 * 10000000; // 15 seconds
const uint32_t SaneDelay = 2 * 10000000; // 2 seconds

std::unordered_map<uint64_t, TimerClientInfo> ModNetworking::hostTimerSyncronizationData;

// Handle sending out the delay calculation packets as needed
bool HostTimerSync(void* unused)
{
    //exit this once we're no longer hosting
    auto session_action_o = Game::get_SessionManagerImp_session_action_result();
    if (session_action_o.has_value())
    {
        auto session_action_result = session_action_o.value();
        if (session_action_result != TryToCreateSession && session_action_result != CreateSessionSuccess)
        {
            return false;
        }
    }

    //only do anything if we're in non-legacy mode
    if (!Mod::legacy_mode)
    {
        //as the host our offset is always 0
        ModNetworking::timer_offset = 0;

        //get the session members
        auto steamsessionlight_o = Game::get_SessionManagerImp_SteamSessionLight();
        if (!steamsessionlight_o.has_value())
        {
            ConsoleWrite("Unable to get_SessionManagerImp_SteamSessionLight in %s", __FUNCTION__);
            return true;
        }
        uint64_t steamsessionlight = (uint64_t)steamsessionlight_o.value();
        uint64_t SessionMembersStart = *(uint64_t*)(steamsessionlight + 8 + 0x68);
        SessionMembersStart += 8; //skip the first connection (which is ourselves)
        uint64_t SessionMembersEnd = *(uint64_t*)(steamsessionlight + 8 + 0x70);

        while (SessionMembersStart != SessionMembersEnd)
        {
            uint64_t SteamSessionMemberLight = *(uint64_t*)SessionMembersStart;
            uint64_t SteamId = *(uint64_t*)(SteamSessionMemberLight + 0xc8);

            // If this is a new guest, start tracking them
            if (ModNetworking::hostTimerSyncronizationData.count(SteamId) == 0)
            {
                ModNetworking::hostTimerSyncronizationData.emplace(SteamId, TimerClientInfo());
            }

            // If it's time to resync with this guest, do so
            //if ((Game::get_accurate_time() - ModNetworking::hostTimerSyncronizationData[SteamId].timeOfLastResync) > ResyncPeriod)
            //TMP: disable till calculation is fixed
            if (ModNetworking::hostTimerSyncronizationData[SteamId].timeOfLastResync == 0)
            {
                // Send the ping packet out to compute the latency
                if (!ModNetworking::hostTimerSyncronizationData[SteamId].waitingForPingResponse)
                {
                    uint8_t resyncbuf[] = {
                        (4 | (1 << 4)), //custom clock sync packet type, TCP
                        (uint8_t)TimestampSyncPacketTypes::DelayCalc, //type of clock sync packet
                    };

                    void* SteamInternal = (*SteamInternal_ContextInit)(Init_SteamInternal_FUNCPTR);
                    uint64_t SteamNetworking = *(uint64_t*)((uint64_t)SteamInternal + 0x40);
                    SteamInternal_SteamNetworkingSend_FUNC* SteamNetworkingSend = (SteamInternal_SteamNetworkingSend_FUNC*)**(uint64_t**)SteamNetworking;
                    bool success = SteamNetworkingSend((void*)SteamNetworking, SteamId, resyncbuf, sizeof(resyncbuf), 2, 0);
                    if (success) //if this fails to send retry from beginning
                    {
                        ConsoleWrite("Resync sent");

                        ModNetworking::hostTimerSyncronizationData[SteamId].timePingPacketSent = Game::get_accurate_time();
                        ModNetworking::hostTimerSyncronizationData[SteamId].waitingForPingResponse = true;
                    }
                }
            }

            SessionMembersStart += 8;
        }
    }

    return true;
}

// Handle receiving the clock sync packets
void ParseRawP2PPacketType_injection_helper(uint8_t* data, uint64_t steamId_remote)
{
    if (Mod::legacy_mode)
    {
        return;
    }

    auto session_action_o = Game::get_SessionManagerImp_session_action_result();
    if (!session_action_o.has_value())
    {
        ConsoleWrite("Unable to get SessionAction in %s", __FUNCTION__);
        return;
    }
    auto session_action_result = session_action_o.value();

    // If we're the host, save the ping packet response and immediately do the reply
    if (session_action_result == TryToCreateSession || session_action_result == CreateSessionSuccess)
    {
        if (data[1] == TimestampSyncPacketTypes::DelayCalc)
        {
            ConsoleWrite("Resync response got for %llu", steamId_remote);

            // Using the ping packet data, send out the guest's corrected clock
            // compute the 1 way latency
            uint64_t ping = (Game::get_accurate_time() - ModNetworking::hostTimerSyncronizationData[steamId_remote].timePingPacketSent);
            // TODO should update this as a running average of some kind
            ModNetworking::hostTimerSyncronizationData[steamId_remote].packetDelay = ping / 2;
            // sanity check the computed latency
            if (ping > SaneDelay)
            {
                ConsoleWrite("Host computed ping value of realllly high (%d). Ignoring.", ping);
            }
            else
            {
                ConsoleWrite("Timer update sent for ping of = %fms", ping/10000.0);

                // Send the time the guest clock should be, accounting for latency
                uint8_t updatebuf[10] = {
                    (4 | (1 << 4)), //custom clock sync packet type, TCP
                    (uint8_t)TimestampSyncPacketTypes::UpdateTime, //type of clock sync packet
                };
                *(uint64_t*)(updatebuf + 2) = Game::get_accurate_time() + ModNetworking::hostTimerSyncronizationData[steamId_remote].packetDelay;

                void* SteamInternal = (*SteamInternal_ContextInit)(Init_SteamInternal_FUNCPTR);
                uint64_t SteamNetworking = *(uint64_t*)((uint64_t)SteamInternal + 0x40);
                SteamInternal_SteamNetworkingSend_FUNC* SteamNetworkingSend = (SteamInternal_SteamNetworkingSend_FUNC*)**(uint64_t**)SteamNetworking;
                SteamNetworkingSend((void*)SteamNetworking, steamId_remote, updatebuf, sizeof(updatebuf), 2, 0); //if this fails to send we'll just resend in 15 sec anyway

                //update our sync time with this guest
                ModNetworking::hostTimerSyncronizationData[steamId_remote].timeOfLastResync = Game::get_accurate_time();
                ModNetworking::hostTimerSyncronizationData[steamId_remote].waitingForPingResponse = false;
            }
        }
    }

    // If we're the guest, handle the clock packets
    else if (session_action_result == TryToJoinSession || session_action_result == JoinSessionSuccess)
    {
        // Host is computing the delay. Just resend the packet back to them
        if (data[1] == TimestampSyncPacketTypes::DelayCalc)
        {
            ConsoleWrite("Resync request got for %llu", steamId_remote);

            void* SteamInternal = (*SteamInternal_ContextInit)(Init_SteamInternal_FUNCPTR);
            uint64_t SteamNetworking = *(uint64_t*)((uint64_t)SteamInternal + 0x40);
            SteamInternal_SteamNetworkingSend_FUNC* SteamNetworkingSend = (SteamInternal_SteamNetworkingSend_FUNC*)**(uint64_t**)SteamNetworking;
            SteamNetworkingSend((void*)SteamNetworking, steamId_remote, data, 2, 2, 0);
        }
        // Host has sent us the correct time. Use this to update our offset
        else if (data[1] == TimestampSyncPacketTypes::UpdateTime)
        {
            uint64_t trueTime = *(uint64_t*)(data + 2);
            uint64_t ourTime = Game::get_accurate_time();
            //can't sanity check the offset value since the timer starts at desktop boot time
            ModNetworking::timer_offset = trueTime - ourTime;
            ConsoleWrite("Timer update got: %lld", ModNetworking::timer_offset);
        }
    }
}

// Handle including the rollback data in the type1 packet sending
void type1_p2pPacket_send_rollback_injection_helper(void* sessionMan, uint32_t type, byte * data, int size)
{
    //do the original code if we're in legacy
    if (Mod::legacy_mode)
    {
        sendNetMessageToAllPlayers(sessionMan, type, data, size);
        return;
    }

    //create a new, larger, packet, and send that
    byte* new_data = (byte*)malloc(size + 8);
    memcpy(new_data, data, size);

    //insert the current time into this
    //TODO in theory the current offset in the animation here may not be 0, so the correction to the other player might not be fully accurate
    *(uint64_t*)(new_data + size) = Game::get_synced_time();

    //send the changed packet
    sendNetMessageToAllPlayers(sessionMan, type, new_data, size + 8);

    free(new_data);
}

// Handle parsing and getting the rollback data from the type1 packet reading
uint64_t type1_p2pPacket_parse_rollback_injection_helper(void* session_man, void* player, uint32_t type, byte *store_data_here, uint32_t size)
{
    uint64_t gotbytes;

    //do the original code if we're in legacy
    if (Mod::legacy_mode)
    {
        gotbytes = getNetMessage(session_man, player, type, store_data_here, size);
        return gotbytes;
    }

    //get the data for this animation if we recieve a packet
    gotbytes = getNetMessage(session_man, player, type, store_data_here, size + 8);
    if (gotbytes == 0)
    {
        return gotbytes;
    }

    uint64_t timeAnimationTriggered = *(uint64_t*)(store_data_here + size);
    uint32_t animationToUpdate = *(uint32_t*)(store_data_here + 12);

    //check if this is the starting packet for this animation, and discard if not
    uint8_t counter = (animationToUpdate >> 28);
    uint64_t SteamId = getSteamIDForConnectedPlayerData(player);
    if (counter == ModNetworking::hostTimerSyncronizationData[SteamId].lastAidCount)
    {
        return gotbytes;
    }
    ModNetworking::hostTimerSyncronizationData[SteamId].lastAidCount = counter;

    //check if this is an animation state we care about
    uint16_t animState = (uint16_t)(animationToUpdate & 0xffff);
    if (AnimationEdits::STATEIDS_TO_ROLLBACK.count(animState) == 0)
    {
        return gotbytes;
    }

    //get a pointer to the animation mediator for this player
    uint64_t playerins = getPlayerInsForConnectedPlayerData(*(void**)Game::world_chr_man_imp, player);
    void* animationMediator = Game::get_PlayerIns_AnimationMediator(playerins);
    if (animationMediator == NULL)
    {
        ConsoleWrite("Unable to get Animation Mediator for player");
        return gotbytes;
    }

    //pass data to the callback
    SetAnimationTimeOffsetArg* animTimeArg = (SetAnimationTimeOffsetArg*)malloc(sizeof(SetAnimationTimeOffsetArg));
    animTimeArg->animationState = animState;
    animTimeArg->timeAnimationTriggered = timeAnimationTriggered;
    animTimeArg->animationMediatorPtr = animationMediator;
    animTimeArg->frameStart = Game::get_frame_count();
    MainLoop::setup_mainloop_callback(AnimationEdits::SetAnimationTimeOffset, animTimeArg, "SetAnimationTimeOffset");

    return gotbytes;
}

uint64_t type1_32byte_p2pPacket_parse_rollback_injection_helper(void* session_man, void* player, uint32_t type, byte *store_data_here)
{
    return type1_p2pPacket_parse_rollback_injection_helper(session_man, player, type, store_data_here, 32);
}
uint64_t type1_40byte_p2pPacket_parse_rollback_injection_helper(void* session_man, void* player, uint32_t type, byte *store_data_here)
{
    return type1_p2pPacket_parse_rollback_injection_helper(session_man, player, type, store_data_here, 40);
}


/*
 * ===========CONNECTION HANDSHAKE SECTION
 */

const uint8_t data_buf[] = {
    (1 | (1 << 4)), //p2p packet type
    78, 88, 82, 86, //magic
    5, 132, //header
    46, //type KickOutTask
    255, 0, 0, 25 }; //data (const 0xff000019)

// Only call as Host, since this assumes session is connected and you can D/C other player
void HostForceDisconnectSession(void* SteamSessionMemberLight, const wchar_t* dc_reason)
{
    void* SteamInternal = (*SteamInternal_ContextInit)(Init_SteamInternal_FUNCPTR);

    if (SteamInternal == NULL)
    {
        ConsoleWrite("Unable to disconnect player: unable to create SteamInternal");
        return;
    }

    uint64_t SteamNetworking = *(uint64_t*)((uint64_t)SteamInternal + 0x40);
    SteamInternal_SteamNetworkingSend_FUNC* SteamNetworkingSend = (SteamInternal_SteamNetworkingSend_FUNC*)**(uint64_t**)SteamNetworking;

    uint64_t steamid = *(uint64_t*)((uint64_t)SteamSessionMemberLight + 0xc8);

    //eP2PSendType = k_EP2PSendReliable
    bool success = SteamNetworkingSend((void*)SteamNetworking, steamid, data_buf, sizeof(data_buf), 2, 0);
    if (!success)
    {
        ConsoleWrite("Unable to disconnect player: error return val from SteamNetworkingSend");
    }

    //TODO also set variables in this SessionMember struct to disable the connection locally

    //Tell the host why we're dcing this guest
    wchar_t dc_msg[300];
    swprintf(dc_msg, 300, L"Disconnecting incoming guest player.\n%s", dc_reason);
    Game::show_popup_message(dc_msg);
}

//returns the type of the intercepted packet
uint32_t GetSteamData_Packet_injection_helper(void* data, uint32_t type, void* SteamSessionMemberLight)
{
    int64_t* data_offset_ptr = (int64_t*)(((uint64_t)data) + 0x18);
    int64_t data_offset = *data_offset_ptr;
    int64_t* data_size_ptr = (int64_t*)(((uint64_t)data) + 0x8);
    int64_t data_size = *data_size_ptr;
    uint8_t* data_buf = (uint8_t*)(*(uint64_t*)((uint64_t)data + 0x10) + data_offset);
    int64_t data_remaining = data_size - data_offset;

    if (!Game::get_SessionManagerImp_session_action_result().has_value())
    {
        return type;
    }
    SessionActionResultEnum session_action_result = Game::get_SessionManagerImp_session_action_result().value();

    //ConsoleWrite("GetSteamData_Packet_injection_helper session_action=%x type=%d from=%x", session_action_result, type, *(uint64_t*)((uint64_t)SteamSessionMemberLight + 0xc8));

    // 4. If we recieve a type36 AND we're the host (we created the session)
    if (type == 36 && (session_action_result == TryToCreateSession || session_action_result == CreateSessionSuccess))
    {
        //if we don't have the extra flags byte at the end of the packet, non-mod user
        //handle the fact that the dword at the end of the normal packet is optional
        if (data_remaining != 5 && data_remaining != 1)
        {
            ModNetworking::guest_mod_installed = false;
        }
        else
        {
            uint8_t value = *(data_buf+(data_remaining-1)); //ignore the dword of normal data at the end if it exists

            //ConsoleWrite("Host Read custom type36=%x from=%x", value, *(uint64_t*)((uint64_t)SteamSessionMemberLight + 0xc8));

            if ((value & MOD_ENABLED) != 0)
            {
                ModNetworking::guest_mod_installed = true;
            }
            else
            {
                ModNetworking::guest_mod_installed = false;
            }

            if ((value & LEGACY_ENABLED) != 0)
            {
                ModNetworking::guest_legacy_enabled = true;
            }
            else
            {
                ModNetworking::guest_legacy_enabled = false;
            }
        }

        // Parse the received data
        // As the host, we only change our settings if the connecting user is non-mod, and we allow non-mod connections, and we don't have any other non-mod phantoms in here
        if (ModNetworking::guest_mod_installed == false && ModNetworking::allow_connect_with_non_mod_guest == true)
        {
            // first non-mod user (we haven't updated the playernum at this point, the 1st guest will be 2), change settings
            if (Game::get_SessionManagerImp_Next_Player_Num().value_or(0) == 2)
            {
                Mod::set_mode(true, ModNetworking::guest_mod_installed);
            }
            // not first non-mod user
            else if (Game::get_SessionManagerImp_Next_Player_Num().value_or(0) > 2)
            {
                // If this new guest is a non-mod user but we already have a mod user connected
                if (Mod::get_mode() != Compatability)
                {
                    HostForceDisconnectSession(SteamSessionMemberLight, L"Incoming guest is a non-mod user, but mod user is already connected.");
                }
                else
                {
                    //no change needed, we're already in compatability mode
                }
            }
            // unknown playernum state
            else
            {
                HostForceDisconnectSession(SteamSessionMemberLight, L"Error. Next player number is <2, somehow. Report me.");
            }
        }
        // If specified in options, we must disconnect the non-mod player, since they won't on their own
        else if (ModNetworking::guest_mod_installed == false && ModNetworking::allow_connect_with_non_mod_guest == false)
        {
            HostForceDisconnectSession(SteamSessionMemberLight, L"Incoming guest is non-mod user, and you do not allow connections with non-mod users.");
        }
        // At this point, we've already sent our info packet
        // If the guest hasn't already updated to it and this packet doesn't reflect our configs, something is wrong, so DC them
        else if (ModNetworking::guest_mod_installed == true && ModNetworking::guest_legacy_enabled != Mod::legacy_mode)
        {
            HostForceDisconnectSession(SteamSessionMemberLight, L"Incoming guest is mod-user, but isn't matching your legacy/overhaul mode.");
        }
        // Otherwise we're good to go, the configs match!
        //ConsoleWrite("Host Config match!");

        // As the host, start up the clock syncronization function
        // TMP: Disable entirly
        //MainLoop::setup_mainloop_callback(HostTimerSync, NULL, "HostTimerSync");

        //game doesn't parse this extra byte in the packet, no need to remove
        return type;
    }

    // 2. If we recieve a type12 AND we're the new guest (we're joining the session)
    if (type == 12 && (session_action_result == TryToJoinSession))
    {
        //if we don't have the extra flags byte at the end of the packet, non-mod user
        if (!(data_remaining > 0))
        {
            ModNetworking::host_mod_installed = false;
        }
        else
        {
            uint8_t value = *data_buf;

            //ConsoleWrite("Guest Read custom type12=%x from=%x", value, *(uint64_t*)((uint64_t)SteamSessionMemberLight + 0xc8));

            if ((value & MOD_ENABLED) != 0)
            {
                ModNetworking::host_mod_installed = true;
            }
            else
            {
                ModNetworking::host_mod_installed = false;
            }

            if ((value & LEGACY_ENABLED) != 0)
            {
                ModNetworking::host_legacy_enabled = true;
            }
            else
            {
                ModNetworking::host_legacy_enabled = false;
            }
        }

        // Parse the received data
        // As the guest, we must change our settings to match the host (based on how we've configured our options), or else disconnect.
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

            //we can't kick out the other player since we're not the host. So pretend we received the kickout packet
            *data_size_ptr += 4;
            ((uint32_t*)data_buf)[0] = 0xff000019;
            return 46;
        }

        //game doesn't parse this extra byte in the packet, no need to remove
        return type;
    }

    return type;
}

uint64_t SendRawP2PPacket_injection_helper(uint8_t* data, uint64_t size, uint32_t type)
{
    // verify we're the SteamData packet type
    if (!(type == 1 || type == 3))
    {
        return size;
    }

    //sanity check size
    if (size < 7)
    {
        return size;
    }

    if (!Game::get_SessionManagerImp_session_action_result().has_value())
    {
        return size;
    }
    SessionActionResultEnum session_action_result = Game::get_SessionManagerImp_session_action_result().value();

    uint8_t packet_type = data[6];

    //ConsoleWrite("SendRawP2PPacket_injection_helper session_action=%x type=%d", session_action_result, packet_type);

    // 1. If we send a type12 AND we're the host (we created the session) OR a current guest
    // existing guest phantoms also send the type12 packet to the new phantom, so we need to account for that
    // It's ok to increase the length since the underlying buffer is 128 bytes long
    if (packet_type == 12 && (session_action_result == TryToCreateSession || session_action_result == CreateSessionSuccess || session_action_result == JoinSessionSuccess))
    {
        //ConsoleWrite("Send custom type12");
        //the mod is active (always true, since we're running the mod)
        uint8_t value = MOD_ENABLED;

        if (Mod::legacy_mode)
        {
            value |= LEGACY_ENABLED;
        }

        data[size] = value;
        return size+1;
    }

    // 3. If we send a type36 AND we're the new guest (we're joining the session)
    // It's ok to increase the length since the underlying buffer is 128 bytes long
    if (packet_type == 36 && (session_action_result == TryToJoinSession))
    {
        //ConsoleWrite("Guest Send custom type36");
        //the mod is active (always true, since we're running the mod)
        uint8_t value = MOD_ENABLED;

        if (Mod::legacy_mode)
        {
            value |= LEGACY_ENABLED;
        }

        data[size] = value;
        return size+1;
    }

    return size;
}

void getNetMessage_injection_helper(uint8_t* data, uint32_t size, uint32_t type)
{

}

void sendPacket_injection_helper(uint8_t* data, uint32_t size, uint32_t type)
{

}
