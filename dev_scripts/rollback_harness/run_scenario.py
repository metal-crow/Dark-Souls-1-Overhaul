#!/usr/bin/env python3
"""
run_scenario.py -- one-shot rollback scenario: bring-up, run, verify, teardown,
and one JSON result on stdout.

The individual CLIs (launch_harness.py, harness_client.py, rollback_compare.py)
each do one step. Sequencing them correctly is the fiddly part -- rollback has to
be on *before* the peer joins, matchmaking has to find the other instance and not
a stranger, and teardown has to happen even when a stage fails. This wraps the
whole thing so an orchestrator can run one command and read one verdict.

    python run_scenario.py --frames 3600 --script-a a.txt --script-b b.txt

Stages: preflight -> launch -> connect -> script -> run -> teardown.
Every stage is recorded in the result with its duration and error, so a failure
says which step broke rather than just "it didn't work".

Exit codes (same contract as rollback_compare.py):
    0  PASS    -- every overlapping confirmed frame identical
    1  DESYNC  -- the comparator found a divergence (details + dumps in the result)
    2  ERROR   -- bring-up / connect / usage failure; the run never produced a verdict

Matchmaking isolation
---------------------
Seamless Co-op matches on the co-op password in SeamlessCoop/ds1sc_settings.ini.
With a common value ("test", "", "password") the two instances happily connect to
*strangers* instead of each other -- which silently invalidates the comparison
(you are diffing against someone else's game) and drags an unrelated player into
a test session. So by default this runner writes a harness-specific password into
that file (backing the original up once to <name>.preharness-backup) and refuses
to run against a known-common one. --no-password-management opts out.

Windows only (it launches the game); stdlib + the sibling harness modules.
"""

import argparse
import json
import os
import re
import shutil
import socket
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
if HERE not in sys.path:
    sys.path.insert(0, HERE)

from harness_client import HarnessClient, HarnessError, discover  # noqa: E402

LAUNCH_HARNESS = os.path.join(HERE, "launch_harness.py")
ROLLBACK_COMPARE = os.path.join(HERE, "rollback_compare.py")

DEFAULT_LAUNCHER = (
    r"C:\Program Files (x86)\Steam\steamapps\common"
    r"\DARK SOULS REMASTERED\ds1sc_launcher.exe"
)
DEFAULT_BOX = "DefaultBox"
DEFAULT_PORTS = "42800,42810"
DEFAULT_COOP_PASSWORD = "dsr-harness-7f3a91"
# Passwords near-guaranteed to collide with public sessions.
COMMON_PASSWORDS = {"", "test", "password", "123", "1234", "coop", "seamless", "ds1", "dsr"}
BACKUP_SUFFIX = ".preharness-backup"

# The port the DLL actually binds may be the requested one +0..9 (fall-forward
# when the other instance already took it), so we always re-discover by pid.
PORT_SPAN = 10

EXIT_PASS, EXIT_DESYNC, EXIT_ERROR = 0, 1, 2


class ScenarioError(Exception):
    """A stage failed in a way that means there is no verdict to report."""


# ---------------------------------------------------------------------------
# stage bookkeeping
# ---------------------------------------------------------------------------

class Stages:
    def __init__(self, log):
        self.items = []
        self.log = log

    def run(self, name, fn):
        self.log("== %s ==" % name)
        t0 = time.time()
        try:
            detail = fn()
        except Exception as e:
            self.items.append({
                "name": name, "ok": False, "seconds": round(time.time() - t0, 2),
                "error": "%s: %s" % (type(e).__name__, e),
            })
            raise
        self.items.append({
            "name": name, "ok": True, "seconds": round(time.time() - t0, 2),
            "detail": detail,
        })
        return detail


# ---------------------------------------------------------------------------
# preflight
# ---------------------------------------------------------------------------

def port_is_free(port, host="127.0.0.1"):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.bind((host, port))
        return True
    except OSError:
        return False
    finally:
        s.close()


def settings_path(launcher):
    return os.path.join(os.path.dirname(launcher), "SeamlessCoop", "ds1sc_settings.ini")


def read_coop_password(path):
    try:
        with open(path, "r", errors="replace") as fh:
            for line in fh:
                m = re.match(r"\s*cooppassword\s*=\s*(.*?)\s*$", line)
                if m:
                    return m.group(1)
    except OSError:
        return None
    return None


