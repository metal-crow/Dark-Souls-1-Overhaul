# Rollback test harness

Tooling for testing the GGPO rollback netcode of the Dark Souls Overhaul mod
without a human on each controller. Two game instances run on one box (A native,
B in a Sandboxie box with its own Steam), connect through Seamless Co-op, and an
orchestrator drives both over a localhost control plane, feeds each one a
deterministic input script, and compares the per-frame state hashes both sides
emit.

Everything in this directory is stdlib-only Python 3.8+. The DLL side lives in
`OverhaulDLL/` (see "DLL components").

## Status

| Layer | State |
|---|---|
| Determinism oracle (`STATEHASH` digests, `StateHash.h` / `StateSerializer.h`; `hashes` / `dump_at` over the control plane) | in the DLL, **not yet exercised in-game** |
| Comparator (`rollback_compare.py`: log files, or `--live` over the control plane with dump-and-diff on desync) | done, tested against fake instances |
| Launch + menu navigation (`launch_harness.py`) | done, in-game verified |
| Input record/replay (`RollbackReplay.h`) | done |
| Scripted input (`RollbackScript.h`) | done, **not yet exercised in-game** |
| Control plane (`HarnessControl.cpp`) + client (`harness_client.py`) | done, **not yet exercised in-game** |
| Network impairment shim, orchestrator/MCP | not started |

## DLL components

* `include/RollbackReplay.h` — records the local player's `RollbackInput` per GGPO
  frame to `rollback_recording.bin`, or replays one. Activation: replay file
  present at session start wins; else F12 / `record arm` / `rollback_record.on`.
* `include/RollbackScript.h` — the frame-indexed input **script** laid over the
  live input (format below).
* `src/HarnessControl.cpp`, `include/HarnessControl.h` — localhost TCP control
  plane; replaces the F6–F12 hotkeys and streams the mod log.
* The seam, in `Rollback.cpp::rollback_game_frame_start_helper`, right before
  `ggpo_add_local_input`:

      replay (base) -> script (overlay) -> record (captures the final input)

  so a scripted session's recording replays faithfully, and a script can patch a
  replayed recording. Everything is keyed by the GGPO frame count from
  `ggpo_get_frame_info`, the same index the `STATEHASH` lines use, so a desync at
  frame N points at script/recording frame N.

## Enabling the control plane

It is **off by default** (no open port for ordinary players). Per instance:

* env var `DSR_HARNESS_PORT=<port>` for the game process —
  `launch_harness.py --launch --control-port 42800` sets it; or
* `d3d11_mod.ini`: `[Dark Souls Overhaul Testing]` / `HarnessControlPort=<port>`.

The DLL binds `127.0.0.1:<port>`; if the port is taken (the other instance) it
falls forward up to `+9` and logs `HARNESS: control plane listening on
127.0.0.1:<port> (pid <pid>)`. `launch_harness.py` prints
`INSTANCE hwnd=... pid=... control_port=...` when the window appears, and
`harness_client.py discover --pid <pid>` finds which port that process bound.

Under Sandboxie the env var has to survive `Start.exe`; if it doesn't, use the
ini key (both instances read the same ini unless the box has its own copy) and
rely on fall-forward + `discover`.

## Protocol

One command per line, one JSON object per reply (`{"ok":true,...}` or
`{"ok":false,"error":"..."}`). Commands run on the **game thread** (a MainLoop
callback drains a queue), so they are ordered with frames; the socket side waits
up to 10 s. `ping` is answered immediately. A connection that sends `subscribe`
becomes a log stream (`{"log":"...","t":<ms>}` per `ConsoleWrite`) and accepts
no further commands — open a second connection for those.

