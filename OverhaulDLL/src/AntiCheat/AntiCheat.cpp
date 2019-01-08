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
#include <cmath>

extern "C" {
    uint64_t npc_guard_WorldChrBase;
    uint64_t npc_guard_check_exit;
    void npc_guard_asm_check();

    uint64_t boss_guard_return;
    void boss_guard_asm_check();

    uint64_t TeleBackstabProtect_store_AnimationId_return;
    void TeleBackstabProtect_store_AnimationId();
    volatile uint32_t saved_anim_id;

    uint64_t TeleBackstabProtect_setPosition_return;
    void TeleBackstabProtect_setPosition_check();
    alignas(64) float new_player_position[4];
    void TeleBackstabProtect_helper_check(float* old_position);
}

namespace AntiCheat {

void start() {
    global::cmd_out << Mod::output_prefix + "Starting anti-cheat protections:";
    Mod::startup_messages.push_back(Mod::output_prefix + "Starting anti-cheat protections:");

    // Start NpcGuard anti-cheat
    global::cmd_out << "    Enabling NpcGuard...";
    Mod::startup_messages.push_back("    Enabling NpcGuard...");
    uint64_t write_address = Game::ds1_base + NpcGuard_offset;
    npc_guard_WorldChrBase = Game::world_char_base;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &npc_guard_check_exit, 2, &npc_guard_asm_check, true);

    // Start BossGuard anti-cheat
    global::cmd_out << "    Enabling BossGuard...";
    Mod::startup_messages.push_back("    Enabling BossGuard...");
    write_address = Game::ds1_base + BossGuard_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &boss_guard_return, 0, &boss_guard_asm_check);

    // Start TeleBackstabProtect anti-cheat
    global::cmd_out << "    Enabling TeleBackstabProtect...";
    Mod::startup_messages.push_back("    Enabling TeleBackstabProtect...");
    write_address = Game::ds1_base + TeleBackstab_getBSAnimation_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &TeleBackstabProtect_store_AnimationId_return, 0, &TeleBackstabProtect_store_AnimationId);
    write_address = Game::ds1_base + TeleBackstab_setPlayerLocation_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &TeleBackstabProtect_setPosition_return, 5, &TeleBackstabProtect_setPosition_check);
}

} // namespace AntiCheat

void TeleBackstabProtect_helper_check(float* old_position) {
    bool update_pos = true;

    //if this is the PC position and they're being backstabbed
    if (old_position == Game::get_pc_position() && (saved_anim_id == 9000 || saved_anim_id == 9420)) {
        //check that the distance isn't too great
        //(new_x-old_x)^2 + (new_y-old_y)^2) > 8^2
        if ((std::pow(new_player_position[0] - old_position[0], 2.0f) + std::pow(new_player_position[2] - old_position[2], 2.0f)) > 64) {
            update_pos = false;
        }
    }

    //update the position
    if (update_pos) {
        old_position[0] = new_player_position[0];
        old_position[1] = new_player_position[1];
        old_position[2] = new_player_position[2];
        old_position[3] = new_player_position[3];
    }
}
