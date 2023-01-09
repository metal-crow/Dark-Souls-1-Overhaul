#pragma once
#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <stdint.h>
#include <unordered_map>

#include "DarkSoulsOverhaulMod.h"

#include "PlayerInsStructFunctions.h"
#include "PadManStructFunctions.h"
#include "BulletManStructFunctions.h"
#include "SfxManStructFunctions.h"
#include "DamageManStructFunctions.h"

#include "NetcodePackets.h"

class Rollback
{
public:
    static void start();

    static void GameStateSave();
    static void GameStateLoad();

    static void GameInputSave();
    static void GameInputLoad();

    static void NetcodeFix();

    static void LoadRemotePlayerPacket(MainPacket* pkt, PlayerIns* player, uint64_t ConnectedPlayerData);

    static bool gsave;
    static bool gload;
    static bool isave;
    static bool iload;
    static bool netcodeSwap;

    static bool rollbackEnabled;

    static const uint32_t RollbackSinglePacketType = 2; //this is unused, and passes the checks the game does on the type
private:

    static PlayerIns* saved_playerins;
    static PadMan* saved_padman;
    static BulletMan* saved_bulletman;
    static class_14152d360* saved_sfxobjs;
    static DamageMan* saved_damageman;

    static const uint64_t sendNetMessage_offset = 0x50b6b0;
    static const uint64_t getNetMessage_offset = 0x050b540;
    static const uint64_t send_generalplayerinfo_offset = 0x3976e0;
    static const uint64_t Read_GeneralPlayerData_offset = 0x3953f0;
    static const uint64_t disableType18PacketEnforcement_offset = 0x3226e0;
    static const uint64_t fixPhantomBulletGenIssue_offset = 0x4229bf;
    static const uint64_t isPacketTypeValid_offset = 0x50f2d0;
};

inline void* malloc_(size_t size)
{
    void* out = malloc(size);
    if (out == NULL)
    {
        FATALERROR("Malloc failure. Unable to allocate %d bytes.", size);
    }
    memset(out, 0, size);
    return out;
}

#endif
