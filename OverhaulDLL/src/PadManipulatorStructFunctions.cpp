#include "PadManipulatorStructFunctions.h"
#include "Rollback.h"
#include <format>

std::string print_PadManipulatorPacked(PadManipulatorPacked* to)
{
    std::string out = "PadManipulatorPacked\n";
    out += std::to_string(to->camera_x_direction_movement_input_amount) + "\n";
    out += std::to_string(to->camera_y_direction_movement_input_amount) + "\n";
    out += std::to_string(to->continuous_weapon_controlled_angle) + "\n";
    out += std::to_string(to->delta_pc_rotation_perframe) + "\n";
    out += std::to_string(to->weapon_controlled_angle) + "\n";
    out += std::to_string(to->pc_rotation) + "\n";
    out += std::to_string(to->delta_camera_y_rotation_perframe) + "\n";
    out += std::to_string(to->delta_camera_x_rotation_perframe) + "\n";
    out += std::to_string(to->camera_y_rotation) + "\n";
    out += std::to_string(to->camera_x_rotation) + "\n";
    out += std::to_string(to->camera_x_direction_movement_input_amount_alt) + "\n";
    out += std::to_string(to->camera_y_direction_movement_input_amount_alt) + "\n";
    out += std::to_string(to->movement_velocity[0]) + "\n";
    out += std::to_string(to->movement_velocity[1]) + "\n";
    out += std::to_string(to->movement_velocity[2]) + "\n";
    out += std::to_string(to->movement_velocity[3]) + "\n";
    out += std::to_string(to->r1_weapon_attack_input) + "\n";
    out += std::to_string(to->l1_input) + "\n";
    out += std::to_string(to->r1_magic_attack_input) + "\n";
    out += std::to_string(to->l1_magic_attack_input) + "\n";
    out += std::to_string(to->r2_input) + "\n";
    out += std::to_string(to->backstep_input) + "\n";
    out += std::to_string(to->use_button_pressed) + "\n";
    out += std::to_string(to->roll_forward_input) + "\n";
    out += std::to_string(to->lefthand_weapon_attack) + "\n";
    out += std::to_string(to->parry_input) + "\n";
    out += std::to_string(to->block_input) + "\n";
    out += std::to_string(to->jump_input) + "\n";
    out += std::to_string(to->l1_weapon_attack) + "\n";
    out += std::to_string(to->l2_weapon_attack) + "\n";
    out += std::to_string(to->change_2handing_state) + "\n";
    out += std::to_string(to->left_hand_slot_selected) + "\n";
    out += std::to_string(to->right_hand_slot_selected) + "\n";
    out += std::to_string(to->isSprintingAnim) + "\n";
    out += std::to_string(to->LockonTargetHandle) + "\n";
    out += std::to_string(to->movement_related_flags) + "\n";
    out += std::to_string(to->not_getting_movement_input) + "\n";
    out += std::to_string(to->y_movement_input) + "\n";
    out += std::to_string(to->x_movement_input) + "\n";
    out += std::to_string(to->cur_movement_input_index_to_use) + "\n";
    return out;
}

