#include "EquipmentUnlock.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t equipment_change_unlock_injection_return;
    void equipment_change_unlock_injection();
    bool equipment_change_unlock_injection_helper(InventorySlots menu_slot);
}

void EquipmentUnlock::start() {
    ConsoleWrite("Enabling Equipment Change Unlock...");

    //nop out the check which says if the menu is enterable or not
    uint8_t nop_patch1[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    uint8_t *write_address = (uint8_t*)(EquipmentUnlock::menu_entry_allowed_check_injection_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch1, sizeof(nop_patch1));

    //nop out the check if you can/can't remove an equipped item
    uint8_t nop_patch2[] = { 0x90, 0x90, 0x90, 0x90 };
    write_address = (uint8_t*)(EquipmentUnlock::disable_remove_equipment_check_injection_offset + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch2, sizeof(nop_patch2));

    //add our custom check that dertmines what you can change in the menu
    write_address = (uint8_t*)(EquipmentUnlock::equipment_change_unlock_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &equipment_change_unlock_injection_return, 2, &equipment_change_unlock_injection);
}

typedef bool ok_to_enter_equipment_menu_FUNC(void* playerins);
ok_to_enter_equipment_menu_FUNC* ok_to_enter_equipment_menu = (ok_to_enter_equipment_menu_FUNC*)0x140361a80;

//return true if the change is allowed, false otherwise
bool equipment_change_unlock_injection_helper(InventorySlots menu_slot)
{
    auto animation_mediator_o = Game::get_player_animationMediator();
    if (!animation_mediator_o.has_value())
    {
        ConsoleWrite("Warning: unable to get_player_animationMediator in %s\n", __FUNCTION__);
        return true;
    }
    auto animation_mediator = animation_mediator_o.value();

    auto state_anim = Game::get_animation_mediator_state_animation(animation_mediator, Upper_ThrowAnim);

    //if we're currently backstabbing, don't allow any swaps
    //(anim ids ending in 400 are backstab, 401 is hornet bs)
    if ((state_anim % 1000) == 400 || (state_anim % 1000) == 401)
    {
        return false;
    }

    //if we're currently riposting, don't allow any swaps
    if ((state_anim % 1000) == 201 || (state_anim % 1000) == 202 || (state_anim % 1000) == 203)
    {
        return false;
    }

    //if we're being backstabbed, allow ALL swaps (so weapon blocking can be done on getup)
    if (state_anim == 9000 || state_anim == 9420)
    {
        return true;
    }

    auto playerIns_o = Game::get_PlayerIns();
    if (!playerIns_o.has_value())
    {
        ConsoleWrite("Warning: unable to get_PlayerIns in %s\n", __FUNCTION__);
        return true;
    }
    auto playerIns = playerIns_o.value();

    //normally, allow all swaps (except weapons if the game would normally restrict all inventory edits)
    return (menu_slot > RightHand2 || ok_to_enter_equipment_menu(playerIns));
}
