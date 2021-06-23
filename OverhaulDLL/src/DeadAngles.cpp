#include "DeadAngles.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#define _USE_MATH_DEFINES
#include <math.h>

extern "C" {
    uint64_t main_dead_angle_injection_return;
    void main_dead_angle_injection();
    uint64_t main_dead_angle_helper(uint64_t, uint64_t);
}

void DeadAngles::start() {
    ConsoleWrite("Enabling Dead Angles...");

    uint8_t *write_address = (uint8_t*)(DeadAngles::main_dead_angle_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &main_dead_angle_injection_return, 2, &main_dead_angle_injection);
}

//The default angle shields block is 90 degrees (90 on the left and 90 on the right, so 180 total)
//This can be used to reduce shield blocking angle as well
//Bring it down a little
static const float dead_angle_radians = (float)(80.0 * (M_PI/180.0));

uint64_t main_dead_angle_helper(uint64_t attacker, uint64_t target)
{
    auto target_rotation = Game::get_entity_rotation((void*)target);
    auto attacker_rotation = Game::get_entity_rotation((void*)attacker);
    if (!target_rotation.has_value() ||! attacker_rotation.has_value())
    {
        ConsoleWrite("WARNING: Unable to get players rotation values");
        return 1;
    }

    //normalize both angles from 0 to 2PI
    float target_heading = (float)(target_rotation.value() + M_PI);
    float attacker_heading = (float)(attacker_rotation.value() + M_PI);

    float angle_diff;
    //Compute the angle difference between the two char headings.
    if (attacker_heading >= target_heading) {
        angle_diff = (float)abs((attacker_heading - M_PI) - target_heading);
    }
    else {
        angle_diff = (float)abs((target_heading - M_PI) - attacker_heading);
    }

    //sprintf_s(pout, 200, "Atk=%f Tgt=%f Angle=%f\n", attacker_heading, target_heading, angle_diff);
    //global::cmd_out << pout;

    // If attack is > X degrees of target's heading, dead angle
    if (angle_diff > dead_angle_radians) {
        //global::cmd_out << "Dead Angle\n";
        return 0;
    }
    //tell asm not to do anything
    else {
        return 1;
    }
}
