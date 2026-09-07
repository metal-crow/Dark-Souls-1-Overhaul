#!/usr/bin/env python3
"""
rollback_compare.py — the determinism-oracle comparator (harness step 1, Tier 0).

GGPO's invariant: once a frame is *confirmed* (all inputs known, no further
rollback possible), both peers must hold bit-identical game state. The mod's
RollbackHash::emit_confirmed() (OverhaulDLL/include/StateHash.h) records one
digest set per confirmed frame, available two ways:

  * a line in each instance's own **dsoverhaul_statehash_logging.txt** (next to
    the exe; NOT the main dsoverhaul_logging.txt, and no `[ms]` prefix):

        STATEHASH frame=1452 player=<hex16> bullet=<hex16> damage=<hex16> \
                  havok=<hex16> throw=<hex16> dmghit=<hex16> comb=<hex16>

  * the control plane's `hashes` command (HarnessControl.cpp), which serves the
    same columns as JSON from an in-memory ring (~2.3 min), so a running pair of
    instances can be checked live without touching either log file (the
    Sandboxie'd instance's file is inside the box's virtual filesystem).

This script aligns the two instances' streams by frame number and reports the
FIRST confirmed frame whose digests differ. Per-subsystem columns say *which*
subsystem diverged, so a vague "looked wrong" becomes "DamageMan diverged at
frame 1452" — handed straight to the LLM to localize.

Parsing is schema-flexible: every `key=hexvalue` token after `STATEHASH` (or
every key in a `hashes` entry) is a column, so adding `sfx=...` later needs no
change here. `frame` is decimal; every other column is a hex digest. `comb` is
treated as the rollup and reported separately from the subsystem columns.

Usage:
    rollback_compare.py HOST_LOG GUEST_LOG [--json] [--context N] [--labels a,b]
    rollback_compare.py --live A_PORT,B_PORT [--frames N] [--duration S]
                        [--dump-on-desync [--dump-ahead N] [--out-dir DIR]]
                        [--json] [--labels a,b] [--host 127.0.0.1]

Live mode polls both instances' `hashes` until N frames have been compared, S
seconds have passed, the GGPO session ends, one side stalls, or (unless
--continue) the first divergence. With --dump-on-desync it then asks BOTH
instances to capture the full canonical state text at the same future GGPO frame
(`dump_at`), fetches both (`dump_get`), writes statedump_<label>_<frame>.txt plus
a unified diff, and prints the head of the diff — Tier 1 in one step.

Exit codes (stable contract for the orchestrator / MCP):
    0  PASS    — every overlapping confirmed frame present in both, all identical
    1  DESYNC  — at least one overlapping frame's digests differ
    2  ERROR   — usage / parse failure / no comparable frames / control plane down

Coverage gaps (a frame confirmed on one side but missing on the other within the
overlap range — e.g. one instance stalled or crashed) are reported as warnings
and surfaced in --json, but do not by themselves set a non-zero exit; only a
provable value divergence is a determinism failure.

No third-party dependencies (Python 3.8+, stdlib only). Live mode imports
harness_client.py from this directory.
"""

import argparse
import collections
import difflib
import json
import os
import re
import sys
import time

# Canonical column order from StateHash.h::emit_confirmed. Unknown columns (e.g.
# a future `sfx`) are appended after these in first-seen order.
PREFERRED_ORDER = ["player", "bullet", "damage", "havok", "throw", "dmghit", "comb"]
ROLLUP_COL = "comb"  # derived rollup, not a subsystem

_STATEHASH_RE = re.compile(r"\bSTATEHASH\b(.*)$")
_TOKEN_RE = re.compile(r"(\w+)=([0-9a-fA-F]+)")