def write_coop_password(path, value):
    """Rewrite cooppassword in place, backing the file up once. Returns the old value."""
    with open(path, "r", errors="replace") as fh:
        text = fh.read()
    old = read_coop_password(path)
    backup = path + BACKUP_SUFFIX
    if not os.path.exists(backup):
        shutil.copyfile(path, backup)
    new_text, n = re.subn(r"(?m)^(\s*cooppassword\s*=\s*).*$", lambda m: m.group(1) + value, text)
    if n == 0:
        raise ScenarioError("no 'cooppassword' key in %s" % path)
    with open(path, "w", newline="") as fh:
        fh.write(new_text)
    return old


def preflight(args, log):
    if os.name != "nt":
        raise ScenarioError("run_scenario.py is Windows-only (it launches the game)")
    for p in (LAUNCH_HARNESS, ROLLBACK_COMPARE):
        if not os.path.isfile(p):
            raise ScenarioError("missing sibling script: %s" % p)
    if not args.attach and not os.path.isfile(args.launcher):
        raise ScenarioError("launcher not found: %s" % args.launcher)

    detail = {"launcher": args.launcher, "ports": [args.port_a, args.port_b]}

    if not args.attach:
        busy = [p for p in (args.port_a, args.port_b) if not port_is_free(p)]
        if busy:
            raise ScenarioError(
                "control port(s) %s already in use -- a previous instance is still running. "
                "Kill it, or use --attach to drive the running pair."
                % ", ".join(str(b) for b in busy))

    # matchmaking isolation
    sp = settings_path(args.launcher)
    detail["settings"] = sp
    current = read_coop_password(sp)
    detail["coop_password_before"] = current
    if args.manage_password:
        if current != args.coop_password:
            old = write_coop_password(sp, args.coop_password)
            log("preflight: co-op password %r -> %r (backup: %s)"
                % (old, args.coop_password, os.path.basename(sp) + BACKUP_SUFFIX))
            detail["coop_password_changed"] = True
        else:
            log("preflight: co-op password already %r" % args.coop_password)
            detail["coop_password_changed"] = False
        detail["coop_password"] = args.coop_password
    else:
        detail["coop_password"] = current
        if current is None:
            log("preflight: WARNING could not read the co-op password from %s" % sp)
        elif current.strip().lower() in COMMON_PASSWORDS:
            raise ScenarioError(
                "co-op password %r is a common value: the instances will match with "
                "strangers instead of each other and the comparison would be "
                "meaningless. Set a unique one, or drop --no-password-management."
                % current)
    return detail


# ---------------------------------------------------------------------------
# launch
# ---------------------------------------------------------------------------

INSTANCE_RE = re.compile(
    r"INSTANCE\s+hwnd=(?P<hwnd>\S+)\s+pid=(?P<pid>\d+)\s+control_port=(?P<port>\d*)\s*sandbox=(?P<box>\S*)")


def launch_one(label, port, sandbox, args, log):
    cmd = [sys.executable, LAUNCH_HARNESS, "--launch", "--control-port", str(port),
           "--launcher", args.launcher, "--timeout", str(args.load_timeout)]
    if sandbox:
        cmd += ["--sandbox", sandbox]
    if args.sandboxie_start:
        cmd += ["--sandboxie-start", args.sandboxie_start]
    log("launch %s: %s" % (label, " ".join(cmd[2:])))
    proc = subprocess.run(cmd, capture_output=True, text=True,
                          timeout=args.load_timeout + args.launch_timeout)
    out = (proc.stdout or "") + (proc.stderr or "")
    for line in out.splitlines():
        log("  [%s] %s" % (label, line))
    m = INSTANCE_RE.search(out)
    if proc.returncode != 0:
        raise ScenarioError("launch_harness for %s exited %d (see log above)" % (label, proc.returncode))
    if not m:
        raise ScenarioError("launch_harness for %s printed no INSTANCE line" % label)
    pid = int(m.group("pid"))
    # The DLL may have fallen forward off the requested port; find the real one.
    found = discover(port, PORT_SPAN, args.host, pid=pid, timeout=2.0)
    if not found:
        raise ScenarioError(
            "%s (pid %d) came up but no control plane answered on %d..%d -- was the "
            "DLL built with HarnessControl, and did DSR_HARNESS_PORT reach the game?"
            % (label, pid, port, port + PORT_SPAN - 1))
    actual_port, st = found[0]
    if actual_port != port:
        log("  [%s] control plane fell forward to %d" % (label, actual_port))
    return {"label": label, "pid": pid, "port": actual_port, "requested_port": port,
            "sandbox": sandbox or "", "hwnd": m.group("hwnd"),
            "steam_id": st.get("steam_id", ""), "sandboxed": st.get("sandboxed")}


