#pragma once

#ifndef ModNetworking_H
#define ModNetworking_H

#include <cstdint>
#include <unordered_map>
#include <atomic>
#pragma warning( push )
#pragma warning( disable : 4996 )
#include "steam/steam_api.h"
#pragma warning( pop )
#include "ModData.h"

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
    STEAM_CALLBACK(ModNetworking, LobbyDataUpdateCallback, LobbyDataUpdate_t);

    //callback for the new ISteamNetworkingMessages api
    STEAM_CALLBACK(ModNetworking, SteamNetworkingMessagesSessionRequestCallback, SteamNetworkingMessagesSessionRequest_t);

    //Create a persistant lobby when a host, so it isn't JIT'd for new connecting users
    static void CreatePersistentLobby();
    //Register a LobbyCreated call result so we can handle the new persistant lobby we create
    static CCallResult<ModNetworking, LobbyCreated_t> LobbyCreatedCallResult;
    void LobbyCreatedCallback(LobbyCreated_t* result, bool bIOFailure);

    //The persistant lobby this player will always be in
    static void UpdatePersistentLobbyData(ModMode mode);
    static std::atomic<uint64_t> selfPersisantLobby;

    //Set upon entry into someone else's lobby; set to 0 upon exit
    static std::atomic<uint64_t> currentLobby;

    static std::atomic<uint64_t> incoming_guest_to_not_accept;

    //configuration options for who can connect if you're host/how to change settings if you're guest
    static bool allow_connect_with_non_mod_host;
    static bool allow_connect_with_legacy_mod_host;
    static bool allow_connect_with_overhaul_mod_host;
    static bool allow_connect_with_non_mod_guest;

private:
    static const uint64_t AcceptP2PSessionWithUser_injection_offset = 0x10b6ce0;
    static const uint64_t IsP2PPacketAvailable_1_injection_offset = 0x10ba160;
    static const uint64_t IsP2PPacketAvailable_2_injection_offset = 0x10ba1ee;
    static const uint64_t ReadP2PPacket_injection_offset = 0x10b879d;
    static const uint64_t SendP2PPacket_voice_injection_offset = 0x10bb1d4;
    static const uint64_t SendP2PPacket_injection_offset = 0x10bd8e7;
    static const uint64_t CloseP2PSessionWithUser_Replacement_injection_offset = 0x10bdf55;
    static const uint64_t sendType34SteamMessage_data_offset = 0x108bb78;
    static const uint64_t Start_SessionDisconnect_Task_offset = 0x1089190;
    static const uint64_t Steam_LeaveLobby_offset = 0x10b9b20;
    static const uint64_t Steam_CreateLobby_offset = 0x10b7c40;
};

#endif
