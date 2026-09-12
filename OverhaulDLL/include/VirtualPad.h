#pragma once
#ifndef VIRTUALPAD_H
#define VIRTUALPAD_H

#include <stdint.h>
#include <stddef.h>
#include <string>

//Build the input test scaffolding: the GGPO-free input loopback ("network" harness
//command), the "pad" diagnostics and the per-player input counters ("inputdiag").
#ifndef ROLLBACK_INPUT_TESTING
#define ROLLBACK_INPUT_TESTING 0
#endif

//Build the passthrough self-test ("pad selftest"). Its only trigger is that harness command
//and it reports through the diagnostics, so it needs ROLLBACK_INPUT_TESTING as well.
#ifndef VIRTUALPAD_SELFTEST
#define VIRTUALPAD_SELFTEST 0
#endif
#if VIRTUALPAD_SELFTEST && !ROLLBACK_INPUT_TESTING
#error VIRTUALPAD_SELFTEST requires ROLLBACK_INPUT_TESTING
#endif

//PadDeviceInputs ids the gameplay input path queries.
//When replaying during rollback, we detect if the game queries an id that isn't in this list and trigger an error
static const uint32_t VIRTUALPAD_BUTTON_IDS[] = {
    0x36, // R3(on click)      -- lock on
    0x3B, // R1_weapon
    0x3C, // R2_weapon
    0x3D, // L1_weapon
    0x3E, // L1_2Handed
    0x3F, // L2_2Handed
    0x41, // R1
    0x42, // R2
    0x43, // L1
    0x44, // L2
    0x45, // (bow precision)
    0x4A, // A
    0x4C, // Y_alt
    0x4D, // A(on click)
    0x4E, // Start(on click)
    0x4F, // Select(on click)
    0x50, // Y(on click)
    0x51, // RightDpad(on click)
    0x52, // LeftDpad(on click)
    0x53, // DownDpad(on click)
    0x54, // UpDpad(on click)
    0x5A, // X
    0x5B, // B
    0x5C, // B(on click)
    0x2E, // (unnamed in the PadDeviceInputs enum)
    0x55, // R1_alt
    0x6E, // (unnamed)
    0x70, // (unnamed)
    //The gate on item use: DbgMapWalkPad_GetInputI0x5A_X_ does NOT read X directly.
    // It first requires DAT_141c6ab00 == 1 OR GetInputI(0x57), and only then reads 0x5A
    0x57, // (unnamed; sits next to R1_alt 0x55 / L1_alt 0x56 -- the use-button alternate)
};
static const size_t VIRTUALPAD_BUTTON_COUNT = sizeof(VIRTUALPAD_BUTTON_IDS) / sizeof(VIRTUALPAD_BUTTON_IDS[0]);
static_assert(VIRTUALPAD_BUTTON_COUNT <= 32, "button set must fit the uint32 mask");

static const uint32_t VIRTUALPAD_AXIS_IDS[] = {
    0x10, // LStickY
    0x11, // LStickX
    0x12, // RStickX
    0x13, // RStickY
    0x18, // R2_analog
    0x19, // L2_analog
    //unnamed second analog quad sitting directly below LStickY(0x10).
    0x0C, 0x0D, 0x0E, 0x0F,
};
static const size_t VIRTUALPAD_AXIS_COUNT = sizeof(VIRTUALPAD_AXIS_IDS) / sizeof(VIRTUALPAD_AXIS_IDS[0]);

#pragma pack(push, 1)
//What one player's pad looked like on one frame.
struct VirtualPadState
{
    uint32_t buttons;                     // bit i <-> VIRTUALPAD_BUTTON_IDS[i]
    float    axes[VIRTUALPAD_AXIS_COUNT]; // raw, pre-deadzone: the accessors apply the
                                          // deadzone themselves, so capturing post-deadzone
                                          // values and feeding them back would apply it twice
};
#pragma pack(pop)

namespace VirtualPad
{
    //Resolve the device and remember the original accessors. Safe to call repeatedly;
    //returns false until PadMan has produced a device (i.e. before the game is up).
    bool install();

    //Read the local player's pad through the ORIGINAL accessors. Must not be called
    //while a replay is active, or it would read back our own injected values.
    bool capture(VirtualPadState* out);

    //Given the input we captured, pass it into the game.
    //This uses our injections to answer every input query from `state`, until end_replay().
    //`state` must outlive the region.
    void begin_replay(const VirtualPadState* state);
    void end_replay();
    bool replaying();

#if VIRTUALPAD_SELFTEST
    /*
     * Passthrough self-test. For `frames` frames: capture the real pad at the top of the
     * frame, then install the stubs feeding that same state back for the rest of it, so the
     * WHOLE frame -- Step_PadManipulator, MoveMapStep, menus -- reads through the replay
     * path instead of the device.
     *
     * It answers the two things capture alone cannot:
     *   - completeness: any id the game asks for that we do not carry lands in
     *     unknown_button_ids / unknown_axis_ids instead of raising the FATALERROR.
     *   - that replay works at all, without needing a second machine.
     *
     * The capture reads the device as polled by the PREVIOUS frame's Step_PadMan (MainLoop
     * runs at MainUpdate's entry, before it), so the character plays one frame behind. That
     * is a consistent delay, never a mix of two frames, and is harmless for both checks.
 *
 * It only sees ids the game queries during the run. An id read behind a condition that is
 * already satisfied is never asked for -- 0x57 sits behind the DAT_141c6ab00 latch -- so an
 * empty unknown list is not proof the id set is complete.
     */
    void selftest_begin(uint32_t frames);
    void selftest_frame_start();   // MainLoop tick, at MainUpdate entry
    void selftest_frame_end();     // MainUpdate end
    extern uint32_t selftest_frames_remaining;
    extern uint32_t selftest_frames_done;
#endif

#if ROLLBACK_INPUT_TESTING
    //Harness diagnostics: unknown ids the self-test collected, how often a captured read fell
    //through to the raw device, and how often the stubs were reached.
    std::string status_json();
    std::string state_json(const VirtualPadState* s);
    void reset_diagnostics();

    extern VirtualPadState last_capture;
    extern bool last_capture_valid;

    //Sticky OR of every capture's buttons and peak |value| per axis since the last reset.
    //Capture runs at 60 Hz and the harness polls far slower, so an instantaneous read would
    //miss the "(on click)" ids, which are true for a single frame.
    extern VirtualPadState latched;
    extern uint32_t latched_frames;
#endif
}

#endif // VIRTUALPAD_H
