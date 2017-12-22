/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Ainsley Harriott  -  NPC Guard & Boss Guard
        Ashley            -  Anti-Tele-Backstab
        Metal-crow        -  NPC Guard
        Sean Pesce        -  C++ conversions
*/


#include "AntiCheat.h"
#include "DllMain.h"


bool TeleBackstabProtect::active = true;
static uint32_t  TeleBackstabProtect_check_return;
static uint32_t  TeleBackstabProtect_store_anim_id_return;
static uint32_t  TeleBackstabProtect_anim_id;
static uint32_t *TeleBackstabProtect_anim_id_ptr = &TeleBackstabProtect_anim_id;
static uint32_t  TeleBackstabProtect_no_move;
static float     TeleBackstabProtect_math_storage;
static float    *TeleBackstabProtect_math_storage_ptr = &TeleBackstabProtect_math_storage;

bool BossGuard::active = true;
static uint32_t BossGuard_check_return;

bool NpcGuard::active = true;
static uint32_t NpcGuard_check_return;


void AntiCheat::start() {
    if (!print_console(Mod::output_prefix + "Starting anti-cheat protections:"))
        Mod::startup_messages.push_back(Mod::output_prefix + "Starting anti-cheat protections:");
    // Start NpcGuard anti-cheat
    NpcGuard::start();
    // Start BossGuard anti-cheat
    BossGuard::start();
    // Start TeleBackstabProtect anti-cheat
    TeleBackstabProtect::start();
}


void TeleBackstabProtect::start() {
    TeleBackstabProtect_no_move = ((uint32_t)Game::ds1_base) + 0xABDBE5;
    if (TeleBackstabProtect::active)
    {
        if (!print_console("    Enabling TeleBackstabProtect..."))
            Mod::startup_messages.push_back("    Enabling TeleBackstabProtect...");

        // Inject function to store new animation ID
        uint8_t *write_address = (uint8_t*)(TeleBackstabProtect::store_anim_injection_offset + ((uint32_t)Game::ds1_base));
        set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
        inject_jmp_5b(write_address, &TeleBackstabProtect_store_anim_id_return, 0, &TeleBackstabProtect::store_new_animation_id);

        // Inject main anti-cheat function
        write_address = (uint8_t*)(TeleBackstabProtect::check_injection_offset + ((uint32_t)Game::ds1_base));
        set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
        inject_jmp_5b(write_address, &TeleBackstabProtect_check_return, 1, &TeleBackstabProtect::check);
    }
    else
    {
        if (!print_console("    WARNING: TeleBackstabProtect is disabled"))
            Mod::startup_messages.push_back("    WARNING: TeleBackstabProtect is disabled");
    }
}