std::string print_PadManipulator(PadManipulator* pad)
{
    std::string out = "PadManipulatorPacked\n";
    if (!pad)
    {
        out += std::format("NULL PadManipulator pointer\n");
        return out;
    }

    // PadManipulator.ChrManipulator
    out += std::format("chrManipulator.field1_0x8: {}\n", pad->chrManipulator.field1_0x8);
    out += std::format("chrManipulator.field2_0x9: {}\n", pad->chrManipulator.field2_0x9);
    out += std::format("chrManipulator.field3_0xa: {}\n", pad->chrManipulator.field3_0xa);
    out += std::format("chrManipulator.field4_0xb: {}\n", pad->chrManipulator.field4_0xb);
    out += std::format("chrManipulator.field5_0xc: {}\n", pad->chrManipulator.field5_0xc);
    out += std::format("chrManipulator.field6_0xd: {}\n", pad->chrManipulator.field6_0xd);
    out += std::format("chrManipulator.field7_0xe: {}\n", pad->chrManipulator.field7_0xe);
    out += std::format("chrManipulator.field8_0xf: {}\n", pad->chrManipulator.field8_0xf);
    out += std::format("chrManipulator.camera_x_direction_movement_input_amount: {}\n", pad->chrManipulator.camera_x_direction_movement_input_amount);
    out += std::format("chrManipulator.camera_z_direction_movement_input_amount: {}\n", pad->chrManipulator.camera_z_direction_movement_input_amount);
    out += std::format("chrManipulator.camera_y_direction_movement_input_amount: {}\n", pad->chrManipulator.camera_y_direction_movement_input_amount);
    out += std::format("chrManipulator.camera_w_direction_movement_input_amount: {}\n", pad->chrManipulator.camera_w_direction_movement_input_amount);
    out += std::format("chrManipulator.continuous_weapon_controlled_angle: {}\n", pad->chrManipulator.continuous_weapon_controlled_angle);
    out += std::format("chrManipulator.delta_pc_rotation_perframe: {}\n", pad->chrManipulator.delta_pc_rotation_perframe);
    out += std::format("chrManipulator.field15_0x28: {}\n", pad->chrManipulator.field15_0x28);
    out += std::format("chrManipulator.field16_0x2c: {}\n", pad->chrManipulator.field16_0x2c);
    out += std::format("chrManipulator.weapon_controlled_angle: {}\n", pad->chrManipulator.weapon_controlled_angle);
    out += std::format("chrManipulator.pc_rotation: {}\n", pad->chrManipulator.pc_rotation);
    out += std::format("chrManipulator.field19_0x38: {}\n", pad->chrManipulator.field19_0x38);
    out += std::format("chrManipulator.field20_0x3c: {}\n", pad->chrManipulator.field20_0x3c);
    out += std::format("chrManipulator.delta_camera_y_rotation_perframe: {}\n", pad->chrManipulator.delta_camera_y_rotation_perframe);
    out += std::format("chrManipulator.delta_camera_x_rotation_perframe: {}\n", pad->chrManipulator.delta_camera_x_rotation_perframe);
    out += std::format("chrManipulator.field23_0x48: {}\n", pad->chrManipulator.field23_0x48);
    out += std::format("chrManipulator.field24_0x4c: {}\n", pad->chrManipulator.field24_0x4c);
    out += std::format("chrManipulator.camera_y_rotation: {}\n", pad->chrManipulator.camera_y_rotation);
    out += std::format("chrManipulator.camera_x_rotation: {}\n", pad->chrManipulator.camera_x_rotation);
    out += std::format("chrManipulator.field27_0x58: {}\n", pad->chrManipulator.field27_0x58);
    out += std::format("chrManipulator.field28_0x5c: {}\n", pad->chrManipulator.field28_0x5c);
    out += std::format("chrManipulator.camera_x_direction_movement_input_amount_alt: {}\n", pad->chrManipulator.camera_x_direction_movement_input_amount_alt);
    out += std::format("chrManipulator.camera_z_direction_movement_input_amount_alt: {}\n", pad->chrManipulator.camera_z_direction_movement_input_amount_alt);
    out += std::format("chrManipulator.camera_y_direction_movement_input_amount_alt: {}\n", pad->chrManipulator.camera_y_direction_movement_input_amount_alt);
    out += std::format("chrManipulator.camera_w_direction_movement_input_amount_alt: {}\n", pad->chrManipulator.camera_w_direction_movement_input_amount_alt);
    for (int i = 0; i < 4; i++)
    {
        out += std::format("chrManipulator.movement_velocity[{}]: {}\n", i, pad->chrManipulator.movement_velocity[i]);
    }
    out += std::format("chrManipulator.field34_0x80: {}\n", pad->chrManipulator.field34_0x80);

    // chrManipulator.CurrentFrame_ActionInputs
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.r1_weapon_attack_input_1: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.r1_weapon_attack_input_1);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.l1_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.l1_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.r1_magic_attack_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.r1_magic_attack_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.l1_magic_attack_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.l1_magic_attack_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field4_0x4: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field4_0x4);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.r2_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.r2_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field6_0x6: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field6_0x6);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.r1_weapon_attack_input_2: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.r1_weapon_attack_input_2);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field8_0x8: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field8_0x8);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field9_0x9: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field9_0x9);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.backstep_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.backstep_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field11_0xb: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field11_0xb);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field12_0xc: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field12_0xc);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field13_0xd: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field13_0xd);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.use_ButtonPressed: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.use_ButtonPressed);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.roll_forward_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.roll_forward_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field16_0x10: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field16_0x10);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field17_0x11: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field17_0x11);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field18_0x12: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field18_0x12);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.lefthand_weapon_attack: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.lefthand_weapon_attack);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.parry_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.parry_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.block_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.block_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field22_0x16: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field22_0x16);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field23_0x17: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field23_0x17);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field24_0x18: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field24_0x18);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field25_0x19: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field25_0x19);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field26_0x1a: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field26_0x1a);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field27_0x1b: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field27_0x1b);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field28_0x1c: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field28_0x1c);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field29_0x1d: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field29_0x1d);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field30_0x1e: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field30_0x1e);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field31_0x1f: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field31_0x1f);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field32_0x20: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field32_0x20);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field33_0x21: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field33_0x21);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field34_0x22: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field34_0x22);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field35_0x23: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field35_0x23);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field36_0x24: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field36_0x24);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field37_0x25: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field37_0x25);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field38_0x26: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field38_0x26);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field39_0x27: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field39_0x27);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field40_0x28: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field40_0x28);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field41_0x29: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field41_0x29);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.jump_input: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.jump_input);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field43_0x2b: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field43_0x2b);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field44_0x2c: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field44_0x2c);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field45_0x2d: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field45_0x2d);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field46_0x2e: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field46_0x2e);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field47_0x2f: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field47_0x2f);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field48_0x30: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field48_0x30);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field49_0x31: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field49_0x31);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.field50_0x32: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.field50_0x32);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.l1_weapon_attack: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.l1_weapon_attack);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs.l2_weapon_attack: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs.l2_weapon_attack);

    out += std::format("field37_0xee: {}\n", pad->chrManipulator.field37_0xee);
    out += std::format("field38_0xef: {}\n", pad->chrManipulator.field38_0xef);
    //ActionInputtedTimeHeld not included
    out += std::format("chrManipulator.AnyActionInputted: {}\n", pad->chrManipulator.AnyActionInputted);
    out += std::format("chrManipulator.buttonInteract_pressed: {}\n", pad->chrManipulator.buttonInteract_pressed);
    out += std::format("field46_0x1c6: {}\n", pad->chrManipulator.field46_0x1c6);
    out += std::format("field47_0x1c7: {}\n", pad->chrManipulator.field47_0x1c7);
    out += std::format("chrManipulator.change_2handing_state: {}\n", pad->chrManipulator.change_2handing_state);
    out += std::format("chrManipulator.left_hand_slot_selected: {}\n", pad->chrManipulator.left_hand_slot_selected);
    out += std::format("chrManipulator.right_hand_slot_selected: {}\n", pad->chrManipulator.right_hand_slot_selected);
    out += std::format("field51_0x1d4: {}\n", pad->chrManipulator.field51_0x1d4);
    out += std::format("field52_0x1d8: {}\n", pad->chrManipulator.field52_0x1d8);
    out += std::format("field53_0x1dc: {}\n", pad->chrManipulator.field53_0x1dc);
    out += std::format("field54_0x1e0: {}\n", pad->chrManipulator.field54_0x1e0);
    out += std::format("chrManipulator.isSprintingAnim: {}\n", pad->chrManipulator.isSprintingAnim);
    out += std::format("field56_0x1e5: {}\n", pad->chrManipulator.field56_0x1e5);
    out += std::format("field57_0x1e6: {}\n", pad->chrManipulator.field57_0x1e6);
    out += std::format("field58_0x1e7: {}\n", pad->chrManipulator.field58_0x1e7);
    out += std::format("field59_0x1e8: {}\n", pad->chrManipulator.field59_0x1e8);
    out += std::format("chrManipulator.ezStateActiveState: {}\n", pad->chrManipulator.ezStateActiveState);
    out += std::format("chrManipulator.ezStatePassiveState: {}\n", pad->chrManipulator.ezStatePassiveState);
    out += std::format("field62_0x1f4: {}\n", pad->chrManipulator.field62_0x1f4);
    out += std::format("field63_0x1f5: {}\n", pad->chrManipulator.field63_0x1f5);
    out += std::format("field64_0x1f6: {}\n", pad->chrManipulator.field64_0x1f6);
    out += std::format("field65_0x1f7: {}\n", pad->chrManipulator.field65_0x1f7);
    out += std::format("field66_0x1f8: {}\n", pad->chrManipulator.field66_0x1f8);
    out += std::format("field67_0x1f9: {}\n", pad->chrManipulator.field67_0x1f9);
    out += std::format("field68_0x1fa: {}\n", pad->chrManipulator.field68_0x1fa);
    out += std::format("field69_0x1fb: {}\n", pad->chrManipulator.field69_0x1fb);
    out += std::format("field70_0x1fc: {}\n", pad->chrManipulator.field70_0x1fc);
    out += std::format("field71_0x1fd: {}\n", pad->chrManipulator.field71_0x1fd);
    out += std::format("field72_0x1fe: {}\n", pad->chrManipulator.field72_0x1fe);
    out += std::format("field73_0x1ff: {}\n", pad->chrManipulator.field73_0x1ff);
    out += std::format("field74_0x200: {}\n", pad->chrManipulator.field74_0x200);
    out += std::format("field75_0x204: {}\n", pad->chrManipulator.field75_0x204);
    out += std::format("field76_0x208: {}\n", pad->chrManipulator.field76_0x208);
    out += std::format("field77_0x20c: {}\n", pad->chrManipulator.field77_0x20c);
    for (int i = 0; i < 4; i++)
    {
        out += std::format("field78_0x210[{}]: {}\n", i, pad->chrManipulator.field78_0x210[i]);
    }
    out += std::format("chrManipulator.LockonTargetHandle: {}\n", pad->chrManipulator.LockonTargetHandle);
    out += std::format("chrManipulator.CurrentFrame_ActionInputs_ButtonId: {}\n", pad->chrManipulator.CurrentFrame_ActionInputs_ButtonId);
    out += std::format("field81_0x228: {}\n", pad->chrManipulator.field81_0x228);
    out += std::format("field82_0x229: {}\n", pad->chrManipulator.field82_0x229);
    out += std::format("field83_0x22a: {}\n", pad->chrManipulator.field83_0x22a);
    out += std::format("field84_0x22b: {}\n", pad->chrManipulator.field84_0x22b);
    out += std::format("field85_0x22c: {}\n", pad->chrManipulator.field85_0x22c);
    out += std::format("field86_0x22d: {}\n", pad->chrManipulator.field86_0x22d);
    out += std::format("field87_0x22e: {}\n", pad->chrManipulator.field87_0x22e);
    out += std::format("field88_0x22f: {}\n", pad->chrManipulator.field88_0x22f);

    // PadManipulator
    out += std::format("movement_related_flags: {}\n", pad->movement_related_flags);
    out += std::format("not_getting_movement_input: {}\n", pad->not_getting_movement_input);
    out += std::format("field5_0x23e: {}\n", pad->field5_0x23e);
    out += std::format("field6_0x23f: {}\n", pad->field6_0x23f);
    out += std::format("field10_0x258: {}lu\n", pad->field10_0x258);
    out += std::format("DashInputTimer: {}\n", pad->DashInputTimer);
    for (int i = 0; i < 6; i++)
    {
        out += std::format("y_movement_input[{}]: {}\n", i, pad->y_movement_input[i]);
    }
    out += std::format("field13_0x27c: {}\n", pad->field13_0x27c);
    out += std::format("field14_0x27d: {}\n", pad->field14_0x27d);
    out += std::format("field15_0x27e: {}\n", pad->field15_0x27e);
    out += std::format("field16_0x27f: {}\n", pad->field16_0x27f);
    out += std::format("field17_0x280: {}\n", pad->field17_0x280);
    out += std::format("field18_0x284: {}\n", pad->field18_0x284);
    out += std::format("field19_0x285: {}\n", pad->field19_0x285);
    out += std::format("field20_0x286: {}\n", pad->field20_0x286);
    out += std::format("field21_0x287: {}\n", pad->field21_0x287);
    out += std::format("field22_0x288: {}\n", pad->field22_0x288);
    out += std::format("field23_0x289: {}\n", pad->field23_0x289);
    out += std::format("field24_0x28a: {}\n", pad->field24_0x28a);
    out += std::format("field25_0x28b: {}\n", pad->field25_0x28b);
    out += std::format("field26_0x28c: {}\n", pad->field26_0x28c);
    out += std::format("field27_0x290: {}\n", pad->field27_0x290);
    out += std::format("field28_0x291: {}\n", pad->field28_0x291);
    out += std::format("field29_0x292: {}\n", pad->field29_0x292);
    out += std::format("field30_0x293: {}\n", pad->field30_0x293);
    out += std::format("field31_0x294: {}\n", pad->field31_0x294);
    out += std::format("field32_0x295: {}\n", pad->field32_0x295);
    out += std::format("field33_0x296: {}\n", pad->field33_0x296);
    out += std::format("field34_0x297: {}\n", pad->field34_0x297);
    out += std::format("field35_0x298: {}\n", pad->field35_0x298);
    out += std::format("field36_0x299: {}\n", pad->field36_0x299);
    out += std::format("field37_0x29a: {}\n", pad->field37_0x29a);
    out += std::format("field38_0x29b: {}\n", pad->field38_0x29b);
    out += std::format("field39_0x29c: {}\n", pad->field39_0x29c);
    out += std::format("field40_0x29d: {}\n", pad->field40_0x29d);
    out += std::format("field41_0x29e: {}\n", pad->field41_0x29e);
    out += std::format("field42_0x29f: {}\n", pad->field42_0x29f);
    out += std::format("field43_0x2a0: {}\n", pad->field43_0x2a0);
    out += std::format("field44_0x2a1: {}\n", pad->field44_0x2a1);
    out += std::format("field45_0x2a2: {}\n", pad->field45_0x2a2);
    out += std::format("field46_0x2a3: {}\n", pad->field46_0x2a3);
    out += std::format("field47_0x2a4: {}\n", pad->field47_0x2a4);
    out += std::format("field48_0x2a5: {}\n", pad->field48_0x2a5);
    out += std::format("field49_0x2a6: {}\n", pad->field49_0x2a6);
    out += std::format("field50_0x2a7: {}\n", pad->field50_0x2a7);
    out += std::format("field51_0x2a8: {}\n", pad->field51_0x2a8);
    out += std::format("field52_0x2a9: {}\n", pad->field52_0x2a9);
    out += std::format("field53_0x2aa: {}\n", pad->field53_0x2aa);
    out += std::format("field54_0x2ab: {}\n", pad->field54_0x2ab);
    out += std::format("field55_0x2ac: {}\n", pad->field55_0x2ac);
    out += std::format("field56_0x2ad: {}\n", pad->field56_0x2ad);
    out += std::format("field57_0x2ae: {}\n", pad->field57_0x2ae);
    out += std::format("field58_0x2af: {}\n", pad->field58_0x2af);
    out += std::format("field59_0x2b0: {}\n", pad->field59_0x2b0);
    out += std::format("field60_0x2b1: {}\n", pad->field60_0x2b1);
    out += std::format("field61_0x2b2: {}\n", pad->field61_0x2b2);
    out += std::format("field62_0x2b3: {}\n", pad->field62_0x2b3);
    for (int i = 0; i < 6; i++)
    {
        out += std::format("x_movement_input[{}]: {}\n", i, pad->x_movement_input[i]);
    }
    out += std::format("field64_0x2cc: {}\n", pad->field64_0x2cc);
    out += std::format("field65_0x2cd: {}\n", pad->field65_0x2cd);
    out += std::format("field66_0x2ce: {}\n", pad->field66_0x2ce);
    out += std::format("field67_0x2cf: {}\n", pad->field67_0x2cf);
    out += std::format("field68_0x2d0: {}\n", pad->field68_0x2d0);
    out += std::format("field69_0x2d1: {}\n", pad->field69_0x2d1);
    out += std::format("field70_0x2d2: {}\n", pad->field70_0x2d2);
    out += std::format("field71_0x2d3: {}\n", pad->field71_0x2d3);
    out += std::format("field72_0x2d4: {}\n", pad->field72_0x2d4);
    out += std::format("field73_0x2d5: {}\n", pad->field73_0x2d5);
    out += std::format("field74_0x2d6: {}\n", pad->field74_0x2d6);
    out += std::format("field75_0x2d7: {}\n", pad->field75_0x2d7);
    out += std::format("field76_0x2d8: {}\n", pad->field76_0x2d8);
    out += std::format("field77_0x2d9: {}\n", pad->field77_0x2d9);
    out += std::format("field78_0x2da: {}\n", pad->field78_0x2da);
    out += std::format("field79_0x2db: {}\n", pad->field79_0x2db);
    out += std::format("field80_0x2dc: {}\n", pad->field80_0x2dc);
    out += std::format("field81_0x2dd: {}\n", pad->field81_0x2dd);
    out += std::format("field82_0x2de: {}\n", pad->field82_0x2de);
    out += std::format("field83_0x2df: {}\n", pad->field83_0x2df);
    out += std::format("field84_0x2e0: {}\n", pad->field84_0x2e0);
    out += std::format("field85_0x2e1: {}\n", pad->field85_0x2e1);
    out += std::format("field86_0x2e2: {}\n", pad->field86_0x2e2);
    out += std::format("field87_0x2e3: {}\n", pad->field87_0x2e3);
    out += std::format("field88_0x2e4: {}\n", pad->field88_0x2e4);
    out += std::format("field89_0x2e5: {}\n", pad->field89_0x2e5);
    out += std::format("field90_0x2e6: {}\n", pad->field90_0x2e6);
    out += std::format("field91_0x2e7: {}\n", pad->field91_0x2e7);
    out += std::format("field92_0x2e8: {}\n", pad->field92_0x2e8);
    out += std::format("field93_0x2e9: {}\n", pad->field93_0x2e9);
    out += std::format("field94_0x2ea: {}\n", pad->field94_0x2ea);
    out += std::format("field95_0x2eb: {}\n", pad->field95_0x2eb);
    out += std::format("field96_0x2ec: {}\n", pad->field96_0x2ec);
    out += std::format("field97_0x2ed: {}\n", pad->field97_0x2ed);
    out += std::format("field98_0x2ee: {}\n", pad->field98_0x2ee);
    out += std::format("field99_0x2ef: {}\n", pad->field99_0x2ef);
    out += std::format("field100_0x2f0: {}\n", pad->field100_0x2f0);
    out += std::format("field101_0x2f1: {}\n", pad->field101_0x2f1);
    out += std::format("field102_0x2f2: {}\n", pad->field102_0x2f2);
    out += std::format("field103_0x2f3: {}\n", pad->field103_0x2f3);
    out += std::format("field104_0x2f4: {}\n", pad->field104_0x2f4);
    out += std::format("field105_0x2f5: {}\n", pad->field105_0x2f5);
    out += std::format("field106_0x2f6: {}\n", pad->field106_0x2f6);
    out += std::format("field107_0x2f7: {}\n", pad->field107_0x2f7);
    out += std::format("field108_0x2f8: {}\n", pad->field108_0x2f8);
    out += std::format("field109_0x2f9: {}\n", pad->field109_0x2f9);
    out += std::format("field110_0x2fa: {}\n", pad->field110_0x2fa);
    out += std::format("field111_0x2fb: {}\n", pad->field111_0x2fb);
    out += std::format("field112_0x2fc: {}\n", pad->field112_0x2fc);
    out += std::format("field113_0x2fd: {}\n", pad->field113_0x2fd);
    out += std::format("field114_0x2fe: {}\n", pad->field114_0x2fe);
    out += std::format("field115_0x2ff: {}\n", pad->field115_0x2ff);
    out += std::format("field116_0x300: {}\n", pad->field116_0x300);
    out += std::format("field117_0x301: {}\n", pad->field117_0x301);
    out += std::format("field118_0x302: {}\n", pad->field118_0x302);
    out += std::format("field119_0x303: {}\n", pad->field119_0x303);
    out += std::format("cur_movement_input_index_to_use: {}\n", pad->cur_movement_input_index_to_use);
    out += std::format("time_spend_forward_strafing: {}\n", pad->time_spend_forward_strafing);
    out += std::format("time_spend_back_strafing: {}\n", pad->time_spend_back_strafing);
    out += std::format("time_spend_left_strafing: {}\n", pad->time_spend_left_strafing);
    out += std::format("time_spend_right_strafing: {}\n", pad->time_spend_right_strafing);
    out += std::format("camera_xaxis_rotation: {}\n", pad->camera_xaxis_rotation);
    out += std::format("field126_0x31c: {}\n", pad->field126_0x31c);
    out += std::format("field127_0x31d: {}\n", pad->field127_0x31d);
    out += std::format("field128_0x31e: {}\n", pad->field128_0x31e);
    out += std::format("field129_0x31f: {}\n", pad->field129_0x31f);
    out += std::format("field130_0x320: {}\n", pad->field130_0x320);
    out += std::format("Backstep_timer: {}\n", pad->Backstep_timer);
    out += std::format("EnableBackStep: {}\n", pad->EnableBackStep);
    out += std::format("EnableBackStep_forward: {}\n", pad->EnableBackStep_forward);
    out += std::format("field134_0x32a: {}\n", pad->field134_0x32a);
    out += std::format("field135_0x32b: {}\n", pad->field135_0x32b);
    out += std::format("field136_0x32c: {}\n", pad->field136_0x32c);
    out += std::format("x_movement_input_reversed: {}\n", pad->x_movement_input_reversed);
    out += std::format("field141_0x334: {}\n", pad->field141_0x334);
    out += std::format("y_movement_input_reversed: {}\n", pad->y_movement_input_reversed);
    out += std::format("field143_0x33c: {}\n", pad->field143_0x33c);
    out += std::format("jump_trigger_time_remaining: {}\n", pad->jump_trigger_time_remaining);
    out += std::format("field145_0x344: {}\n", pad->field145_0x344);
    out += std::format("field147_0x350: {}\n", pad->field147_0x350);
    out += std::format("field148_0x351: {}\n", pad->field148_0x351);
    out += std::format("field149_0x352: {}\n", pad->field149_0x352);
    out += std::format("field150_0x353: {}\n", pad->field150_0x353);
    out += std::format("field151_0x354: {}\n", pad->field151_0x354);
    out += std::format("field152_0x355: {}\n", pad->field152_0x355);
    out += std::format("field153_0x356: {}\n", pad->field153_0x356);
    out += std::format("field154_0x357: {}\n", pad->field154_0x357);
    out += std::format("field155_0x358: {}\n", pad->field155_0x358);
    out += std::format("field156_0x359: {}\n", pad->field156_0x359);
    out += std::format("field157_0x35a: {}\n", pad->field157_0x35a);
    out += std::format("field158_0x35b: {}\n", pad->field158_0x35b);
    out += std::format("field159_0x35c: {}\n", pad->field159_0x35c);
    out += std::format("field160_0x35d: {}\n", pad->field160_0x35d);
    out += std::format("field161_0x35e: {}\n", pad->field161_0x35e);
    out += std::format("field162_0x35f: {}\n", pad->field162_0x35f);

    return out;
}

