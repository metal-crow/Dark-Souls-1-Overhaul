#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "InputUtil.h"
#include "ModNetworking.h"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#include "PlayerInsStructFunctions.h"
#include "BulletManStructFunctions.h"
#include "SfxManStructFunctions.h"
#include "DamageManStructFunctions.h"
#include "PadManipulatorStructFunctions.h"
#include "ThrowManStructFunctions.h"
#include "DmgHitRecordManImpStructFunctions.h"
#include "FrpgHavokManImpStructFunctions.h"
#include "StateHash.h"
#include "RollbackReplay.h"
#include "RollbackScript.h"
#include "VirtualPad.h"
#include "HavokTrace.h"
#include "WorldFreeze.h"
#include "RollbackStartSync.h"
#include "PlayerHandles.h"

FILE* hash_logfile = NULL;

GGPOSession* Rollback::ggpo = NULL;
GGPOPlayerHandle Rollback::ggpoHandles[GGPO_MAX_PLAYERS] = {};
bool Rollback::ggpoStarted = false;
GGPOREADY Rollback::ggpoReady = GGPOREADY::NotReady;

GGPOSessionCallbacks Rollback::ggpoCallbacks = {
    .begin_game = rollback_begin_game_callback,
    .save_game_state = rollback_save_game_state_callback,
    .load_game_state = rollback_load_game_state_callback,
    .log_game_state = rollback_log_game_state,
    .copy_buffer = rollback_copy_buffer,
    .free_buffer = rollback_free_buffer,
    .advance_frame = rollback_advance_frame_callback,
    .on_event = rollback_on_event_callback,
};

bool Rollback::inRollbackResim = false;
#if ROLLBACK_INPUT_TESTING
RollbackInput Rollback::lastAppliedInput[GGPO_MAX_PLAYERS] = {};
InputDiag Rollback::inputDiag[GGPO_MAX_PLAYERS] = {};
bool Rollback::networkToggle = false;
bool Rollback::networkTest = false;
bool network_toggle(void* unused)
{
    if (Rollback::networkToggle)
    {
        //must be toggled before other player joins
        Rollback::networkTest = !Rollback::networkTest;
        ConsoleWrite("Netcode %d", Rollback::networkTest);
        Rollback::networkToggle = false;
    }
    return true;
}

//Test-only: sample the pad outside a session so the harness can read it back. During a
//session PackRollbackInput does the capture.
bool virtualpad_capture_tick(void* unused)
{
#if VIRTUALPAD_SELFTEST
    //The self-test installs a replay for a whole frame; UnpackRollbackInput installs one per
    //player inside that frame. Nesting them would have the inner end_replay restore the
    //originals mid-frame, so they are mutually exclusive.
    if (VirtualPad::selftest_frames_remaining > 0 && !Rollback::ggpoStarted)
    {
        //captures internally, then installs the stubs for the rest of the frame
        VirtualPad::selftest_frame_start();
        return true;
    }
#endif
    //While a session is running PackRollbackInput does the capture; this tick would only be a
    //redundant second read of the same device state.
    if (!VirtualPad::replaying() && !Rollback::ggpoStarted)
    {
        VirtualPadState s{};
        VirtualPad::capture(&s);
    }
    return true;
}
#endif

bool Rollback::rollbackToggle = false;
bool Rollback::rollbackEnabled = false;
bool ggpo_toggle(void* unused)
{
    if (Rollback::rollbackToggle)
    {
        Rollback::rollbackEnabled = !Rollback::rollbackEnabled;
        ConsoleWrite("rollback %d", Rollback::rollbackEnabled);
        Rollback::rollbackToggle = false;
    }
    return true;
}


extern "C" {
    uint64_t get_item_currently_being_used_return;
    void get_item_currently_being_used_injection();
    uint8_t get_item_currently_being_used_injection_helper(EquipGameData*, ItemUsed*);
}

//The item each player is using, indexed the same way as Game::get_connected_player. -1 = no override.
static int32_t ItemIdOverride[GGPO_MAX_PLAYERS] = { -1, -1, -1, -1, -1, -1 };
#if ROLLBACK_INPUT_TESTING
int32_t Rollback::item_override_for(uint32_t i) { return i < GGPO_MAX_PLAYERS ? ItemIdOverride[i] : -1; }
#endif
static_assert(GGPO_MAX_PLAYERS == 6, "ItemIdOverride initialiser must cover every player slot");

uint8_t get_item_currently_being_used_injection_helper(EquipGameData* equip, ItemUsed* out)
{
    if (!Rollback::rollbackEnabled && !Rollback::networkTest)
    {
        return 0;
    }

    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            continue;
        }
        PlayerIns* player = (PlayerIns*)player_o.value();
        if (player->playergamedata == NULL || &player->playergamedata->equipGameData != equip)
        {
            continue;
        }

        if (ItemIdOverride[i] == -1)
        {
            return 0;
        }
        out->itemId = ItemIdOverride[i];
        out->amountUsed = 1;
        return 1;
    }
    return 0;
}

//The item id at an inventory index. -1 when the index is -1 (nothing selected, e.g. once the last item in a quickbar slot is used up) or out of range.
static int32_t inventory_item_id_at(const EquipInventoryDataItem* itemlist, uint32_t itemlistlen, int32_t index)
{
    if (index < 0 || (uint32_t)index >= itemlistlen)
    {
        return -1;
    }
    return itemlist[index].item_id;
}

//The session's players as (steam id, this machine's handle), in steam id order: the order RollbackHash hashes them in
static uint32_t session_players_by_steam_id(uint64_t* ids, uint32_t* handles)
{
    uint32_t n = 0;
    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount && n < GGPO_MAX_PLAYERS; i++)
    {
        auto p = Game::get_connected_player(i);
        if (!p.has_value() || p.value() == 0)
        {
            continue;
        }
        PlayerIns* pi = (PlayerIns*)p.value();
        uint64_t id = (pi->steamPlayerData != NULL && pi->steamPlayerData->steamOnlineIDData != NULL) ? pi->steamPlayerData->steamOnlineIDData->steam_id : 0;
        if (id == 0 && i == 0 && ModNetworking::SteamUser != NULL)
        {
            id = ModNetworking::SteamUser->GetSteamID().ConvertToUint64();
        }
        uint32_t k = n++;
        while (k > 0 && ids[k - 1] > id)
        {
            ids[k] = ids[k - 1];
            handles[k] = handles[k - 1];
            k--;
        }
        ids[k] = id;
        handles[k] = (uint32_t)pi->chrins.handle;
    }
    return n;
}

uint32_t PlayerHandles::to_canonical(uint32_t handle)
{
    uint64_t ids[GGPO_MAX_PLAYERS];
    uint32_t handles[GGPO_MAX_PLAYERS];
    const uint32_t n = session_players_by_steam_id(ids, handles);
    for (uint32_t k = 0; k < n; k++)
    {
        if (handles[k] == handle)
        {
            return CANONICAL_TAG | k;
        }
    }
    return handle;
}

uint32_t PlayerHandles::from_canonical(uint32_t canonical)
{
    if ((canonical & 0xFFFF0000) != CANONICAL_TAG)
    {
        return canonical;
    }
    uint64_t ids[GGPO_MAX_PLAYERS];
    uint32_t handles[GGPO_MAX_PLAYERS];
    const uint32_t n = session_players_by_steam_id(ids, handles);
    const uint32_t k = canonical & 0xFFFF;
    if (k >= n)
    {
        FATALERROR("PlayerHandles::from_canonical: player %u of %u", k, n);
    }
    return handles[k];
}

//Step_Chr_sub2's list of the characters it steps every frame, in world block order
struct ChrStepNode
{
    PlayerIns* chr;
    float frame_delta;
    uint32_t pad;
    ChrStepNode* next;
};
static_assert(offsetof(ChrStepNode, frame_delta) == 0x8);
static_assert(offsetof(ChrStepNode, next) == 0x10);

static uint32_t step_order_logs = 0;

//Each character's proxy integrates (moves and resolves its contacts) inside its own step, against where the others are at
//that moment. Two players in contact therefore come out differently depending on which steps first, and world block order
//puts each machine's own character first. Step the session players in steam id order instead: the nodes keep their places
//in the list and the players are dealt back into them sorted.
extern "C" void Step_Chr_canonical_order_helper(ChrStepNode* head)
{
    if (!Rollback::ggpoStarted)
    {
        return;
    }
    uint64_t ids[GGPO_MAX_PLAYERS];
    uint32_t handles[GGPO_MAX_PLAYERS];
    const uint32_t n = session_players_by_steam_id(ids, handles);

    ChrStepNode* nodes[GGPO_MAX_PLAYERS];
    uint32_t ranks[GGPO_MAX_PLAYERS];
    uint32_t found = 0;
    for (ChrStepNode* e = head; e != NULL && found < GGPO_MAX_PLAYERS; e = e->next)
    {
        if (e->chr == NULL)
        {
            continue;
        }
        for (uint32_t k = 0; k < n; k++)
        {
            if ((uint32_t)e->chr->chrins.handle == handles[k])
            {
                nodes[found] = e;
                ranks[found] = k;
                found++;
                break;
            }
        }
    }
    if (found != n)
    {
        FATALERROR("Step_Chr_canonical_order: %u of the session's %u players are in the step list", found, n);
    }

    PlayerIns* chrs[GGPO_MAX_PLAYERS];
    float deltas[GGPO_MAX_PLAYERS];
    for (uint32_t i = 0; i < found; i++)
    {
        chrs[ranks[i]] = nodes[i]->chr;
        deltas[ranks[i]] = nodes[i]->frame_delta;
    }
    bool reordered = false;
    for (uint32_t i = 0; i < found; i++)
    {
        reordered |= nodes[i]->chr != chrs[i];
        nodes[i]->chr = chrs[i];
        nodes[i]->frame_delta = deltas[i];
    }
    if (step_order_logs < 2)
    {
        step_order_logs++;
        ConsoleWrite("Step_Chr_canonical_order: %u session players in the step list, %s", found, reordered ? "reordered" : "already in order");
    }
}

//An inventory change the session's input cannot carry was refused (the injections in RollbackASM.asm say which)
extern "C" void rollback_item_change_blocked(uint32_t which)
{
    static const char* const names[] = { "map item pickup", "item lot award", "event script item award", "item drop",
        "Bottomless Box deposit", "Bottomless Box withdrawal" };
    static uint32_t logged = 0;
    if (logged < 20)
    {
        logged++;
        ConsoleWrite("Rollback: refused a %s during the session (only the input may change an inventory)",
            which < sizeof(names) / sizeof(names[0]) ? names[which] : "?");
    }
}

