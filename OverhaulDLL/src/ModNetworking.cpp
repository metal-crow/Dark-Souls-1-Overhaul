#include "ModNetworking.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"

bool ModNetworking::host_mod_installed = false;
bool ModNetworking::host_legacy_enabled = false;
bool ModNetworking::guest_mod_installed = false;
bool ModNetworking::guest_legacy_enabled = false;

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
    if (!(type == 7 || type == 5) || size == 0)
    {
        return;
    }

    // If we receive a type5, we're the guest
    if (type == 5)
    {
        uint32_t value = *(uint32_t*)data;

        //it's encoded that the mod is active in the most significant bit
        if ((value & 0x80000000) != 0)
        {
            ModNetworking::host_mod_installed = true;
        }
        else
        {
            ModNetworking::host_mod_installed = false;
        }

        //it's encoded that the mod is in legacy mode in the 2nd most significant bit
        if ((value & 0x40000000) != 0)
        {
            ModNetworking::host_legacy_enabled = true;
        }
        else
        {
            ModNetworking::host_legacy_enabled = false;
        }

        //remove these flags before the game parses it
        *(uint32_t*)data = value & 0x3FFFFFFF;
        return;
    }

    // If we recieve a type7, we're the host
    if (type == 7)
    {
        uint8_t value = *data;

        //it's encoded that the mod is active in the most significant bit
        if ((value & 0b10000000) != 0)
        {
            ModNetworking::guest_mod_installed = true;
        }
        else
        {
            ModNetworking::guest_mod_installed = false;
        }

        //it's encoded that the mod is in legacy mode in the 2nd most significant bit
        if ((value & 0b01000000) != 0)
        {
            ModNetworking::guest_legacy_enabled = true;
        }
        else
        {
            ModNetworking::guest_legacy_enabled = false;
        }

        *data = 0; //just to be safe
        return;
    }
}

void sendPacket_injection_helper(uint8_t* data, uint32_t size, uint32_t type)
{
    //ignore types we don't care about/empty packets
    if (!(type == 7 || type == 5) || size == 0)
    {
        return;
    }

    // If we're sending a type5, we're the host
    // Abuse the player number packet to encode extra information. Since it can only be between 0 and MAX_PLAYERS, we can cut a few bits off
    if (type == 5)
    {
        uint32_t value = *(uint32_t*)data;

        //encode that the mod is active in the most significant bit (always gonna be true, since we're running the mod)
        value |= 0x80000000;

        //encode that the mod is in legacy mode in the 2nd most significant bit
        if (Mod::legacy_mode)
        {
            value |= 0x40000000;
        }

        *(uint32_t*)data = value;
        return;
    }

    // If we're sending a type7, we're the guest
    // This is always 1 byte of 0, which isn't read (other than getting checked that it's received)
    if (type == 7)
    {
        //encode that the mod is active in the most significant bit (always gonna be true, since we're running the mod)
        uint8_t value = 0b10000000;

        //encode that the mod is in legacy mode in the 2nd most significant bit
        if (Mod::legacy_mode)
        {
            value |= 0b01000000;
        }

        *data = value;
        return;
    }
}
