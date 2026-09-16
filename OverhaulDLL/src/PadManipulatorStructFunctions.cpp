#include "PadManipulatorStructFunctions.h"
#include "Rollback.h"
#include "StateSerializer.h"
#include <format>
#include <cstddef>


void copy_PadManipulator(PadManipulator* to, PadManipulator* from)
{
    memcpy(to, from, sizeof(PadManipulator));
}

// ---- serializer (field-by-field, derived from print_PadManipulator) ----------
//
// Drives both print_PadManipulator (Print mode) and hash_PadManipulator (Hash
// mode), so print and hash always agree. Covers the whole struct that
// copy_PadManipulator memcpy's: the named fields the old print enumerated, plus
// the few groups it omitted but which are still saved -- vtable (fixed code addr),
// PrevFrame_ActionInputs, ActionInputtedTimeHeld, and the heap pointers (recorded
// as null/non-null since their addresses vary across instances). Opaque single-
// byte fieldN_0xNN runs are hashed as blobs (bit-exact; struct is #pragma pack(1)
// so offsetof gives exact ranges).
static void serialize_ChrManipulator_ActionInputted(StateVisitor& v, const char* name, const ChrManipulator_ActionInputted* e)
{
    v.begin(name);
    v.field("r1_weapon_attack_input_1", e->r1_weapon_attack_input_1);
    v.field("l1_input", e->l1_input);
    v.field("r1_magic_attack_input", e->r1_magic_attack_input);
    v.field("l1_magic_attack_input", e->l1_magic_attack_input);
    v.field("field4_0x4", e->field4_0x4);
    v.field("r2_input", e->r2_input);
    v.field("field6_0x6", e->field6_0x6);
    v.field("r1_weapon_attack_input_2", e->r1_weapon_attack_input_2);
    v.field("field8_0x8", e->field8_0x8);
    v.field("field9_0x9", e->field9_0x9);
    v.field("backstep_input", e->backstep_input);
    v.field("field11_0xb", e->field11_0xb);
    v.field("field12_0xc", e->field12_0xc);
    v.field("field13_0xd", e->field13_0xd);
    v.field("use_ButtonPressed", e->use_ButtonPressed);
    v.field("roll_forward_input", e->roll_forward_input);
    v.field("field16_0x10", e->field16_0x10);
    v.field("field17_0x11", e->field17_0x11);
    v.field("field18_0x12", e->field18_0x12);
    v.field("lefthand_weapon_attack", e->lefthand_weapon_attack);
    v.field("parry_input", e->parry_input);
    v.field("block_input", e->block_input);
    v.excluded("beckon_emote_input", sizeof(e->beckon_emote_input));
    v.excluded("point_forward_emote_input", sizeof(e->point_forward_emote_input));
    v.excluded("hurrah_emote_input", sizeof(e->hurrah_emote_input));
    v.excluded("bow_emote_input", sizeof(e->bow_emote_input));
    v.excluded("joy_emote_input", sizeof(e->joy_emote_input));
    v.excluded("shrug_emote_input", sizeof(e->shrug_emote_input));
    v.excluded("wave_emote_input", sizeof(e->wave_emote_input));
    v.excluded("praise_the_sun_emote_input", sizeof(e->praise_the_sun_emote_input));
    v.excluded("point_up_emote_input", sizeof(e->point_up_emote_input));
    v.excluded("point_down_emote_input", sizeof(e->point_down_emote_input));
    v.excluded("look_skyward_emote_input", sizeof(e->look_skyward_emote_input));
    v.excluded("well_what_is_it_emote_input", sizeof(e->well_what_is_it_emote_input));
    v.excluded("prostration_emote_input", sizeof(e->prostration_emote_input));
    v.excluded("proper_bow_emote_input", sizeof(e->proper_bow_emote_input));
    v.excluded("prayer_emote_input", sizeof(e->prayer_emote_input));
    v.field("field37_0x25", e->field37_0x25);
    v.field("field38_0x26", e->field38_0x26);
    v.field("field39_0x27", e->field39_0x27);
    v.field("field40_0x28", e->field40_0x28);
    v.field("field41_0x29", e->field41_0x29);
    v.field("jump_input", e->jump_input);
    v.field("field43_0x2b", e->field43_0x2b);
    v.field("field44_0x2c", e->field44_0x2c);
    v.field("field45_0x2d", e->field45_0x2d);
    v.field("field46_0x2e", e->field46_0x2e);
    v.field("field47_0x2f", e->field47_0x2f);
    v.field("field48_0x30", e->field48_0x30);
    v.field("field49_0x31", e->field49_0x31);
    v.field("field50_0x32", e->field50_0x32);
    v.field("l1_weapon_attack", e->l1_weapon_attack);
    v.field("l2_weapon_attack", e->l2_weapon_attack);
    v.end();
}

