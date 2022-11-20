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
#include "PlayerInsStruct.h"

extern "C" {
    uint64_t npc_guard_check_exit;
    void npc_guard_asm_check();
    uint32_t npc_guard_asm_check_helper(uint64_t attacker, uint64_t target, uint32_t damage);

    uint64_t boss_guard_return;
    void boss_guard_asm_check();
    uint64_t boss_guard_asm_check_helper(ChrIns* target);

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
    ConsoleWrite("Starting anti-cheat protections:");

    // Start NpcGuard anti-cheat
    ConsoleWrite("    Enabling NpcGuard...");
    uint64_t write_address = Game::ds1_base + NpcGuard_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &npc_guard_check_exit, 2, &npc_guard_asm_check, true);

    // Start BossGuard anti-cheat
    ConsoleWrite("    Enabling BossGuard...");
    write_address = Game::ds1_base + BossGuard_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &boss_guard_return, 2, &boss_guard_asm_check);

    // Start TeleBackstabProtect anti-cheat
    ConsoleWrite("    Enabling TeleBackstabProtect...");
    write_address = Game::ds1_base + TeleBackstab_getBSAnimation_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &TeleBackstabProtect_store_AnimationId_return, 0, &TeleBackstabProtect_store_AnimationId);
    write_address = Game::ds1_base + TeleBackstab_setPlayerLocation_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &TeleBackstabProtect_setPosition_return, 1, &TeleBackstabProtect_setPosition_check);

    // Start Namecrash prevention anti-cheat
    ConsoleWrite("    Enabling NameCrash prevention...");
    write_address = Game::ds1_base + NameCrash_prevention_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &NameCrash_prevent_return, 0, &NameCrash_prevent);

    // Starting bad-speffect packet anti-cheat
    ConsoleWrite("    Enabling Bad Speffect Packet prevention...");
    write_address = Game::ds1_base + ReadParseType18_packet_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &ReadParseType18_packet_return, 2, &ReadParseType18_packet_injection);

    write_address = Game::ds1_base + ReadParseType35_packet_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &ReadParseType35_packet_return, 3, &ReadParseType35_packet_injection);
}

} // namespace AntiCheat

