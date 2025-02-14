#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "InputUtil.h"
#include "ModNetworking.h"
#include <string>
#include <unordered_map>

PlayerIns* Rollback::saved_playerins = NULL;
BulletMan* Rollback::saved_bulletman = NULL;
FXManager* Rollback::saved_sfxobjs = NULL;
DamageMan* Rollback::saved_damageman = NULL;
ThrowMan* Rollback::saved_throwman = NULL;
DmgHitRecordManImp* Rollback::saved_DmgHitRecordMan = NULL;
PadManipulatorPacked** Rollback::saved_PadManipulator = NULL;

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

bool Rollback::gsave = false;
bool Rollback::gload = false;
bool state_test(void* unused)
{
    if (Rollback::gsave)
    {
        auto player_o = Game::get_PlayerIns();
        PlayerIns* player = (PlayerIns*)player_o.value();
        copy_PlayerIns(Rollback::saved_playerins, player, false);
        copy_BulletMan(Rollback::saved_bulletman, *(BulletMan**)Game::bullet_man, false);
        //copy_FXManager(Rollback::saved_sfxobjs, (*(SfxMan**)Game::sfx_man)->FrpgFxManagerBase->base.fXManager, false);
        copy_DamageMan(Rollback::saved_damageman, *(DamageMan**)Game::damage_man, false);
        copy_ThrowMan(Rollback::saved_throwman, *(ThrowMan**)Game::throw_man, false);
        copy_DmgHitRecordManImp(Rollback::saved_DmgHitRecordMan, *(DmgHitRecordManImp**)Game::dmg_hit_record_man, false);

        Rollback::gsave = false;
    }

    if (Rollback::gload)
    {
        auto player_o = Game::get_PlayerIns();
        PlayerIns* player = (PlayerIns*)player_o.value();
        copy_PlayerIns(player, Rollback::saved_playerins, true);
        copy_BulletMan(*(BulletMan**)Game::bullet_man, Rollback::saved_bulletman, true);
        //copy_FXManager((*(SfxMan**)Game::sfx_man)->FrpgFxManagerBase->base.fXManager, Rollback::saved_sfxobjs, true);
        copy_DamageMan(*(DamageMan**)Game::damage_man, Rollback::saved_damageman, true);
        copy_ThrowMan(*(ThrowMan**)Game::throw_man, Rollback::saved_throwman, true);
        copy_DmgHitRecordManImp(*(DmgHitRecordManImp**)Game::dmg_hit_record_man, Rollback::saved_DmgHitRecordMan, true);

        Game::Step_GameSimulation();

        Rollback::gload = false;
    }

    return true;
}

bool Rollback::isave = false;
bool Rollback::iload = false;
static uint32_t inputSaveFrameI = 0;
static const size_t INPUT_ROLLBACK_LENGTH = 5 * 60; //5 seconds
bool input_test(void* unused)
{
    if (Rollback::isave)
    {
        auto player_o = Game::get_PlayerIns();
        PlayerIns* player = (PlayerIns*)player_o.value();
        PadManipulator_to_PadManipulatorPacked(Rollback::saved_PadManipulator[inputSaveFrameI], player->chrins.padManipulator);

        inputSaveFrameI++;
        if (inputSaveFrameI >= INPUT_ROLLBACK_LENGTH)
        {
            ConsoleWrite("Input save finish");
            Rollback::isave = false;
            inputSaveFrameI = 0;
        }
    }

    if (Rollback::iload)
    {
        Game::set_ReadInputs_allowed(false);

        auto player_o = Game::get_PlayerIns();
        PlayerIns* player = (PlayerIns*)player_o.value();
        PadManipulatorPacked_to_PadManipulator(player, Rollback::saved_PadManipulator[inputSaveFrameI], true);

        inputSaveFrameI++;
        if (inputSaveFrameI >= INPUT_ROLLBACK_LENGTH)
        {
            Game::set_ReadInputs_allowed(true);
            ConsoleWrite("Input restore finish");
            Rollback::iload = false;
            inputSaveFrameI = 0;
        }
    }

    return true;
}

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
    int32_t ItemIdOverride = -1;
    uint64_t get_item_currently_being_used_return;
    void get_item_currently_being_used_injection();
    uint8_t get_item_currently_being_used_injection_helper(EquipGameData*, ItemUsed*);
}

//Return 1 if we have an override itemid we want to use
uint8_t get_item_currently_being_used_injection_helper(EquipGameData* equip, ItemUsed* out)
{
    if (Rollback::rollbackEnabled && ItemIdOverride != -1)
    {
        out->itemId = ItemIdOverride;
        out->amountUsed = 1;
        return 1;
    }
    return 0;
}