class ParsedLog:
    def __init__(self, path, label):
        self.path = path
        self.label = label
        self.frames = {}        # frame:int -> {col:str -> value:int}
        self.columns = []       # column names in first-seen order (excl. 'frame')
        self.dup_frames = []    # frames that appeared more than once (last wins)
        self.line_count = 0

    def ingest(self, frame, cols):
        """Record one frame's digests ({col: int}). Last occurrence of a frame wins.
        Used by parse() for log lines and by live mode for `hashes` entries."""
        self.line_count += 1
        for k in cols:
            if k not in self.columns:
                self.columns.append(k)
        if frame in self.frames:
            self.dup_frames.append(frame)
        self.frames[frame] = cols

    def parse(self):
        try:
            with open(self.path, "r", encoding="utf-8", errors="replace") as fh:
                for raw in fh:
                    m = _STATEHASH_RE.search(raw)
                    if not m:
                        continue
                    tokens = dict(
                        (k, v) for k, v in _TOKEN_RE.findall(m.group(1))
                    )
                    if "frame" not in tokens:
                        continue
                    frame = int(tokens.pop("frame"), 10)
                    self.ingest(frame, {k: int(v, 16) for k, v in tokens.items()})
        except OSError as e:
            sys.stderr.write(f"error: cannot read {self.path}: {e}\n")
            return False
        return True

    @property
    def min_frame(self):
        return min(self.frames) if self.frames else None

    @property
    def max_frame(self):
        return max(self.frames) if self.frames else None


def ordered_columns(*logs):
    """Union of columns across logs, in canonical-then-first-seen order."""
    seen = []
    for col in PREFERRED_ORDER:
        if any(col in lg.columns for lg in logs):
            seen.append(col)
    for lg in logs:
        for col in lg.columns:
            if col not in seen:
                seen.append(col)
    return seen


def subsystem_columns(columns):
    return [c for c in columns if c != ROLLUP_COL]


def short(v):
    return f"{v:016x}"


def compare(host, guest, context, tail_tolerance):
    cols = ordered_columns(host, guest)
    subs = subsystem_columns(cols)

    lo = max(host.min_frame, guest.min_frame)
    hi = min(host.max_frame, guest.max_frame)  # last frame both could have

    common = sorted(f for f in host.frames if lo <= f <= hi and f in guest.frames)

    # Coverage gaps *inside* the overlap window (one side confirmed it, the other
    # never did -> a dropped/partial log line or a localized desync stall).
    gaps = []
    for f in range(lo, hi + 1):
        in_h = f in host.frames
        in_g = f in guest.frames
        if in_h != in_g:
            gaps.append((f, "guest" if in_h else "host"))

    # Truncation: one instance stopped emitting confirmed frames well before the
    # other (likely crash / disconnect / hang). A few frames' difference is benign
    # (confirmed-frame lag + uneven shutdown), so only flag beyond tolerance.
    truncation = None
    if host.max_frame != guest.max_frame:
        if host.max_frame < guest.max_frame:
            stopped_side, stopped_at, other_reached = host.label, host.max_frame, guest.max_frame
        else:
            stopped_side, stopped_at, other_reached = guest.label, guest.max_frame, host.max_frame
        tail = other_reached - stopped_at
        truncation = {
            "stopped_side": stopped_side,
            "stopped_at": stopped_at,
            "other_reached": other_reached,
            "tail": tail,
            "significant": tail > tail_tolerance,
        }

    first_div = None
    diff_cols_at_first = []
    for f in common:
        hc, gc = host.frames[f], guest.frames[f]
        diffs = [c for c in cols if hc.get(c) != gc.get(c)]
        if diffs:
            first_div = f
            diff_cols_at_first = diffs
            break

    return {
        "columns": cols,
        "subsystems": subs,
        "overlap": (lo, hi),
        "common_count": len(common),
        "common_frames": common,
        "gaps": gaps,
        "truncation": truncation,
        "first_div": first_div,
        "diff_cols": diff_cols_at_first,
    }


def build_context_table(host, guest, cols, center, context):
    """Frames around `center`: '=' equal, 'X' differ, '-' missing one side."""
    frames = sorted(set(host.frames) | set(guest.frames))
    window = [f for f in frames if center - context <= f <= center + context]
    rows = []
    for f in window:
        hc = host.frames.get(f)
        gc = guest.frames.get(f)
        cells = []
        for c in cols:
            if hc is None or gc is None:
                cells.append("-")
            elif hc.get(c) == gc.get(c):
                cells.append("=")
            else:
                cells.append("X")
        rows.append((f, cells, hc is not None, gc is not None))
    return window, rows