def attach_one(label, port, args):
    found = discover(port, PORT_SPAN, args.host, timeout=2.0)
    if not found:
        raise ScenarioError("no control plane on %d..%d for %s" % (port, port + PORT_SPAN - 1, label))
    actual_port, st = found[0]
    return {"label": label, "pid": st.get("pid"), "port": actual_port, "requested_port": port,
            "sandbox": "", "hwnd": None, "steam_id": st.get("steam_id", ""),
            "sandboxed": st.get("sandboxed")}


def launch(args, log):
    if args.attach:
        a = attach_one("A", args.port_a, args)
        b = attach_one("B", args.port_b, args)
    else:
        # A first: it hosts, then B joins it.
        a = launch_one("A", args.port_a, None, args, log)
        b = launch_one("B", args.port_b, args.sandbox, args, log)
    return {"A": a, "B": b}


# ---------------------------------------------------------------------------
# connect
# ---------------------------------------------------------------------------

def connect(inst, args, log):
    pa, pb = inst["A"]["port"], inst["B"]["port"]
    with HarnessClient(pa, args.host) as a, HarnessClient(pb, args.host) as b:
        sa = a.wait_char_loaded(timeout=args.load_timeout)
        sb = b.wait_char_loaded(timeout=args.load_timeout)
        log("connect: characters loaded (A pid %s, B pid %s)" % (sa.get("pid"), sb.get("pid")))

        # Rollback must be on before the peer joins -- the session is created in
        # the lobby callback. launch_harness taps F6; verify rather than assume.
        for name, cl, st in (("A", a, sa), ("B", b, sb)):
            if not st.get("rollback_enabled"):
                log("connect: %s rollback was off, enabling" % name)
                cl.rollback(True)
        if not a.status().get("rollback_enabled") or not b.status().get("rollback_enabled"):
            raise ScenarioError("rollback could not be enabled on both instances")

        ids = {"A": a.status().get("steam_id"), "B": b.status().get("steam_id")}
        log("connect: waiting for both instances to see 2 players ...")
        a.wait_players(2, timeout=args.connect_timeout)
        b.wait_players(2, timeout=args.connect_timeout)
        log("connect: waiting for GGPO Ready on both sides ...")
        ra = a.wait_ggpo_ready(timeout=args.ggpo_timeout)
        rb = b.wait_ggpo_ready(timeout=args.ggpo_timeout)
        log("connect: GGPO ready (A frame %s, B frame %s)" % (ra.get("frame"), rb.get("frame")))
        return {"steam_ids": ids, "a_frame": ra.get("frame"), "b_frame": rb.get("frame"),
                "connected_players": ra.get("connected_players")}


# ---------------------------------------------------------------------------
# scripts
# ---------------------------------------------------------------------------

DEFAULT_SCRIPT = "!name idle-determinism\n!neutral\n"


def upload_scripts(inst, args, log):
    out = {}
    for label, path in (("A", args.script_a), ("B", args.script_b)):
        if path:
            with open(path, "r", errors="replace") as fh:
                text = fh.read()
            src = path
        else:
            text = DEFAULT_SCRIPT
            src = "<default idle>"
        with HarnessClient(inst[label]["port"], args.host) as c:
            st = c.script_upload(text)
        log("script %s: %s -> %s directive(s), frames %s..%s, neutral=%s"
            % (label, os.path.basename(src), st.get("directives"),
               st.get("first_frame"), st.get("last_frame"), st.get("neutral")))
        out[label] = {"source": src, "status": st}
    return out


# ---------------------------------------------------------------------------
# run (the comparator)
# ---------------------------------------------------------------------------