void PackRollbackInput(RollbackInput* out, PlayerIns* player)
{
    EquipInventoryDataItem* itemlist = player->playergamedata->equipGameData.equippedInventory.itemlist2;
    uint32_t itemlistlen = player->playergamedata->equipGameData.equippedInventory.itemList2_len;

    PadManipulator_to_PadManipulatorPacked(&out->padmanipulator, player->chrins.padManipulator);
    //need to tell if GGPO has actually returned us a real input, or padding
    //TODO improve this
    out->const1 = 1;

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
    out->curSelectedQuickbarItemId = itemlist[player->playergamedata->equipGameData.equippedItemsInQuickbar.selectedQuickbarItem].item_id;

    out->curSelectedMagicSlot = player->playergamedata->equipGameData.equipMagicData->curSelectedMagicSlot;

    //this doesn't actually control the item being used, but just what item will be used when the use button is pressed
    out->curUsingInventoryItemId = -1;
    if (player->playergamedata->equipGameData.itemInventoryIdCurrentlyBeingUsedFromInventory != -1)
    {
        out->curUsingInventoryItemId = itemlist[player->playergamedata->equipGameData.itemInventoryIdCurrentlyBeingUsedFromInventory].item_id;
    }

    for (size_t i = 0; i < InventorySlots::END; i++)
    {
        out->equipment_array[i] = Game::get_equipped_inventory((uint64_t)player, (InventorySlots)i);
    }
}

void UnpackRollbackInput(RollbackInput* in, PlayerIns* player)
{
    EquipInventoryDataItem* itemlist = player->playergamedata->equipGameData.equippedInventory.itemlist2;
    uint32_t itemlistlen = player->playergamedata->equipGameData.equippedInventory.itemList2_len;

    //Replicate the code from ChrAsm_Set_Equipped_Items
    //This only updates the chrasm equip items, since the game will dynamically update the chr elsewhere based on this
    for (uint32_t equip_index = 0; equip_index < InventorySlots::END; equip_index++)
    {
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
                if (player->playergamedata->equipGameData.chrasm.equip_items[equip_index] != in->equipment_array[equip_index])
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
        ItemIdOverride = in->curUsingInventoryItemId;
    }
    //fall back to the quickbar if we don't have an inventory item
    else
    {
        ItemIdOverride = in->curSelectedQuickbarItemId;
    }

    uint32_t playerHandle = *(uint32_t*)(((uint64_t)player) + 8);
    if (playerHandle > Game::PC_Handle && playerHandle < Game::PC_Handle + 10)
    {
        PadManipulatorPacked_to_PadManipulator(player, &in->padmanipulator, true);

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
        PadManipulatorPacked_to_PadManipulator(player, &in->padmanipulator, false);

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
}

void rollback_sync_inputs()
{
    RollbackInput inputs[GGPO_MAX_PLAYERS];
    int disconnect_flags; //TODO

    //get the inputs for this frame
    GGPOErrorCode res = ggpo_synchronize_input(Rollback::ggpo, inputs, sizeof(RollbackInput) * GGPO_MAX_PLAYERS, &disconnect_flags);
    if (!GGPO_SUCCEEDED(res))
    {
        FATALERROR("ggpo_synchronize_input call returned %d", res);
    }

    //load the input states into the game to be used this frame
    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            FATALERROR("Unable to get playerins in rollback_load_game_state_callback");
        }
        PlayerIns* player = (PlayerIns*)player_o.value();
        //When ggpo doesn't have the input during it's prediction stage, it just defaults to a controller with nothing pressed (no change from last input)
        //We handle that by just not loading it at all, so the PadManipulator stays unchanged from the rollback frame it was saved from
        if (inputs[i].const1 == 1)
        {
            UnpackRollbackInput(&inputs[i], player);
        }
        else
        {
            ConsoleWrite("sync_input returned an empty input, ignoring");
        }
    }
}

