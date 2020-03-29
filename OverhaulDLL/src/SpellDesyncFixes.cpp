#include "SpellDesyncFixes.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t homing_spell_trigger_injection_return;
    void homing_spell_trigger_injection();
    uint64_t world_char_base_spelldesync_asm;
}

void SpellDesync::start() {
    global::cmd_out << Mod::output_prefix << "Fixing Spell Desync...\n";

    world_char_base_spelldesync_asm = Game::world_char_base;

    uint8_t *write_address = (uint8_t*)(SpellDesync::homing_spell_trigger_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &homing_spell_trigger_injection_return, 1, &homing_spell_trigger_injection);
}
