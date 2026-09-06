#!/usr/bin/env python3
"""
harness_client.py -- orchestrator-side API for the rollback test harness.

Three things live here, all stdlib-only:

1. HarnessClient -- talks to one game instance's control plane (OverhaulDLL
   HarnessControl.cpp: localhost TCP, one command per line, one JSON reply per
   line). Replaces the F6-F12 hotkeys and gives the orchestrator a reliable
   "is the character loaded / is GGPO running / what frame is it" signal, plus a
   live stream of the mod log (also for the Sandboxie'd instance, whose log
   file is otherwise buried in the sandbox's virtual filesystem).

2. InputScript -- builds the deterministic per-frame input list the DLL lays
   over the live controller input (OverhaulDLL RollbackScript.h). Frames are
   GGPO frames: the same index STATEHASH lines and recordings use, and shared
   by both players, so a two-player scenario is one script per instance with
   frame numbers that mean the same thing on each side.

3. Recording tools -- decode rollback_recording.bin (the RollbackReplay
   "DSRINP01" format) into named fields, show what changed frame to frame, and
   convert a recorded segment into an editable script. This is the practical
   way to get MOVEMENT into a script: the packed movement fields are resolved
   world-space values, not stick positions (see RollbackScript.h), so record a
   human doing it, then lift/convert the frames.

Enable the control plane per instance with env var DSR_HARNESS_PORT=<port>
(launch_harness.py --control-port does this) or HarnessControlPort= in the
[Dark Souls Overhaul Testing] section of d3d11_mod.ini. The DLL falls forward
to the next free port (up to +9) if two instances share one base port; use
`discover` or match `status`["pid"] to tell them apart.

CLI (see --help): status, cmd, rollback, record, script, end-session, logs,
discover, dump-recording, rec2script, validate, example.
"""

import argparse
import json
import os
import socket
import struct
import sys
import threading
import time

DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 42800
DEFAULT_PORT_SPAN = 10          # the DLL tries base..base+9

# ---------------------------------------------------------------------------
# RollbackInput wire layout (OverhaulDLL Rollback.h / PadManipulatorStruct.h,
# #pragma pack(1)). RollbackScript.h static_asserts pin these sizes/offsets;
# if the DLL build fails on one of those asserts, fix this table to match.
# ---------------------------------------------------------------------------

INPUT_MAGIC = b"DSRINP01"
INPUT_SIZE = 231
HEADER_FMT = "<8sII"            # magic, input_size, reserved
RECORD_FRAME_FMT = "<i"         # int32 frame, then INPUT_SIZE bytes

# (name, kind, offset[, bit, width]) -- kind: B=u8, i=i32, I=u32, f=f32, bit
_L = []


def _add(name, kind, off, bit=None, width=1):
    _L.append((name, kind, off, bit, width))


_add("const1", "B", 0)
# RollbackInput bitfield storage unit (unsigned int) @1
_add("bTargetLocked", "bit", 1, 0)
_add("bTargetLocked_Alt", "bit", 1, 1)
# PadManipulatorPacked @5 -- bitfield storage unit (unsigned int) @5
_PM = 5
for _i, _n in enumerate([
        "r1_weapon_attack_input", "l1_input", "r1_magic_attack_input", "l1_magic_attack_input",
        "r2_input", "backstep_input", "use_button_pressed", "roll_forward_input",
        "lefthand_weapon_attack", "parry_input", "block_input", "jump_input",
        "l1_weapon_attack", "l2_weapon_attack", "isSprintingAnim", "not_getting_movement_input"]):
    _add(_n, "bit", _PM, _i)
_add("change_2handing_state", "bit", _PM, 16, 2)
_add("left_hand_slot_selected", "bit", _PM, 18)
_add("right_hand_slot_selected", "bit", _PM, 19)
_add("any_action_inputted", "bit", _PM, 20)
_add("EnableBackStep", "bit", _PM, 21)
_add("EnableBackStep_forward", "bit", _PM, 22)
_add("movement_related_flags", "B", _PM + 4)
_o = _PM + 5
for _n in ["camera_x_direction_movement_input_amount", "camera_x_direction_movement_input_amount_alt",
           "camera_y_direction_movement_input_amount", "camera_y_direction_movement_input_amount_alt",
           "continuous_weapon_controlled_angle", "delta_pc_rotation_perframe",
           "weapon_controlled_angle", "pc_rotation",
           "delta_camera_y_rotation_perframe", "delta_camera_x_rotation_perframe",
           "camera_y_rotation", "camera_x_rotation"]:
    _add(_n, "f", _o)
    _o += 4
for _i in range(4):
    _add("movement_velocity[%d]" % _i, "f", _o)
    _o += 4