extern "C" {
    uint64_t MapItem_Pickup_return;
    void MapItem_Pickup_injection();
    uint64_t AwardItemLot_return;
    void AwardItemLot_injection();
    uint64_t LuaAddInventoryItem_return;
    void LuaAddInventoryItem_injection();
    uint64_t ItemDrop_return;
    void ItemDrop_injection();
    uint64_t BottomlessBoxDeposit_return;
    void BottomlessBoxDeposit_injection();
    uint64_t BottomlessBoxWithdraw_return;
    void BottomlessBoxWithdraw_injection();
}

extern "C" {
    uint64_t Step_Chr_canonical_order_return;
    void Step_Chr_canonical_order_injection();
    uint64_t ResonanceMagic_Arm_return;
    void ResonanceMagic_Arm_injection();
    uint64_t ChrCam_ApplyToPadManipulator_return;
    void ChrCam_ApplyToPadManipulator_injection();
    //While a rollback session runs, the live camera angles Apply_ChrCam_To_PlayerInsPadManipulator computes for the local
    //player land here instead of in its PadManipulator (see ChrCam_ApplyToPadManipulator_injection): [0] camera_y_rotation,
    //[1] camera_x_rotation, then the two fields it zeroes. The PadManipulator's are input, set from RollbackInput.
    alignas(16) float rollback_live_camera[4] = {};
}

void PackRollbackInput(RollbackInput* out, PlayerIns* player)
{
    EquipInventoryDataItem* itemlist = player->playergamedata->equipGameData.equippedInventory.itemlist2;
    uint32_t itemlistlen = player->playergamedata->equipGameData.equippedInventory.itemList2_len;

    //Read the game pad inputs.
    VirtualPad::capture(&out->vpad.pad);

    if (Rollback::ggpoStarted)
    {
        out->vpad.camera_x_rotation = rollback_live_camera[1];
        out->vpad.camera_y_rotation = rollback_live_camera[0];
    }
#if ROLLBACK_INPUT_TESTING
    else
    {
        out->vpad.camera_x_rotation = player->chrins.padManipulator->chrManipulator.camera_x_rotation;
        out->vpad.camera_y_rotation = player->chrins.padManipulator->chrManipulator.camera_y_rotation;
    }
#endif
    //a player's handle is this machine's; the receiver translates it back into its own (PlayerHandles.h)
    out->vpad.LockonTargetHandle = PlayerHandles::to_canonical(player->chrins.padManipulator->chrManipulator.LockonTargetHandle);

    //Read before our own Step_PadManipulator consumes it. Pack runs at MainUpdate entry, so
    //this is the request the menus left last frame
    out->vpad.menu_item_use_request = MenuMan_Get_Index(MENUMAN_INDEX_UNKNOWN109);

    //Outputs of the local menu step, not the game pad
    out->vpad.left_hand_slot_selected = player->chrins.padManipulator->chrManipulator.left_hand_slot_selected;
    out->vpad.right_hand_slot_selected = player->chrins.padManipulator->chrManipulator.right_hand_slot_selected;

    void* PadDevice = PadMan_GetPadDevice(0);
    //need to manually replicate the lockon code the game normally does
    bool lockonButton = PadDevice_GetInputI(PadDevice, 0x36); //R3(on click)
    uint8_t* bTargetLocked = (uint8_t*)((*(uint64_t*)Game::LockTgtManImp) + 0x1430);
    uint8_t* bTargetLocked_Alt = (uint8_t*)((*(uint64_t*)Game::LockTgtManImp) + 0x1431);
    if (lockonButton)
    {
        *bTargetLocked_Alt = (*bTargetLocked == 0);
    }
    out->bTargetLocked = *bTargetLocked;
    out->bTargetLocked_Alt = *bTargetLocked_Alt;

    //this is used both for what item we are using, and for saving the quickbar selected index
    out->curSelectedQuickbarItemId = inventory_item_id_at(itemlist, itemlistlen, (int32_t)player->playergamedata->equipGameData.equippedItemsInQuickbar.selectedQuickbarItem);

    out->curSelectedMagicSlot = player->playergamedata->equipGameData.equipMagicData->curSelectedMagicSlot;

    //this doesn't actually control the item being used, but just what item will be used when the use button is pressed
    out->curUsingInventoryItemId = inventory_item_id_at(itemlist, itemlistlen, (int32_t)player->playergamedata->equipGameData.itemInventoryIdCurrentlyBeingUsedFromInventory);

    for (size_t i = 0; i < InventorySlots::END; i++)
    {
        out->equipment_array[i] = Game::get_equipped_inventory((uint64_t)player, (InventorySlots)i);
    }
}

//Drive one player's PadManipulator by re-running the Step_PadManipulator with the
//sender's pad substituted at the accessor layer.
//Step_PadManipulator runs exactly once per player per frame, since we only enable set_ReadInputs_allowed here.
static void apply_virtualpad_input(RollbackInput* in, PlayerIns* player, uint32_t playerIndex)
{
    //Suppress Step_PadManipulator's global side effects for anyone but the local player on a
    //live frame. For a REMOTE player they would consult and mutate this machine's menus, which
    //are not theirs; on a RE-SIMULATED frame they would fire a second time for a frame that
    //already ran. The local player on a live frame is the one case where consulting our own
    //menus is exactly right, so leave it alone there.
    const bool suppress_globals = (playerIndex != 0) || Rollback::inRollbackResim;
    ChrManipulator* cm = &player->chrins.padManipulator->chrManipulator;

    //Give the player their own camera before the step. This mirrors exactly what
    //Apply_ChrCam_To_PlayerInsPadManipulator @140235400 writes for the viewing player,
    //including the two fields it always zeroes.
    cm->camera_x_rotation = in->vpad.camera_x_rotation;
    cm->camera_y_rotation = in->vpad.camera_y_rotation;
    cm->field27_0x58 = 0.0f;
    cm->field28_0x5c = 0;

    //Step_PadManipulator reads MenuMan slot 109 -- the in-game menus' "use this item" request
    //-- and consumes it by writing 109 and 99. Both are global, so for a remote player it would
    //read OUR menus and mutate them, and on a re-simulated frame it would consume them twice.
    //So substitute the SENDER's request for the duration of the step and put both slots back
    //afterwards.
    const uint32_t saved_109 = MenuMan_Get_Index(MENUMAN_INDEX_UNKNOWN109);
    const uint32_t saved_99 = MenuMan_Get_Index(MENUMAN_INDEX_UNKNOWN99);
#if ROLLBACK_INPUT_TESTING
    if (playerIndex < GGPO_MAX_PLAYERS && in->vpad.menu_item_use_request != 0) Rollback::inputDiag[playerIndex].menu109++;
#endif
    if (suppress_globals)
    {
        MenuMan_Set_IndexLookup(MENUMAN_INDEX_UNKNOWN109, (int32_t)in->vpad.menu_item_use_request);
    }

    VirtualPad::begin_replay(&in->vpad.pad);
    //Allow Step_PadManipulator to run
    Game::set_ReadInputs_allowed(true);
    Step_PadManipulator(player->chrins.padManipulator, FRAMETIME, player->chrins.playerCtrl);
    Game::set_ReadInputs_allowed(false);
    VirtualPad::end_replay();

    if (suppress_globals)
    {
        MenuMan_Set_IndexLookup(MENUMAN_INDEX_UNKNOWN109, (int32_t)saved_109);
        MenuMan_Set_IndexLookup(MENUMAN_INDEX_UNKNOWN99, (int32_t)saved_99);
    }

    cm->LockonTargetHandle = PlayerHandles::from_canonical(in->vpad.LockonTargetHandle);

    //Weapon-slot selection, for players whose in-game menu step did not run
    if (suppress_globals)
    {
        cm->left_hand_slot_selected = in->vpad.left_hand_slot_selected;
        cm->right_hand_slot_selected = in->vpad.right_hand_slot_selected;
    }
}

