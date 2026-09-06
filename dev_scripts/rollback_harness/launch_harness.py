#!/usr/bin/env python3
"""
launch_harness.py -- scripted main-menu -> in-game navigation for the test harness.

The orchestrator's bring-up state machine needs each freshly launched instance
to get from the title screen into gameplay (the IN_GAME stage) before co-op
connect. We do it the dumb-but-robust way the harness asked for: tap the menu
*confirm* key (default 'e') over and over -- which walks "Press Any Button" ->
"Load Game" -> character slot -> confirm -- and watch the screen to know when the
character has actually loaded, so we stop tapping *before* 'e' starts firing
in-game interactions.

This lives entirely outside the DLL on purpose: no test-harness code ships in
d3d11.dll. It only needs a window handle (by PID, which the launcher already
knows, or by title), so it drives the native instance and the Sandboxie instance
identically -- call it once per window. With --launch it also *starts* the
instance first -- the Seamless Co-op launcher (ds1sc_launcher.exe) natively, or
the same launcher inside a Sandboxie box -- then waits for the game window and
navigates it. That makes one call the whole bring-up for one instance.

Why these mechanisms (DSR specifics):
  * Input  -- DSR reads the keyboard as a DirectInput device-state buffer (see
    OverhaulDLL InputUtil: the `uint8_t kb[256]` DIK array = physical key state).
    PostMessage(WM_KEYDOWN) never reaches DirectInput; only SendInput with
    KEYEVENTF_SCANCODE injects into the raw-input stream DirectInput reads. So we
    send hardware *scancodes* (E = 0x12), not virtual keys or window messages.
  * Capture -- GetDC(hwnd)/PrintWindow on a D3D11 swapchain typically returns
    black. We BitBlt the desktop at the window's client rect instead (after
    foregrounding the window to send the key), which is the actual composited
    pixels. => run the game **windowed / borderless**, not exclusive fullscreen.

Load detection (the stop condition) -- a single lightness threshold:
    On this setup the menus and the loading screen are dark and the in-game scene
    (the test arena) is bright. So we tap 'e' while the frame is dark (a menu /
    loading screen) and declare LOADED once mean luma stays at/above --light-thresh
    for --confirm-samples checks in a row. Brightness is tested *before* each tap,
    so reaching gameplay stops us before 'e' can fire in-game interactions.
    Once LOADED, we wait 5s for the character to settle, then tap F6 and 'r'.

--light-thresh is mean luma 0..255 and depends on resolution / UI scale / HDR, so
it needs a one-time calibration: set it above the brightest menu and below the
in-game scene. Use --debug to print per-sample luma and --save-captures DIR to
dump frames (.bmp, stdlib-only) off your actual game. Once the control plane
exists, `get_status` -> Game::playerchar_is_loaded() is a strictly more reliable
LOADED signal than pixels; this script is the no-DLL-dependency path until then.

Usage -- attach to an already-running game window:
    menu_nav.py --pid PID    [options]
    menu_nav.py --title STR  [--match-index N] [options]

Usage -- launch through the Seamless Co-op launcher, then navigate:
    menu_nav.py --launch                [options]   # native (instance A)
    menu_nav.py --launch --sandbox BOX  [options]   # 2nd Steam in Sandboxie (B)

Exit codes (same contract style as rollback_compare.py):
    0  LOADED   -- reached gameplay; stopped tapping
    1  TIMEOUT  -- no load within --timeout, or the game window never appeared
    2  ERROR    -- usage / no window / launcher or Start.exe missing / not Windows

No third-party dependencies (Windows only; Python 3.8+, stdlib + ctypes).
"""

import argparse
import ctypes
import os
import struct
import subprocess
import sys
import time
from ctypes import wintypes

# ---------------------------------------------------------------------------
# Win32 plumbing (ctypes). Handles are pointer-sized: every HWND/HDC/HGDIOBJ
# restype/argtype is declared so nothing gets truncated to 32 bits on x64.
# ---------------------------------------------------------------------------

if os.name != "nt":
    sys.stderr.write("error: menu_nav.py is Windows-only (needs SendInput + GDI).\n")
    raise SystemExit(2)

user32 = ctypes.WinDLL("user32", use_last_error=True)
gdi32 = ctypes.WinDLL("gdi32", use_last_error=True)
kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)

