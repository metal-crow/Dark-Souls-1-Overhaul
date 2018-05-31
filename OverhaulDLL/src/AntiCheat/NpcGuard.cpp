/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ainsley Harriott  -  NPC Guard
        Metal-crow        -  NPC Guard, in-line ASM fixes
        Sean Pesce        -  C++
*/


#include "AntiCheat.h"
#include "DllMain_Legacy.h"

#define DS1_AC_NPC_GUARD_INJECTION_OFFSET 0xA6BFF0

namespace AntiCheat {
namespace NpcGuard {

bool active = true;
uint32_t check_return;

void start()
{
    if (active)
    {
        if (!print_console("    Enabling NpcGuard..."))
            Mod::startup_messages.push_back("    Enabling NpcGuard...");
        uint8_t *write_address = (uint8_t*)(DS1_AC_NPC_GUARD_INJECTION_OFFSET + ((uint32_t)Game::ds1_base));
        set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
        inject_jmp_5b(write_address, &check_return, 0, &asm_check);
    } else {
        if (!print_console("    WARNING: NpcGuard is disabled"))
            Mod::startup_messages.push_back("    WARNING: NpcGuard is disabled");
    }
}



#define GRIGGS_BURG            0x1798  // Griggs of Vinheim (Undead Burg)
#define GRIGGS_FIRELINK        0x179A  // Griggs of Vinheim (Firelink Shrine)
#define RICKERT                0x184C  // Blacksmith Rickert
#define LOGAN                  0x178E  // Big Hat Logan
#define LOGAN_ARCHIVES         0x178F  // Big Hat Logan (The Duke's Archives)
#define DUSK                   0x17A2  // Dusk of Oolacile
#define DUSK_PAST              0x1A2C  // Dusk of Oolacile (Past forest)
#define INGWARD_NEW_LONDO      0x1824  // Ingward (New Londo)
#define INGWARD_FIRELINK       0x1825  // Ingward (Firelink Shrine)
#define LAURENT_DEPTHS         0x17F2  // Laurentius (Depths)
#define LAURENT_FIRELINK       0x17F3  // Laurentius (Firelink Shrine)
#define QUELANA                0x181A  // Quelana of Izalith
#define EINGYI                 0x1810  // Eingyi
#define PETRUS                 0x17C0  // Petrus of Thorolund
#define PETRUS2                0x17C1  // Petrus of Thorolund
#define RHEA                   0x17B6  // Rhea of Thorolund
#define CRESTFALLEN            0x187E  // Crestfallen Warrior
#define ANASTACIA              0x43238 // Anastacia
#define FRAMPT                 0x82208 // Kingseeker Frampt
#define VINCE                  0x17CA  // Vince of Thorolund
#define NICO                   0x17D4  // Nico of Thorolund
#define SOLAIRE                0x1770  // Solaire of Astora
#define SOLAIRE_BURG           0x1771  // Solaire of Astora (Shrine in Undead Burg?)
#define SOLAIRE_ANOR_LONDO     0x1772  // Solaire of Astora (Anor Londo)
#define SOLAIRE_RUINS          0x1773  // Solaire of Astora (Demon Ruins/Lost Izalith)
#define SOLAIRE_RUINS2         0x1774  // Solaire of Astora (Demon Ruins/Lost Izalith; heliozoan?)
#define SOLAIRE_2              0x1775  // Solaire of Astora
#define SOLAIRE_3              0x1776  // Solaire of Astora
#define SOLAIRE_4              0x1777  // Solaire of Astora
#define SOLAIRE_5              0x1778  // Solaire of Astora
#define UNDEAD_MERCH_M         0x1856  // Undead Male Merchant
#define UNDEAD_MERCH_M2        0x1857  // Undead Male Merchant
#define UNDEAD_MERCH_M3        0x1858  // Undead Male Merchant
#define UNDEAD_MERCH_F         0x3D478 // Undead Merchant
#define UNDEAD_MERCH_F2        0x3D479 // Undead Merchant
#define ANDRE                  0x40740 // Andre of Astora
#define LAUTREC                0x189C  // Knight Lautrec of Carim
#define LAUTREC2               0x189D  // Knight Lautrec of Carim
#define OSWALD                 0x18E2  // Oswald of Carim
#define ALVINA_NO_TEX          0x82E24 // Alvina (No textures)
#define ALVINA_GRAY            0x82E88 // Alvina (No textures gray)
#define SHIVA                  0x18A6  // Shiva of the East
#define DOMHNALL_DEPTHS        0x1874  // Domhnall of Zena (Depths)
#define DOMHNALL_FIRELINK      0x1875  // Domhnall of Zena (Firelink Shrine)
#define FAIR_LADY              0x825F0 // The Fair Lady
#define SIEGMEYER_SENS         0x1888  // Siegmeyer of Catarina (Sen's Fortress)
#define SIEGMEYER_ANOR_LONDO   0x1889  // Siegmeyer of Catarina (Anor Londo)
#define SIEGMEYER_FIRELINK     0x188A  // Siegmeyer of Catarina (Firelink Shrine)
#define SIEGMEYER_BLIGHTTOWN   0x188B  // Siegmeyer of Catarina (Blighttown)
#define SIEGMEYER_IZALITH      0x188C  // Siegmeyer of Catarina (Lost Izalith)
#define SIEGLINDE_ARCHIVES     0x1892  // Sieglinde of Catarina (The Duke's Archives)
#define SIEGLINDE_GREAT_HOLLOW 0x1893  // Sieglinde of Catarina (The GreHollow)
#define CRESTFALLEN_MERCH      0x186A  // Crestfallen Merchant
#define KAATHE                 0x82209 // Kaathe
#define LADY_DARKLING          0x177A  // Lady of the Darkling
#define GWYNEVERE              0x81A38 // Gwynevere
#define GIANT_SMITH            0x45D31 // Giant Blacksmith
#define VAMOS                  0x474A0 // Vamos
#define VAMOS2                 0x474A2 // Vamos
#define ELIZABETH              0x65130 // Elizabeth
#define ELIZABETH_NO_TEX       0x65130 // Elizabeth (No textures)
#define CHESTER                0x3F3B8 // Marvellous Chester
#define CIARAN                 0x1A54  // Lord's Blade Ciaran
#define CIARAN2                0x1A55  // Lord's Blade Ciaran
#define CIARAN3                0x1A56  // Lord's Blade Ciaran
#define CIARAN4                0x1A57  // Lord's Blade Ciaran
#define CIARAN5                0x1A58  // Lord's Blade Ciaran
#define CIARAN6                0x1A59  // Lord's Blade Ciaran
#define GOUGH                  0x64578 // Hawkeye Gough
#define GOUGH_NO_TEX           0x6476C // Hawkeye Gough (No textures)


void __declspec(naked) __stdcall asm_check()
{
    __asm
    {
        // esi is entityPointer of the attacker
        // If local player is attacking, do not protect NPCs
        push eax
        mov  eax, DWORD PTR ds:0x137DC70
        mov  eax, [eax + 4] // eax is now entityPointer to local player
        cmp[eax], esi // If entityPointer for local player == attacker
        pop  eax
        je bypass_check

        // eax is entityPointer of ATTACKER's target; check if they are a non-hostile NPC
        push eax
        mov  eax, [eax + 0x208] // Event Entity ID for the target entity

        // Check for event ID of a protected NPC
        cmp eax, GRIGGS_BURG
        je abort_damage
        cmp eax, GRIGGS_FIRELINK
        je abort_damage
        cmp eax, RICKERT
        je abort_damage
        cmp eax, LOGAN
        je abort_damage
        cmp eax, LOGAN_ARCHIVES
        je abort_damage
        cmp eax, DUSK
        je abort_damage
        cmp eax, DUSK_PAST
        je abort_damage
        cmp eax, INGWARD_NEW_LONDO
        je abort_damage
        cmp eax, INGWARD_FIRELINK
        je abort_damage
        cmp eax, LAURENT_DEPTHS
        je abort_damage
        cmp eax, LAURENT_FIRELINK
        je abort_damage
        cmp eax, QUELANA
        je abort_damage
        cmp eax, EINGYI
        je abort_damage
        cmp eax, PETRUS
        je abort_damage
        cmp eax, PETRUS2
        je abort_damage
        cmp eax, RHEA
        je abort_damage
        cmp eax, CRESTFALLEN
        je abort_damage
        cmp eax, ANASTACIA
        je abort_damage
        cmp eax, FRAMPT
        je abort_damage
        cmp eax, VINCE
        je abort_damage
        cmp eax, NICO
        je abort_damage
        cmp eax, SOLAIRE
        je abort_damage
        cmp eax, SOLAIRE_BURG
        je abort_damage
        cmp eax, SOLAIRE_ANOR_LONDO
        je abort_damage
        cmp eax, SOLAIRE_RUINS
        je abort_damage
        cmp eax, SOLAIRE_RUINS2
        je abort_damage
        cmp eax, SOLAIRE_2
        je abort_damage
        cmp eax, SOLAIRE_3
        je abort_damage
        cmp eax, SOLAIRE_4
        je abort_damage
        cmp eax, SOLAIRE_5
        je abort_damage
        cmp eax, UNDEAD_MERCH_M
        je abort_damage
        cmp eax, UNDEAD_MERCH_M2
        je abort_damage
        cmp eax, UNDEAD_MERCH_M3
        je abort_damage
        cmp eax, UNDEAD_MERCH_F
        je abort_damage
        cmp eax, UNDEAD_MERCH_F2
        je abort_damage
        cmp eax, ANDRE
        je abort_damage
        cmp eax, LAUTREC
        je abort_damage
        cmp eax, LAUTREC2
        je abort_damage
        cmp eax, OSWALD
        je abort_damage
        cmp eax, ALVINA_NO_TEX
        je abort_damage
        cmp eax, ALVINA_GRAY
        je abort_damage
        cmp eax, SHIVA
        je abort_damage
        cmp eax, DOMHNALL_DEPTHS
        je abort_damage
        cmp eax, DOMHNALL_FIRELINK
        je abort_damage
        cmp eax, FAIR_LADY
        je abort_damage
        cmp eax, SIEGMEYER_SENS
        je abort_damage
        cmp eax, SIEGMEYER_ANOR_LONDO
        je abort_damage
        cmp eax, SIEGMEYER_FIRELINK
        je abort_damage
        cmp eax, SIEGMEYER_BLIGHTTOWN
        je abort_damage
        cmp eax, SIEGMEYER_IZALITH
        je abort_damage
        cmp eax, SIEGLINDE_ARCHIVES
        je abort_damage
        cmp eax, SIEGLINDE_GREAT_HOLLOW
        je abort_damage
        cmp eax, CRESTFALLEN_MERCH
        je abort_damage
        cmp eax, KAATHE
        je abort_damage
        cmp eax, LADY_DARKLING
        je abort_damage
        cmp eax, GWYNEVERE
        je abort_damage
        cmp eax, GIANT_SMITH
        je abort_damage
        cmp eax, VAMOS
        je abort_damage
        cmp eax, VAMOS2
        je abort_damage
        cmp eax, ELIZABETH
        je abort_damage
        cmp eax, ELIZABETH_NO_TEX
        je abort_damage
        cmp eax, CHESTER
        je abort_damage
        cmp eax, CIARAN
        je abort_damage
        cmp eax, CIARAN2
        je abort_damage
        cmp eax, CIARAN3
        je abort_damage
        cmp eax, CIARAN4
        je abort_damage
        cmp eax, CIARAN5
        je abort_damage
        cmp eax, CIARAN6
        je abort_damage
        cmp eax, GOUGH
        je abort_damage
        cmp eax, GOUGH_NO_TEX
        je abort_damage

        pop eax
        jmp bypass_check

        abort_damage:
        pop eax
        // Set damage to 0
        mov[ebx + 0x16C], -1

        bypass_check:
        comiss xmm0, [ebx]
        push esi
        push edi
        jmp check_return
    }
}


#undef GRIGGS_BURG
#undef GRIGGS_FIRELINK
#undef RICKERT
#undef LOGAN
#undef LOGAN_ARCHIVES
#undef DUSK
#undef DUSK_PAST
#undef INGWARD_NEW_LONDO
#undef INGWARD_FIRELINK
#undef LAURENT_DEPTHS
#undef LAURENT_FIRELINK
#undef QUELANA
#undef EINGYI
#undef PETRUS
#undef PETRUS2
#undef RHEA
#undef CRESTFALLEN
#undef ANASTACIA
#undef FRAMPT
#undef VINCE
#undef NICO
#undef SOLAIRE
#undef SOLAIRE_BURG
#undef SOLAIRE_ANOR_LONDO
#undef SOLAIRE_RUINS
#undef SOLAIRE_RUINS2
#undef SOLAIRE_2
#undef SOLAIRE_3
#undef SOLAIRE_4
#undef SOLAIRE_5
#undef UNDEAD_MERCH_M
#undef UNDEAD_MERCH_M2
#undef UNDEAD_MERCH_M3
#undef UNDEAD_MERCH_F
#undef UNDEAD_MERCH_F2
#undef ANDRE
#undef LAUTREC
#undef LAUTREC2
#undef OSWALD
#undef ALVINA_NO_TEX
#undef ALVINA_GRAY
#undef SHIVA
#undef DOMHNALL_DEPTHS
#undef DOMHNALL_FIRELINK
#undef FAIR_LADY
#undef SIEGMEYER_SENS
#undef SIEGMEYER_ANOR_LONDO
#undef SIEGMEYER_FIRELINK
#undef SIEGMEYER_BLIGHTTOWN
#undef SIEGMEYER_IZALITH
#undef SIEGLINDE_ARCHIVES
#undef SIEGLINDE_GREAT_HOLLOW
#undef CRESTFALLEN_MERCH
#undef KAATHE
#undef LADY_DARKLING
#undef GWYNEVERE
#undef GIANT_SMITH
#undef VAMOS
#undef VAMOS2
#undef ELIZABETH
#undef ELIZABETH_NO_TEX
#undef CHESTER
#undef CIARAN
#undef CIARAN2
#undef CIARAN3
#undef CIARAN4
#undef CIARAN5
#undef CIARAN6
#undef GOUGH
#undef GOUGH_NO_TEX

} // namespace NpcGuard
} // namespace AntiCheat

#undef DS1_AC_NPC_GUARD_INJECTION_OFFSET
