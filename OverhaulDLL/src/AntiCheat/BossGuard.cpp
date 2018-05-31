/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
*/


#include "AntiCheat.h"
#include "DllMain_Legacy.h"

#define DS1_AC_BOSS_GUARD_INJECTION_OFFSET 0xA688E5

namespace AntiCheat {
namespace BossGuard {

bool active = true;
uint32_t check_return;

void start()
{
    if (active)
    {
        if (!print_console("    Enabling BossGuard..."))
            Mod::startup_messages.push_back("    Enabling BossGuard...");
        uint8_t *write_address = (uint8_t*)(DS1_AC_BOSS_GUARD_INJECTION_OFFSET + ((uint32_t)Game::ds1_base));
        set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
        inject_jmp_5b(write_address, &check_return, 6, &asm_check);
    } else {
        if (!print_console("    WARNING: BossGuard is disabled"))
            Mod::startup_messages.push_back("    WARNING: BossGuard is disabled");
    }
}

// Injected BossGuard assembly function
void __declspec(naked) __stdcall asm_check()
{
    __asm
    {
        mov ebp, [esp + 0x8]

        cmp [ebp + 0x208], 0x186D20 //4 kings don't use EnableLogic
        je dont_check
        push eax
        mov eax, [ebp + 0x28]
        mov eax, [eax + 0xC0] // Load flag info for entity
        test eax, 1 // Bit 00000001 is EnableLogic for boss
        pop eax
        jz abort_damage // If result EnableLogic & 1 is 0, abort

        test byte ptr[ebp + 0x000003C4], 0x40
        dont_check:
        jmp check_return

        abort_damage:
        pop ebp
        retn 8
    }
}

} // namespace BossGuard
} // namespace AntiCheat

#undef DS1_AC_BOSS_GUARD_INJECTION_OFFSET
