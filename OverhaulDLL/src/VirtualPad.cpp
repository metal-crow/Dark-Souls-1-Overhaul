#include <set>
#include <cmath>

#include "VirtualPad.h"
#include "DarkSoulsOverhaulMod.h"
#include "GameData.h"

/*
 * Layout, from the Ghidra structs:
 *
 *   PadDevice                +0x08  VirtualMultiDevice*
 *   VirtualMultiDevice       +0x00  DLUserInputDeviceImpl base
 *   DLUserInputDeviceImpl    +0x00  DLUserInputDevice base (0x88 bytes)
 *                            +0x88  VirtualMultiDevice_GetInputI*
 *                            +0x90  VirtualMultiDevice_GetStickInputI*
 *                            +0x158 VirtualInputData
 *   DLUserInputDevice        +0x80  suppression flag (forces a handled read to false)
 *   VirtualInputData         +0x30  DynamicBitset keys
 *   DynamicBitset            +0x08  length in bytes
 *                            +0x10  uint32_t* bits, indexed by PadDeviceInputs id
 */
static const size_t OFF_PADDEVICE_VMD         = 0x08;
static const size_t OFF_VMD_GETINPUTI         = 0x88;
static const size_t OFF_VMD_GETSTICKINPUTI    = 0x90;
static const size_t OFF_VMD_SUPPRESS          = 0x80;
static const size_t OFF_VMD_VIRTUALINPUTDATA  = 0x158;
static const size_t OFF_VID_KEYS              = 0x30;
static const size_t OFF_BITSET_LEN            = 0x08;
static const size_t OFF_BITSET_BITS           = 0x10;
//VirtualInputData +0x08 is VirtualAnalogKeyInfo<float>, which is a float vector:
//+0x10 start, +0x18 end, +0x20 capacity. Indexed by PadDeviceInputs id, same as the bitset.
static const size_t OFF_VID_ANALOG            = 0x08;
static const size_t OFF_ANALOG_START          = 0x10;
static const size_t OFF_ANALOG_END            = 0x18;

//Both accessors are called WITHOUT a device argument -- the device is implicit, which is
//why the stubs have to be told which player is being stepped by the caller rather than
//working it out themselves.
typedef bool  (*GetInputI_FUNC)(uint32_t inputI, bool* handled);
typedef float (*GetStickInputI_FUNC)(uint32_t inputI, bool* handled);

#if ROLLBACK_INPUT_TESTING
VirtualPadState VirtualPad::last_capture = {};
bool VirtualPad::last_capture_valid = false;
VirtualPadState VirtualPad::latched = {};
uint32_t VirtualPad::latched_frames = 0;
#endif
#if VIRTUALPAD_SELFTEST
uint32_t VirtualPad::selftest_frames_remaining = 0;
uint32_t VirtualPad::selftest_frames_done = 0;
#endif

static uint8_t* g_vmd = NULL;
static GetInputI_FUNC g_orig_GetInputI = NULL;
static GetStickInputI_FUNC g_orig_GetStickInputI = NULL;

static const VirtualPadState* g_replay_state = NULL;

#if ROLLBACK_INPUT_TESTING
//diagnostics
static std::set<uint32_t> g_unknown_button_ids;
static std::set<uint32_t> g_unknown_axis_ids;
static uint32_t g_button_fellback_to_raw = 0;
static uint32_t g_axis_unhandled = 0;
static uint32_t g_axis_unreadable = 0;
//frames where the device-level suppression flag was set, which forces every handled read
//to false. If an input "did not register", this says whether the game was muting it.
static uint32_t g_suppressed_frames = 0;
//how often the stubs were actually reached -- zero would mean the swap never took effect,
//which looks identical to "everything matched" if you only watch the unknown-id lists
static uint32_t g_stub_button_queries = 0;
static uint32_t g_stub_axis_queries = 0;
#endif
#if VIRTUALPAD_SELFTEST
//the self-test replays this; it must outlive the region, so it is file-static
static VirtualPadState g_selftest_state = {};
static bool g_selftest_owns_replay = false;
#endif
//device suppression flag, saved across a replay region (see begin_replay)
static uint8_t g_suppress_saved = 0;

static uint8_t* resolve_vmd()
{
    void* padDevice = PadMan_GetPadDevice(0);
    if (padDevice == NULL)
    {
        return NULL;
    }
    return *(uint8_t**)((uint8_t*)padDevice + OFF_PADDEVICE_VMD);
}

static int button_slot(uint32_t id)
{
    for (size_t i = 0; i < VIRTUALPAD_BUTTON_COUNT; i++)
    {
        if (VIRTUALPAD_BUTTON_IDS[i] == id) return (int)i;
    }
    return -1;
}

