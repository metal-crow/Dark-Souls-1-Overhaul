#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "InputUtil.h"

bool Rollback::rollbackEnabled = false;

PlayerIns* Rollback::saved_playerins = NULL;
PadMan* Rollback::saved_padman = NULL;

bool Rollback::gsave = false;
bool Rollback::gload = false;

bool rollback_test(void* unused)
{
    if (Rollback::gsave)
    {
        Rollback::GameStateSave();
        Rollback::gsave = false;
    }

    if (Rollback::gload)
    {
        Rollback::GameStateLoad();
        Game::Step_GameSimulation();
        Rollback::gload = false;
    }

    return true;
}

bool Rollback::isave = false;
bool Rollback::iload = false;
bool input_test(void* unused)
{
    if (Rollback::isave)
    {
        Rollback::GameInputSave();
        Rollback::isave = false;
    }

    if (Rollback::iload)
    {
        Rollback::GameInputLoad();
        Game::Step_GameSimulation();
        Rollback::iload = false;
    }

    return true;
}

void Rollback::start()
{
    ConsoleWrite("Rollback...");

    Rollback::NetcodeFix();

    //TMP init out copy of the playerins struct, for saving/restoring with rollback
    Rollback::saved_playerins = init_PlayerIns();
    Rollback::saved_padman = init_PadMan();

    //TMP save/restore with a hotkey
    MainLoop::setup_mainloop_callback(rollback_test, NULL, "rollback_test");
    MainLoop::setup_mainloop_callback(input_test, NULL, "input_test");
}

void Rollback::GameStateSave()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        return;
    }
    PlayerIns* player = (PlayerIns*)player_o.value();

    //we pre-allocate a static playerins on boot, so we can assume all pointers are set up
    copy_PlayerIns(Rollback::saved_playerins, player, false);
}

void Rollback::GameStateLoad()
{
    auto player_o = Game::get_PlayerIns();
    if (!player_o.has_value())
    {
        return;
    }
    PlayerIns* player = (PlayerIns*)player_o.value();

    copy_PlayerIns(player, Rollback::saved_playerins, true);
}

void Rollback::GameInputSave()
{
    //we pre-allocate a static padman on boot, so we can assume all pointers are set up
    copy_PadMan(Rollback::saved_padman, *(PadMan**)Game::pad_man);
}

void Rollback::GameInputLoad()
{
    copy_PadMan(*(PadMan**)Game::pad_man, Rollback::saved_padman);
}