ULONG_PTR = ctypes.c_uint64 if ctypes.sizeof(ctypes.c_void_p) == 8 else ctypes.c_uint32

SW_RESTORE = 9
PROCESS_QUERY_LIMITED_INFORMATION = 0x1000
SRCCOPY = 0x00CC0020
BI_RGB = 0
DIB_RGB_COLORS = 0
INPUT_KEYBOARD = 1
KEYEVENTF_KEYUP = 0x0002
KEYEVENTF_SCANCODE = 0x0008

# DirectInput (DIK_*) scancodes == hardware scancodes for the keys we care about.
KEY_SCANCODES = {
    "e": 0x12, "enter": 0x1C, "return": 0x1C, "space": 0x39,
    "f": 0x21, "q": 0x10, "r": 0x13, "esc": 0x01, "f6": 0x40,
}

# Seconds to wait after the lightness check says LOADED before tapping 'r':
# the scene is bright a moment before the character actually accepts input.
POST_LOAD_DELAY = 5.0

GAME_EXE = "DarkSoulsRemastered.exe"
CONSOLE_WINDOW_CLASSES = {"ConsoleWindowClass", "CASCADIA_HOSTING_WINDOW_CLASS",
                          "PseudoConsoleWindow"}


class RECT(ctypes.Structure):
    _fields_ = [("left", ctypes.c_long), ("top", ctypes.c_long),
                ("right", ctypes.c_long), ("bottom", ctypes.c_long)]


class POINT(ctypes.Structure):
    _fields_ = [("x", ctypes.c_long), ("y", ctypes.c_long)]


class BITMAPINFOHEADER(ctypes.Structure):
    _fields_ = [("biSize", wintypes.DWORD), ("biWidth", ctypes.c_long),
                ("biHeight", ctypes.c_long), ("biPlanes", wintypes.WORD),
                ("biBitCount", wintypes.WORD), ("biCompression", wintypes.DWORD),
                ("biSizeImage", wintypes.DWORD), ("biXPelsPerMeter", ctypes.c_long),
                ("biYPelsPerMeter", ctypes.c_long), ("biClrUsed", wintypes.DWORD),
                ("biClrImportant", wintypes.DWORD)]


class KEYBDINPUT(ctypes.Structure):
    _fields_ = [("wVk", wintypes.WORD), ("wScan", wintypes.WORD),
                ("dwFlags", wintypes.DWORD), ("time", wintypes.DWORD),
                ("dwExtraInfo", ULONG_PTR)]


class MOUSEINPUT(ctypes.Structure):
    _fields_ = [("dx", ctypes.c_long), ("dy", ctypes.c_long),
                ("mouseData", wintypes.DWORD), ("dwFlags", wintypes.DWORD),
                ("time", wintypes.DWORD), ("dwExtraInfo", ULONG_PTR)]


class _INPUTunion(ctypes.Union):
    _fields_ = [("ki", KEYBDINPUT), ("mi", MOUSEINPUT)]


class INPUT(ctypes.Structure):
    _fields_ = [("type", wintypes.DWORD), ("u", _INPUTunion)]


WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

user32.GetForegroundWindow.restype = wintypes.HWND
user32.GetWindowThreadProcessId.argtypes = [wintypes.HWND, ctypes.POINTER(wintypes.DWORD)]
user32.GetWindowThreadProcessId.restype = wintypes.DWORD
user32.EnumWindows.argtypes = [WNDENUMPROC, wintypes.LPARAM]
user32.IsWindowVisible.argtypes = [wintypes.HWND]
user32.GetWindowTextW.argtypes = [wintypes.HWND, wintypes.LPWSTR, ctypes.c_int]
user32.GetWindowTextLengthW.argtypes = [wintypes.HWND]
user32.GetClassNameW.argtypes = [wintypes.HWND, wintypes.LPWSTR, ctypes.c_int]
user32.GetClientRect.argtypes = [wintypes.HWND, ctypes.POINTER(RECT)]
user32.ClientToScreen.argtypes = [wintypes.HWND, ctypes.POINTER(POINT)]
user32.ShowWindow.argtypes = [wintypes.HWND, ctypes.c_int]
user32.SetForegroundWindow.argtypes = [wintypes.HWND]
user32.BringWindowToTop.argtypes = [wintypes.HWND]
user32.AttachThreadInput.argtypes = [wintypes.DWORD, wintypes.DWORD, wintypes.BOOL]
user32.GetDC.argtypes = [wintypes.HWND]
user32.GetDC.restype = wintypes.HDC
user32.ReleaseDC.argtypes = [wintypes.HWND, wintypes.HDC]
user32.SendInput.argtypes = [wintypes.UINT, ctypes.POINTER(INPUT), ctypes.c_int]
user32.SendInput.restype = wintypes.UINT

