#pragma once
#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <cstdint>

class MainLoop
{
public:
    static void start();

private:
    static const uint64_t main_game_loop_injection_offset = 0x15b660;
};

#endif