_add("TimeRollButtonHeld", "f", _o); _o += 4
_add("Backstep_timer", "f", _o); _o += 4
_add("LockonTargetHandle", "i", _o); _o += 4          # uint32 in C; -1 == none, shown signed
_add("CurrentFrame_ActionInputs_ButtonId", "i", _o); _o += 4
for _i in range(6):
    _add("y_movement_input[%d]" % _i, "f", _o); _o += 4
for _i in range(6):
    _add("x_movement_input[%d]" % _i, "f", _o); _o += 4
_add("cur_movement_input_index_to_use", "i", _o); _o += 4
assert _o == 142, _o                                   # sizeof(PadManipulatorPacked) == 137
_add("curSelectedMagicSlot", "B", 142)
_add("curUsingInventoryItemId", "i", 143)
_add("curSelectedQuickbarItemId", "i", 147)
for _i in range(20):
    _add("equipment_array[%d]" % _i, "i", 151 + 4 * _i)
assert 151 + 80 == INPUT_SIZE

LAYOUT = tuple(_L)
FIELD_NAMES = [f[0] for f in LAYOUT]
del _L, _o, _i, _n

# Short names accepted by the DLL parser (RollbackScript.h ALIASES / GROUPS).
ALIASES = {
    "r1": "r1_weapon_attack_input", "l1": "l1_input", "r2": "r2_input", "l2": "l2_weapon_attack",
    "r1_magic": "r1_magic_attack_input", "l1_magic": "l1_magic_attack_input",
    "roll": "roll_forward_input", "backstep": "backstep_input", "use": "use_button_pressed",
    "parry": "parry_input", "block": "block_input", "jump": "jump_input",
    "lefthand_attack": "lefthand_weapon_attack", "l1_weapon": "l1_weapon_attack",
    "sprint": "isSprintingAnim", "two_hand": "change_2handing_state", "lockon": "LockonTargetHandle",
    "move_x": "camera_x_direction_movement_input_amount", "move_y": "camera_y_direction_movement_input_amount",
    "move_x_alt": "camera_x_direction_movement_input_amount_alt",
    "move_y_alt": "camera_y_direction_movement_input_amount_alt",
    "rotation": "pc_rotation", "turn": "delta_pc_rotation_perframe", "no_move": "not_getting_movement_input",
    "any_action": "any_action_inputted",
}
GROUPS = {
    "movement_velocity": ["movement_velocity[%d]" % i for i in range(4)],
    "x_movement_input": ["x_movement_input[%d]" % i for i in range(6)],
    "stick_x": ["x_movement_input[%d]" % i for i in range(6)],
    "y_movement_input": ["y_movement_input[%d]" % i for i in range(6)],
    "stick_y": ["y_movement_input[%d]" % i for i in range(6)],
    "equipment_array": ["equipment_array[%d]" % i for i in range(20)],
}
KNOWN_KEYS = set(FIELD_NAMES) | set(ALIASES) | set(GROUPS)

BUTTONS = ["r1_weapon_attack_input", "l1_input", "r1_magic_attack_input", "l1_magic_attack_input",
           "r2_input", "backstep_input", "use_button_pressed", "roll_forward_input",
           "lefthand_weapon_attack", "parry_input", "block_input", "jump_input",
           "l1_weapon_attack", "l2_weapon_attack"]

# Fields the DLL's "!neutral" pragma forces every frame, with their neutral
# value (RollbackScript.h FIELDS neutral_zero/neutral_value). Everything else is
# state-derived (facing, camera angles, lock-on, equipment...) and passes through.
NEUTRAL = {n: 0 for n in BUTTONS}
NEUTRAL.update({
    "isSprintingAnim": 0, "not_getting_movement_input": 1, "change_2handing_state": 0,
    "left_hand_slot_selected": 0, "right_hand_slot_selected": 0, "any_action_inputted": 0,
    "camera_x_direction_movement_input_amount": 0.0, "camera_x_direction_movement_input_amount_alt": 0.0,
    "camera_y_direction_movement_input_amount": 0.0, "camera_y_direction_movement_input_amount_alt": 0.0,
    "delta_pc_rotation_perframe": 0.0, "delta_camera_y_rotation_perframe": 0.0,
    "delta_camera_x_rotation_perframe": 0.0, "TimeRollButtonHeld": 0.0, "Backstep_timer": 0.0,
    "CurrentFrame_ActionInputs_ButtonId": -1,
})
for _i in range(4):
    NEUTRAL["movement_velocity[%d]" % _i] = 0.0
for _i in range(6):
    NEUTRAL["x_movement_input[%d]" % _i] = 0.0
    NEUTRAL["y_movement_input[%d]" % _i] = 0.0
del _i
PLAYER_FIELDS = set(NEUTRAL)          # "player-driven"; the rest are state-derived
STATE_FIELDS = set(FIELD_NAMES) - PLAYER_FIELDS

MOVEMENT_FIELDS = [n for n in FIELD_NAMES if n.startswith(("camera_", "movement_velocity", "x_movement", "y_movement"))
                   or n in ("pc_rotation", "delta_pc_rotation_perframe", "not_getting_movement_input",
                            "isSprintingAnim", "cur_movement_input_index_to_use", "movement_related_flags")]