| Command | Effect |
|---|---|
| `ping` | liveness |
| `status` | pid, sandboxed, seamless_coop, char_loaded, rollback_enabled, ggpo_started, ggpo_ready, frame, confirmed_frame, connected_players, steam_id, replay/record state, script status |
| `frame` | `{"frame":N,"confirmed_frame":M}` |
| `input` | the last local `RollbackInput` sent to GGPO, as named fields |
| `rollback on\|off\|toggle` | `Rollback::rollbackEnabled` (F6) — set **before** the other player joins |
| `network on\|off` | `Rollback::networkTest` (F7) |
| `record arm\|disarm`, `record file <path>` | input recording (F12) / output file for the next session |
| `replay file <path>`, `replay off` | recording replayed at the next session start |
| `script load <path>` | replace the script with a file the game can open |
| `script add <directive>` | append one script line (this is how `harness_client.py` uploads) |
| `script clear` / `script status` / `script neutral on\|off` / `script name <text>` | |
| `hotkey gsave\|gload\|isave\|iload` | the F8–F11 save/restore tests |
| `end_session` | `Rollback::rollback_end_session()` |
| `log <text>` | writes `HARNESS: <text>` to the mod log (marker) |
| `subscribe` | turn this connection into a log stream |
| `hashes [since] [max]` | confirmed per-frame state digests (frame > since, oldest first) from the DLL's ~2.3 min ring; `gap:true` if the ring no longer reaches `since+1` |
| `dump_at <frame>\|+<n>` | capture the full canonical state text when GGPO saves that frame (re-saves during a rollback overwrite it until the frame is confirmed) |
| `dump_status` / `dump_get` | is it captured and confirmed / fetch the text (hundreds of KB) |
| `probe` | per connected player: `hp`, `max_hp`, `x`, `y`, `z`, `rot`, plus the current frame |
| `help` | list |

`harness_client.py` wraps all of this (`HarnessClient`), plus `wait_for(...)`
helpers (`wait_char_loaded`, `wait_players`, `wait_ggpo_ready`, `wait_frame`) and
`subscribe(callback)` for the log stream.

## Input scripts

A script is a list of **field overrides keyed by GGPO frame**. On each frame the
DLL takes the input the game produced (the "live baseline": the neutral pad plus
state-derived values such as the character's facing, camera angles, lock-on
handle and equipment) and writes the script's fields over it. Fields a frame
doesn't name pass through unchanged.

```
# comment
!name walk-then-r1            # label shown in status
!neutral                      # zero the player-driven fields on EVERY frame first
@120 r1=1                     # one frame
@200-260 block=1              # inclusive range
@300+5 roll=1                 # 5 frames from 300 (300..304)
@* not_getting_movement_input=1   # every frame
@400 pc_rotation=1.5707 camera_x_direction_movement_input_amount=0.8
```

* Field names are the `RollbackInput` / `PadManipulatorPacked` struct members
  (`harness_client.py fields` lists them; arrays as `name[i]`, or the bare array
  name for "all elements"), or aliases: `r1 l1 r2 l2 r1_magic l1_magic roll
  backstep use parry block jump lefthand_attack l1_weapon sprint two_hand lockon
  move_x move_y move_x_alt move_y_alt rotation turn no_move any_action`,
  `stick_x`/`stick_y` (= all 6 `x_/y_movement_input` copies).
* Values: decimal/hex integers, floats, `true/false`. Later directives win.
* Setting any action button recomputes `any_action_inputted` the way
  `Step_PadManipulator` does (OR of the buttons) unless set explicitly.
* `!neutral` makes the run independent of whoever is touching the keyboard: all
  buttons, movement amounts, stick history, turn deltas and timers are forced to
  their idle values every frame; state-derived fields still pass through.
* Frames restart at 0 every GGPO session (= every `end_session` + reconnect), and
  a loaded script persists, so the same script re-runs on every reset. Frame 0 is
  the first frame GGPO is `Ready`; the players stand at their spawn points until
  then if nothing is pressed.
* Both players see the same frame numbers, so a two-player scenario is one script
  per instance with frames that mean the same thing on each side.
* Auto-load: `rollback_script.txt` next to the exe is loaded at session start if
  nothing was loaded through the control plane.

Python builder:

```python
from harness_client import HarnessClient, InputScript
s = InputScript(name="r1 into block", neutral=True)
s.press("r1", at=120)            # one-frame press
s.hold("block", 200, 260)        # inclusive
s.span(300, 5, roll=1)           # 5 frames
with HarnessClient(42800) as a:
    a.script_upload(s)           # clear + "script add" per line; no shared filesystem needed
```

### Movement — read this before scripting it

`PadManipulatorPacked` carries *resolved* movement, not stick positions
(`Compute_PadManipulator_forPCUnlocked` in the game): `camera_x/y_direction_
movement_input_amount` is a **world-space vector along the character's current
facing, scaled by stick magnitude**; the `_alt` pair is the camera-relative stick
direction; `delta_pc_rotation_perframe` is the per-frame turn toward the stick;
`pc_rotation` is the current facing and the unpack writes it straight into the
Havok character as the authoritative rotation. Hand-writing those is possible
(the fields are all settable) but the axis conventions are unverified, so the
reliable route is empirical:

