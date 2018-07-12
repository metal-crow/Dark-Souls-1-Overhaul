#include "DeadAngles.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#define _USE_MATH_DEFINES
#include <math.h>

extern "C" {
    uint64_t main_dead_angle_injection_return;
    void main_dead_angle_injection();
    uint32_t main_dead_angle_helper(uint64_t, uint64_t);
}

void DeadAngles::start() {
    global::cmd_out << Mod::output_prefix << "Enabling Dead Angles...\n";
    Mod::startup_messages.push_back("Enabling Dead Angles...");

    uint8_t *write_address = (uint8_t*)(DeadAngles::main_dead_angle_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &main_dead_angle_injection_return, 2, &main_dead_angle_injection, true);
}

static const float dead_angle_degree = 60.0;

uint32_t main_dead_angle_helper(uint64_t attacker, uint64_t target) {
    //These are tracked as PI to -PI
    float target_heading = *((uint64_t*)(*((uint64_t*)(*((uint64_t*)(target + 0x48)) + 0x28)) + 0x4));
    float attacker_heading_ajusted = *((uint64_t*)(*((uint64_t*)(*((uint64_t*)(attacker + 0x48)) + 0x28)) + 0x4)) + M_PI; //Adjust by adding 180 degrees to account for faceing
    float angle_diff = abs(attacker_heading_ajusted - target_heading);

    // If attack is < X degrees of target's heading, ignored
    if (angle_diff < dead_angle_degree) {
        return 0;
    }

    //TODO how do i determine damage?
    global::cmd_out << "Dead angle";
}
