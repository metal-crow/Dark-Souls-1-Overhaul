/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce        -  C++
*/


#include "AntiCheat.h"
#include "DllMain_Legacy.h"

namespace AntiCheat {
namespace BinocsTriggerBlock {

bool active = false;
void *patch_address = NULL;
uint8_t original_bytes[1] = { 0x7F };
uint8_t  patched_bytes[1] = { 0x79 };

void enable()
{
    if (!Game::characters_loaded) {
        if (!print_console(Mod::output_prefix + "ERROR: Unable to start BinocsTriggerBlock anti-cheat protection (data not yet loaded)"))
            Mod::startup_messages.push_back(Mod::output_prefix + "ERROR: Unable to start BinocsTriggerBlock anti-cheat protection (data not yet loaded)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    if (patch_address == NULL) {
        // First time enabling; get patch address
        patch_address = aob_scan("00 0F 00 00 00 00 FF 00 7F");
        if (patch_address == NULL) {
            if (!print_console(Mod::output_prefix + "ERROR: Unable to start BinocsTriggerBlock anti-cheat protection (data not yet loaded)"))
                Mod::startup_messages.push_back(Mod::output_prefix + "ERROR: Unable to start BinocsTriggerBlock anti-cheat protection (data not yet loaded)");
            SetLastError(ERROR_FILE_NOT_FOUND);
            return;
        } else {
            patch_address = (void*)(((uint32_t)patch_address) + 8);
        }
    }
    if (!print_console(Mod::output_prefix + "Enabling BinocsTriggerBlock anti-cheat protection"))
        Mod::startup_messages.push_back(Mod::output_prefix + "Enabling BinocsTriggerBlock anti-cheat protection");
    apply_byte_patch(patch_address, patched_bytes, 1);
    active = true;
    SetLastError(ERROR_SUCCESS);
}

void disable()
{
    if (patch_address == NULL) {
        if (!print_console(Mod::output_prefix + "Disabling BinocsTriggerBlock anti-cheat protection"))
            Mod::startup_messages.push_back(Mod::output_prefix + "Disabling BinocsTriggerBlock anti-cheat protection");
        active = false;
    }
    if (active) {
        if (!print_console(Mod::output_prefix + "Disabling BinocsTriggerBlock anti-cheat protection"))
            Mod::startup_messages.push_back(Mod::output_prefix + "Disabling BinocsTriggerBlock anti-cheat protection");
        apply_byte_patch(patch_address, original_bytes, 1);
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



} // namespace BinocsTriggerBlock
} // namespace AntiCheat
