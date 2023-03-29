#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "InputUtil.h"
#include "ModNetworking.h"

PlayerIns* Rollback::saved_playerins = NULL;
BulletMan* Rollback::saved_bulletman = NULL;
FXManager* Rollback::saved_sfxobjs = NULL;
DamageMan* Rollback::saved_damageman = NULL;
PadManipulator** Rollback::saved_PadManipulator = NULL;

GGPOSession* Rollback::ggpo = NULL;
GGPOPlayerHandle Rollback::ggpoHandles[GGPO_MAX_PLAYERS] = {};
bool Rollback::ggpoStarted = false;
GGPOREADY Rollback::ggpoReady = GGPOREADY::NotReady;

GGPOSessionCallbacks Rollback::ggpoCallbacks = {
    .begin_game = rollback_begin_game_callback,
    .save_game_state = rollback_save_game_state_callback,
    .load_game_state = rollback_load_game_state_callback,
    .log_game_state = rollback_log_game_state,
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
        copy_PadManipulator(Rollback::saved_PadManipulator[inputSaveFrameI], player->chrins.padManipulator);

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
        copy_PadManipulator(player->chrins.padManipulator, Rollback::saved_PadManipulator[inputSaveFrameI]);

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
        if (inputs[i].padmanipulator.const1 == 1)
        {
            PadManipulatorPacked_to_PadManipulator(player->chrins.padManipulator, &inputs[i].padmanipulator);
        }
        else
        {
            ConsoleWrite("sync_input returned an empty input list, ignoring");
        }
    }
}

extern "C" {
    uint64_t rollback_game_frame_sync_inputs_return;
    void rollback_game_frame_sync_inputs_injection();
    void rollback_game_frame_sync_inputs_helper();
}

void rollback_game_frame_sync_inputs_helper()
{
    if (Rollback::rollbackEnabled && Rollback::ggpoStarted)
    {
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
                FATALERROR("Unable to get playerins for PC in rollback_game_frame_sync_inputs_helper");
            }
            PlayerIns* player = (PlayerIns*)player_o.value();

            //Manually call the PadManipulator function to read the inputs
            //ONLY allow it to be called here, so we don't have it called normally by the game and overwrite our custom inputs
            Game::set_ReadInputs_allowed(true);
            Step_PadManipulator(player->chrins.padManipulator, FRAMETIME, player->chrins.playerCtrl);
            Game::set_ReadInputs_allowed(false);

            //read local inputs
            RollbackInput localInput{};
            PadManipulator_to_PadManipulatorPacked(&localInput.padmanipulator, player->chrins.padManipulator);
            localInput.padmanipulator.const1 = 1;

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
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            FATALERROR("Unable to get playerins for PC in rollback_game_frame_sync_inputs_helper");
        }
        PlayerIns* player = (PlayerIns*)player_o.value();
        auto guest_o = Game::get_connected_player(1);
        if (!guest_o.has_value() || guest_o.value() == NULL)
        {
            FATALERROR("Unable to get guest in rollback_load_game_state_callback");
        }
        PlayerIns* guest = (PlayerIns*)guest_o.value();

        Game::set_ReadInputs_allowed(true);
        Step_PadManipulator(player->chrins.padManipulator, FRAMETIME, player->chrins.playerCtrl);
        Game::set_ReadInputs_allowed(false);

        //send out our input
        RollbackInput localInput{};
        PadManipulator_to_PadManipulatorPacked(&localInput.padmanipulator, player->chrins.padManipulator);
        SteamNetworkingIdentity target;
        target.SetSteamID(guest->steamPlayerData->steamOnlineIDData->steam_id);
        ModNetworking::SteamNetMessages->SendMessageToUser(target, &localInput, sizeof(localInput), k_nSteamNetworkingSend_UnreliableNoDelay, 1);

        //read in and set the other player input
        SteamNetworkingMessage_t* new_message;
        int num_messages = ModNetworking::SteamNetMessages->ReceiveMessagesOnChannel(1, &new_message, 1);
        if (num_messages == 1)
        {
            RollbackInput* remoteInput = (RollbackInput*)new_message->GetData();
            PadManipulatorPacked_to_PadManipulator(guest->chrins.padManipulator, &remoteInput->padmanipulator);
            new_message->Release();
        }
    }
}

extern "C" {
    uint64_t dsr_frame_finished_return;
    void dsr_frame_finished_injection();
    void dsr_frame_finished_helper();
}

void dsr_frame_finished_helper()
{
    if (Rollback::rollbackEnabled && Rollback::ggpoStarted)
    {
        //only start telling ggpo we're running once the players are synced
        if (Rollback::ggpoReady == GGPOREADY::Ready)
        {
            ggpo_advance_frame(Rollback::ggpo);

            if (Rollback::rollbackVisual)
            {
                float* visability = (float*)((uint64_t)Game::get_PlayerIns().value() + 0x328);
                if (*visability < 1.0f)
                {
                    *visability += 0.2f;
                }
            }
        }
    }
}