def run_compare(inst, args, log):
    os.makedirs(args.out_dir, exist_ok=True)
    cmd = [sys.executable, ROLLBACK_COMPARE,
           "--live", "%d,%d" % (inst["A"]["port"], inst["B"]["port"]),
           "--host", args.host, "--labels", "A,B",
           "--out-dir", args.out_dir, "--json"]
    if args.frames:
        cmd += ["--frames", str(args.frames)]
    if args.duration:
        cmd += ["--duration", str(args.duration)]
    if args.dump_on_desync:
        cmd += ["--dump-on-desync", "--dump-ahead", str(args.dump_ahead)]
    log("run: " + " ".join(cmd[2:]))
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.stderr:
        for line in proc.stderr.strip().splitlines():
            log("  [compare] " + line)
    report = None
    if proc.stdout.strip():
        try:
            report = json.loads(proc.stdout)
        except ValueError:
            log("  [compare] non-JSON stdout: " + proc.stdout[:400])
    if proc.returncode == EXIT_ERROR or (report is None and proc.returncode != EXIT_PASS):
        raise ScenarioError("rollback_compare failed (exit %d): %s"
                            % (proc.returncode, (proc.stderr or "").strip()[:300] or "no detail"))
    return {"exit": proc.returncode, "report": report}


# ---------------------------------------------------------------------------
# teardown
# ---------------------------------------------------------------------------

def kill_pid(pid, log):
    try:
        subprocess.run(["taskkill", "/PID", str(pid), "/F", "/T"],
                       capture_output=True, text=True, timeout=30)
        log("teardown: killed pid %s" % pid)
        return True
    except Exception as e:  # noqa: BLE001
        log("teardown: could not kill pid %s: %s" % (pid, e))
        return False


def teardown(inst, args, log, launched):
    done = {"end_session": {}, "killed": [], "password_restored": False}
    if inst:
        for label in ("A", "B"):
            info = inst.get(label)
            if not info:
                continue
            try:
                with HarnessClient(info["port"], args.host, timeout=5.0) as c:
                    c.end_session()
                done["end_session"][label] = True
                log("teardown: end_session on %s" % label)
            except (OSError, HarnessError) as e:
                done["end_session"][label] = "error: %s" % e
    if launched and not args.keep_alive and inst:
        for label in ("A", "B"):
            info = inst.get(label)
            if info and info.get("pid") and kill_pid(info["pid"], log):
                done["killed"].append(info["pid"])
    if args.restore_password and args.manage_password:
        sp = settings_path(args.launcher)
        backup = sp + BACKUP_SUFFIX
        if os.path.exists(backup):
            shutil.copyfile(backup, sp)
            done["password_restored"] = True
            log("teardown: restored %s from backup" % os.path.basename(sp))
    return done


# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------

