#pragma once

#ifndef ModNetworking_H
#define ModNetworking_H

#include <cstdint>

class ModNetworking
{
public:
    static void start();

    static bool host_mod_installed;
    static bool host_legacy_enabled;
    static bool guest_mod_installed;
    static bool guest_legacy_enabled;
private:
    static const uint64_t sendPacket_injection_offset = 0x5096dc; //injection is before the send call so we can modify the packet
    static const uint64_t getNetMessage_injection_offset = 0x509606;
};

#endif
