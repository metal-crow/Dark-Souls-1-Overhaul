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

typedef void Step_Chr_FUNC(void* movemapstep, float frame_time, byte param_3);
Step_Chr_FUNC* Step_Chr = (Step_Chr_FUNC*)0x14024f7f0;

typedef void Step_Bullet_FUNC(void* bulletman, float frame_time);
Step_Bullet_FUNC* Step_Bullet = (Step_Bullet_FUNC*)0x140428e40;

typedef void Step_DamageMan_FUNC(void* damageman, float frame_time);
Step_DamageMan_FUNC* Step_DamageMan = (Step_DamageMan_FUNC*)0x1403c97c0;

static const float FRAMETIME = 0.01666666666f;

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
        Step_Chr(movemapstep_o.value(), FRAMETIME, 1);
        Step_Bullet(*(void**)Game::bullet_man, FRAMETIME);
        Step_DamageMan(*(void**)Game::damage_man, FRAMETIME);
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
        Step_Chr(movemapstep_o.value(), FRAMETIME, 1);
        Step_Bullet(*(void**)Game::bullet_man, FRAMETIME);
        Step_DamageMan(*(void**)Game::damage_man, FRAMETIME);
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