kernel32.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
kernel32.OpenProcess.restype = wintypes.HANDLE
kernel32.QueryFullProcessImageNameW.argtypes = [
    wintypes.HANDLE, wintypes.DWORD, wintypes.LPWSTR, ctypes.POINTER(wintypes.DWORD)]
kernel32.CloseHandle.argtypes = [wintypes.HANDLE]

gdi32.CreateCompatibleDC.argtypes = [wintypes.HDC]
gdi32.CreateCompatibleDC.restype = wintypes.HDC
gdi32.CreateCompatibleBitmap.argtypes = [wintypes.HDC, ctypes.c_int, ctypes.c_int]
gdi32.CreateCompatibleBitmap.restype = wintypes.HBITMAP
gdi32.SelectObject.argtypes = [wintypes.HDC, wintypes.HGDIOBJ]
gdi32.SelectObject.restype = wintypes.HGDIOBJ
gdi32.BitBlt.argtypes = [wintypes.HDC, ctypes.c_int, ctypes.c_int, ctypes.c_int,
                         ctypes.c_int, wintypes.HDC, ctypes.c_int, ctypes.c_int,
                         wintypes.DWORD]
gdi32.GetDIBits.argtypes = [wintypes.HDC, wintypes.HBITMAP, wintypes.UINT,
                            wintypes.UINT, ctypes.c_void_p,
                            ctypes.POINTER(BITMAPINFOHEADER), wintypes.UINT]
gdi32.DeleteObject.argtypes = [wintypes.HGDIOBJ]
gdi32.DeleteDC.argtypes = [wintypes.HDC]


# ---------------------------------------------------------------------------
# Window selection
# ---------------------------------------------------------------------------

def _window_title(hwnd):
    n = user32.GetWindowTextLengthW(hwnd)
    if n <= 0:
        return ""
    buf = ctypes.create_unicode_buffer(n + 1)
    user32.GetWindowTextW(hwnd, buf, n + 1)
    return buf.value


def _window_class(hwnd):
    buf = ctypes.create_unicode_buffer(256)
    user32.GetClassNameW(hwnd, buf, 256)
    return buf.value


def _process_image(pid):
    """Basename of pid's exe, or "" if we can't read it (e.g. a Sandboxie'd process)."""
    h = kernel32.OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, False, pid)
    if not h:
        return ""
    try:
        size = wintypes.DWORD(260)
        buf = ctypes.create_unicode_buffer(size.value)
        if not kernel32.QueryFullProcessImageNameW(h, 0, buf, ctypes.byref(size)):
            return ""
        return os.path.basename(buf.value)
    finally:
        kernel32.CloseHandle(h)


def is_game_window(hwnd, pid):
    """True if hwnd plausibly belongs to the game and not to something else that
    merely matches the title.

    This matters because ds1sc_launcher.exe is a *console* program: launched
    detached (always so under Sandboxie's Start.exe) it gets a console window
    whose title is the launcher's full path -- which contains "DARK SOULS
    REMASTERED". Pick that by title and we'd foreground a console and type the
    confirm key into it forever while the game sat untouched behind it. Explorer
    windows on the game folder have the same problem.

    So: never a console window, and when we can read the owning process's image
    it has to be the game. Sandboxie can deny us the image name, hence the class
    check standing on its own as well.
    """
    if _window_class(hwnd) in CONSOLE_WINDOW_CLASSES:
        return False
    image = _process_image(pid)
    return image == "" or image.lower() == GAME_EXE.lower()


def enum_top_windows():
    """All visible top-level windows as (hwnd, pid, title)."""
    out = []

    @WNDENUMPROC
    def _cb(hwnd, _lparam):
        if user32.IsWindowVisible(hwnd):
            pid = wintypes.DWORD(0)
            user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
            out.append((hwnd, pid.value, _window_title(hwnd)))
        return True

    user32.EnumWindows(_cb, 0)
    return out


