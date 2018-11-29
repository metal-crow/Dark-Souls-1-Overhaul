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
    uint64_t c0000_esd_data_buffer;
}

//each spell animation id and it's 
static const uint32_t SPELL_AIDS_TO_ALLOW_MOVEMENT[] = {
    6202, 6204, 6205, 6206, 6208, 6210, 6215, 6217, 6218, 6219, 6224,
    6302, 6304, 6305, 6306, 6308, 6310, 6315, 6317, 6318, 6319, 6324,
    6402, 6404, 6406, 6408, 6410, 6411, 6417, 6418, 6419, 6424, 6426,
    6500, 6502, 6504, 6506, 6508, 6510, 6511, 6517, 6518, 6519, 6524, 6526
};

typedef struct {
    uint8_t function_id;
    uint32_t function_arg;
} Animation_Dataval_Entry_OneArg;

typedef struct {
    uint8_t function_id;
} Animation_Dataval_Entry_NoArg;

typedef struct {
    void *function_list; //non-homogenious array of mixed Animation_Dataval_Entry_NoArg and Animation_Dataval_Entry_OneArg structs
    uint64_t datavals_list_len;
} Animation_Datavalues;

typedef struct {
    uint32_t parsing_type; //pretty much always 1
    uint32_t category; //the type of the animation. 1 is standard (can move during), 3 is locked (can't move during).
    Animation_Datavalues *datavalues_list;
    uint64_t list_len;
} Animation_EzState;

void CastingMovement::start() {
    global::cmd_out << Mod::output_prefix << ("    Enabling casting while moving patch...\n");
    Mod::startup_messages.push_back("    Enabling casting while moving patch...");

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

    //Hook into the function that copies the parsed c0000.esd file to the final buffer, and get the buffer addr
    write_address = (uint8_t*)(CastingMovement::c0000_esd_reader + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &c0000_esd_reader_return, 0, &c0000_esd_reader_injection);

    //Disable the TAE events for disabling rotation during spell casting
    /*int aids_changed = 0;
    if (Game::characters_loaded && Game::player_tae.is_initialized())
    {
        for (int i = 0; i < sizeof(SPELL_AIDS_TO_ALLOW_MOVEMENT) / sizeof(std::tuple<uint32_t, float>); i++)
        {
            uint32_t aid = std::get<0>(SPELL_AIDS_TO_ALLOW_MOVEMENT[i]);

            int n_events = Game::player_tae.get_event_count_by_id(aid);
            for (int i = 0; i < n_events; i++) {
                if (Game::player_tae.get_event_type_by_id(aid, i) == 0 && Game::player_tae.get_event_param_by_id(aid, i, 0) == TAE_type0_param_values::lock_rotation) {
                    Game::player_tae.set_event_start_by_id(aid, i, 0.0f);
                    Game::player_tae.set_event_end_by_id(aid, i, 0.0f);
                    //print_console("Updated gesture " + std::to_string(aid));
                }
            }
        }
    }*/
}

void CastingMovement::on_char_load() {
    //Alter the spell animation ezstates to allow movement during
    //All the offsets are pts to Animation_EzState[2], where the 1st is of category 0x76, and the second is category 3 (no walk)

    //Spells
    //6400/6500, offset 0x29DA0
    //6424/6524, offset 0x29e90
    //6401/6501, offset 0x2a610
    //6416/6516, offset 0x29620
    //6402/6502, offset 0x2a700
    //6419/6519, offset 0x299e0
    //6411/6511, offset 0x29290

    //Pyro
    //6404/6502, offset 0x2a520
    //Do not fix 6409/6509, offset 0x2a070. Firestorm
    //6426/6526, offset 0x29f80
    //Do not fix 6407/6507, offset 0x2a250. Combustion
    //6408/6508, offset 0x2a160
    //6405/6505, offset 0x2a430
    //6410/6510, offset 0x29ad0

    //Miracles
    //6418/6518, offset 0x298f0
    //6414/6514, offset 0x29bc0
}
