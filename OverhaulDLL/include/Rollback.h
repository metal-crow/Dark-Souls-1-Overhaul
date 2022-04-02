#pragma once
#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <stdint.h>
#include "PlayerInsStruct.h"

class Rollback
{
public:
    static void start();
    static void save();

private:
    static PlayerIns* saved_playerins;
    static const uint64_t disableType18PacketEnforcement = 0x35b13b;
    static const uint64_t disableType18PacketSending = 0x5067f1;
};

#endif
