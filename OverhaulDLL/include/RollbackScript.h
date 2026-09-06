#pragma once
#ifndef ROLLBACK_SCRIPT_H
#define ROLLBACK_SCRIPT_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <climits>
#include <string>
#include <vector>
#include <type_traits>

#include "Rollback.h"               // RollbackInput / PadManipulatorPacked
#include "DarkSoulsOverhaulMod.h"   // ConsoleWrite

/*
 * rollback test harness feature: scripted, deterministic local input.
 *
 * A script is a frame-indexed list of field overrides that is laid OVER the
 * RollbackInput the game produced for the frame (the "live baseline"), right
 * before it is handed to ggpo_add_local_input. Frames are GGPO frames -- the
 * same index RollbackReplay records with and STATEHASH lines are keyed by --
 * so "@120 r1=1" means "on GGPO frame 120, the local player has R1 pressed".
 *
 * Only the fields a directive names are touched; everything else on that frame
 * passes through from the live baseline (which, with nobody touching the pad, is
 * the neutral controller plus state-derived values like pc_rotation, the camera
 * angles, lock-on handle and equipment). "!neutral" additionally zeroes the
 * player-driven fields on every frame so a stray keypress can't leak in.
 *
 * Text format (one directive per line):
 *
 *     # comment
 *     !name walk-then-r1            pragma: label shown in status
 *     !neutral                      pragma: zero player-driven fields every frame
 *     @120 r1=1                     single frame
 *     @200-260 block=1              inclusive frame range
 *     @300+5 roll=1                 5 frames starting at 300 (300..304)
 *     @* not_getting_movement_input=1   every frame
 *     @400 pc_rotation=1.5707 camera_x_direction_movement_input_amount=0.8
 *
 * A field is any RollbackInput / PadManipulatorPacked member by its struct name
 * (array members as name[i]), or one of the short aliases below. Values are
 * decimal/hex integers, floats, or true/false. Later directives win on a frame.
 * Setting any action button also recomputes any_action_inputted the way
 * Step_PadManipulator does (OR of the action bits) unless the directive sets it
 * explicitly.
 *
 * Ordering at the seam (Rollback.cpp): replay (base) -> script (overlay) ->
 * record (captures the final input), so a scripted session's recording replays
 * faithfully and a script can patch a replayed recording.
 *
 * Activation: HarnessControl "script ..." commands, or drop "rollback_script.txt"
 * next to the exe (auto-loaded at GGPO session start if nothing is loaded).
 * A loaded script persists across sessions; frames restart at 0 each session,
 * so the same script re-applies on every "reset". All mutation happens on the
 * game thread (the control plane marshals commands there).
 *
 * Movement caveat (from Step_PadManipulator / Compute_PadManipulator_forPC*):
 * camera_x/y_direction_movement_input_amount is a WORLD-SPACE vector along the
 * character's current facing scaled by stick magnitude, *_alt is the
 * camera-relative stick direction, delta_pc_rotation_perframe is the turn
 * toward the stick, and pc_rotation is the current facing (which the unpack
 * writes back to havokChara as the authoritative rotation). Authoring movement
 * by hand therefore means writing those resolved values; the practical route is
 * to record a human doing the movement and lift/convert the frames
 * (harness_client.py rec2script / dump-recording).
 */

// Wire layout, documented for the Python side (harness_client.py decodes
// rollback_recording.bin with these offsets). If a static_assert fires here the
// Python FIELD table must be updated to match.
static_assert(sizeof(PadManipulatorPacked) == 137, "PadManipulatorPacked layout changed; update harness_client.py");
static_assert(offsetof(RollbackInput, padmanipulator) == 5, "RollbackInput layout changed; update harness_client.py");
static_assert(offsetof(RollbackInput, curSelectedMagicSlot) == 142, "RollbackInput layout changed; update harness_client.py");
static_assert(offsetof(RollbackInput, equipment_array) == 151, "RollbackInput layout changed; update harness_client.py");
static_assert(sizeof(RollbackInput) == 231, "RollbackInput layout changed; update harness_client.py");

namespace RollbackScript
{
    inline const char* SCRIPT_FILE = "rollback_script.txt";

    template<class T> inline T num_from(double v)
    {
        if constexpr (std::is_floating_point_v<T>) return (T)v;
        else return (T)(long long)v;   // via signed so -1 -> all-ones for unsigned targets
    }

