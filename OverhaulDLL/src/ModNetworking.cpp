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

//don't encode anything in most significant bit since -1 is a reserved value in dark souls player nums
const uint32_t MOD_ENABLED = 0x40000000; //it's encoded that the mod is active in the 2nd most significant bit
const uint32_t LEGACY_ENABLED = 0x20000000; //it's encoded that the mod is in legacy mode in the 3nd most significant bit
const uint32_t REMOVE_FLAGS = ~(MOD_ENABLED | LEGACY_ENABLED);

extern "C" {
    uint64_t sendPacket_injection_return;
    void sendPacket_injection();
    void sendPacket_injection_helper(uint8_t* data, uint32_t size, uint32_t type);

    uint64_t getNetMessage_injection_return;
    void getNetMessage_injection();
    void getNetMessage_injection_helper(uint8_t* data, uint32_t size, uint32_t type);
}

/*
 * Inject code that will piggypack on the packet sent upon the start of an initial connection with a person, and will provide data on the mod.
 * This will specify if the mod is installed, and what mode it is in (legacy/non-legacy)
 */
void ModNetworking::start()
{
    //handle reading the handshake
    uint8_t *write_address = (uint8_t*)(ModNetworking::sendPacket_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &sendPacket_injection_return, 1, &sendPacket_injection);

    //handle setting the handshake
    write_address = (uint8_t*)(ModNetworking::getNetMessage_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &getNetMessage_injection_return, 4, &getNetMessage_injection);
}

void getNetMessage_injection_helper(uint8_t* data, uint32_t size, uint32_t type)
{
    //ignore types we don't care about/empty packets
    if (!(type == 10 || type == 5) || size == 0)
    {
        return;
    }

    // If we receive a type5, we're the guest
    if (type == 5)
    {
        uint32_t value = *(uint32_t*)data;

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

        // Parse the received data
        // As the guest, we must change our settings to match the host (based on how we've configured our options), or else disconnect.
        if (ModNetworking::host_mod_installed == false && ModNetworking::allow_connect_with_non_mod_host == true)
        {
            //connecting to non-mod
            Mod::set_mode(true, ModNetworking::host_mod_installed);
        }
        else if (ModNetworking::host_mod_installed == true && ModNetworking::host_legacy_enabled == false && ModNetworking::allow_connect_with_overhaul_mod_host == true)
        {
            //connecting to non-legacy
            Mod::set_mode(ModNetworking::host_legacy_enabled, ModNetworking::host_mod_installed);
        }
        else if (ModNetworking::host_mod_installed == true && ModNetworking::host_legacy_enabled == true && ModNetworking::allow_connect_with_legacy_mod_host == true)
        {
            //connecting to legacy
            Mod::set_mode(ModNetworking::host_legacy_enabled, ModNetworking::host_mod_installed);
        }
        else
        {
            //disconnect
        }

        //remove these flags before the game parses it
        *(uint32_t*)data = value & REMOVE_FLAGS;
        return;
    }

    // If we recieve a type10 AND we're the host (we created the session)
    if (type == 10 && Game::get_SessionManagerImp_session_action_result() == CreateSessionSuccess)
    {
        uint32_t value = *(uint32_t*)data;

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

        // Parse the received data
        // As the host, we only change our settings if the connecting user is non-mod, and we allow non-mod connections, and we don't have any other non-mod phantoms in here
        // (Playernum is 1 for host, 2+ is the 1st connected guest)
        if (ModNetworking::guest_mod_installed == false && ModNetworking::allow_connect_with_non_mod_guest == true)
        {
            // first non-mod user, change settings
            if ((value & REMOVE_FLAGS) == 2)
            {
                Mod::set_mode(true, ModNetworking::guest_mod_installed);
            }
            // not first non-mod user
            else if ((value & REMOVE_FLAGS) > 2)
            {
                // If this new guest is a non-mod user but we already have a mod user connected
                if (Mod::get_mode() != Compatability)
                {
                    //disconnect
                }
                else
                {
                    //no change needed, we're already in compatability mode
                }
            }
        }
        // If specified in options, we must disconnect the non-mod player, since they won't on their own
        else if (ModNetworking::guest_mod_installed == false && ModNetworking::allow_connect_with_non_mod_guest == false)
        {
            //disconnect
        }
        // At this point, we've already sent our info packet (type5)
        // If the guest hasn't already updated to it and this packet doesn't reflect our configs, something is wrong, so DC them
        else if (ModNetworking::guest_mod_installed == true && ModNetworking::guest_legacy_enabled != Mod::legacy_mode)
        {
            //disconnect
        }
        // Otherwise we're good to go, the configs match!

        //remove these flags before the game parses it
        *(uint32_t*)data = value & REMOVE_FLAGS;
        return;
    }
}

void sendPacket_injection_helper(uint8_t* data, uint32_t size, uint32_t type)
{
    //ignore types we don't care about/empty packets
    if (!(type == 10 || type == 5) || size == 0)
    {
        return;
    }

    // If we're sending a type5, we're the host
    // Abuse the player number packet to encode extra information. Since it can only be between 0 and MAX_PLAYERS, we can cut a few bits off
    if (type == 5)
    {
        uint32_t value = *(uint32_t*)data;

        //the mod is active (always true, since we're running the mod)
        value |= MOD_ENABLED;

        if (Mod::legacy_mode)
        {
            value |= LEGACY_ENABLED;
        }

        *(uint32_t*)data = value;
        return;
    }

    // If we're sending a type10 AND we're the guest (check the player num in the type10 packet we're sending. 1 is host, 2+ is guest)
    // This also contains the player number value, so same as above
    if (type == 10 && *(uint32_t*)data > 1)
    {
        uint32_t value = *(uint32_t*)data;

        //the mod is active (always true, since we're running the mod)
        value |= MOD_ENABLED;

        if (Mod::legacy_mode)
        {
            value |= LEGACY_ENABLED;
        }

        *(uint32_t*)data = value;
        return;
    }
}
