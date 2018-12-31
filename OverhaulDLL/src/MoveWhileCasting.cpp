/*
    DARK SOULS OVERHAUL

    Contributors to this file:
    Metal-Crow    -    Reverse engineering, C++

*/

#include "MoveWhileCasting.h"
#include "GameData.h"
#include "SP/memory/injection/asm/x64.h"

extern "C" {
    uint64_t c0000_esd_reader_return;
    void c0000_esd_reader_injection();
    uint64_t c0000_esd_data_buffer = 0;
}

static const uint32_t SPELL_AIDS_TO_ALLOW_MOVEMENT[] = {
    //overhead cast
    6400, 6500,
    //hold overhead cast
    6424, 6524,
    //buff rh wep
    6401,
    //
    6416, 6516,
    //wave over body
    6402, 6502,
    //homing soul mass type spell
    6419, 6519,
    //wave over body
    6411, 6511,

    //pyro ball throw
    6404, 6502,
    //fire whip
    6426, 6526,
    //pyro breath
    6408, 6508,
    //iron flesh
    6405, 6505,
    //undead rapport
    6410, 6510,

    //lighting spear
    6418, 6518,
    //buff weapon
    6414, 6514,
};



//The spells ezstate offset's in the c0000 ezstate buffer, commented with the corresponding animation id
static const uint64_t SPELL_OFFSETS_TO_ALLOW_MOVEMENT[] = {
    //Spells
    //6400/6500
    0x29DA0,
    //6424/6524
    0x29e90,
    //6401
    0x2a610,
    //6416/6516
    0x29620,
    //6402/6502
    0x2a700,
    //6419/6519
    0x299e0,
    //6411/6511
    0x29290,

    //Pyro
    //6404/6502
    0x2a520,
    //Do not fix 6409/6509 Firestorm
    //0x2a070,
    //6426/6526
    0x29f80,
    //Do not fix 6407/6507 Combustion
    //0x2a250,
    //6408/6508
    0x2a160,
    //6405/6505
    0x2a430,
    //6410/6510
    0x29ad0,

    //Miracles
    //6418/6518
    0x298f0,
    //6414/6514
    0x29bc0
};

typedef struct {
    uint8_t *bytecode;
    uint64_t bytecode_len;
} Animation_Datavalues;

typedef struct {
    uint32_t parsing_type; //pretty much always 1
    uint32_t category; //the type of the animation. 1 is standard (can move during), 3 is locked (can't move during).
    Animation_Datavalues *datavalues_list;
    uint64_t list_len;
} Animation_EzState;

void CastingMovement::early_inits() {
    global::cmd_out << Mod::output_prefix << ("Injecting ESD locator...\n");
    Mod::startup_messages.push_back("Injecting ESD locator...");

    //Hook into the function that copies the parsed c0000.esd file to the final buffer, and get the buffer addr
    //This is hit VERY early (before splash screens), so needs to be loaded fast
    uint8_t *write_address = (uint8_t*)(CastingMovement::c0000_esd_reader + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &c0000_esd_reader_return, 0, &c0000_esd_reader_injection);
}

