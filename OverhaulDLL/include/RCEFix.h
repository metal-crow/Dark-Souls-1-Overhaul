#pragma once

#ifndef _RCEFIX_H_
#define _RCEFIX_H_

#include <cstdint>

class RCEFix
{
public:
    static void start();
private:
    static const uint64_t rce1_offset = 0x1078a1a;
};

#endif