bool rollback_game_frame_start_helper(void* unused)
{
    if (Rollback::rollbackEnabled && Rollback::ggpoStarted)
    {
        //this is where the game state is loaded/a rollback is triggered
        ggpo_idle(Rollback::ggpo, 0); //timeout isn't actually used by this function

        if (Rollback::ggpoReady == GGPOREADY::ReadyAwaitingFrameHead)
        {
            Rollback::ggpoReady = GGPOREADY::Ready;
        }

        if (Rollback::ggpoReady == GGPOREADY::Ready)
        {
            auto player_o = Game::get_PlayerIns();
            if (!player_o.has_value() || player_o.value() == NULL)
            {
                FATALERROR("Unable to get playerins for PC in rollback_game_frame_start_helper");
            }
            PlayerIns* player = (PlayerIns*)player_o.value();

            //Manually call the PadMan and PadManipulator functions to read the inputs
            //ONLY allow it to be called here, so we don't have it called normally by the game and overwrite our custom inputs
            Game::set_ReadInputs_allowed(true);
            //This function is called as part of MainUpdate, and needs to be called first before Step_PadManipulator.
            //It reads the controller directly and normalizes it post-keybinds. All stored in the PadMan global
            Step_PadMan(FRAMETIME);
            //This function is called as part of the Step_TaskMan list, so we need to directly call it here and block TaskMan from calling it.
            //It reads the normalized controller, post processes inputs based on game state (camera, weapons, etc), and sets a standardized struct
            //This reads from the PadMan global and puts it's results in the player PadManipulator and player PlayerCtrl structs
            //These 2 structs are what we actually use for rollback input. PadMan itself is overly complex and too raw.
            Step_PadManipulator(player->chrins.padManipulator, FRAMETIME, player->chrins.playerCtrl);
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
            //TODO********! probably wanna get stuff from playerctrl in here also
            RollbackInput localInput{};
            PackRollbackInput(&localInput, player);

            //notify ggpo of the local player's inputs
            GGPOErrorCode result = ggpo_add_local_input(Rollback::ggpo, Rollback::ggpoHandles[0], &localInput, sizeof(RollbackInput));
            if (!GGPO_SUCCEEDED(result))
            {
                FATALERROR("Unable to ggpo_add_local_input. %d", result);
            }

            rollback_sync_inputs();
        }
    }

    if (Rollback::networkTest)
    {
        auto player_o = Game::get_PlayerIns();
        if (player_o.has_value() && player_o.value() != NULL)
        {
            PlayerIns* player = (PlayerIns*)player_o.value();

            auto guest_o = Game::get_connected_player(1);
            if (guest_o.has_value() && guest_o.value() != NULL)
            {
                PlayerIns* guest = (PlayerIns*)guest_o.value();

                //only allow 1 player to be the controller, otherwise we can get a feedback loop
                //first ring slot == old witch's ring
                if (player->chrasm->equip_items[0xD] == 137)
                {
                    Game::set_ReadInputs_allowed(true);
                    Step_PadMan(FRAMETIME);
                    Step_PadManipulator(player->chrins.padManipulator, FRAMETIME, player->chrins.playerCtrl);
                    Game::set_ReadInputs_allowed(false);
                    Game::set_StepInGameMenu_allowed(true);
                    uint64_t ingamestep = (uint64_t)Game::get_InGameStep();
                    uint64_t taskitem = *(uint64_t*)(ingamestep + 0x5ae0);
                    uint64_t ingamemenustep = *(uint64_t*)(taskitem + 0x20);
                    Step_InGameMenus((void*)ingamemenustep, FRAMETIME, (void*)taskitem);
                    Game::set_StepInGameMenu_allowed(false);

                    //send out our input
                    RollbackInput localInput{};
                    PackRollbackInput(&localInput, player);
                    SteamNetworkingIdentity target{};
                    target.SetSteamID(guest->steamPlayerData->steamOnlineIDData->steam_id);
                    ModNetworking::SteamNetMessages->SendMessageToUser(target, &localInput, sizeof(localInput), k_nSteamNetworkingSend_UnreliableNoNagle, 1);
                }
                else
                {
                    Game::set_ReadInputs_allowed(false);
                    Game::set_StepInGameMenu_allowed(false);
                }

                //read in and set the other player input. Do this in lockstep
                SteamNetworkingMessage_t* new_message;
                int num_messages = 0;
                int i = 0;
                do
                {
                    num_messages = ModNetworking::SteamNetMessages->ReceiveMessagesOnChannel(1, &new_message, 1);
                    i++;
                } while (num_messages < 1 && i < 5);
                if (num_messages == 1)
                {
                    RollbackInput* remoteInput = (RollbackInput*)new_message->GetData();
                    PadManipulatorPacked_to_PadManipulator(guest, &remoteInput->padmanipulator, true);
                    PadManipulatorPacked_to_PadManipulator(player, &remoteInput->padmanipulator, true); //need to treat as networked pc since they are not doing any input processing
                    new_message->Release();
                }
                else
                {
                    //ConsoleWrite("Missed input");
                }
            }
        }
    }

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
    if (Rollback::rollbackEnabled && Rollback::ggpoStarted)
    {
        //only start telling ggpo we're running once the players are synced
        if (Rollback::ggpoReady == GGPOREADY::Ready)
        {
            //this is where the game state is actually saved (and also loaded if we are in SyncTest)
            ggpo_advance_frame(Rollback::ggpo);

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
void followupBullet_loop_helper(uint64_t FXEntry_Substruct_2)
{
    uint64_t followupBullet = *(uint64_t*)(FXEntry_Substruct_2 + 0x20);
    uint64_t FXEntry_Substruct = *(uint64_t*)(FXEntry_Substruct_2 + 0x8);
    uint64_t* fxentry_a = (uint64_t*)(followupBullet + 0x10);
    *fxentry_a = FXEntry_Substruct;
}

bool rollback_await_init(void* steamMsgs);

void Rollback::start()
{
    ConsoleWrite("Rollback...");
    uint8_t* write_address;

    SetEnvironmentVariable("ggpo.log", "1");

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

    //Testing rollback related stuff
    Rollback::saved_playerins = init_PlayerIns();
    Rollback::saved_bulletman = init_BulletMan();
    Rollback::saved_sfxobjs = init_FXManager();
    Rollback::saved_damageman = init_DamageMan();
    Rollback::saved_throwman = init_ThrowMan();
    Rollback::saved_DmgHitRecordMan = init_DmgHitRecordManImp();
    Rollback::saved_PadManipulator = (PadManipulatorPacked**)malloc_(sizeof(PadManipulatorPacked*) * INPUT_ROLLBACK_LENGTH);
    for (size_t i = 0; i < INPUT_ROLLBACK_LENGTH; i++)
    {
        Rollback::saved_PadManipulator[i] = (PadManipulatorPacked*)malloc(sizeof(PadManipulatorPacked));
    }
    //Testing save/restore with a hotkey
    MainLoop::setup_mainloop_callback(state_test, NULL, "state_test");
    MainLoop::setup_mainloop_callback(input_test, NULL, "input_test");
    MainLoop::setup_mainloop_callback(ggpo_toggle, NULL, "ggpo_toggle");
    MainLoop::setup_mainloop_callback(network_toggle, NULL, "network_toggle");

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
    rollback_sync_inputs();

    //step next frame
    Game::Step_GameSimulation();
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

    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            FATALERROR("Unable to get playerins %d in rollback_load_game_state_callback", i);
        }
        PlayerIns* player = (PlayerIns*)player_o.value();

        copy_PlayerIns(player, state->playerins[i], true);
    }

    copy_BulletMan(*(BulletMan**)Game::bullet_man, state->bulletman, true);
    //copy_SfxMan(*(SfxMan**)Game::sfx_man, state->sfxman, true);
    copy_DamageMan(*(DamageMan**)Game::damage_man, state->damageman, true);
    copy_ThrowMan(*(ThrowMan**)Game::throw_man, state->throwman, true);
    copy_DmgHitRecordManImp(*(DmgHitRecordManImp**)Game::dmg_hit_record_man, state->dmghitrecordman, true);

    if (Rollback::rollbackVisual)
    {
        *(float*)((uint64_t)Game::get_PlayerIns().value() + 0x328) = 0.4f;
    }
    //ConsoleWrite("rollback_load_game_state_callback finish");

    return true;
}

/*
 * Save the current state to a buffer and return it to GGPO via the
 * buffer and len parameters.
 */
bool rollback_save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int)
{
    RollbackState* state = (RollbackState*)malloc(sizeof(RollbackState));
    if (state == NULL)
    {
        FATALERROR("Unable to get allocate state for rollback_save_game_state_callback");
    }

    //The checksum is only used when we run the GGPO synctest, disable otherwise
    size_t our_checksum = 0;

    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            FATALERROR("Unable to get playerins %d in rollback_save_game_state_callback", i);
        }
        PlayerIns* player = (PlayerIns*)player_o.value();

        state->playerins[i] = init_PlayerIns();
        copy_PlayerIns(state->playerins[i], player, false);
