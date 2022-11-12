#pragma once

#ifndef _DS1_OVERHAUL_INVISIBLE_BACKSTAB_FIX_H_
#define _DS1_OVERHAUL_INVISIBLE_BACKSTAB_FIX_H_

#include <cstdint>

class InvisBackstabFix
{
public:
    static void start();
private:
    static const uint64_t send_throw_packet_injection_offset = 0x3a90dd;
    static const uint64_t force_sent_anim_packet_to_backstab_offset = 0x397965;
};

#endif
