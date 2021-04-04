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
#include <unordered_set>

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

    uint64_t ReadParseType18_packet_return;
    void ReadParseType18_packet_injection();
    void ReadParseType18_packet_injection_helper(uint64_t packet);

    uint64_t ReadParseType35_packet_return;
    void ReadParseType35_packet_injection();
    void ReadParseType35_packet_injection_helper(uint64_t packet);
}

namespace AntiCheat {

void start() {
    ConsoleWrite("%sStarting anti-cheat protections:", Mod::output_prefix);

    // Start NpcGuard anti-cheat
    ConsoleWrite("    Enabling NpcGuard...");
    uint64_t write_address = Game::ds1_base + NpcGuard_offset;
    npc_guard_WorldChrBase = Game::world_chr_man_imp;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &npc_guard_check_exit, 2, &npc_guard_asm_check, true);

    // Start BossGuard anti-cheat
    ConsoleWrite("    Enabling BossGuard...");
    write_address = Game::ds1_base + BossGuard_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &boss_guard_return, 0, &boss_guard_asm_check);

    // Start TeleBackstabProtect anti-cheat
    ConsoleWrite("    Enabling TeleBackstabProtect...");
    write_address = Game::ds1_base + TeleBackstab_getBSAnimation_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &TeleBackstabProtect_store_AnimationId_return, 0, &TeleBackstabProtect_store_AnimationId);
    write_address = Game::ds1_base + TeleBackstab_setPlayerLocation_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &TeleBackstabProtect_setPosition_return, 5, &TeleBackstabProtect_setPosition_check);

    // Start Namecrash prevention anti-cheat
    ConsoleWrite("    Enabling NameCrash prevention...");
    write_address = Game::ds1_base + NameCrash_prevention_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &NameCrash_prevent_return, 0, &NameCrash_prevent);

    // Starting bad-speffect packet anti-cheat
    ConsoleWrite("    Enabling Bad Speffect Packet prevention...");
    write_address = Game::ds1_base + ReadParseType18_packet_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &ReadParseType18_packet_return, 7, &ReadParseType18_packet_injection);

    write_address = Game::ds1_base + ReadParseType35_packet_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &ReadParseType35_packet_return, 0, &ReadParseType35_packet_injection);
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

static const std::unordered_set<uint32_t> badSpeffectsList = {
    6, 7, 13, 33, 34, 71, 72, 73, 74, 80, 1520, 1530, 3220, 3240, 3250, 5210, 5211, 5212, 5213,
    5214, 5295, 5296, 5221, 5222, 5280, 5290, 5220, 5281, 5291, 5282, 5292, 5283, 5293, 5284, 5294
};

// Check if we receive a packet with a known bad speffect, and remove it if it contains it
void ReadParseType18_packet_injection_helper(uint64_t packet)
{
    uint32_t* spell_speffect = (uint32_t*)(packet + 0x28);
    uint32_t* weapon_speffect = (uint32_t*)(packet + 0x3c);
    float* damagephys = (float*)(packet + 0x0);
    float* damagemagic = (float*)(packet + 0x4);
    float* damagefire = (float*)(packet + 0x8);
    float* damagelight = (float*)(packet + 0xc);

    // if the spell_speffect is the spell for TWOP/StoneGreatsword, the packet must do no damage otherwise we get TWOD
    if (*spell_speffect == 500 && ((*damagephys > 0.0f) || (*damagemagic > 0.0f) || (*damagefire > 0.0f) || (*damagelight > 0.0f)))
    {
        *damagephys = 0;
        *damagemagic = 0;
        *damagefire = 0;
        *damagelight = 0;
    }

    // clear out the normal bad speffects
    if (badSpeffectsList.count(*spell_speffect))
    {
        *spell_speffect = -1;
    }

    if (badSpeffectsList.count(*weapon_speffect))
    {
        *weapon_speffect = -1;
    }

    return;
}

void ReadParseType35_packet_injection_helper(uint64_t packet)
{
    uint32_t* speffect = (uint32_t*)(packet);
    if (badSpeffectsList.count(*speffect))
    {
        *speffect = -1;
    }

    return;
}
