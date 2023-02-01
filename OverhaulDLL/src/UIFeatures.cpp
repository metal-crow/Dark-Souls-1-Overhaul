#include "UIFeatures.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t Insert_IconObj_FrpgMenuDlgFloatingPCGauge_return;
    void Insert_IconObj_FrpgMenuDlgFloatingPCGauge_injection();

    uint64_t Null_New_IconObj_FrpgMenuDlgFloatingPCGauge_return;
    void Null_New_IconObj_FrpgMenuDlgFloatingPCGauge_injection();

    uint64_t Render_IconObj_FrpgMenuDlgFloatingPCGauge_return;
    void Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection();
    void Render_IconObj_FrpgMenuDlgFloatingPCGauge_helper(uint64_t FrpgMenuDlgFloatingPCGauge);
}

void UIFeatures::start()
{
    ConsoleWrite("Enabling UI Features/changes...");

    uint8_t *write_address;

    //This allows us to show a new icon next to a PC (like the LevelSync icon DSR added) that indicates they are a mod user

    //increase the size of the FrpgMenuDlgFloatingPCGauge class by 1 pointer so we can fit a new Obj
    uint32_t new_FrpgMenuDlgFloatingPCGauge_class_size = 0x250 + 8;
    write_address = (uint8_t*)(UIFeatures::Increase_FrpgMenuDlgFloatingPCGauge_class_size_offset1 + Game::ds1_base);
    bool res = sp::mem::patch_bytes((void*)(write_address + 1), (uint8_t*)&new_FrpgMenuDlgFloatingPCGauge_class_size, sizeof(new_FrpgMenuDlgFloatingPCGauge_class_size));
    if (!res) FATALERROR("Unable to set new_FrpgMenuDlgFloatingPCGauge_class_size");
    write_address = (uint8_t*)(UIFeatures::Increase_FrpgMenuDlgFloatingPCGauge_class_size_offset2 + Game::ds1_base);
    res = sp::mem::patch_bytes((void*)(write_address + 1), (uint8_t*)&new_FrpgMenuDlgFloatingPCGauge_class_size, sizeof(new_FrpgMenuDlgFloatingPCGauge_class_size));
    if (!res) FATALERROR("Unable to set new_FrpgMenuDlgFloatingPCGauge_class_size");

    //When Initing the FrpgMenuDlgFloatingPCGauge class, set the new Obj pointer to null
    write_address = (uint8_t*)(UIFeatures::Null_New_IconObj_FrpgMenuDlgFloatingPCGauge_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Null_New_IconObj_FrpgMenuDlgFloatingPCGauge_return, 2, &Null_New_IconObj_FrpgMenuDlgFloatingPCGauge_injection);

    //using the extra bytes alloc'd above, insert and setup a new icon obj
    write_address = (uint8_t*)(UIFeatures::Insert_IconObj_FrpgMenuDlgFloatingPCGauge_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Insert_IconObj_FrpgMenuDlgFloatingPCGauge_return, 1, &Insert_IconObj_FrpgMenuDlgFloatingPCGauge_injection);

    //insert code into the FrpgMenuDlgFloatingPCGauge render function to, if needed, render the inserted icon and remove other icons
    //This is injected after the other UI elements have been set, so we can rewrite them freely
    write_address = (uint8_t*)(UIFeatures::Render_InsertedIconObj_FrpgMenuDlgFloatingPCGauge_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Render_IconObj_FrpgMenuDlgFloatingPCGauge_return, 0, &Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection);

}

void Render_IconObj_FrpgMenuDlgFloatingPCGauge_helper(uint64_t FrpgMenuDlgFloatingPCGauge)
{
    //hide all the existing UI elements
    uint64_t* MenuObj_list = *(uint64_t**)(FrpgMenuDlgFloatingPCGauge + 0x18);
    uint32_t MenuObj_list_len = *(uint32_t*)(FrpgMenuDlgFloatingPCGauge + 0x20);
    for (uint32_t i = 0; i < MenuObj_list_len; i++)
    {
        uint8_t* flags = (uint8_t*)(MenuObj_list[i] + 0x28);
        *flags = *flags & 0b11111101; //unset the 2nd bit, which hides it
    }

    //show our custom ui element
    uint64_t CustomObj = *(uint64_t*)(FrpgMenuDlgFloatingPCGauge + 0x250);
    uint8_t* CustomObjFlags = (uint8_t*)(CustomObj + 0x28);
    *CustomObjFlags = *CustomObjFlags | 0b00000010; //set the visible bit
}