void UnpackRollbackInput(RollbackInput* in, PlayerIns* player, uint32_t playerIndex)
{
#if ROLLBACK_INPUT_TESTING
    //kept purely so the harness can show what each player was actually handed (see "inputdiag")
    if (playerIndex < GGPO_MAX_PLAYERS) Rollback::lastAppliedInput[playerIndex] = *in;
#endif
    EquipInventoryDataItem* itemlist = player->playergamedata->equipGameData.equippedInventory.itemlist2;
    uint32_t itemlistlen = player->playergamedata->equipGameData.equippedInventory.itemList2_len;

    //Replicate the code from ChrAsm_Set_Equipped_Items
    //This only updates the chrasm equip items, since the game will dynamically update the chr elsewhere based on this
    for (uint32_t equip_index = 0; equip_index < InventorySlots::END; equip_index++)
    {
        const uint32_t prev_equip_item = player->playergamedata->equipGameData.chrasm.equip_items[equip_index];

        //set the chr's equipped items
        //setting this here causes the game to update the other values dynamically (and correctly) when we later run PlayerIns_ComputeChanges
        player->playergamedata->equipGameData.chrasm.equip_items[equip_index] = in->equipment_array[equip_index];
        if (player->playergamedata->equipGameData.chrasm_alt != NULL)
        {
            player->playergamedata->equipGameData.chrasm_alt->equip_items[equip_index] = in->equipment_array[equip_index];
        }

        //handle changing equipment while 2 handing
        if (equip_index < 7)
        {
            if ((equip_index == player->playergamedata->equipGameData.chrasm.l_hand_equipped_index * 0x2) ||
                (equip_index == player->playergamedata->equipGameData.chrasm.r_hand_equipped_index * 0x2 + 0x1))
            {
                if (prev_equip_item != in->equipment_array[equip_index])
                {
                    player->playergamedata->equipGameData.chrasm.equipped_weapon_style = 0x1;
                }
            }
        }

        //update the mapping for this equipment's location in the inventory
        uint32_t inventory_index = Game::locate_inventory_index_for_itemid(itemlist, itemlistlen, in->equipment_array[equip_index]);
        if (equip_index < 7)
        {
            player->playergamedata->equipGameData.EquipItemToInventoryIndexMap_index_updated[equip_index] =
                player->playergamedata->equipGameData.EquipItemToInventoryIndexMap[equip_index] != inventory_index;
        }
        player->playergamedata->equipGameData.EquipItemToInventoryIndexMap[equip_index] = inventory_index;
    }

    //Update the current selected spell
    player->playergamedata->equipGameData.equipMagicData->curSelectedMagicSlot = in->curSelectedMagicSlot;

    //Update the current item to be used
    //the game relies on the inventory id for getting the current item in use (see get_item_currently_being_used)
    //this won't work for the remote user (no inventory), so we have manually inject our item id
    if (in->curUsingInventoryItemId != -1)
    {
        ItemIdOverride[playerIndex] = in->curUsingInventoryItemId;
    }
    //fall back to the quickbar if we don't have an inventory item
    else
    {
        ItemIdOverride[playerIndex] = in->curSelectedQuickbarItemId;
    }

    apply_virtualpad_input(in, player, playerIndex);

    uint32_t playerHandle = *(uint32_t*)(((uint64_t)player) + 8);
    if (playerHandle > Game::PC_Handle && playerHandle < Game::PC_Handle + 10)
    {
        //forcably set the PlayerCtrl->chrctrl_parent.NotLockedOn flag if the player is locked on. Dark souls will never set this itself
        uint32_t LockonTargetHandle = *(uint32_t*)(((uint64_t)(&player->chrins.padManipulator->chrManipulator)) + 0x220);
        uint8_t* NotLockedOn = (uint8_t*)(((uint64_t)(&player->chrins.playerCtrl->chrCtrl)) + 0x21D);
        if (LockonTargetHandle != -1)
        {
            *NotLockedOn = 0;
        }
        else
        {
            *NotLockedOn = 1;
        }
    }
    else
    {
        //manually set the LockTgtManImp->bTargetLocked_Alt flags for the host, since the game needs this flag set and directly sets it from the controller input
        uint8_t* bTargetLocked = (uint8_t*)((*(uint64_t*)Game::LockTgtManImp) + 0x1430);
        *bTargetLocked = in->bTargetLocked;
        uint8_t* bTargetLocked_Alt = (uint8_t*)((*(uint64_t*)Game::LockTgtManImp) + 0x1431);
        *bTargetLocked_Alt = in->bTargetLocked_Alt;

        //update the quickbar. only needed for host since it looks it up by inventory index
        player->playergamedata->equipGameData.equippedItemsInQuickbar.quickbar[0] = Game::locate_inventory_index_for_itemid(itemlist, itemlistlen, in->equipment_array[InventorySlots::Quickbar1]);
        player->playergamedata->equipGameData.equippedItemsInQuickbar.quickbar[1] = Game::locate_inventory_index_for_itemid(itemlist, itemlistlen, in->equipment_array[InventorySlots::Quickbar2]);
        player->playergamedata->equipGameData.equippedItemsInQuickbar.quickbar[2] = Game::locate_inventory_index_for_itemid(itemlist, itemlistlen, in->equipment_array[InventorySlots::Quickbar3]);
        player->playergamedata->equipGameData.equippedItemsInQuickbar.quickbar[3] = Game::locate_inventory_index_for_itemid(itemlist, itemlistlen, in->equipment_array[InventorySlots::Quickbar4]);
        player->playergamedata->equipGameData.equippedItemsInQuickbar.quickbar[4] = Game::locate_inventory_index_for_itemid(itemlist, itemlistlen, in->equipment_array[InventorySlots::Quickbar5]);
        player->playergamedata->equipGameData.equippedItemsInQuickbar.selectedQuickbarItem = Game::locate_inventory_index_for_itemid(itemlist, itemlistlen, in->curSelectedQuickbarItemId);

        //update the itemInventoryIdCurrentlyBeingUsedFromInventory. For the local player, we need this so the rollback doesn't clear it next frame and prevent us from continuing to read it
        player->playergamedata->equipGameData.itemInventoryIdCurrentlyBeingUsedFromInventory = Game::locate_inventory_index_for_itemid(itemlist, itemlistlen, in->curUsingInventoryItemId);
    }
#if ROLLBACK_INPUT_TESTING
    //Sticky record of what this player was actually handed, so the harness can be read
    //after the fact instead of having to catch the exact frame a button was down.
    if (playerIndex < GGPO_MAX_PLAYERS)
    {
        InputDiag& d = Rollback::inputDiag[playerIndex];
        d.frames++;
        d.buttons_or |= in->vpad.pad.buttons;
        if (ItemIdOverride[playerIndex] != -1) d.item_override_last = ItemIdOverride[playerIndex];
        if (player->chrins.padManipulator != NULL)
        {
            ChrManipulator* dcm = &player->chrins.padManipulator->chrManipulator;
            if (dcm->CurrentFrame_ActionInputs.use_ButtonPressed) d.use_seen = 1;
            if (dcm->change_2handing_state > d.twohand_seen) d.twohand_seen = dcm->change_2handing_state;
            //post-step, so this is what the step actually produced rather than what arrived
            d.left_slot_or |= dcm->left_hand_slot_selected;
            d.right_slot_or |= dcm->right_hand_slot_selected;
            if (dcm->CurrentFrame_ActionInputs.r1_weapon_attack_input_1) d.attack_seen = 1;
        }
        if (player->playergamedata != NULL)
        {
            ChrAsm& dca = player->playergamedata->equipGameData.chrasm;
            if (dca.l_hand_equipped_index < 32) d.l_index_mask |= (1u << dca.l_hand_equipped_index);
            if (dca.r_hand_equipped_index < 32) d.r_index_mask |= (1u << dca.r_hand_equipped_index);
            if (dca.equipped_weapon_style < 32) d.style_mask |= (1u << dca.equipped_weapon_style);
        }
        //The gate Step_PadManipulator puts in front of its whole action-input region.
        if (player->chrins.playerCtrl != NULL)
        {
            ChrCtrl& cc = player->chrins.playerCtrl->chrCtrl;
            d.enable_or |= cc.enable;
            if (cc.actionctrl != NULL)
            {
                const uint8_t* ac = (const uint8_t*)cc.actionctrl;
                const bool recv_state = (ac[0x1de] & 0x2) != 0;   // bitfield@0x1dc, RecieveStateInput
                const bool use_override = ac[0x1ae] != 0;         // ItemBeingUsedOverride
                if (recv_state) d.recv_state++;
                if (use_override) d.item_use_override++;
                if ((cc.enable & 4) != 0 && !recv_state && !use_override) d.gate_skip++;
            }
        }
        //Step_PadManipulator wraps ONLY its magic and use assignments in bit 4 of this byte
        //(140397412: TEST byte ptr [RAX + 0x2a6], 0x10 / JNZ past them); the attack
        //assignments sit above it, which is exactly the split we are seeing.
        if ((player->chrins.unk_2a6 & 0x10) != 0) d.bow_precision++;
        d.chr_2a6_or |= player->chrins.unk_2a6;
    }
#endif
}

//When GGPO has no input for a frame it predicts by repeating the newest input it has received from that player.
//It only returns zeroed bytes when there is nothing to repeat: before that player's first input has arrived, and
//for every frame after they disconnect.
//A real input is never all zero (curUsingInventoryItemId alone is -1 or an item id),
// and unpacking one would write item id 0 into every equipment slot.
static bool rollback_input_is_empty(const RollbackInput* in)
{
    const uint8_t* bytes = (const uint8_t*)in;
    for (size_t i = 0; i < sizeof(RollbackInput); i++)
    {
        if (bytes[i] != 0)
        {
            return false;
        }
    }
    return true;
}

void rollback_sync_inputs()
{
    RollbackInput inputs[GGPO_MAX_PLAYERS];
    int disconnect_flags = 0;

    //get the inputs for this frame
    GGPOErrorCode res = ggpo_synchronize_input(Rollback::ggpo, inputs, sizeof(RollbackInput) * GGPO_MAX_PLAYERS, &disconnect_flags);
    if (!GGPO_SUCCEEDED(res))
    {
        FATALERROR("ggpo_synchronize_input call returned %d", res);
    }

    //load the input states into the game to be used this frame
    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        //Skip the inputs GGPO had nothing to fill with, so the PadManipulator stays as it was on the frame being simulated from.
        if ((disconnect_flags & (1 << i)) != 0)
        {
            continue;
        }
        if (rollback_input_is_empty(&inputs[i]))
        {
            ConsoleWrite("sync_input returned an empty input for player %u, ignoring", i);
            continue;
        }

        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            FATALERROR("Unable to get playerins in rollback_load_game_state_callback");
        }
        PlayerIns* player = (PlayerIns*)player_o.value();
        UnpackRollbackInput(&inputs[i], player, i);
    }
}

/*
 * Input loopback test -- no GGPO, no rollback, no state save/restore.
 *
 * One instance is the SENDER: it reads its own pad, packs a RollbackInput exactly as the
 * rollback path would, ships it to the peer over Steam channel 1, and also applies it to
 * itself so the sender's character responds normally.
 *
 * The other is the RECEIVER: it applies whatever arrives to the SENDER's character (the
 * remote PlayerIns), through the same UnpackRollbackInput the rollback path uses.
 *
 * So if the receiver's view of the sender's character matches what the sender is doing, then
 * capture -> pack -> wire -> unpack -> Step_PadManipulator is correct. That is the half of
 * rollback worth isolating from prediction and state save/restore.
 *
 * Roles are explicit (`network role send|recv`) rather than inferred: this used to key off
 * having an Old Witch's Ring equipped, which is not something a test should depend on.
 */

