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
      network on|off                Rollback::networkTest (F7, the lockstep test path)
      record arm|disarm             RollbackReplay::record_armed (F12), applies at next session start
      record file <path>            file the next recording is written to
      replay file <path>            file replayed at next session start ("replay off" disables)
      script load <path>            replace the input script with a file (see RollbackScript.h)
      script add <directive>        append one script line, e.g. "script add @120-125 r1=1"
      script clear                  drop the script
      script neutral on|off         zero player-driven fields every frame
      script name <text>            label for status/logs
      script status                 just the script part of status
      hotkey gsave|gload|isave|iload  the F8-F11 state/input save-restore tests
      end_session                   Rollback::rollback_end_session()
      log <text>                    write "HARNESS: <text>" into the mod log (marker for alignment)
      subscribe                     turn this connection into a log stream
      hashes [since] [max]          confirmed per-frame state digests (StateHash.h ring) with frame > since,
                                    oldest first; "gap":true if the ring no longer reaches since+1
      dump_at <frame>|+<n>          capture the full canonical state text when GGPO saves that frame
                                    (absolute, or n frames past the current one); re-saves overwrite until confirmed
      dump_status                   dump_requested / dump_frame / dump_confirmed / dump_size / dump_file
      dump_get                      the captured text (+ dump_status fields); large (hundreds of KB)
      probe                         per connected player: hp, max_hp, x, y, z, rot -- a cheap "what is happening" view
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
        s += ",\"network_test\":" + b2s(Rollback::networkTest);
        s += ",\"ggpo_started\":" + b2s(Rollback::ggpoStarted);
        s += ",\"ggpo_ready\":" + q(ready_name(Rollback::ggpoReady));
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
        return s;
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

        if (cmd == "network")
        {
            bool v;
            if (!parse_on_off(sub, Rollback::networkTest, &v)) return err_json("usage: network on|off|toggle");
            Rollback::networkTest = v;
            ConsoleWrite("HARNESS: netcode test %d", (int)v);
            return ok_json("\"network_test\":" + b2s(v));
        }

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

        if (cmd == "hotkey")
        {
            if (sub == "gsave") Rollback::gsave = true;
            else if (sub == "gload") Rollback::gload = true;
            else if (sub == "isave") Rollback::isave = true;
            else if (sub == "iload") Rollback::iload = true;
            else return err_json("usage: hotkey gsave|gload|isave|iload");
            return ok_json("\"hotkey\":" + q(sub));
        }

        if (cmd == "end_session")
        {
            bool was = Rollback::ggpoStarted;
            Rollback::rollback_end_session();
            ConsoleWrite("HARNESS: end_session (was_started=%d)", (int)was);
            return ok_json("\"was_started\":" + b2s(was));
        }

        if (cmd == "log")
        {
            std::string text = rest_after(line, 1);
            ConsoleWrite("HARNESS: %s", text.c_str());
            return ok_json();
        }

        // ---- determinism oracle (StateHash.h) ----
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

        if (cmd == "help")
        {
            return ok_json("\"commands\":[\"ping\",\"status\",\"frame\",\"input\",\"rollback on|off|toggle\",\"network on|off\","
                           "\"record arm|disarm\",\"record file <path>\",\"replay file <path>\",\"replay off\","
                           "\"script load <path>\",\"script add <directive>\",\"script clear\",\"script neutral on|off\","
                           "\"script name <text>\",\"script status\",\"hotkey gsave|gload|isave|iload\",\"end_session\","
                           "\"log <text>\",\"subscribe\",\"hashes [since] [max]\",\"dump_at <frame>|+<n>\",\"dump_status\","
                           "\"dump_get\",\"probe\",\"help\"]");
        }

        return err_json("unknown command '" + cmd + "' (try help)");
    }

    // MainLoop callback: run queued commands on the game thread. Never unregisters.
    bool pump(void* unused)
    {
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

    ConsoleWrite_tap = &log_tap;
    MainLoop::setup_mainloop_callback(pump, NULL, "harness_control_pump");
    std::thread(listener_thread, port).detach();
}

int HarnessControl::port()
{
    return g_port.load();
}
