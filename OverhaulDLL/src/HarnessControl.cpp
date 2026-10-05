/*
    DARK SOULS OVERHAUL -- rollback test-harness control plane.

    A localhost TCP server the orchestrator uses instead of the F6-F12 hotkeys
    (and instead of tailing dsoverhaul_logging.txt, which for a sandboxed
    instance lives inside Sandboxie's virtual filesystem). See HarnessControl.h
    for how it is enabled; it is inert otherwise.

    Protocol: one command per line, one JSON object per reply ({"ok":true,...}
    or {"ok":false,"error":"..."}). Commands run on the GAME THREAD (a MainLoop
    callback drains a queue), so anything that touches game state is safe and
    ordered with respect to frames; the socket thread waits up to 10 s for the
    reply. "ping" is answered on the socket thread. A connection that sends
    "subscribe" becomes a log stream: it gets {"log":"...","t":<ms>} for every
    ConsoleWrite from then on and accepts no further commands (open a second
    connection for those).

    Commands:
      ping                          liveness (no game thread needed)
      status                        instance/session snapshot (pid, sandboxed, char_loaded,
                                    rollback/ggpo state, frame, players, steam_id, replay/script)
      frame                         {"frame":N,"confirmed_frame":M} from ggpo_get_frame_info
      input                         the last local RollbackInput sent to GGPO, as named fields
      rollback on|off|toggle        Rollback::rollbackEnabled (F6). Set BEFORE the other player joins.
      network on|off                Rollback::networkTest (F7) -- ROLLBACK_INPUT_TESTING only
      record arm|disarm             RollbackReplay::record_armed (F12), applies at next session start
      record file <path>            file the next recording is written to
      replay file <path>            file replayed at next session start ("replay off" disables)
      script load <path>            replace the input script with a file (see RollbackScript.h)
      script add <directive>        append one script line, e.g. "script add @120-125 r1=1"
      script clear                  drop the script
      script neutral on|off         zero player-driven fields every frame
      script name <text>            label for status/logs
      script status                 just the script part of status
      end_session                   Rollback::rollback_end_session()
      start_session                 re-arm the session start (rollback_await_init) after end_session
      freeze on|off|status          WorldFreeze (WorldFreeze.h) without a GGPO session: disable enemies, freeze objects/bodies,
                                    block world damage. A session started while frozen keeps the freeze and ends it with the session
      warp <x> <y> <z> <yaw>        move the local character within its map with the game's own warp and refill its HP.
                                    Only without a GGPO session: a live-only change would be undone by the next load
      log <text>                    write "HARNESS: <text>" into the mod log (marker for alignment)
      subscribe                     turn this connection into a log stream
      hashes [since] [max]          confirmed per-frame state digests (StateHash.h ring) with frame > since,
                                    oldest first; "gap":true if the ring no longer reaches since+1
      dump_at <frame>|+<n>          capture the full canonical state text when GGPO saves that frame
                                    (absolute, or n frames past the current one); re-saves overwrite until confirmed
      dump_status                   dump_requested / dump_frame / dump_confirmed / dump_size / dump_file
      dump_get                      the captured text (+ dump_status fields); large (hundreds of KB)
      probe                         per connected player: hp, max_hp, x, y, z, rot -- a cheap "what is happening" view
      peek <hexaddr> [len]          raw bytes (at most 512), refused unless readable; for reverse engineering live objects
      giveitem <cat> <id> <qty> [select]  before a session: give the local player an item (cat weapon|protector|accessory|goods),
                                    into a free quickbar slot if it goes there; select makes it the selected quickbar item
      rtti <hexaddr>                MSVC RTTI class name of the object at an address (its vtable's CompleteObjectLocator)
      watch <hexaddr> [1|2|4|8] [slot 0-3]  hardware write watchpoint (DR0-3) on the game thread; records every instruction that writes there
      watch exec <hexaddr> [slot] [edx]  hardware execute breakpoint (optionally only calls with this edx): records rcx, rdx, r8 and the return address at [rsp] of each call
      watch off | watch log         clear them all | the recorded writers (slot, address after the write, module offset, new value, count, callers)
      synctest [reset]              GGPO_SYNCTEST builds: replayed-frame mismatch counts per subsystem
                                    (also in status); reset zeroes them and re-arms the state dumps
      synctest dump <subsys> [f|+n] only dump mismatches in these subsystems (all|none|player,damage,...),
                                    from frame f; also re-arms the dump budget
      help                          list commands
*/

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "HarnessControl.h"
#include "DarkSoulsOverhaulMod.h"
#include "GameData.h"
#include "ModData.h"
#include "MainLoop.h"
#include "ModNetworking.h"
#include "Rollback.h"
#include "RollbackReplay.h"
#include "RollbackScript.h"
#include "StateHash.h"
#include "VirtualPad.h"
#include "WorldFreeze.h"
#include "RollbackStartSync.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace
{
    // ---------------- request marshalling: socket thread -> game thread ----------------
    struct Request
    {
        std::string line;
        std::string response;
        bool done = false;
    };
    std::mutex g_req_mtx;
    std::condition_variable g_req_cv;
    std::deque<std::shared_ptr<Request>> g_requests;
    const int REQUEST_TIMEOUT_SECONDS = 10;

    // ---------------- log subscribers ----------------
    struct Subscriber
    {
        SOCKET sock = INVALID_SOCKET;
        std::mutex mtx;
        std::condition_variable cv;
        std::deque<std::string> lines;
        size_t dropped = 0;
    };
    std::mutex g_sub_mtx;
    std::vector<std::shared_ptr<Subscriber>> g_subs;
    const size_t SUB_MAX_QUEUE = 5000;

    std::atomic<int> g_port{ 0 };

    // ---------------- small helpers ----------------
    std::string b2s(bool b) { return b ? "true" : "false"; }
    std::string q(const std::string& s) { return "\"" + RollbackScript::json_escape(s) + "\""; }
    std::string ok_json(const std::string& extra = "") { return "{\"ok\":true" + (extra.empty() ? std::string() : "," + extra) + "}"; }
    std::string err_json(const std::string& msg) { return "{\"ok\":false,\"error\":" + q(msg) + "}"; }

    bool send_all(SOCKET s, const std::string& data)
    {
        size_t off = 0;
        while (off < data.size())
        {
            int n = send(s, data.data() + off, (int)(data.size() - off), 0);
            if (n <= 0) return false;
            off += (size_t)n;
        }
        return true;
    }
    bool send_line(SOCKET s, const std::string& line) { return send_all(s, line + "\n"); }

    std::vector<std::string> split_ws(const std::string& s)
    {
        std::vector<std::string> out;
        size_t i = 0;
        while (i < s.size())
        {
            while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
            size_t j = i;
            while (j < s.size() && s[j] != ' ' && s[j] != '\t') j++;
            if (j > i) out.push_back(s.substr(i, j - i));
            i = j;
        }
        return out;
    }

    // Text after the first `nwords` whitespace-separated tokens, verbatim (trimmed).
    std::string rest_after(const std::string& line, size_t nwords)
    {
        size_t pos = 0;
        for (size_t k = 0; k < nwords; k++)
        {
            pos = line.find_first_not_of(" \t", pos);
            if (pos == std::string::npos) return "";
            pos = line.find_first_of(" \t", pos);
            if (pos == std::string::npos) return "";
        }
        pos = line.find_first_not_of(" \t", pos);
        return pos == std::string::npos ? "" : RollbackScript::_trim(line.substr(pos));
    }

    bool parse_on_off(const std::string& s, bool current, bool* out)
    {
        if (s == "on" || s == "1" || s == "true" || s == "arm")  { *out = true;  return true; }
        if (s == "off" || s == "0" || s == "false" || s == "disarm") { *out = false; return true; }
        if (s == "toggle") { *out = !current; return true; }
        return false;
    }

    // ---------------- ConsoleWrite tap (any thread) ----------------
    void log_tap(uint64_t time_ms, const char* line)
    {
        std::string msg = "{\"log\":" + q(line) + ",\"t\":" + std::to_string(time_ms) + "}";
        std::lock_guard<std::mutex> g(g_sub_mtx);
        for (auto& sub : g_subs)
        {
            std::lock_guard<std::mutex> l(sub->mtx);
            if (sub->lines.size() >= SUB_MAX_QUEUE)
            {
                sub->lines.pop_front();
                sub->dropped++;
            }
            sub->lines.push_back(msg);
            sub->cv.notify_one();
        }
    }

    // ---------------- command execution (GAME THREAD) ----------------
    const char* ready_name(GGPOREADY r)
    {
        switch (r)
        {
        case GGPOREADY::Ready:                  return "Ready";
        case GGPOREADY::ReadyAwaitingFrameHead: return "AwaitingFrameHead";
        default:                                return "NotReady";
        }
    }

    void frame_info(int* frame, int* confirmed)
    {
        *frame = -1;
        *confirmed = -1;
        if (Rollback::ggpoStarted && Rollback::ggpo != NULL)
        {
            ggpo_get_frame_info(Rollback::ggpo, frame, confirmed);
        }
    }

    std::string world_freeze_json()
    {
        WorldFreeze::Status st = WorldFreeze::status();
        return "{\"frozen\":" + b2s(st.frozen) + ",\"chrs_disabled\":" + std::to_string(st.chrs)
            + ",\"objs_unbreakable\":" + std::to_string(st.objs) + ",\"bodies_fixed\":" + std::to_string(st.bodies)
            + ",\"player_bodies_awake\":" + std::to_string(st.awake) + ",\"damage_blocked\":" + std::to_string(st.blocked_damage)
            + ",\"world_bodies_fixed\":" + std::to_string(st.world_fixed) + ",\"world_bodies_movable\":" + std::to_string(st.world_movable) + "}";
    }

    std::string status_fields()
    {
        std::string s;
        s += "\"pid\":" + std::to_string((unsigned long)GetCurrentProcessId());
        s += ",\"control_port\":" + std::to_string(g_port.load());
        s += ",\"sandboxed\":" + b2s(GetModuleHandleA("SbieDll.dll") != NULL);
        s += ",\"seamless_coop\":" + b2s(is_seamless_coop_present());
        s += ",\"mod_mode\":" + std::to_string((int)Mod::get_mode());
        bool loaded = Game::playerchar_is_loaded();
        s += ",\"char_loaded\":" + b2s(loaded);
        s += ",\"rollback_enabled\":" + b2s(Rollback::rollbackEnabled);
#if ROLLBACK_INPUT_TESTING
        s += ",\"network_test\":" + b2s(Rollback::networkTest);
#endif
        s += ",\"ggpo_started\":" + b2s(Rollback::ggpoStarted);
        s += ",\"ggpo_ready\":" + q(ready_name(Rollback::ggpoReady));
        s += ",\"start_sync\":" + RollbackStartSync::status_json();
        int frame, confirmed;
        frame_info(&frame, &confirmed);
        s += ",\"frame\":" + std::to_string(frame);
        s += ",\"confirmed_frame\":" + std::to_string(confirmed);

        int players = 0;
        std::string steam;
        if (loaded)
        {
            for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
            {
                auto p = Game::get_connected_player(i);
                if (p.has_value() && p.value() != 0) players++;
            }
            auto pc = Game::get_PlayerIns();
            if (pc.has_value() && pc.value() != NULL)
            {
                PlayerIns* pi = (PlayerIns*)pc.value();
                if (pi->steamPlayerData != NULL && pi->steamPlayerData->steamOnlineIDData != NULL)
                {
                    char b[32];
                    snprintf(b, sizeof(b), "%llx", (unsigned long long)pi->steamPlayerData->steamOnlineIDData->steam_id);
                    steam = b;
                }
            }
        }
        s += ",\"connected_players\":" + std::to_string(players);
        s += ",\"steam_id\":" + q(steam);
        s += ",\"replay_mode\":" + q(RollbackReplay::mode_name());
        s += ",\"record_armed\":" + b2s(RollbackReplay::record_armed);
        s += ",\"replay_file\":" + q(RollbackReplay::replay_file);
        s += ",\"record_file\":" + q(RollbackReplay::record_file);
        s += ",\"script\":" + RollbackScript::status_json();
        s += ",\"world_freeze\":" + world_freeze_json();
#ifdef GGPO_SYNCTEST
        s += ",\"synctest\":" + RollbackHash::synctest_json();
#endif
        return s;
    }

#if ROLLBACK_INPUT_TESTING
    std::string network_test_fields()
    {
        std::string s = "\"network_test\":" + b2s(Rollback::networkTest);
        s += ",\"role\":" + q(Rollback::networkTestRole == NetworkTestRole::Send ? "send" : "recv");
        s += ",\"sent\":" + std::to_string(Rollback::networkTestSent);
        s += ",\"received\":" + std::to_string(Rollback::networkTestRecv);
        s += ",\"missed\":" + std::to_string(Rollback::networkTestMissed);
        return s;
    }
#endif

// True if [addr, addr+len) is committed, readable memory. Not an SEH guard: the mod's vectored exception handler
    // (CrashHandler.cpp) sees every access violation before any __except does, and brings up the crash report.
    bool mem_readable(uint64_t addr, size_t len)
    {
        while (len > 0)
        {
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery((const void*)addr, &mbi, sizeof(mbi)) == 0 || mbi.State != MEM_COMMIT
                || (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0
                || (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) == 0)
            {
                return false;
            }
            const uint64_t region_end = (uint64_t)mbi.BaseAddress + mbi.RegionSize;
            const uint64_t step = region_end - addr < len ? region_end - addr : len;
            addr += step;
            len -= (size_t)step;
        }
        return true;
    }

    // Checked raw read for the reverse-engineering commands
    bool safe_read(uint64_t addr, void* out, size_t len)
    {
        if (addr == 0 || !mem_readable(addr, len)) return false;
        memcpy(out, (const void*)addr, len);
        return true;
    }

    // MSVC x64 RTTI: vtable[-1] is the CompleteObjectLocator {signature 1, offset, cdOffset, TypeDescriptor RVA @0xc,
    // ClassHierarchyDescriptor RVA @0x10, own RVA @0x14}; the TypeDescriptor's decorated name starts at +0x10
    std::string rtti_name(uint64_t obj)
    {
        uint64_t vtable = 0, col = 0;
        if (!safe_read(obj, &vtable, 8) || vtable == 0 || !safe_read(vtable - 8, &col, 8) || col == 0) return "";
        uint32_t sig = 0, td_rva = 0, self_rva = 0;
        if (!safe_read(col, &sig, 4) || !safe_read(col + 0xc, &td_rva, 4) || !safe_read(col + 0x14, &self_rva, 4) || sig != 1) return "";
        char name[160] = {};
        if (!safe_read(col - self_rva + td_rva + 0x10, name, sizeof(name) - 1)) return "";
        return std::string(name);
    }

    // ---- hardware write watchpoint (reverse engineering) ----
    // DR0 on the game thread, write-only. A data breakpoint raises EXCEPTION_SINGLE_STEP after the writing instruction; it is not
    // an 0xC... code, so the crash handler's vectored handler ignores it, and ours (added first) records it and continues.
    //callers: the first stack slots that point into the game's code, a poor man's backtrace (the writer may be reached
    //through Arxan's jump-return stubs, which leave no static callers)
    struct WatchHit { uint32_t slot; uint64_t rip; uint64_t value; uint32_t count; uint64_t callers[8]; uint64_t rcx, rdx, r8, ret; int32_t frame; bool resim; };

    //A stack slot is taken as a return address only if the bytes before it are a call: E8 rel32, or FF /2 (call reg,
    //call [reg], call [reg+disp8], call [reg+disp32], call [rip+disp32], with or without a REX prefix)
    bool follows_call(uint64_t ret)
    {
        const uint8_t* b = (const uint8_t*)ret;
        if (b[-5] == 0xE8) return true;
        for (int len = 2; len <= 7; len++)
        {
            const uint8_t* c = b - len;
            if (c[0] == 0xFF && ((c[1] >> 3) & 7) == 2) return true;
            if ((c[0] & 0xF0) == 0x40 && c[1] == 0xFF && ((c[2] >> 3) & 7) == 2) return true;
        }
        return false;
    }
    std::mutex watch_mutex;
    std::vector<WatchHit> watch_hits;
    uint64_t watch_addr[4] = {};
    uint32_t watch_len[4] = {};
    bool watch_exec[4] = {};
    int64_t watch_rdx_filter[4] = { -1, -1, -1, -1 };   //exec breakpoints: record only calls whose edx is this (-1 = all)
    PVOID watch_handler = NULL;

    LONG WINAPI watch_exception_handler(EXCEPTION_POINTERS* info)
    {
        if (info->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP || (info->ContextRecord->Dr6 & 0xF) == 0)
        {
            return EXCEPTION_CONTINUE_SEARCH;
        }
        uint32_t slot = 0;
        while (slot < 3 && (info->ContextRecord->Dr6 & (1ULL << slot)) == 0) slot++;
        info->ContextRecord->Dr6 = 0;
        if (watch_addr[slot] == 0)
        {
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        uint64_t value = 0;
        const uint64_t rip = (uint64_t)info->ExceptionRecord->ExceptionAddress;
        WatchHit hit = { slot, rip, 0, 1, {} };
        if (watch_exec[slot])
        {
            //an instruction breakpoint fires before the instruction runs: resume flag, or it fires again forever
            info->ContextRecord->EFlags |= 0x10000;
            if (watch_rdx_filter[slot] >= 0 && (uint32_t)info->ContextRecord->Rdx != (uint32_t)watch_rdx_filter[slot])
            {
                return EXCEPTION_CONTINUE_EXECUTION;
            }
            hit.rcx = info->ContextRecord->Rcx;
            hit.rdx = info->ContextRecord->Rdx;
            hit.r8 = info->ContextRecord->R8;
            hit.ret = *(const uint64_t*)info->ContextRecord->Rsp;
        }
        else
        {
            memcpy(&value, (const void*)watch_addr[slot], watch_len[slot]);
            hit.value = value;
        }
        //the GGPO frame being simulated, and whether it is a re-simulation after a rollback
        hit.frame = -1;
        if (Rollback::ggpoStarted && Rollback::ggpo != NULL)
        {
            int f = 0, c = 0;
            ggpo_get_frame_info(Rollback::ggpo, &f, &c);
            hit.frame = f;
        }
        hit.resim = Rollback::inRollbackResim;
        //writes from outside the game (the mod restoring saved state) are not told apart by frame, or they fill the log
        if (rip < Game::ds1_base || rip > Game::ds1_base + 0x3200000)
        {
            hit.frame = -1;
        }
        const uint64_t* stack = (const uint64_t*)info->ContextRecord->Rsp;
        for (size_t i = 0, n = 0; i < 256 && n < 8; i++)
        {
            //the game's .text (0x1000..0x1ae0000) and the Arxan-rewritten code sections (0x2019000..)
            const uint64_t v = stack[i];
            if (v > Game::ds1_base + 0x1010 && v < Game::ds1_base + 0x3200000 && (v < Game::ds1_base + 0x1ae0000 || v > Game::ds1_base + 0x2019010)
                && follows_call(v))
            {
                hit.callers[n++] = v;
            }
        }
        {
            std::lock_guard<std::mutex> lock(watch_mutex);
            bool found = false;
            for (WatchHit& h : watch_hits)
            {
                if (h.slot == slot && h.rip == rip && h.value == hit.value && h.rcx == hit.rcx && h.rdx == hit.rdx && h.r8 == hit.r8 && h.ret == hit.ret
                    && h.frame == hit.frame && h.resim == hit.resim
                    && memcmp(h.callers, hit.callers, sizeof(hit.callers)) == 0) { h.count++; found = true; break; }
            }
            if (!found && watch_hits.size() < 256) watch_hits.push_back(hit);
        }
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // Debug registers can only be changed on a suspended thread, so a helper thread does it
    struct WatchSet { DWORD thread_id; uint32_t slot; uint64_t addr; uint32_t len; bool exec; bool clear_all; bool ok; };
    DWORD WINAPI watch_set_thread(LPVOID p)
    {
        WatchSet* w = (WatchSet*)p;
        HANDLE t = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, w->thread_id);
        if (t == NULL) return 0;
        if (SuspendThread(t) != (DWORD)-1)
        {
            CONTEXT c = {};
            c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
            if (GetThreadContext(t, &c))
            {
                if (w->clear_all)
                {
                    c.Dr0 = c.Dr1 = c.Dr2 = c.Dr3 = 0;
                    c.Dr7 = 0;
                }
                else
                {
                    (&c.Dr0)[w->slot] = w->addr;
                    const uint64_t len_bits = w->len == 8 ? 2 : (w->len == 4 ? 3 : (w->len == 2 ? 1 : 0));
                    c.Dr7 &= ~(((uint64_t)0xF << (16 + 4 * w->slot)) | (3ULL << (2 * w->slot)));
                    //Ln, RWn = write (or 00 execute, with LEN 00), LENn
                    if (w->exec)
                    {
                        c.Dr7 |= (1ULL << (2 * w->slot));
                    }
                    else
                    {
                        c.Dr7 |= (1ULL << (2 * w->slot)) | (1ULL << (16 + 4 * w->slot)) | (len_bits << (18 + 4 * w->slot));
                    }
                }
                c.Dr6 = 0;
                w->ok = SetThreadContext(t, &c) != 0;
            }
            ResumeThread(t);
        }
        CloseHandle(t);
        return 0;
    }

    bool watch_set(uint32_t slot, uint64_t addr, uint32_t len, bool exec = false)
    {
        if (watch_handler == NULL)
        {
            watch_handler = AddVectoredExceptionHandler(1, watch_exception_handler);
        }
        const bool clear_all = addr == 0;
        WatchSet w = { GetCurrentThreadId(), slot, addr, len, exec, clear_all, false };
        //called on the game thread (the harness pump), which is the one to watch
        if (clear_all)
        {
            for (int i = 0; i < 4; i++) watch_addr[i] = 0;
        }
        else
        {
            watch_addr[slot] = 0;
        }
        HANDLE h = CreateThread(NULL, 0, watch_set_thread, &w, 0, NULL);
        if (h == NULL) return false;
        //the helper suspends this thread, so wait without holding anything
        WaitForSingleObject(h, 5000);
        CloseHandle(h);
        if (w.ok && !clear_all)
        {
            std::lock_guard<std::mutex> lock(watch_mutex);
            watch_len[slot] = len;
            watch_exec[slot] = exec;
            watch_addr[slot] = addr;
        }
        return w.ok;
    }

    std::string hex64(uint64_t v)
    {
        char b[24];
        snprintf(b, sizeof(b), "0x%llx", (unsigned long long)v);
        return b;
    }

    std::string execute(const std::string& line)
    {
        std::vector<std::string> a = split_ws(line);
        if (a.empty()) return err_json("empty command");
        const std::string& cmd = a[0];
        const std::string sub = a.size() > 1 ? a[1] : "";

        if (cmd == "status") return ok_json(status_fields());

        if (cmd == "frame")
        {
            int frame, confirmed;
            frame_info(&frame, &confirmed);
            return ok_json("\"frame\":" + std::to_string(frame) + ",\"confirmed_frame\":" + std::to_string(confirmed));
        }

        if (cmd == "input") return ok_json("\"input\":" + RollbackScript::input_json());

        if (cmd == "rollback")
        {
            bool v;
            if (!parse_on_off(sub, Rollback::rollbackEnabled, &v)) return err_json("usage: rollback on|off|toggle");
            Rollback::rollbackEnabled = v;
            ConsoleWrite("HARNESS: rollback %d", (int)v);
            std::string extra = "\"rollback_enabled\":" + b2s(v);
            if (Rollback::ggpoStarted) extra += ",\"warning\":\"GGPO session already running; takes effect at the next session\"";
            return ok_json(extra);
        }

#if ROLLBACK_INPUT_TESTING
        if (cmd == "network")
        {
            //no-GGPO input loopback: "network role send" on the instance holding the controller,
            //"network role recv" on the other, then "network on" on both.
            if (sub == "role")
            {
                const std::string r = a.size() > 2 ? a[2] : "";
                if (r == "send") Rollback::networkTestRole = NetworkTestRole::Send;
                else if (r == "recv") Rollback::networkTestRole = NetworkTestRole::Recv;
                else return err_json("usage: network role send|recv");
                Rollback::networkTestSent = Rollback::networkTestRecv = Rollback::networkTestMissed = 0;
                ConsoleWrite("HARNESS: networkTest role %s", r.c_str());
                return ok_json(network_test_fields());
            }
            if (sub == "status") return ok_json(network_test_fields());
            bool v = false;
            if (!parse_on_off(sub, Rollback::networkTest, &v)) return err_json("usage: network on|off|toggle | network role send|recv | network status");
            Rollback::networkTest = v;
            Rollback::networkTestSent = Rollback::networkTestRecv = Rollback::networkTestMissed = 0;
            ConsoleWrite("HARNESS: networkTest %d", v);
            return ok_json(network_test_fields());
        }
#endif

        if (cmd == "record")
        {
            if (sub == "file")
            {
                std::string path = rest_after(line, 2);
                if (path.empty()) return err_json("usage: record file <path>");
                RollbackReplay::record_file = path;
                return ok_json("\"record_file\":" + q(path));
            }
            bool v;
            if (!parse_on_off(sub, RollbackReplay::record_armed, &v)) return err_json("usage: record arm|disarm | record file <path>");
            RollbackReplay::record_armed = v;
            ConsoleWrite("HARNESS: rollback input record armed=%d (applies at next session start)", (int)v);
            return ok_json("\"record_armed\":" + b2s(v));
        }

        if (cmd == "replay")
        {
            if (sub == "file")
            {
                std::string path = rest_after(line, 2);
                if (path.empty()) return err_json("usage: replay file <path>");
                RollbackReplay::replay_file = path;
                return ok_json("\"replay_file\":" + q(path));
            }
            if (sub == "off")
            {
                RollbackReplay::replay_file.clear();   // nothing to open => Off at next session start
                return ok_json("\"replay_file\":\"\"");
            }
            return err_json("usage: replay file <path> | replay off");
        }

        if (cmd == "script")
        {
            std::string err;
            if (sub == "load")
            {
                std::string path = rest_after(line, 2);
                if (path.empty()) return err_json("usage: script load <path>");
                if (!RollbackScript::load_file(path.c_str(), &err)) return err_json(err);
                ConsoleWrite("HARNESS: script loaded from '%s' (%d directives)", path.c_str(), (int)RollbackScript::directives.size());
                return ok_json("\"script\":" + RollbackScript::status_json());
            }
            if (sub == "add")
            {
                std::string directive = rest_after(line, 2);
                if (directive.empty()) return err_json("usage: script add <directive>");
                if (!RollbackScript::loaded) RollbackScript::source = "control";
                int line_no = (int)RollbackScript::directives.size() + 1;
                if (!RollbackScript::add_line(directive, line_no, &err)) return err_json(err);
                if (RollbackScript::neutral) RollbackScript::loaded = true;
                return ok_json("\"script\":" + RollbackScript::status_json());
            }
            if (sub == "clear")
            {
                RollbackScript::clear();
                ConsoleWrite("HARNESS: script cleared");
                return ok_json("\"script\":" + RollbackScript::status_json());
            }
            if (sub == "neutral")
            {
                bool v;
                if (!parse_on_off(a.size() > 2 ? a[2] : "", RollbackScript::neutral, &v)) return err_json("usage: script neutral on|off");
                RollbackScript::neutral = v;
                if (v) { RollbackScript::loaded = true; if (RollbackScript::source.empty()) RollbackScript::source = "control"; }
                return ok_json("\"script\":" + RollbackScript::status_json());
            }
            if (sub == "name")
            {
                RollbackScript::name = rest_after(line, 2);
                return ok_json("\"script\":" + RollbackScript::status_json());
            }
            if (sub == "status") return ok_json("\"script\":" + RollbackScript::status_json());
            return err_json("usage: script load <path> | add <directive> | clear | neutral on|off | name <text> | status");
        }

        if (cmd == "end_session")
        {
            bool was = Rollback::ggpoStarted;
            Rollback::rollback_end_session();
            ConsoleWrite("HARNESS: end_session (was_started=%d)", (int)was);
            return ok_json("\"was_started\":" + b2s(was));
        }

        if (cmd == "start_session")
        {
            if (Rollback::ggpoStarted) return err_json("a GGPO session is already running");
            Rollback::rollback_start_session(NULL);
            ConsoleWrite("HARNESS: start_session");
            return ok_json("\"rollback_enabled\":" + b2s(Rollback::rollbackEnabled));
        }

        if (cmd == "freeze")
        {
            if (sub == "" || sub == "status") return ok_json("\"world_freeze\":" + world_freeze_json());
            if (sub != "on" && sub != "off") return err_json("usage: freeze on|off|status");
            if (Rollback::ggpoStarted) return err_json("a GGPO session is running: it froze the world itself and unfreezes it when it ends");
            if (sub == "on")
            {
                if (!Game::playerchar_is_loaded()) return err_json("character not loaded");
                WorldFreeze::freeze();
            }
            else
            {
                WorldFreeze::unfreeze();
            }
            ConsoleWrite("HARNESS: freeze %s", sub.c_str());
            return ok_json("\"world_freeze\":" + world_freeze_json());
        }

        if (cmd == "warp")
        {
            if (Rollback::ggpoStarted) return err_json("end the GGPO session first (end_session): the next load would undo the warp");
            if (a.size() < 5) return err_json("usage: warp <x> <y> <z> <yaw>");
            if (!Game::playerchar_is_loaded()) return err_json("character not loaded");
            auto p = Game::get_connected_player(0);
            if (!p.has_value() || p.value() == 0) return err_json("no local player");
            PlayerIns* pi = (PlayerIns*)p.value();
            HavokChara* hc = (pi->chrins.playerCtrl != NULL) ? pi->chrins.playerCtrl->chrCtrl.havokChara : NULL;
            if (hc == NULL) return err_json("no HavokChara");
            //FieldArea_RemoteWarpPlayer (the debug menu's warp) ends in FieldArea_WarpHostPlayer(FieldArea, &hit map id, &position,
            //&rotation); passing the player's own map id takes the same-map path
            typedef int32_t* PlayerIns_Get_MapId_FUNC(PlayerIns*, int32_t*);
            typedef void FieldArea_WarpHostPlayer_FUNC(void* field_area, int32_t* map_id, float* position, float* rotation);
            PlayerIns_Get_MapId_FUNC* PlayerIns_Get_MapId = (PlayerIns_Get_MapId_FUNC*)0x140359d80;
            FieldArea_WarpHostPlayer_FUNC* FieldArea_WarpHostPlayer = (FieldArea_WarpHostPlayer_FUNC*)0x1403ce050;
            void* field_area = *(void**)((uint8_t*)Game::get_MoveMapStep() + 0x60);
            if (field_area == NULL) return err_json("no FieldArea");
            int32_t map_id = 0;
            PlayerIns_Get_MapId(pi, &map_id);
            alignas(16) float position[4] = { std::stof(a[1]), std::stof(a[2]), std::stof(a[3]), 0.0f };
            alignas(16) float rotation[4] = {};
            memcpy(rotation, hc->RotAngleUnkWep, sizeof(rotation));
            rotation[1] = std::stof(a[4]);
            FieldArea_WarpHostPlayer(field_area, &map_id, position, rotation);
            pi->chrins.curHp = pi->chrins.maxHp;
            ConsoleWrite("HARNESS: warp %s %s %s %s", a[1].c_str(), a[2].c_str(), a[3].c_str(), a[4].c_str());
            return ok_json("\"map_id\":" + std::to_string(map_id));
        }

        if (cmd == "log")
        {
            std::string text = rest_after(line, 1);
            ConsoleWrite("HARNESS: %s", text.c_str());
            return ok_json();
        }

        // ---- determinism oracle (StateHash.h) ----
#ifdef GGPO_SYNCTEST
        if (cmd == "synctest")
        {
            if (sub == "reset")
            {
                RollbackHash::synctest = RollbackHash::SyncTestStats{};
            }
            else if (sub == "dump")
            {
                uint32_t mask;
                if (a.size() < 3 || !RollbackHash::synctest_parse_mask(a[2], mask))
                    return err_json("usage: synctest dump <all|none|player,bullet,damage,havok,throw,dmghit> [frame|+n]");
                int from_frame = -1;
                if (a.size() > 3)
                {
                    int frame, confirmed;
                    frame_info(&frame, &confirmed);
                    from_frame = a[3][0] == '+' ? frame + atoi(a[3].c_str() + 1) : atoi(a[3].c_str());
                }
                RollbackHash::synctest_dump.mask = mask;
                RollbackHash::synctest_dump.from_frame = from_frame;
                RollbackHash::synctest.dump_files_written = 0;
            }
            else if (sub == "unrestored")
            {
                if (a.size() > 2 && a[2] == "reset")
                {
                    synctest_unrestored_reset();
                }
                size_t top = (a.size() > 2 && a[2] != "reset") ? (size_t)atoi(a[2].c_str()) : 40;
                return ok_json("\"unrestored\":" + synctest_unrestored_json(top));
            }
            else if (!sub.empty())
            {
                return err_json("usage: synctest [reset | dump <subsystems> [frame|+n] | unrestored [top|reset]]");
            }
            return ok_json("\"synctest\":" + RollbackHash::synctest_json());
        }
#endif

        if (cmd == "hashes")
        {
            int since = -1;
            int maxn = 2000;
            if (a.size() > 1) since = atoi(a[1].c_str());
            if (a.size() > 2) maxn = atoi(a[2].c_str());
            if (maxn <= 0 || maxn > (int)RollbackHash::HISTORY_MAX) maxn = (int)RollbackHash::HISTORY_MAX;
            int frame, confirmed;
            frame_info(&frame, &confirmed);
            return ok_json("\"frame\":" + std::to_string(frame) + ",\"confirmed_frame\":" + std::to_string(confirmed)
                           + ",\"ggpo_started\":" + b2s(Rollback::ggpoStarted)
                           + ",\"hashes\":" + RollbackHash::hashes_json(since, maxn));
        }

        if (cmd == "dump_at")
        {
            if (sub.empty()) return err_json("usage: dump_at <frame> | dump_at +<frames ahead>");
            int frame, confirmed;
            frame_info(&frame, &confirmed);
            int target;
            if (sub[0] == '+')
            {
                if (frame < 0) return err_json("no GGPO session running; use an absolute frame");
                target = frame + atoi(sub.c_str() + 1);
            }
            else
            {
                target = atoi(sub.c_str());
            }
            if (target < 0) return err_json("bad frame '" + sub + "'");
            RollbackHash::dump_request_frame = target;
            ConsoleWrite("HARNESS: state dump requested for frame %d (current %d)", target, frame);
            return ok_json("\"dump_requested\":" + std::to_string(target) + ",\"frame\":" + std::to_string(frame));
        }

        if (cmd == "dump_status") return ok_json(RollbackHash::dump_status_fields());

        if (cmd == "dump_get")
        {
            if (RollbackHash::dump_frame < 0) return err_json("no state dump captured yet (use dump_at)");
            return ok_json(RollbackHash::dump_status_fields() + ",\"text\":" + q(RollbackHash::dump_text_store));
        }

        if (cmd == "probe")
        {
            int frame, confirmed;
            frame_info(&frame, &confirmed);
            std::string s = "\"frame\":" + std::to_string(frame) + ",\"confirmed_frame\":" + std::to_string(confirmed) + ",\"players\":[";
            int n = 0;
            if (Game::playerchar_is_loaded())
            {
                for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
                {
                    auto p = Game::get_connected_player(i);
                    if (!p.has_value() || p.value() == 0) continue;
                    PlayerIns* pi = (PlayerIns*)p.value();
                    if (n) s += ",";
                    s += "{\"slot\":" + std::to_string(i);
                    s += ",\"playerins\":" + q(hex64((uint64_t)pi));
                    s += ",\"playerctrl\":" + q(hex64((uint64_t)pi->chrins.playerCtrl));
                    s += ",\"hp\":" + std::to_string(pi->chrins.curHp) + ",\"max_hp\":" + std::to_string(pi->chrins.maxHp);
                    HavokChara* hc = (pi->chrins.playerCtrl != NULL) ? pi->chrins.playerCtrl->chrCtrl.havokChara : NULL;
                    if (hc != NULL)
                    {
                        s += ",\"x\":" + RollbackScript::json_number(hc->current_coords[0]);
                        s += ",\"y\":" + RollbackScript::json_number(hc->current_coords[1]);
                        s += ",\"z\":" + RollbackScript::json_number(hc->current_coords[2]);
                        // the rotation the mod treats as authoritative (PadManipulatorPacked_to_PadManipulator writes it here)
                        s += ",\"rot\":" + RollbackScript::json_number(*(float*)((uint8_t*)hc + 4));
                    }
                    s += "}";
                    n++;
                }
            }
            s += "]";
            return ok_json(s);
        }

        if (cmd == "watch")
        {
            if (sub == "off")
            {
                const bool ok = watch_set(0, 0, 0);
                {
                    std::lock_guard<std::mutex> lock(watch_mutex);
                    watch_hits.clear();
                }
                return ok_json("\"cleared\":" + b2s(ok));
            }
            if (sub == "log")
            {
                std::string s = "\"addresses\":[" + q(hex64(watch_addr[0])) + "," + q(hex64(watch_addr[1])) + "," + q(hex64(watch_addr[2])) + "," + q(hex64(watch_addr[3])) + "],\"hits\":[";
                std::lock_guard<std::mutex> lock(watch_mutex);
                for (size_t i = 0; i < watch_hits.size(); i++)
                {
                    const WatchHit& h = watch_hits[i];
                    const bool in_game = h.rip >= Game::ds1_base && h.rip < Game::ds1_base + 0x4000000;
                    if (i) s += ",";
                    std::string callers;
                    for (uint64_t c : h.callers)
                    {
                        if (c == 0) break;
                        if (!callers.empty()) callers += ",";
                        callers += q(hex64(c - Game::ds1_base));
                    }
                    s += "{\"slot\":" + std::to_string(h.slot) + ",\"rcx\":" + q(hex64(h.rcx)) + ",\"rdx\":" + q(hex64(h.rdx)) + ",\"r8\":" + q(hex64(h.r8))
                        + ",\"ret\":" + q(h.ret >= Game::ds1_base && h.ret < Game::ds1_base + 0x4000000 ? hex64(h.ret - Game::ds1_base) : hex64(h.ret))
                        + ",\"rip\":" + q(hex64(h.rip)) + ",\"game_offset\":" + q(in_game ? hex64(h.rip - Game::ds1_base) : "") + ",\"value\":" + q(hex64(h.value)) + ",\"count\":" + std::to_string(h.count) + ",\"frame\":" + std::to_string(h.frame) + ",\"resim\":" + (h.resim ? "true" : "false") + ",\"callers\":[" + callers + "]}";
                }
                return ok_json(s + "]");
            }
            if (sub.empty()) return err_json("usage: watch <hexaddr> [1|2|4|8] [slot 0-3] | watch exec <hexaddr> [slot] | watch off | watch log");
            if (sub == "exec")
            {
                if (a.size() < 3) return err_json("usage: watch exec <hexaddr> [slot]");
                uint64_t addr = 0;
                try { addr = std::stoull(a[2], nullptr, 16); } catch (...) { return err_json("bad address"); }
                uint32_t slot = 0;
                if (a.size() > 3) { try { slot = (uint32_t)std::stoul(a[3]); } catch (...) { return err_json("bad slot"); } }
                if (slot > 3) return err_json("slot must be 0..3");
                watch_rdx_filter[slot] = -1;
                if (a.size() > 4) { try { watch_rdx_filter[slot] = (int64_t)(uint32_t)std::stoul(a[4], nullptr, 0); } catch (...) { return err_json("bad edx filter"); } }
                if (!mem_readable(addr, 1)) return err_json("unreadable");
                if (!watch_set(slot, addr, 1, true)) return err_json("could not set the debug registers");
                return ok_json("\"exec\":" + q(hex64(addr)) + ",\"slot\":" + std::to_string(slot));
            }
            uint64_t addr = 0;
            try { addr = std::stoull(sub, nullptr, 16); } catch (...) { return err_json("bad address"); }
            uint32_t len = 1;
            if (a.size() > 2) { try { len = (uint32_t)std::stoul(a[2]); } catch (...) { return err_json("bad length"); } }
            if (len != 1 && len != 2 && len != 4 && len != 8) return err_json("length must be 1, 2, 4 or 8");
            uint32_t slot = 0;
            if (a.size() > 3) { try { slot = (uint32_t)std::stoul(a[3]); } catch (...) { return err_json("bad slot"); } }
            if (slot > 3) return err_json("slot must be 0..3");
            if ((addr & (len - 1)) != 0) return err_json("address must be aligned to the length");
            if (!mem_readable(addr, len)) return err_json("unreadable");
            if (!watch_set(slot, addr, len)) return err_json("could not set the debug registers");
            return ok_json("\"address\":" + q(hex64(addr)) + ",\"length\":" + std::to_string(len) + ",\"slot\":" + std::to_string(slot));
        }

        if (cmd == "peek" || cmd == "rtti")
        {
            if (sub.empty()) return err_json("usage: peek <hexaddr> [len] | rtti <hexaddr>");
            uint64_t addr = 0;
            try { addr = std::stoull(sub, nullptr, 16); } catch (...) { return err_json("bad address"); }
            if (cmd == "rtti")
            {
                uint64_t vtable = 0;
                safe_read(addr, &vtable, 8);
                return ok_json("\"address\":" + q(hex64(addr)) + ",\"vtable\":" + q(hex64(vtable)) + ",\"class\":" + q(rtti_name(addr)));
            }
            size_t len = 64;
            if (a.size() > 2) { try { len = std::stoul(a[2]); } catch (...) { return err_json("bad length"); } }
            if (len == 0 || len > 512) return err_json("length must be 1..512");
            std::vector<uint8_t> buf(len);
            if (!safe_read(addr, buf.data(), len)) return err_json("unreadable");
            std::string h;
            char t[4];
            for (uint8_t c : buf) { snprintf(t, sizeof(t), "%02x", c); h += t; }
            return ok_json("\"address\":" + q(hex64(addr)) + ",\"hex\":" + q(h));
        }

        //Test setup: give the local player an item with the game's own GiveItemToPlayer, forced into the first free quickbar
        //slot when it is equippable there, and optionally select it. Refused once a session runs (the start-state handshake
        //sends the inventory to the other players, and during a session only the input may change it).
        if (cmd == "giveitem")
        {
            if (a.size() < 4) return err_json("usage: giveitem <category: weapon|protector|accessory|goods> <id> <quantity> [select]");
            if (Rollback::ggpoStarted) return err_json("a session is running");
            uint32_t category;
            if (a[1] == "weapon") category = 0x00000000;
            else if (a[1] == "protector") category = 0x10000000;
            else if (a[1] == "accessory") category = 0x20000000;
            else if (a[1] == "goods") category = 0x40000000;
            else return err_json("unknown category");
            int32_t id = 0, quantity = 0;
            try { id = std::stoi(a[2]); quantity = std::stoi(a[3]); } catch (...) { return err_json("bad id or quantity"); }
            auto local_o = Game::get_connected_player(0);
            if (!local_o.has_value() || local_o.value() == 0) return err_json("no local player");
            PlayerIns* local = (PlayerIns*)local_o.value();
            EquipGameData* egd = &local->playergamedata->equipGameData;
            //NS_FRPG::EquipParam::GiveItemToPlayer(EquipGameData*, category, id, quantity, unused, allow_trophy, ?, attempt_auto_equip):
            //attempt_auto_equip false equips goods into the first free quickbar slot regardless of the param's isAutoEquip
            typedef int32_t (*GiveItemToPlayer_t)(EquipGameData*, uint32_t, int32_t, int32_t, bool, bool, bool, bool);
            const int32_t index = ((GiveItemToPlayer_t)(Game::ds1_base + 0x7479e0))(egd, category, id, quantity, false, false, false, false);
            if (index < 0) return err_json("GiveItemToPlayer refused it (" + std::to_string(index) + ")");
            if (a.size() > 4 && a[4] == "select")
            {
                egd->equippedItemsInQuickbar.selectedQuickbarItem = (uint32_t)index;
            }
            std::string bar;
            for (int i = 0; i < 5; i++) bar += (i ? "," : "") + std::to_string((int32_t)egd->equippedItemsInQuickbar.quickbar[i]);
            return ok_json("\"inventory_index\":" + std::to_string(index) + ",\"quickbar\":[" + bar + "],\"selected\":" +
                std::to_string((int32_t)egd->equippedItemsInQuickbar.selectedQuickbarItem));
        }

        //Per-player view of what the input path actually delivered. Press the button in
        //question and read this on BOTH instances: it says whether the button bit arrived,
        //whether Step_PadManipulator turned it into an action, and whether the item/weapon
        //state that cannot be derived locally came across.
#if ROLLBACK_INPUT_TESTING
        if (cmd == "inputdiag")
        {
            if (sub == "reset")
            {
                for (uint32_t i = 0; i < GGPO_MAX_PLAYERS; i++) Rollback::inputDiag[i] = InputDiag{};
                return ok_json("\"reset\":true");
            }
            std::string s = "\"players\":[";
            int n = 0;
            for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
            {
                auto p = Game::get_connected_player(i);
                if (!p.has_value() || p.value() == 0) continue;
                PlayerIns* pi = (PlayerIns*)p.value();
                const RollbackInput& in = Rollback::lastAppliedInput[i];
                if (n) s += ",";
                s += "{\"slot\":" + std::to_string(i);
                s += ",\"local\":" + b2s(i == 0);
                const InputDiag& d = Rollback::inputDiag[i];
                //the latched view -- this is the one to read after pressing something
                s += ",\"seen\":{\"frames\":" + std::to_string(d.frames);
                s += ",\"use\":" + b2s(d.use_seen != 0);
                s += ",\"twohand\":" + std::to_string(d.twohand_seen);
                s += ",\"left_slot\":" + std::to_string(d.left_slot_or);
                s += ",\"right_slot\":" + std::to_string(d.right_slot_or);
                s += ",\"item_override\":" + std::to_string(d.item_override_last);
                s += ",\"l_index_mask\":" + std::to_string(d.l_index_mask);
                s += ",\"r_index_mask\":" + std::to_string(d.r_index_mask);
                s += ",\"style_mask\":" + std::to_string(d.style_mask);
                s += ",\"enable\":" + std::to_string(d.enable_or);
                s += ",\"gate_skip\":" + std::to_string(d.gate_skip);
                s += ",\"recv_state\":" + std::to_string(d.recv_state);
                s += ",\"item_use_override\":" + std::to_string(d.item_use_override);
                s += ",\"attack\":" + b2s(d.attack_seen != 0);
                s += ",\"x_read\":" + std::to_string(d.x_read);
                s += ",\"menu109\":" + std::to_string(d.menu109);
                s += ",\"bow_precision\":" + std::to_string(d.bow_precision);
                s += ",\"chr_2a6\":" + std::to_string(d.chr_2a6_or);
                {
                    VirtualPadState seen{};
                    seen.buttons = d.buttons_or;
                    s += ",\"buttons\":" + VirtualPad::state_json(&seen);
                }
                s += "}";
                s += ",\"item_override\":" + std::to_string(Rollback::item_override_for(i));
                s += ",\"in_curUsingInventoryItemId\":" + std::to_string((int32_t)in.curUsingInventoryItemId);
                s += ",\"in_curSelectedQuickbarItemId\":" + std::to_string((int32_t)in.curSelectedQuickbarItemId);
                    s += ",\"in_pad\":" + VirtualPad::state_json(&in.vpad.pad);
                if (pi->chrins.padManipulator != NULL)
                {
                    ChrManipulator* cm = &pi->chrins.padManipulator->chrManipulator;
                    s += ",\"use_cur\":" + b2s(cm->CurrentFrame_ActionInputs.use_ButtonPressed != 0);
                    s += ",\"use_prev\":" + b2s(cm->PrevFrame_ActionInputs.use_ButtonPressed != 0);
                    s += ",\"change_2handing_state\":" + std::to_string(cm->change_2handing_state);
                    s += ",\"left_hand_slot_selected\":" + std::to_string(cm->left_hand_slot_selected);
                    s += ",\"right_hand_slot_selected\":" + std::to_string(cm->right_hand_slot_selected);
                }
                if (pi->playergamedata != NULL)
                {
                    ChrAsm& ca = pi->playergamedata->equipGameData.chrasm;
                    s += ",\"equipped_weapon_style\":" + std::to_string(ca.equipped_weapon_style);
                    s += ",\"l_hand_equipped_index\":" + std::to_string(ca.l_hand_equipped_index);
                    s += ",\"r_hand_equipped_index\":" + std::to_string(ca.r_hand_equipped_index);
                    s += ",\"itemBeingUsedFromInventory\":" + std::to_string((int32_t)pi->playergamedata->equipGameData.itemInventoryIdCurrentlyBeingUsedFromInventory);
                }
                s += "}";
                n++;
            }
            s += "]";
            return ok_json(s);
        }
#endif

#if ROLLBACK_INPUT_TESTING
        if (cmd == "pad")
        {
            VirtualPad::install();
            if (sub == "reset")
            {
                VirtualPad::reset_diagnostics();
                return ok_json(VirtualPad::status_json());
            }
#if VIRTUALPAD_SELFTEST
            //Passthrough round-trip: replay the pad back through the stubs for N frames, so
            //the id set can be proven complete (unknown_button_ids stays empty) and replay
            //itself is exercised, without needing a second machine.
            if (sub == "selftest")
            {
                long frames = a.size() > 2 ? strtol(a[2].c_str(), NULL, 10) : 300;
                if (frames <= 0 || frames > 60 * 60 * 5) return err_json("usage: pad selftest [frames 1..18000]");
                VirtualPad::selftest_begin((uint32_t)frames);
                return ok_json(VirtualPad::status_json());
            }
#endif
            if (sub.empty() || sub == "status")
            {
                return ok_json(VirtualPad::status_json());
            }
#if VIRTUALPAD_SELFTEST
            return err_json("usage: pad [status] | pad reset | pad selftest [frames]");
#else
            return err_json("usage: pad [status] | pad reset");
#endif
        }
#endif

        if (cmd == "help")
        {
            return ok_json("\"commands\":[\"ping\",\"status\",\"frame\",\"input\",\"rollback on|off|toggle\","
                           "\"record arm|disarm\",\"record file <path>\",\"replay file <path>\",\"replay off\","
                           "\"script load <path>\",\"script add <directive>\",\"script clear\",\"script neutral on|off\","
                           "\"script name <text>\",\"script status\",\"peek <hexaddr> [len]\",\"rtti <hexaddr>\",\"end_session\",\"start_session\",\"freeze on|off|status\",\"warp <x> <y> <z> <yaw>\","
                           "\"log <text>\",\"subscribe\",\"hashes [since] [max]\",\"dump_at <frame>|+<n>\",\"dump_status\","
                           "\"dump_get\",\"probe\""
#if ROLLBACK_INPUT_TESTING
                           ",\"inputdiag\",\"network on|off\",\"network role send|recv\",\"network status\",\"pad [status|reset]\""
#if VIRTUALPAD_SELFTEST
                           ",\"pad selftest N\""
#endif
#endif
                           ",\"help\"]");
        }

        return err_json("unknown command '" + cmd + "' (try help)");
    }

    // MainLoop callback: run queued commands on the game thread. Never unregisters.
    bool pump(void* unused)
    {
        //a harness freeze has no session frames to tick it: freeze whatever loaded since
        if (!Rollback::ggpoStarted)
        {
            WorldFreeze::tick();
        }

        std::deque<std::shared_ptr<Request>> batch;
        {
            std::lock_guard<std::mutex> g(g_req_mtx);
            batch.swap(g_requests);
        }
        if (batch.empty()) return true;
        for (auto& r : batch)
        {
            std::string resp = execute(r->line);
            std::lock_guard<std::mutex> g(g_req_mtx);
            r->response = resp;
            r->done = true;
        }
        g_req_cv.notify_all();
        return true;
    }

    // Socket thread: hand a command to the game thread and wait for its reply.
    std::string dispatch(const std::string& line)
    {
        auto req = std::make_shared<Request>();
        req->line = line;
        std::unique_lock<std::mutex> lk(g_req_mtx);
        g_requests.push_back(req);
        if (!g_req_cv.wait_for(lk, std::chrono::seconds(REQUEST_TIMEOUT_SECONDS), [&] { return req->done; }))
        {
            // The request stays queued and will still run; only this reply is lost.
            return err_json("timeout: game thread did not service the command within "
                            + std::to_string(REQUEST_TIMEOUT_SECONDS) + "s (main loop not running yet?)");
        }
        return req->response;
    }

    // ---------------- connections ----------------
    void subscriber_loop(SOCKET s)
    {
        auto sub = std::make_shared<Subscriber>();
        sub->sock = s;
        {
            std::lock_guard<std::mutex> g(g_sub_mtx);
            g_subs.push_back(sub);
        }
        bool alive = send_line(s, ok_json("\"subscribed\":true,\"pid\":" + std::to_string((unsigned long)GetCurrentProcessId())));
        while (alive)
        {
            std::deque<std::string> batch;
            size_t dropped = 0;
            {
                std::unique_lock<std::mutex> lk(sub->mtx);
                sub->cv.wait_for(lk, std::chrono::seconds(1), [&] { return !sub->lines.empty(); });
                batch.swap(sub->lines);
                dropped = sub->dropped;
                sub->dropped = 0;
            }
            if (dropped) alive = send_line(s, "{\"log\":\"HARNESS: log stream dropped " + std::to_string(dropped) + " lines (subscriber too slow)\",\"t\":0}");
            for (auto& m : batch)
            {
                if (!alive) break;
                alive = send_line(s, m);
            }
            if (alive && batch.empty())
            {
                // Idle: notice a peer that went away. Anything it sends is ignored.
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(s, &fds);
                timeval tv = { 0, 0 };
                if (select(0, &fds, NULL, NULL, &tv) > 0)
                {
                    char t[256];
                    int n = recv(s, t, sizeof(t), 0);
                    if (n <= 0) alive = false;
                }
            }
        }
        {
            std::lock_guard<std::mutex> g(g_sub_mtx);
            g_subs.erase(std::remove(g_subs.begin(), g_subs.end(), sub), g_subs.end());
        }
        closesocket(s);
    }

    void connection_thread(SOCKET s)
    {
        std::string buf;
        char tmp[4096];
        for (;;)
        {
            int n = recv(s, tmp, sizeof(tmp), 0);
            if (n <= 0) break;
            buf.append(tmp, (size_t)n);
            size_t nl;
            while ((nl = buf.find('\n')) != std::string::npos)
            {
                std::string line = RollbackScript::_trim(buf.substr(0, nl));
                buf.erase(0, nl + 1);
                if (line.empty()) continue;
                if (line == "ping")
                {
                    if (!send_line(s, ok_json("\"pong\":true,\"pid\":" + std::to_string((unsigned long)GetCurrentProcessId())))) { closesocket(s); return; }
                    continue;
                }
                if (line == "subscribe")
                {
                    subscriber_loop(s);   // owns and closes the socket
                    return;
                }
                if (!send_line(s, dispatch(line))) { closesocket(s); return; }
            }
            if (buf.size() > 65536)
            {
                send_line(s, err_json("line too long"));
                break;
            }
        }
        closesocket(s);
    }

    void listener_thread(int base_port)
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            ConsoleWrite("HARNESS: WSAStartup failed, control plane disabled");
            return;
        }
        SOCKET ls = INVALID_SOCKET;
        int port = 0;
        int last_err = 0;
        for (int p = base_port; p < base_port + 10 && p <= 65535; p++)
        {
            ls = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (ls == INVALID_SOCKET) { last_err = WSAGetLastError(); break; }
            // Deliberately no SO_REUSEADDR: a second instance on this box must fail
            // here and fall forward to the next port.
            sockaddr_in addr = {};
            addr.sin_family = AF_INET;
            addr.sin_port = htons((u_short)p);
            inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
            if (bind(ls, (sockaddr*)&addr, sizeof(addr)) == 0) { port = p; break; }
            last_err = WSAGetLastError();
            closesocket(ls);
            ls = INVALID_SOCKET;
        }
        if (port == 0)
        {
            ConsoleWrite("HARNESS: control plane could not bind 127.0.0.1:%d..%d (WSA error %d), disabled", base_port, base_port + 9, last_err);
            return;
        }
        if (listen(ls, 8) != 0)
        {
            ConsoleWrite("HARNESS: listen failed (WSA error %d), control plane disabled", WSAGetLastError());
            closesocket(ls);
            return;
        }
        g_port = port;
        ConsoleWrite("HARNESS: control plane listening on 127.0.0.1:%d (pid %lu)", port, (unsigned long)GetCurrentProcessId());
        for (;;)
        {
            SOCKET c = accept(ls, NULL, NULL);
            if (c == INVALID_SOCKET)
            {
                int e = WSAGetLastError();
                if (e == WSAEINTR) continue;
                ConsoleWrite("HARNESS: accept failed (WSA error %d), control plane stopped", e);
                break;
            }
            std::thread(connection_thread, c).detach();
        }
        closesocket(ls);
        g_port = 0;
    }
}