def find_window(pid=None, title=None, match_index=0):
    """Resolve a single HWND by PID (preferred) or title substring (case-insensitive).

    A game can briefly own >1 top-level window during startup (splash, etc.), so
    we prefer the one with a non-empty title and a sane client size, and let
    --match-index disambiguate if more than one still qualifies.
    """
    cands = []
    for hwnd, wpid, wtitle in enum_top_windows():
        if pid is not None and wpid != pid:
            continue
        if title is not None and title.lower() not in wtitle.lower():
            continue
        if pid is None and not is_game_window(hwnd, wpid):
            continue  # a console/Explorer window that merely matches the title
        cands.append((hwnd, wpid, wtitle))

    def _score(c):
        hwnd, _wpid, wtitle = c
        rect = RECT()
        ok = user32.GetClientRect(hwnd, ctypes.byref(rect))
        area = (rect.right - rect.left) * (rect.bottom - rect.top) if ok else 0
        return (1 if wtitle.strip() else 0, area)

    cands.sort(key=_score, reverse=True)
    if not cands:
        return None
    if match_index >= len(cands):
        return None
    return cands[match_index][0]


# ---------------------------------------------------------------------------
# Input
# ---------------------------------------------------------------------------

def _is_foreground(hwnd):
    """True if hwnd -- or another window of its process -- owns the foreground.

    SetForegroundWindow can report success without actually activating (the
    foreground lock), and an elevated game refuses injected input from a
    non-elevated script, so we check the result instead of trusting it."""
    fg = user32.GetForegroundWindow()
    if not fg:
        return False
    if fg == hwnd:
        return True
    a, b = wintypes.DWORD(0), wintypes.DWORD(0)
    user32.GetWindowThreadProcessId(fg, ctypes.byref(a))
    user32.GetWindowThreadProcessId(hwnd, ctypes.byref(b))
    return a.value == b.value and a.value != 0


def set_foreground(hwnd):
    """Bring `hwnd` to the foreground, defeating the foreground-lock via the
    AttachThreadInput dance so SendInput lands on the game and not on us."""
    user32.ShowWindow(hwnd, SW_RESTORE)
    fg = user32.GetForegroundWindow()
    if fg == hwnd:
        return True
    cur_tid = kernel32.GetCurrentThreadId()
    tgt_tid = user32.GetWindowThreadProcessId(hwnd, None)
    fg_tid = user32.GetWindowThreadProcessId(fg, None) if fg else 0
    if fg_tid:
        user32.AttachThreadInput(cur_tid, fg_tid, True)
    user32.AttachThreadInput(cur_tid, tgt_tid, True)
    user32.BringWindowToTop(hwnd)
    ok = user32.SetForegroundWindow(hwnd)
    user32.AttachThreadInput(cur_tid, tgt_tid, False)
    if fg_tid:
        user32.AttachThreadInput(cur_tid, fg_tid, False)
    return bool(ok)


def tap_scancode(scancode, hold=0.04):
    """Press and release a hardware scancode via SendInput (DirectInput-visible)."""
    down = INPUT(type=INPUT_KEYBOARD,
                 u=_INPUTunion(ki=KEYBDINPUT(0, scancode, KEYEVENTF_SCANCODE, 0, 0)))
    up = INPUT(type=INPUT_KEYBOARD,
               u=_INPUTunion(ki=KEYBDINPUT(0, scancode,
                                           KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP, 0, 0)))
    user32.SendInput(1, ctypes.byref(down), ctypes.sizeof(INPUT))
    time.sleep(hold)
    user32.SendInput(1, ctypes.byref(up), ctypes.sizeof(INPUT))


# ---------------------------------------------------------------------------
# Screen capture + luma metrics
# ---------------------------------------------------------------------------

class Capture:
    """A captured client-area frame plus a fixed-grid luma sample for cheap
    brightness / frame-difference metrics (compared positionally across frames)."""

    __slots__ = ("w", "h", "bgra", "samples")

    def __init__(self, w, h, bgra, max_samples):
        self.w = w
        self.h = h
        self.bgra = bgra
        self.samples = _luma_grid(bgra, w, h, max_samples)

    @property
    def mean_luma(self):
        return sum(self.samples) / len(self.samples) if self.samples else 0.0

    def diff(self, other):
        """Mean abs luma difference vs another capture on the same grid (or None)."""
        if other is None or len(other.samples) != len(self.samples) or not self.samples:
            return None
        total = sum(abs(a - b) for a, b in zip(self.samples, other.samples))
        return total / len(self.samples)