#if defined(GGPO_SYNCTEST) && 0
        our_checksum ^= std::hash<std::string>{}(print_PlayerIns(player));
#endif
    }

    state->bulletman = init_BulletMan();
    copy_BulletMan(state->bulletman, *(BulletMan**)Game::bullet_man, false);
    //state->sfxman = init_SfxMan();
    //copy_SfxMan(state->sfxman, *(SfxMan**)Game::sfx_man, false);
    state->damageman = init_DamageMan();
    copy_DamageMan(state->damageman, *(DamageMan**)Game::damage_man, false);
    state->throwman = init_ThrowMan();
    copy_ThrowMan(state->throwman, *(ThrowMan**)Game::throw_man, false);
    state->dmghitrecordman = init_DmgHitRecordManImp();
    copy_DmgHitRecordManImp(state->dmghitrecordman, *(DmgHitRecordManImp**)Game::dmg_hit_record_man, false);
#if defined(GGPO_SYNCTEST) && 0
    our_checksum ^= std::hash<std::string>{}(print_BulletMan(*(BulletMan**)Game::bullet_man));
#endif

    *buffer = (unsigned char*)state;
    *len = sizeof(RollbackState);
    *checksum = (int)our_checksum;

    return true;
}

void rollback_copy_buffer(void* buffer_dst, void* buffer_src)
{
    RollbackState* state_src = (RollbackState*)buffer_src;
    RollbackState* state_dst = (RollbackState*)buffer_dst;

    for (size_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        state_dst->playerins[i] = init_PlayerIns();
        copy_PlayerIns(state_dst->playerins[i], state_src->playerins[i], false);
    }
    state_dst->bulletman = init_BulletMan();
    copy_BulletMan(state_dst->bulletman, state_src->bulletman, false);
    //state_dst->sfxman = init_SfxMan();
    //copy_SfxMan(state_dst->sfxman, state_src->sfxman, false);
    state_dst->damageman = init_DamageMan();
    copy_DamageMan(state_dst->damageman, state_src->damageman, false);
    state_dst->throwman = init_ThrowMan();
    copy_ThrowMan(state_dst->throwman, state_src->throwman, false);
    state_dst->dmghitrecordman = init_DmgHitRecordManImp();
    copy_DmgHitRecordManImp(state_dst->dmghitrecordman, state_src->dmghitrecordman, false);
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
    //state->sfxman = NULL;
    free_DamageMan(state->damageman);
    state->damageman = NULL;
    free_ThrowMan(state->throwman);
    state->throwman = NULL;
    free_DmgHitRecordManImp(state->dmghitrecordman);
    state->dmghitrecordman = NULL;

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

bool rollback_log_game_state(char* filename, unsigned char* buffer, int)
{
    FILE* fp = nullptr;
    fopen_s(&fp, filename, "w");
    if (!fp)
    {
        return true;
    }

    RollbackState* state = (RollbackState*)buffer;

    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        std::string player = print_PlayerIns(state->playerins[i]);
        fprintf(fp, "Player %d\n%s", i, player.c_str());
    }
    std::string bulletman = print_BulletMan(state->bulletman);
    fprintf(fp, "%s", bulletman.c_str());
    //std::string sfxman = print_SfxMan(state->sfxman);
    //fprintf(fp, "%s", sfxman.c_str());
    //std::string damage = print_DamageMan(state->damageman);
    //fprintf(fp, "%s", damage.c_str());
    //std::string throwman = print_ThrowMan(state->throwman);
    //fprintf(fp, "%s", throwman.c_str());

    fclose(fp);
    return true;
}

