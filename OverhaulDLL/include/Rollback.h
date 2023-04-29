#pragma once
#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <stdint.h>
#include <unordered_map>

#include "DarkSoulsOverhaulMod.h"

#include "PlayerInsStructFunctions.h"
#include "BulletManStructFunctions.h"
#include "SfxManStructFunctions.h"
#include "DamageManStructFunctions.h"
#include "PadManipulatorStructFunctions.h"

#include "ggponet.h"

enum class GGPOREADY
{
    NotReady,
    ReadyAwaitingFrameHead,
    Ready
};

class Rollback
{
public:
    static void start();

    static void NetcodeFix();

    static bool rollbackToggle;
    static bool rollbackEnabled;

    static bool ggpoStarted;
    static GGPOREADY ggpoReady;
    static GGPOSession* ggpo;
    static GGPOSessionCallbacks ggpoCallbacks;
    static GGPOPlayerHandle ggpoHandles[GGPO_MAX_PLAYERS];
    static const size_t ggpoCurrentPlayerCount = 2;

    static void rollback_start_session(ISteamNetworkingMessages* steamMsgs);
    static void rollback_end_session();

    //used for testing only
    static const bool rollbackVisual = true; //a visual indicator of rollback for testing
    static bool gsave;
    static bool gload;
    static bool isave;
    static bool iload;
    static bool networkToggle;
    static bool networkTest;
    static PlayerIns* saved_playerins;
    static BulletMan* saved_bulletman;
    static FXManager* saved_sfxobjs;
    static DamageMan* saved_damageman;
    static PadManipulator** saved_PadManipulator;

private:
    static const uint64_t sendNetMessage_offset = 0x50b6b0;
    static const uint64_t getNetMessage_offset = 0x050b540;
    static const uint64_t init_playerins_with_padmanip_offset = 0x27ba143;
    static const uint64_t isPacketTypeValid_offset = 0x50f2d0;

    static const uint64_t rollback_game_frame_sync_inputs_offset = 0x15d4d5;
    static const uint64_t MainUpdate_end_offset = 0x15d701;
    static const uint64_t MoveMapStep_SetPlayerLockOn_FromController_offset = 0x24fd0b;

    static const uint64_t PlayerIns_Is_NetworkedPlayer_offsets[];
    static const uint64_t PlayerIns_IsHostPlayerIns_offsets[];

    //this overwrites unneeded dbg strings after the vtable that we can use to store another pointer so modifying the vtable call is trivial
    static const uint64_t EnemyIns_PlayerIns_Is_NetworkedPlayer_trampoline_offset = 0x1322e68 + 0x1702; //this one is if enemyins class uses it. Must be same offset
    static const uint64_t PlayerIns_PlayerIns_Is_NetworkedPlayer_trampoline_offset = 0x13251f0 + 0x1702; //this one is if playerins class uses it.  Must be same offset
    static const uint64_t ReplayGhostIns_PlayerIns_Is_NetworkedPlayer_trampoline_offset = 0x1326d20 + 0x1702; //this one is if ReplayGhostIns class uses it.  Must be same offset

    static const uint64_t EnemyIns_WorldChrManImp_IsHostPlayerIns_trampoline_offset = 0x1322e68 + 0x170A;
    static const uint64_t PlayerIns_WorldChrManImp_IsHostPlayerIns_trampoline_offset = 0x13251f0 + 0x170A;
    static const uint64_t ReplayGhostIns_WorldChrManImp_IsHostPlayerIns_trampoline_offset = 0x1326d20 + 0x170A;
};


typedef struct RollbackInput RollbackInput;
typedef struct RollbackState RollbackState;

#pragma pack(push, 1)

struct RollbackInput
{
    uint8_t const1;

    //this is only used for the local player, the remote player only needs LockonTargetHandle
    unsigned int bTargetLocked : 1;
    unsigned int bTargetLocked_Alt : 1;

    unsigned int toggle_left_hand_slot_selected : 1;
    unsigned int toggle_right_hand_slot_selected : 1;
    PadManipulatorPacked padmanipulator;
    uint16_t curSelectedMagicId; //this could be simulated, but let's not
    uint32_t curUsingItemId; //we can't simulate the inventory menus for non-host
    uint32_t equipment_array[20];//we can't simulate the inventory menus for non-host
};

#pragma pack(pop)

void PackRollbackInput(RollbackInput* out, PlayerIns* player);
void UnpackRollbackInput(RollbackInput* in, PlayerIns* player);

struct RollbackState
{
    PlayerIns* playerins[GGPO_MAX_PLAYERS];
    BulletMan* bulletman;
    SfxMan* sfman;
    DamageMan* damageman;
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

bool rollback_begin_game_callback(const char*);
bool rollback_advance_frame_callback(int);
bool rollback_load_game_state_callback(unsigned char* buffer, int);
bool rollback_save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int);
void rollback_free_buffer(void* buffer);
bool rollback_on_event_callback(GGPOEvent* info);
bool rollback_log_game_state(char* filename, unsigned char* buffer, int);

#endif
