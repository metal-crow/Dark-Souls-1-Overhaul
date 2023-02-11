#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "InputUtil.h"

bool Rollback::rollbackEnabled = false;

PlayerIns* Rollback::saved_playerins = NULL;
PadMan** Rollback::saved_padman = NULL;
QInputMgrWindows** Rollback::saved_qinputman = NULL;
InputDirectionMovementMan** Rollback::saved_InputDirectionMovementMan = NULL;
BulletMan* Rollback::saved_bulletman = NULL;
FXManager* Rollback::saved_sfxobjs = NULL;
DamageMan* Rollback::saved_damageman = NULL;

bool Rollback::gsave = false;
bool Rollback::gload = false;

static const size_t INPUT_ROLLBACK_LENGTH = 5 * 60; //5 seconds

bool rollback_test(void* unused)
{
    if (Rollback::gsave)
    {
        Rollback::GameStateSave();
        Rollback::gsave = false;
    }

    if (Rollback::gload)
    {
        Game::SuspendThreads();
        Rollback::GameStateLoad();
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
        Rollback::GameInputSave(inputSaveFrameI);
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
        Rollback::GameInputLoad(inputSaveFrameI);
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
bool netcode_test(void* unused)
{
    if (Rollback::netcodeSwap)
    {
        Rollback::rollbackEnabled = !Rollback::rollbackEnabled;
        ConsoleWrite("Rollback netcode %d", Rollback::rollbackEnabled);
        Rollback::netcodeSwap = false;
    }
    return true;
}

void Rollback::start()
{
    ConsoleWrite("Rollback...");

    Rollback::NetcodeFix();

    //TMP init out copy of the playerins struct, for saving/restoring with rollback
    Rollback::saved_playerins = init_PlayerIns(false);

    //1 seconds worth of inputs
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

    //TMP save/restore with a hotkey
    MainLoop::setup_mainloop_callback(rollback_test, NULL, "rollback_test");
    MainLoop::setup_mainloop_callback(input_test, NULL, "input_test");
    MainLoop::setup_mainloop_callback(netcode_test, NULL, "netcode_test");
}

void Rollback::GameStateSave()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        return;
    }
    PlayerIns* player = (PlayerIns*)player_o.value();
    if (player == NULL)
    {
        return;
    }

    //we pre-allocate a static playerins on boot, so we can assume all pointers are set up
    copy_PlayerIns(Rollback::saved_playerins, player, false, false);
    copy_BulletMan(Rollback::saved_bulletman, *(BulletMan**)Game::bullet_man, false);
    //copy_FXManager(Rollback::saved_sfxobjs, (*(SfxMan**)Game::sfx_man)->FrpgFxManagerBase->base.fXManager, false);
    copy_DamageMan(Rollback::saved_damageman, *(DamageMan**)Game::damage_man, false);
}

void Rollback::GameStateLoad()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        return;
    }
    PlayerIns* player = (PlayerIns*)player_o.value();
    if (player == NULL)
    {
        return;
    }

    copy_PlayerIns(player, Rollback::saved_playerins, true, false);
    copy_BulletMan(*(BulletMan**)Game::bullet_man, Rollback::saved_bulletman, true);
    //copy_FXManager((*(SfxMan**)Game::sfx_man)->FrpgFxManagerBase->base.fXManager, Rollback::saved_sfxobjs, true);
    copy_DamageMan(*(DamageMan**)Game::damage_man, Rollback::saved_damageman, true);
}

void Rollback::GameInputSave(uint32_t frame)
{
    //we pre-allocate a static padman on boot, so we can assume all pointers are set up
    copy_PadMan(Rollback::saved_padman[frame], *(PadMan**)Game::pad_man);
    copy_QInputMgrWindows(Rollback::saved_qinputman[frame], *(QInputMgrWindows**)Game::QInputMgrWindowsFantasy);
    copy_InputDirectionMovementMan(Rollback::saved_InputDirectionMovementMan[frame], *(InputDirectionMovementMan**)Game::InputDirectionMovementMan);
}

void Rollback::GameInputLoad(uint32_t frame)
{
    copy_PadMan(*(PadMan**)Game::pad_man, Rollback::saved_padman[frame]);
    copy_QInputMgrWindows(*(QInputMgrWindows**)Game::QInputMgrWindowsFantasy, Rollback::saved_qinputman[frame]);
    copy_InputDirectionMovementMan(*(InputDirectionMovementMan**)Game::InputDirectionMovementMan, Rollback::saved_InputDirectionMovementMan[frame]);
}
