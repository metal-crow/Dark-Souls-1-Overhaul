/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++, Multi-Consume technique
*/


#include "MultiConsume.h"
#include "AnimationEdits.h"
#include "DllMain.h"


namespace MultiConsume {


void __stdcall intercept_item_consumption();
void __stdcall intercept_item_decrement();
void __stdcall intercept_dlg_result();
void __stdcall intercept_dlg_no_result();
//void __stdcall intercept_whiff_anim();
void __stdcall force_item_consumption();
void __stdcall soft_humanity_count_correction();
void __stdcall soft_soul_correction();
void __stdcall intercept_belt_quantity();
void __stdcall intercept_inventory_quantity();



uint32_t originalcode_consume_item_jne_address = 0xE53E5A;
uint32_t item_decrement_original_jle_address   = 0xC09765; //"DARKSOULS.exe"+0x809765
uint32_t dlg_no_result_call_address            = 0xC7F7F0; //"DARKSOULS.exe"+0x87F7F0


uint32_t skip_next_multiconsume_dialog = 0;
uint32_t force_consume_next_item       = 0;
uint32_t last_multiconsume_item        = 0;
uint32_t last_consumption_quantity     = 0;
//uint32_t skip_next_whiff_anim          = 0;
uint32_t item_quantity                 = 0;
uint32_t capture_next_dlg_result       = 0;
uint32_t consumed_from_inventory       = 0;


const static uint8_t soft_hum_correct_post_patch1_bytes[2] = { 0x74, 0x59 };
const static uint8_t soft_hum_correct_post_patch2_bytes[8] = { 0x5E, 0x5B, 0x83, 0xC4, 0x08, 0xC2, 0x04, 0x00 };


// Original bytes
const static uint8_t item_consumption_original_bytes[13]              = { 0x80, 0xB8, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x75, 0x0A, 0x8B, 0x54, 0x24, 0x14 };
const static uint8_t item_decrement_original_bytes[6]                 = { 0x03, 0xD6, 0x3B, 0xD0, 0x7E, 0x0C };
const static uint8_t dlg_result_original_bytes[6]                     = { 0x89, 0x81, 0xB4, 0x01, 0x00, 0x00 };
const static uint8_t dlg_no_result_original_bytes[5]                  = { 0xE8, 0xA4, 0xF9, 0xFF, 0xFF };
//const static uint8_t intercept_whiff_anim_original_bytes[6]           = { 0x89, 0x4E, 0x60, 0x8B, 0x50, 0x04 };
const static uint8_t force_item_consumption_original_bytes[6]         = { 0x08, 0x8B, 0xC4, 0x01, 0x00, 0x00 };
const static uint8_t soft_humanity_count_correction_original_bytes[5] = { 0x89, 0x46, 0x7C, 0x5E, 0x5B };
const static uint8_t soft_soul_correction_original_bytes[6]           = { 0x89, 0x86, 0x8C, 0x00, 0x00, 0x00 };
const static uint8_t intercept_belt_quantity_original_bytes[5]        = { 0x8B, 0x40, 0x04, 0x5E, 0xC3 };
const static uint8_t intercept_inventory_quantity_original_bytes[5]   = { 0x8B, 0x40, 0x04, 0x5E, 0xC3 };

const static uint8_t soft_hum_correct_post_patch1_original_bytes[2]   = { 0x74, 0x4E };
const static uint8_t soft_hum_correct_post_patch2_original_bytes[8]   = { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };


// Injection addresses
const static uint8_t* item_consumption_injection_point               = (uint8_t*)0xE53E47;
const static uint8_t* item_decrement_injection_point                 = (uint8_t*)0xC09753;
const static uint8_t* dlg_result_injection_point                     = (uint8_t*)0xC7F8E6;
const static uint8_t* dlg_no_result_injection_point                  = (uint8_t*)0xC7FE47;
//const static uint8_t* intercept_whiff_anim_injection_point           = (uint8_t*)0xE2B2CF;
const static uint8_t* force_item_consumption_injection_point         = (uint8_t*)0xE1DA05;
const static uint8_t* soft_humanity_count_correction_injection_point = (uint8_t*)0xE67A26;
const static uint8_t* soft_soul_correction_injection_point           = (uint8_t*)0xBFC4E2;
const static uint8_t* intercept_belt_quantity_injection_point        = (uint8_t*)0xC0AAA3;
const static uint8_t* intercept_inventory_quantity_injection_point   = (uint8_t*)0xC0AA80;

const static uint8_t* soft_hum_correct_post_patch_addr1 = (uint8_t*)0xE679D9; //"DARKSOULS.exe"+0xA679D9
const static uint8_t* soft_hum_correct_post_patch_addr2 = (uint8_t*)0xE67A34; //"DARKSOULS.exe"+0xA67A34


// Return addresses
uint32_t returnhere_item_consumption       = 0;
uint32_t returnhere_item_dec               = 0;
uint32_t returnhere_dlg_result             = 0;
uint32_t returnhere_dlg_no_result          = 0;
//uint32_t returnhere_disable_cant_use_anim  = 0;
uint32_t returnhere_force_item_consumption = 0;
uint32_t returnhere_humanity_correction    = 0;
uint32_t returnhere_soft_soul_correction   = 0;
uint32_t returnhere_belt_quantity          = 0;
uint32_t returnhere_inventory_quantity     = 0;



void apply(const std::string& output_prefix)
{
    if (!is_active())
    {
        print_console(output_prefix + "Enabling multi-consumption (EXPERIMENTAL)");

        skip_next_multiconsume_dialog = 0;
        force_consume_next_item = 0;
        last_multiconsume_item = 0;
        last_consumption_quantity = 0;
        item_quantity = 0;
        capture_next_dlg_result = 0;
        consumed_from_inventory = 0;

        set_mem_protection(const_cast<uint8_t*>(item_consumption_injection_point), 13, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(item_consumption_injection_point), &returnhere_item_consumption, 8, &intercept_item_consumption);

        set_mem_protection(const_cast<uint8_t*>(item_decrement_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(item_decrement_injection_point), &returnhere_item_dec, 1, &intercept_item_decrement);

        set_mem_protection(const_cast<uint8_t*>(dlg_result_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(dlg_result_injection_point), &returnhere_dlg_result, 1, &intercept_dlg_result);

        set_mem_protection(const_cast<uint8_t*>(dlg_no_result_injection_point), 5, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(dlg_no_result_injection_point), &returnhere_dlg_no_result, 0, &intercept_dlg_no_result);

        //set_mem_protection(const_cast<uint8_t*>(intercept_whiff_anim_injection_point), 6, MEM_PROTECT_RWX);
        //inject_jmp_5b(const_cast<uint8_t*>(intercept_whiff_anim_injection_point), &returnhere_disable_cant_use_anim, 1, &intercept_whiff_anim);
        AnimationEdits::skip_next_whiff_anim = 0;

        set_mem_protection(const_cast<uint8_t*>(force_item_consumption_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(force_item_consumption_injection_point), &returnhere_force_item_consumption, 1, &force_item_consumption);

        set_mem_protection(const_cast<uint8_t*>(soft_humanity_count_correction_injection_point), 5, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(soft_humanity_count_correction_injection_point), &returnhere_humanity_correction, 0, &soft_humanity_count_correction);

        set_mem_protection(const_cast<uint8_t*>(soft_soul_correction_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(soft_soul_correction_injection_point), &returnhere_soft_soul_correction, 1, &soft_soul_correction);

        set_mem_protection(const_cast<uint8_t*>(intercept_belt_quantity_injection_point), 5, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(intercept_belt_quantity_injection_point), &returnhere_belt_quantity, 0, &intercept_belt_quantity);

        set_mem_protection(const_cast<uint8_t*>(intercept_inventory_quantity_injection_point), 5, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(intercept_inventory_quantity_injection_point), &returnhere_inventory_quantity, 0, &intercept_inventory_quantity);

        apply_byte_patch(const_cast<uint8_t*>(soft_hum_correct_post_patch_addr1), soft_hum_correct_post_patch1_bytes, 2);
        apply_byte_patch(const_cast<uint8_t*>(soft_hum_correct_post_patch_addr2), soft_hum_correct_post_patch2_bytes, 8);
    }
}


void unpatch(const std::string& output_prefix)
{
    if (is_active())
    {
        print_console(output_prefix + "Disabling multi-consumption...");

        skip_next_multiconsume_dialog = 0;
        force_consume_next_item = 0;
        last_multiconsume_item = 0;
        last_consumption_quantity = 0;
        item_quantity = 0;
        capture_next_dlg_result = 0;
        consumed_from_inventory = 0;

        forget_injection((uint32_t)item_consumption_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(item_consumption_injection_point), item_consumption_original_bytes, 13);

        forget_injection((uint32_t)item_decrement_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(item_decrement_injection_point), item_decrement_original_bytes, 6);

        forget_injection((uint32_t)dlg_result_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(dlg_result_injection_point), dlg_result_original_bytes, 6);

        forget_injection((uint32_t)dlg_no_result_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(dlg_no_result_injection_point), dlg_no_result_original_bytes, 5);

        //forget_injection((uint32_t)intercept_whiff_anim_injection_point);
        //apply_byte_patch(const_cast<uint8_t*>(intercept_whiff_anim_injection_point), intercept_whiff_anim_original_bytes, 6);
        AnimationEdits::skip_next_whiff_anim = 0;

        forget_injection((uint32_t)force_item_consumption_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(force_item_consumption_injection_point), force_item_consumption_original_bytes, 6);

        forget_injection((uint32_t)soft_humanity_count_correction_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(soft_humanity_count_correction_injection_point), soft_humanity_count_correction_original_bytes, 5);

        forget_injection((uint32_t)soft_soul_correction_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(soft_soul_correction_injection_point), soft_soul_correction_original_bytes, 6);

        forget_injection((uint32_t)intercept_belt_quantity_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(intercept_belt_quantity_injection_point), intercept_belt_quantity_original_bytes, 5);

        forget_injection((uint32_t)intercept_inventory_quantity_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(intercept_inventory_quantity_injection_point), intercept_inventory_quantity_original_bytes, 5);

        apply_byte_patch(const_cast<uint8_t*>(soft_hum_correct_post_patch_addr1), soft_hum_correct_post_patch1_original_bytes, 2);
        apply_byte_patch(const_cast<uint8_t*>(soft_hum_correct_post_patch_addr2), soft_hum_correct_post_patch2_original_bytes, 8);
    }
}


bool is_active()
{
    return (*((uint8_t*)item_consumption_injection_point)) != 0x80;
}



// Main code interception when player uses an item
void __declspec(naked) __stdcall intercept_item_consumption()
{
    __asm
    {
        //multiconsume_main:
        // Check whether to open multiconsume dialog
        cmp [skip_next_multiconsume_dialog], 1
        je overwrite_belt_item_data

        // Determine item type
        // Humanity variants
        cmp dword ptr [esp+0x14], 0x1F4 // 500 = Humanity item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x1F5 // 501 = Twin Humanities item ID
        je multiconsume_item
        // Soul variants
        cmp dword ptr [esp+0x14], 0x190 // 400 = Soul of a Lost Undead item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x191 // 401 = Large Soul of a Lost Undead item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x192 // 402 = Soul of a Nameless Soldier item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x193 // 403 = Large Soul of a Nameless Soldier item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x194 // 404 = Soul of a Proud Knight item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x195 // 405 = Large Soul of a Proud Knight item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x196 // 406 = Soul of a Brave Warrior item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x197 // 407 = Large Soul of a Brave Warrior item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x198 // 408 = Soul of a Hero item ID
        je multiconsume_item
        cmp dword ptr [esp+0x14], 0x199 // 409 = Soul of a Great Hero item ID
        je multiconsume_item

        jmp originalcode_consume_item


        // Not using a multi-consumable item
        originalcode_consume_item:
        cmp byte ptr [eax+0x000000C7], 0
        jne originalcode_consume_item_jne_lbl
        mov edx,[esp+0x14]
        jmp exit_item_consumption
        originalcode_consume_item_jne_lbl:
        jmp originalcode_consume_item_jne_address //"DARKSOULS.exe"+0xA53E5A


        // Player used a multi-consume item
        multiconsume_item:
        // Store multiconsume item ID
        push dword ptr [esp+0x14]
        pop [last_multiconsume_item]
        // Check whether to open multiconsume dialog
        cmp [skip_next_multiconsume_dialog], 1
        je skip_multiconsume_dialog
        // Delay use of item until quantity is determined via numberpicker dialog
        mov dword ptr [esp+0x14], 0
        // Skip next whiff animation (caused by item-use delay)
        mov [AnimationEdits::skip_next_whiff_anim], 1
        jmp show_multiconsume_dialog


        // Overwrite active belt item ID with selected inventory item ID
        overwrite_belt_item_data:
        push eax // Offsets the stack by +4; item ID is now in [esp+0x18] instead of [esp+0x14]
        mov eax, [last_multiconsume_item]
        mov [esp+0x18], eax
        pop eax
        jmp skip_multiconsume_dialog

        skip_multiconsume_dialog:
        mov [skip_next_multiconsume_dialog], 0
        jmp originalcode_consume_item

        show_multiconsume_dialog:
        push eax
        push ecx
        push edx
        // Set number picker dialog menu content
        mov edx, 0x12E33E0 //"DARKSOULS.exe"+0xEE33E0
        mov dword ptr [edx], 0x468D // Dialog message ID (18061 = "Offer your humanity?")
        // If using a soul (rather than a humanity), change dialog message
        cmp [last_multiconsume_item], 0x1F4 // 500 = Humanity item ID
        je dont_use_soul_dialog_msg
        cmp [last_multiconsume_item], 0x1F5 // 501 = Twin Humanities item ID
        je dont_use_soul_dialog_msg
        mov edx, 0x12E33E0 //"DARKSOULS.exe"+0xEE33E0
        mov dword ptr [edx], 0x4E34 // Dialog message ID (20020 = "Confirm reinforcement") // @TODO: Fix placeholder message
        //mov ["DARKSOULS.exe"+0xEE33E0], ?? // Dialog message ID (?? = "??")
        dont_use_soul_dialog_msg:
        mov edx, 0x12E33E4 //"DARKSOULS.exe"+0xEE33E4
        mov dword ptr [edx], 1 // Left button string ID ("OK")
        mov edx, 0x12E33E8 //"DARKSOULS.exe"+0xEE33E8
        mov dword ptr [edx], 2 // Right button string ID ("CANCEL")
        mov eax, 0x13786D0 //"DARKSOULS.exe"+0x00F786D0
        mov eax, [eax]
        mov ecx, item_quantity
        mov dword ptr [eax+0x208], 1  // Set min number picker value
        mov dword ptr [eax+0x20C], 1  // Set default number picker value
        // Set max number picker value
        mov [eax+0x204], ecx  // [eax+0x204] = total number of selected item in inventory

        // Check for humanity edge cases (soft humanity + hard humanities > 99, etc.)
        cmp [last_multiconsume_item], 0x1F4 // 500 = Humanity item ID
        jne not_showing_humanity_dlg
        // Humanity used
        // Get current soft humanity count
        mov ecx, 0x1378700 //"DARKSOULS.exe"+0xF78700
        mov ecx, [ecx]
        mov ecx, [ecx+8]
        mov ecx, [ecx+0x7C]
        // Check if player already has 98 or 99 humanity
        cmp ecx, 0x62 // 98
        jl using_humanity_but_not_maxed
        mov dword ptr [eax+0x204], 1
        jmp finalize_multiconsume_dialog
        using_humanity_but_not_maxed:
        // Determine maximum quantity that can be used
        mov edx, 0x63 // 99 = maximum soft humanity
        sub edx, ecx
        push eax
        mov eax, item_quantity
        cmp edx, eax
        pop eax
        jge finalize_multiconsume_dialog
        mov [eax+0x204], edx // Quantity of Humanity in inventory exceeds remaining humanity point availability; decrease maximum
        jmp finalize_multiconsume_dialog
        not_showing_humanity_dlg:

        cmp [last_multiconsume_item], 0x1F5 // 501 = Twin Humanities item ID
        jne not_showing_twin_humanity_dlg
        // Twin Humanities used
        // Get current soft humanity count
        mov ecx, 0x1378700 //"DARKSOULS.exe"+0xF78700
        mov ecx, [ecx]
        mov ecx, [ecx+8]
        mov ecx, [ecx+0x7C]
        // Check if player already has max humanity (97, 98, or 99 when using Twin Humanity)
        cmp ecx, 0x61 // 97
        jl using_twin_humanity_but_not_maxed
        mov dword ptr [eax+0x204], 1
        jmp finalize_multiconsume_dialog
        using_twin_humanity_but_not_maxed:
        // Determine maximum quantity that can be used
        mov edx, 0x63 // 99 = maximum soft humanity
        sub edx, ecx
        shr edx, 1 // Divide by 2 because Twin Humanities award 2 points each
        cmp edx, 0
        jg max_twin_humanities_not_zero_multiconsume_dlg
        mov edx, 1
        max_twin_humanities_not_zero_multiconsume_dlg:
        mov ecx, item_quantity
        cmp edx, ecx
        jge finalize_multiconsume_dialog
        mov [eax+0x204], edx // Quantity of Twin Humanity in inventory would award too many points; decrease maximum
        jmp finalize_multiconsume_dialog
        not_showing_twin_humanity_dlg:

        // Check if player used a soul variant
        cmp [last_multiconsume_item], 0x190 // 400 = Soul of a Lost Undead item ID
        je check_lost_undead_soul_use_limit
        cmp [last_multiconsume_item], 0x191 // 401 = Large Soul of a Lost Undead item ID
        je check_lost_undead_large_soul_use_limit
        cmp [last_multiconsume_item], 0x192 // 402 = Soul of a Nameless Soldier item ID
        je check_nameless_soldier_soul_use_limit
        cmp [last_multiconsume_item], 0x193 // 403 = Large Soul of a Nameless Soldier item ID
        je check_nameless_soldier_large_soul_use_limit
        cmp [last_multiconsume_item], 0x194 // 404 = Soul of a Proud Knight item ID
        je check_proud_knight_soul_use_limit
        cmp [last_multiconsume_item], 0x195 // 405 = Large Soul of a Proud Knight item ID
        je check_proud_knight_large_soul_use_limit
        cmp [last_multiconsume_item], 0x196 // 406 = Soul of a Brave Warrior item ID
        je check_brave_warrior_soul_use_limit
        cmp [last_multiconsume_item], 0x197 // 407 = Large Soul of a Brave Warrior item ID
        je check_brave_warrior_large_soul_use_limit
        cmp [last_multiconsume_item], 0x198 // 408 = Soul of a Hero item ID
        je check_hero_soul_use_limit
        cmp [last_multiconsume_item], 0x199 // 409 = Soul of a Great Hero item ID
        je check_hero_large_soul_use_limit

        jmp finalize_multiconsume_dialog

        // Player used a soul variant
        // Helpers for storing soul variant reward value in ebx:
        check_lost_undead_soul_use_limit:
        push ebx
        mov ebx, 0xC8 // 200 = Soul of a Lost Undead reward
        jmp check_soul_variant_use_limit
        check_lost_undead_large_soul_use_limit:
        push ebx
        mov ebx, 0x190 // 400 = Large Soul of a Lost Undead reward
        jmp check_soul_variant_use_limit
        check_nameless_soldier_soul_use_limit:
        push ebx
        mov ebx, 0x320 // 800 = Soul of a Nameless Soldier reward
        jmp check_soul_variant_use_limit
        check_nameless_soldier_large_soul_use_limit:
        push ebx
        mov ebx, 0x3E8 // 1000 = Large Soul of a Nameless Soldier reward
        jmp check_soul_variant_use_limit
        check_proud_knight_soul_use_limit:
        push ebx
        mov ebx, 0x7D0 // 2000 = Soul of a Proud Knight reward
        jmp check_soul_variant_use_limit
        check_proud_knight_large_soul_use_limit:
        push ebx
        mov ebx, 0xBB8 // 3000 = Large Soul of a Proud Knight reward
        jmp check_soul_variant_use_limit
        check_brave_warrior_soul_use_limit:
        push ebx
        mov ebx, 0x1388 // 5000 = Soul of a Brave Warrior reward
        jmp check_soul_variant_use_limit
        check_brave_warrior_large_soul_use_limit:
        push ebx
        mov ebx, 0x1F40 // 8000 = Large Soul of a Brave Warrior reward
        jmp check_soul_variant_use_limit
        check_hero_soul_use_limit:
        push ebx
        mov ebx, 0x2710 // 10000 = Soul of a Hero reward
        jmp check_soul_variant_use_limit
        check_hero_large_soul_use_limit:
        push ebx
        mov ebx, 0x4E20 // 20000 = Soul of a Great Hero reward
        jmp check_soul_variant_use_limit

        // Generic check for hard soul limits (can't exceed 0x3B9AC9FF aka 999999999 souls)
        check_soul_variant_use_limit:
        // ebx = soul variant reward value
        // Get current soul count
        mov ecx, 0x1378700 //"DARKSOULS.exe"+0xF78700
        mov ecx, [ecx]
        mov ecx, [ecx+8]
        mov ecx, [ecx+0x8C] // ecx = current soul count
        // Check if player already has max souls
        mov edx, 0x3B9AC9FF // 999999999 = max souls
        sub edx, ebx
        cmp ecx, edx
        jl using_soul_variant_but_not_maxed
        mov dword ptr [eax+0x204], 1 // Player has max souls; limit usage to avoid wasting them
        jmp cleanup_soul_variant_check
        using_soul_variant_but_not_maxed:
        // Determine maximum quantity that can be used
        mov edx, 0x3B9AC9FF // 999999999 = max souls
        sub edx, ecx
        // Divide edx by reward value (edx = edx/ebx)
        push eax
        // Note: div instruction reads and alters edx
        mov eax, edx
        mov edx, 0
        div ebx // eax = edx/ebx
        mov edx, eax
        pop eax
        cmp edx, 0
        jg max_soul_variant_not_zero_multiconsume_dlg
        mov edx, 1
        max_soul_variant_not_zero_multiconsume_dlg:
        mov ecx, item_quantity
        cmp edx, ecx
        jge cleanup_soul_variant_check
        mov [eax+0x204], edx // Reward value exceeds limit; decrease max allowed usage
        cleanup_soul_variant_check:
        pop ebx

        jmp finalize_multiconsume_dialog


        finalize_multiconsume_dialog:
        // Capture next dialog result
        mov [capture_next_dlg_result], 1

        // Show number picker dialog
        mov dword ptr [eax+0x60], 3
        pop edx
        pop ecx
        pop eax

        // Restore original values of flag registers to avoid interrupting normal execution
        cmp byte ptr [eax+0x000000C7], 0
        mov edx,[esp+0x14]
        jmp exit_item_consumption

        exit_item_consumption:
        jmp returnhere_item_consumption
    }
}


// Item count decrement intercept
void __declspec(naked) __stdcall intercept_item_decrement() //check for esi > 0
{
    __asm
    {
        // Check for valid multi-use item ID
        // Humanity variants
        //item_decrement_intercept:
        cmp dword ptr [ecx+4], 0x1F4 // 500 = Humanity item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x1F5 // 501 = Twin Humanities item ID
        je adjust_item_dec
        // Soul variants
        cmp dword ptr [ecx+4], 0x190 // 400 = Soul of a Lost Undead item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x191 // 401 = Large Soul of a Lost Undead item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x192 // 402 = Soul of a Nameless Soldier item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x193 // 403 = Large Soul of a Nameless Soldier item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x194 // 404 = Soul of a Proud Knight item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x195 // 405 = Large Soul of a Proud Knight item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x196 // 406 = Soul of a Brave Warrior item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x197 // 407 = Large Soul of a Brave Warrior item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x198 // 408 = Soul of a Hero item ID
        je adjust_item_dec
        cmp dword ptr [ecx+4], 0x199 // 409 = Soul of a Great Hero item ID
        je adjust_item_dec

        jmp normal_item_count_dec

        // Check how many items the player consumed
        adjust_item_dec:
        //push esi
        //mov esi, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        //cmp dword ptr [esi], 0xFFFFFFFF // -1
        //pop esi
        //je normal_item_count_dec
        // If item count is being incremented, continue with normal execution
        push esi
        shr esi, 0x1F // 31
        cmp esi, 0
        pop esi
        je normal_item_count_dec
        push esi
        mov esi, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        cmp dword ptr [esi], 0
        pop esi
        je normal_item_count_dec
        push esi
        mov esi, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        cmp dword ptr [esi], 1
        pop esi
        je normal_item_count_dec
        // Multiple items consumed; prepare adjustments
        push esi
        // Decrement the chosen quantity
        mov esi, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        mov esi, [esi]
        sub edx, esi
        pop esi
        // ?? Can't remember what the purpose of this was
        //// If a humanity type was used, nothing else needs to be done
        //cmp [ecx+4], 0x1F4 // 500 = Humanity item ID
        //je exit_item_dec

        jmp exit_item_dec

        normal_item_count_dec:
        add edx, esi
        jmp exit_item_dec

        exit_item_dec:
        mov [consumed_from_inventory], 0
        cmp edx, eax
        jle item_decrement_original_jle_lbl
        jmp returnhere_item_dec
        item_decrement_original_jle_lbl:
        jmp item_decrement_original_jle_address
    }
}


// Interception of number picker dialog callback function (if a button is pressed)
void __declspec(naked) __stdcall intercept_dlg_result()
{
    __asm
    {
        //dlg_result_intercept:
        // Original code
        mov [ecx+0x000001B4], eax

        // Check whether to capture this result
        cmp [capture_next_dlg_result], 0
        je exit_dlg_result

        // Check last-used item ID
        // Humanity variants
        cmp dword ptr [last_multiconsume_item], 0x1F4 // 500 = Humanity item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x1F5 // 501 = Twin Humanities item ID
        je do_capture_dlg_result
        // Soul variants
        cmp dword ptr [last_multiconsume_item], 0x190 // 400 = Soul of a Lost Undead item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x191 // 401 = Large Soul of a Lost Undead item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x192 // 402 = Soul of a Nameless Soldier item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x193 // 403 = Large Soul of a Nameless Soldier item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x194 // 404 = Soul of a Proud Knight item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x195 // 405 = Large Soul of a Proud Knight item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x196 // 406 = Soul of a Brave Warrior item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x197 // 407 = Large Soul of a Brave Warrior item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x198 // 408 = Soul of a Hero item ID
        je do_capture_dlg_result
        cmp dword ptr [last_multiconsume_item], 0x199 // 409 = Soul of a Great Hero item ID
        je do_capture_dlg_result


        // Invalid item; return without changing flags
        jmp returnhere_dlg_result


        do_capture_dlg_result:
        // Check if the dialog was cancelled
        cmp eax, 1
        jne dialog_cancelled

        // Capture current dialog result
        push eax
        mov eax, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        mov eax, dword ptr [eax]
        // Check for valid quantity
        cmp eax, 0xFFFFFFFF
        jne not_neg_dlg_res
        mov eax, 0
        not_neg_dlg_res:
        mov [last_consumption_quantity], eax
        pop eax
        // Don't capture next dialog result
        mov [capture_next_dlg_result], 0
        mov [skip_next_multiconsume_dialog], 1
        mov [force_consume_next_item], 1
        push eax
        push ebx
        // Update last-used item ID
        mov eax, last_multiconsume_item
        mov ebx, 0x12E29E8 //"DARKSOULS.exe"+0x00EE29E8
        mov ebx, [ebx]
        mov ebx, [ebx]
        mov [ebx+0x628], eax
        mov [ebx+0x1D8], eax
        pop ebx
        pop eax

        jmp exit_dlg_result

        dialog_cancelled:
        mov [skip_next_multiconsume_dialog], 0
        mov [force_consume_next_item], 0
        mov [consumed_from_inventory], 0
        mov [last_multiconsume_item], 0
        mov [last_consumption_quantity], 0
        jmp exit_dlg_result


        exit_dlg_result:
        // Don't capture next dialog result
        mov [capture_next_dlg_result], 0
        mov [AnimationEdits::skip_next_whiff_anim], 0
        jmp returnhere_dlg_result
    }
}


// Interception of dialog callback function (if cancelled and neither button is pressed)
void __declspec(naked) __stdcall intercept_dlg_no_result()
{
    __asm
    {
        //dlg_no_result_intercept:
        mov [skip_next_multiconsume_dialog], 0
        mov [force_consume_next_item], 0
        mov [capture_next_dlg_result], 0
        mov [AnimationEdits::skip_next_whiff_anim], 0
        mov [consumed_from_inventory], 0
        mov [last_multiconsume_item], 0
        mov [last_consumption_quantity], 0
        call dlg_no_result_call_address //0xC7F7F0 //"DARKSOULS.exe"+0x87F7F0 // Original instruction
        jmp returnhere_dlg_no_result
    }
}


// Temporarily disable "Can't use item" animation from playing
//   (triggered by the technique used to delay item usage until
//    after quantity is selected)
//void __declspec(naked) __stdcall intercept_whiff_anim()
//{
//    __asm
//    {
//        //disable_cant_use_anim:
//        cmp [skip_next_whiff_anim], 0
//        je originalcode_disable_cant_use_anim
//        cmp ecx, 0x1D56 // 7510 = "Can't use item"/"whiff" animation ID
//        jne originalcode_disable_cant_use_anim
//        // Skip this whiff animation
//        mov [skip_next_whiff_anim], 0
//        jmp exit_disable_cant_use_anim
//        originalcode_disable_cant_use_anim:
//        mov [esi+0x60],ecx
//        exit_disable_cant_use_anim:
//        mov edx,[eax+4]
//        jmp returnhere_disable_cant_use_anim
//    }
//}


// Force player character to re-activate multi-consumption item after selecting quantity to use
void __declspec(naked) __stdcall force_item_consumption()
{
    __asm
    {
        //force_item_consumption:
        cmp [force_consume_next_item], 0
        je originalcode_force_item_consumption
        mov [force_consume_next_item], 0
        // ?? Next 2 lines seem to be useless; can't remember what they were for
        //cmp [last_multiconsume_item], 0
        //je originalcode_force_item_consumption

        // Update last-used item ID
        push eax
        push ebx
        mov eax, last_multiconsume_item
        mov ebx, 0x12E29E8 //"DARKSOULS.exe"+0x00EE29E8
        mov ebx, [ebx]
        mov ebx, [ebx]
        mov [ebx+0x628], eax
        mov [ebx+0x1D8], eax
        pop ebx
        pop eax

        // Forcibly activate the item
        mov ecx, 0x12E29E8 //"DARKSOULS.exe"+0x00EE29E8
        mov ecx, [ecx]
        mov ecx, [ecx]
        mov cl, 1

        originalcode_force_item_consumption:
        or [ebx+0x000001C4], cl // Original code
        jmp returnhere_force_item_consumption
    }
}


// Adjustment for soft humanity absorption
void __declspec(naked) __stdcall soft_humanity_count_correction()
{
    __asm
    {
        //soft_humanity_count_correction:
        // Check if humanity count is being increased
        cmp ebx, 1
        jl exit_humanity_correction
        push ecx
        push edx
        // Get last-used item ID
        mov ecx, 0x12E29E8 //"DARKSOULS.exe"+0x00EE29E8
        mov ecx, [ecx]
        mov ecx, [ecx]
        mov ecx, [ecx+0x628]
        // Get current soft humanity count
        mov edx, 0x1378700 //"DARKSOULS.exe"+0xF78700
        mov edx, [edx]
        mov edx, [edx+8]
        mov edx, [edx+0x7C]
        // Check item ID
        cmp ecx, 0x1F4 // 500 = Humanity item ID
        je single_humanity_correction
        cmp ecx, 0x1F5 // 501 = Twin Humanities item ID
        je twin_humanity_correction
        jmp cleanup_humanity_correction

        single_humanity_correction:
        mov ecx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400 // Number of humanities absorbed
        mov ecx, [ecx]
        mov ebx, ecx
        mov eax, edx
        add eax, ecx
        cmp eax, 0x63 // 99 = maximum humanity points
        jle cleanup_humanity_correction
        mov eax, 0x63 // If eax > 99, set eax = 99
        jmp cleanup_humanity_correction

        twin_humanity_correction:
        mov ecx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400 // Number of twin humanities absorbed
        mov ecx, [ecx]
        shl ecx, 1 // Multiply by 2
        mov ebx, ecx
        mov eax, edx
        add eax, ecx
        cmp eax, 0x63 // 99 = maximum humanity points
        jle cleanup_humanity_correction
        mov eax, 0x63 // If eax > 99, set eax = 99
        jmp cleanup_humanity_correction

        cleanup_humanity_correction:
        pop edx
        pop ecx

        exit_humanity_correction:
        // If using a Humanity variant, clear last-used data
        cmp [last_multiconsume_item], 0x1F4 // 500 = Humanity item ID
        je clear_data_humanity_correction
        cmp [last_multiconsume_item], 0x1F5 // 501 = Twin Humanities item ID
        je clear_data_humanity_correction
        jmp dont_clear_data_humanity_correction
        clear_data_humanity_correction:
        mov [last_multiconsume_item], 0
        mov [last_consumption_quantity], 0
        dont_clear_data_humanity_correction:
        mov [esi+0x7C], eax
        pop esi
        pop ebx
        jmp returnhere_humanity_correction
    }
}


// Soul count adjustment
void __declspec(naked) __stdcall soft_soul_correction()
{
    __asm
    {
        //soft_soul_correction:
        cmp dword ptr [last_multiconsume_item], 0
        je originalcode_soft_soul_correction
        cmp dword ptr [last_consumption_quantity], 1
        je originalcode_soft_soul_correction
        // If using a Humanity variant, proceed with normal execution
        cmp dword ptr [last_multiconsume_item], 0x1F4 // 500 = Humanity item ID
        je originalcode_soft_soul_correction
        cmp dword ptr [last_multiconsume_item], 0x1F5 // 501 = Twin Humanities item ID
        je originalcode_soft_soul_correction
        // Check for valid soul variant ID and reward value
        cmp dword ptr [last_multiconsume_item], 0x190 // 400 = Soul of a Lost Undead item ID
        je check_for_lost_undead_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x191 // 401 = Large Soul of a Lost Undead item ID
        je check_for_lost_undead_large_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x192 // 402 = Soul of a Nameless Soldier item ID
        je check_for_nameless_soldier_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x193 // 403 = Large Soul of a Nameless Soldier item ID
        je check_for_nameless_soldier_large_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x194 // 404 = Soul of a Proud Knight item ID
        je check_for_proud_knight_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x195 // 405 = Large Soul of a Proud Knight item ID
        je check_for_proud_knight_large_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x196 // 406 = Soul of a Brave Warrior item ID
        je check_for_brave_warrior_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x197 // 407 = Large Soul of a Brave Warrior item ID
        je check_for_brave_warrior_large_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x198 // 408 = Soul of a Hero item ID
        je check_for_hero_soul_reward
        cmp dword ptr [last_multiconsume_item], 0x199 // 409 = Soul of a Great Hero item ID
        je check_for_hero_large_soul_reward

        jmp originalcode_soft_soul_correction

        // Valid item ID and reward value; make adjustments
        do_soft_soul_correction:
        push ebx
        push edx // edx is changed by mul instruction
        mov ebx, eax // ebx = old soul count + base reward value
        mov eax, ebp // eax = base reward value
        mul [last_consumption_quantity] // eax = adjusted reward value // @TODO: Is this line correct?
        mov edx, ebp // edx = base reward value
        mov ebp, eax // ebp = adjusted reward value
        mov eax, ebx // eax = old soul count + base reward value
        sub eax, edx // eax = old soul count
        add eax, ebp // eax = final adjusted soul count
        pop edx
        pop ebx
        // Check that soul count doesn't exceed maximum
        cmp eax, 0x3B9AC9FF // 999999999 = maximum souls
        jle cleanup_soft_soul_correction
        mov eax, 0x3B9AC9FF // If eax > 999999999, set eax = 999999999
        jmp cleanup_soft_soul_correction

        cleanup_soft_soul_correction:
        mov [last_multiconsume_item], 0
        mov [last_consumption_quantity], 0

        originalcode_soft_soul_correction:
        // Original instruction
        //   (eax = new soul count and ebp = number of souls that were just received)
        mov [esi+0x0000008C], eax
        jmp returnhere_soft_soul_correction


        // Soul count adjustment helpers
        // Soul of a Lost Undead
        check_for_lost_undead_soul_reward:
        cmp ebp, 0xC8 // 200 = Soul of a Lost Undead reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Large Soul of a Lost Undead
        check_for_lost_undead_large_soul_reward:
        cmp ebp, 0x190 // 400 = Large Soul of a Lost Undead reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Soul of a Nameless Soldier
        check_for_nameless_soldier_soul_reward:
        cmp ebp, 0x320 // 800 = Soul of a Nameless Soldier reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Large Soul of a Nameless Soldier
        check_for_nameless_soldier_large_soul_reward:
        cmp ebp, 0x3E8 // 1000 = Large Soul of a Nameless Soldier reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Soul of a Proud Knight
        check_for_proud_knight_soul_reward:
        cmp ebp, 0x7D0 // 2000 = Soul of a Proud Knight reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Large Soul of a Proud Knight
        check_for_proud_knight_large_soul_reward:
        cmp ebp, 0xBB8 // 3000 = Large Soul of a Proud Knight reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Soul of a Brave Warrior
        check_for_brave_warrior_soul_reward:
        cmp ebp, 0x1388 // 5000 = Soul of a Brave Warrior reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Large Soul of a Brave Warrior
        check_for_brave_warrior_large_soul_reward:
        cmp ebp, 0x1F40 // 8000 = Large Soul of a Brave Warrior reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Soul of a Hero
        check_for_hero_soul_reward:
        cmp ebp, 0x2710 // 10000 = Soul of a Hero reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Soul of a Great Hero
        check_for_hero_large_soul_reward:
        cmp ebp, 0x4E20 // 20000 = Soul of a Great Hero reward
        jne originalcode_soft_soul_correction
        jmp do_soft_soul_correction
        // Not a valid item ID/reward pair
        jmp originalcode_soft_soul_correction
    }
}


// Intercept code that accesses active belt item quantity
void __declspec(naked) __stdcall intercept_belt_quantity()
{
    __asm
    {
        //belt_quantity_intercept:
        // Check if inventory is open
        push ebx
        mov ebx, 0x13786D0 //"DARKSOULS.exe"+0x00F786D0
        mov ebx, [ebx]
        cmp dword ptr [ebx+0x24], 0
        jne cleanup_belt_quantity
        cmp dword ptr [ebx+0x60], 0
        jne cleanup_belt_quantity
        // Check if item is being consumed from player's inventory
        cmp dword ptr [consumed_from_inventory], 0
        jne cleanup_belt_quantity
        // Save item quantity
        mov ebx, [eax+8]
        mov [item_quantity], ebx
        cleanup_belt_quantity:
        pop ebx
        mov eax, [eax+4]
        pop esi
        ret
        jmp returnhere_belt_quantity // Probably unnecessary
    }
}


// Intercept code that accesses highlighted inventory item quantity
void __declspec(naked) __stdcall intercept_inventory_quantity()
{
    __asm
    {
        //inventory_quantity_intercept:
        // Save item quantity
        push ebx
        mov ebx, [eax+8]
        mov [item_quantity], ebx
        cmp [last_multiconsume_item], 0
        je skip_inventory_consumption_flag
        // Set flag to lock item quantity
        mov [consumed_from_inventory], 1
        skip_inventory_consumption_flag:
        pop ebx
        mov eax, [eax+4]
        pop esi
        ret
        jmp returnhere_inventory_quantity // Probably unnecessary
    }
}


} // namespace MultiConsume

