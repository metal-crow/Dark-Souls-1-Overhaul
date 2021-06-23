#include "FixAnkles.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t main_fix_ankles_injection_return;
    void main_fix_ankles_injection();
    void main_fix_ankles_helper(uint64_t parentfootIk);
}

void FixAnkles::start() {
    ConsoleWrite("Enabling Ankle fix...");

    //injection to change the foot ik type
    uint8_t *write_address = (uint8_t*)(FixAnkles::set_foot_ikvalue_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &main_fix_ankles_injection_return, 1, &main_fix_ankles_injection);
}

void main_fix_ankles_helper(uint64_t parentfootIk) {
    uint64_t id = *(uint64_t*)(*(uint64_t*)(*(uint64_t*)(*(uint64_t*)(*(uint64_t*)(parentfootIk) + 0x48) + 0x30 + 0x20) + 0x20));
    uint8_t* footik_enabled = (uint8_t*)(parentfootIk + 0x180);

    switch (id) {
        //1h rolls
        case 32:
        case 33:
        case 34:
        case 35:
        //2h rolls
        case 94:
        case 95:
        case 96:
        case 97:
            *footik_enabled = 0;
            return;
        default:
            *footik_enabled = 1;
            return;
    }
}
