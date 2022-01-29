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

    //insert code into the FrpgMenuDlgFloatingPCGauge render function to, if needed, render the inserted icon
    write_address = (uint8_t*)(UIFeatures::Render_InsertedIconObj_FrpgMenuDlgFloatingPCGauge_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &Render_IconObj_FrpgMenuDlgFloatingPCGauge_return, 2, &Render_IconObj_FrpgMenuDlgFloatingPCGauge_injection);

}
