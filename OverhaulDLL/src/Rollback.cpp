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

static const size_t INPUT_ROLLBACK_LENGTH = 5 * 60; //5 seconds

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

        Game::SuspendThreads();
        Game::Step_GameSimulation();
        Game::ResumeThreads();

        Rollback::gload = false;
    }

    return true;
}

bool Rollback::isave = false;
bool Rollback::iload = false;
static uint32_t inputSaveFrameI = 0;
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

void Rollback::start()
{
    ConsoleWrite("Rollback...");

    Rollback::NetcodeFix();

    //Synchronize input at the start of each frame


    //Testing rollback related stuff
    Rollback::saved_playerins = init_PlayerIns(false);
    Rollback::saved_padman = (PadMan**)malloc(sizeof(PadMan*) * INPUT_ROLLBACK_LENGTH);
    for (size_t i = 0; i < INPUT_ROLLBACK_LENGTH; i++)
    {
        Rollback::saved_padman[i] = init_PadMan();
    }
    Rollback::saved_qinputman = (QInputMgrWindows**)malloc(sizeof(QInputMgrWindows*) * INPUT_ROLLBACK_LENGTH);
    for (size_t i = 0; i < INPUT_ROLLBACK_LENGTH; i++)
    {
        Rollback::saved_qinputman[i] = init_QInputMgrWindows();
    }
    Rollback::saved_InputDirectionMovementMan = (InputDirectionMovementMan**)malloc(sizeof(InputDirectionMovementMan*) * INPUT_ROLLBACK_LENGTH);
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
    const size_t numplayers = 2;
    RollbackInput inputs[numplayers];
    int disconnect_flags;

    //get the inputs for this frame
    GGPOErrorCode res = ggpo_synchronize_input(Rollback::ggpo, (void*)inputs, sizeof(RollbackInput) * numplayers, &disconnect_flags);
    if (res != GGPOErrorCode::GGPO_ERRORCODE_SUCCESS)
    {
        FATALERROR("ggpo_synchronize_input call in rollback_advance_frame_callback returned %d", res);
    }

    //load the inputs
    for (size_t i = 0; i < numplayers; i++)
    {
        if (inputs[i].isLocal)
        {
            copy_PadMan(*(PadMan**)Game::pad_man, inputs[i].local.padman);
            copy_QInputMgrWindows(*(QInputMgrWindows**)Game::QInputMgrWindowsFantasy, inputs[i].local.qInputMgrWindows);
            copy_InputDirectionMovementMan(*(InputDirectionMovementMan**)Game::InputDirectionMovementMan, inputs[i].local.inputDirectionMovementMan);
        }
        else
        {
            Rollback::LoadRemotePlayerPacket(&inputs[i].remote.pkt, inputs[i].remote.playerins, inputs[i].remote.session_player_num);
        }
    }

    //step next frame, ignoring inputs
    Game::SuspendThreads();
    Game::set_ReadInputs_allowed(false);
    Game::Step_GameSimulation();
    Game::set_ReadInputs_allowed(true);
    Game::ResumeThreads();
    return true;
}

/*
 * Makes our current state match the state passed in by GGPO.
 */
bool rollback_load_game_state_callback(unsigned char* buffer, int)
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        FATALERROR("Unable to get playerins for PC in rollback_load_game_state_callback");
    }
    PlayerIns* player = (PlayerIns*)player_o.value();
    if (player == NULL)
    {
        FATALERROR("Unable to get playerins for PC in rollback_load_game_state_callback. Got null");
    }

    PlayerIns* player_guest1 = 0;

    RollbackState* state = (RollbackState*)buffer;

    copy_PlayerIns(player, state->playerins_PC, true, false);
    copy_PlayerIns(player_guest1, state->playerins_Guest1, true, true);
    copy_BulletMan(*(BulletMan**)Game::bullet_man, state->bulletman, true);
    //copy_FXManager
    copy_DamageMan(*(DamageMan**)Game::damage_man, state->damageman, true);

    return true;
}

/*
 * Save the current state to a buffer and return it to GGPO via the
 * buffer and len parameters.
 */
bool rollback_save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int)
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        FATALERROR("Unable to get playerins for PC in rollback_save_game_state_callback");
    }
    PlayerIns* player = (PlayerIns*)player_o.value();
    if (player == NULL)
    {
        FATALERROR("Unable to get playerins for PC in rollback_save_game_state_callback. Got null");
    }

    PlayerIns* player_guest1 = 0;//Game::get_connected_player

    RollbackState* state = (RollbackState*)malloc(sizeof(RollbackState));
    if (state == NULL)
    {
        FATALERROR("Unable to get allocate state for rollback_save_game_state_callback");
    }

    state->playerins_PC = init_PlayerIns(false);
    copy_PlayerIns(state->playerins_PC, player, false, false);
    state->playerins_Guest1 = init_PlayerIns(true);
    copy_PlayerIns(state->playerins_Guest1, player_guest1, false, true);
    state->bulletman = init_BulletMan();
    copy_BulletMan(state->bulletman, *(BulletMan**)Game::bullet_man, false);
    //copy_FXManager;
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

    free_PlayerIns(state->playerins_PC, false);
    free_PlayerIns(state->playerins_Guest1, true);
    //free_FXManager
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

void Rollback::rollback_start_session(ISteamNetworkingMessages* steamMsgs, CSteamID steamid)
{
    if (Rollback::rollbackEnabled)
    {
        GGPOErrorCode result = ggpo_start_session(&Rollback::ggpo, &Rollback::ggpoCallbacks, steamMsgs, "DSR_GGPO", 2, sizeof(RollbackInput));
        if (result != GGPO_ERRORCODE_SUCCESS)
        {
            FATALERROR("unable to start ggpo. %d", result);
        }

        ConsoleWrite("GGPO connecting to %llx", steamid.ConvertToUint64());

        GGPOPlayer pc;
        GGPOPlayerHandle pc_handle;
        pc.size = sizeof(GGPOPlayer);
        pc.type = GGPO_PLAYERTYPE_LOCAL;
        pc.player_num = 1;
        result = ggpo_add_player(ggpo, &pc, &pc_handle);
        if (result != GGPO_ERRORCODE_SUCCESS)
        {
            FATALERROR("unable to ggpo_add_player for self. %d", result);
        }

        GGPOPlayer guest1;
        GGPOPlayerHandle guest1_handle;
        guest1.size = sizeof(GGPOPlayer);
        guest1.type = GGPO_PLAYERTYPE_REMOTE;
        guest1.player_num = 2;
        guest1.u.remote.steamid.SetSteamID(steamid);
        result = ggpo_add_player(ggpo, &guest1, &guest1_handle);
        if (result != GGPO_ERRORCODE_SUCCESS)
        {
            FATALERROR("unable to ggpo_add_player for guest. %d", result);
        }
    }
}