#if ROLLBACK_INPUT_TESTING
NetworkTestRole Rollback::networkTestRole = NetworkTestRole::Send;
uint32_t Rollback::networkTestSent = 0;
uint32_t Rollback::networkTestRecv = 0;
uint32_t Rollback::networkTestMissed = 0;
static void networkTest_tick()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value() || player_o.value() == NULL) return;
    PlayerIns* player = (PlayerIns*)player_o.value();

    auto guest_o = Game::get_connected_player(1);
    if (!guest_o.has_value() || guest_o.value() == NULL) return;
    PlayerIns* guest = (PlayerIns*)guest_o.value();

    if (Rollback::networkTestRole == NetworkTestRole::Send)
    {
        //Refresh the device. Both input paths need this: the legacy one so
        //Step_PadManipulator has fresh pad state, the VirtualPad one because it is what
        //updates the VirtualMultiDevice that VirtualPad::capture reads.

        Game::set_ReadInputs_allowed(true);
        Step_PadMan(FRAMETIME);
        Game::set_ReadInputs_allowed(false);

        //menu/equipment resolution -- treated as input on both paths
        Game::set_StepInGameMenu_allowed(true);
        uint64_t ingamestep = (uint64_t)Game::get_InGameStep();
        uint64_t taskitem = *(uint64_t*)(ingamestep + 0x5ae0);
        uint64_t ingamemenustep = *(uint64_t*)(taskitem + 0x20);
        Step_InGameMenus((void*)ingamemenustep, FRAMETIME, (void*)taskitem);
        Game::set_StepInGameMenu_allowed(false);

        RollbackInput localInput{};
        PackRollbackInput(&localInput, player);


        SteamNetworkingIdentity target{};
        target.SetSteamID(guest->steamPlayerData->steamOnlineIDData->steam_id);
        ModNetworking::SteamNetMessages->SendMessageToUser(target, &localInput, sizeof(localInput),
            k_nSteamNetworkingSend_UnreliableNoNagle, 1);
        Rollback::networkTestSent++;

        //drive our own character from the very same input, so what the sender sees is produced
        //by the same pack/unpack pipeline the receiver runs -- otherwise a mismatch could be
        //blamed on either side
        UnpackRollbackInput(&localInput, player, 0);
        RollbackScript::observe(0, &localInput);
    }
    else
    {
        //receiver: never read the local pad, so the peer is the only thing moving anything
        Game::set_ReadInputs_allowed(false);
        Game::set_StepInGameMenu_allowed(false);

        SteamNetworkingMessage_t* new_message = NULL;
        int num_messages = 0;
        int i = 0;
        do
        {
            num_messages = ModNetworking::SteamNetMessages->ReceiveMessagesOnChannel(1, &new_message, 1);
            i++;
        } while (num_messages < 1 && i < 5);

        if (num_messages == 1)
        {
            if (new_message->GetSize() == sizeof(RollbackInput))
            {
                RollbackInput* remoteInput = (RollbackInput*)new_message->GetData();
                //index 1: this input belongs to the SENDER, who is our remote player
                UnpackRollbackInput(remoteInput, guest, 1);
                Rollback::networkTestRecv++;
                RollbackScript::observe(0, remoteInput);
            }
            else
            {
                //There is one input layout now, so a size mismatch means the two instances are running
                //different builds of the DLL.
                ConsoleWrite("networkTest: input size %d, expected %d -- the two instances are running different DLL builds",
                    new_message->GetSize(), (int)sizeof(RollbackInput));
            }
            new_message->Release();
        }
        else
        {
            Rollback::networkTestMissed++;
        }
    }
}
#endif

#ifdef GGPO_SYNCTEST
//curSp and the ChrAttachSys head slot of player 0 around every live and every re-simulated frame. A value that changes in
//live frames but never in re-simulated ones is stepped outside Step_GameSimulation, rather than not being restored.
struct SyncTestProbe
{
    bool valid = false;
    int32_t sp = 0;
    int32_t attach_head = -1;
    std::string dmg;      //active DamageMan entries (sorted: Step_DamageMan reverses the list every frame), the free-list head slot and player 0's current entry id
    //named values whose changes are logged per live and re-simulated frame ("SYNCTEST watch"): a value that changes on a
    //different frame in one than the other points at state the rollback does not save. Add to it while chasing a mismatch
    std::vector<std::pair<const char*, int64_t>> watch;
};

static std::string synctest_damage_summary(PlayerIns* player)
{
    DamageMan* dm = *(DamageMan**)Game::damage_man;
    if (dm == NULL)
    {
        return "";
    }
    std::vector<std::string> entries;
    char buf[64];
    for (DamageEntry* e = dm->active_damage_entries_list; e != NULL; e = e->next)
    {
        snprintf(buf, sizeof(buf), "%x%s ", e->id, e->attackerHandle == (uint32_t)player->chrins.handle ? "P" : "");
        entries.push_back(buf);
    }
    std::sort(entries.begin(), entries.end());
    std::string s;
    for (const std::string& e : entries)
    {
        s += e;
    }
    int64_t head = dm->all_damage_entries_list_cur == NULL ? -1 : dm->all_damage_entries_list_cur - dm->all_damage_entries_list_start;
    snprintf(buf, sizeof(buf), "| free_head=%lld cur=%x", head, *(uint32_t*)((uint8_t*)&player->chrins.unk_2d8 + 8));
    s += buf;
    return s;
}

static SyncTestProbe synctest_probe()
{
    SyncTestProbe p;
    auto player_o = Game::get_connected_player(0);
    if (!player_o.has_value() || player_o.value() == 0)
    {
        return p;
    }
    PlayerIns* player = (PlayerIns*)player_o.value();
    p.valid = true;
    p.sp = (int32_t)player->chrins.curSp;
    p.attach_head = player->chrins.chrattachsys.SysSlots != NULL ? (int32_t)player->chrins.chrattachsys.SysSlots->slotType : -1;
    p.dmg = synctest_damage_summary(player);
    if (player->playergamedata != NULL)
    {
        p.watch.push_back({ "attr_10b", ((uint8_t*)&player->playergamedata->attribs)[0x10b] });
        p.watch.push_back({ "gd_wstyle", player->playergamedata->equipGameData.chrasm.equipped_weapon_style });
    }
    if (player->chrasm != NULL)
    {
        p.watch.push_back({ "pi_wstyle", player->chrasm->equipped_weapon_style });
    }
    p.watch.push_back({ "c2h", player->chrins.padManipulator->chrManipulator.change_2handing_state });
    //ChrIns+0x160 (ghidra: frame_delta_update_amount_ms): the dt the character update (and the ragdoll keyframe) used, in microseconds
    p.watch.push_back({ "chr_dt_us", (int64_t)(*(float*)((uint8_t*)player + 0x160) * 1e6f) });
    return p;
}

static void synctest_observe(bool resim, const SyncTestProbe& before, const SyncTestProbe& after)
{
    if (!before.valid || !after.valid)
    {
        return;
    }
    RollbackHash::SyncTestStats& t = RollbackHash::synctest;
    (resim ? t.resim_frames : t.live_frames)++;
    if (after.sp > before.sp)
    {
        (resim ? t.sp_up_resim : t.sp_up_live)++;
    }
    if (after.watch != before.watch)
    {
        int frame = 0, confirmed = 0;
        ggpo_get_frame_info(Rollback::ggpo, &frame, &confirmed);
        std::string s;
        for (size_t i = 0; i < after.watch.size() && i < before.watch.size(); i++)
        {
            if (after.watch[i].second != before.watch[i].second)
            {
                s += " " + std::string(after.watch[i].first) + " " + std::to_string(before.watch[i].second) + "->" + std::to_string(after.watch[i].second);
            }
        }
        ConsoleWrite("SYNCTEST watch %s frame=%d%s", resim ? "resim" : "live", frame, s.c_str());
    }
    if (after.dmg != before.dmg)
    {
        int frame = 0, confirmed = 0;
        ggpo_get_frame_info(Rollback::ggpo, &frame, &confirmed);
        ConsoleWrite("SYNCTEST dmg %s frame=%d  %s  ->  %s", resim ? "resim" : "live", frame, before.dmg.c_str(), after.dmg.c_str());
    }
    if (after.attach_head != before.attach_head)
    {
        uint64_t& changes = resim ? t.attach_head_changes_resim : t.attach_head_changes_live;
        changes++;
        if (changes <= 10)
        {
            int frame = 0, confirmed = 0;
            ggpo_get_frame_info(Rollback::ggpo, &frame, &confirmed);
            ConsoleWrite("SYNCTEST %s frame=%d attach head slot %d -> %d", resim ? "resim" : "live", frame, before.attach_head, after.attach_head);
        }
    }
}

static SyncTestProbe synctest_live_before;

//Unrestored-write detector: bytes of player 0's objects that a live frame changed and the following load did not put
//back, i.e. state the simulation writes that RollbackState does not save. Captured at three points per frame:
//  PostAdvance  after ggpo_advance_frame returns (after the resim): the state the next live frame starts from
//  LiveEnd      at the end of that live frame, before ggpo_advance_frame saves it
//  AfterLoad    once the load of the previous frame is done
//A byte with PostAdvance != LiveEnd (the live frame wrote it) and AfterLoad == LiveEnd (the load did not revert it) is counted.
enum class UnrestoredStage { PostAdvance, LiveEnd, AfterLoad };

struct UnrestoredRegion
{
    const char* name;
    size_t size;                       //ghidra struct sizes
    std::vector<uint8_t> post_advance;
    std::vector<uint8_t> live_end;
    std::vector<uint32_t> hits;
    bool have_post = false;
    bool have_live = false;
};

static UnrestoredRegion unrestored_regions[] = {
    { "PlayerIns", 0x9d0 },
    { "PlayerGameData", 0x660 },
    { "PlayerCtrl", 0x370 },
    { "ChrIns_field0x18", sizeof(ChrIns_field0x18) },
    { "ChrIns_1c0", 0x68 },
    { "PadManipulator", sizeof(PadManipulator) },
    { "SpecialEffect", sizeof(SpecialEffect) },
    { "QwcSpEffectEquipCtrl", sizeof(QwcSpEffectEquipCtrl) },
    { "ChrIns_field0x2c8", sizeof(ChrIns_field0x2c8) },
};
static uint64_t unrestored_loads = 0;

static const uint8_t* unrestored_base(size_t region, PlayerIns* player)
{
    switch (region)
    {
    case 0: return (const uint8_t*)player;
    case 1: return (const uint8_t*)player->playergamedata;
    case 2: return (const uint8_t*)player->chrins.playerCtrl;
    case 3: return (const uint8_t*)player->chrins.field0x18;
    case 4: return *(const uint8_t**)((const uint8_t*)player + 0x1c8); //ChrIns_1c0* (ghidra), inside padding_4b
    case 5: return (const uint8_t*)player->chrins.padManipulator;
    case 6: return (const uint8_t*)player->chrins.specialEffects;
    case 7: return (const uint8_t*)player->chrins.qwcSpEffectEquipCtrl;
    case 8: return (const uint8_t*)player->chrins.field0x2c8;
    }
    return NULL;
}

