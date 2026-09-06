#pragma once

#ifndef _DS1_OVERHAUL_HARNESS_CONTROL_H_
#define _DS1_OVERHAUL_HARNESS_CONTROL_H_

#include <cstdint>

// Rollback test-harness control plane: a localhost-only, line-oriented TCP
// server the orchestrator (dev_scripts/rollback_harness/harness_client.py)
// drives instead of the F6-F12 hotkeys. Commands are marshalled onto the game
// thread by a MainLoop callback; every reply is one JSON line. A connection that
// sends "subscribe" instead receives every ConsoleWrite line as it happens,
// which is how the sandboxed instance's log is read without touching its
// virtualised filesystem.
//
// OFF unless enabled: set env var DSR_HARNESS_PORT=<port> for the game process,
// or [Dark Souls Overhaul Testing] HarnessControlPort=<port> in d3d11_mod.ini.
// Binds 127.0.0.1:<port>, falling forward up to +9 if the port is taken (two
// instances on one box), and logs "HARNESS: control plane listening on ...".
// See HarnessControl.cpp for the command list.
class HarnessControl
{
public:
    static void start();

    // Port actually bound, 0 if disabled or not (yet) listening.
    static int port();
};

#endif
