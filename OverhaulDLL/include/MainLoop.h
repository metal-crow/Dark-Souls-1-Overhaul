#pragma once
#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <cstdint>
#include <string>

typedef bool(*MainLoopCallback)(void*);

class MainLoop
{
public:
    static void start();
    static void setup_mainloop_callback(MainLoopCallback func, void* data, std::string name);
private:
    static const uint64_t main_game_loop_injection_offset = 0x15ce90;
};

#endif
