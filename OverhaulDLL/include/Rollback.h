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
#include "QInputMgrWindowsStructFunctions.h"
#include "InputDirectionMovementManStructFunctions.h"

#include "NetcodePackets.h"

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

    static void BuildRemotePlayerPacket(PlayerIns* playerins, MainPacket* out);
    static void LoadRemotePlayerPacket(MainPacket* pkt, PlayerIns* player, int32_t session_player_num);

    static bool rollbackToggle;
    static bool rollbackEnabled;

    static bool ggpoStarted;
    static GGPOREADY ggpoReady;
    static GGPOSession* ggpo;
    static GGPOSessionCallbacks ggpoCallbacks;
    static GGPOPlayerHandle ggpoHandles[GGPO_MAX_PLAYERS];
    static const size_t ggpoCurrentPlayerCount = 2;

    static void rollback_start_session(ISteamNetworkingMessages* steamMsgs);

    //used for testing only
    static bool gsave;
    static bool gload;
    static bool isave;
    static bool iload;
    static bool netcodeSwap;
    static bool netcodeTestingEnabled;
    static const uint32_t RollbackSinglePacketType = 2; //this is unused, and passes the checks the game does on the type
    static PlayerIns* saved_playerins;
    static PadMan** saved_padman;
    static QInputMgrWindows** saved_qinputman;
    static InputDirectionMovementMan** saved_InputDirectionMovementMan;
    static BulletMan* saved_bulletman;
    static FXManager* saved_sfxobjs;
    static DamageMan* saved_damageman;

private:
    static const uint64_t sendNetMessage_offset = 0x50b6b0;
    static const uint64_t getNetMessage_offset = 0x050b540;
    static const uint64_t send_generalplayerinfo_offset = 0x3976e0;
    static const uint64_t Read_GeneralPlayerData_offset = 0x3953f0;
    static const uint64_t disableType18PacketEnforcement_offset = 0x3226e0;
    static const uint64_t fixPhantomBulletGenIssue_offset = 0x4229bf;
    static const uint64_t isPacketTypeValid_offset = 0x50f2d0;
    static const uint64_t rollback_game_frame_sync_inputs_offset = 0x15d4d5;
    static const uint64_t MainUpdate_end_offset = 0x15d701;
};


typedef struct RollbackInput RollbackInput;
typedef struct RollbackLocalInput RollbackLocalInput;
typedef struct RollbackState RollbackState;

struct RollbackLocalInput
{
    PadMan* padman;
    QInputMgrWindows* qInputMgrWindows;
    InputDirectionMovementMan* inputDirectionMovementMan;
};

//this full packet is used both locally, and sent to the remote player
struct RollbackInput
{
    RollbackLocalInput local;
    MainPacket remote;
};

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

bool rollback_await_player_added_before_init(void* steamMsgs);

#endif