def is_clean(r):
    trunc = r["truncation"]
    return (
        r["first_div"] is None
        and not r["gaps"]
        and not (trunc and trunc["significant"])
    )


def print_human(host, guest, r, context):
    cols = r["columns"]
    lo, hi = r["overlap"]
    trunc = r["truncation"]

    def src_line(lg):
        rng = (
            f"{lg.min_frame}..{lg.max_frame}" if lg.frames else "no STATEHASH frames"
        )
        dup = f", {len(lg.dup_frames)} dup frame(s)" if lg.dup_frames else ""
        return f"  {lg.label:<6} {lg.path}  ({len(lg.frames)} frames, {rng}{dup})"

    if is_clean(r):
        print("ROLLBACK STATE COMPARE -- PASS")
        print(src_line(host))
        print(src_line(guest))
        print(
            f"  compared {r['common_count']} overlapping confirmed frames "
            f"[{lo}..{hi}]; all digests identical."
        )
        if trunc:
            print(
                f"  (benign tail: {trunc['stopped_side']} ends {trunc['tail']} "
                f"frame(s) before {trunc['other_reached']}; within tolerance)"
            )
        return

    # Headline frame = earliest anomaly among in-window gap, value divergence,
    # and a significant truncation (where the stalled side stopped).
    gap_frame = r["gaps"][0][0] if r["gaps"] else None
    div_frame = r["first_div"]
    trunc_frame = trunc["stopped_at"] + 1 if (trunc and trunc["significant"]) else None
    candidates = [f for f in (gap_frame, div_frame, trunc_frame) if f is not None]
    center = min(candidates)

    label = "DESYNC" if div_frame is not None else "COVERAGE GAP"
    if div_frame is None and trunc_frame is not None:
        label = "TRUNCATED"
    print(f"ROLLBACK STATE COMPARE -- {label}")
    print(src_line(host))
    print(src_line(guest))
    print(f"  overlap [{lo}..{hi}], {r['common_count']} frames present in both")

    if trunc and trunc["significant"]:
        print(
            f"\n  ! truncation: {trunc['stopped_side']} stopped emitting confirmed "
            f"frames at {trunc['stopped_at']} while the other reached "
            f"{trunc['other_reached']} ({trunc['tail']} frame tail)."
        )
        print(
            f"    The {trunc['stopped_side']} instance likely crashed, hung, or "
            f"disconnected here -- check the tail of its log."
        )

    if gap_frame is not None:
        side = r["gaps"][0][1]
        print(
            f"\n  ! coverage gap: frame {gap_frame} confirmed by the other "
            f"instance but MISSING on {side} ({len(r['gaps'])} gap frame(s) total)."
        )

    if div_frame is not None:
        print(f"\n  first value divergence at frame {div_frame}")
        sub_diffs = [c for c in r["diff_cols"] if c != ROLLUP_COL]
        print(
            "    diverged subsystem(s): "
            + (", ".join(sub_diffs) if sub_diffs else "(none -- only rollup differs?)")
        )
        for c in r["diff_cols"]:
            hv = host.frames[div_frame].get(c)
            gv = guest.frames[div_frame].get(c)
            print(
                f"      {c:<8} {host.label}={short(hv) if hv is not None else '-'} "
                f"{guest.label}={short(gv) if gv is not None else '-'}"
            )
        if div_frame - 1 in r["common_frames"]:
            print(f"    (agree through frame {div_frame - 1})")

    _, rows = build_context_table(host, guest, cols, center, context)
    if rows:
        print(f"\n  context [{rows[0][0]}..{rows[-1][0]}]:")
        header = "    " + "frame".ljust(8) + "".join(c[:6].ljust(8) for c in cols)
        print(header)
        for f, cells, in_h, in_g in rows:
            mark = " <<" if f == center else ""
            missing = ""
            if not in_h:
                missing = f"  (no {host.label})"
            elif not in_g:
                missing = f"  (no {guest.label})"
            print(
                "    "
                + str(f).ljust(8)
                + "".join(x.ljust(8) for x in cells)
                + mark
                + missing
            )

    if div_frame is not None:
        print(
            f"\n  next: localize the field with a same-frame state dump of both instances:\n"
            f"        rollback_compare.py --live A,B --dump-on-desync   (automatic), or\n"
            f"        harness_client.py --port P dump at +30 / dump get FILE on each, then diff\n"
            f"        (re-running the same deterministic script reproduces frame {div_frame})."
        )


