#pragma once

#ifndef ModNetworking_H
#define ModNetworking_H

#include <cstdint>
#include <unordered_map>
#pragma warning( push )
#pragma warning( disable : 4996 )
#include "steam/steam_api.h"
#pragma warning( pop )

class TimerClientInfo
{
public:
    TimerClientInfo()
    {
        timeOfLastResync = 0;
        timePingPacketSent = 0;
        packetDelay = 0;
        waitingForPingResponse = false;
        lastAidCount = 0xff;
    };
    uint64_t timeOfLastResync;
    uint64_t timePingPacketSent;
    uint64_t packetDelay;
    bool waitingForPingResponse;
    //state indicator for what animation we've received (the most sig 3 bits in the type1 packet)
    uint8_t lastAidCount;
};

class ModNetworking
{
public:
    ModNetworking() { }; //This is needed for the steam callbacks to work
    static void start();

    static ISteamUser* SteamUser;
    static ISteamFriends* SteamFriends;
    static ISteamMatchmaking* SteamMatchmaking;
    static ISteamNetworking* SteamNetworking;
    static ISteamNetworkingMessages* SteamNetMessages;
    static ISteamNetworkingUtils* SteamNetUtils;

    //Register a LobbyEnterCallback so that we can differentiate between mod and non-mod users without even using the ISteamNetworking Interface
    //This allows us to avoid leaking our IP on that interface
    STEAM_CALLBACK(ModNetworking, LobbyEnterCallback, LobbyEnter_t);
    STEAM_CALLBACK(ModNetworking, LobbyChatUpdateCallback, LobbyChatUpdate_t);
    STEAM_CALLBACK(ModNetworking, LobbyChatMsgCallback, LobbyChatMsg_t);
    STEAM_CALLBACK(ModNetworking, LobbyDataUpdateCallback, LobbyDataUpdate_t);

    //callback for the new ISteamNetworkingMessages api
    STEAM_CALLBACK(ModNetworking, SteamNetworkingMessagesSessionRequestCallback, SteamNetworkingMessagesSessionRequest_t);

    static uint64_t currentLobby; //Set upon entry into a lobby; set to 0 upon exit
    static bool lobby_setup_complete; //Set to true when a lobby handshake is complete and all settings are finalized; set to false upon lobby exit

    //configuration options for who can connect if you're host/how to change settings if you're guest
    static bool allow_connect_with_non_mod_host;
    static bool allow_connect_with_legacy_mod_host;
    static bool allow_connect_with_overhaul_mod_host;
    static bool allow_connect_with_non_mod_guest;

    //info about the current host/guest
    static bool host_got_info;
    static bool host_mod_installed;
    static bool host_legacy_enabled;
    static bool incoming_guest_got_info;
    static bool incoming_guest_mod_installed;
    static bool incoming_guest_legacy_enabled;
    static uint64_t incoming_guest_to_not_accept; //The steamid of the incoming user if we want to d/c them. When AcceptSessionWithUser/AcceptP2PSessionWithUser is called with this id, decline the session

    //info for clock synchronization
    static int64_t timer_offset;
    static std::unordered_map<uint64_t, TimerClientInfo> hostTimerSyncronizationData; //key: steamid
private:
    static const uint64_t AcceptP2PSessionWithUser_injection_offset = 0x10b3846;
    static const uint64_t IsP2PPacketAvailable_1_injection_offset = 0x10b6cc0;
    static const uint64_t IsP2PPacketAvailable_2_injection_offset = 0x10b6d45;
    static const uint64_t ReadP2PPacket_injection_offset = 0x10b52fd;
};

#endif