static int axis_slot(uint32_t id)
{
    for (size_t i = 0; i < VIRTUALPAD_AXIS_COUNT; i++)
    {
        if (VIRTUALPAD_AXIS_IDS[i] == id) return (int)i;
    }
    return -1;
}

//The raw fallback the accessors use when GetInputI reports "not handled": a bit array
//indexed directly by the PadDeviceInputs id.
static bool raw_button(uint32_t id)
{
    if (g_vmd == NULL) return false;
    uint8_t* keys = g_vmd + OFF_VMD_VIRTUALINPUTDATA + OFF_VID_KEYS;
    uint64_t len_bytes = *(uint64_t*)(keys + OFF_BITSET_LEN);
    uint32_t* bits = *(uint32_t**)(keys + OFF_BITSET_BITS);
    if (bits == NULL) return false;
    if ((uint64_t)(id / 8) >= len_bytes) return false;
    return ((bits[id >> 5] >> (id & 31)) & 1) != 0;
}

//The raw fallback the stick accessor uses when GetStickInputI reports "not handled" --
//which measurement shows is the NORMAL path for the movement/camera axes, unlike buttons.
static bool raw_axis(uint32_t id, float* out)
{
    if (g_vmd == NULL) return false;
    uint8_t* info = g_vmd + OFF_VMD_VIRTUALINPUTDATA + OFF_VID_ANALOG;
    float* start = *(float**)(info + OFF_ANALOG_START);
    float* end = *(float**)(info + OFF_ANALOG_END);
    if (start == NULL || end <= start) return false;
    if ((size_t)id >= (size_t)(end - start)) return false;
    *out = start[id];
    return true;
}

/* ---- the stubs -------------------------------------------------------- */

static bool vp_GetInputI(uint32_t inputI, bool* handled)
{
    const VirtualPadState* s = g_replay_state;
    if (s == NULL)
    {
        *handled = false;
        return false;
    }
    int slot = button_slot(inputI);
    if (slot < 0)
    {
#if VIRTUALPAD_SELFTEST
        //the self-test replays whole frames, menus included, specifically to collect these
        if (g_selftest_owns_replay)
        {
            g_unknown_button_ids.insert(inputI);
            *handled = false;
            return false;
        }
#endif
        //Answering "not handled" would make the game read this machine's device for this id,
        //silently mixing local input into the replayed pad.
        FATALERROR("VirtualPad: button id 0x%X was queried during a replay but is not in VIRTUALPAD_BUTTON_IDS", inputI);
    }
#if ROLLBACK_INPUT_TESTING
    g_stub_button_queries++;
#endif
    *handled = true;
    return ((s->buttons >> slot) & 1) != 0;
}

static float vp_GetStickInputI(uint32_t inputI, bool* handled)
{
    const VirtualPadState* s = g_replay_state;
    if (s == NULL)
    {
        *handled = false;
        return 0.0f;
    }
    int slot = axis_slot(inputI);
    if (slot < 0)
    {
#if VIRTUALPAD_SELFTEST
        if (g_selftest_owns_replay)
        {
            g_unknown_axis_ids.insert(inputI);
            *handled = false;
            return 0.0f;
        }
#endif
        FATALERROR("VirtualPad: axis id 0x%X was queried during a replay but is not in VIRTUALPAD_AXIS_IDS", inputI);
    }
#if ROLLBACK_INPUT_TESTING
    g_stub_axis_queries++;
#endif
    *handled = true;
    return s->axes[slot];
}

/* ---- api -------------------------------------------------------------- */

bool VirtualPad::install()
{
    uint8_t* vmd = resolve_vmd();
    if (vmd == NULL)
    {
        return false;
    }

    //Re-resolve every time: PadMan can hand out a different device across a map load, and
    //caching a stale one would mean capture() reading, and end_replay() writing, freed memory.
    if (vmd == g_vmd && g_orig_GetInputI != NULL)
    {
        return true;
    }
    if (vmd != g_vmd && g_replay_state != NULL)
    {
        //the device changed under an active replay; the old pointers are not ours to put back
        g_replay_state = NULL;
    }

    GetInputI_FUNC gi = *(GetInputI_FUNC*)(vmd + OFF_VMD_GETINPUTI);
    GetStickInputI_FUNC gs = *(GetStickInputI_FUNC*)(vmd + OFF_VMD_GETSTICKINPUTI);
    //don't half-commit: a partially populated device would leave us unable to restore
    if (gi == NULL || gs == NULL || gi == &vp_GetInputI || gs == &vp_GetStickInputI)
    {
        return false;
    }

    g_vmd = vmd;
    g_orig_GetInputI = gi;
    g_orig_GetStickInputI = gs;

    ConsoleWrite("VirtualPad: device %p, GetInputI %p, GetStickInputI %p", vmd, gi, gs);
    return true;
}