def print_dump_info(dump_info):
    if not dump_info:
        return
    if dump_info.get("error"):
        print(f"\n  ! state dump failed: {dump_info['error']}")
        return
    print(
        f"\n  state dump at frame {dump_info['frame']}: "
        f"{dump_info['diff_lines']} differing line(s) -> {dump_info['diff_file']}"
    )
    for f in dump_info["files"]:
        print(f"    {f}")
    if dump_info["preview"]:
        print("  diff head:")
        for line in dump_info["preview"]:
            print("    " + line)


def build_json(host, guest, r):
    div = r["first_div"]
    trunc = r["truncation"]
    if is_clean(r):
        result = "PASS"
    elif div is not None:
        result = "DESYNC"
    elif r["gaps"]:
        result = "COVERAGE_GAP"
    else:
        result = "TRUNCATED"
    out = {
        "result": result,
        "overlap": list(r["overlap"]),
        "common_frames_compared": r["common_count"],
        "columns": r["columns"],
        "host": {
            "path": host.path,
            "frames": len(host.frames),
            "range": [host.min_frame, host.max_frame],
            "dup_frames": sorted(set(host.dup_frames)),
        },
        "guest": {
            "path": guest.path,
            "frames": len(guest.frames),
            "range": [guest.min_frame, guest.max_frame],
            "dup_frames": sorted(set(guest.dup_frames)),
        },
        "first_divergence_frame": div,
        "diverged_columns": r["diff_cols"],
        "diverged_subsystems": [c for c in r["diff_cols"] if c != ROLLUP_COL],
        "coverage_gaps": [{"frame": f, "missing_on": s} for f, s in r["gaps"]],
        "truncation": trunc,
    }
    if div is not None:
        out["values_at_divergence"] = {
            c: {
                host.label: host.frames[div].get(c),
                guest.label: guest.frames[div].get(c),
            }
            for c in r["diff_cols"]
        }
    return out


# ---------------------------------------------------------------------------
# Live mode: pull confirmed digests from two running instances over the control
# plane instead of reading their log files, and optionally dump+diff on desync.
# ---------------------------------------------------------------------------

def _hex_cols(entry):
    return {k: int(v, 16) for k, v in entry.items() if k != "f"}


def _clients(port_a, port_b, host):
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from harness_client import HarnessClient  # noqa: WPS433 (same directory)
    return HarnessClient(port_a, host).connect(), HarnessClient(port_b, host).connect()


# Wholesale difflib on two ~140k-line state dumps that differ in a third of their
# lines took ~3 minutes of CPU on the first real in-game run (2026-09-07) and
# dominated the whole comparison. Two cheap wins below: strip the identical
# prefix/suffix (a desync is normally localised, so this usually removes nearly
# everything), and if what remains is still enormous, skip the line diff and emit
# a field-grouped summary -- which is what you end up reading anyway.

DIFF_LINE_BUDGET = 20000     # max lines of divergence handed to difflib


def _diff_field_name(line):
    m = re.match(r"\s*([A-Za-z_][A-Za-z0-9_]*)\s*[:=]", line)
    return m.group(1) if m else line.strip()[:40]


