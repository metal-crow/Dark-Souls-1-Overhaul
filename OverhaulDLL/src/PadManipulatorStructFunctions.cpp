#include "PadManipulatorStructFunctions.h"
#include "Rollback.h"

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
    to->y_movement_input = from->y_movement_input[0];
    //debugging
    if (from->y_movement_input[0] != from->y_movement_input[1] ||
        from->y_movement_input[0] != from->y_movement_input[2] ||
        from->y_movement_input[0] != from->y_movement_input[3] ||
        from->y_movement_input[0] != from->y_movement_input[4] ||
        from->y_movement_input[0] != from->y_movement_input[5])
    {
        FATALERROR("y_movement_input not all the same");
    }
    to->x_movement_input = from->x_movement_input[0];
    //debugging
    if (from->x_movement_input[0] != from->x_movement_input[1] ||
        from->x_movement_input[0] != from->x_movement_input[2] ||
        from->x_movement_input[0] != from->x_movement_input[3] ||
        from->x_movement_input[0] != from->x_movement_input[4] ||
        from->x_movement_input[0] != from->x_movement_input[5])
    {
        FATALERROR("x_movement_input not all the same");
    }
    to->cur_movement_input_index_to_use = from->cur_movement_input_index_to_use;
}

void PadManipulatorPacked_to_PadManipulator(PadManipulator* to, PadManipulatorPacked* from, bool networkedPc)
{
    to->chrManipulator.camera_x_direction_movement_input_amount = from->camera_x_direction_movement_input_amount;
    to->chrManipulator.camera_y_direction_movement_input_amount = from->camera_y_direction_movement_input_amount;
    to->chrManipulator.continuous_weapon_controlled_angle = from->continuous_weapon_controlled_angle;
    to->chrManipulator.delta_pc_rotation_perframe = from->delta_pc_rotation_perframe;
    to->chrManipulator.weapon_controlled_angle = from->weapon_controlled_angle;
    to->chrManipulator.pc_rotation = from->pc_rotation;
    to->chrManipulator.delta_camera_y_rotation_perframe = from->delta_camera_y_rotation_perframe;
    to->chrManipulator.delta_camera_x_rotation_perframe = from->delta_camera_x_rotation_perframe;
    to->chrManipulator.camera_y_rotation = from->camera_y_rotation;
    to->chrManipulator.camera_x_rotation = from->camera_x_rotation;
    to->chrManipulator.camera_x_direction_movement_input_amount_alt = from->camera_x_direction_movement_input_amount;
    to->chrManipulator.camera_y_direction_movement_input_amount_alt = from->camera_y_direction_movement_input_amount;
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
    to->y_movement_input[0] = from->y_movement_input;
    to->y_movement_input[1] = from->y_movement_input;
    to->y_movement_input[2] = from->y_movement_input;
    to->y_movement_input[3] = from->y_movement_input;
    to->y_movement_input[4] = from->y_movement_input;
    to->y_movement_input[5] = from->y_movement_input;
    to->x_movement_input[0] = from->x_movement_input;
    to->x_movement_input[1] = from->x_movement_input;
    to->x_movement_input[2] = from->x_movement_input;
    to->x_movement_input[3] = from->x_movement_input;
    to->x_movement_input[4] = from->x_movement_input;
    to->x_movement_input[5] = from->x_movement_input;
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
    memcpy(to, from, sizeof(to));
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