    struct FieldDef
    {
        const char* name;
        void (*set)(RollbackInput*, double);
        double (*get)(const RollbackInput*);
        bool button;       // an action-button bit: participates in any_action_inputted
        bool neutral_zero; // zeroed by !neutral (player-driven, not state-derived)
        double neutral_value;
    };

#define RS_NUM(NAME, EXPR, NEUT, NEUTV) { NAME, \
        [](RollbackInput* i, double v) { i->EXPR = num_from<std::remove_reference_t<decltype(i->EXPR)>>(v); }, \
        [](const RollbackInput* i) -> double { return (double)(i->EXPR); }, false, NEUT, NEUTV }
#define RS_BIT(NAME, EXPR, BUTTON, NEUT, NEUTV) { NAME, \
        [](RollbackInput* i, double v) { i->EXPR = (v != 0.0) ? 1u : 0u; }, \
        [](const RollbackInput* i) -> double { return (double)(i->EXPR); }, BUTTON, NEUT, NEUTV }
#define RS_BITS2(NAME, EXPR, NEUT) { NAME, \
        [](RollbackInput* i, double v) { i->EXPR = ((unsigned)(long long)v) & 3u; }, \
        [](const RollbackInput* i) -> double { return (double)(i->EXPR); }, false, NEUT, 0.0 }

    // Every settable field, by struct member name. Order here = order in input_json().
    inline const FieldDef FIELDS[] = {
        // ---- RollbackInput top level ----
        RS_NUM("const1",                     const1,                     false, 0),
        RS_BIT("bTargetLocked",              bTargetLocked,              false, false, 0),
        RS_BIT("bTargetLocked_Alt",          bTargetLocked_Alt,          false, false, 0),
        RS_NUM("curSelectedMagicSlot",       curSelectedMagicSlot,       false, 0),
        RS_NUM("curUsingInventoryItemId",    curUsingInventoryItemId,    false, 0),
        RS_NUM("curSelectedQuickbarItemId",  curSelectedQuickbarItemId,  false, 0),
        RS_NUM("equipment_array[0]",  equipment_array[0],  false, 0), RS_NUM("equipment_array[1]",  equipment_array[1],  false, 0),
        RS_NUM("equipment_array[2]",  equipment_array[2],  false, 0), RS_NUM("equipment_array[3]",  equipment_array[3],  false, 0),
        RS_NUM("equipment_array[4]",  equipment_array[4],  false, 0), RS_NUM("equipment_array[5]",  equipment_array[5],  false, 0),
        RS_NUM("equipment_array[6]",  equipment_array[6],  false, 0), RS_NUM("equipment_array[7]",  equipment_array[7],  false, 0),
        RS_NUM("equipment_array[8]",  equipment_array[8],  false, 0), RS_NUM("equipment_array[9]",  equipment_array[9],  false, 0),
        RS_NUM("equipment_array[10]", equipment_array[10], false, 0), RS_NUM("equipment_array[11]", equipment_array[11], false, 0),
        RS_NUM("equipment_array[12]", equipment_array[12], false, 0), RS_NUM("equipment_array[13]", equipment_array[13], false, 0),
        RS_NUM("equipment_array[14]", equipment_array[14], false, 0), RS_NUM("equipment_array[15]", equipment_array[15], false, 0),
        RS_NUM("equipment_array[16]", equipment_array[16], false, 0), RS_NUM("equipment_array[17]", equipment_array[17], false, 0),
        RS_NUM("equipment_array[18]", equipment_array[18], false, 0), RS_NUM("equipment_array[19]", equipment_array[19], false, 0),
        // ---- PadManipulatorPacked: action buttons (participate in any_action_inputted) ----
        RS_BIT("r1_weapon_attack_input",     padmanipulator.r1_weapon_attack_input,     true, true, 0),
        RS_BIT("l1_input",                   padmanipulator.l1_input,                   true, true, 0),
        RS_BIT("r1_magic_attack_input",      padmanipulator.r1_magic_attack_input,      true, true, 0),
        RS_BIT("l1_magic_attack_input",      padmanipulator.l1_magic_attack_input,      true, true, 0),
        RS_BIT("r2_input",                   padmanipulator.r2_input,                   true, true, 0),
        RS_BIT("backstep_input",             padmanipulator.backstep_input,             true, true, 0),
        RS_BIT("use_button_pressed",         padmanipulator.use_button_pressed,         true, true, 0),
        RS_BIT("roll_forward_input",         padmanipulator.roll_forward_input,         true, true, 0),
        RS_BIT("lefthand_weapon_attack",     padmanipulator.lefthand_weapon_attack,     true, true, 0),
        RS_BIT("parry_input",                padmanipulator.parry_input,                true, true, 0),
        RS_BIT("block_input",                padmanipulator.block_input,                true, true, 0),
        RS_BIT("jump_input",                 padmanipulator.jump_input,                 true, true, 0),
        RS_BIT("l1_weapon_attack",           padmanipulator.l1_weapon_attack,           true, true, 0),
        RS_BIT("l2_weapon_attack",           padmanipulator.l2_weapon_attack,           true, true, 0),
        // ---- PadManipulatorPacked: other flags ----
        RS_BIT("isSprintingAnim",            padmanipulator.isSprintingAnim,            false, true, 0),
        RS_BIT("not_getting_movement_input", padmanipulator.not_getting_movement_input, false, true, 1),
        RS_BITS2("change_2handing_state",    padmanipulator.change_2handing_state,      true),
        RS_BIT("left_hand_slot_selected",    padmanipulator.left_hand_slot_selected,    false, true, 0),
        RS_BIT("right_hand_slot_selected",   padmanipulator.right_hand_slot_selected,   false, true, 0),
        RS_BIT("any_action_inputted",        padmanipulator.any_action_inputted,        false, true, 0),
        RS_BIT("EnableBackStep",             padmanipulator.EnableBackStep,             false, false, 0),
        RS_BIT("EnableBackStep_forward",     padmanipulator.EnableBackStep_forward,     false, false, 0),
        RS_NUM("movement_related_flags",     padmanipulator.movement_related_flags,     false, 0),
        // ---- PadManipulatorPacked: movement / camera (see header comment) ----
        RS_NUM("camera_x_direction_movement_input_amount",     padmanipulator.camera_x_direction_movement_input_amount,     true, 0),
        RS_NUM("camera_x_direction_movement_input_amount_alt", padmanipulator.camera_x_direction_movement_input_amount_alt, true, 0),
        RS_NUM("camera_y_direction_movement_input_amount",     padmanipulator.camera_y_direction_movement_input_amount,     true, 0),
        RS_NUM("camera_y_direction_movement_input_amount_alt", padmanipulator.camera_y_direction_movement_input_amount_alt, true, 0),
        RS_NUM("continuous_weapon_controlled_angle", padmanipulator.continuous_weapon_controlled_angle, false, 0),
        RS_NUM("delta_pc_rotation_perframe",         padmanipulator.delta_pc_rotation_perframe,         true, 0),
        RS_NUM("weapon_controlled_angle",            padmanipulator.weapon_controlled_angle,            false, 0),
        RS_NUM("pc_rotation",                        padmanipulator.pc_rotation,                        false, 0),
        RS_NUM("delta_camera_y_rotation_perframe",   padmanipulator.delta_camera_y_rotation_perframe,   true, 0),
        RS_NUM("delta_camera_x_rotation_perframe",   padmanipulator.delta_camera_x_rotation_perframe,   true, 0),
        RS_NUM("camera_y_rotation",                  padmanipulator.camera_y_rotation,                  false, 0),
        RS_NUM("camera_x_rotation",                  padmanipulator.camera_x_rotation,                  false, 0),
        RS_NUM("movement_velocity[0]", padmanipulator.movement_velocity[0], true, 0),
        RS_NUM("movement_velocity[1]", padmanipulator.movement_velocity[1], true, 0),
        RS_NUM("movement_velocity[2]", padmanipulator.movement_velocity[2], true, 0),
        RS_NUM("movement_velocity[3]", padmanipulator.movement_velocity[3], true, 0),
        RS_NUM("TimeRollButtonHeld",   padmanipulator.TimeRollButtonHeld,   true, 0),
        RS_NUM("Backstep_timer",       padmanipulator.Backstep_timer,       true, 0),
        RS_NUM("LockonTargetHandle",   padmanipulator.LockonTargetHandle,   false, 0),
        RS_NUM("CurrentFrame_ActionInputs_ButtonId", padmanipulator.CurrentFrame_ActionInputs_ButtonId, true, -1),
        RS_NUM("y_movement_input[0]", padmanipulator.y_movement_input[0], true, 0), RS_NUM("y_movement_input[1]", padmanipulator.y_movement_input[1], true, 0),
        RS_NUM("y_movement_input[2]", padmanipulator.y_movement_input[2], true, 0), RS_NUM("y_movement_input[3]", padmanipulator.y_movement_input[3], true, 0),
        RS_NUM("y_movement_input[4]", padmanipulator.y_movement_input[4], true, 0), RS_NUM("y_movement_input[5]", padmanipulator.y_movement_input[5], true, 0),
        RS_NUM("x_movement_input[0]", padmanipulator.x_movement_input[0], true, 0), RS_NUM("x_movement_input[1]", padmanipulator.x_movement_input[1], true, 0),
        RS_NUM("x_movement_input[2]", padmanipulator.x_movement_input[2], true, 0), RS_NUM("x_movement_input[3]", padmanipulator.x_movement_input[3], true, 0),
        RS_NUM("x_movement_input[4]", padmanipulator.x_movement_input[4], true, 0), RS_NUM("x_movement_input[5]", padmanipulator.x_movement_input[5], true, 0),
        RS_NUM("cur_movement_input_index_to_use", padmanipulator.cur_movement_input_index_to_use, false, 0),
    };
    inline constexpr int FIELD_COUNT = (int)(sizeof(FIELDS) / sizeof(FIELDS[0]));

