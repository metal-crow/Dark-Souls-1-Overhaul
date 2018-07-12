/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ainsley Harriott  -  NPC Guard & Boss Guard
        Ashley            -  Anti-Tele-Backstab
        Metal-crow        -  NPC Guard, in-line ASM fixes
        Sean Pesce        -  C++
*/


#include "AntiCheat.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t npc_guard_WorldChrBase;
    uint64_t npc_guard_check_exit;
    void npc_guard_asm_check();
}

namespace AntiCheat {

void start() {
    global::cmd_out << Mod::output_prefix + "Starting anti-cheat protections:";
    Mod::startup_messages.push_back(Mod::output_prefix + "Starting anti-cheat protections:");

    // Start NpcGuard anti-cheat
    Mod::startup_messages.push_back("    Enabling NpcGuard...");
    uint64_t write_address = Game::ds1_base + 0x3206C0;
    npc_guard_WorldChrBase = Game::world_char_base;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &npc_guard_check_exit, 2, &npc_guard_asm_check, true);

    // Start BossGuard anti-cheat
    //BossGuard::start();
    // Start TeleBackstabProtect anti-cheat
    //TeleBackstabProtect::start();
}

} // namespace AntiCheat