static void synctest_unrestored_capture(UnrestoredStage stage)
{
    auto player_o = Game::get_connected_player(0);
    if (!player_o.has_value() || player_o.value() == 0)
    {
        return;
    }
    PlayerIns* player = (PlayerIns*)player_o.value();
    for (size_t i = 0; i < sizeof(unrestored_regions) / sizeof(unrestored_regions[0]); i++)
    {
        UnrestoredRegion& r = unrestored_regions[i];
        const uint8_t* base = unrestored_base(i, player);
        if (base == NULL)
        {
            continue;
        }
        switch (stage)
        {
        case UnrestoredStage::PostAdvance:
            r.post_advance.assign(base, base + r.size);
            r.have_post = true;
            break;
        case UnrestoredStage::LiveEnd:
            r.live_end.assign(base, base + r.size);
            r.have_live = true;
            break;
        case UnrestoredStage::AfterLoad:
            if (!r.have_post || !r.have_live)
            {
                break;
            }
            if (r.hits.size() != r.size)
            {
                r.hits.assign(r.size, 0);
            }
            for (size_t b = 0; b < r.size; b++)
            {
                if (r.post_advance[b] != r.live_end[b] && base[b] == r.live_end[b])
                {
                    r.hits[b]++;
                }
            }
            r.have_live = false; //one comparison per live frame
            break;
        }
    }
    if (stage == UnrestoredStage::AfterLoad)
    {
        unrestored_loads++;
    }
}

void synctest_unrestored_reset()
{
    for (UnrestoredRegion& r : unrestored_regions)
    {
        r.hits.assign(r.hits.size(), 0);
    }
    unrestored_loads = 0;
}

//The most-hit runs of adjacent unrestored bytes, with their bytes as of the last live frame
std::string synctest_unrestored_json(size_t top)
{
    struct Run
    {
        const char* region;
        size_t offset;
        size_t len;
        uint32_t max_hits;
        std::string bytes;
    };
    std::vector<Run> runs;
    for (const UnrestoredRegion& r : unrestored_regions)
    {
        size_t b = 0;
        while (b < r.hits.size())
        {
            if (r.hits[b] == 0)
            {
                b++;
                continue;
            }
            Run run{ r.name, b, 0, 0, "" };
            while (b < r.hits.size() && r.hits[b] != 0)
            {
                if (r.hits[b] > run.max_hits)
                {
                    run.max_hits = r.hits[b];
                }
                if (b < r.live_end.size() && run.bytes.size() < 64)
                {
                    char hex[3];
                    snprintf(hex, sizeof(hex), "%02x", r.live_end[b]);
                    run.bytes += hex;
                }
                b++;
            }
            run.len = b - run.offset;
            runs.push_back(run);
        }
    }
    std::sort(runs.begin(), runs.end(), [](const Run& x, const Run& y) { return x.max_hits > y.max_hits; });

    std::string s = "{\"loads\":" + std::to_string(unrestored_loads) + ",\"run_count\":" + std::to_string(runs.size()) + ",\"runs\":[";
    for (size_t i = 0; i < runs.size() && i < top; i++)
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s{\"region\":\"%s\",\"offset\":\"0x%zx\",\"len\":%zu,\"max_hits\":%u,\"bytes\":\"%s\"}",
            i ? "," : "", runs[i].region, runs[i].offset, runs[i].len, runs[i].max_hits, runs[i].bytes.c_str());
        s += buf;
    }
    return s + "]}";
}
#endif

bool rollback_game_frame_start_helper(void* unused)
{
    if (Rollback::rollbackEnabled && Rollback::ggpoStarted)
    {
        //this is where the game state is loaded/a rollback is triggered
        ggpo_idle(Rollback::ggpo, 0); //timeout isn't actually used by this function

        if (Rollback::ggpoReady == GGPOREADY::ReadyAwaitingFrameHead)
        {
            //Frame 0 is saved on the first ggpo_add_local_input, so every machine's characters have to be
            //made identical before it (see RollbackStartSync.h)
            if (RollbackStartSync::tick())
            {
                Rollback::ggpoReady = GGPOREADY::Ready;
            }
        }

        if (Rollback::ggpoReady == GGPOREADY::Ready)
        {
            auto player_o = Game::get_PlayerIns();
            if (!player_o.has_value() || player_o.value() == NULL)
            {
                FATALERROR("Unable to get playerins for PC in rollback_game_frame_start_helper");
            }
            PlayerIns* player = (PlayerIns*)player_o.value();


            //Manually call the PadMan function here
            //ONLY allow it to be called here, so we don't have it called normally by the game and overwrite our custom inputs
            Game::set_ReadInputs_allowed(true);
            //This function is called as part of MainUpdate, and needs to be called first before Step_PadManipulator.
            //It reads the controller directly and normalizes it post-keybinds.
            //This refreshes the VirtualMultiDevice that VirtualPad::capture reads.
            Step_PadMan(FRAMETIME);
            Game::set_ReadInputs_allowed(false);

            //We need to process the menu actions here and resolve equipment state, since they are effectivly "input"
            //So they have to be called early so the state is resolved and we can read its outcome, then overwrite it
            //Which means we also have to block them from being called later by Step_TaskMan, which might cause side effects since the function is called twice
            //It also probably directly reads from the controller/PadDevice struct, so need to do it here anyway because we don't overwrite that. We overwrite the PadManipulator
            //This includes "menus" like toggling equipped spells
            Game::set_StepInGameMenu_allowed(true);
            uint64_t ingamestep = (uint64_t)Game::get_InGameStep();
            uint64_t taskitem = *(uint64_t*)(ingamestep + 0x5ae0);
            uint64_t ingamemenustep = *(uint64_t*)(taskitem + 0x20);
            Step_InGameMenus((void*)ingamemenustep, FRAMETIME, (void*)taskitem);
            Game::set_StepInGameMenu_allowed(false);

            //read the local inputs the above calls have resolved to
            RollbackInput localInput{};
            PackRollbackInput(&localInput, player);


            //Test-harness input pipeline, keyed by the GGPO framecount (the same index STATEHASH uses):
            //  replay (base) -> script (overlay) -> record (captures the final input)
            //so a scripted session's recording replays faithfully, and a script can patch a replayed recording.
            int rr_framecount = 0, rr_last_confirmed = 0;
            ggpo_get_frame_info(Rollback::ggpo, &rr_framecount, &rr_last_confirmed);
            RollbackReplay::apply_replay(rr_framecount, &localInput);
            RollbackScript::apply(rr_framecount, &localInput);
            RollbackReplay::capture_record(rr_framecount, &localInput);
            RollbackScript::observe(rr_framecount, &localInput);

            //notify ggpo of the local player's inputs
            GGPOErrorCode result = ggpo_add_local_input(Rollback::ggpo, Rollback::ggpoHandles[0], &localInput, sizeof(RollbackInput));
            //We are as far ahead of the other player as GGPO can predict. Hold this frame until their inputs arrive
            //(ggpo_idle polls the network, and may roll back as they do) instead of simulating past what can be corrected.
            const ULONGLONG waiting_since = GetTickCount64();
            uint32_t waited_polls = 0;
            while (result == GGPO_ERRORCODE_PREDICTION_THRESHOLD && GetTickCount64() - waiting_since < 10000)
            {
                Sleep(1);
                ggpo_idle(Rollback::ggpo, 0);
                result = ggpo_add_local_input(Rollback::ggpo, Rollback::ggpoHandles[0], &localInput, sizeof(RollbackInput));
                waited_polls++;
            }
            if (waited_polls != 0 && GGPO_SUCCEEDED(result))
            {
                ConsoleWrite("GGPO: held frame %d for %llu ms at the prediction threshold", rr_framecount, GetTickCount64() - waiting_since);
            }
            if (!GGPO_SUCCEEDED(result))
            {
                FATALERROR("Unable to ggpo_add_local_input. %d", result);
            }

            rollback_sync_inputs();
            WorldFreeze::tick();
            clean_free_DamageEntry_slots(*(DamageMan**)Game::damage_man);
            HavokTrace::sample(HavokTrace::Stage::LiveBegin);
#ifdef GGPO_SYNCTEST
            synctest_live_before = synctest_probe();
#endif
        }
    }

#if ROLLBACK_INPUT_TESTING
    if (Rollback::networkTest)
    {
        networkTest_tick();
    }
#endif

    return true;
}

extern "C" {
    uint64_t dsr_frame_finished_return;
    void dsr_frame_finished_injection();
    void dsr_frame_finished_helper();
}

static int ggpo_eventcode_timesync_frames_ahead = 0;

void dsr_frame_finished_helper()
{
#if VIRTUALPAD_SELFTEST
    //outside the session checks below: the self-test runs with or without a session
    VirtualPad::selftest_frame_end();
#endif

    if (Rollback::rollbackEnabled && Rollback::ggpoStarted)
    {
        //only start telling ggpo we're running once the players are synced
        if (Rollback::ggpoReady == GGPOREADY::Ready)
        {
#ifdef GGPO_SYNCTEST
            synctest_observe(false, synctest_live_before, synctest_probe());
            synctest_unrestored_capture(UnrestoredStage::LiveEnd);
#endif
            HavokTrace::sample(HavokTrace::Stage::LiveEnd);
            //this is where the game state is actually saved (and also loaded if we are in SyncTest)
            ggpo_advance_frame(Rollback::ggpo);
#ifdef GGPO_SYNCTEST
            synctest_unrestored_capture(UnrestoredStage::PostAdvance);
#endif

            //Emit STATEHASH log lines for any frames that are now confirmed
            int framecount_unused = 0;
            int last_confirmed_frame = 0;
            if (GGPO_SUCCEEDED(ggpo_get_frame_info(Rollback::ggpo, &framecount_unused, &last_confirmed_frame)))
            {
                RollbackHash::emit_confirmed(last_confirmed_frame);
            }

            if (Rollback::rollbackVisual)
            {
                float* visability = (float*)((uint64_t)Game::get_PlayerIns().value() + 0x328);
                if (*visability < 1.0f)
                {
                    *visability += 0.2f;
                }
            }

            if (ggpo_eventcode_timesync_frames_ahead > 0)
            {
                //slow us down a bit
                Sleep((1000 * 1) / 60);
                ggpo_eventcode_timesync_frames_ahead--;
            }
        }
    }
}

extern "C" {
    uint64_t MoveMapStep_SetPlayerLockOn_FromController_offset_return;
    void MoveMapStep_SetPlayerLockOn_FromController_offset_injection();
    bool* ggpoStarted_ptr;

    uint64_t followupBullet_loop_return;
    void followupBullet_loop_injection();
    void followupBullet_loop_helper(uint64_t);
}

//manually set the fxentry_a ptr in the followupbullet
//I can't figure out why it's sometimes not set (caused by my code somehow),
//so i'm patching it this way to fix it.
void followupBullet_loop_helper(uint64_t FxBehaviorNode_body)
{
    uint64_t followupBullet = *(uint64_t*)(FxBehaviorNode_body + 0x20);
    uint64_t FxBehaviorNode = *(uint64_t*)(FxBehaviorNode_body + 0x8);
    uint64_t* fxentry_a = (uint64_t*)(followupBullet + 0x10);
    *fxentry_a = FxBehaviorNode;
}