void Rollback::rollback_end_session()
{
    if (Rollback::ggpoStarted)
    {
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

bool rollback_await_init(void* steamMsgs)
{
    if (!Rollback::rollbackEnabled)
    {
        return true;
    }

    //Wait for all the players to be loaded in before we start ggpo
    if (!Game::playerchar_is_loaded())
    {
        return true;
    }

    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            return true;
        }
        PlayerIns* player = (PlayerIns*)player_o.value();

        //some sanity checks
        if (player->chrins.maxHp <= 1 || player->chrins.curHp < 10)
        {
            return true;
        }
        if (player->chrins.playerCtrl == NULL)
        {
            return true;
        }
        if (player->chrins.playerCtrl->chrCtrl.havokChara == NULL)
        {
            return true;
        }
        float x_pos = *(float*)(((uint64_t)player->chrins.playerCtrl->chrCtrl.havokChara) + 0x10);
        if (x_pos == 0.0f)
        {
            return true;
        }
        if (player->chrins.playerCtrl->chrCtrl.animationMediator == NULL)
        {
            return true;
        }
    }

#ifdef GGPO_SYNCTEST
    GGPOErrorCode result = ggpo_start_synctest(&Rollback::ggpo, &Rollback::ggpoCallbacks, (char*)"DSR_GGPO", Rollback::ggpoCurrentPlayerCount, sizeof(RollbackInput), 1);
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

    ConsoleWrite("GGPO started");
    Rollback::ggpoStarted = true;

    return false;
}

void Rollback::rollback_start_session(ISteamNetworkingMessages* steamMsgs)
{
    if (Rollback::rollbackEnabled)
    {
        MainLoop::setup_mainloop_callback(rollback_await_init, steamMsgs, "rollback_await_init");
    }
}