void PadManipulator_to_PadManipulatorPacked(PadManipulatorPacked* to, PadManipulator* from)
{
    to->camera_x_direction_movement_input_amount = from->chrManipulator.camera_x_direction_movement_input_amount;
    to->camera_y_direction_movement_input_amount = from->chrManipulator.camera_y_direction_movement_input_amount;
    to->continuous_weapon_controlled_angle = from->chrManipulator.continuous_weapon_controlled_angle;
    to->delta_pc_rotation_perframe = from->chrManipulator.delta_pc_rotation_perframe;
    to->weapon_controlled_angle = from->chrManipulator.weapon_controlled_angle;
    to->pc_rotation = from->chrManipulator.pc_rotation;
    to->delta_camera_y_rotation_perframe = from->chrManipulator.delta_camera_y_rotation_perframe;
    to->delta_camera_x_rotation_perframe = from->chrManipulator.delta_camera_x_rotation_perframe;
    to->camera_y_rotation = from->chrManipulator.camera_y_rotation;
    to->camera_x_rotation = from->chrManipulator.camera_x_rotation;
    to->camera_x_direction_movement_input_amount_alt = from->chrManipulator.camera_x_direction_movement_input_amount_alt;
    to->camera_y_direction_movement_input_amount_alt = from->chrManipulator.camera_y_direction_movement_input_amount_alt;
    to->movement_velocity[0] = from->chrManipulator.movement_velocity[0];
    to->movement_velocity[1] = from->chrManipulator.movement_velocity[1];
    to->movement_velocity[2] = from->chrManipulator.movement_velocity[2];
    to->movement_velocity[3] = from->chrManipulator.movement_velocity[3];

    to->r1_weapon_attack_input = from->chrManipulator.CurrentFrame_ActionInputs.r1_weapon_attack_input_1;
    to->l1_input = from->chrManipulator.CurrentFrame_ActionInputs.l1_input;
    to->r1_magic_attack_input = from->chrManipulator.CurrentFrame_ActionInputs.r1_magic_attack_input;
    to->l1_magic_attack_input = from->chrManipulator.CurrentFrame_ActionInputs.l1_magic_attack_input;
    to->r2_input = from->chrManipulator.CurrentFrame_ActionInputs.r2_input;
    to->backstep_input = from->chrManipulator.CurrentFrame_ActionInputs.backstep_input;
    to->use_button_pressed = from->chrManipulator.CurrentFrame_ActionInputs.use_ButtonPressed;
    to->roll_forward_input = from->chrManipulator.CurrentFrame_ActionInputs.roll_forward_input;
    to->lefthand_weapon_attack = from->chrManipulator.CurrentFrame_ActionInputs.lefthand_weapon_attack;
    to->parry_input = from->chrManipulator.CurrentFrame_ActionInputs.parry_input;
    to->block_input = from->chrManipulator.CurrentFrame_ActionInputs.block_input;
    to->jump_input = from->chrManipulator.CurrentFrame_ActionInputs.jump_input;
    to->l1_weapon_attack = from->chrManipulator.CurrentFrame_ActionInputs.l1_weapon_attack;
    to->l2_weapon_attack = from->chrManipulator.CurrentFrame_ActionInputs.l2_weapon_attack;

    to->change_2handing_state = from->chrManipulator.change_2handing_state;
    to->left_hand_slot_selected = from->chrManipulator.left_hand_slot_selected;
    to->right_hand_slot_selected = from->chrManipulator.right_hand_slot_selected;
    to->isSprintingAnim = from->chrManipulator.isSprintingAnim;
    to->LockonTargetHandle = from->chrManipulator.LockonTargetHandle;
    to->movement_related_flags = from->movement_related_flags;
    to->not_getting_movement_input = from->not_getting_movement_input;
    to->y_movement_input = from->y_movement_input[from->cur_movement_input_index_to_use];
    to->x_movement_input = from->x_movement_input[from->cur_movement_input_index_to_use];
    to->cur_movement_input_index_to_use = from->cur_movement_input_index_to_use;
}