bool VirtualPad::capture(VirtualPadState* out)
{
    if (!install() || g_replay_state != NULL)
    {
        return false;
    }

    memset(out, 0, sizeof(*out));

    //Mirror what DbgMapWalkPad_GetInputI0xNN does: ask the binding manager first, honour
    //the suppression flag when it answered, and only fall back to the raw device bits.
    const bool suppressed = (*(uint8_t*)(g_vmd + OFF_VMD_SUPPRESS)) != 0;
#if ROLLBACK_INPUT_TESTING
    if (suppressed) g_suppressed_frames++;
#endif
    for (size_t i = 0; i < VIRTUALPAD_BUTTON_COUNT; i++)
    {
        bool handled = false;
        bool v = g_orig_GetInputI(VIRTUALPAD_BUTTON_IDS[i], &handled);
        if (handled)
        {
            if (suppressed) v = false;
        }
        else
        {
            v = raw_button(VIRTUALPAD_BUTTON_IDS[i]);
#if ROLLBACK_INPUT_TESTING
            g_button_fellback_to_raw++;
#endif
        }
        if (v) out->buttons |= (1u << i);
    }

    //Axes come back pre-deadzone; PadDevice_Get2StickInputI applies the deadzone and
    //normalisation itself, so this must NOT be the post-deadzone value.
    for (size_t i = 0; i < VIRTUALPAD_AXIS_COUNT; i++)
    {
        bool handled = false;
        float v = g_orig_GetStickInputI(VIRTUALPAD_AXIS_IDS[i], &handled);
        if (!handled)
        {
            //The normal path for these axes: GetStickInputI only claims an axis when all four
            //of its resolved binding slots are bound.
#if ROLLBACK_INPUT_TESTING
            g_axis_unhandled++;
#endif
            if (!raw_axis(VIRTUALPAD_AXIS_IDS[i], &v))
            {
#if ROLLBACK_INPUT_TESTING
                g_axis_unreadable++;
#endif
                v = 0.0f;
            }
        }
        out->axes[i] = v;
    }

#if ROLLBACK_INPUT_TESTING
    last_capture = *out;
    last_capture_valid = true;
    latched.buttons |= out->buttons;
    for (size_t i = 0; i < VIRTUALPAD_AXIS_COUNT; i++)
    {
        if (fabsf(out->axes[i]) > fabsf(latched.axes[i])) latched.axes[i] = out->axes[i];
    }
    latched_frames++;
#endif
    return true;
}

void VirtualPad::begin_replay(const VirtualPadState* state)
{
    if (!install() || state == NULL)
    {
        return;
    }
    if (g_replay_state != NULL)
    {
        //nesting would have the inner end_replay put the originals back mid-region, leaving
        //the outer one reading the real device without noticing
        ConsoleWrite("VirtualPad: begin_replay while already replaying; ignoring");
        return;
    }
    g_replay_state = state;
    *(GetInputI_FUNC*)(g_vmd + OFF_VMD_GETINPUTI) = &vp_GetInputI;
    *(GetStickInputI_FUNC*)(g_vmd + OFF_VMD_GETSTICKINPUTI) = &vp_GetStickInputI;

    //The accessor applies the device suppression flag AFTER our stub returns:
    //  if (handled) { if (field_0x80) value = false; }
    //so a local menu or cutscene would silently zero everything we replay -- including the
    //REMOTE player's inputs, which have nothing to do with this machine's menu state. The
    //flag was already honoured when the state was captured, so clearing it for the duration
    //of the replay is what makes the captured value authoritative.
    g_suppress_saved = *(uint8_t*)(g_vmd + OFF_VMD_SUPPRESS);
    *(uint8_t*)(g_vmd + OFF_VMD_SUPPRESS) = 0;
}

void VirtualPad::end_replay()
{
    if (g_vmd == NULL || g_orig_GetInputI == NULL)
    {
        return;
    }
    *(GetInputI_FUNC*)(g_vmd + OFF_VMD_GETINPUTI) = g_orig_GetInputI;
    *(GetStickInputI_FUNC*)(g_vmd + OFF_VMD_GETSTICKINPUTI) = g_orig_GetStickInputI;
    if (g_replay_state != NULL)
    {
        *(uint8_t*)(g_vmd + OFF_VMD_SUPPRESS) = g_suppress_saved;
    }
    g_replay_state = NULL;
}