1. `record arm`, play the movement by hand, `end_session` (writes
   `rollback_recording.bin`).
2. `harness_client.py dump-recording rollback_recording.bin --fields movement
   --changes` to see exactly which fields carry it.
3. `harness_client.py rec2script rollback_recording.bin walk.txt --frames 300-420
   --fields player` to turn that window into an editable script (emitted under
   `!neutral`, only non-idle fields, so it is exact for the player-driven set),
   then splice/retime it with the builder.

## Recording format

`rollback_recording.bin`: header `"DSRINP01"` + `uint32 input_size` (231 for the
current struct) + `uint32 reserved`, then `{int32 frame, RollbackInput}` records
to EOF, flushed per frame (a crash leaves a usable file; a truncated tail record
is ignored). `harness_client.LAYOUT` mirrors the struct; `RollbackScript.h`
static_asserts the sizes/offsets so a layout change fails the DLL build instead
of silently desynchronising the decoder.

## Verifying that every frame is identical

`RollbackHash::record` hashes every subsystem of the `RollbackState` GGPO saves
each frame; once a frame is *confirmed* (no more rollback possible) its digests
are final and must be bit-identical on both instances. `emit_confirmed` writes
them to `dsoverhaul_statehash_logging.txt` next to the exe **and** into a ring the
control plane serves as `hashes`. Frames are the shared GGPO frame numbers, so
the two sides align directly.

```
# live, while both instances run (stops at the first divergence):
python rollback_compare.py --live 42800,42810 --frames 3600 --dump-on-desync --out-dir runs/001
# after the fact, from the two log files (the Sandboxie'd one is under the box folder):
python rollback_compare.py "C:\...\DARK SOULS REMASTERED\dsoverhaul_statehash_logging.txt" ^
                           "C:\Sandbox\<user>\<box>\drive\C\...\dsoverhaul_statehash_logging.txt"
```

Exit 0 = PASS (every overlapping confirmed frame identical), 1 = DESYNC (first
differing frame + which subsystems: player / bullet / damage / havok / throw /
dmghit), 2 = error. `--json` for the orchestrator. With `--dump-on-desync` the
live mode immediately asks both instances for a full canonical state dump at the
same future frame, saves `statedump_<label>_<frame>.txt` for each plus a unified
diff, and prints the head of the diff, which names the exact field. That is the
three tiers: hash (frame + subsystem), dump diff (field), then re-simulate the
same deterministic script to classify.

`harness_client.py --port P hashes`, `dump at +30`, `dump get FILE`, and `probe`
(hp/position/rotation per player) expose the same data by hand.

## A two-instance run, end to end

```
python launch_harness.py --launch                --control-port 42800   # A (native)
python launch_harness.py --launch --sandbox DSR2 --control-port 42810   # B (Sandboxie)
python harness_client.py --port 42800 rollback on ; python harness_client.py --port 42810 rollback on
# connect the two through Seamless Co-op (password matchmaking) -> GGPO starts
python harness_client.py --port 42800 script load scenario_A.txt
python harness_client.py --port 42810 script load scenario_B.txt
python rollback_compare.py --live 42800,42810 --frames 3600 --dump-on-desync --json > result.json
python harness_client.py --port 42800 end-session   # frames restart at 0 on the next connect
```

`rollback on` must be set before the peer joins (the session is created in the
lobby callback). `script load` can be issued at any time; directives for frames
already past are simply skipped. The mod log itself (`subscribe` /
`harness_client.py logs`) carries the GGPO events and `HARNESS:` markers, not
the STATEHASH lines.

## Known gaps

* **`Rollback::start()` is commented out** in `DarkSoulsOverhaulMod.cpp`
  (`on_process_attach_async`) and has been since 2022. Without it the rollback
  hooks, the GGPO frame callback and the input seam are never installed, so
  `rollback on` has no effect. Re-enable it (or gate it on the harness being
  enabled) before a rollback test run.
* Nothing in this iteration has been run against the live game yet: the control
  plane, script overlay, `hashes`/`dump_at`, and `--control-port` env
  pass-through (especially through Sandboxie's `Start.exe`) need one in-game
  check. Note the STATEHASH writer previously opened the wrong file name (the
  main log's, already held deny-write), which left a NULL stream; fixed, so this
  is the first build where `dsoverhaul_statehash_logging.txt` can appear.
* Movement field conventions (signs/axes) are inferred from decompilation; use
  the record → `rec2script` path until a scripted `move` has been calibrated.
