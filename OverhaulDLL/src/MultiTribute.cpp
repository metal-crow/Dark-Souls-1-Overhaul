/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++, Multi-Tribute technique
*/


#include "MultiTribute.h"
#include "DllMain.h"


namespace MultiTribute {


void __stdcall intercept_tribute_humanity();
void __stdcall intercept_dec_humanity();
void __stdcall intercept_dec_tribute_item();
void __stdcall intercept_show_dialog();
void __stdcall intercept_inventory_item_id_read();
void __stdcall intercept_darkwraith_inc();
void __stdcall intercept_chaos_servant_inc();
void __stdcall intercept_sunlight_warrior_inc();
void __stdcall intercept_darkmoon_inc();
void __stdcall intercept_path_of_the_dragon_inc();
void __stdcall intercept_gravelord_inc();


uint32_t change_next_dlg_to_number_picker = 0;
uint32_t tmp_last_talk        = 0;
uint32_t adjust_tribute_dec   = 0;
uint32_t sunlight_medal_count = 0;
uint32_t dragon_scale_count   = 0;
uint32_t souvenir_count       = 0;
uint32_t eye_of_death_count   = 0;


uint32_t dialog_msg_id = 0x12E33E0;
uint32_t inventory_item_read_orig_je_address = 0xC0AD20;


// Original bytes
const static uint8_t tribute_humanity_original_bytes[7]       = { 0xFF, 0x24, 0x95, 0x48, 0xB1, 0xD4, 0x00 };
const static uint8_t dec_humanity_original_bytes[7]           = { 0xFF, 0x24, 0x95, 0x9C, 0xB2, 0xD4, 0x00 };
const static uint8_t dec_tribute_item_original_bytes[5]       = { 0x89, 0x51, 0x08, 0x8B, 0xC2 };
const static uint8_t show_dialog_original_bytes[5]            = { 0x89, 0x51, 0x60, 0x59, 0xC3 };
const static uint8_t inventory_item_id_read_original_bytes[5] = { 0x39, 0x79, 0x04, 0x74, 0x0B };
const static uint8_t darkwraith_inc_original_bytes[6]         = { 0x88, 0x91, 0xE6, 0x00, 0x00, 0x00 };
const static uint8_t chaos_servant_inc_original_bytes[6]      = { 0x88, 0x81, 0xEB, 0x00, 0x00, 0x00 };
const static uint8_t sunlight_warrior_inc_original_bytes[6]   = { 0x88, 0x81, 0xE5, 0x00, 0x00, 0x00 };
const static uint8_t darkmoon_inc_original_bytes[6]           = { 0x88, 0x91, 0xEA, 0x00, 0x00, 0x00 };
const static uint8_t path_of_the_dragon_inc_original_bytes[6] = { 0x88, 0x81, 0xE7, 0x00, 0x00, 0x00 };
const static uint8_t gravelord_inc_original_bytes[6]          = { 0x88, 0x91, 0xE8, 0x00, 0x00, 0x00 };


// Injection addresses
const static uint8_t* tribute_humanity_injection_point       = (uint8_t*)0xD4B03B;
const static uint8_t* dec_humanity_injection_point           = (uint8_t*)0xD4B1F5;
const static uint8_t* dec_tribute_item_injection_point       = (uint8_t*)0xC09766;
const static uint8_t* show_dialog_injection_point            = (uint8_t*)0xC7EE41;
const static uint8_t* inventory_item_id_read_injection_point = (uint8_t*)0xC0AD10;
const static uint8_t* darkwraith_inc_injection_point         = (uint8_t*)0xBFC0E0;
const static uint8_t* chaos_servant_inc_injection_point      = (uint8_t*)0xBFC17B;
const static uint8_t* sunlight_warrior_inc_injection_point   = (uint8_t*)0xBFC0C1;
const static uint8_t* darkmoon_inc_injection_point           = (uint8_t*)0xBFC15C;
const static uint8_t* path_of_the_dragon_inc_injection_point = (uint8_t*)0xBFC0FF;
const static uint8_t* gravelord_inc_injection_point          = (uint8_t*)0xBFC11E;


// Return addresses
uint32_t returnhere_tribute_humanity       = 0;
uint32_t returnhere_dec_humanity           = 0;
uint32_t returnhere_show_dialog            = 0;
uint32_t returnhere_dec_tribute_item       = 0;
uint32_t returnhere_inventory_item_id_read = 0;
uint32_t returnhere_darkwraith_inc         = 0;
uint32_t returnhere_chaos_servant_inc      = 0;
uint32_t returnhere_sunlight_warrior_inc   = 0;
uint32_t returnhere_darkmoon_inc           = 0;
uint32_t returnhere_path_of_the_dragon_inc = 0;
uint32_t returnhere_gravelord_inc          = 0;



void apply(const std::string& output_prefix)
{
    if (!is_active())
    {
        print_console(output_prefix + "Enabling multi-tributes (EXPERIMENTAL)");

        set_mem_protection(const_cast<uint8_t*>(tribute_humanity_injection_point), 7, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(tribute_humanity_injection_point), &returnhere_tribute_humanity, 2, &intercept_tribute_humanity);

        set_mem_protection(const_cast<uint8_t*>(dec_humanity_injection_point), 7, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(dec_humanity_injection_point), &returnhere_dec_humanity, 2, &intercept_dec_humanity);

        set_mem_protection(const_cast<uint8_t*>(dec_tribute_item_injection_point), 5, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(dec_tribute_item_injection_point), &returnhere_dec_tribute_item, 0, &intercept_dec_tribute_item);

        set_mem_protection(const_cast<uint8_t*>(show_dialog_injection_point), 5, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(show_dialog_injection_point), &returnhere_show_dialog, 0, &intercept_show_dialog);

        set_mem_protection(const_cast<uint8_t*>(inventory_item_id_read_injection_point), 5, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(inventory_item_id_read_injection_point), &returnhere_inventory_item_id_read, 0, &intercept_inventory_item_id_read);

        set_mem_protection(const_cast<uint8_t*>(darkwraith_inc_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(darkwraith_inc_injection_point), &returnhere_darkwraith_inc, 1, &intercept_darkwraith_inc);

        set_mem_protection(const_cast<uint8_t*>(chaos_servant_inc_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(chaos_servant_inc_injection_point), &returnhere_chaos_servant_inc, 1, &intercept_chaos_servant_inc);

        set_mem_protection(const_cast<uint8_t*>(sunlight_warrior_inc_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(sunlight_warrior_inc_injection_point), &returnhere_sunlight_warrior_inc, 1, &intercept_sunlight_warrior_inc);

        set_mem_protection(const_cast<uint8_t*>(darkmoon_inc_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(darkmoon_inc_injection_point), &returnhere_darkmoon_inc, 1, &intercept_darkmoon_inc);

        set_mem_protection(const_cast<uint8_t*>(path_of_the_dragon_inc_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(path_of_the_dragon_inc_injection_point), &returnhere_path_of_the_dragon_inc, 1, &intercept_path_of_the_dragon_inc);

        set_mem_protection(const_cast<uint8_t*>(gravelord_inc_injection_point), 6, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(gravelord_inc_injection_point), &returnhere_gravelord_inc, 1, &intercept_gravelord_inc);
    }
}


void unpatch(const std::string& output_prefix)
{
    if (is_active())
    {
        print_console(output_prefix + "Disabling multi-tributes...");

        forget_injection((uint32_t)tribute_humanity_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(tribute_humanity_injection_point), tribute_humanity_original_bytes, 7);

        forget_injection((uint32_t)dec_humanity_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(dec_humanity_injection_point), dec_humanity_original_bytes, 7);

        forget_injection((uint32_t)dec_tribute_item_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(dec_tribute_item_injection_point), dec_tribute_item_original_bytes, 5);

        forget_injection((uint32_t)show_dialog_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(show_dialog_injection_point), show_dialog_original_bytes, 5);

        forget_injection((uint32_t)inventory_item_id_read_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(inventory_item_id_read_injection_point), inventory_item_id_read_original_bytes, 5);

        forget_injection((uint32_t)darkwraith_inc_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(darkwraith_inc_injection_point), darkwraith_inc_original_bytes, 6);

        forget_injection((uint32_t)chaos_servant_inc_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(chaos_servant_inc_injection_point), chaos_servant_inc_original_bytes, 6);

        forget_injection((uint32_t)sunlight_warrior_inc_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(sunlight_warrior_inc_injection_point), sunlight_warrior_inc_original_bytes, 6);

        forget_injection((uint32_t)darkmoon_inc_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(darkmoon_inc_injection_point), darkmoon_inc_original_bytes, 6);

        forget_injection((uint32_t)path_of_the_dragon_inc_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(path_of_the_dragon_inc_injection_point), path_of_the_dragon_inc_original_bytes, 6);

        forget_injection((uint32_t)gravelord_inc_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(gravelord_inc_injection_point), gravelord_inc_original_bytes, 6);
    }
}


bool is_active()
{
    return (*((uint8_t*)dec_humanity_injection_point)) == 0xE9;
}


// Intercept for offering Humanity tribute (Darkwraith & Chaos Servant)
void __declspec(naked) __stdcall intercept_tribute_humanity()
{
    __asm
    {
        push eax
        push ebx
        // Save LastTalkID
        mov eax, 0x13786D0 //"DARKSOULS.exe"+0x00F786D0
        mov eax, [eax]
        mov eax, [eax+0xB08]
        mov [tmp_last_talk], eax
        // Check for valid game state
        //lea eax, dword ptr [edx*4+0xD4B148]
        mov eax, edx
        shl eax, 2
        add eax, 0xD4B148
        mov eax, [eax]
        mov ebx, 0xD4B06F //"DARKSOULS.exe"+0x94B06F
        cmp eax, ebx
        pop ebx
        pop eax
        jne originalcode_tribute_humanity
        // Check for valid LastTalkID
        cmp [tmp_last_talk], 0x2737A // 160634 = Kaathe LastTalkID
        je set_intercept_tribute_humanity
        cmp [tmp_last_talk], 0x22547 // 140615 = Daughter of Chaos LastTalkID
        je set_intercept_tribute_humanity
        jmp originalcode_tribute_humanity

        set_intercept_tribute_humanity:
        mov[change_next_dlg_to_number_picker], 1
        jmp originalcode_tribute_humanity

        originalcode_tribute_humanity:
        jmp dword ptr[edx*4+0xD4B148]
    }
}

// Intercept for decreasing soft humanity count after tribute (Darkwraith/Chaos Servant)
void __declspec(naked) __stdcall intercept_dec_humanity()
{
    __asm
    {
        //cmp [adjust_tribute_dec], 0
        //je originalcode_dec_humanity
        push eax
        push ebx
        // Check for valid game state
        //lea eax, dword ptr [edx*4+0xD4B29C]
        mov eax, edx
        shl eax, 2
        add eax, 0xD4B29C
        mov eax, [eax]
        mov ebx, 0xD4B22A //"DARKSOULS.exe"+0x94B22A
        cmp eax, ebx
        pop ebx
        pop eax
        jne originalcode_dec_humanity
        // eax = new humanity count (after tribute)
        inc eax
        push ebx
        mov ebx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400 // Subtract the number of humanities tributed
        sub eax, [ebx]
        pop ebx
        mov [adjust_tribute_dec], 0
        jmp originalcode_dec_humanity

        originalcode_dec_humanity:
        jmp dword ptr [edx*4+0xD4B29C]
    }
}

// Intercept for decreasing item count after tribute (Sunlight/Darkmoon/Gravelord/Dragon)
void __declspec(naked) __stdcall intercept_dec_tribute_item()
{
    __asm
    {
        // [ecx+8] = old item count
        // edx = new item count
        // If item count is increasing, proceed with normal behavior
        cmp edx, [ecx+8]
        jge originalcode_dec_tribute_item
        // If inventory is open, proceed with normal behavior
        push eax
        mov eax, 0x13786D0 //"DARKSOULS.exe"+0x00F786D0
        mov eax, [eax]
        mov eax, [eax+0x24] // eax = inventory_is_open flag
        cmp eax, 0
        pop eax
        jne originalcode_dec_tribute_item
        // If "Reinforce Equipment" menu is open, proceed with normal behavior
        push eax
        mov eax, 0x13786D0 //"DARKSOULS.exe"+0x00F786D0
        mov eax, [eax]
        mov eax, [eax+0x50] // eax = reinforce_menu_is_open flag
        cmp eax, 0
        pop eax
        jne originalcode_dec_tribute_item
        // If Bottomless Box is open, proceed with normal behavior
        push eax
        mov eax, 0x13786D0 //"DARKSOULS.exe"+0x00F786D0
        mov eax, [eax]
        mov eax, [eax+0x84] // eax = bottomless_box_is_open flag
        cmp eax, 0
        pop eax
        jne originalcode_dec_tribute_item
        // Check for valid item ID
        push eax
        mov eax, 0
        cmp [tmp_last_talk], 0x18D10 // 101648 = Sunlight Altar LastTalkID
        //jne 7
        //db 75 05
        jne not_sunlight_altar_last_talk_id
        mov eax, 1
        not_sunlight_altar_last_talk_id:
        cmp [tmp_last_talk], 0x20627 // 132647 = Stone Dragon LastTalkID
        //jne 7
        //db 75 05
        jne not_stone_dragon_last_talk_id
        mov eax, 1
        not_stone_dragon_last_talk_id:
        cmp [tmp_last_talk], 0x2023E // 131646 = Gravelord Altar LastTalkID
        //jne 7
        //db 75 05
        jne not_gravelord_altar_last_talk_id
        mov eax, 1
        not_gravelord_altar_last_talk_id:
        cmp [tmp_last_talk], 0x2503C // 151612 = Gwyndolin LastTalkID
        //jne 7
        //db 75 05
        jne not_gwyndolin_last_talk_id
        mov eax, 1
        not_gwyndolin_last_talk_id:
        cmp eax, 1
        pop eax
        jne originalcode_dec_tribute_item
        // Proceed with item count adjustment
        //do_dec_tribute_item:
        push eax
        push ebx
        mov eax, [ecx+8] // eax = old item count
        mov ebx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        sub eax, [ebx] // eax = corrected item count
        mov edx, eax
        mov [ecx+8], edx
        pop ebx
        pop eax
        jmp cleanup_dec_tribute_item

        originalcode_dec_tribute_item:
        mov [ecx+8], edx
        cleanup_dec_tribute_item:
        mov eax, edx
        jmp returnhere_dec_tribute_item
    }
}

// Intercept for opening dialog box
void __declspec(naked) __stdcall intercept_show_dialog()
{
    __asm
    {
        // Check whether to change the dialog type
        cmp [change_next_dlg_to_number_picker], 0
        je check_non_humanity_tribute_prompt
        cmp edx, 0
        je originalcode_show_dialog

        check_non_humanity_tribute_prompt:
        cmp edx, 8 // 8 = Yes/No prompt dialog type
        jne originalcode_show_dialog
        // Save LastTalkID
        push eax
        mov eax, 0x13786D0 //"DARKSOULS.exe"+0x00F786D0
        mov eax, [eax]
        mov eax, [eax+0xB08]
        mov [tmp_last_talk], eax
        //pop eax
        mov eax, [dialog_msg_id]
        cmp [eax], 0x98E568 //["DARKSOULS.exe"+0xEE33E0] // 10020200 = "Offer Sunlight Medal" Message ID
        je do_show_dialog_intercept
        cmp [eax], 0x98E569 //["DARKSOULS.exe"+0xEE33E0] // 10020201 = "Offer humanity?" Message ID
        je do_show_dialog_intercept
        cmp [eax], 0x98E56A //["DARKSOULS.exe"+0xEE33E0] // 10020202 = "Offer Dragon Scale?" Message ID
        je do_show_dialog_intercept
        cmp [eax], 0x98E56B //["DARKSOULS.exe"+0xEE33E0] // 10020203 = "Offer Eye of Death?" Message ID
        je do_show_dialog_intercept
        cmp [eax], 0x98E56C //["DARKSOULS.exe"+0xEE33E0] // 10020204 = "Offer Souvenir of Reprisal?" Message ID
        je do_show_dialog_intercept
        cmp [eax], 0x98E56D //["DARKSOULS.exe"+0xEE33E0] // 10020205 = "Offer humanity?" Message ID
        je do_show_dialog_intercept
        pop eax
        jmp originalcode_show_dialog

        do_show_dialog_intercept:
        //pop eax
        //push eax
        push ecx
        push edx
        // Get current soft humanity count
        mov ecx, 0x1378700 //"DARKSOULS.exe"+0xF78700
        mov ecx, [ecx]
        mov ecx, [ecx+8]
        mov eax, ecx
        mov ecx, [ecx+0x7C] // ecx = current soft humanity count
        mov edx, 0x64 // 100 = Max tributes for all covenants (except Chaos Servant?)

        // Get current Covenant & devotion level
        cmp byte ptr [eax+0x10B], 3 // 3 = Warriors of Sunlight covenant ID
        jne not_sunlight_covenant
        movzx eax, byte ptr [eax+0xE5] // eax = Warriors of Sunlight covenant devotion level
        mov ecx, [sunlight_medal_count]
        jmp set_max_show_dialog
        not_sunlight_covenant:
        cmp byte ptr [eax+0x10B], 4 // 4 = Darkwraith covenant ID
        jne not_darkwraith_covenant
        movzx eax, byte ptr [eax+0xE6] // eax = Darkwraith covenant devotion level
        jmp set_max_show_dialog
        not_darkwraith_covenant:
        cmp byte ptr [eax+0x10B], 5 // 5 = Dragon covenant ID
        jne not_dragon_covenant
        movzx eax, byte ptr [eax+0xE7] // eax = Dragon covenant devotion level
        mov ecx, [dragon_scale_count]
        jmp set_max_show_dialog
        not_dragon_covenant:
        cmp byte ptr [eax+0x10B], 6 // 6 = Gravelord covenant ID
        jne not_gravelord_covenant
        movzx eax, byte ptr [eax+0xE8] // eax = Gravelord covenant devotion level
        mov ecx, [eye_of_death_count]
        jmp set_max_show_dialog
        not_gravelord_covenant:
        cmp byte ptr [eax+0x10B], 8 // 8 = Darkmoon covenant ID
        jne not_darkmoon_covenant
        movzx eax, byte ptr [eax+0xEA] // eax = Darkmoon covenant devotion level
        mov ecx, [souvenir_count]
        jmp set_max_show_dialog
        not_darkmoon_covenant:
        cmp byte ptr [eax+0x10B], 9 // 9 = Chaos Servant covenant ID
        jne not_chaos_servant_covenant
        movzx eax, byte ptr [eax+0xEB] // eax = Chaos Servant covenant devotion level
        //mov edx, 0xEA60 // 60,000 = Max tributes for Chaos Servant covenant (??)
        jmp set_max_show_dialog
        not_chaos_servant_covenant:
        // Irrelevant covenant; proceed with normal execution
        pop edx
        pop ecx
        pop eax
        jmp cleanup_show_dialog

        set_max_show_dialog:
        push ebx
        // Determine max tribute value
        // eax = Current covenant devotion level
        cmp eax, 0xA // 10 = First covenant upgrade
        jge above_first_covenant_lvl
        mov ebx, 0xA
        sub ebx, eax
        mov eax, ebx // eax = Number of tributes to next covenant upgrade
        cmp ecx, eax
        jle done_setting_max_show_dialog
        mov ecx, eax
        jmp done_setting_max_show_dialog
        above_first_covenant_lvl:
        cmp eax, 0x1E // 30 = Second covenant upgrade
        jge above_second_covenant_lvl
        mov ebx, 0x1E
        sub ebx, eax
        mov eax, ebx // eax = Number of tributes to next covenant upgrade
        cmp ecx, eax
        jle done_setting_max_show_dialog
        mov ecx, eax
        jmp done_setting_max_show_dialog
        above_second_covenant_lvl:
        cmp eax, 0x50 // 80 = Third covenant upgrade
        jge above_third_covenant_lvl
        mov ebx, 0x50
        sub ebx, eax
        mov eax, ebx // eax = Number of tributes to next covenant upgrade
        cmp ecx, eax
        jle done_setting_max_show_dialog
        mov ecx, eax
        jmp done_setting_max_show_dialog
        above_third_covenant_lvl:
        cmp eax, edx // edx = Max allowed tributes for current covenant
        jge tributes_maxed
        mov ebx, edx
        sub ebx, eax
        mov eax, ebx // eax = Number of tributes to max devotion
        cmp ecx, eax
        jle done_setting_max_show_dialog
        mov ecx, eax
        jmp done_setting_max_show_dialog
        tributes_maxed:
        mov ecx, 1
        done_setting_max_show_dialog:
        pop ebx
        pop edx

        //set_args_show_dialog:
        // Set number picker arguments
        mov eax, 0x13786D0 //"DARKSOULS.exe"+0x00F786D0
        mov eax, [eax]
        mov [eax+0x208], 1   // Set min number picker value
        mov [eax+0x20C], 1   // Set default number picker value
        mov [eax+0x204], ecx // Set max number picker value (current soft humanity count)
        pop ecx
        pop eax
        cmp edx, 8 // 8 = Yes/No prompt dialog type
        jne cleanup_show_dialog
        // Change dialog type to number picker
        mov edx, 3 // 3 = Number picker dialog type
        cleanup_show_dialog:
        mov [change_next_dlg_to_number_picker], 0
        mov [tmp_last_talk], 0
        jmp originalcode_show_dialog

        // Intercept for opening dialog box
        originalcode_show_dialog:
        mov [ecx+0x60],edx
        pop ecx
        ret
    }
}

// Intercept of inventory-reading instruction
void __declspec(naked) __stdcall intercept_inventory_item_id_read()
{
    __asm
    {
        // [ecx+4] = item ID
        cmp [ecx+4], 0x177 // 375 = Sunlight Medal item ID
        jne not_sunlight_medal_item_id
        push eax
        mov eax, [ecx+8] // [ecx+8] = Sunlight Medal quantity
        mov [sunlight_medal_count], eax
        pop eax
        jmp originalcode_inventory_item_id_read
        not_sunlight_medal_item_id:
        cmp [ecx+4], 0x456 // 1110 = Dragon Scale item ID
        jne not_dragon_scale_item_id
        push eax
        mov eax, [ecx+8] // [ecx+8] = Dragon Scale quantity
        mov [dragon_scale_count], eax
        pop eax
        jmp originalcode_inventory_item_id_read
        not_dragon_scale_item_id:
        cmp [ecx+4], 0x176 // 374 = Souvenir of Reprisal item ID
        jne not_souvenir_item_id
        push eax
        mov eax, [ecx+8] // [ecx+8] = Souvenir of Reprisal quantity
        mov [souvenir_count], eax
        pop eax
        jmp originalcode_inventory_item_id_read
        not_souvenir_item_id:
        cmp [ecx+4], 0x6D // 109 = Eye of Death item ID
        jne originalcode_inventory_item_id_read
        push eax
        mov eax, [ecx+8] // [ecx+8] = Eye of Death quantity
        mov [eye_of_death_count], eax
        pop eax
        jmp originalcode_inventory_item_id_read

        originalcode_inventory_item_id_read:
        cmp [ecx+4],edi
        je inventory_item_read_orig_je //0xC0AD20 //"DARKSOULS.exe"+0x80AD20
        jmp returnhere_inventory_item_id_read
        inventory_item_read_orig_je:
        jmp inventory_item_read_orig_je_address //0xC0AD20
    }
}

// Intercept for increasing Darkwraith devotion
void __declspec(naked) __stdcall intercept_darkwraith_inc()
{
    __asm
    {
        sub dl, 1
        push ecx
        mov ecx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        add dl, byte ptr [ecx] // Add specified number of tributes (result of number picker dialog)
        pop ecx
        push ebx
        movzx ebx, dl
        mov [esp+0xC], ebx
        pop ebx
        cmp dl, 0x64 // 100 = max devotion
        jle originalcode_darkwraith_inc
        mov dl, 0x64 // If devotion > 100, set it to 100
        mov [esp+8], 0x64
        originalcode_darkwraith_inc:
        mov [ecx+0x000000E6], dl
        jmp returnhere_darkwraith_inc
    }
}

// Intercept for increasing Chaos Servant devotion
void __declspec(naked) __stdcall intercept_chaos_servant_inc()
{
    __asm
    {
        sub al, 1
        push ecx
        mov ecx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        add al, byte ptr [ecx] // Add specified number of tributes (result of number picker dialog)
        pop ecx
        push ebx
        movzx ebx, al
        mov [esp+0xC], ebx
        pop ebx
        cmp al, 0x64 // 100 = max devotion
        jle originalcode_chaos_servant_inc
        mov al, 0x64 // If devotion > 100, set it to 100
        mov [esp+8], 0x64
        originalcode_chaos_servant_inc:
        mov [ecx+0x000000EB], al
        jmp returnhere_chaos_servant_inc
    }
}

// Intercept for increasing Warriors of Sunlight devotion
void __declspec(naked) __stdcall intercept_sunlight_warrior_inc()
{
    __asm
    {
        sub al, 1
        push ecx
        mov ecx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        add al, byte ptr [ecx] // Add specified number of tributes (result of number picker dialog)
        pop ecx
        push ebx
        movzx ebx, al
        mov [esp+0xC], ebx
        pop ebx
        cmp al, 0x64 // 100 = max devotion
        jle originalcode_sunlight_warrior_inc
        mov al, 0x64 // If devotion > 100, set it to 100
        mov [esp+8], 0x64
        originalcode_sunlight_warrior_inc:
        mov [ecx+0x000000E5], al
        jmp returnhere_sunlight_warrior_inc
    }
}

// Intercept for increasing Darkmoon Blade covenant devotion
void __declspec(naked) __stdcall intercept_darkmoon_inc()
{
    __asm
    {
        sub dl, 1
        push ecx
        mov ecx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        add dl, byte ptr [ecx] // Add specified number of tributes (result of number picker dialog)
        pop ecx
        push ebx
        movzx ebx, dl
        mov [esp+0xC], ebx
        pop ebx
        cmp dl, 0x64 // 100 = max devotion
        jle originalcode_darkmoon_inc
        mov dl, 0x64 // If devotion > 100, set it to 100
        mov [esp+8], 0x64
        originalcode_darkmoon_inc:
        mov [ecx+0x000000EA], dl
        jmp returnhere_darkmoon_inc
    }
}

// Intercept for increasing Path of the Dragon devotion
void __declspec(naked) __stdcall intercept_path_of_the_dragon_inc()
{
    __asm
    {
        sub al, 1
        push ecx
        mov ecx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        add al, byte ptr [ecx] // Add specified number of tributes (result of number picker dialog)
        pop ecx
        push ebx
        movzx ebx, al
        mov [esp+0xC], ebx
        pop ebx
        cmp al, 0x64 // 100 = max devotion
        jle originalcode_path_of_the_dragon_inc
        mov al, 0x64 // If devotion > 100, set it to 100
        mov [esp+8], 0x64
        originalcode_path_of_the_dragon_inc:
        mov [ecx+0x000000E7], al
        jmp returnhere_path_of_the_dragon_inc
    }
}

// Intercept for increasing Gravelord Servant covenant devotion
void __declspec(naked) __stdcall intercept_gravelord_inc()
{
    __asm
    {
        sub dl, 1
        push ecx
        mov ecx, 0x12E3400 //"DARKSOULS.exe"+0xEE3400
        add dl, byte ptr [ecx] // Add specified number of tributes (result of number picker dialog)
        pop ecx
        push ebx
        movzx ebx, dl
        mov [esp+0xC], ebx
        pop ebx
        cmp dl, 0x64 // 100 = max devotion
        jle originalcode_gravelord_inc
        mov dl, 0x64 // If devotion > 100, set it to 100
        mov [esp+8], 0x64
        originalcode_gravelord_inc:
        mov [ecx+0x000000E8], dl
        jmp returnhere_gravelord_inc
    }
}


} // namespace MultiTribute

