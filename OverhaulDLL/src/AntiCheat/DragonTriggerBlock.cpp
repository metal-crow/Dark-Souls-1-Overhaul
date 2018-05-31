/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
*/


#include "AntiCheat.h"
#include "DllMain_Legacy.h"

namespace AntiCheat {
namespace DragonTriggerBlock {

bool active = false;
void *patch_address_head = NULL;
void *patch_address_body = NULL;
uint8_t original_bytes[1] = { 0x7F };
uint8_t  patched_bytes[1] = { 0x79 };

void enable()
{
    if (!Game::characters_loaded) {
        if (!print_console(Mod::output_prefix + "ERROR: Unable to start DragonTriggerBlock anti-cheat protection (data not yet loaded)"))
            Mod::startup_messages.push_back(Mod::output_prefix + "ERROR: Unable to start DragonTriggerBlock anti-cheat protection (data not yet loaded)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    // First time enabling; get patch addresses
    if (patch_address_head == NULL) {
        patch_address_head = aob_scan("60 00 00 00 00 FF 00 7F");
        if (patch_address_head == NULL) {
            if (!print_console(Mod::output_prefix + "ERROR: Unable to start DragonTriggerBlock anti-cheat protection (data not yet loaded)"))
                Mod::startup_messages.push_back(Mod::output_prefix + "ERROR: Unable to start DragonTriggerBlock anti-cheat protection (data not yet loaded)");
            SetLastError(ERROR_FILE_NOT_FOUND);
            return;
        } else {
            patch_address_head = (void*)(((uint32_t)patch_address_head) + 7);
        }
    }
    if (patch_address_body == NULL) {
        patch_address_body = aob_scan("A7 00 00 00 00 FF 00 7F");
        if (patch_address_body == NULL) {
            if (!print_console(Mod::output_prefix + "ERROR: Unable to start DragonTriggerBlock anti-cheat protection (data not yet loaded)"))
                Mod::startup_messages.push_back(Mod::output_prefix + "ERROR: Unable to start DragonTriggerBlock anti-cheat protection (data not yet loaded)");
            SetLastError(ERROR_FILE_NOT_FOUND);
            return;
        } else {
            patch_address_body = (void*)(((uint32_t)patch_address_body) + 7);
        }
    }
    if (!print_console(Mod::output_prefix + "Enabling DragonTriggerBlock anti-cheat protection"))
        Mod::startup_messages.push_back(Mod::output_prefix + "Enabling DragonTriggerBlock anti-cheat protection");
    apply_byte_patch(patch_address_head, patched_bytes, 1);
    apply_byte_patch(patch_address_body, patched_bytes, 1);
    active = true;
    SetLastError(ERROR_SUCCESS);
}

void disable()
{
    if (patch_address_head == NULL || patch_address_body == NULL) {
        if (!print_console(Mod::output_prefix + "Disabling DragonTriggerBlock anti-cheat protection"))
            Mod::startup_messages.push_back(Mod::output_prefix + "Disabling DragonTriggerBlock anti-cheat protection");
        active = false;
    }
    if (active) {
        if (!print_console(Mod::output_prefix + "Disabling DragonTriggerBlock anti-cheat protection"))
            Mod::startup_messages.push_back(Mod::output_prefix + "Disabling DragonTriggerBlock anti-cheat protection");
        apply_byte_patch(patch_address_head, original_bytes, 1);
        apply_byte_patch(patch_address_body, original_bytes, 1);
        active = false;
    }
}

void toggle()
{
    if (active) {
        disable();
    } else {
        enable();
    }
}

} // namespace DragonTriggerBlock
} // namespace AntiCheat
