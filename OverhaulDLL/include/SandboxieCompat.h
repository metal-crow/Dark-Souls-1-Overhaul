#pragma once

#ifndef _DS1_OVERHAUL_SANDBOXIE_COMPAT_H_
#define _DS1_OVERHAUL_SANDBOXIE_COMPAT_H_


// Reconciles Sandboxie's hook on the game's entry point with Seamless Co-op's arxan-disabler,
// which otherwise corrupt each other and crash the process on its first call into the entry point.
// Does nothing when the process isn't running under Sandboxie. See SandboxieCompat.cpp.
class SandboxieCompat
{
public:
    static void start();
};

#endif
