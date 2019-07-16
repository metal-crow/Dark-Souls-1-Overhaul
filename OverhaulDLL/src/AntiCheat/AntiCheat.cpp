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

    uint64_t NameCrash_prevent_return;
    void NameCrash_prevent();
    void NameCrash_prevent_helper(wchar_t* name);
}

namespace AntiCheat {

void start() {
    global::cmd_out << Mod::output_prefix + "Starting anti-cheat protections:\n";

    // Start NpcGuard anti-cheat
    global::cmd_out << "    Enabling NpcGuard...\n";
    uint64_t write_address = Game::ds1_base + NpcGuard_offset;
    npc_guard_WorldChrBase = Game::world_char_base;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &npc_guard_check_exit, 2, &npc_guard_asm_check, true);

    // Start BossGuard anti-cheat
    global::cmd_out << "    Enabling BossGuard...\n";
    write_address = Game::ds1_base + BossGuard_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &boss_guard_return, 0, &boss_guard_asm_check);

    // Start TeleBackstabProtect anti-cheat
    global::cmd_out << "    Enabling TeleBackstabProtect...\n";
    write_address = Game::ds1_base + TeleBackstab_getBSAnimation_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &TeleBackstabProtect_store_AnimationId_return, 0, &TeleBackstabProtect_store_AnimationId);
    write_address = Game::ds1_base + TeleBackstab_setPlayerLocation_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &TeleBackstabProtect_setPosition_return, 5, &TeleBackstabProtect_setPosition_check);

    // Start BinocsTriggerBlock anti-cheat
    global::cmd_out << "    Enabling BinocsTriggerBlock...\n";
    void* bino_params = sp::mem::aob_scan("00 0F 00 00 00 00 FF 00 7F");
    if (bino_params == NULL) {
        FATALERROR("Unable to activate BinocsTriggerBlock");
    }
    uint8_t bino_params_patched_bytes[1] = { 0x79 };
    sp::mem::patch_bytes((void*)((uint64_t)bino_params + 8), bino_params_patched_bytes, 1);

    // Start DragonTriggerBlock anti-cheat
    global::cmd_out << "    Enabling DragonTriggerBlock...\n";
    uint8_t dragon_params_patched_bytes[1] = { 0x79 };
    void* dragon_head_params = sp::mem::aob_scan("60 00 00 00 00 FF 00 7F");
    void* dragon_body_params = sp::mem::aob_scan("A7 00 00 00 00 FF 00 7F");
    if (dragon_head_params == NULL || dragon_body_params == NULL) {
        FATALERROR("Unable to activate DragonTriggerBlock");
    }
    sp::mem::patch_bytes((void*)((uint64_t)dragon_head_params + 7), dragon_params_patched_bytes, 1);
    sp::mem::patch_bytes((void*)((uint64_t)dragon_body_params + 7), dragon_params_patched_bytes, 1);

    // Start Namecrash prevention anti-cheat
    global::cmd_out << "    Enabling NameCrash prevention...\n";
    write_address = Game::ds1_base + NameCrash_prevention_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &NameCrash_prevent_return, 0, &NameCrash_prevent);
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

void NameCrash_prevent_helper(wchar_t* name) {
    size_t i = 0;
    wchar_t cur_char = name[i];
    while (cur_char != L'\0')
    {
        //if we locate the starting or ending tokens "<?" or "?>", then replace the </> to nullify it
        if (cur_char == L'<' && name[i + 1] == L'?') {
            name[i] = L'(';
        }
        else if (cur_char == L'?' && name[i + 1] == L'>') {
            name[i + 1] = L')';
        }

        i++;
        cur_char = name[i];
    }
}
