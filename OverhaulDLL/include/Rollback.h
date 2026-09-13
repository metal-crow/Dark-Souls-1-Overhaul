#pragma once
#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <stdint.h>
#include <unordered_map>

#include "DarkSoulsOverhaulMod.h"

#include "PlayerInsStruct.h"
#include "BulletManStruct.h"
#include "SfxManStruct.h"
#include "DamageManStruct.h"
#include "PadManipulatorStruct.h"
#include "ThrowManStruct.h"
#include "DmgHitRecordManImpStruct.h"
#include "FrpgHavokManImpStruct.h"

#include "VirtualPad.h"
#include "ggponet.h"

enum class GGPOREADY
{
    NotReady,
    ReadyAwaitingFrameHead,
    Ready
};

#if ROLLBACK_INPUT_TESTING
//Which end of the no-GGPO input loopback this instance is (see networkTest_tick).
enum class NetworkTestRole
{
    Send,   // read the local pad, ship it to the peer, and drive our own character with it
    Recv    // apply whatever arrives to the SENDER's character; never read the local pad
};
#endif

enum class StateTarget
{
    ToGame,
    ToLocal,
    Copy
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
//#define GGPO_SYNCTEST 1
#ifdef GGPO_SYNCTEST
    static const size_t ggpoCurrentPlayerCount = 1;
#else
    static const size_t ggpoCurrentPlayerCount = 2;
#endif

    static void rollback_start_session(ISteamNetworkingMessages* steamMsgs);
    static void rollback_end_session();

    //used for testing only
#ifdef GGPO_SYNCTEST
    static const bool rollbackVisual = false; //a visual indicator of rollback for testing
#else
    static const bool rollbackVisual = false; // true; //a visual indicator of rollback for testing
#endif
    //true only inside rollback_advance_frame_callback: this frame already ran once, so
    //anything that must happen once per REAL frame has to be skipped.
    static bool inRollbackResim;
#if ROLLBACK_INPUT_TESTING
    static bool networkToggle;
    static bool networkTest;
    static NetworkTestRole networkTestRole;
    static uint32_t networkTestSent;
    static uint32_t networkTestRecv;
    static uint32_t networkTestMissed;
    //last input handed to each player by UnpackRollbackInput, for "inputdiag"
    static struct RollbackInput lastAppliedInput[GGPO_MAX_PLAYERS];
    static struct InputDiag inputDiag[GGPO_MAX_PLAYERS];
    static int32_t item_override_for(uint32_t playerIndex);
#else
    //the item override and the P2P hooks gate on this; with the loopback compiled out it is
    //always false, so those tests fold away
    static constexpr bool networkTest = false;
#endif

private:
    static const uint64_t sendNetMessage_offset = 0x50b6b0;
    static const uint64_t getNetMessage_offset = 0x050b540;
    static const uint64_t init_playerins_with_padmanip_offset = 0x27ba143;
    static const uint64_t isPacketTypeValid_offset = 0x50f2d0;

    static const uint64_t MainUpdate_end_offset = 0x15d701;
    static const uint64_t MoveMapStep_SetPlayerLockOn_FromController_offset = 0x24fd0b;

    static const uint64_t Build_BulletIns_FollowupBullet_loop_fix_offset = 0x0fe6100;
    static const uint64_t get_item_currently_being_used_offset = 0x74a0e0;
    static const uint64_t call_EquipGameData_Reset_ItemBeingUsedFromInventory_offset = 0x3577c9;
    static const uint64_t simpleshapephantom_collisionDetails_iterate_offset = 0x9c763f;

    // SFX graveyard: hook the dealloc functions for SFXEntries and FxBehaviorNodes
    static const uint64_t Destruct_SFXEntry_offset = 0xff9490;
    static const uint64_t Destruct_FxBehaviorNode_offset = 0xfdf530;

