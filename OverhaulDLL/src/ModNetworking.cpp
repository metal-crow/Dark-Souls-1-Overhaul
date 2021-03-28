#include "ModNetworking.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"

bool ModNetworking::allow_connect_with_non_mod_host = false;
bool ModNetworking::allow_connect_with_legacy_mod_host = false;
bool ModNetworking::allow_connect_with_overhaul_mod_host = false;
bool ModNetworking::allow_connect_with_non_mod_guest = false;

bool ModNetworking::host_mod_installed = false;
bool ModNetworking::host_legacy_enabled = false;
bool ModNetworking::guest_mod_installed = false;
bool ModNetworking::guest_legacy_enabled = false;

const uint8_t MOD_ENABLED = 0x80; //it's encoded that the mod is active in the most significant bit
const uint8_t LEGACY_ENABLED = 0x40; //it's encoded that the mod is in legacy mode in the 2nd most significant bit
const uint8_t REMOVE_FLAGS = ~(MOD_ENABLED | LEGACY_ENABLED);

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
}

/*
 * Inject code that will piggypack on the packet sent upon the start of an initial connection with a person, and will provide data on the mod.
 * This will specify if the mod is installed, and what mode it is in (legacy/non-legacy)
 */
void ModNetworking::start()
{
    global::cmd_out << (Mod::output_prefix + "Enabling mod networking...\n");

    uint8_t *write_address;

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
}


typedef void* SteamInternal_ContextInit_FUNC(uint64_t Init_SteamInternal_FUNCPTR);
SteamInternal_ContextInit_FUNC** SteamInternal_ContextInit = (SteamInternal_ContextInit_FUNC**)0x1420B6738; //the address of the location for the imported function address

uint64_t Init_SteamInternal_FUNCPTR = 0x141AC1020;

typedef void* SteamInternal_SteamNetworkingSend_FUNC(void* SteamNetworking, uint64_t steamid, const uint8_t* packet_data, uint32_t packet_len, uint32_t eP2PSendType, uint32_t nChannel);

const uint8_t data_buf[] = {
    (1 | (1 << 4)), //p2p packet type
    78, 88, 82, 86, //magic
    5, 132, //header
    46, //type KickOutTask
    255, 0, 0, 25 }; //data (const 0xff000019)

// Only call as Host, since this assumes session is connected and you can D/C other player
void HostForceDisconnectSession(void* SteamSessionMemberLight)
{
    void* SteamInternal = (*SteamInternal_ContextInit)(Init_SteamInternal_FUNCPTR);

    if (SteamInternal == NULL)
    {
        global::cmd_out << "Unable to disconnect player: unable to create SteamInternal\n";
        return;
    }

    uint64_t SteamNetworking = *(uint64_t*)((uint64_t)SteamInternal + 0x40);
    SteamInternal_SteamNetworkingSend_FUNC* SteamNetworkingSend = (SteamInternal_SteamNetworkingSend_FUNC*)**(uint64_t**)SteamNetworking;

    uint64_t steamid = *(uint64_t*)((uint64_t)SteamSessionMemberLight + 0xc8);

    //eP2PSendType = k_EP2PSendReliable
    bool success = SteamNetworkingSend((void*)SteamNetworking, steamid, data_buf, sizeof(data_buf), 2, 0);
    if (!success)
    {
        global::cmd_out << "Unable to disconnect player: error return val from SteamNetworkingSend\n";
    }

    //TODO also set variables in this SessionMember struct to disable the connection locally
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
        //handle the the that the dword at the end is optional
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
                    HostForceDisconnectSession(SteamSessionMemberLight);
                }
                else
                {
                    //no change needed, we're already in compatability mode
                }
            }
            // unknown playernum state
            else
            {
                HostForceDisconnectSession(SteamSessionMemberLight);
            }
        }
        // If specified in options, we must disconnect the non-mod player, since they won't on their own
        else if (ModNetworking::guest_mod_installed == false && ModNetworking::allow_connect_with_non_mod_guest == false)
        {
            HostForceDisconnectSession(SteamSessionMemberLight);
        }
        // At this point, we've already sent our info packet
        // If the guest hasn't already updated to it and this packet doesn't reflect our configs, something is wrong, so DC them
        else if (ModNetworking::guest_mod_installed == true && ModNetworking::guest_legacy_enabled != Mod::legacy_mode)
        {
            HostForceDisconnectSession(SteamSessionMemberLight);
        }
        // Otherwise we're good to go, the configs match!
        //ConsoleWrite("Host Config match!");

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
            //we can't kick out the other player since we're not the host. So pretend we received the kickout packet
            //ConsoleWrite("Guest Self-disconnect");
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