def _diff_summary(mid_a, mid_b, header):
    """Alignment-independent summary: which field names differ, and how often."""
    out = list(header)
    out.append("@@ divergence too large for a line diff (%d vs %d lines); "
               "field-grouped summary instead @@" % (len(mid_a), len(mid_b)))
    ca, cb = collections.Counter(mid_a), collections.Counter(mid_b)
    only_a, only_b = ca - cb, cb - ca
    fa, fb = collections.Counter(), collections.Counter()
    for ln, k in only_a.items():
        fa[_diff_field_name(ln)] += k
    for ln, k in only_b.items():
        fb[_diff_field_name(ln)] += k
    names = sorted(set(fa) | set(fb), key=lambda n: -(fa[n] + fb[n]))
    out.append("  %-44s %8s %8s" % ("field", "A-only", "B-only"))
    for n in names[:40]:
        out.append("  %-44s %8d %8d" % (n[:44], fa[n], fb[n]))
    out.append("@@ first differing lines (positional) @@")
    shown = 0
    for i in range(min(len(mid_a), len(mid_b))):
        if mid_a[i] != mid_b[i]:
            out.append("-" + mid_a[i][:160])
            out.append("+" + mid_b[i][:160])
            shown += 1
            if shown >= 20:
                break
    return out, sum(only_a.values()) + sum(only_b.values())


def diff_dumps(ta, tb, pa, pb):
    """Return (diff_lines, changed_count) for two state dump texts."""
    a, b = ta.splitlines(), tb.splitlines()
    n_a, n_b = len(a), len(b)

    lo = 0
    while lo < n_a and lo < n_b and a[lo] == b[lo]:
        lo += 1
    hi = 0
    while hi < (n_a - lo) and hi < (n_b - lo) and a[n_a - 1 - hi] == b[n_b - 1 - hi]:
        hi += 1
    mid_a, mid_b = a[lo:n_a - hi], b[lo:n_b - hi]

    header = ["--- %s" % pa, "+++ %s" % pb,
              "@@ identical prefix %d lines, identical suffix %d lines @@" % (lo, hi)]
    if not mid_a and not mid_b:
        return header + ["(dumps are identical)"], 0
    if max(len(mid_a), len(mid_b)) <= DIFF_LINE_BUDGET:
        body = list(difflib.unified_diff(mid_a, mid_b, fromfile=pa, tofile=pb,
                                         lineterm="", n=2))
        changed = sum(1 for l in body if l[:1] in "+-" and not l.startswith(("+++", "---")))
        return header[2:] + body, changed
    return _diff_summary(mid_a, mid_b, header)


def dump_both(a, b, label_a, label_b, ahead, out_dir, log):
    """Capture both instances' full state text at the same future GGPO frame and diff it.

    Returns a dict for the report: frame, files, diff_file, diff_lines, preview
    (or {"error": ...})."""
    try:
        fa = a.frame()[0]
        fb = b.frame()[0]
        if fa < 0 or fb < 0:
            return {"error": "no GGPO session on one side (frame %s/%s)" % (fa, fb)}
        target = max(fa, fb) + ahead
        a.dump_at(target)
        b.dump_at(target)
        log(f"  requesting state dumps at frame {target} (now {fa}/{fb}) ...")
        wait = max(30.0, ahead / 60.0 * 4 + 10)
        a.wait_dump(target, timeout=wait)
        b.wait_dump(target, timeout=wait)
        os.makedirs(out_dir, exist_ok=True)
        pa = os.path.join(out_dir, f"statedump_{label_a}_{target}.txt")
        pb = os.path.join(out_dir, f"statedump_{label_b}_{target}.txt")
        ta = a.dump_get(save_to=pa)["text"]
        tb = b.dump_get(save_to=pb)["text"]
        t0 = time.time()
        diff, changed = diff_dumps(ta, tb, pa, pb)
        log(f"  diffed {len(ta.splitlines())}/{len(tb.splitlines())} dump lines in {time.time() - t0:.1f}s")
        pd = os.path.join(out_dir, f"statedump_diff_{target}.txt")
        with open(pd, "w", newline="\n") as fh:
            fh.write("\n".join(diff) + ("\n" if diff else ""))
        return {"frame": target, "files": [pa, pb], "diff_file": pd,
                "diff_lines": changed, "preview": diff[:60]}
    except Exception as e:  # network, timeout, protocol -- report, don't crash the verdict
        return {"error": f"{type(e).__name__}: {e}"}