bool rollback_await_init(void* steamMsgs);

void* hkThreadMemory_BlockAlloc(void* hkThreadMemory, uint32_t size)
{
    void* p = malloc_(size);
    HavokTrace::on_alloc(p, size, _ReturnAddress());
    return p;
}

void hkThreadMemory_BlockFree(void* hkThreadMemory, void* ptr, uint32_t size)
{
    HavokTrace::on_free(ptr, size, _ReturnAddress());
    return free(ptr);
}

void* hkThreadMemory_bufAlloc(void* param_1, uint32_t* reqNumInOut)
{
    return malloc_(*reqNumInOut);
}

void hkThreadMemory_bufFree(void* param_1, void* ptr, uint32_t numElem)
{
    return free(ptr);
}

void* hkThreadMemory_bufRealloc(void* param_1, void* ptr, int oldNum, int* reqNumInOut)
{
    void* ptr2 = malloc_(*reqNumInOut);
    uint32_t copySize = oldNum;
    if (*reqNumInOut < oldNum)
    {
        copySize = *reqNumInOut;
    }
    memcpy(ptr2, ptr, copySize);
    free(ptr);
    return ptr2;
}

void hkThreadMemory_blockAllocBatch(void* param_1, void** ptrsOut, int numPtrs, int blockSize)
{
    for (size_t i = 0; i < numPtrs; i++)
    {
        ptrsOut[i] = malloc_(blockSize);
        HavokTrace::on_alloc(ptrsOut[i], blockSize, _ReturnAddress());
    }
}
void hkThreadMemory_blockFreeBatch(void* param_1, void** ptrsIn, int numPtrs, int blockSize)
{
    for (size_t i = 0; i < numPtrs; i++)
    {
        HavokTrace::on_free(ptrsIn[i], blockSize, _ReturnAddress());
        free(ptrsIn[i]);
    }
}

extern "C" {
    uint64_t simpleshapephantom_collisionDetails_iterate_return;
    void simpleshapephantom_collisionDetails_iterate_injection();

    uint64_t Destruct_SFXEntry_return;
    void Destruct_SFXEntry_injection();

    uint64_t Destruct_FxBehaviorNode_return;
    void Destruct_FxBehaviorNode_injection();

    uint64_t DamageEntry_Clear_id_return;
    void DamageEntry_Clear_id_injection();

    uint64_t WorldObjMan_step_live_return;
    void WorldObjMan_step_live_injection();
    uint64_t WorldObjMan_step_objs_return;
    void WorldObjMan_step_objs_injection();
    uint64_t WorldObjMan_step_objs_post_return;
    void WorldObjMan_step_objs_post_injection();
    uint64_t WorldObjActMan_step_return;
    void WorldObjActMan_step_injection();
    uint64_t DamageMan_EntryCount_return;
    void DamageMan_EntryCount_injection();
    uint64_t DamageEntry_HitChr_return;
    void DamageEntry_HitChr_injection();
}

//EquipGameData_New (1407463c0) sizes a player's inventory list by its "main player" flag: 0x800 items and 0x40 keys for
//the local player, 0x20 and 4 for every other player slot, since vanilla only ever keeps the other players' equipment.
//Under rollback every machine simulates every character from the same state, and UnpackRollbackInput resolves each
//equipped item id against that character's inventory list, so the other players need their whole inventory too (the
//start-state handshake sends it). GameDataMan_New builds all the slots once at boot, before rollback can be switched
//on, so this is unconditional: it only makes the other slots' lists as large as the local one (~0.3 MB in total).
//The flag's other use, allowTrophyUnlock, is untouched.
static const uint64_t EquipGameData_New_keys_capacity_offset = 0x74642d;   //41 B8 04 00 00 00   mov r8d, 4
static const uint64_t EquipGameData_New_items_capacity_offset = 0x74643a;  //BA 20 00 00 00      mov edx, 0x20

void Rollback::full_size_remote_inventories()
{
    uint8_t* keys = (uint8_t*)(Game::ds1_base + EquipGameData_New_keys_capacity_offset);
    uint8_t* items = (uint8_t*)(Game::ds1_base + EquipGameData_New_items_capacity_offset);
    const uint8_t keys_original[] = { 0x41, 0xB8, 0x04, 0x00, 0x00, 0x00 };
    const uint8_t items_original[] = { 0xBA, 0x20, 0x00, 0x00, 0x00 };
    if (memcmp(keys, keys_original, sizeof(keys_original)) != 0 || memcmp(items, items_original, sizeof(items_original)) != 0)
    {
        FATALERROR("full_size_remote_inventories: EquipGameData_New is not the code this patch was written for");
    }
    uint8_t keys_patch[] = { 0x41, 0xB8, 0x40, 0x00, 0x00, 0x00 };   //mov r8d, 0x40
    uint8_t items_patch[] = { 0xBA, 0x00, 0x08, 0x00, 0x00 };        //mov edx, 0x800
    sp::mem::patch_bytes(keys, keys_patch, sizeof(keys_patch));
    sp::mem::patch_bytes(items, items_patch, sizeof(items_patch));
}