    // DamageEntry graveyard: hook the callers to intercept all destructs+frees
    static const uint64_t Destruct_DamageEntry_offset1 = 0x3ca737;
    static const uint64_t Destruct_DamageEntry_offset2 = 0x3c8d03;
    static const uint64_t Destruct_DamageEntry_offset3 = 0x3c9015;

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


#if ROLLBACK_INPUT_TESTING
//Sticky per-player view of what the input path delivered, accumulated by
//UnpackRollbackInput and cleared by "inputdiag reset". An instantaneous read cannot see a
//button: the game runs at 60 Hz and the control plane is polled by hand, so the odds of
//catching the frame you pressed on are poor -- and the "(on click)" ids are true for one
//frame only. Same reason VirtualPad has a latch.
struct InputDiag
{
    uint32_t frames;
    uint32_t buttons_or;        // OR of every pad button mask seen
    uint32_t use_seen;          // CurrentFrame_ActionInputs.use_ButtonPressed was ever set
    uint32_t twohand_seen;      // highest change_2handing_state seen (0 = never toggled)
    uint32_t left_slot_or;
    uint32_t right_slot_or;
    int32_t  item_override_last = -1; // last override that was not -1; -1 == never overridden
    uint32_t l_index_mask;      // bit per l_hand_equipped_index value observed
    uint32_t r_index_mask;      // bit per r_hand_equipped_index value observed
    uint32_t style_mask;        // bit per equipped_weapon_style observed (1=OneHand,2=THL,3=THR)
    //Step_PadManipulator gates its whole action-input region (attacks AND the use button)
    //on ChrCtrl.enable & 4 together with ActionCtrl: when (enable & 4) is set and both
    //RecieveStateInput and ItemBeingUsedOverride are clear, it skips the assignments
    //entirely. These say whether that is what is eating the remote player's inputs.
    uint32_t enable_or;         // OR of ChrCtrl.enable
    uint32_t gate_skip;         // frames the action-input region would be skipped
    uint32_t recv_state;        // frames with ActionCtrl.bitfield.RecieveStateInput set
    uint32_t item_use_override; // frames with ActionCtrl.ItemBeingUsedOverride set
    uint32_t attack_seen;       // r1_weapon_attack_input_1 ever set (a gated input that is not 'use')
    //The two terms of Step_PadManipulator's Use_ButtonPressed = X || MenuMan109 || vtable_0x350
    //that we can observe from outside it, measured per player around the replayed step.
    uint32_t x_read;            // frames the pad accessor returned 0x5A pressed during replay
    uint32_t menu109;           // frames MenuMan slot 109 was non-zero going into the step
    //ChrIns inBowPrecisionShoot guards ONLY the magic and use assignments in
    //Step_PadManipulator -- the attack assignments sit above it -- which is exactly the split
    //we see. It is bit 4 of the byte at ChrIns+0x2a6, NOT the whole byte and not bit 0:
    //140397412 is TEST byte ptr [RAX + 0x2a6], 0x10 / JNZ past the block.
    //between what does and does not replicate.
    uint32_t bow_precision;     // frames (chrins[0x2a6] & 0x10) was set
    uint32_t chr_2a6_or;        // OR of that whole byte, to show which bits are actually live
};
#endif

typedef struct RollbackInput RollbackInput;
typedef struct RollbackState RollbackState;

#pragma pack(push, 1)

struct RollbackInput
{
    //this is only used for the local player, the remote player only needs LockonTargetHandle
    unsigned int bTargetLocked : 1;
    unsigned int bTargetLocked_Alt : 1;

    struct
    {
        //The pad as read at the VirtualMultiDevice accessor layer
        VirtualPadState pad;
        //Required since stick input is camera-relative: Compute_PadManipulator uses it
        float camera_x_rotation;
        float camera_y_rotation;
        //Which entity the sender is locked on to
        uint32_t LockonTargetHandle;
        //Which weapon slot is selected in each hand
        uint32_t left_hand_slot_selected;
        uint32_t right_hand_slot_selected;
        //The in-game menus' "use this item" request.
        //using an item from the inventory screen goes through MenuMan(109) and involves no button press at all
        uint32_t menu_item_use_request;
    } vpad;
    uint8_t curSelectedMagicSlot; //this could be simulated, but let's not
    uint32_t curUsingInventoryItemId; //we can't simulate the inventory menus
    uint32_t curSelectedQuickbarItemId;
    //uint8_t equipment_change_index;
    //uint32_t equipment_change_id;
    uint32_t equipment_array[20];//we can't simulate the inventory menus. can optimize this since only 1 item will change per-frame, just need to figure out how to get what equip changed
};

#pragma pack(pop)

void PackRollbackInput(RollbackInput* out, PlayerIns* player);
void UnpackRollbackInput(RollbackInput* in, PlayerIns* player, uint32_t playerIndex);

struct RollbackState
{
    PlayerIns* playerins[GGPO_MAX_PLAYERS];
    // Steam id of the player in the matching playerins[] slot
    // this gives us network-stable key to canonicalise the hash order
    // Not part of the simulation.
    uint64_t player_steam_ids[GGPO_MAX_PLAYERS];
    BulletMan* bulletman;
    SfxMan* sfxman;
    DamageMan* damageman;
    ThrowMan* throwman;
    DmgHitRecordManImp* dmghitrecordman;
    FrpgHavokManImp* havokman;
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

inline void* realloc_(void* dst, size_t size)
{
    if (dst == NULL)
    {
        return malloc_(size);
    }
    void* out = realloc(dst, size);
    if (out == NULL)
    {
        FATALERROR("Realloc failure. Unable to allocate %d bytes.", size);
    }
    return out;
}


bool rollback_begin_game_callback(const char*);
bool rollback_advance_frame_callback(int);
bool rollback_load_game_state_callback(unsigned char* buffer, int);
bool rollback_save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int);
void rollback_copy_buffer(void* buffer_dst, void* buffer_src);
void rollback_free_buffer(void* buffer);
bool rollback_on_event_callback(GGPOEvent* info);
bool rollback_log_game_state(char* filename, unsigned char* buffer, int);

#endif