def live_compare(port_a, port_b, labels, host, frames_wanted, duration, poll, stall,
                 start_timeout, context, tail_tolerance, dump_on_desync, dump_ahead,
                 out_dir, stop_on_desync, log):
    """Poll both instances' `hashes` and compare as frames confirm.

    Returns (host_log, guest_log, meta) where meta has: reason (desync | frames |
    duration | session_ended | stall | start_timeout), polls, dump (or None)."""
    a, b = _clients(port_a, port_b, host)
    la = ParsedLog(f"live:{host}:{port_a}", labels[0])
    lb = ParsedLog(f"live:{host}:{port_b}", labels[1])
    sides = (("a", a, la), ("b", b, lb))
    since = {"a": -1, "b": -1}
    last_advance = {"a": time.time(), "b": time.time()}
    seen_started = {"a": False, "b": False}
    ended = {"a": False, "b": False}
    t0 = time.time()
    meta = {"reason": None, "polls": 0, "dump": None, "desync_frame": None}
    try:
        while True:
            meta["polls"] += 1
            for key, cli, lg in sides:
                r = cli.hashes(since[key])
                h = r["hashes"]
                if h.get("gap"):
                    raise RuntimeError(
                        f"{lg.label}: hash ring no longer reaches frame {since[key] + 1} "
                        f"(oldest kept {h['oldest']}); poller fell behind -- restart with a shorter --poll")
                for e in h["frames"]:
                    lg.ingest(e["f"], _hex_cols(e))
                if h["frames"]:
                    since[key] = h["frames"][-1]["f"]
                    last_advance[key] = time.time()
                started = bool(r.get("ggpo_started", True))
                if started:
                    seen_started[key] = True
                elif seen_started[key]:
                    ended[key] = True

            r = compare(la, lb, context, tail_tolerance) if (la.frames and lb.frames) else None
            if r and r["first_div"] is not None and meta["desync_frame"] is None:
                meta["desync_frame"] = r["first_div"]
                log(f"  DESYNC detected at frame {r['first_div']} "
                    f"({', '.join(c for c in r['diff_cols'] if c != ROLLUP_COL) or 'rollup only'})")
                if dump_on_desync:
                    meta["dump"] = dump_both(a, b, la.label, lb.label, dump_ahead, out_dir, log)
                if stop_on_desync:
                    meta["reason"] = "desync"
                    break
            if frames_wanted and r and r["common_count"] >= frames_wanted:
                meta["reason"] = "frames"
                break
            if duration and time.time() - t0 >= duration:
                meta["reason"] = "duration"
                break
            if ended["a"] or ended["b"]:
                meta["reason"] = "session_ended"
                break
            if not (seen_started["a"] and seen_started["b"]) and time.time() - t0 >= start_timeout:
                meta["reason"] = "start_timeout"
                break
            now = time.time()
            for key, other in (("a", "b"), ("b", "a")):
                if (seen_started[key] and now - last_advance[key] >= stall
                        and now - last_advance[other] < stall / 2):
                    meta["reason"] = "stall"
                    meta["stalled"] = key
                    break
            if meta["reason"]:
                break
            time.sleep(poll)
    finally:
        a.close()
        b.close()
    return la, lb, meta


