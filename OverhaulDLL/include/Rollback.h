#pragma once
#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <stdint.h>

class Rollback
{
public:
    static void start();
private:
    static const uint64_t sendType1NetMessage_offset = 0x5031f0;
    static const uint64_t recvType1NetMessage_offset = 0x503440;
    static const uint64_t sendType9NetMessage_offset = 0x5031f0;
    static const uint64_t recvType9NetMessage_offset = 0x503440;
    static const uint64_t sendType16NetMessage_offset = 0x5056a0;
    static const uint64_t recvType16NetMessage_offset = 0x505700;
};

#endif