void HarnessControl::start()
{
    int port = 0;
    char buf[32] = {};
    DWORD n = GetEnvironmentVariableA("DSR_HARNESS_PORT", buf, sizeof(buf));
    if (n > 0 && n < sizeof(buf)) port = atoi(buf);
    if (port <= 0)
    {
        port = (int)GetPrivateProfileInt(_DS1_OVERHAUL_TESTING_SECTION_, _DS1_OVERHAUL_TESTING_CONTROL_PORT_, 0, _DS1_OVERHAUL_SETTINGS_FILE_);
    }
    if (port <= 0 || port > 65535)
    {
        return;   // disabled: the normal case for players
    }

    //An early-frame state dump has to be armed before the GGPO session exists, and no
    //control-plane command can be: the peer joins during launch/pairing, so by the time an
    //orchestrator gets a turn the session is already hundreds of frames in (measured: frame
    //426 at the earliest a "dump_at" could land). Arming it from the environment at startup is
    //the only way to catch frame 0. RollbackHash::reset_session deliberately preserves a
    //pending request across the session boundary so this survives until the session starts.
    char dumpbuf[32] = {};
    DWORD dn = GetEnvironmentVariableA("DSR_HARNESS_DUMP_FRAME", dumpbuf, sizeof(dumpbuf));
    if (dn > 0 && dn < sizeof(dumpbuf))
    {
        int f = atoi(dumpbuf);
        const char* dash = strchr(dumpbuf, '-');
        if (f >= 0 && dash != NULL && dash != dumpbuf && atoi(dash + 1) >= f)
        {
            RollbackHash::dump_range_first = f;
            RollbackHash::dump_range_last = atoi(dash + 1);
            ConsoleWrite("HARNESS: state dumps pre-armed for frames %d-%d from DSR_HARNESS_DUMP_FRAME", f, RollbackHash::dump_range_last);
        }
        else if (f >= 0)
        {
            RollbackHash::dump_request_frame = f;
            ConsoleWrite("HARNESS: state dump pre-armed for frame %d from DSR_HARNESS_DUMP_FRAME", f);
        }
    }

    ConsoleWrite_tap = &log_tap;
    MainLoop::setup_mainloop_callback(pump, NULL, "harness_control_pump");
    std::thread(listener_thread, port).detach();
}

int HarnessControl::port()
{
    return g_port.load();
}