void CastingMovement::start() {
    global::cmd_out << Mod::output_prefix << ("Enabling casting while moving patch...\n");
    Mod::startup_messages.push_back("Enabling casting while moving patch...");

    //Fix one of the checks that prevents WalkFB aid from being set
    uint8_t nop_patch[] = { 0x90, 0x90 };
    uint8_t *write_address = (uint8_t*)(CastingMovement::walkfb_check_1 + Game::ds1_base);
    sp::mem::patch_bytes(write_address, nop_patch, sizeof(nop_patch));

    //Fix one of the checks that blocks WalkFb by setting a variable to 0
    //Do via inline replace cmp with mov ebx (ebx is always 0, actually. See the below cmp instruction), and jmp with unconditional
    write_address = (uint8_t*)(CastingMovement::walkfb_check_2 + Game::ds1_base);
    uint8_t patch2[] = { 0x89, 0x9F, 0x40, 0x01, 0x00, 0x00, 0x90, 0xEB, 0x0C };
    sp::mem::patch_bytes(write_address, patch2, sizeof(patch2));

    //Fix one of the checks that blocks WalkFb by setting a variable to 0
    //Do via inline XOR and patch to uncond jump, instead of making code cave
    uint8_t patch3[] = { 0x31, 0xD2, 0xEB, 0x1C };
    write_address = (uint8_t*)(CastingMovement::walkfb_check_3 + Game::ds1_base);
    sp::mem::patch_bytes(write_address, patch3, sizeof(patch3));

    //Disable the TAE events for disabling rotation during spell casting
    int aids_changed = 0;
    if (Game::player_tae.is_initialized())
    {
        for (uint32_t aid : SPELL_AIDS_TO_ALLOW_MOVEMENT)
        {
            int n_events = Game::player_tae.get_event_count_by_id(aid);
            //global::cmd_out << std::to_string(n_events) << " events for aid " << std::to_string(aid) << "\n";
            for (int i = 0; i < n_events; i++) {
                if (Game::player_tae.get_event_type_by_id(aid, i) == 0 && Game::player_tae.get_event_param_by_id(aid, i, 0) == TAE_type0_param_values::lock_rotation) {
                    Game::player_tae.set_event_start_by_id(aid, i, 0.0f);
                    Game::player_tae.set_event_end_by_id(aid, i, 0.0f);
                    global::cmd_out << "Updated movement animation " << std::to_string(aid) << "\n";
                }
            }
        }
    }
    global::cmd_out << "Movement animations updated\n";
}

void CastingMovement::on_char_load() {
    //Alter the spell animation ezstates to allow movement during
    //All the offsets are pts to Animation_EzState[2], where the 1st is of category 0x76, and the second is category 3 (no walk)
    if (c0000_esd_data_buffer != 0) {
        char buf[50];
        snprintf(buf, sizeof(buf), "ESD data buffer is at %llx\n", c0000_esd_data_buffer);
        global::cmd_out << buf;
    }
    else {
        FATALERROR((Mod::output_prefix + "!!ERROR!! c0000 ezstate buffer not found.\n").c_str());
    }
    for (uint64_t offset : SPELL_OFFSETS_TO_ALLOW_MOVEMENT) {
        Animation_EzState* entry = (Animation_EzState*)((uint64_t)(c0000_esd_data_buffer + offset + sizeof(Animation_EzState)));

        //change category to 1
        entry->category = 1;

        //swap the 0th and 1th dataval pointers
        uint8_t* zero_ptr = entry->datavalues_list[0].bytecode;
        uint64_t zero_len = (uint64_t)entry->datavalues_list[0].bytecode_len;
        entry->datavalues_list[0].bytecode = entry->datavalues_list[1].bytecode;
        entry->datavalues_list[0].bytecode_len = entry->datavalues_list[1].bytecode_len;
        entry->datavalues_list[1].bytecode = zero_ptr;
        entry->datavalues_list[1].bytecode_len = zero_len;

        //Set datavals 1-4 to be correct now that it's movable.
        if (entry->list_len != 5) {
            FATALERROR((Mod::output_prefix + "!!ERROR!! Tried to change the EzState of an animation with an unexpected structure (Not 5 datavals)."
                                             "Occured for animation at offset 0x%x\n").c_str(), offset);
        }
        entry->datavalues_list[1].bytecode[0] = 64 + 5; //Change from (int 2) to (int 5)
        //swap #2 (int 6) with #3 (double 0.1)
        uint8_t* two_ptr = entry->datavalues_list[2].bytecode;
        uint64_t two_len = (uint64_t)entry->datavalues_list[0].bytecode_len;
        entry->datavalues_list[2].bytecode = entry->datavalues_list[3].bytecode;
        entry->datavalues_list[2].bytecode_len = entry->datavalues_list[3].bytecode_len;
        entry->datavalues_list[3].bytecode = two_ptr;
        entry->datavalues_list[3].bytecode_len = two_len;
        entry->datavalues_list[3].bytecode[0] = 64; //Change from (int 6) to (int 0)
        entry->datavalues_list[4].bytecode[0] = 64 + 2; //Change from (int 0) to (int 2)
    }
}