# ---------------------------------------------------------------------------
# Recording decode
# ---------------------------------------------------------------------------

def decode_input(data, offset=0):
    """Decode one RollbackInput (INPUT_SIZE bytes at offset) into {field: value}."""
    out = {}
    for name, kind, off, bit, width in LAYOUT:
        o = offset + off
        if kind == "bit":
            unit = struct.unpack_from("<I", data, o)[0]
            out[name] = (unit >> bit) & ((1 << width) - 1)
        elif kind == "f":
            out[name] = struct.unpack_from("<f", data, o)[0]
        elif kind == "i":
            out[name] = struct.unpack_from("<i", data, o)[0]
        elif kind == "I":
            out[name] = struct.unpack_from("<I", data, o)[0]
        else:
            out[name] = data[o]
    return out


def read_recording(path):
    """Yield (frame, fields) for every record in a RollbackReplay .bin.

    Tolerates a truncated tail record (the DLL writes+flushes per frame, so a
    crash mid-write leaves a partial one)."""
    with open(path, "rb") as fh:
        data = fh.read()
    hsz = struct.calcsize(HEADER_FMT)
    if len(data) < hsz:
        raise ValueError("%s: too short for a recording header" % path)
    magic, input_size, _reserved = struct.unpack_from(HEADER_FMT, data, 0)
    if magic != INPUT_MAGIC:
        raise ValueError("%s: bad magic %r (expected %r)" % (path, magic, INPUT_MAGIC))
    if input_size != INPUT_SIZE:
        raise ValueError("%s: input_size %d != %d known to this script; the DLL's RollbackInput "
                         "layout differs from harness_client.LAYOUT" % (path, input_size, INPUT_SIZE))
    pos = hsz
    rec = struct.calcsize(RECORD_FRAME_FMT) + input_size
    while pos + rec <= len(data):
        frame = struct.unpack_from(RECORD_FRAME_FMT, data, pos)[0]
        yield frame, decode_input(data, pos + 4)
        pos += rec


def frame_changes(records, fields=None):
    """Yield (frame, {name: (old, new)}) for fields that changed since the previous record."""
    prev = None
    for frame, cur in records:
        if prev is None:
            yield frame, {k: (None, v) for k, v in cur.items() if fields is None or k in fields}
        else:
            d = {}
            for k, v in cur.items():
                if fields is not None and k not in fields:
                    continue
                if prev.get(k) != v:
                    d[k] = (prev.get(k), v)
            if d:
                yield frame, d
        prev = cur


def _fmt_value(v):
    if isinstance(v, bool):
        return "1" if v else "0"
    if isinstance(v, int):
        return str(v)
    if isinstance(v, float):
        if v == int(v) and abs(v) < 1e15:
            return str(int(v))
        return "%.9g" % v
    raise TypeError("unsupported value %r" % (v,))


def _parse_frames_arg(s):
    """'A-B' -> (A, B); 'A' -> (A, A); None -> (None, None)."""
    if s is None:
        return None, None
    if "-" in s:
        a, b = s.split("-", 1)
        return int(a, 0), int(b, 0)
    v = int(s, 0)
    return v, v


# ---------------------------------------------------------------------------
# Input script builder
# ---------------------------------------------------------------------------

class ScriptError(ValueError):
    pass


def _check_key(key):
    if key not in KNOWN_KEYS:
        raise ScriptError("unknown field %r (struct member, alias, or group; see harness_client.FIELD_NAMES)" % key)


def _frame_spec(start, end):
    if start == "*":
        return "@*"
    if not isinstance(start, int) or start < 0:
        raise ScriptError("frame must be a non-negative int or '*', got %r" % (start,))
    if end is None or end == start:
        return "@%d" % start
    if not isinstance(end, int) or end < start:
        raise ScriptError("end frame %r must be >= start %r" % (end, start))
    return "@%d-%d" % (start, end)


