/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ainsley Harriott  -  NPC Guard & Boss Guard
        Ashley            -  Anti-Tele-Backstab
        Metal-crow        -  NPC Guard, in-line ASM fixes
        Sean Pesce        -  C++
*/


#include "AntiCheat.h"
#include "DllMain.h"

namespace AntiCheat {

void start() {
    if (!print_console(Mod::output_prefix + "Starting anti-cheat protections:"))
        Mod::startup_messages.push_back(Mod::output_prefix + "Starting anti-cheat protections:");
    // Start NpcGuard anti-cheat
    NpcGuard::start();
    // Start BossGuard anti-cheat
    BossGuard::start();
    // Start TeleBackstabProtect anti-cheat
    TeleBackstabProtect::start();
}

} // namespace AntiCheat