void Rollback::start()
{
    ConsoleWrite("Rollback...");
    uint8_t* write_address;

    SetEnvironmentVariable("ggpo.log", "1");
#ifdef GGPO_SYNCTEST
    //Log a checksum mismatch and keep running, so one session reports every desyncing frame.
    //Otherwise synctest raises 0xC0000000, which the crash handler turns into a crash-report dialog.
    SetEnvironmentVariable("ggpo.synctest.nonfatal", "1");
#endif

    Rollback::NetcodeFix();

    //Synchronize input at the start of each frame
    MainLoop::setup_mainloop_callback(rollback_game_frame_start_helper, NULL, "rollback_game_frame_start_helper");

    //Inform ggpo after a frame has been rendered
    write_address = (uint8_t*)(Rollback::MainUpdate_end_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &dsr_frame_finished_return, 0, &dsr_frame_finished_injection);

    //prevent the game from directly reading the controller and setting our lockon. We need to only use the RollbackInput
    write_address = (uint8_t*)(Rollback::MoveMapStep_SetPlayerLockOn_FromController_offset + Game::ds1_base);
    ggpoStarted_ptr = &Rollback::ggpoStarted;
    sp::mem::code::x64::inject_jmp_14b(write_address, &MoveMapStep_SetPlayerLockOn_FromController_offset_return, 2, &MoveMapStep_SetPlayerLockOn_FromController_offset_injection);

    //fix an issue where the FollowupBullet doesn't have the right ptr to it's parent sometimes
    write_address = (uint8_t*)(Rollback::Build_BulletIns_FollowupBullet_loop_fix_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &followupBullet_loop_return, 2, &followupBullet_loop_injection);

    //manually specify the item ID being used, instead of the inventory id
    write_address = (uint8_t*)(Rollback::get_item_currently_being_used_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &get_item_currently_being_used_return, 6, &get_item_currently_being_used_injection);

    //fix an issue where the itemInventoryIdCurrentlyBeingUsedFromInventory is getting instantly cleared after 1 frame. We need it to stay around for the whole animation
    //unclear why this happens, but this call is what's doing it. Normally it's cleared by the TAE
    write_address = (uint8_t*)(Rollback::call_EquipGameData_Reset_ItemBeingUsedFromInventory_offset + Game::ds1_base);
    uint8_t nop[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
    sp::mem::patch_bytes(write_address, nop, 5);

    //SFX graveyard: hook the raw dealloc paths so any object in the graveyard
    //is held alive (and the dealloc args remembered) until refcount drops to 0
    //write_address = (uint8_t*)(Rollback::Destruct_SFXEntry_offset + Game::ds1_base);
    //sp::mem::code::x64::inject_jmp_14b(write_address, &Destruct_SFXEntry_return, 1, &Destruct_SFXEntry_injection);
    //write_address = (uint8_t*)(Rollback::Destruct_FxBehaviorNode_offset + Game::ds1_base);
    //sp::mem::code::x64::inject_jmp_14b(write_address, &Destruct_FxBehaviorNode_return, 1, &Destruct_FxBehaviorNode_injection);

    //Disable all thread specific allocations. Replace it with global malloc/free
    //this is needed because otherwise we can't know what hkThreadMemory instance to allocate or free an object under
    uint64_t* hkThreadMemory_vftable = (uint64_t*)0x14144cb48;
    unsigned long old;
    VirtualProtect(hkThreadMemory_vftable, 0x68, PAGE_READWRITE, &old);
    hkThreadMemory_vftable[1] = (uint64_t)(&hkThreadMemory_BlockAlloc);
    hkThreadMemory_vftable[2] = (uint64_t)(&hkThreadMemory_BlockFree);
    hkThreadMemory_vftable[3] = (uint64_t)(&hkThreadMemory_bufAlloc);
    hkThreadMemory_vftable[4] = (uint64_t)(&hkThreadMemory_bufFree);
    hkThreadMemory_vftable[5] = (uint64_t)(&hkThreadMemory_bufRealloc);
    hkThreadMemory_vftable[6] = (uint64_t)(&hkThreadMemory_blockAllocBatch);
    hkThreadMemory_vftable[7] = (uint64_t)(&hkThreadMemory_blockFreeBatch);

    //Check if a m_collisionDetail is null before trying to use it in-game. This makes life simpler for me when copying them
    write_address = (uint8_t*)(Game::ds1_base + Rollback::simpleshapephantom_collisionDetails_iterate_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &simpleshapephantom_collisionDetails_iterate_return, 1, &simpleshapephantom_collisionDetails_iterate_injection);

    //A retired damage entry's new id must not reuse a generation a discarded timeline handed out (DamageManStructFunctions.cpp)
    write_address = (uint8_t*)(Game::ds1_base + Rollback::Clear_DamageEntry_id_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &DamageEntry_Clear_id_return, 3, &DamageEntry_Clear_id_injection);

    //While a session runs the object steps get a frame time of 0 (WorldFreeze.h)
    write_address = (uint8_t*)(Game::ds1_base + Rollback::WorldObjMan_step_live_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &WorldObjMan_step_live_return, 1, &WorldObjMan_step_live_injection);
    write_address = (uint8_t*)(Game::ds1_base + Rollback::WorldObjMan_step_objs_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &WorldObjMan_step_objs_return, 1, &WorldObjMan_step_objs_injection);
    write_address = (uint8_t*)(Game::ds1_base + Rollback::WorldObjMan_step_objs_post_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &WorldObjMan_step_objs_post_return, 1, &WorldObjMan_step_objs_post_injection);
    write_address = (uint8_t*)(Game::ds1_base + Rollback::WorldObjActMan_step_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &WorldObjActMan_step_return, 1, &WorldObjActMan_step_injection);
    //...and only the session players can create damage entries
    write_address = (uint8_t*)(Game::ds1_base + Rollback::DamageMan_EntryCount_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &DamageMan_EntryCount_return, 2, &DamageMan_EntryCount_injection);
    //...and they can only hit each other
    write_address = (uint8_t*)(Game::ds1_base + Rollback::DamageEntry_HitChr_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &DamageEntry_HitChr_return, 0, &DamageEntry_HitChr_injection);

    write_address = (uint8_t*)(Game::ds1_base + Rollback::ChrCam_ApplyToPadManipulator_store_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &ChrCam_ApplyToPadManipulator_return, 0, &ChrCam_ApplyToPadManipulator_injection);

    //no covenant resonance during sessions: see ResonanceMagic_Arm_injection
    write_address = (uint8_t*)(Game::ds1_base + Rollback::ResonanceMagic_Arm_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &ResonanceMagic_Arm_return, 1, &ResonanceMagic_Arm_injection);

    //step the session players in the same order on every machine: see Step_Chr_canonical_order_helper
    write_address = (uint8_t*)(Game::ds1_base + Rollback::Step_Chr_canonical_order_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Step_Chr_canonical_order_return, 1, &Step_Chr_canonical_order_injection);

    //only the input may change an inventory during a session: refuse pickups, awards, drops and the Bottomless Box
    write_address = (uint8_t*)(Game::ds1_base + Rollback::MapItem_Pickup_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &MapItem_Pickup_return, 3, &MapItem_Pickup_injection);
    write_address = (uint8_t*)(Game::ds1_base + Rollback::AwardItemLot_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &AwardItemLot_return, 0, &AwardItemLot_injection);
    write_address = (uint8_t*)(Game::ds1_base + Rollback::LuaAddInventoryItem_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &LuaAddInventoryItem_return, 0, &LuaAddInventoryItem_injection);
    write_address = (uint8_t*)(Game::ds1_base + Rollback::ItemDrop_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &ItemDrop_return, 0, &ItemDrop_injection);
    write_address = (uint8_t*)(Game::ds1_base + Rollback::BottomlessBoxDeposit_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &BottomlessBoxDeposit_return, 0, &BottomlessBoxDeposit_injection);
    write_address = (uint8_t*)(Game::ds1_base + Rollback::BottomlessBoxWithdraw_offset);
    sp::mem::code::x64::inject_jmp_14b(write_address, &BottomlessBoxWithdraw_return, 1, &BottomlessBoxWithdraw_injection);

    MainLoop::setup_mainloop_callback(ggpo_toggle, NULL, "ggpo_toggle");
#if ROLLBACK_INPUT_TESTING
    MainLoop::setup_mainloop_callback(network_toggle, NULL, "network_toggle");
#endif
#if ROLLBACK_INPUT_TESTING
    MainLoop::setup_mainloop_callback(virtualpad_capture_tick, NULL, "virtualpad_capture_tick");
#endif

#ifdef GGPO_SYNCTEST
    //used for GGPO SyncTest
    MainLoop::setup_mainloop_callback(rollback_await_init, NULL, "rollback_await_init");
#endif
}

bool rollback_begin_game_callback(const char*)
{
    return true;
}

/*
* Notification from GGPO we should step foward exactly 1 frame
* during a rollback.
*/
bool rollback_advance_frame_callback(int)
{
    //Marks everything in this callback as a RE-simulation of a frame that already ran, so the
    //input path can skip side effects that must only happen once per real frame.
    Rollback::inRollbackResim = true;
    rollback_sync_inputs();
    clean_free_DamageEntry_slots(*(DamageMan**)Game::damage_man);

    //step next frame
#ifdef GGPO_SYNCTEST
    const SyncTestProbe synctest_before = synctest_probe();
#endif
    HavokTrace::sample(HavokTrace::Stage::ResimBegin);
    Game::Step_GameSimulation();
    HavokTrace::sample(HavokTrace::Stage::ResimEnd);
#ifdef GGPO_SYNCTEST
    synctest_observe(true, synctest_before, synctest_probe());
#endif
    Rollback::inRollbackResim = false;
    ggpo_advance_frame(Rollback::ggpo);

    //ConsoleWrite("rollback_advance_frame_callback finished");
    return true;
}

/*
 * Makes our current state match the state passed in by GGPO.
 */
bool rollback_load_game_state_callback(unsigned char* buffer, int)
{
    RollbackState* state = (RollbackState*)buffer;
    HavokTrace::sample(HavokTrace::Stage::LoadBegin);

    //sfx must be restored before playerins/bulletman so that linked_followupBullet chain heads
    //are cleared before any FollowupBullet data is modified or freed
    //copy_SfxMan(*(SfxMan**)Game::sfx_man, state->sfxman, StateTarget::ToGame);
    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            FATALERROR("Unable to get playerins %d in rollback_load_game_state_callback", i);
        }
        PlayerIns* player = (PlayerIns*)player_o.value();

        copy_PlayerIns(player, state->playerins[i], StateTarget::ToGame);
    }
    copy_BulletMan(*(BulletMan**)Game::bullet_man, state->bulletman, StateTarget::ToGame);
    copy_DamageMan(*(DamageMan**)Game::damage_man, state->damageman, StateTarget::ToGame);
    copy_ThrowMan(*(ThrowMan**)Game::throw_man, state->throwman, StateTarget::ToGame);
    copy_DmgHitRecordManImp(*(DmgHitRecordManImp**)Game::dmg_hit_record_man, state->dmghitrecordman, StateTarget::ToGame);
    //havok goes last: damage phantoms are put in or taken out of the world to match the DamageMan just restored,
    //and the character proxy's restored contacts are checked against its restored phantom
    copy_FrpgHavokManImp(*(FrpgHavokManImp**)Game::frpg_havok_man_imp, state->havokman, StateTarget::ToGame);

    if (Rollback::rollbackVisual)
    {
        *(float*)((uint64_t)Game::get_PlayerIns().value() + 0x328) = 0.4f;
    }
    HavokTrace::sample(HavokTrace::Stage::LoadEnd);
#ifdef GGPO_SYNCTEST
    synctest_unrestored_capture(UnrestoredStage::AfterLoad);
#endif
    //ConsoleWrite("rollback_load_game_state_callback finish");

    return true;
}

/*
 * Save the current state to a buffer and return it to GGPO via the
 * buffer and len parameters.
 */
RollbackState* rollback_capture_state()
{
    RollbackState* state = (RollbackState*)malloc(sizeof(RollbackState));
    if (state == NULL)
    {
        FATALERROR("Unable to get allocate state for rollback_save_game_state_callback");
    }

    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            FATALERROR("Unable to get playerins %d in rollback_save_game_state_callback", i);
        }
        PlayerIns* player = (PlayerIns*)player_o.value();

        state->playerins[i] = init_PlayerIns();
        copy_PlayerIns(state->playerins[i], player, StateTarget::ToLocal);
        // Capture the network-stable identity of this slot for the oracle.
        state->player_steam_ids[i] =
            (player->steamPlayerData != NULL && player->steamPlayerData->steamOnlineIDData != NULL)
                ? player->steamPlayerData->steamOnlineIDData->steam_id : 0;
    }
    state->bulletman = init_BulletMan();
    copy_BulletMan(state->bulletman, *(BulletMan**)Game::bullet_man, StateTarget::ToLocal);
    //state->sfxman = init_SfxMan();
    //copy_SfxMan(state->sfxman, *(SfxMan**)Game::sfx_man, StateTarget::ToLocal);
    state->damageman = init_DamageMan();
    copy_DamageMan(state->damageman, *(DamageMan**)Game::damage_man, StateTarget::ToLocal);
    state->throwman = init_ThrowMan();
    copy_ThrowMan(state->throwman, *(ThrowMan**)Game::throw_man, StateTarget::ToLocal);
    state->dmghitrecordman = init_DmgHitRecordManImp();
    copy_DmgHitRecordManImp(state->dmghitrecordman, *(DmgHitRecordManImp**)Game::dmg_hit_record_man, StateTarget::ToLocal);
    state->havokman = init_FrpgHavokManImp();
    copy_FrpgHavokManImp(state->havokman, *(FrpgHavokManImp**)Game::frpg_havok_man_imp, StateTarget::ToLocal);
    return state;
}

bool rollback_save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int frame)
{
    HavokTrace::sample(HavokTrace::Stage::Save);
    RollbackState* state = rollback_capture_state();

    //Record the per-subsystem state digest for the determinism oracle, keyed by the
    //GGPO frame being saved. It is emitted later as a STATEHASH log line once the
    //frame is confirmed. combined() also serves as the GGPO synctest checksum
    //(covers single-process save/restore determinism with the same hash).
    RollbackHash::StateDigest digest = RollbackHash::record(frame, state);

    *buffer = (unsigned char*)state;
    *len = sizeof(RollbackState);
    *checksum = (int)RollbackHash::combined(digest);

    return true;
}

void rollback_copy_buffer(void* buffer_dst, void* buffer_src)
{
    RollbackState* state_src = (RollbackState*)buffer_src;
    RollbackState* state_dst = (RollbackState*)buffer_dst;

    for (size_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        state_dst->playerins[i] = init_PlayerIns();
        copy_PlayerIns(state_dst->playerins[i], state_src->playerins[i], StateTarget::Copy);
        state_dst->player_steam_ids[i] = state_src->player_steam_ids[i];
    }
    state_dst->bulletman = init_BulletMan();
    copy_BulletMan(state_dst->bulletman, state_src->bulletman, StateTarget::Copy);
    //state_dst->sfxman = init_SfxMan();
    //copy_SfxMan(state_dst->sfxman, state_src->sfxman, StateTarget::Copy);
    state_dst->damageman = init_DamageMan();
    copy_DamageMan(state_dst->damageman, state_src->damageman, StateTarget::Copy);
    state_dst->throwman = init_ThrowMan();
    copy_ThrowMan(state_dst->throwman, state_src->throwman, StateTarget::Copy);
    state_dst->dmghitrecordman = init_DmgHitRecordManImp();
    copy_DmgHitRecordManImp(state_dst->dmghitrecordman, state_src->dmghitrecordman, StateTarget::Copy);
    state_dst->havokman = init_FrpgHavokManImp();
    copy_FrpgHavokManImp(state_dst->havokman, state_src->havokman, StateTarget::Copy);
#ifdef GGPO_SYNCTEST
    //synctest replays against this copy, so it must hash the same as the save it came from
    RollbackHash::synctest_check_copy(state_src, state_dst);
#endif
}