bool VirtualPad::replaying()
{
    return g_replay_state != NULL;
}

#if ROLLBACK_INPUT_TESTING
void VirtualPad::reset_diagnostics()
{
    g_unknown_button_ids.clear();
    g_unknown_axis_ids.clear();
    g_button_fellback_to_raw = 0;
    g_axis_unhandled = 0;
    g_axis_unreadable = 0;
    g_suppressed_frames = 0;
    g_stub_button_queries = 0;
    g_stub_axis_queries = 0;
    memset(&latched, 0, sizeof(latched));
    latched_frames = 0;
}

static std::string id_list_json(const std::set<uint32_t>& ids)
{
    std::string s = "[";
    bool first = true;
    for (uint32_t id : ids)
    {
        if (!first) s += ",";
        char b[8];
        snprintf(b, sizeof(b), "%u", id);
        s += b;
        first = false;
    }
    return s + "]";
}

std::string VirtualPad::state_json(const VirtualPadState* s)
{
    std::string out = "{\"buttons\":[";
    bool first = true;
    for (size_t i = 0; i < VIRTUALPAD_BUTTON_COUNT; i++)
    {
        if (((s->buttons >> i) & 1) == 0) continue;
        if (!first) out += ",";
        char b[8];
        snprintf(b, sizeof(b), "%u", VIRTUALPAD_BUTTON_IDS[i]);
        out += b;
        first = false;
    }
    out += "],\"mask\":" + std::to_string(s->buttons) + ",\"axes\":{";
    for (size_t i = 0; i < VIRTUALPAD_AXIS_COUNT; i++)
    {
        char b[64];
        snprintf(b, sizeof(b), "%s\"%u\":%.4f", i ? "," : "", VIRTUALPAD_AXIS_IDS[i], s->axes[i]);
        out += b;
    }
    return out + "}}";
}

std::string VirtualPad::status_json()
{
    std::string s = "\"installed\":" + std::string(g_orig_GetInputI != NULL ? "true" : "false");
    s += ",\"replaying\":" + std::string(g_replay_state != NULL ? "true" : "false");
    s += ",\"capture_valid\":" + std::string(last_capture_valid ? "true" : "false");
    s += ",\"unknown_button_ids\":" + id_list_json(g_unknown_button_ids);
    s += ",\"unknown_axis_ids\":" + id_list_json(g_unknown_axis_ids);
    s += ",\"button_fellback_to_raw\":" + std::to_string(g_button_fellback_to_raw);
    s += ",\"axis_unhandled\":" + std::to_string(g_axis_unhandled);
    s += ",\"axis_unreadable\":" + std::to_string(g_axis_unreadable);
    s += ",\"suppressed_frames\":" + std::to_string(g_suppressed_frames);
    s += ",\"stub_button_queries\":" + std::to_string(g_stub_button_queries);
    s += ",\"stub_axis_queries\":" + std::to_string(g_stub_axis_queries);
#if VIRTUALPAD_SELFTEST
    s += ",\"selftest_remaining\":" + std::to_string(selftest_frames_remaining);
    s += ",\"selftest_done\":" + std::to_string(selftest_frames_done);
#endif
    s += ",\"latched_frames\":" + std::to_string(latched_frames);
    s += ",\"latched\":" + state_json(&latched);
    if (last_capture_valid)
    {
        s += ",\"capture\":" + state_json(&last_capture);
    }
    return s;
}
#endif

#if VIRTUALPAD_SELFTEST
/* ---- passthrough self-test ------------------------------------------- */

void VirtualPad::selftest_begin(uint32_t frames)
{
    reset_diagnostics();
    selftest_frames_done = 0;
    selftest_frames_remaining = frames;
    ConsoleWrite("VirtualPad: selftest armed for %u frames", frames);
}

void VirtualPad::selftest_frame_start()
{
    if (selftest_frames_remaining == 0 || g_replay_state != NULL)
    {
        return;
    }
    if (!capture(&g_selftest_state))
    {
        //not in a state where the device can be read; don't burn a frame of the budget
        return;
    }
    begin_replay(&g_selftest_state);
    g_selftest_owns_replay = true;
}

void VirtualPad::selftest_frame_end()
{
    if (!g_selftest_owns_replay)
    {
        return;
    }
    end_replay();
    g_selftest_owns_replay = false;
    selftest_frames_done++;
    if (selftest_frames_remaining > 0 && --selftest_frames_remaining == 0)
    {
        ConsoleWrite("VirtualPad: selftest finished, %u frames, %u button / %u axis stub queries",
            selftest_frames_done, g_stub_button_queries, g_stub_axis_queries);
    }
}
#endif