void PadManipulatorPacked_to_PadManipulator(PlayerIns* target, PadManipulatorPacked* from, bool networkedPc)
{
    PadManipulator* to = target->chrins.padManipulator;

    to->chrManipulator.camera_x_direction_movement_input_amount = from->camera_x_direction_movement_input_amount;
    to->chrManipulator.camera_y_direction_movement_input_amount = from->camera_y_direction_movement_input_amount;
    to->chrManipulator.continuous_weapon_controlled_angle = from->continuous_weapon_controlled_angle;
    to->chrManipulator.delta_pc_rotation_perframe = from->delta_pc_rotation_perframe;
    to->chrManipulator.weapon_controlled_angle = from->weapon_controlled_angle;
    to->chrManipulator.pc_rotation = from->pc_rotation;
    *(float*)((uint64_t)(target->chrins.playerCtrl->chrCtrl.havokChara) + 4) = from->pc_rotation; //need to write this here since it's the "real" rotation
    to->chrManipulator.delta_camera_y_rotation_perframe = from->delta_camera_y_rotation_perframe;
    to->chrManipulator.delta_camera_x_rotation_perframe = from->delta_camera_x_rotation_perframe;
    to->chrManipulator.camera_y_rotation = from->camera_y_rotation;
    to->chrManipulator.camera_x_rotation = from->camera_x_rotation;
    to->chrManipulator.camera_x_direction_movement_input_amount_alt = from->camera_x_direction_movement_input_amount_alt;
    to->chrManipulator.camera_y_direction_movement_input_amount_alt = from->camera_y_direction_movement_input_amount_alt;
    to->chrManipulator.movement_velocity[0] = from->movement_velocity[0];
    to->chrManipulator.movement_velocity[1] = from->movement_velocity[1]; 
    to->chrManipulator.movement_velocity[2] = from->movement_velocity[2]; 
    to->chrManipulator.movement_velocity[3] = from->movement_velocity[3];

    //move the CurrentFrame_ActionInputs to the PrevFrame_ActionInputs
    if (networkedPc)
    {
        memcpy(&to->chrManipulator.PrevFrame_ActionInputs, &to->chrManipulator.CurrentFrame_ActionInputs, sizeof(ChrManipulator_ActionInputted));
    }

    to->chrManipulator.CurrentFrame_ActionInputs.r1_weapon_attack_input_1 = from->r1_weapon_attack_input;
    to->chrManipulator.CurrentFrame_ActionInputs.l1_input = from->l1_input;
    to->chrManipulator.CurrentFrame_ActionInputs.r1_magic_attack_input = from->r1_magic_attack_input;
    to->chrManipulator.CurrentFrame_ActionInputs.l1_magic_attack_input = from->l1_magic_attack_input;
    to->chrManipulator.CurrentFrame_ActionInputs.r2_input = from->r2_input;
    to->chrManipulator.CurrentFrame_ActionInputs.backstep_input = from->backstep_input;
    to->chrManipulator.CurrentFrame_ActionInputs.use_ButtonPressed = from->use_button_pressed;
    to->chrManipulator.CurrentFrame_ActionInputs.roll_forward_input = from->roll_forward_input;
    to->chrManipulator.CurrentFrame_ActionInputs.lefthand_weapon_attack = from->lefthand_weapon_attack;
    to->chrManipulator.CurrentFrame_ActionInputs.parry_input = from->parry_input;
    to->chrManipulator.CurrentFrame_ActionInputs.block_input = from->block_input;
    to->chrManipulator.CurrentFrame_ActionInputs.jump_input = from->jump_input;
    to->chrManipulator.CurrentFrame_ActionInputs.l1_weapon_attack = from->l1_weapon_attack;
    to->chrManipulator.CurrentFrame_ActionInputs.l2_weapon_attack = from->l2_weapon_attack;

    //update the ActionInputtedTimeHeld and AnyActionInputted
    if (networkedPc)
    {
        to->chrManipulator.AnyActionInputted = false;
        for (size_t i = 0; i < sizeof(ChrManipulator_ActionInputted); i++)
        {
            bool* buttonPressed = (bool*)((uint64_t)(&to->chrManipulator.CurrentFrame_ActionInputs) + i);
            float* timeHeld = (float*)((uint64_t)(&to->chrManipulator.ActionInputtedTimeHeld) + i * 4);

            if (*buttonPressed)
            {
                *timeHeld += FRAMETIME;
                to->chrManipulator.AnyActionInputted |= true;
            }
            else
            {
                *timeHeld = 0.0f;
            }
        }
    }

    to->chrManipulator.change_2handing_state = from->change_2handing_state;
    to->chrManipulator.left_hand_slot_selected = from->left_hand_slot_selected;
    to->chrManipulator.right_hand_slot_selected = from->right_hand_slot_selected;
    to->chrManipulator.isSprintingAnim = from->isSprintingAnim;
    to->chrManipulator.LockonTargetHandle = from->LockonTargetHandle;
    //CurrentFrame_ActionInputs_ButtonId
    to->movement_related_flags = from->movement_related_flags;
    to->not_getting_movement_input = from->not_getting_movement_input;
    //DashInputTimer
    to->y_movement_input[from->cur_movement_input_index_to_use] = from->y_movement_input;
    to->x_movement_input[from->cur_movement_input_index_to_use] = from->x_movement_input;
    to->cur_movement_input_index_to_use = from->cur_movement_input_index_to_use;
    //time_spend_forward_strafing
    //time_spend_back_strafing
    //time_spend_left_strafing
    //time_spend_right_strafing
    //camera_xaxis_rotation
    //Backstep_timer
    //EnableBackStep
    //EnableBackStep_forward
    //x_movement_input_reversed
    //y_movement_input_reversed
    //jump_trigger_time_remaining
}

void copy_PadManipulator(PadManipulator* to, PadManipulator* from)
{
    memcpy(to, from, sizeof(PadManipulator));
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
