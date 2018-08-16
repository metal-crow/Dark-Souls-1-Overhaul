/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  C++, L3 jump technique
*/


#include "L3Jump.h"
#include "DllMain.h"


namespace L3Jump {


bool _active = false;

uint32_t force_jump_on_sprint = 0;
uint32_t sprint_begin_force_jump_address = 0xE18132;

const static uint8_t* sprint_begin_injection_point = (uint8_t*)0xE18011;
uint32_t returnhere_sprint_begin = 0;
const static uint8_t sprint_begin_original_bytes[7] = { 0x8B, 0x44, 0x24, 0x5C, 0x8B, 0x48, 0x04 };

void __declspec(naked) __stdcall intercept_sprint_begin()
{
    __asm
    {
        cmp [force_jump_on_sprint], 0
        je originalcode_sprint_begin

        jmp sprint_begin_force_jump_address

        originalcode_sprint_begin:
        mov eax, [esp+0x5C]
        mov ecx, [eax+4]

        jmp returnhere_sprint_begin
    }
}


void apply(const std::string& output_prefix)
{
    if (!_active)
    {
        set_mem_protection(reinterpret_cast<uint8_t*>(0xE18130), 2, MEM_PROTECT_RWX);
        *reinterpret_cast<uint16_t*>(0xE18130) = 0x3DEB;

        set_mem_protection(const_cast<uint8_t*>(sprint_begin_injection_point), 7, MEM_PROTECT_RWX);
        inject_jmp_5b(const_cast<uint8_t*>(sprint_begin_injection_point), &returnhere_sprint_begin, 2, &intercept_sprint_begin);

        _active = true;
        print_console(output_prefix + "Enabled L3 jump");
    }
}


void unpatch(const std::string& output_prefix)
{
    if (_active)
    {
        *reinterpret_cast<uint16_t*>(0xE18130) = 0x3D76;

        forget_injection((uint32_t)sprint_begin_injection_point);
        apply_byte_patch(const_cast<uint8_t*>(sprint_begin_injection_point), sprint_begin_original_bytes, 7);

        _active = false;
        print_console(output_prefix + "Disabled L3 jump");
    }
}


void force_jump(bool force)
{
    if (_active)
    {
        if (force)
        {
            //*reinterpret_cast<uint16_t*>(0xE18130) = 0x9090;
            force_jump_on_sprint = 1;
        }
        else
        {
            //*reinterpret_cast<uint16_t*>(0xE18130) = 0x3DEB;
            force_jump_on_sprint = 0;
        }
    }
}


bool is_active()
{
    return _active;
}


} // namespace L3Jump