class InputScript:
    """Build the text the DLL's RollbackScript parser reads.

    Semantics (RollbackScript.h): each directive names fields to override on a
    frame range; unnamed fields keep the live value. With neutral=True the
    player-driven fields are zeroed on every frame first, so only what the
    script says is pressed is pressed.

        s = InputScript(name="r1 then roll", neutral=True)
        s.press("r1", at=120)              # one frame
        s.hold("block", 200, 260)          # inclusive
        s.span(300, 5, roll=1)             # 5 frames from 300
        s.set(400, pc_rotation=1.5707, camera_x_direction_movement_input_amount=0.8)
        client.script_upload(s)            # or s.save(path); DLL: "script load <path>"
    """

    def __init__(self, name=None, neutral=False):
        self.name = name
        self.neutral = neutral
        self._directives = []          # (start, end, [(key, value), ...])

    # -- authoring --
    def set(self, start, end=None, **fields):
        """Override `fields` on frames start..end (inclusive; end=None -> just start; start='*' -> every frame)."""
        if not fields:
            raise ScriptError("no fields given")
        items = []
        for k, v in fields.items():
            _check_key(k)
            if isinstance(v, bool):
                v = int(v)
            if not isinstance(v, (int, float)):
                raise ScriptError("value for %s must be int/float/bool, got %r" % (k, v))
            items.append((k, v))
        _frame_spec(start, end)   # validate
        self._directives.append((start, end, items))
        return self

    def span(self, start, count, **fields):
        """Override on `count` frames starting at `start`."""
        if count < 1:
            raise ScriptError("count must be >= 1")
        return self.set(start, start + count - 1, **fields)

    def every(self, **fields):
        return self.set("*", None, **fields)

    def press(self, button, at, hold=1):
        """Button (alias or struct name) held for `hold` frames from `at`."""
        return self.span(at, hold, **{button: 1})

    def hold(self, button, start, end):
        return self.set(start, end, **{button: 1})

    def raw(self, line):
        """Append a pre-formatted directive/pragma line (validated by validate_text)."""
        errs = validate_text(line)
        if errs:
            raise ScriptError("; ".join(errs))
        self._directives.append(("raw", None, line))
        return self

    # -- output --
    def lines(self):
        out = []
        if self.name:
            out.append("!name %s" % self.name)
        if self.neutral:
            out.append("!neutral")
        for start, end, items in self._directives:
            if start == "raw":
                out.append(items)
                continue
            out.append(_frame_spec(start, end) + " " + " ".join("%s=%s" % (k, _fmt_value(v)) for k, v in items))
        return out

    def text(self):
        return "\n".join(self.lines()) + "\n"

    def save(self, path):
        with open(path, "w", newline="\n") as fh:
            fh.write("# generated by harness_client.py\n")
            fh.write(self.text())
        return path

    def __len__(self):
        return len(self._directives)


def validate_text(text):
    """Mirror of the DLL parser's grammar; returns a list of 'line N: ...' errors (empty = OK)."""
    errs = []
    for n, raw in enumerate(text.splitlines(), 1):
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        if line.startswith("!"):
            pragma = line.split(None, 1)[0]
            if pragma not in ("!neutral", "!name"):
                errs.append("line %d: unknown pragma %s" % (n, pragma))
            continue
        toks = line.split()
        if not toks[0].startswith("@"):
            errs.append("line %d: directive must start with @N, @A-B, @A+N or @*" % n)
            continue
        spec = toks[0][1:]
        try:
            if spec == "*":
                pass
            elif "-" in spec:
                a, b = spec.split("-", 1)
                if int(b, 0) < int(a, 0):
                    errs.append("line %d: range end < start" % n)
            elif "+" in spec:
                a, c = spec.split("+", 1)
                int(a, 0)
                if int(c, 0) < 1:
                    errs.append("line %d: span count must be >= 1" % n)
            else:
                int(spec, 0)
        except ValueError:
            errs.append("line %d: bad frame spec %s" % (n, toks[0]))
        if len(toks) < 2:
            errs.append("line %d: no field assignments" % n)
        for t in toks[1:]:
            if "=" not in t:
                errs.append("line %d: expected field=value, got %r" % (n, t))
                continue
            k, v = t.split("=", 1)
            if k not in KNOWN_KEYS:
                errs.append("line %d: unknown field %r" % (n, k))
            if v not in ("true", "false", "on", "off"):
                try:
                    float(v) if not v.lower().startswith("0x") else int(v, 16)
                except ValueError:
                    errs.append("line %d: bad value %r for %s" % (n, v, k))
    return errs


def recording_to_script(records, frames=None, fields=None, include_state=False, name=None):
    """Convert recorded frames into an InputScript that reproduces them.

    Emits, per frame, the player-driven fields whose value differs from the
    neutral default, under "!neutral" -- so idle frames emit nothing and the
    result is exact for those fields. With include_state=True every field is
    emitted (a full per-frame override, i.e. an editable replay).
    `frames`=(lo, hi) limits the window; `fields` limits which fields are kept.
    """
    lo, hi = frames if frames else (None, None)
    s = InputScript(name=name, neutral=True)
    for frame, cur in records:
        if lo is not None and frame < lo:
            continue
        if hi is not None and frame > hi:
            break
        items = {}
        for k, v in cur.items():
            if fields is not None and k not in fields:
                continue
            if k in NEUTRAL:
                if v != NEUTRAL[k]:
                    items[k] = v
            elif include_state:
                items[k] = v
        if items:
            s.set(frame, **items)
    return s


# ---------------------------------------------------------------------------
# Control-plane client
# ---------------------------------------------------------------------------

class HarnessError(RuntimeError):
    def __init__(self, message, reply=None):
        super().__init__(message)
        self.reply = reply