void rollback_free_buffer(void* buffer)
{
    RollbackState* state = (RollbackState*)buffer;

    for (size_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        free_PlayerIns(state->playerins[i]);
        state->playerins[i] = NULL;
    }
    free_BulletMan(state->bulletman);
    state->bulletman = NULL;
    //free_SfxMan(state->sfxman);
    state->sfxman = NULL;
    free_DamageMan(state->damageman);
    state->damageman = NULL;
    free_ThrowMan(state->throwman);
    state->throwman = NULL;
    free_DmgHitRecordManImp(state->dmghitrecordman);
    state->dmghitrecordman = NULL;
    free_FrpgHavokManImp(state->havokman);
    state->havokman = NULL;

    free(state);
}

bool rollback_on_event_callback(GGPOEvent* info)
{
    switch (info->code)
    {
    case GGPO_EVENTCODE_CONNECTED_TO_PEER:
        ConsoleWrite("GGPO_EVENTCODE_CONNECTED_TO_PEER");
        break;
    case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
        ConsoleWrite("GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER");
        break;
    case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
        ConsoleWrite("GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER");
        break;
    case GGPO_EVENTCODE_RUNNING:
        ConsoleWrite("GGPO_EVENTCODE_RUNNING");
        Rollback::ggpoReady = GGPOREADY::ReadyAwaitingFrameHead;
        break;
    case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
        ConsoleWrite("GGPO_EVENTCODE_CONNECTION_INTERRUPTED");
        break;
    case GGPO_EVENTCODE_CONNECTION_RESUMED:
        ConsoleWrite("GGPO_EVENTCODE_CONNECTION_RESUMED");
        break;
    case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
        ConsoleWrite("GGPO_EVENTCODE_DISCONNECTED_FROM_PEER");
        break;
    case GGPO_EVENTCODE_TIMESYNC:
        ConsoleWrite("GGPO_EVENTCODE_TIMESYNC");
        ggpo_eventcode_timesync_frames_ahead = info->u.timesync.frames_ahead;
        break;
    }
    return true;
}

//Only the synctest backend calls this: once for the original and once for the replayed state of a mismatching frame.
bool rollback_log_game_state(char* filename, unsigned char* buffer, int)
{
#ifdef GGPO_SYNCTEST
    //"synctest dump" picks which subsystems' mismatches are worth a dump
    if (!RollbackHash::synctest_dump_this_mismatch)
    {
        return true;
    }
    //A mismatch that repeats every frame would otherwise write two full dumps per frame. "synctest reset" re-arms this.
    if (RollbackHash::synctest.dump_files_written >= RollbackHash::SYNCTEST_MAX_DUMP_FILES)
    {
        return true;
    }
    RollbackHash::synctest.dump_files_written++;
#endif
    FILE* fp = nullptr;
    fopen_s(&fp, filename, "w");
    if (!fp)
    {
        return true;
    }

    //The oracle's canonical text for every hashed subsystem, so the original/replay pair diffs down to the field
    std::string text = RollbackHash::dump_text_of((RollbackState*)buffer);
    fwrite(text.data(), 1, text.size(), fp);
    fclose(fp);
    ConsoleWrite("SYNCTEST wrote %s", filename);
    return true;
}

void Rollback::rollback_end_session()
{
    if (Rollback::ggpoStarted)
    {
        WorldFreeze::unfreeze();
        RollbackStartSync::end_session();
        RollbackReplay::end_session();
        RollbackScript::end_session();
        Rollback::ggpoStarted = false;
        Rollback::ggpoReady = GGPOREADY::NotReady;
        GGPOErrorCode result = ggpo_close_session(Rollback::ggpo);
        Rollback::ggpo = NULL;
        if (!GGPO_SUCCEEDED(result))
        {
            FATALERROR("unable to close ggpo. %d", result);
        }
    }
}

//How many consecutive frames every player has to pass the checks in rollback_await_init before the session starts.
//A character that has just loaded in is still mid-spawn: it plays its spawn animation, allocates animation buffers and
//links itself to the floor it stands on over its first frames, and the other machine's copy of it does all of that on
//different frames. The start-state handshake (RollbackStartSync) can copy values but not that shape, so let it settle.
static const uint32_t ROLLBACK_SETTLE_FRAMES = 180;
static uint32_t rollback_settled_frames = 0;

bool rollback_await_init(void* steamMsgs)
{
    if (!Rollback::rollbackEnabled)
    {
        rollback_settled_frames = 0;
        return true;
    }

    //Wait for all the players to be loaded in before we start ggpo
    if (!Game::playerchar_is_loaded())
    {
        rollback_settled_frames = 0;
        return true;
    }

    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            rollback_settled_frames = 0;
            return true;
        }
        PlayerIns* player = (PlayerIns*)player_o.value();

        //some sanity checks
        if (player->chrins.maxHp <= 1 || player->chrins.curHp < 10)
        {
            rollback_settled_frames = 0;
            return true;
        }
        if (player->chrins.playerCtrl == NULL)
        {
            rollback_settled_frames = 0;
            return true;
        }
        if (player->chrins.playerCtrl->chrCtrl.havokChara == NULL)
        {
            rollback_settled_frames = 0;
            return true;
        }
        float x_pos = *(float*)(((uint64_t)player->chrins.playerCtrl->chrCtrl.havokChara) + 0x10);
        if (x_pos == 0.0f)
        {
            rollback_settled_frames = 0;
            return true;
        }
        if (player->chrins.playerCtrl->chrCtrl.animationMediator == NULL)
        {
            rollback_settled_frames = 0;
            return true;
        }
    }

    if (rollback_settled_frames < ROLLBACK_SETTLE_FRAMES)
    {
        rollback_settled_frames++;
        return true;
    }
    rollback_settled_frames = 0;

#ifdef GGPO_SYNCTEST
    //How many frames synctest runs ahead before loading the last verified frame and re-simulating them.
    //At most GGPO_MAX_PREDICTION_FRAMES: the sync layer only keeps that many saved frames (+2).
    int check_distance = 1;
    char check_distance_env[16];
    if (GetEnvironmentVariableA("DSR_SYNCTEST_DISTANCE", check_distance_env, sizeof(check_distance_env)) > 0)
    {
        check_distance = atoi(check_distance_env);
        if (check_distance < 1) check_distance = 1;
        if (check_distance > GGPO_MAX_PREDICTION_FRAMES) check_distance = GGPO_MAX_PREDICTION_FRAMES;
    }
    ConsoleWrite("GGPO synctest, check distance %d", check_distance);
    GGPOErrorCode result = ggpo_start_synctest(&Rollback::ggpo, &Rollback::ggpoCallbacks, (char*)"DSR_GGPO", Rollback::ggpoCurrentPlayerCount, sizeof(RollbackInput), check_distance);
#else
    //Start ggpo
    GGPOErrorCode result = ggpo_start_session(&Rollback::ggpo, &Rollback::ggpoCallbacks, (ISteamNetworkingMessages*)steamMsgs, "DSR_GGPO", Rollback::ggpoCurrentPlayerCount, sizeof(RollbackInput));
#endif
    if (!GGPO_SUCCEEDED(result))
    {
        FATALERROR("unable to start ggpo. %d", result);
    }

    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        GGPOPlayer ggpoplayer = {};
        ggpoplayer.size = sizeof(GGPOPlayer);
        if (i > 0)
        {
            auto player_o = Game::get_connected_player(i);
            if (!player_o.has_value() || player_o.value() == NULL)
            {
                FATALERROR("Unable to get playerins for guest in rollback_start_session");
            }
            PlayerIns* player = (PlayerIns*)player_o.value();
            uint64_t steamid = player->steamPlayerData->steamOnlineIDData->steam_id;

            ConsoleWrite("GGPO connecting to guest %llx", steamid);

            ggpoplayer.type = GGPO_PLAYERTYPE_REMOTE;
            ggpoplayer.u.remote.steamid.SetSteamID(steamid);
        }
        else
        {
            ggpoplayer.type = GGPO_PLAYERTYPE_LOCAL;
        }
        ggpoplayer.player_num = i + 1;
        result = ggpo_add_player(Rollback::ggpo, &ggpoplayer, &Rollback::ggpoHandles[i]);
        if (!GGPO_SUCCEEDED(result))
        {
            FATALERROR("unable to ggpo_add_player. %d", result);
        }
    }

    //ggpo_set_frame_delay(ggpo, Rollback::ggpoHandles[0], 1);

    //the camera stops going into the local PadManipulator now (ChrCam_ApplyToPadManipulator_injection); start from where it is
    auto local_o = Game::get_connected_player(0);
    if (local_o.has_value() && local_o.value() != 0)
    {
        const ChrManipulator& cm = ((PlayerIns*)local_o.value())->chrins.padManipulator->chrManipulator;
        rollback_live_camera[0] = cm.camera_y_rotation;
        rollback_live_camera[1] = cm.camera_x_rotation;
    }

    ConsoleWrite("GGPO started");
    Rollback::ggpoStarted = true;
    WorldFreeze::freeze();

    RollbackStartSync::begin_session();
    RollbackReplay::init_session();
    RollbackScript::init_session();
    RollbackHash::reset_session();

#ifdef GGPO_SYNCTEST
    //DSR_SYNCTEST_RESERVE_DAMAGE_POOL=1 empties DamageMan's free list for the session, so DamageMan_PopHead_DamageEntry heap-allocates
    //every new entry and synctest exercises saving and loading heap entries. The detached pool slots stay out of both lists.
    char reserve_pool_env[8];
    if (GetEnvironmentVariableA("DSR_SYNCTEST_RESERVE_DAMAGE_POOL", reserve_pool_env, sizeof(reserve_pool_env)) > 0 && reserve_pool_env[0] == '1')
    {
        DamageMan* damageman = *(DamageMan**)Game::damage_man;
        damageman->all_damage_entries_list_cur = NULL;
        ConsoleWrite("GGPO synctest: DamageMan pool reserved, new damage entries are heap-allocated");
    }
#endif

    return false;
}

void Rollback::rollback_start_session(ISteamNetworkingMessages* steamMsgs)
{
    if (Rollback::rollbackEnabled)
    {
        MainLoop::setup_mainloop_callback(rollback_await_init, steamMsgs, "rollback_await_init");
    }
}
