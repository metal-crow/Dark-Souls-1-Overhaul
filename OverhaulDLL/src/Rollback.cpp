#include "Rollback.h"
#include "SP/memory.h"
#include "SP/memory/injection/asm/x64.h"
#include "DarkSoulsOverhaulMod.h"
#include "MainLoop.h"
#include "InputUtil.h"

bool Rollback::rollbackEnabled = false;

PlayerIns* Rollback::saved_playerins = NULL;
PadMan* Rollback::saved_padman = NULL;
BulletMan* Rollback::saved_bulletman = NULL;

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

        //set and unset this flag before stepping to clean or reset the player or something. it prevents an animation bug
        uint64_t pi = (uint64_t)Game::get_PlayerIns().value();
        *(uint8_t*)(pi + 0x526) |= 0x10;
        Game::Step_GameSimulation();
        *(uint8_t*)(pi + 0x526) &= ~0x10;

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
    Rollback::saved_bulletman = init_BulletMan();

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
    copy_BulletMan(Rollback::saved_bulletman, *(BulletMan**)Game::bullet_man, false);
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
    copy_BulletMan(*(BulletMan**)Game::bullet_man, Rollback::saved_bulletman, true);
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
