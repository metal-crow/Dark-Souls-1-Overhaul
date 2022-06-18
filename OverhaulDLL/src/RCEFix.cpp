#include "RCEFix.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t rce1_injection_return;
    void rce1_injection();
    uint32_t rce1_helper(uint32_t, wchar_t*);
}

void RCEFix::start()
{
    ConsoleWrite("Fixing RCE...");

    uint8_t *write_address = (uint8_t*)(RCEFix::rce1_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &rce1_injection_return, 1, &rce1_injection);
}

uint32_t rce1_helper(uint32_t offset, wchar_t* str)
{
    if (offset > 1022)
    {
        str[1022] = L'\0'; //null terminate the end of the string
        return 1022;
    }
    return offset;
}
