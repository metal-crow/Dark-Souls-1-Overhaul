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

typedef void MoveMapStep_Step_13_FUNC(void* MoveMapStep, float frame_delta_in_seconds);
MoveMapStep_Step_13_FUNC* MoveMapStep_Step_13 = (MoveMapStep_Step_13_FUNC*)0x14024ddd0;

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
        auto movemapstep_o = Game::get_MoveMapStep();
        if (!movemapstep_o.has_value())
        {
            ConsoleWrite("!movemapstep_o.has_value()");
            return true;
        }
        MoveMapStep_Step_13(movemapstep_o.value(), 0.016666668f);
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
        auto movemapstep_o = Game::get_MoveMapStep();
        if (!movemapstep_o.has_value())
        {
            ConsoleWrite("!movemapstep_o.has_value()");
            return true;
        }
        MoveMapStep_Step_13(movemapstep_o.value(), 0.016666668f);
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
