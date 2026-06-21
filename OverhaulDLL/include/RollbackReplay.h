#pragma once
#ifndef ROLLBACK_REPLAY_H
#define ROLLBACK_REPLAY_H

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>

#include "Rollback.h"               // RollbackInput
#include "DarkSoulsOverhaulMod.h"   // ConsoleWrite

/*
 * rollback test harness feature: deterministic record / replay of the
 * local player's GGPO input stream.
 *
 * Each player saves input once per frame, right after PackRollbackInput in rollback_game_frame_start_helper.
 * Record a round, then replay instance A's stream on A and B's stream on B.
 *
 * Inputs are keyed by the GGPO framecount (number of ggpo_advance_frame calls) --
 * which the StateHash/Serialize also keys on -- so a desync report at frame N maps
 * to recorded input at frame N.
 * Since framecount is by ggpo (input needs to be synced to frame), the
 * stream recorded on one instance replays frame-for-frame on the other.
 *
 * Activation:
 *   - If "rollback_replay.bin" exists at session start  -> REPLAY (load it,
 *     override the local input from frame 0).
 *   - Else if record is armed (F12) or the marker file "rollback_record.on"
 *     exists                                            -> RECORD to
 *     "rollback_recording.bin".
 *   - Else                                              -> OFF (normal play).
 *
 * A recording always begins at the first frame GGPO is Ready (framecount ~0) and
 * runs to session end, so it is meaningful only when replayed from the same start
 * state (same save / arena / loadout) -- same constraint as StateHash/Serialize.
 * The format is crash-robust: frame records written + flushed as they happen
 * so a session that ends in a desync-crash still leaves a usable file.
 */
namespace RollbackReplay
{
    enum class Mode { Off, Record, Replay };

    inline const char* REPLAY_FILE   = "rollback_replay.bin";
    inline const char* RECORD_FILE   = "rollback_recording.bin";
    inline const char* RECORD_MARKER = "rollback_record.on";

    // 8-byte magic + the input size this file was written with, so a replay
    // captured by a different build (different RollbackInput layout) is rejected
    // instead of silently feeding garbage into GGPO.
    inline constexpr char REPLAY_MAGIC[8] = { 'D','S','R','I','N','P','0','1' };
    struct ReplayHeader
    {
        char     magic[8];
        uint32_t input_size;
        uint32_t reserved;
    };

    inline Mode  mode = Mode::Off;
    inline bool  record_armed = false;      // set by hotkey before the session
    inline FILE* record_fp = nullptr;
    inline bool  _exhausted_logged = false;

    inline std::map<int, RollbackInput>& _replay_store()
    {
        static std::map<int, RollbackInput> m;   // GGPO frame -> recorded local input
        return m;
    }

    inline bool _file_exists(const char* path)
    {
        FILE* f = nullptr;
        fopen_s(&f, path, "rb");
        if (f) { fclose(f); return true; }
        return false;
    }

    // Decide record vs replay vs off, once, when the GGPO session starts. Safe to
    // call before the first frame; does no per-frame work.
    inline void init_session()
    {
        mode = Mode::Off;
        record_fp = nullptr;
        _replay_store().clear();
        _exhausted_logged = false;

        FILE* rf = nullptr;
        fopen_s(&rf, REPLAY_FILE, "rb");
        if (rf)
        {
            ReplayHeader hdr = {};
            size_t got = fread(&hdr, 1, sizeof(hdr), rf);
            if (got != sizeof(hdr) || memcmp(hdr.magic, REPLAY_MAGIC, sizeof(hdr.magic)) != 0)
            {
                ConsoleWrite("RollbackReplay: '%s' is not a valid replay file, ignoring", REPLAY_FILE);
                fclose(rf);
                return;
            }
            if (hdr.input_size != sizeof(RollbackInput))
            {
                ConsoleWrite("RollbackReplay: replay input_size %u != current %u (build mismatch), ignoring",
                             (unsigned)hdr.input_size, (unsigned)sizeof(RollbackInput));
                fclose(rf);
                return;
            }
            int min_frame = 0, max_frame = 0, count = 0;
            for (;;)
            {
                int32_t frame = 0;
                if (fread(&frame, 1, sizeof(frame), rf) != sizeof(frame)) break;
                RollbackInput in{};
                if (fread(&in, 1, sizeof(in), rf) != sizeof(in)) break;   // truncated tail record
                if (count == 0) min_frame = frame;
                max_frame = frame;
                _replay_store()[frame] = in;
                count++;
            }
            fclose(rf);
            mode = Mode::Replay;
            ConsoleWrite("RollbackReplay: REPLAY %d frames [%d..%d] from '%s'",
                         count, min_frame, max_frame, REPLAY_FILE);
            return;
        }

        if (record_armed || _file_exists(RECORD_MARKER))
        {
            fopen_s(&record_fp, RECORD_FILE, "wb");
            if (!record_fp)
            {
                ConsoleWrite("RollbackReplay: failed to open '%s' for recording", RECORD_FILE);
                return;
            }
            ReplayHeader hdr = {};
            memcpy(hdr.magic, REPLAY_MAGIC, sizeof(hdr.magic));
            hdr.input_size = (uint32_t)sizeof(RollbackInput);
            hdr.reserved = 0;
            fwrite(&hdr, 1, sizeof(hdr), record_fp);
            fflush(record_fp);
            mode = Mode::Record;
            ConsoleWrite("RollbackReplay: RECORD -> '%s'", RECORD_FILE);
            return;
        }

        ConsoleWrite("RollbackReplay: OFF (no '%s', record not armed)", REPLAY_FILE);
    }

    // Called once per frame at the ggpo_add_local_input seam with the GGPO
    // framecount this input is for. In Replay mode it overwrites *io with the
    // recorded input; in Record mode it captures *io. Off mode is a no-op.
    inline void on_local_input(int frame, RollbackInput* io)
    {
        switch (mode)
        {
        case Mode::Record:
            if (record_fp)
            {
                int32_t f = frame;
                fwrite(&f, 1, sizeof(f), record_fp);
                fwrite(io, 1, sizeof(*io), record_fp);
                fflush(record_fp);          // write-through so a crash keeps the recording
            }
            break;

        case Mode::Replay:
        {
            std::map<int, RollbackInput>& m = _replay_store();
            auto it = m.find(frame);
            if (it != m.end())
            {
                *io = it->second;
            }
            else if (!_exhausted_logged)
            {
                // Past the end of (or a gap in) the recording: leave the live
                // input in place and note where the deterministic window ended.
                ConsoleWrite("RollbackReplay: no recorded input for frame %d (replay exhausted / gap); live input from here", frame);
                _exhausted_logged = true;
            }
            break;
        }

        case Mode::Off:
        default:
            break;
        }
    }

    inline void end_session()
    {
        if (record_fp)
        {
            fclose(record_fp);
            record_fp = nullptr;
            ConsoleWrite("RollbackReplay: recording closed");
        }
        _replay_store().clear();
        mode = Mode::Off;
        _exhausted_logged = false;
    }
}

#endif // ROLLBACK_REPLAY_H
