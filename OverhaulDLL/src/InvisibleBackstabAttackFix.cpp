#include "InvisibleBackstabAttackFix.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t send_throw_packet_injection_return;
    void send_throw_packet_injection();
    uint8_t force_sending_ezstate_packet_flag = 0;
    uint64_t force_sent_anim_packet_to_backstab_injection_return;
    void force_sent_anim_packet_to_backstab_injection();
}

void InvisBackstabFix::start() {
    global::cmd_out << Mod::output_prefix << "Fixing Invisible Backstab attack bug...\n";

    //Forcably send a packet containing the attacker's ezstate info after the throw packet.
    uint8_t *write_address = (uint8_t*)(InvisBackstabFix::send_throw_packet_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &send_throw_packet_injection_return, 5, &send_throw_packet_injection);

    //force that the ezstate info packet has the "backstabbing" ezstate id
    write_address = (uint8_t*)(InvisBackstabFix::force_sent_anim_packet_to_backstab_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &force_sent_anim_packet_to_backstab_injection_return, 1, &force_sent_anim_packet_to_backstab_injection);
}