#undef RS_NUM
#undef RS_BIT
#undef RS_BITS2

    // Short names. A group alias expands to several fields (array "all copies").
    struct Alias { const char* alias; const char* target; };
    inline const Alias ALIASES[] = {
        { "r1", "r1_weapon_attack_input" }, { "l1", "l1_input" }, { "r2", "r2_input" }, { "l2", "l2_weapon_attack" },
        { "r1_magic", "r1_magic_attack_input" }, { "l1_magic", "l1_magic_attack_input" },
        { "roll", "roll_forward_input" }, { "backstep", "backstep_input" }, { "use", "use_button_pressed" },
        { "parry", "parry_input" }, { "block", "block_input" }, { "jump", "jump_input" },
        { "lefthand_attack", "lefthand_weapon_attack" }, { "l1_weapon", "l1_weapon_attack" },
        { "sprint", "isSprintingAnim" }, { "two_hand", "change_2handing_state" }, { "lockon", "LockonTargetHandle" },
        { "move_x", "camera_x_direction_movement_input_amount" }, { "move_y", "camera_y_direction_movement_input_amount" },
        { "move_x_alt", "camera_x_direction_movement_input_amount_alt" }, { "move_y_alt", "camera_y_direction_movement_input_amount_alt" },
        { "rotation", "pc_rotation" }, { "turn", "delta_pc_rotation_perframe" }, { "no_move", "not_getting_movement_input" },
        { "any_action", "any_action_inputted" },
    };
    // name-without-index -> every element
    struct GroupAlias { const char* alias; const char* member; int count; };
    inline const GroupAlias GROUPS[] = {
        { "movement_velocity", "movement_velocity", 4 },
        { "x_movement_input", "x_movement_input", 6 }, { "stick_x", "x_movement_input", 6 },
        { "y_movement_input", "y_movement_input", 6 }, { "stick_y", "y_movement_input", 6 },
        { "equipment_array", "equipment_array", 20 },
    };

    inline int field_index(const char* name)
    {
        for (int i = 0; i < FIELD_COUNT; i++)
            if (strcmp(FIELDS[i].name, name) == 0) return i;
        return -1;
    }

    // Resolve a user-facing name (struct member, alias, or group) to field indices.
    inline std::vector<int> resolve(const std::string& name)
    {
        std::vector<int> out;
        int idx = field_index(name.c_str());
        if (idx >= 0) { out.push_back(idx); return out; }
        for (const Alias& a : ALIASES)
            if (name == a.alias) { out.push_back(field_index(a.target)); return out; }
        for (const GroupAlias& g : GROUPS)
            if (name == g.alias)
            {
                for (int i = 0; i < g.count; i++)
                {
                    char buf[64];
                    snprintf(buf, sizeof(buf), "%s[%d]", g.member, i);
                    out.push_back(field_index(buf));
                }
                return out;
            }
        return out;   // empty = unknown
    }

    struct Directive
    {
        int start;
        int end;       // inclusive; INT_MAX for "@*"
        std::vector<std::pair<int, double>> sets;   // (field index, value)
        int line;      // 1-based source line, for diagnostics
    };

    // ---- state (game thread only) ----
    inline bool        loaded = false;
    inline bool        neutral = false;
    inline std::string name;
    inline std::string source;          // file path or "control"
    inline std::vector<Directive> directives;
    inline int         applied_frames = 0;      // frames this session on which the script changed the input
    inline int         last_applied_frame = -1;
    inline int         first_frame = -1, last_frame = -1;
    // Snapshot of the final local input sent to GGPO (post replay/script), for the "input" command.
    inline RollbackInput last_input{};
    inline int         last_input_frame = -1;

    inline void clear()
    {
        loaded = false;
        neutral = false;
        name.clear();
        source.clear();
        directives.clear();
        applied_frames = 0;
        last_applied_frame = -1;
        first_frame = last_frame = -1;
    }

    inline void _recompute_bounds()
    {
        first_frame = last_frame = -1;
        for (const Directive& d : directives)
        {
            if (first_frame < 0 || d.start < first_frame) first_frame = d.start;
            int e = (d.end == INT_MAX) ? d.start : d.end;
            if (e > last_frame) last_frame = e;
        }
    }

    inline std::string _trim(const std::string& s)
    {
        size_t a = s.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) return "";
        size_t b = s.find_last_not_of(" \t\r\n");
        return s.substr(a, b - a + 1);
    }

    inline bool _parse_int(const std::string& s, int* out)
    {
        if (s.empty()) return false;
        char* end = nullptr;
        long v = strtol(s.c_str(), &end, 0);
        if (end == s.c_str() || *end != '\0') return false;
        if (v < 0 || v > INT_MAX) return false;
        *out = (int)v;
        return true;
    }

    inline bool _parse_value(const std::string& s, double* out)
    {
        if (s == "true" || s == "on")  { *out = 1.0; return true; }
        if (s == "false" || s == "off") { *out = 0.0; return true; }
        if (s.empty()) return false;
        // hex integers (strtod's hex support varies; do it by hand)
        if (s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        {
            char* end = nullptr;
            unsigned long long v = strtoull(s.c_str(), &end, 16);
            if (end == s.c_str() || *end != '\0') return false;
            *out = (double)v;
            return true;
        }
        char* end = nullptr;
        double v = strtod(s.c_str(), &end);
        if (end == s.c_str() || *end != '\0') return false;
        *out = v;
        return true;
    }

    // "@A", "@A-B", "@A+N", "@*"
    inline bool _parse_frames(const std::string& tok, int* start, int* end, std::string* err)
    {
        std::string s = tok.substr(1);
        if (s == "*") { *start = 0; *end = INT_MAX; return true; }
        size_t dash = s.find('-');
        size_t plus = s.find('+');
        if (dash != std::string::npos)
        {
            int a = 0, b = 0;
            if (!_parse_int(s.substr(0, dash), &a) || !_parse_int(s.substr(dash + 1), &b)) { *err = "bad frame range '" + tok + "'"; return false; }
            if (b < a) { *err = "frame range end < start in '" + tok + "'"; return false; }
            *start = a; *end = b; return true;
        }
        if (plus != std::string::npos)
        {
            int a = 0, n = 0;
            if (!_parse_int(s.substr(0, plus), &a) || !_parse_int(s.substr(plus + 1), &n) || n < 1) { *err = "bad frame span '" + tok + "'"; return false; }
            *start = a; *end = a + n - 1; return true;
        }
        int a = 0;
        if (!_parse_int(s, &a)) { *err = "bad frame '" + tok + "'"; return false; }
        *start = a; *end = a; return true;
    }

    // Parse one line (directive or pragma) and append it. Comments/blank lines are
    // accepted and ignored. Returns false with *err set on any problem.
    inline bool add_line(const std::string& raw_line, int line_no, std::string* err)
    {
        std::string line = raw_line;
        size_t hash = line.find('#');
        if (hash != std::string::npos) line = line.substr(0, hash);
        line = _trim(line);
        if (line.empty()) return true;

        if (line[0] == '!')
        {
            size_t sp = line.find_first_of(" \t");
            std::string pragma = (sp == std::string::npos) ? line : line.substr(0, sp);
            std::string rest = (sp == std::string::npos) ? "" : _trim(line.substr(sp));
            if (pragma == "!neutral") { neutral = (rest.empty() || rest == "on" || rest == "1" || rest == "true"); return true; }
            if (pragma == "!name")    { name = rest; return true; }
            *err = "unknown pragma '" + pragma + "'";
            return false;
        }

        // tokenize on whitespace
        std::vector<std::string> toks;
        {
            size_t i = 0;
            while (i < line.size())
            {
                while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) i++;
                size_t j = i;
                while (j < line.size() && line[j] != ' ' && line[j] != '\t') j++;
                if (j > i) toks.push_back(line.substr(i, j - i));
                i = j;
            }
        }
        if (toks.empty()) return true;
        if (toks[0][0] != '@') { *err = "directive must start with a frame spec (@N, @A-B, @A+N, @*)"; return false; }

        Directive d{};
        d.line = line_no;
        if (!_parse_frames(toks[0], &d.start, &d.end, err)) return false;
        if (toks.size() < 2) { *err = "no field assignments after '" + toks[0] + "'"; return false; }

        for (size_t t = 1; t < toks.size(); t++)
        {
            size_t eq = toks[t].find('=');
            if (eq == std::string::npos) { *err = "expected field=value, got '" + toks[t] + "'"; return false; }
            std::string key = toks[t].substr(0, eq);
            std::string val = toks[t].substr(eq + 1);
            std::vector<int> idxs = resolve(key);
            if (idxs.empty()) { *err = "unknown field '" + key + "'"; return false; }
            double v = 0.0;
            if (!_parse_value(val, &v)) { *err = "bad value '" + val + "' for '" + key + "'"; return false; }
            for (int idx : idxs) d.sets.emplace_back(idx, v);
        }
        directives.push_back(d);
        loaded = true;
        _recompute_bounds();
        return true;
    }

    // Replace the current script with the given text. On error the previous
    // script is discarded too (a half-loaded script is worse than none) and
    // *err names the line.
    inline bool parse_text(const std::string& text, const char* src, std::string* err)
    {
        clear();
        source = src ? src : "";
        int line_no = 0;
        size_t pos = 0;
        while (pos <= text.size())
        {
            size_t nl = text.find('\n', pos);
            std::string line = text.substr(pos, (nl == std::string::npos) ? std::string::npos : nl - pos);
            line_no++;
            std::string e;
            if (!add_line(line, line_no, &e))
            {
                char buf[64];
                snprintf(buf, sizeof(buf), "line %d: ", line_no);
                *err = buf + e;
                clear();
                return false;
            }
            if (nl == std::string::npos) break;
            pos = nl + 1;
        }
        // a pragma-only script (e.g. just "!neutral") is still a script
        if (!directives.empty() || neutral) loaded = true;
        return true;
    }

    inline bool load_file(const char* path, std::string* err)
    {
        FILE* f = nullptr;
        fopen_s(&f, path, "rb");
        if (!f) { *err = std::string("cannot open '") + path + "'"; return false; }
        std::string text;
        char buf[4096];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), f)) > 0) text.append(buf, n);
        fclose(f);
        return parse_text(text, path, err);
    }

    inline bool _file_exists(const char* path)
    {
        FILE* f = nullptr;
        fopen_s(&f, path, "rb");
        if (f) { fclose(f); return true; }
        return false;
    }

    inline unsigned _any_action(const RollbackInput* i)
    {
        const PadManipulatorPacked& p = i->padmanipulator;
        return (p.r1_weapon_attack_input | p.l1_input | p.r1_magic_attack_input | p.l1_magic_attack_input |
                p.r2_input | p.backstep_input | p.use_button_pressed | p.roll_forward_input |
                p.lefthand_weapon_attack | p.parry_input | p.block_input | p.jump_input |
                p.l1_weapon_attack | p.l2_weapon_attack) ? 1u : 0u;
    }

    // Overlay the script onto the frame's live input. Pure function of (frame,
    // script) apart from the counters, so it is rollback-safe.
    inline void apply(int frame, RollbackInput* io)
    {
        if (!loaded) return;
        bool touched = false, touched_button = false, set_any = false;
        const int any_idx = field_index("any_action_inputted");

        if (neutral)
        {
            for (int i = 0; i < FIELD_COUNT; i++)
                if (FIELDS[i].neutral_zero) FIELDS[i].set(io, FIELDS[i].neutral_value);
            touched = true;
        }
        for (const Directive& d : directives)
        {
            if (frame < d.start || frame > d.end) continue;
            for (const auto& s : d.sets)
            {
                FIELDS[s.first].set(io, s.second);
                touched = true;
                if (FIELDS[s.first].button) touched_button = true;
                if (s.first == any_idx) set_any = true;
            }
        }
        if (touched_button && !set_any)
        {
            io->padmanipulator.any_action_inputted = _any_action(io);
        }
        if (touched)
        {
            applied_frames++;
            last_applied_frame = frame;
        }
    }

    // Remember the final input for this frame (after replay+script) for "input".
    inline void observe(int frame, const RollbackInput* io)
    {
        last_input = *io;
        last_input_frame = frame;
    }

    inline void init_session()
    {
        applied_frames = 0;
        last_applied_frame = -1;
        if (!loaded && _file_exists(SCRIPT_FILE))
        {
            std::string err;
            if (load_file(SCRIPT_FILE, &err))
                ConsoleWrite("RollbackScript: loaded '%s' (%d directives, frames %d..%d, neutral=%d)",
                             SCRIPT_FILE, (int)directives.size(), first_frame, last_frame, (int)neutral);
            else
                ConsoleWrite("RollbackScript: failed to load '%s': %s", SCRIPT_FILE, err.c_str());
        }
        if (loaded)
            ConsoleWrite("RollbackScript: ACTIVE '%s' (%d directives, frames %d..%d, neutral=%d)",
                         name.empty() ? source.c_str() : name.c_str(), (int)directives.size(), first_frame, last_frame, (int)neutral);
        else
            ConsoleWrite("RollbackScript: none");
    }

    inline void end_session()
    {
        if (loaded)
            ConsoleWrite("RollbackScript: session ended, script changed input on %d frames (last %d)", applied_frames, last_applied_frame);
    }

    // ---- JSON helpers (shared with HarnessControl) ----
    inline std::string json_escape(const std::string& s)
    {
        std::string out;
        out.reserve(s.size() + 8);
        for (unsigned char c : s)
        {
            switch (c)
            {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) { char b[8]; snprintf(b, sizeof(b), "\\u%04x", c); out += b; }
                else out += (char)c;
            }
        }
        return out;
    }

    inline std::string json_number(double v)
    {
        char b[48];
        if (v == (double)(long long)v && v > -9.0e15 && v < 9.0e15) snprintf(b, sizeof(b), "%lld", (long long)v);
        else snprintf(b, sizeof(b), "%.9g", v);
        return b;
    }

    inline std::string status_json()
    {
        std::string s = "{";
        s += "\"loaded\":" + std::string(loaded ? "true" : "false");
        s += ",\"source\":\"" + json_escape(source) + "\"";
        s += ",\"name\":\"" + json_escape(name) + "\"";
        s += ",\"neutral\":" + std::string(neutral ? "true" : "false");
        s += ",\"directives\":" + std::to_string(directives.size());
        s += ",\"first_frame\":" + std::to_string(first_frame);
        s += ",\"last_frame\":" + std::to_string(last_frame);
        s += ",\"applied_frames\":" + std::to_string(applied_frames);
        s += ",\"last_applied_frame\":" + std::to_string(last_applied_frame);
        s += "}";
        return s;
    }

    inline std::string input_json()
    {
        std::string s = "{\"frame\":" + std::to_string(last_input_frame) + ",\"fields\":{";
        for (int i = 0; i < FIELD_COUNT; i++)
        {
            if (i) s += ",";
            s += "\"";
            s += FIELDS[i].name;
            s += "\":" + json_number(FIELDS[i].get(&last_input));
        }
        s += "}}";
        return s;
    }
}

#endif // ROLLBACK_SCRIPT_H