def main(argv=None):
    ap = argparse.ArgumentParser(
        description="One-shot rollback scenario: bring-up, run, verify, teardown -> one JSON result.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    ap.add_argument("--frames", type=int, default=1800,
                    help="stop with PASS once this many confirmed frames matched (~60/s)")
    ap.add_argument("--duration", type=float, default=0,
                    help="stop after this many seconds instead of a frame count (0 = off)")
    ap.add_argument("--script-a", metavar="FILE", help="input script for instance A")
    ap.add_argument("--script-b", metavar="FILE", help="input script for instance B")
    ap.add_argument("--out-dir", default=os.path.join(HERE, "runs", "latest"),
                    help="where state dumps, diffs and result.json go")

    src = ap.add_argument_group("instances")
    src.add_argument("--ports", default=DEFAULT_PORTS, metavar="A,B",
                     help="requested control-plane ports")
    src.add_argument("--host", default="127.0.0.1")
    src.add_argument("--launcher", default=DEFAULT_LAUNCHER, help="ds1sc_launcher.exe")
    src.add_argument("--sandbox", default=DEFAULT_BOX, metavar="BOX",
                     help="Sandboxie box for instance B")
    src.add_argument("--sandboxie-start", metavar="PATH", help="path to Sandboxie Start.exe")
    src.add_argument("--attach", action="store_true",
                     help="drive already-running instances instead of launching")
    src.add_argument("--keep-alive", action="store_true",
                     help="leave the games running after the run")

    mm = ap.add_argument_group("matchmaking isolation")
    mm.add_argument("--coop-password", default=DEFAULT_COOP_PASSWORD,
                    help="password written into ds1sc_settings.ini so the two instances "
                         "match each other and not strangers")
    mm.add_argument("--no-password-management", dest="manage_password", action="store_false",
                    help="do not touch ds1sc_settings.ini (fails preflight on a common password)")
    mm.add_argument("--restore-password", action="store_true",
                    help="restore the settings file from its backup during teardown")

    tmo = ap.add_argument_group("timeouts (seconds)")
    tmo.add_argument("--load-timeout", type=float, default=240.0, help="title screen -> in game")
    tmo.add_argument("--launch-timeout", type=float, default=180.0, help="extra slack for the launcher")
    tmo.add_argument("--connect-timeout", type=float, default=300.0, help="until both see 2 players")
    tmo.add_argument("--ggpo-timeout", type=float, default=180.0, help="until GGPO reports Ready")

    dmp = ap.add_argument_group("desync forensics")
    dmp.add_argument("--dump-on-desync", action="store_true", default=True,
                     help="capture and diff both sides' full canonical state on divergence")
    dmp.add_argument("--no-dump-on-desync", dest="dump_on_desync", action="store_false")
    dmp.add_argument("--dump-ahead", type=int, default=30, help="frames ahead to schedule the dump")

    ap.add_argument("--quiet", action="store_true", help="only the JSON result on stdout")
    args = ap.parse_args(argv)

    try:
        args.port_a, args.port_b = (int(x) for x in args.ports.split(","))
    except ValueError:
        ap.error("--ports needs A,B")

    # Progress goes to stderr so stdout stays a single clean JSON document.
    def log(msg):
        if not args.quiet:
            sys.stderr.write(msg + "\n")
            sys.stderr.flush()

    started = time.time()
    stages = Stages(log)
    result = {"result": "ERROR", "scenario": {
        "frames": args.frames, "duration": args.duration,
        "script_a": args.script_a, "script_b": args.script_b,
        "attach": args.attach, "sandbox": args.sandbox}}
    inst = None
    launched = False
    exit_code = EXIT_ERROR

    try:
        stages.run("preflight", lambda: preflight(args, log))
        inst = stages.run("launch", lambda: launch(args, log))
        launched = not args.attach
        result["instances"] = inst
        stages.run("connect", lambda: connect(inst, args, log))
        result["scripts"] = stages.run("script", lambda: upload_scripts(inst, args, log))
        run = stages.run("run", lambda: run_compare(inst, args, log))
        result["compare"] = run["report"]
        verdict = (run["report"] or {}).get("result")
        if run["exit"] == EXIT_PASS:
            result["result"] = verdict or "PASS"
            exit_code = EXIT_PASS
        else:
            result["result"] = verdict or "DESYNC"
            exit_code = EXIT_DESYNC
    except ScenarioError as e:
        result["error"] = str(e)
        log("ERROR: %s" % e)
    except TimeoutError as e:
        result["result"] = "TIMEOUT"
        result["error"] = str(e)
        log("TIMEOUT: %s" % e)
    except KeyboardInterrupt:
        result["error"] = "interrupted"
        log("interrupted")
    except Exception as e:  # noqa: BLE001 -- the runner must always emit a result
        result["error"] = "%s: %s" % (type(e).__name__, e)
        log("ERROR: %s: %s" % (type(e).__name__, e))
    finally:
        try:
            td = teardown(inst, args, log, launched)
            stages.items.append({"name": "teardown", "ok": True, "detail": td})
        except Exception as e:  # noqa: BLE001
            stages.items.append({"name": "teardown", "ok": False, "error": str(e)})

    result["stages"] = stages.items
    result["seconds"] = round(time.time() - started, 1)

    try:
        os.makedirs(args.out_dir, exist_ok=True)
        with open(os.path.join(args.out_dir, "result.json"), "w", newline="\n") as fh:
            json.dump(result, fh, indent=2)
        result["result_file"] = os.path.join(args.out_dir, "result.json")
    except OSError:
        pass

    print(json.dumps(result, indent=2))
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
