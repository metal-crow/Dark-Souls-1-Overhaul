/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++ & Durability bars technique
*/


#include "DurabilityBars.h"
#include "DllMain.h"
#include "Param/Params.h"

namespace DurabilityBars {


DurabilityBars::dx_data render_data;


// Equipped weapon data as stored in game memory
// pragma pack() turns off implicit struct padding
#pragma pack(push, 1)
struct equipment_data {
    uint8_t state = 1; // WIELD_STATE
    uint8_t unk0[3];
    uint8_t l_using_secondary = 0; // 0 or 1
    uint8_t unk1[3];
    uint8_t r_using_secondary = 0; // 0 or 1
    uint8_t unk2[3];
    uint8_t unk3[16];
    uint32_t l_weap_id1;
    uint32_t r_weap_id1;
    uint32_t l_weap_id2;
    uint32_t r_weap_id2;
};
#pragma pack(pop)

//// Flags that indicate whether menus/dialogs are currently open
//#pragma pack(push, 1)
//struct menu_flags {
//    uint32_t unk0;
//    uint8_t unk;
//};
//#pragma pack(pop)

enum menu_flags_enum {
    inventory_menu = 36,
    equipment_menu = 40,
    bonfire_menu = 64,
    repair_equipment = 76,
    reinforce_equipment = 80,
    interaction_prompt = 88, // "A: Rest at bonfire" etc.
    generic_dialog = 96,
    write_message_page_2 = 100,
    general_settings = 104,
    character_status_menu = 112,
    level_up = 120,
    attune_magic = 128,
    bottomless_box = 132,
    write_message_page_1 = 136,
    summon_phantom_prompt = 156,
    timed_dialog = 160,
    black_background = 164,
    warp_menu = 172,
    playing_credits = 180,
    book_of_guilty = 216,
    yes_no_dialog = 256,
    settings_screen_brightness = 264, // Adjust screen brightness
    feed_item = 284,
    written_messages = 288,
    dpad_icons_hidden = 512,
    rendered_cutscene_1 = 716,
    rendered_cutscene_2 = 720,
    settings_keybinds = 732,
    pc_settings = 736,
};

SpPointer equip_data((void*)0x1378700 /* DARKSOULS.EXE+0xF78700 */, { 0x8, 0x318, 0x78 });
SpPointer menu_flags((void*)0x13786D0 /* DARKSOULS.EXE+0xF786D0 */, { 0x0 });
SpPointer hud_enabled_flag((void*)0x1378700 /* DARKSOULS.EXE+0xF78700 */, { 0x2C, 0x0D });


bool enable_pref = true;

constexpr const size_t patch_byte_count = 7;
const static uint8_t*  injection_point = (uint8_t*)0xFAB8D8; // DARKSOULS.exe+0xBAB8D8
const static uint8_t   original_bytes[patch_byte_count] = { 0x8D, 0x2C, 0xFD, 0x00, 0x00, 0x00, 0x00 };
uint32_t return_address  = 0;

uint32_t l_slot_address  = 0;
uint32_t r_slot_address  = 0;
uint32_t l_slot_address2 = 0;
uint32_t r_slot_address2 = 0;

uint32_t l_durability  = 0; // Current durability of primary   weapon (left hand)
uint32_t r_durability  = 0; // Current durability of primary   weapon (right hand)
uint32_t l_durability2 = 0; // Current durability of secondary weapon (left hand)
uint32_t r_durability2 = 0; // Current durability of secondary weapon (right hand)

uint16_t l_max_durability  = 0; // Max durability of primary   weapon (left hand)
uint16_t r_max_durability  = 0; // Max durability of primary   weapon (right hand)
uint16_t l_max_durability2 = 0; // Max durability of secondary weapon (left hand)
uint16_t r_max_durability2 = 0; // Max durability of secondary weapon (right hand)


void __stdcall durability_intercept();


bool intercept_active()
{
    return (*reinterpret_cast<const uint8_t*>(injection_point)) != original_bytes[0];
}


void apply(bool silent)
{
    if (!intercept_active())
    {
        if (!silent)
        {
            print_console("Enabling HUD durability bars...");
        }
        set_mem_protection(const_cast<uint8_t*>(injection_point), patch_byte_count, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(injection_point), &return_address, 2, &durability_intercept);
    }
}


void unpatch()
{
    if (intercept_active())
    {
        print_console("Disabling HUD durability bars...");
        forget_injection((uint32_t)injection_point);
        apply_byte_patch(const_cast<uint8_t*>(injection_point), original_bytes, patch_byte_count);
    }
}


void update_data()
{
    if (DurabilityBars::enable_pref && DurabilityBars::intercept_active())
    {
        static DurabilityBars::equipment_data* equipped_weaps = NULL;
        equipped_weaps = reinterpret_cast<DurabilityBars::equipment_data*>(DurabilityBars::equip_data.resolve());

        // Get weapon slot addresses
        if (equipped_weaps)
        {
            DurabilityBars::l_slot_address  = (uint32_t)&(equipped_weaps->l_weap_id1);
            DurabilityBars::r_slot_address  = (uint32_t)&(equipped_weaps->r_weap_id1);
            DurabilityBars::l_slot_address2 = (uint32_t)&(equipped_weaps->l_weap_id2);
            DurabilityBars::r_slot_address2 = (uint32_t)&(equipped_weaps->r_weap_id2);

            // Get maximum durability values
            if (Params::all_loaded())
            {
                WeaponParam* l_param1 = (WeaponParam*)Params::Weapon().get_by_id(equipped_weaps->l_weap_id1 - (equipped_weaps->l_weap_id1 % 100));
                WeaponParam* r_param1 = (WeaponParam*)Params::Weapon().get_by_id(equipped_weaps->r_weap_id1 - (equipped_weaps->r_weap_id1 % 100));
                WeaponParam* l_param2 = (WeaponParam*)Params::Weapon().get_by_id(equipped_weaps->l_weap_id2 - (equipped_weaps->l_weap_id2 % 100));
                WeaponParam* r_param2 = (WeaponParam*)Params::Weapon().get_by_id(equipped_weaps->r_weap_id2 - (equipped_weaps->r_weap_id2 % 100));

                DurabilityBars::l_max_durability  = l_param1 ? l_param1->durabilityMax : 0;
                DurabilityBars::r_max_durability  = r_param1 ? r_param1->durabilityMax : 0;
                DurabilityBars::l_max_durability2 = l_param2 ? l_param2->durabilityMax : 0;
                DurabilityBars::r_max_durability2 = r_param2 ? r_param2->durabilityMax : 0;

                float l_modifier = 1.0f;
                float r_modifier = 1.0f;

                // Calculate durability percentages
                if (!equipped_weaps->l_using_secondary)
                {
                    // Left hand using primary weapon
                    l_modifier = static_cast<float>(l_durability) / static_cast<float>(l_max_durability);
                }
                else
                {
                    // Left hand using secondary weapon
                    l_modifier = static_cast<float>(l_durability2) / static_cast<float>(l_max_durability2);
                }

                if (!equipped_weaps->r_using_secondary)
                {
                    // Right hand using primary weapon
                    r_modifier = static_cast<float>(r_durability) / static_cast<float>(r_max_durability);
                }
                else
                {
                    // Right hand using secondary weapon
                    r_modifier = static_cast<float>(r_durability2) / static_cast<float>(r_max_durability2);
                }


                long width  = render_data.display_mode.Width,
                     height = render_data.display_mode.Height;
                WINDOWINFO window_info;
                memset(&window_info, 0, sizeof(window_info));
                if (GetWindowInfo(_game_window, &window_info))
                {
                    width  = window_info.rcClient.right  - window_info.rcClient.left;
                    height = window_info.rcClient.bottom - window_info.rcClient.top;

                    // Check if fullscreen
                    HWND desktop_window = GetDesktopWindow();
                    if (desktop_window)
                    {
                        RECT desktop;
                        memset(&desktop, 0, sizeof(desktop));
                        if (GetWindowRect(desktop_window, &desktop))
                        {
                            if (desktop.bottom == window_info.rcWindow.bottom
                                && desktop.top == window_info.rcWindow.top
                                && desktop.left == window_info.rcWindow.left
                                && desktop.right == window_info.rcWindow.right)
                            {
                                // Game is full-screen
                                width  = window_info.rcWindow.right  - window_info.rcWindow.left;
                                height = window_info.rcWindow.bottom - window_info.rcWindow.top;
                            }
                        }
                    }


                    // Window info successfully obtained
                    //print_console("window:  width=" + std::to_string(window_info.rcClient.right - window_info.rcClient.left)
                    //              +      "  height=" + std::to_string(window_info.rcClient.bottom - window_info.rcClient.top)
                    //              + "      render: width=" + std::to_string(render_data.display_mode.Width)
                    //              + "  height=" + std::to_string(render_data.display_mode.Height)
                    //              + "   (fullscreen=" + std::to_string((width == window_info.rcWindow.right - window_info.rcWindow.left)) + ")");
                }

                // Format HUD element data
                render_data.background_l.x1 = static_cast<long>(width  * 0.0833f) + render_data.offset_left_x;
                render_data.background_l.y1 = static_cast<long>(height * 0.8009f) + render_data.offset_y;
                render_data.background_l.x2 = render_data.background_l.x1 + static_cast<long>((width  * 0.0521f)  * render_data.scale);
                render_data.background_l.y2 = render_data.background_l.y1 + static_cast<long>((height * 0.00463f) * render_data.scale);

                render_data.background_r.x1 = render_data.background_l.x1 + static_cast<long>((width * 0.14583f) * render_data.scale) + render_data.offset_right_x - render_data.offset_left_x;
                render_data.background_r.y1 = render_data.background_l.y1;
                render_data.background_r.x2 = render_data.background_r.x1 + static_cast<long>((width  * 0.0521f)  * render_data.scale);
                render_data.background_r.y2 = render_data.background_l.y2;

                render_data.bar_l.x1 = render_data.background_l.x1;
                render_data.bar_l.y1 = render_data.background_l.y1;
                render_data.bar_l.x2 = render_data.bar_l.x1 + static_cast<long>((render_data.background_l.x2 - render_data.background_l.x1) * l_modifier);
                render_data.bar_l.y2 = render_data.background_l.y2;

                render_data.bar_r.x1 = render_data.background_r.x1;
                render_data.bar_r.y1 = render_data.background_r.y1;
                render_data.bar_r.x2 = render_data.bar_r.x1 + static_cast<long>((render_data.background_r.x2 - render_data.background_r.x1) * r_modifier);
                render_data.bar_r.y2 = render_data.background_r.y2;

                //print_console("left bar x=" + std::to_string(render_data.background_l.x1)
                //              + "  y=" + std::to_string(render_data.background_l.y1)
                //              + "  width=" + std::to_string(render_data.background_l.x2- render_data.background_l.x1)
                //              + "  height=" + std::to_string(render_data.background_l.y2 - render_data.background_l.y1));

                // Check if menus are open
                static uint8_t* flags = reinterpret_cast<uint8_t*>(menu_flags.resolve());
                if (flags)
                {
                    if (   flags[inventory_menu]
                        || flags[equipment_menu]
                        || flags[bonfire_menu]
                        || flags[repair_equipment]
                        || flags[reinforce_equipment]
                        || flags[interaction_prompt]
                        || flags[generic_dialog]
                        || flags[write_message_page_2]
                        || flags[general_settings]
                        || flags[character_status_menu]
                        || flags[level_up]
                        || flags[attune_magic]
                        || flags[bottomless_box]
                        || flags[write_message_page_1]
                        || flags[summon_phantom_prompt]
                        || flags[timed_dialog]
                        || flags[black_background]
                        || flags[warp_menu]
                        || flags[playing_credits]
                        || flags[book_of_guilty]
                        || flags[yes_no_dialog]
                        || flags[settings_screen_brightness]
                        || flags[feed_item]
                        || flags[written_messages]
                        || flags[dpad_icons_hidden]
                        || flags[settings_keybinds]
                        || flags[pc_settings]
                        || (flags[rendered_cutscene_1] && flags[rendered_cutscene_2]) // Can this be OR?
                        || console_is_open()
                        || (hud_enabled_flag.resolve() && (!*reinterpret_cast<uint8_t*>(hud_enabled_flag.resolve())))
                        )
                    {
                        render_data.display = false;
                        return;
                    }
                }
                render_data.display = true;
            }
            else
            {
                render_data.display = false;
                DurabilityBars::l_max_durability  = 0;
                DurabilityBars::r_max_durability  = 0;
                DurabilityBars::l_max_durability2 = 0;
                DurabilityBars::r_max_durability2 = 0;
            }
        }
        else
        {
            render_data.display = false;
            DurabilityBars::l_slot_address  = 0;
            DurabilityBars::r_slot_address  = 0;
            DurabilityBars::l_slot_address2 = 0;
            DurabilityBars::r_slot_address2 = 0;
        }
    }
    else
    {
        render_data.display = false;
    }
}


// Injected function for obtaining current weapon durability data
void __declspec(naked) __stdcall durability_intercept()
{
    __asm
    {
        // Save registers
        push ebx

        // Determine which weapon slot to store data for
        mov ebx, l_slot_address
        cmp esi, ebx
        je l_prim

        mov ebx, r_slot_address
        cmp esi, ebx
        je r_prim

        mov ebx, l_slot_address2
        cmp esi, ebx
        je l_sec

        mov ebx, r_slot_address2
        cmp esi, ebx
        je r_sec

        // No relevant data ATM; don't update any values
        jmp cleanup


        // eax holds current durability of primary left-hand weapon
        l_prim:
        mov l_durability, eax
        jmp cleanup

        // eax holds current durability of primary right-hand weapon
        r_prim:
        mov r_durability, eax
        jmp cleanup

        // eax holds current durability of secondary left-hand weapon
        l_sec:
        mov l_durability2, eax
        jmp cleanup

        // eax holds current durability of secondary right-hand weapon
        r_sec:
        mov r_durability2, eax
        jmp cleanup


        cleanup:
        // Restore registers
        pop ebx

        // Original code
        lea ebp, [edi * 0x8 + 0x00000000]

        jmp return_address;
    }
}


// Alternate version that writes 0 to durability value if slot is empty
//void __declspec(naked) __stdcall durability_intercept_check_empty()
//{
//    __asm
//    {
//        // Save registers
//        push ebx
//        push edx
//        push ecx
//
//        // Store weapon ID for bare fists (empty weapon slot)
//        mov edx, 0xDBBA0 // 900000
//
//        // Determine which weapon slot to store data for
//        mov ebx, l_slot_address
//        cmp esi, ebx
//        je l_prim
//
//        mov ebx, r_slot_address
//        cmp esi, ebx
//        je r_prim
//
//        mov ebx, l_slot_address2
//        cmp esi, ebx
//        je l_sec
//
//        mov ebx, r_slot_address2
//        cmp esi, ebx
//        je r_sec
//
//        // No relevant data ATM; don't update any values
//        jmp cleanup
//
//
//        // eax holds current durability of primary left-hand weapon
//        l_prim:
//        // Check for empty slot
//        mov ecx, [ebx]
//        cmp edx, ecx // Compare ID to bare fists ID
//        jne l_prim_nonempty
//        mov l_durability, 0
//        jmp cleanup
//        // Non-empty slot
//        l_prim_nonempty:
//        mov l_durability, eax
//        jmp cleanup
//
//        // eax holds current durability of primary right-hand weapon
//        r_prim:
//        // Check for empty slot
//        mov ecx, [ebx]
//        cmp edx, ecx // Compare ID to bare fists ID
//        jne r_prim_nonempty
//        mov r_durability, 0
//        jmp cleanup
//        // Non-empty slot
//        r_prim_nonempty:
//        mov r_durability, eax
//        jmp cleanup
//
//        // eax holds current durability of secondary left-hand weapon
//        l_sec:
//        // Check for empty slot
//        mov ecx, [ebx]
//        cmp edx, ecx // Compare ID to bare fists ID
//        jne l_sec_nonempty
//        mov l_durability2, 0
//        jmp cleanup
//        // Non-empty slot
//        l_sec_nonempty:
//        mov l_durability2, eax
//        jmp cleanup
//
//        // eax holds current durability of secondary right-hand weapon
//        r_sec:
//        // Check for empty slot
//        mov ecx, [ebx]
//        cmp edx, ecx // Compare ID to bare fists ID
//        jne r_sec_nonempty
//        mov r_durability2, 0
//        jmp cleanup
//        // Non-empty slot
//        r_sec_nonempty:
//        mov r_durability2, eax
//        jmp cleanup
//
//
//        cleanup:
//        // Restore registers
//        pop ecx
//        pop edx
//        pop ebx
//
//        // Original code
//        lea ebp, [edi * 0x8 + 0x00000000]
//
//        jmp return_address;
//    }
//}



} // namespace DurabilityBars