class HarnessClient:
    """One connection to one instance's control plane. Thread-unsafe; use one per thread."""

    def __init__(self, port=DEFAULT_PORT, host=DEFAULT_HOST, timeout=15.0):
        self.host = host
        self.port = port
        self.timeout = timeout
        self._sock = None
        self._buf = b""

    # -- connection --
    def connect(self):
        s = socket.create_connection((self.host, self.port), timeout=self.timeout)
        s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        self._sock = s
        return self

    def close(self):
        if self._sock:
            try:
                self._sock.close()
            finally:
                self._sock = None

    def __enter__(self):
        return self.connect()

    def __exit__(self, *exc):
        self.close()

    def _readline(self):
        while b"\n" not in self._buf:
            chunk = self._sock.recv(65536)
            if not chunk:
                raise HarnessError("connection closed by the game")
            self._buf += chunk
        line, self._buf = self._buf.split(b"\n", 1)
        return line.decode("utf-8", "replace")

    def cmd(self, line, check=True):
        """Send one command line, return the parsed JSON reply. Raises HarnessError if !ok and check."""
        if self._sock is None:
            self.connect()
        if "\n" in line:
            raise ValueError("command must be a single line")
        self._sock.sendall((line + "\n").encode("utf-8"))
        raw = self._readline()
        try:
            reply = json.loads(raw)
        except ValueError:
            raise HarnessError("non-JSON reply: %r" % raw)
        if check and not reply.get("ok", False):
            raise HarnessError("%s -> %s" % (line, reply.get("error", raw)), reply)
        return reply

    # -- simple wrappers --
    def ping(self):
        return self.cmd("ping")

    def status(self):
        return self.cmd("status")

    def frame(self):
        r = self.cmd("frame")
        return r["frame"], r["confirmed_frame"]

    def input(self):
        return self.cmd("input")["input"]

    def rollback(self, on):
        return self.cmd("rollback %s" % ("on" if on else "off"))

    def network(self, on):
        return self.cmd("network %s" % ("on" if on else "off"))

    def record(self, arm):
        return self.cmd("record %s" % ("arm" if arm else "disarm"))

    def record_file(self, path):
        return self.cmd("record file %s" % path)

    def replay_file(self, path):
        return self.cmd("replay file %s" % path)

    def replay_off(self):
        return self.cmd("replay off")

    def script_load(self, path_in_game):
        """Load a script file by a path the GAME process can open."""
        return self.cmd("script load %s" % path_in_game)["script"]

    def script_clear(self):
        return self.cmd("script clear")["script"]

    def script_status(self):
        return self.cmd("script status")["script"]

    def script_add(self, directive):
        return self.cmd("script add %s" % directive)["script"]

    def script_upload(self, script_or_text):
        """Replace the instance's script with an InputScript / text, line by line
        (no shared filesystem needed). Raises HarnessError naming the bad line."""
        text = script_or_text.text() if isinstance(script_or_text, InputScript) else script_or_text
        errs = validate_text(text)
        if errs:
            raise ScriptError("; ".join(errs))
        self.script_clear()
        last = None
        for n, raw in enumerate(text.splitlines(), 1):
            line = raw.split("#", 1)[0].strip()
            if not line:
                continue
            try:
                last = self.script_add(line)
            except HarnessError as e:
                raise HarnessError("script line %d (%r): %s" % (n, line, e), e.reply)
        return last if last is not None else self.script_status()

    def hotkey(self, name):
        return self.cmd("hotkey %s" % name)

    def end_session(self):
        return self.cmd("end_session")

    def log(self, text):
        return self.cmd("log %s" % text)

    # -- determinism oracle (StateHash.h via the control plane) --
    def hashes(self, since=-1, max_count=2000):
        """Confirmed per-frame state digests with frame > since, oldest first.

        Reply: {"frame", "confirmed_frame", "ggpo_started", "hashes": {"frames":
        [{"f": N, "player": hex, "bullet": hex, ..., "comb": hex}], "count",
        "oldest", "newest", "gap"}}. "gap" true means the DLL's ring (~2.3 min)
        no longer reaches since+1 -- the poller fell behind."""
        return self.cmd("hashes %d %d" % (since, max_count))

    def dump_at(self, frame):
        """Arm a full canonical state dump for GGPO frame `frame` (int) or '+N' frames ahead.
        Both instances dumped at the same frame diff field-by-field (rollback_compare.py --live --dump-on-desync)."""
        return self.cmd("dump_at %s" % frame)

    def dump_status(self):
        return self.cmd("dump_status")

    def dump_get(self, save_to=None):
        """Fetch the captured dump text (large). Optionally write it to `save_to`."""
        r = self.cmd("dump_get")
        if save_to:
            with open(save_to, "w", newline="\n") as fh:
                fh.write(r["text"])
        return r

    def wait_dump(self, frame, timeout=60.0, poll=0.25):
        """Block until the dump for `frame` is captured AND confirmed (no more re-saves)."""
        deadline = time.time() + timeout
        last = None
        while time.time() < deadline:
            last = self.dump_status()
            if last["dump_frame"] == frame and last["dump_confirmed"]:
                return last
            time.sleep(poll)
        raise TimeoutError("dump for frame %d not confirmed after %.0fs (last: %s)" % (frame, timeout, json.dumps(last)))

    def probe(self):
        """Per connected player: hp, max_hp, x, y, z, rot -- plus the current GGPO frame."""
        return self.cmd("probe")

    # -- waiting --
    def wait_for(self, predicate, timeout=120.0, poll=0.25, what="condition"):
        """Poll status() until predicate(status) is true; return the status. Raises TimeoutError."""
        deadline = time.time() + timeout
        last = None
        while time.time() < deadline:
            last = self.status()
            if predicate(last):
                return last
            time.sleep(poll)
        raise TimeoutError("timed out after %.0fs waiting for %s (last status: %s)" % (timeout, what, json.dumps(last)))

    def wait_char_loaded(self, timeout=180.0):
        return self.wait_for(lambda s: s["char_loaded"], timeout, what="char_loaded")

    def wait_players(self, n=2, timeout=300.0):
        return self.wait_for(lambda s: s["connected_players"] >= n, timeout, what="%d connected players" % n)

    def wait_ggpo_ready(self, timeout=120.0):
        return self.wait_for(lambda s: s["ggpo_started"] and s["ggpo_ready"] == "Ready", timeout, what="GGPO Ready")

    def wait_frame(self, frame, timeout=120.0):
        return self.wait_for(lambda s: s["frame"] >= frame, timeout, poll=0.05, what="frame %d" % frame)

    # -- log stream --
    def subscribe(self, callback, stop=None):
        """Open a SECOND connection in "subscribe" mode and call callback(entry)
        for every {"log":..., "t":...} line until `stop` (threading.Event) is
        set or the game goes away. Returns the daemon thread."""
        return LogStream(self.port, self.host, callback, stop).start()


