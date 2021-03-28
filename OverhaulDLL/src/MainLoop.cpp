#include "MainLoop.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t main_game_loop_injection_return;
    void main_game_loop_injection();
    void main_game_loop_injection_helper();
}

void MainLoop::start()
{
    ConsoleWrite("%s Starting main loop", Mod::output_prefix);

    uint8_t *write_address = (uint8_t*)(MainLoop::main_game_loop_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &main_game_loop_injection_return, 2, &main_game_loop_injection);
}