def main(argv=None):
    ap = argparse.ArgumentParser(
        description="Compare two instances' STATEHASH streams (log files, or live over the "
                    "control plane); report the first divergence."
    )
    ap.add_argument("host_log", nargs="?", help="path to the host instance's dsoverhaul_statehash_logging.txt")
    ap.add_argument("guest_log", nargs="?", help="path to the guest instance's dsoverhaul_statehash_logging.txt")
    ap.add_argument("--json", action="store_true", help="emit a machine-readable JSON report")
    ap.add_argument("--context", type=int, default=6, help="frames of context around the divergence (default 6)")
    ap.add_argument("--labels", default="host,guest", help="comma-separated labels for the two sides")
    ap.add_argument("--tail-tolerance", type=int, default=10,
                    help="max benign difference between the two sides' final confirmed "
                         "frame; beyond this one side is treated as crashed/stalled (default 10)")
    live = ap.add_argument_group("live (poll two running instances instead of reading logs)")
    live.add_argument("--live", metavar="A_PORT,B_PORT", help="control-plane ports of instance A and B")
    live.add_argument("--host", default="127.0.0.1")
    live.add_argument("--frames", type=int, default=0, help="stop (PASS) once this many frames compared identical")
    live.add_argument("--duration", type=float, default=0, help="stop after this many seconds")
    live.add_argument("--poll", type=float, default=0.5, help="seconds between polls (default 0.5)")
    live.add_argument("--stall", type=float, default=10.0,
                      help="seconds without new confirmed frames on one side (while the other advances) "
                           "before reporting a stall (default 10)")
    live.add_argument("--start-timeout", type=float, default=600.0,
                      help="seconds to wait for GGPO to start on both sides (default 600)")
    live.add_argument("--continue", dest="keep_going", action="store_true",
                      help="keep comparing after the first divergence (default: stop)")
    live.add_argument("--dump-on-desync", action="store_true",
                      help="on divergence, dump both instances' full state at the same frame and diff")
    live.add_argument("--dump-ahead", type=int, default=30, help="frames ahead to schedule the dump (default 30)")
    live.add_argument("--out-dir", default=".", help="where statedump_*.txt and the diff go (default .)")
    args = ap.parse_args(argv)

    labels = [s.strip() for s in args.labels.split(",")]
    if len(labels) != 2:
        ap.error("--labels needs exactly two comma-separated values")

    meta = None
    if args.live:
        try:
            pa, pb = (int(x) for x in args.live.split(","))
        except ValueError:
            ap.error("--live needs A_PORT,B_PORT")
        log = (lambda m: None) if args.json else (lambda m: print(m, flush=True))
        log(f"ROLLBACK STATE COMPARE -- live {args.host}:{pa} ({labels[0]}) vs {args.host}:{pb} ({labels[1]})")
        try:
            host, guest, meta = live_compare(
                pa, pb, labels, args.host, args.frames, args.duration, args.poll, args.stall,
                args.start_timeout, args.context, args.tail_tolerance, args.dump_on_desync,
                args.dump_ahead, args.out_dir, not args.keep_going, log)
        except (OSError, RuntimeError, KeyError, ValueError) as e:
            sys.stderr.write(f"error: live compare failed: {e}\n")
            return 2
        if meta["reason"] == "start_timeout":
            sys.stderr.write("error: GGPO never started on both instances (is rollback on, are they connected?)\n")
            return 2
    else:
        if not args.host_log or not args.guest_log:
            ap.error("give HOST_LOG GUEST_LOG, or --live A_PORT,B_PORT")
        host = ParsedLog(args.host_log, labels[0])
        guest = ParsedLog(args.guest_log, labels[1])
        if not host.parse() or not guest.parse():
            return 2

    if not host.frames or not guest.frames:
        sys.stderr.write(
            "error: no STATEHASH frames found in "
            + ("both sides" if not host.frames and not guest.frames
               else (host.path if not host.frames else guest.path))
            + ". Is the determinism oracle wired into the DLL and rollback enabled?\n"
        )
        return 2

    tail_tol = args.tail_tolerance
    if meta and meta["reason"] != "stall":
        # Live: the peers confirm the same frames a few frames apart, so at the
        # instant we stop one side is always "ahead". Only a time-based stall
        # (one side stopped advancing while the other kept going) is a truncation.
        tail_tol = float("inf")
    r = compare(host, guest, args.context, tail_tol)

    if r["common_count"] == 0:
        sys.stderr.write(
            f"error: no overlapping confirmed frames "
            f"({host.label} {host.min_frame}..{host.max_frame}, "
            f"{guest.label} {guest.min_frame}..{guest.max_frame}). "
            f"Were both logs from the same session?\n"
        )
        return 2

    if args.json:
        out = build_json(host, guest, r)
        if meta:
            out["live"] = {"reason": meta["reason"], "polls": meta["polls"],
                           "stalled": meta.get("stalled"), "dump": meta["dump"]}
        print(json.dumps(out, indent=2))
    else:
        print_human(host, guest, r, args.context)
        if meta:
            print(f"\n  live: stopped because of '{meta['reason']}' after {meta['polls']} polls"
                  + (f" ({meta['stalled']} side stalled)" if meta.get("stalled") else ""))
            print_dump_info(meta["dump"])

    return 0 if is_clean(r) else 1


if __name__ == "__main__":
    raise SystemExit(main())