class LogStream:
    """Background reader for the control plane's log stream."""

    def __init__(self, port, host, callback, stop=None):
        self.port, self.host, self.callback = port, host, callback
        self.stop = stop or threading.Event()
        self.thread = None
        self.error = None

    def start(self):
        self.thread = threading.Thread(target=self._run, name="harness-log-%d" % self.port, daemon=True)
        self.thread.start()
        return self

    def _run(self):
        try:
            with socket.create_connection((self.host, self.port), timeout=5.0) as s:
                s.sendall(b"subscribe\n")
                s.settimeout(0.5)
                buf = b""
                while not self.stop.is_set():
                    try:
                        chunk = s.recv(65536)
                    except socket.timeout:
                        continue
                    if not chunk:
                        break
                    buf += chunk
                    while b"\n" in buf:
                        line, buf = buf.split(b"\n", 1)
                        try:
                            entry = json.loads(line.decode("utf-8", "replace"))
                        except ValueError:
                            continue
                        if "log" in entry:
                            self.callback(entry)
        except OSError as e:
            self.error = e

    def join(self, timeout=None):
        self.thread.join(timeout)


def discover(base_port=DEFAULT_PORT, count=DEFAULT_PORT_SPAN, host=DEFAULT_HOST, pid=None, timeout=1.0):
    """Probe base_port..base_port+count-1; return [(port, status)] for live instances
    (optionally only the one whose status["pid"] == pid)."""
    found = []
    for p in range(base_port, base_port + count):
        try:
            with HarnessClient(p, host, timeout=timeout) as c:
                st = c.status()
        except (OSError, HarnessError):
            continue
        if pid is None or st.get("pid") == pid:
            found.append((p, st))
    return found


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

EXAMPLE_SCRIPT = """\
# Example rollback input script (see RollbackScript.h for the grammar).
# Frames are GGPO frames: 0 = first frame GGPO is Ready, ~60/s.
!name example-r1-block-roll
!neutral                      # nobody's pad leaks in; only what is written here is pressed
@120 r1=1                     # a light attack on frame 120 (one-frame press)
@300-330 block=1              # hold block for 31 frames
@400+3 roll=1                 # roll input for 3 frames
# Movement fields are resolved world-space values (see header comment); the
# reliable way to author them is `harness_client.py rec2script` on a recording.
"""


def _cli_instance(args):
    return HarnessClient(args.port, args.host, timeout=args.timeout)


