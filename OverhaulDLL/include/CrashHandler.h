/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow  -  C++

*/
#pragma once

#ifndef DS1_OVERHAUL_CRASHHANDER_H_
#define DS1_OVERHAUL_CRASHHANDER_H_

#include <Windows.h>

void crash_handler(EXCEPTION_POINTERS* ExceptionInfo, char* crash_message_str);
void set_crash_handlers();
void backtrace_debug();

#endif