def _luma_grid(bgra, w, h, max_samples):
    """Subsample the frame to ~max_samples pixels and return their luma (0..255).
    A fixed stride means sample i is the same pixel every frame, so positional
    diffing is meaningful."""
    total = w * h
    if total == 0:
        return []
    step = max(1, total // max_samples)
    out = []
    for i in range(0, total, step):
        o = i * 4
        b = bgra[o]
        g = bgra[o + 1]
        r = bgra[o + 2]
        out.append((b * 29 + g * 150 + r * 77) >> 8)  # ~0.114/0.587/0.299
    return out


def capture_client(hwnd, max_samples):
    """BitBlt the desktop at hwnd's client rect. Returns a Capture or None.

    Reads from the screen DC (not the window DC) so it works for a D3D11
    swapchain; the window must be visible/foreground and not exclusive-fullscreen.
    """
    rect = RECT()
    if not user32.GetClientRect(hwnd, ctypes.byref(rect)):
        return None
    w = rect.right - rect.left
    h = rect.bottom - rect.top
    if w <= 0 or h <= 0:
        return None
    origin = POINT(rect.left, rect.top)
    user32.ClientToScreen(hwnd, ctypes.byref(origin))

    screen_dc = user32.GetDC(0)
    mem_dc = gdi32.CreateCompatibleDC(screen_dc)
    bmp = gdi32.CreateCompatibleBitmap(screen_dc, w, h)
    old = gdi32.SelectObject(mem_dc, bmp)
    try:
        if not gdi32.BitBlt(mem_dc, 0, 0, w, h, screen_dc, origin.x, origin.y, SRCCOPY):
            return None
        bih = BITMAPINFOHEADER()
        bih.biSize = ctypes.sizeof(BITMAPINFOHEADER)
        bih.biWidth = w
        bih.biHeight = -h            # top-down
        bih.biPlanes = 1
        bih.biBitCount = 32
        bih.biCompression = BI_RGB
        buf = (ctypes.c_char * (w * h * 4))()
        got = gdi32.GetDIBits(mem_dc, bmp, 0, h, buf, ctypes.byref(bih), DIB_RGB_COLORS)
        if got == 0:
            return None
        return Capture(w, h, bytes(buf), max_samples)
    finally:
        gdi32.SelectObject(mem_dc, old)
        gdi32.DeleteObject(bmp)
        gdi32.DeleteDC(mem_dc)
        user32.ReleaseDC(0, screen_dc)


def save_bmp(cap, path):
    """Write a 32bpp BGRA capture as a .bmp (stdlib-only) for threshold calibration."""
    w, h = cap.w, cap.h
    row = w * 4
    data_size = row * h
    fileheader = b"BM" + struct.pack("<IHHI", 14 + 40 + data_size, 0, 0, 14 + 40)
    infoheader = struct.pack("<IiiHHIIiiII", 40, w, -h, 1, 32, BI_RGB,
                             data_size, 0, 0, 0, 0)
    with open(path, "wb") as fh:
        fh.write(fileheader)
        fh.write(infoheader)
        fh.write(cap.bgra)


# ---------------------------------------------------------------------------
# The navigator
# ---------------------------------------------------------------------------

class Result:
    LOADED = "LOADED"
    TIMEOUT = "TIMEOUT"
    ERROR = "ERROR"


def advance_to_ingame(hwnd, scancode=0x12, press_interval=0.7, poll_interval=0.2,
                      timeout=180.0, light_thresh=60.0, confirm_samples=10,
                      max_pixel_samples=4000, debug=False, save_dir=None, log=print):
    """Tap the confirm key through the menus until the screen is bright == in-game.

    On this setup the menus and the loading screen are dark while the in-game scene
    (the test arena) is bright, so a single lightness threshold separates them: we
    tap 'e' while the frame is dark (a menu / loading screen) and declare LOADED
    once mean luma stays at/above light_thresh for confirm_samples checks in a row.
    We test brightness *before* each tap, so reaching gameplay stops us before 'e'
    fires an in-game interaction.

    light_thresh is mean luma 0..255 and needs a one-time calibration
    (debug=True / save_dir=...): set it above the brightest menu and below the
    in-game scene.

    Once LOADED is confirmed we wait POST_LOAD_DELAY seconds for the character to
    finish settling in, then tap F6 and 'r' once each. Returns
    Result.LOADED / TIMEOUT.
    """
    deadline = time.time() + timeout
    bright_run = 0
    not_fg = 0
    presses = 0
    frame_no = 0
    luma = None

    while time.time() < deadline:
        set_foreground(hwnd)
        if not _is_foreground(hwnd):
            # SendInput goes to whatever *is* focused, so tapping now would type
            # the confirm key into another window. Wait for the game instead.
            not_fg += 1
            if not_fg in (5, 25) or debug:
                log("menu_nav: game window is not foreground -- not tapping "
                    "(another window has focus, or the game is elevated and this "
                    "script is not: run it as admin)")
            time.sleep(poll_interval)
            continue
        not_fg = 0

        cap = capture_client(hwnd, max_pixel_samples)
        if cap is None:
            if debug:
                log("menu_nav: capture failed (window gone/minimized?)")
            continue
        luma = cap.mean_luma
        if save_dir is not None:
            save_bmp(cap, os.path.join(save_dir, f"cap_{frame_no:04d}.bmp"))
        if debug:
            log(f"menu_nav: f={frame_no} luma={luma:5.1f} "
                f"bright_run={bright_run} presses={presses}")
        frame_no += 1

        if luma >= light_thresh:
            bright_run += 1
            if bright_run >= confirm_samples:
                log(f"menu_nav: LOADED (luma={luma:.1f} >= {light_thresh:.0f} for "
                    f"{confirm_samples} checks, {presses} presses)")
                time.sleep(POST_LOAD_DELAY)
                set_foreground(hwnd)
                tap_scancode(KEY_SCANCODES["f6"])
                time.sleep(press_interval)
                tap_scancode(KEY_SCANCODES["r"])
                log("menu_nav: pressed F6 then 'r'")
                return Result.LOADED
            time.sleep(poll_interval)  # confirming in-game -- do NOT tap
            continue

        # Dark screen (menu / loading): advance with a confirm press.
        bright_run = 0
        tap_scancode(scancode)
        presses += 1
        time.sleep(press_interval)

    log(f"menu_nav: TIMEOUT after {timeout:.0f}s (presses={presses}, "
        f"last_luma={'n/a' if luma is None else f'{luma:.1f}'})")
    return Result.TIMEOUT


# ---------------------------------------------------------------------------
# Launch + bring-up
# ---------------------------------------------------------------------------
# The harness launches the game through the Seamless Co-op launcher
# (ds1sc_launcher.exe): natively for instance A, and inside a Sandboxie box (the
# 2nd Steam) for instance B. The launcher spawns DarkSoulsRemastered.exe and goes
# away, so its PID isn't the game's. Instead we snapshot the existing "DARK SOULS"
# windows, launch, and grab whichever matching top-level window is *new* -- which
# also disambiguates the instances: B's window is the one that wasn't open before
# we launched it (A's already is).

DEFAULT_LAUNCHER = (
    r"C:\Program Files (x86)\Steam\steamapps\common"
    r"\DARK SOULS REMASTERED\ds1sc_launcher.exe"
)
DEFAULT_GAME_TITLE = "DARK SOULS"
SANDBOXIE_START_CANDIDATES = [
    r"C:\Program Files\Sandboxie-Plus\Start.exe",
    r"C:\Program Files\Sandboxie\Start.exe",
    r"C:\Program Files (x86)\Sandboxie-Plus\Start.exe",
    r"C:\Program Files (x86)\Sandboxie\Start.exe",
]


def find_sandboxie_start(explicit=None):
    if explicit:
        return explicit if os.path.isfile(explicit) else None
    for p in SANDBOXIE_START_CANDIDATES:
        if os.path.isfile(p):
            return p
    return None


def _window_client_area(hwnd):
    rect = RECT()
    if not user32.GetClientRect(hwnd, ctypes.byref(rect)):
        return 0
    return (rect.right - rect.left) * (rect.bottom - rect.top)


def game_windows(title):
    """Visible game windows whose title contains `title`, largest first.

    Filtered with is_game_window(), so the launcher's console window -- whose
    title is its own path, i.e. contains "DARK SOULS REMASTERED" -- is not
    mistaken for the game."""
    cands = [hwnd for hwnd, pid, wtitle in enum_top_windows()
             if title.lower() in wtitle.lower() and is_game_window(hwnd, pid)]
    cands.sort(key=_window_client_area, reverse=True)
    return cands


def launch_instance(launcher, sandbox=None, sandboxie_start=None, log=print, env_extra=None):
    """Start the SC launcher natively, or inside a Sandboxie box via Start.exe.

    `env_extra` is merged into the child's environment. The launcher and the
    game inherit it (CreateProcess default), which is how DSR_HARNESS_PORT
    reaches the DLL's control plane. Under Sandboxie the environment goes
    through Start.exe; if it turns out not to survive that hop, the DLL's
    fallback is the ini key HarnessControlPort plus port fall-forward, and the
    orchestrator can still find the instance with `harness_client.py discover
    --pid` using the pid logged below."""
    launcher = os.path.abspath(launcher)
    if not os.path.isfile(launcher):
        raise FileNotFoundError(f"launcher not found: {launcher}")
    workdir = os.path.dirname(launcher)
    env = os.environ.copy()
    env.update(env_extra or {})
    if sandbox:
        start = find_sandboxie_start(sandboxie_start)
        if not start:
            raise FileNotFoundError(
                "Sandboxie Start.exe not found; pass --sandboxie-start PATH")
        cmd = [start, f"/box:{sandbox}", launcher]
        log(f"menu_nav: launching [box:{sandbox}] {launcher}")
    else:
        cmd = [launcher]
        log(f"menu_nav: launching {launcher}")
    if env_extra:
        log("menu_nav: env " + " ".join(f"{k}={v}" for k, v in env_extra.items()))
    subprocess.Popen(cmd, cwd=workdir, close_fds=True, env=env)


def window_pid(hwnd):
    pid = wintypes.DWORD(0)
    user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
    return pid.value


def wait_for_new_window(title, exclude, timeout, poll=1.0):
    """Poll until a `title` window not in `exclude` appears; return it or None."""
    exclude = set(exclude)
    deadline = time.time() + timeout
    while time.time() < deadline:
        for hwnd in game_windows(title):
            if hwnd not in exclude and _window_client_area(hwnd) > 0:
                return hwnd
        time.sleep(poll)
    return None


def launch_and_navigate(launcher, sandbox, sandboxie_start, window_title,
                        launch_timeout, nav_kwargs, log=print, control_port=None):
    """Launch one instance (native or Sandboxie), wait for its window, navigate.

    With `control_port`, the DLL's test-harness control plane is enabled on that
    port (env DSR_HARNESS_PORT; the DLL falls forward up to +9 if it is taken).
    Emits one machine-readable line `INSTANCE hwnd=... pid=... control_port=...`
    once the window is up so an orchestrator can pair the process with its port
    (harness_client.py discover --pid PID)."""
    before = game_windows(window_title)  # already-open windows (e.g. the other instance)
    env_extra = {"DSR_HARNESS_PORT": str(control_port)} if control_port else None
    try:
        launch_instance(launcher, sandbox, sandboxie_start, log=log, env_extra=env_extra)
    except (FileNotFoundError, OSError) as e:
        log(f"menu_nav: launch failed: {e}")
        return Result.ERROR
    log(f"menu_nav: waiting up to {launch_timeout:.0f}s for the game window "
        f"(title ~= {window_title!r})")
    hwnd = wait_for_new_window(window_title, before, launch_timeout)
    if not hwnd:
        log("menu_nav: launch TIMEOUT -- game window never appeared (launcher "
            "needs a click? Steam not running in this box? wrong --sandbox?)")
        return Result.TIMEOUT
    log(f"menu_nav: game window appeared (hwnd={hwnd:#x}, pid={window_pid(hwnd)}, "
        f"title={_window_title(hwnd)!r}, class={_window_class(hwnd)}); "
        f"navigating menus")
    log(f"INSTANCE hwnd={hwnd:#x} pid={window_pid(hwnd)} "
        f"control_port={control_port if control_port else 0} sandbox={sandbox or ''}")
    return advance_to_ingame(hwnd, log=log, **nav_kwargs)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _resolve_scancode(key, scancode):
    if scancode is not None:
        return int(scancode, 0)
    k = key.lower()
    if k in KEY_SCANCODES:
        return KEY_SCANCODES[k]
    if len(k) == 1 and "a" <= k <= "z":
        # rough fallback; prefer the explicit table or --scancode for odd keys
        return KEY_SCANCODES.get(k, ord(k.upper()))
    raise ValueError(f"unknown key '{key}'; pass --scancode 0xNN")


def main(argv=None):
    ap = argparse.ArgumentParser(
        description="Tap the confirm key through DSR's menus until the screen "
                    "brightens to the in-game scene (a lightness threshold). "
                    "One window per call.")
    launch = ap.add_argument_group("launch (start the game, then navigate)")
    launch.add_argument("--launch", action="store_true",
                        help="launch via the Seamless Co-op launcher, then navigate")
    launch.add_argument("--launcher", default=DEFAULT_LAUNCHER,
                        help="path to ds1sc_launcher.exe (default: the Steam install path)")
    launch.add_argument("--sandbox", metavar="BOX",
                        help="Sandboxie box name; if set, launch through Sandboxie "
                             "(omit for a native launch)")
    launch.add_argument("--sandboxie-start", metavar="PATH",
                        help="path to Sandboxie Start.exe (auto-detected if omitted)")
    launch.add_argument("--launch-timeout", type=float, default=120.0,
                        help="seconds to wait for the game window to appear (default 120)")
    launch.add_argument("--window-title", default=DEFAULT_GAME_TITLE,
                        help="game window title substring (default 'DARK SOULS')")
    launch.add_argument("--control-port", type=int, metavar="PORT",
                        help="enable the DLL's test-harness control plane on this "
                             "localhost port (sets DSR_HARNESS_PORT for the game; "
                             "see harness_client.py). Use different ports for A and B.")

    attach = ap.add_argument_group("attach (when not launching)")
    attach.add_argument("--pid", type=int, help="target game process id")
    attach.add_argument("--title", help="match a window whose title contains this")
    ap.add_argument("--match-index", type=int, default=0,
                    help="if several windows match, pick the Nth (default 0)")

    ap.add_argument("--key", default="e", help="confirm key (default 'e')")
    ap.add_argument("--scancode", help="explicit hardware scancode, e.g. 0x12 (overrides --key)")
    ap.add_argument("--press-interval", type=float, default=0.7,
                    help="seconds between key taps while in menus (default 0.7)")
    ap.add_argument("--timeout", type=float, default=180.0,
                    help="overall seconds before giving up (default 180)")

    ap.add_argument("--light-thresh", type=float, default=60.0,
                    help="mean luma 0..255 at/above which the screen is in-game, "
                         "not a dark menu/loading screen (default 60; calibrate)")
    ap.add_argument("--confirm-samples", type=int, default=10,
                    help="consecutive bright samples to declare LOADED (default 10)")
    ap.add_argument("--poll-interval", type=float, default=0.2,
                    help="capture interval while confirming the in-game screen (default 0.2)")

    ap.add_argument("--debug", action="store_true", help="print per-sample luma")
    ap.add_argument("--save-captures", metavar="DIR",
                    help="dump every capture as DIR/cap_NNNN.bmp for calibration")
    args = ap.parse_args(argv)

    try:
        scancode = _resolve_scancode(args.key, args.scancode)
    except ValueError as e:
        ap.error(str(e))

    if not args.launch and args.pid is None and args.title is None:
        ap.error("specify --launch (to start the game) or --pid/--title "
                 "(to attach to an already-running window)")

    if args.save_captures:
        os.makedirs(args.save_captures, exist_ok=True)

    def emit(m):
        print(m, flush=True)

    nav_kwargs = dict(
        scancode=scancode, press_interval=args.press_interval,
        poll_interval=args.poll_interval, timeout=args.timeout,
        light_thresh=args.light_thresh, confirm_samples=args.confirm_samples,
        debug=args.debug, save_dir=args.save_captures)

    if args.launch:
        res = launch_and_navigate(
            args.launcher, args.sandbox, args.sandboxie_start, args.window_title,
            args.launch_timeout, nav_kwargs, log=emit, control_port=args.control_port)
    else:
        hwnd = find_window(pid=args.pid, title=args.title, match_index=args.match_index)
        if not hwnd:
            which = f"pid={args.pid}" if args.pid else f"title~={args.title!r}"
            sys.stderr.write(f"error: no visible top-level window for {which}. "
                             f"Is the game launched and windowed?\n")
            return 2
        res = advance_to_ingame(hwnd, log=emit, **nav_kwargs)

    return {Result.LOADED: 0, Result.TIMEOUT: 1}.get(res, 2)


if __name__ == "__main__":
    raise SystemExit(main())
