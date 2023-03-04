#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "InputUtil.h"

PlayerIns* Rollback::saved_playerins = NULL;
PadMan** Rollback::saved_padman = NULL;
QInputMgrWindows** Rollback::saved_qinputman = NULL;
InputDirectionMovementMan** Rollback::saved_InputDirectionMovementMan = NULL;
BulletMan* Rollback::saved_bulletman = NULL;
FXManager* Rollback::saved_sfxobjs = NULL;
DamageMan* Rollback::saved_damageman = NULL;

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
    .free_input = rollback_on_free_input,
    .compare_inputs = rollback_on_compare_inputs,
};

bool Rollback::gsave = false;
bool Rollback::gload = false;
bool state_test(void* unused)
{
    if (Rollback::gsave)
    {
        auto player_o = Game::get_PlayerIns();
        PlayerIns* player = (PlayerIns*)player_o.value();
        copy_PlayerIns(Rollback::saved_playerins, player, false, false);
        copy_BulletMan(Rollback::saved_bulletman, *(BulletMan**)Game::bullet_man, false);
        //copy_FXManager(Rollback::saved_sfxobjs, (*(SfxMan**)Game::sfx_man)->FrpgFxManagerBase->base.fXManager, false);
        copy_DamageMan(Rollback::saved_damageman, *(DamageMan**)Game::damage_man, false);

        Rollback::gsave = false;
    }

    if (Rollback::gload)
    {
        auto player_o = Game::get_PlayerIns();
        PlayerIns* player = (PlayerIns*)player_o.value();
        copy_PlayerIns(player, Rollback::saved_playerins, true, false);
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
        copy_PadMan(Rollback::saved_padman[inputSaveFrameI], *(PadMan**)Game::pad_man);
        copy_QInputMgrWindows(Rollback::saved_qinputman[inputSaveFrameI], *(QInputMgrWindows**)Game::QInputMgrWindowsFantasy);
        copy_InputDirectionMovementMan(Rollback::saved_InputDirectionMovementMan[inputSaveFrameI], *(InputDirectionMovementMan**)Game::InputDirectionMovementMan);

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

        copy_PadMan(*(PadMan**)Game::pad_man, Rollback::saved_padman[inputSaveFrameI]);
        copy_QInputMgrWindows(*(QInputMgrWindows**)Game::QInputMgrWindowsFantasy, Rollback::saved_qinputman[inputSaveFrameI]);
        copy_InputDirectionMovementMan(*(InputDirectionMovementMan**)Game::InputDirectionMovementMan, Rollback::saved_InputDirectionMovementMan[inputSaveFrameI]);

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

bool Rollback::netcodeSwap = false;
bool Rollback::netcodeTestingEnabled = false;
bool netcode_test(void* unused)
{
    if (Rollback::netcodeSwap)
    {
        Rollback::netcodeTestingEnabled = !Rollback::netcodeTestingEnabled;
        ConsoleWrite("netcodeTestingEnabled netcode %d", Rollback::netcodeTestingEnabled);
        Rollback::netcodeSwap = false;
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
    //Make sure to use the values returned rather than the values you've read from the local controllers
    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (!player_o.has_value() || player_o.value() == NULL)
        {
            FATALERROR("Unable to get playerins in rollback_load_game_state_callback");
        }
        PlayerIns* player = (PlayerIns*)player_o.value();

        //first player is always the local player, since we add them first in setup
        if (i == 0)
        {
            if (inputs[i].local.padman == NULL ||
                inputs[i].local.qInputMgrWindows == NULL ||
                inputs[i].local.inputDirectionMovementMan == NULL)
            {
                FATALERROR("PC local input returned null for ggpo_synchronize_input");
            }
            copy_PadMan(*(PadMan**)Game::pad_man, inputs[i].local.padman);
            copy_QInputMgrWindows(*(QInputMgrWindows**)Game::QInputMgrWindowsFantasy, inputs[i].local.qInputMgrWindows);
            copy_InputDirectionMovementMan(*(InputDirectionMovementMan**)Game::InputDirectionMovementMan, inputs[i].local.inputDirectionMovementMan);
        }
        else
        {
            if (inputs[i].remote.maxHp == 0)
            {
                ConsoleWrite("WARNING: Player remote input returned empty for ggpo_synchronize_input. Leave unchanged for this frame");
            }
            else
            {
                Rollback::LoadRemotePlayerPacket(&inputs[i].remote, player, Game::get_SessionPlayerNumber_For_ConnectedPlayerData((uint64_t)(&player->steamPlayerData)));
            }
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
            RollbackInput localInput;

            auto player_o = Game::get_PlayerIns();
            if (!player_o.has_value() || player_o.value() == NULL)
            {
                FATALERROR("Unable to get playerins for PC in rollback_game_frame_sync_inputs_helper");
            }
            PlayerIns* player = (PlayerIns*)player_o.value();

            //read local controller inputs
            localInput.local.padman = init_PadMan();
            copy_PadMan(localInput.local.padman, *(PadMan**)Game::pad_man);
            localInput.local.qInputMgrWindows = init_QInputMgrWindows();
            copy_QInputMgrWindows(localInput.local.qInputMgrWindows, *(QInputMgrWindows**)Game::QInputMgrWindowsFantasy);
            localInput.local.inputDirectionMovementMan = init_InputDirectionMovementMan();
            copy_InputDirectionMovementMan(localInput.local.inputDirectionMovementMan, *(InputDirectionMovementMan**)Game::InputDirectionMovementMan);

            //setup the remote section for the local player
            Rollback::BuildRemotePlayerPacket(player, &localInput.remote);

            /* notify ggpo of the local player's inputs */
            GGPOErrorCode result = ggpo_add_local_input(Rollback::ggpo, Rollback::ggpoHandles[0], &localInput, sizeof(RollbackInput));

            if (!GGPO_SUCCEEDED(result))
            {
                FATALERROR("Unable to ggpo_add_local_input. %d", result);
            }

            rollback_sync_inputs();
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

    SetEnvironmentVariable("ggpo.log", "1");

    Rollback::NetcodeFix();

    //Synchronize input at the start of each frame
    //do this anytime after the game reads the inputs, but before MoveMapStep_Step_13
    uint8_t* write_address = (uint8_t*)(Rollback::rollback_game_frame_sync_inputs_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &rollback_game_frame_sync_inputs_return, 4, &rollback_game_frame_sync_inputs_injection);

    //Inform ggpo after a frame has been rendered
    write_address = (uint8_t*)(Rollback::MainUpdate_end_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &dsr_frame_finished_return, 0, &dsr_frame_finished_injection);

    //Testing rollback related stuff
    Rollback::saved_playerins = init_PlayerIns(false);
    Rollback::saved_padman = (PadMan**)malloc_(sizeof(PadMan*) * INPUT_ROLLBACK_LENGTH);
    for (size_t i = 0; i < INPUT_ROLLBACK_LENGTH; i++)
    {
        Rollback::saved_padman[i] = init_PadMan();
    }
    Rollback::saved_qinputman = (QInputMgrWindows**)malloc_(sizeof(QInputMgrWindows*) * INPUT_ROLLBACK_LENGTH);
    for (size_t i = 0; i < INPUT_ROLLBACK_LENGTH; i++)
    {
        Rollback::saved_qinputman[i] = init_QInputMgrWindows();
    }
    Rollback::saved_InputDirectionMovementMan = (InputDirectionMovementMan**)malloc_(sizeof(InputDirectionMovementMan*) * INPUT_ROLLBACK_LENGTH);
    for (size_t i = 0; i < INPUT_ROLLBACK_LENGTH; i++)
    {
        Rollback::saved_InputDirectionMovementMan[i] = init_InputDirectionMovementMan();
    }
    Rollback::saved_bulletman = init_BulletMan();
    Rollback::saved_sfxobjs = init_FXManager();
    Rollback::saved_damageman = init_DamageMan();
    //Testing save/restore with a hotkey
    MainLoop::setup_mainloop_callback(state_test, NULL, "state_test");
    MainLoop::setup_mainloop_callback(input_test, NULL, "input_test");
    MainLoop::setup_mainloop_callback(netcode_test, NULL, "netcode_test");
    MainLoop::setup_mainloop_callback(ggpo_toggle, NULL, "ggpo_toggle");
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

        copy_PlayerIns(player, state->playerins[i], true, (i > 0));
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

        state->playerins[i] = init_PlayerIns(i>0);
        copy_PlayerIns(state->playerins[i], player, false, (i>0));
    }

    state->bulletman = init_BulletMan();
    copy_BulletMan(state->bulletman, *(BulletMan**)Game::bullet_man, false);
    //TODO copy_FXManager;
    state->damageman = init_DamageMan();
    copy_DamageMan(state->damageman, *(DamageMan**)Game::damage_man, false);

    *buffer = (unsigned char*)state;
    *len = sizeof(RollbackState*);
    *checksum = 0;

    return true;
}

void rollback_free_buffer(void* buffer)
{
    RollbackState* state = (RollbackState*)buffer;

    for (size_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        free_PlayerIns(state->playerins[i], (i>0));
    }
    //TODO free_FXManager
    free_BulletMan(state->bulletman);
    free_DamageMan(state->damageman);

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
        //TODO
        ConsoleWrite("WARNING: TODO ===== GGPO_EVENTCODE_TIMESYNC =====");
        break;
    }
    return true;
}

bool rollback_log_game_state(char* filename, unsigned char* buffer, int)
{
    return true;
}

void rollback_on_free_input(void* input, int len)
{
    RollbackInput* inputs = (RollbackInput*)input;

    //we only need to free the pointers in the first input, since the others are garbage. Pointers from the other players
    if (len >= sizeof(RollbackInput))
    {
        if (inputs[0].local.padman != NULL)
        {
            free_PadMan(inputs[0].local.padman);
        }
        if (inputs[0].local.qInputMgrWindows != NULL)
        {
            free_QInputMgrWindows(inputs[0].local.qInputMgrWindows);
        }
        if (inputs[0].local.inputDirectionMovementMan != NULL)
        {
            free_InputDirectionMovementMan(inputs[0].local.inputDirectionMovementMan);
        }
    }
}

bool rollback_on_compare_inputs(void* input1, int len1, void* input2, int len2)
{
    RollbackInput* inputs1 = (RollbackInput*)input1;
    RollbackInput* inputs2 = (RollbackInput*)input2;

    if (len1 != len2)
    {
        return false;
    }

    for (size_t i = 0; i < len1 / sizeof(RollbackInput); i++)
    {
        //ignore the local part, just compare the remote
        int isEqual = memcmp(&inputs1[i].remote, &inputs2[i].remote, sizeof(MainPacket));
        if (isEqual != 0)
        {
            return false;
        }
    }

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

        //ggpo_set_frame_delay(ggpo, Rollback::ggpoHandles[0], 1); //bug here where ggpo returns a padded frame 0 on frame 0, which doesn't exist

        ConsoleWrite("GGPO started");
        Rollback::ggpoStarted = true;
    }
}