void __declspec(naked) __stdcall TeleBackstabProtect::check() {
    // Assembly code below is injected into the function that moves the player
    __asm
    {
        // Check if movement is due to a backstab, and if so, check validity of movement
        //anti_tele_bs_check:
        // If in backstab animation
        cmp DWORD PTR[TeleBackstabProtect_anim_id_ptr], 0x2328
        je  anti_tele_bs_check_is_backstab
        cmp DWORD PTR[TeleBackstabProtect_anim_id_ptr], 0x24CC
        je  anti_tele_bs_check_is_backstab
        jmp no_tele_bs_detected
        anti_tele_bs_check_is_backstab:
        // If this is the player's position
        push eax
        push ebx
        mov eax, DWORD PTR ds:0x137D644
        mov eax, [eax + 0x3C]
        mov eax, [eax + 0x28]
        mov eax, [eax + 0x1C]
        mov ebx, [eax + 0x10]
        cmp[esi + 0x10], ebx
        jne no_tele_bs_detected_pre
        mov ebx, [eax + 0x18]
        cmp[esi + 0x18], ebx
        jne no_tele_bs_detected_pre
        pop ebx
        pop eax
        //anti_tele_bs_check__is_player:
        // If the distance to move is reasonable
        //save xmm0[0] and xmm1[0]
        movss DWORD PTR[TeleBackstabProtect_math_storage_ptr], xmm1
        movss DWORD PTR[TeleBackstabProtect_math_storage_ptr + 4], xmm0
        // Calculate distance of movement
        // old_x is [esi+0x10]; new_x is in xmm1
        subss xmm1, DWORD PTR[esi + 0x10] // new_x - old_x
        movss DWORD PTR[TeleBackstabProtect_math_storage_ptr + 8], xmm1
        and DWORD PTR[TeleBackstabProtect_math_storage_ptr + 8], 0x7FFFFFFF // Make result abs
        movss xmm1, DWORD PTR[TeleBackstabProtect_math_storage_ptr + 8] // xmm1 = abs(new_x-old_x)
        mulss xmm1, xmm1 // xmm1 = abs(new_x-old_x)^2
        // old_y is [esi+0x18], new_y is in xmm0
        subss xmm0, DWORD PTR[esi + 0x18]
        movss DWORD PTR[TeleBackstabProtect_math_storage_ptr + 8], xmm0
        and DWORD PTR[TeleBackstabProtect_math_storage_ptr + 8], 0x7FFFFFFF
        movss xmm0, DWORD PTR[TeleBackstabProtect_math_storage_ptr + 8]
        mulss xmm0, xmm0 // xmm0 = abs(new_y-old_y)^2
        // int(abs(new_x-old_x)^2 + abs(new_y-old_y)^2)
        addss xmm1, xmm0
        push eax
        cvttss2si eax, xmm1
        mov DWORD PTR[TeleBackstabProtect_math_storage_ptr + 8], eax
        pop eax
        // Restore xmm0 and xmm1
        movss xmm1, DWORD PTR[TeleBackstabProtect_math_storage_ptr]
        movss xmm0, DWORD PTR[TeleBackstabProtect_math_storage_ptr + 4]
        // Reset our global
        mov DWORD PTR[TeleBackstabProtect_anim_id_ptr], 0
        // int(abs(new_x-old_x)^2 + abs(new_y-old_y)^2) > 8^2
        cmp DWORD PTR[TeleBackstabProtect_math_storage_ptr + 8], 0x40
        //jge DARKSOULS.exe + 0xABDBE5
        jge no_move // Too great a distance; don't move the player
        jmp no_tele_bs_detected
        // else continue normally
        no_tele_bs_detected_pre:
        pop ebx
        pop eax
        no_tele_bs_detected:
        movss[esp + 8], xmm0
        jmp TeleBackstabProtect_check_return

        no_move:
        jmp TeleBackstabProtect_no_move
    }
}


void __declspec(naked) __stdcall TeleBackstabProtect::store_new_animation_id() {
    // Assembly code below is injected into the function that sets the player's animation ID to a backstab
    __asm
    {
        mov esi, [eax + 0x28]
        mov DWORD PTR[TeleBackstabProtect_anim_id_ptr], esi // Store the animation id
        xor ebp, ebp
        jmp TeleBackstabProtect_store_anim_id_return
    }
}




void BossGuard::start() {
    if (BossGuard::active)
    {
        if (!print_console("    Enabling BossGuard..."))
            Mod::startup_messages.push_back("    Enabling BossGuard...");
        uint8_t *write_address = (uint8_t*)(BossGuard::injection_offset + ((uint32_t)Game::ds1_base));
        set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
        inject_jmp_5b(write_address, &BossGuard_check_return, 6, &BossGuard::check);
    }
    else
    {
        if (!print_console("    WARNING: BossGuard is disabled"))
            Mod::startup_messages.push_back("    WARNING: BossGuard is disabled");
    }
}



void __declspec(naked) __stdcall BossGuard::check() {
    __asm
    {
        mov ebp, [esp + 0x8]

        push eax
        mov eax, [ebp + 0x28]
        mov eax, [eax + 0xC0] // Load flag info for entity
        test eax, 1 // Bit 00000001 is EnableLogic for boss
        pop eax
        jz abort_damage // If result EnableLogic & 1 is 0, abort

        test byte ptr[ebp + 0x000003C4], 0x40
        jmp BossGuard_check_return

        abort_damage:
        pop ebp
        retn 8
    }
}



void NpcGuard::start() {
    if (NpcGuard::active)
    {
        if (!print_console("    Enabling NpcGuard..."))
            Mod::startup_messages.push_back("    Enabling NpcGuard...");
        uint8_t *write_address = (uint8_t*)(NpcGuard::injection_offset + ((uint32_t)Game::ds1_base));
        set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
        inject_jmp_5b(write_address, &NpcGuard_check_return, 0, &NpcGuard::check);
    }
    else
    {
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


void __declspec(naked) __stdcall NpcGuard::check() {
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
        jmp NpcGuard_check_return
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