def _print_json(obj):
    print(json.dumps(obj, indent=2, sort_keys=True))


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0], formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--host", default=DEFAULT_HOST)
    ap.add_argument("--port", type=int, default=DEFAULT_PORT, help="control-plane port of the instance (default %d)" % DEFAULT_PORT)
    ap.add_argument("--timeout", type=float, default=15.0)
    sub = ap.add_subparsers(dest="cmd", required=True)

    sub.add_parser("status", help="print the instance status")
    sub.add_parser("ping", help="liveness check")
    p = sub.add_parser("cmd", help="send a raw command line"); p.add_argument("line", nargs="+")
    p = sub.add_parser("rollback", help="Rollback::rollbackEnabled (set before the other player joins)"); p.add_argument("state", choices=["on", "off", "toggle"])
    p = sub.add_parser("network", help="Rollback::networkTest"); p.add_argument("state", choices=["on", "off"])
    p = sub.add_parser("record", help="arm/disarm input recording; 'file PATH' sets the output file"); p.add_argument("what", nargs="+")
    p = sub.add_parser("replay", help="replay file PATH | replay off"); p.add_argument("what", nargs="+")
    p = sub.add_parser("script", help="load FILE (uploaded line by line) | load-path PATH (opened by the game) | clear | status | add DIRECTIVE")
    p.add_argument("what", nargs="+")
    sub.add_parser("end-session", help="Rollback::rollback_end_session()")
    p = sub.add_parser("hotkey", help="gsave|gload|isave|iload"); p.add_argument("name")
    p = sub.add_parser("log", help="write a HARNESS: marker into the mod log"); p.add_argument("text", nargs="+")
    p = sub.add_parser("hashes", help="confirmed per-frame state digests from the DLL's ring")
    p.add_argument("--since", type=int, default=-1); p.add_argument("--max", type=int, default=2000)
    p = sub.add_parser("dump", help="at FRAME|+N (arm) | status | get OUT (fetch the text to a file)")
    p.add_argument("what", nargs="+")
    sub.add_parser("probe", help="per-player hp/position/rotation right now")
    p = sub.add_parser("logs", help="stream the mod log to stdout (Ctrl-C to stop)")
    p.add_argument("--out", help="also append raw lines to this file")
    p.add_argument("--grep", help="only lines containing this text")
    p = sub.add_parser("discover", help="probe a port range for live instances")
    p.add_argument("--base", type=int, default=DEFAULT_PORT); p.add_argument("--count", type=int, default=DEFAULT_PORT_SPAN)
    p.add_argument("--pid", type=int)
    p = sub.add_parser("dump-recording", help="decode a rollback_recording.bin")
    p.add_argument("file"); p.add_argument("--frames", help="A-B window")
    p.add_argument("--fields", help="comma list of fields, or 'movement' / 'buttons' / 'player'")
    p.add_argument("--changes", action="store_true", help="only print fields that changed since the previous frame")
    p.add_argument("--json", action="store_true")
    p = sub.add_parser("rec2script", help="convert a recording (segment) into an input script")
    p.add_argument("file"); p.add_argument("out")
    p.add_argument("--frames", help="A-B window"); p.add_argument("--fields", help="comma list / movement / buttons / player")
    p.add_argument("--include-state", action="store_true", help="also emit state-derived fields (full override)")
    p.add_argument("--name")
    p = sub.add_parser("validate", help="check a script file against the grammar"); p.add_argument("file")
    sub.add_parser("example", help="print an example script")
    sub.add_parser("fields", help="list all field names and aliases")
    args = ap.parse_args(argv)

    def field_set(spec):
        if spec is None:
            return None
        if spec == "movement":
            return set(MOVEMENT_FIELDS)
        if spec == "buttons":
            return set(BUTTONS) | {"any_action_inputted"}
        if spec == "player":
            return set(PLAYER_FIELDS)
        out = set()
        for k in spec.split(","):
            k = k.strip()
            if k in GROUPS:
                out.update(GROUPS[k])
            elif k in ALIASES:
                out.add(ALIASES[k])
            elif k in FIELD_NAMES:
                out.add(k)
            else:
                ap.error("unknown field %r" % k)
        return out

    try:
        if args.cmd == "example":
            sys.stdout.write(EXAMPLE_SCRIPT)
            return 0
        if args.cmd == "fields":
            for n in FIELD_NAMES:
                print(n)
            print("# aliases:")
            for a, t in ALIASES.items():
                print("%s -> %s" % (a, t))
            print("# groups:")
            for g, t in GROUPS.items():
                print("%s -> %s..%s" % (g, t[0], t[-1]))
            return 0
        if args.cmd == "validate":
            with open(args.file) as fh:
                errs = validate_text(fh.read())
            for e in errs:
                print(e)
            print("OK" if not errs else "%d error(s)" % len(errs))
            return 0 if not errs else 1
        if args.cmd == "dump-recording":
            lo, hi = _parse_frames_arg(args.frames)
            fields = field_set(args.fields)
            recs = ((f, d) for f, d in read_recording(args.file) if (lo is None or f >= lo) and (hi is None or f <= hi))
            if args.changes:
                for frame, d in frame_changes(recs, fields):
                    if args.json:
                        print(json.dumps({"frame": frame, "changes": {k: v[1] for k, v in d.items()}}))
                    else:
                        print("@%d %s" % (frame, " ".join("%s=%s" % (k, _fmt_value(v[1])) for k, v in d.items())))
            else:
                for frame, d in recs:
                    if fields is not None:
                        d = {k: v for k, v in d.items() if k in fields}
                    if args.json:
                        print(json.dumps({"frame": frame, "fields": d}))
                    else:
                        print("@%d %s" % (frame, " ".join("%s=%s" % (k, _fmt_value(v)) for k, v in d.items())))
            return 0
        if args.cmd == "rec2script":
            lo, hi = _parse_frames_arg(args.frames)
            s = recording_to_script(read_recording(args.file), (lo, hi) if args.frames else None,
                                    field_set(args.fields), args.include_state, args.name)
            s.save(args.out)
            print("wrote %s (%d directives)" % (args.out, len(s)))
            return 0
        if args.cmd == "discover":
            found = discover(args.base, args.count, args.host, args.pid, timeout=min(args.timeout, 2.0))
            for port, st in found:
                print("%d pid=%s sandboxed=%s char_loaded=%s ggpo=%s/%s frame=%s" % (
                    port, st.get("pid"), st.get("sandboxed"), st.get("char_loaded"),
                    st.get("ggpo_started"), st.get("ggpo_ready"), st.get("frame")))
            return 0 if found else 1

        # everything below talks to one instance
        if args.cmd == "logs":
            stop = threading.Event()
            out = open(args.out, "a") if args.out else None

            def cb(entry):
                line = entry["log"]
                if args.grep and args.grep not in line:
                    return
                print("[%s] %s" % (entry.get("t"), line), flush=True)
                if out:
                    out.write("[%s] %s\n" % (entry.get("t"), line))
                    out.flush()
            stream = LogStream(args.port, args.host, cb, stop).start()
            try:
                while stream.thread.is_alive():
                    stream.join(0.5)
            except KeyboardInterrupt:
                stop.set()
            if stream.error:
                sys.stderr.write("log stream error: %s\n" % stream.error)
                return 1
            return 0

        with _cli_instance(args) as c:
            if args.cmd == "status":
                _print_json(c.status())
            elif args.cmd == "ping":
                _print_json(c.ping())
            elif args.cmd == "cmd":
                _print_json(c.cmd(" ".join(args.line), check=False))
            elif args.cmd == "rollback":
                _print_json(c.cmd("rollback %s" % args.state))
            elif args.cmd == "network":
                _print_json(c.network(args.state == "on"))
            elif args.cmd == "record":
                _print_json(c.cmd("record " + " ".join(args.what)))
            elif args.cmd == "replay":
                _print_json(c.cmd("replay " + " ".join(args.what)))
            elif args.cmd == "script":
                what = args.what[0]
                if what == "load":
                    with open(args.what[1]) as fh:
                        _print_json(c.script_upload(fh.read()))
                elif what == "load-path":
                    _print_json(c.script_load(" ".join(args.what[1:])))
                elif what == "clear":
                    _print_json(c.script_clear())
                elif what == "status":
                    _print_json(c.script_status())
                elif what == "add":
                    _print_json(c.script_add(" ".join(args.what[1:])))
                else:
                    ap.error("script: expected load FILE | load-path PATH | clear | status | add DIRECTIVE")
            elif args.cmd == "hashes":
                r = c.hashes(args.since, args.max)
                h = r["hashes"]
                print("frame=%s confirmed=%s ggpo_started=%s ring=%s..%s gap=%s" % (
                    r["frame"], r["confirmed_frame"], r.get("ggpo_started"), h["oldest"], h["newest"], h["gap"]))
                for e in h["frames"]:
                    print("STATEHASH frame=%d %s" % (e["f"], " ".join("%s=%s" % (k, e[k]) for k in e if k != "f")))
            elif args.cmd == "dump":
                what = args.what[0]
                if what == "at":
                    _print_json(c.dump_at(args.what[1]))
                elif what == "status":
                    _print_json(c.dump_status())
                elif what == "get":
                    r = c.dump_get(save_to=args.what[1] if len(args.what) > 1 else None)
                    text = r.pop("text")
                    r["text_bytes"] = len(text)
                    _print_json(r)
                    if len(args.what) <= 1:
                        sys.stdout.write(text)
                else:
                    ap.error("dump: expected at FRAME|+N | status | get [OUT]")
            elif args.cmd == "probe":
                _print_json(c.probe())
            elif args.cmd == "end-session":
                _print_json(c.end_session())
            elif args.cmd == "hotkey":
                _print_json(c.hotkey(args.name))
            elif args.cmd == "log":
                _print_json(c.log(" ".join(args.text)))
        return 0
    except (HarnessError, ScriptError, ValueError, OSError) as e:
        sys.stderr.write("error: %s\n" % e)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