void Rollback::start()
{
    ConsoleWrite("Rollback...");
    uint8_t* write_address;

    SetEnvironmentVariable("ggpo.log", "1");

    Rollback::NetcodeFix();

    //Synchronize input at the start of each frame
    //do this anytime after the game reads the inputs, but before MoveMapStep_Step_13
    write_address = (uint8_t*)(Rollback::rollback_game_frame_sync_inputs_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &rollback_game_frame_sync_inputs_return, 4, &rollback_game_frame_sync_inputs_injection);

    //Inform ggpo after a frame has been rendered
    write_address = (uint8_t*)(Rollback::MainUpdate_end_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &dsr_frame_finished_return, 0, &dsr_frame_finished_injection);

    //Testing rollback related stuff
    Rollback::saved_playerins = init_PlayerIns();
    Rollback::saved_bulletman = init_BulletMan();
    Rollback::saved_sfxobjs = init_FXManager();
    Rollback::saved_damageman = init_DamageMan();
    Rollback::saved_PadManipulator = (PadManipulator**)malloc_(sizeof(PadManipulator*) * INPUT_ROLLBACK_LENGTH);
    for (size_t i = 0; i < INPUT_ROLLBACK_LENGTH; i++)
    {
        Rollback::saved_PadManipulator[i] = (PadManipulator*)malloc(sizeof(PadManipulator));
    }
    //Testing save/restore with a hotkey
    MainLoop::setup_mainloop_callback(state_test, NULL, "state_test");
    MainLoop::setup_mainloop_callback(input_test, NULL, "input_test");
    MainLoop::setup_mainloop_callback(ggpo_toggle, NULL, "ggpo_toggle");
    MainLoop::setup_mainloop_callback(network_toggle, NULL, "network_toggle");
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
    Game::Step_GameSimulation(true);
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
    //TODO copy_FXManager
    copy_DamageMan(*(DamageMan**)Game::damage_man, state->damageman, true);

    if (Rollback::rollbackVisual)
    {
        *(float*)((uint64_t)Game::get_PlayerIns().value() + 0x328) = 0.0f;
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
    }

    state->bulletman = init_BulletMan();
    copy_BulletMan(state->bulletman, *(BulletMan**)Game::bullet_man, false);
    //TODO copy_FXManager;
    state->damageman = init_DamageMan();
    copy_DamageMan(state->damageman, *(DamageMan**)Game::damage_man, false);

    *buffer = (unsigned char*)state;
    *len = sizeof(RollbackState);
    *checksum = 0;

    return true;
}

void rollback_free_buffer(void* buffer)
{
    RollbackState* state = (RollbackState*)buffer;

    for (size_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        free_PlayerIns(state->playerins[i]);
        state->playerins[i] = NULL;
    }
    //TODO free_FXManager
    free_BulletMan(state->bulletman);
    state->bulletman = NULL;
    free_DamageMan(state->damageman);
    state->damageman = NULL;

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
        Sleep(1000 * info->u.timesync.frames_ahead / 60);
        break;
    }
    return true;
}

bool rollback_log_game_state(char* filename, unsigned char* buffer, int)
{
    return true;
}

static size_t timer = 0;

bool rollback_await_player_added_before_init(void* steamMsgs)
{
    if (!Game::playerchar_is_loaded())
    {
        return true;
    }

    //TODO wait for the players to be fully loaded
    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            return true;
        }
    }

    if (timer * 16.6f < 15 * 1000)
    {
        timer++;
        return true;
    }
    timer = 0;

    Rollback::rollback_start_session((ISteamNetworkingMessages*)steamMsgs);
    return false;
}

void Rollback::rollback_start_session(ISteamNetworkingMessages* steamMsgs)
{
    GGPOErrorCode result;

    if (Rollback::rollbackEnabled)
    {
        //TODO move this to when session ends
        if (Rollback::ggpo != NULL)
        {
            Rollback::ggpoStarted = false;
            Rollback::ggpoReady = GGPOREADY::NotReady;
            result = ggpo_close_session(Rollback::ggpo);
            Rollback::ggpo = NULL;
            if (!GGPO_SUCCEEDED(result))
            {
                FATALERROR("unable to close ggpo. %d", result);
            }
        }

        result = ggpo_start_session(&Rollback::ggpo, &Rollback::ggpoCallbacks, steamMsgs, "DSR_GGPO", Rollback::ggpoCurrentPlayerCount, sizeof(RollbackInput));
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
            ggpoplayer.player_num = i+1;
            result = ggpo_add_player(ggpo, &ggpoplayer, &Rollback::ggpoHandles[i]);
            if (!GGPO_SUCCEEDED(result))
            {
                FATALERROR("unable to ggpo_add_player. %d", result);
            }
        }

        ggpo_set_frame_delay(ggpo, Rollback::ggpoHandles[0], 1);

        ConsoleWrite("GGPO started");
        Rollback::ggpoStarted = true;
    }
}