void serialize_PadManipulator(StateVisitor& v, const PadManipulator* p)
{
    v.begin("PadManipulator");
    const uint8_t* b = (const uint8_t*)p;
    const ChrManipulator* c = &p->chrManipulator;

    // ---- ChrManipulator ----
    v.ptr_flag("chrManipulator.vtable", c->vtable);
    v.field("field1_0x8", c->field1_0x8);
    v.field("field2_0x9", c->field2_0x9);
    v.field("field3_0xa", c->field3_0xa);
    v.field("field4_0xb", c->field4_0xb);
    v.field("field5_0xc", c->field5_0xc);
    v.field("field6_0xd", c->field6_0xd);
    v.field("field7_0xe", c->field7_0xe);
    v.field("field8_0xf", c->field8_0xf);
    v.field("camera_x_direction_movement_input_amount", c->camera_x_direction_movement_input_amount);
    v.field("camera_z_direction_movement_input_amount", c->camera_z_direction_movement_input_amount);
    v.field("camera_y_direction_movement_input_amount", c->camera_y_direction_movement_input_amount);
    v.field("camera_w_direction_movement_input_amount", c->camera_w_direction_movement_input_amount);
    v.field("continuous_weapon_controlled_angle", c->continuous_weapon_controlled_angle);
    v.field("delta_pc_rotation_perframe", c->delta_pc_rotation_perframe);
    v.field("field15_0x28", c->field15_0x28);
    v.field("field16_0x2c", c->field16_0x2c);
    v.field("weapon_controlled_angle", c->weapon_controlled_angle);
    v.field("pc_rotation", c->pc_rotation);
    v.field("field19_0x38", c->field19_0x38);
    v.field("field20_0x3c", c->field20_0x3c);
    v.field("delta_camera_y_rotation_perframe", c->delta_camera_y_rotation_perframe);
    v.field("delta_camera_x_rotation_perframe", c->delta_camera_x_rotation_perframe);
    v.field("field23_0x48", c->field23_0x48);
    v.field("field24_0x4c", c->field24_0x4c);
    v.field("camera_y_rotation", c->camera_y_rotation);
    v.field("camera_x_rotation", c->camera_x_rotation);
    v.field("field27_0x58", c->field27_0x58);
    v.field("field28_0x5c", c->field28_0x5c);
    v.field("camera_x_direction_movement_input_amount_alt", c->camera_x_direction_movement_input_amount_alt);
    v.field("camera_z_direction_movement_input_amount_alt", c->camera_z_direction_movement_input_amount_alt);
    v.field("camera_y_direction_movement_input_amount_alt", c->camera_y_direction_movement_input_amount_alt);
    v.field("camera_w_direction_movement_input_amount_alt", c->camera_w_direction_movement_input_amount_alt);
    for (int i = 0; i < 4; i++) v.field("movement_velocity", c->movement_velocity[i]);
    v.field("field34_0x80", c->field34_0x80);
    serialize_ChrManipulator_ActionInputted(v, "CurrentFrame_ActionInputs", &c->CurrentFrame_ActionInputs);
    serialize_ChrManipulator_ActionInputted(v, "PrevFrame_ActionInputs", &c->PrevFrame_ActionInputs);
    v.field("field37_0xee", c->field37_0xee);
    v.field("field38_0xef", c->field38_0xef);
    //ActionInputtedTimeHeld is one float per CurrentFrame_ActionInputs byte, same index. The 15
    //emote timers are excluded for the same reason the emote button flags above are: emotes are
    //not carried in RollbackInput, so the two instances can never agree on them.
    {
        const uint8_t* th = (const uint8_t*)&c->ActionInputtedTimeHeld;
        const size_t emote_begin = offsetof(ChrManipulator_ActionInputtedTimeHeld, time_beckon_emote_held);
        const size_t emote_end = offsetof(ChrManipulator_ActionInputtedTimeHeld, field37_0x94);
        static_assert(offsetof(ChrManipulator_ActionInputtedTimeHeld, time_beckon_emote_held) == 0x58);
        static_assert(offsetof(ChrManipulator_ActionInputtedTimeHeld, field37_0x94) == 0x94);
        v.blob("ActionInputtedTimeHeld_pre_emotes", th, emote_begin);
        v.excluded("ActionInputtedTimeHeld_emotes", emote_end - emote_begin);
        v.blob("ActionInputtedTimeHeld_post_emotes", th + emote_end, sizeof(c->ActionInputtedTimeHeld) - emote_end);
    }
    v.field("AnyActionInputted", c->AnyActionInputted);
    v.field("buttonInteract_pressed", c->buttonInteract_pressed);
    v.field("field46_0x1c6", c->field46_0x1c6);
    v.field("field47_0x1c7", c->field47_0x1c7);
    v.field("change_2handing_state", c->change_2handing_state);
    v.field("left_hand_slot_selected", c->left_hand_slot_selected);
    v.field("right_hand_slot_selected", c->right_hand_slot_selected);
    v.field("field51_0x1d4", c->field51_0x1d4);
    v.field("field52_0x1d8", c->field52_0x1d8);
    v.field("field53_0x1dc", c->field53_0x1dc);
    v.field("field54_0x1e0", c->field54_0x1e0);
    v.field("isSprintingAnim", c->isSprintingAnim);
    v.field("field56_0x1e5", c->field56_0x1e5);
    v.field("field57_0x1e6", c->field57_0x1e6);
    v.field("field58_0x1e7", c->field58_0x1e7);
    v.field("field59_0x1e8", c->field59_0x1e8);
    v.field("ezStateActiveState", c->ezStateActiveState);
    v.field("ezStatePassiveState", c->ezStatePassiveState);
    v.field("field62_0x1f4", c->field62_0x1f4);
    v.field("field63_0x1f5", c->field63_0x1f5);
    v.field("field64_0x1f6", c->field64_0x1f6);
    v.field("field65_0x1f7", c->field65_0x1f7);
    v.field("field66_0x1f8", c->field66_0x1f8);
    v.field("field67_0x1f9", c->field67_0x1f9);
    v.field("field68_0x1fa", c->field68_0x1fa);
    v.field("field69_0x1fb", c->field69_0x1fb);
    v.field("field70_0x1fc", c->field70_0x1fc);
    v.field("field71_0x1fd", c->field71_0x1fd);
    v.field("field72_0x1fe", c->field72_0x1fe);
    v.field("field73_0x1ff", c->field73_0x1ff);
    v.field("field74_0x200", c->field74_0x200);
    v.field("field75_0x204", c->field75_0x204);
    v.field("field76_0x208", c->field76_0x208);
    v.field("field77_0x20c", c->field77_0x20c);
    for (int i = 0; i < 4; i++) v.field("field78_0x210", c->field78_0x210[i]);
    v.field("LockonTargetHandle", c->LockonTargetHandle);
    v.field("CurrentFrame_ActionInputs_ButtonId", c->CurrentFrame_ActionInputs_ButtonId);
    v.field("field81_0x228", c->field81_0x228);
    v.field("field82_0x229", c->field82_0x229);
    v.field("field83_0x22a", c->field83_0x22a);
    v.field("field84_0x22b", c->field84_0x22b);
    v.field("field85_0x22c", c->field85_0x22c);
    v.field("field86_0x22d", c->field86_0x22d);
    v.field("field87_0x22e", c->field87_0x22e);
    v.field("field88_0x22f", c->field88_0x22f);

    // ---- PadManipulator ----
    v.ptr_flag("SendGeneralPlayerData", p->SendGeneralPlayerData);
    v.field("ticks_remaining_before_packetsent", p->ticks_remaining_before_packetsent);
    v.field("movement_related_flags", p->movement_related_flags);
    v.field("not_getting_movement_input", p->not_getting_movement_input);
    v.field("field5_0x23e", p->field5_0x23e);
    v.field("field6_0x23f", p->field6_0x23f);
    v.ptr_flag("heap", p->heap);
    v.ptr_flag("arry_start", p->arry_start);
    v.ptr_flag("arry_end", p->arry_end);
    v.field("field10_0x258", p->field10_0x258);
    v.field("TimeRollButtonHeld", p->TimeRollButtonHeld);
    for (int i = 0; i < 6; i++) v.field("y_movement_input", p->y_movement_input[i]);
    // opaque single-byte run between the two input arrays
    v.blob("seg_27c", b + offsetof(PadManipulator, field13_0x27c),
           offsetof(PadManipulator, x_movement_input) - offsetof(PadManipulator, field13_0x27c));
    for (int i = 0; i < 6; i++) v.field("x_movement_input", p->x_movement_input[i]);
    // opaque single-byte run after x_movement_input
    v.blob("seg_2cc", b + offsetof(PadManipulator, field64_0x2cc),
           offsetof(PadManipulator, cur_movement_input_index_to_use) - offsetof(PadManipulator, field64_0x2cc));
    v.field("cur_movement_input_index_to_use", p->cur_movement_input_index_to_use);
    v.field("time_spend_forward_strafing", p->time_spend_forward_strafing);
    v.field("time_spend_back_strafing", p->time_spend_back_strafing);
    v.field("time_spend_left_strafing", p->time_spend_left_strafing);
    v.field("time_spend_right_strafing", p->time_spend_right_strafing);
    v.field("camera_xaxis_rotation", p->camera_xaxis_rotation);
    v.field("field126_0x31c", p->field126_0x31c);
    v.field("field127_0x31d", p->field127_0x31d);
    v.field("field128_0x31e", p->field128_0x31e);
    v.field("field129_0x31f", p->field129_0x31f);
    v.field("field130_0x320", p->field130_0x320);
    v.field("Backstep_timer", p->Backstep_timer);
    v.field("EnableBackStep", p->EnableBackStep);
    v.field("EnableBackStep_forward", p->EnableBackStep_forward);
    v.field("field134_0x32a", p->field134_0x32a);
    v.field("field135_0x32b", p->field135_0x32b);
    v.field("field136_0x32c", p->field136_0x32c);
    v.field("x_movement_input_reversed", p->x_movement_input_reversed);
    v.field("field141_0x334", p->field141_0x334);
    v.field("y_movement_input_reversed", p->y_movement_input_reversed);
    v.field("field143_0x33c", p->field143_0x33c);
    v.field("jump_trigger_time_remaining", p->jump_trigger_time_remaining);
    v.field("field145_0x344", p->field145_0x344);
    v.ptr_flag("dbgNode", p->dbgNode);
    // trailing opaque bytes
    v.blob("seg_tail", b + offsetof(PadManipulator, field147_0x350),
           sizeof(PadManipulator) - offsetof(PadManipulator, field147_0x350));
    v.end();
}

std::string print_PadManipulator(PadManipulator* pad)
{
    StateVisitor v(StateVisitor::Mode::Print);
    serialize_PadManipulator(v, pad);
    return v.text();
}

uint64_t hash_PadManipulator(const PadManipulator* p)
{
    StateVisitor v(StateVisitor::Mode::Hash);
    serialize_PadManipulator(v, p);
    return v.digest();
}

PadManipulator* init_PadManipulator()
{
    PadManipulator* local_PadManipulator = (PadManipulator*)malloc_(sizeof(PadManipulator));
    return local_PadManipulator;
}

void free_PadManipulator(PadManipulator* to)
{
    free(to);
}
