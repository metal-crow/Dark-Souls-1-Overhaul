#pragma once
#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <stdint.h>

class Rollback
{
public:
    static void start();
private:
    static const uint64_t disableType18PacketEnforcement = 0x35b13b;
    static const uint64_t disableType18PacketSending = 0x5067f1;
};

#endif