uint32_t npc_guard_asm_check_helper(uint64_t attacker, uint64_t target, uint32_t damage)
{
    // If local player is attacking, do not protect NPCs
    auto playerins_o = Game::get_PlayerIns();
    if (!playerins_o.has_value())
    {
        return 0;
    }
    void* playerins = playerins_o.value();
    if ((uint64_t)playerins == attacker)
    {
        return damage;
    }

    // Check if the target is a non-hostile npc
    switch (target)
    {
    case 0x1798: // Griggs of Vinheim (Undead Burg)
    case 0x179A: // Griggs of Vinheim (Firelink Shrine)
    case 0x184C: // Blacksmith Rickert
    case 0x178E: // Big Hat Logan
    case 0x178F: // Big Hat Logan (The Duke's Archives)
    case 0x17A2: // Dusk of Oolacile
    case 0x1A2C: // Dusk of Oolacile (Past forest)
    case 0x1824: // Ingward (New Londo)
    case 0x1825: // Ingward (Firelink Shrine)
    case 0x17F2: // Laurentius (Depths)
    case 0x17F3: // Laurentius (Firelink Shrine)
    case 0x181A: // Quelana of Izalith
    case 0x1810: // Eingyi
    case 0x17C0: // Petrus of Thorolund
    case 0x17C1: // Petrus of Thorolund
    case 0x17B6: // Rhea of Thorolund
    case 0x187E: // Crestfallen Warrior
    case 0x43238: // Anastacia
    case 0x82208: // Kingseeker Frampt
    case 0x17CA: // Vince of Thorolund
    case 0x17D4: // Nico of Thorolund
    case 0x1770: // Solaire of Astora
    case 0x1771: // Solaire of Astora (Shrine in Undead Burg?)
    case 0x1772: // Solaire of Astora (Anor Londo)
    case 0x1773: // Solaire of Astora (Demon Ruins/Lost Izalith)
    case 0x1774: // Solaire of Astora (Demon Ruins/Lost Izalith// heliozoan?)
    case 0x1775: // Solaire of Astora
    case 0x1776: // Solaire of Astora
    case 0x1777: // Solaire of Astora
    case 0x1778: // Solaire of Astora
    case 0x1856: // Undead Male Merchant
    case 0x1857: // Undead Male Merchant
    case 0x1858: // Undead Male Merchant
    case 0x3D478: // Undead Merchant
    case 0x3D479: // Undead Merchant
    case 0x40740: // Andre of Astora
    case 0x189C: // Knight Lautrec of Carim
    case 0x189D: // Knight Lautrec of Carim
    case 0x18E2: // Oswald of Carim
    case 0x82E24: // Alvina (No textures)
    case 0x82E88: // Alvina (No textures gray)
    case 0x18A6: // Shiva of the East
    case 0x1874: // Domhnall of Zena (Depths)
    case 0x1875: // Domhnall of Zena (Firelink Shrine)
    case 0x825F0: // The Fair Lady
    case 0x1888: // Siegmeyer of Catarina (Sen's Fortress)
    case 0x1889: // Siegmeyer of Catarina (Anor Londo)
    case 0x188A: // Siegmeyer of Catarina (Firelink Shrine)
    case 0x188B: // Siegmeyer of Catarina (Blighttown)
    case 0x188C: // Siegmeyer of Catarina (Lost Izalith)
    case 0x1892: // Sieglinde of Catarina (The Duke's Archives)
    case 0x1893: // Sieglinde of Catarina (The GreHollow)
    case 0x186A: // Crestfallen Merchant
    case 0x82209: // Kaathe
    case 0x177A: // Lady of the Darkling
    case 0x81A38: // Gwynevere
    case 0x45D31: // Giant Blacksmith
    case 0x474A0: // Vamos
    case 0x474A2: // Vamos
    case 0x65130: // Elizabeth
    case 0x3F3B8: // Marvellous Chester
    case 0x1A54: // Lord's Blade Ciaran
    case 0x1A55: // Lord's Blade Ciaran
    case 0x1A56: // Lord's Blade Ciaran
    case 0x1A57: // Lord's Blade Ciaran
    case 0x1A58: // Lord's Blade Ciaran
    case 0x1A59: // Lord's Blade Ciaran
    case 0x64578: // Hawkeye Gough
    case 0x6476C: // Hawkeye Gough (No textures)
        return 0;
    default:
        return damage;
    }
}

uint64_t boss_guard_asm_check_helper(ChrIns* target)
{
    uint32_t entityId = *(uint32_t*)(((uint64_t)target) + 0x2B0);
    uint8_t EnableLogic = *(uint32_t*)((uint64_t)(&target->playerCtrl->chrCtrl) + 0x100) & 1;

    switch (entityId)
    {
    case 1000800: // Gaping Dragon
    case 1010700: // Taurus Demon
    case 1010750: // Caprademon
    case 1010800: // Bell Gargoyles
    case 1100160: // Crossbreed Priscilla
    case 1200800: // Sif, the Great Gray Wolf
    case 1200801: // Moonlight Butterfly
    case 1210401: // Kalameet
    case 1210800: // Sanctuary Guardian
    case 1210820: // Knight Artorias
    case 1210840: // Manus, Father of the Abyss
    case 1300800: // Pinwheel
    case 1310800: // Gravelord Nito
    case 1400800: // Quelaag
    case 1410400: // Demon Firesage
    case 1410600: // Ceaseless Discharge
    case 1410700: // Unlucky Demon (cut content?)
    case 1410800: // Bed of Chaos
    case 1500800: // Iron Golem
    case 1510650: // Dark Sun Gwyndolin
    case 1510801: // Ornstein
    case 1510811: // Smough
    case 1700800: // Seath
    case 1800800: // Gwyn
    case 1810800: // Asylum Demon
    case 1810810: // Stray Demon
        return EnableLogic == 0; //if boss is disabled, don't allow to kill

    case 1600800: // Four Kings (can't support, since they don't use EnableLogic)
    default:
        return 0; //non boss, allow to kill
    }
}

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
