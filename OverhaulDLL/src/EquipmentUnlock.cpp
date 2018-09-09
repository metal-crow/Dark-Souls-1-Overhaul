#include "EquipmentUnlock.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t add_check_equipment_unlock_injection_return;
    void main_equipment_unlock_injection();
}

void EquipmentUnlock::start() {
    global::cmd_out << Mod::output_prefix << "Enabling Equipment Change Unlock...\n";
    Mod::startup_messages.push_back("Enabling Equipment Change Unlock...");

    //nop out the check which says if the menu is enterable or not
    //save the result for later though
    uint8_t save_and_nop_patch[8] = {
        0x41, 0x88, 0xC7, // mov    r15b,al
        0x90, 0x90, 0x90, 0x90, 0x90 };
    uint8_t *write_address = (uint8_t*)(EquipmentUnlock::disable_equipment_unlock_injection_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, save_and_nop_patch, 8);

    //add in a later check which excludes changing weapons in animations
    write_address = (uint8_t*)(EquipmentUnlock::add_check_equipment_unlock_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &add_check_equipment_unlock_injection_return, 2, &main_equipment_unlock_injection);
}
