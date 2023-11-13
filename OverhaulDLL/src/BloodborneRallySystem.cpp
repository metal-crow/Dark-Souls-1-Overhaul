/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow    -    Reverse engineering, Bloodborne rally system, C++

*/

#include "BloodborneRallySystem.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include "MainLoop.h"

const uint64_t MAX_RALLY_RECOVERY_TIME_MS = 5000;

// HP of the player before last hit
uint32_t beforehit_hp = UINT16_MAX;

// Tracker for the number of hits the user has received
uint64_t gothit = 0;

// Time (in milliseonds) player was last hit
uint64_t beforehit_time = 0;

extern "C" {
    uint64_t control_timer_injection_return;
    void control_timer_injection();
    uint64_t control_timer_function(uint64_t, uint64_t);

    uint64_t main_rally_injection_return;
    void main_rally_injection();
    void main_rally_function(uint64_t, uint64_t, uint64_t, uint64_t);

    void set_rally_regain_sfx();
    uint32_t RALLY_EFFECT_ID = 92000; //hp regain sfx
    void set_rally_sfx_rhand();
    uint32_t RALLY_CAPABLE_WEAPON_EFFECT_ID_RHAND = 92001; //weapon can perform rally indicator
    void set_rally_sfx_lhand();
    uint32_t RALLY_CAPABLE_WEAPON_EFFECT_ID_LHAND = 92002; //weapon can perform rally indicator

    uint64_t lua_SetEventSpecialEffect_2 = 0x1404ad880;
}

bool Apply_rally_capable_sfx_and_starting_hp(void*);
bool Check_weapon_swap_for_rally(void*);

void BloodborneRally::start() {
    ConsoleWrite("Setting up Bloodborne Rally System...");

    // Create callback to clear rally on weapon toggle
    MainLoop::setup_mainloop_callback(Check_weapon_swap_for_rally, NULL, "Check_weapon_swap_for_rally");

    // Inject function to control the timer for the current ui bar
    uint8_t* write_address = (uint8_t*)(BloodborneRally::control_timer_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &control_timer_injection_return, 1, &control_timer_injection);

    // Inject function to perform the main rally code
    write_address = (uint8_t*)(BloodborneRally::main_rally_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &main_rally_injection_return, 2, &main_rally_injection, true);
    main_rally_injection_return = 0x140322a98; //use as the jmp we're overwriting

    // Start new callback dedicated to applying rally-capable weapon sfx and setting the starting HP
    MainLoop::setup_mainloop_callback(Apply_rally_capable_sfx_and_starting_hp, NULL, "Apply_rally_capable_sfx_and_starting_hp");
}

static uint32_t equipped_wep_r = 0;
static uint32_t equipped_wep_l = 0;
bool Check_weapon_swap_for_rally(void* unused)
{
    auto pc_o = Game::get_PlayerIns();
    if (!pc_o.has_value() || pc_o.value() == NULL)
    {
        return true;
    }
    uint64_t pc = (uint64_t)(pc_o.value());

    uint32_t new_equipped_wep_r = Game::get_equipped_inventory(pc, Game::return_weaponslot_in_hand(pc, true));
    uint32_t new_equipped_wep_l = Game::get_equipped_inventory(pc, Game::return_weaponslot_in_hand(pc, false));

    //weapon changed
    if (equipped_wep_r != new_equipped_wep_r || equipped_wep_l != new_equipped_wep_l)
    {
        equipped_wep_r = new_equipped_wep_r;
        equipped_wep_l = new_equipped_wep_l;
        beforehit_time = 0; //Fake we got hit infinite time ago, to drop rally
    }

    return true;
}

uint64_t control_timer_function(uint64_t bar_id, uint64_t orange_bar) {
    //0x24 is the orange bar hidden behind the HP bar.
    //Only adjust the bar when feature enabled.
    if (bar_id == 0x24 && Mod::get_mode() == ModMode::Overhaul) {
        uint32_t curtime = 0;
        if (Game::get_game_time_ms().has_value())
        {
            curtime = *Game::get_game_time_ms().value();
        }
        //not yet time to drop orange bar
        if (beforehit_time + MAX_RALLY_RECOVERY_TIME_MS > curtime && gothit==1) {
            return 1;
        }
        //drop orange bar
        if (beforehit_time + MAX_RALLY_RECOVERY_TIME_MS <= curtime && gothit == 1) {
            gothit = 0;
            *((float*)(orange_bar + 0x60)) = 0;
            return 1;
        }
        //got hit before previous timer went down. partially drop orange bar
        if (gothit == 2) {
            gothit = 1;
            float new_orange_bar = (float)beforehit_hp / (float)Game::get_player_char_max_hp().value_or(Game::current_hpbar_max);
            *((float*)(orange_bar + 0x60)) = new_orange_bar;
            return 1;
        }
    }
    return 0;
}

#define DARKHAND_ID 0xDCB40
#define PRISCILLADAGGER_ID 0x19640
#define VELKASRAPIER_ID 0x93378

static bool isOccult(uint32_t weaponid) {
    switch (weaponid) {
        case DARKHAND_ID:
        case PRISCILLADAGGER_ID + 0:
        case PRISCILLADAGGER_ID + 1:
        case PRISCILLADAGGER_ID + 2:
        case PRISCILLADAGGER_ID + 3:
        case PRISCILLADAGGER_ID + 4:
        case PRISCILLADAGGER_ID + 5:
        case VELKASRAPIER_ID + 0:
        case VELKASRAPIER_ID + 1:
        case VELKASRAPIER_ID + 2:
        case VELKASRAPIER_ID + 3:
        case VELKASRAPIER_ID + 4:
        case VELKASRAPIER_ID + 5:
            return true;
        default:
            //check if hundreds place == 7
            return ((weaponid / 100) % 10) == 7;
    }
}

void main_rally_function(uint64_t attacker, uint64_t target, uint64_t attack_data, uint64_t new_hp) {
    //Don't do anything if feature disabled
    if (Mod::get_mode() != ModMode::Overhaul) {
        return;
    }

    uint64_t pc_entity_ptr = Game::get_pc_entity_pointer().value_or(NULL);
    uint32_t game_time = 0;
    if (Game::get_game_time_ms().has_value())
    {
        game_time = *Game::get_game_time_ms().value();
    }

    //if target is host
    if (target == pc_entity_ptr) {
        //Save current time and player HP
        uint32_t curhp = *(uint32_t*)(target + 0x3E8);
        //This sometimes gets called multiple times for 1 hit, so check
        if (curhp == new_hp) return;
        beforehit_hp = curhp;
        beforehit_time = game_time;
        //Marker for getting hit (used for UI)
        gothit++;
        if (gothit <= 2) {
            return;
        }
        gothit = 2;
        return;
    }

    //if attacker is host
    if (attacker == pc_entity_ptr) {
        if (game_time - beforehit_time < MAX_RALLY_RECOVERY_TIME_MS) {
            //check what weapon is used in attack (check hand in weapon_data)
            int32_t weapon_hand = *(int32_t*)(attack_data + 0x4A4);

            uint32_t weaponid;
            //R hand
            if (weapon_hand == -1) {
                weaponid = Game::right_hand_weapon().value_or(-1);
            }
            //L hand
            else if (weapon_hand == -2) {
                weaponid = Game::left_hand_weapon().value_or(-1);
            }
            else {
                return;
            }

            if (isOccult(weaponid)) {
                uint32_t curhp = *(uint32_t*)(attacker + 0x3E8);
                uint32_t maxhp = *(uint32_t*)(attacker + 0x3EC);
                uint32_t damage = beforehit_hp - curhp;

                uint32_t upgrade = weaponid % 100;
                float scale = 0.05f + (upgrade / 10.0f);
                uint32_t recovery = (uint32_t)((float)damage * scale);

                uint32_t rally_post_hp = min(maxhp, curhp + recovery);
                *(uint32_t*)(attacker + 0x3E8) = rally_post_hp;

                set_rally_regain_sfx();

                return;
            }
        }
    }

    return;
}

bool Apply_rally_capable_sfx_and_starting_hp(void* unused) {
    //only apply the rally sfx if character is loaded
    bool loaded = Game::playerchar_is_loaded();

    if (!loaded)
    {
        //prevent first hit having a beforehit hp of zero
        beforehit_hp = UINT16_MAX;
    }

    //don't apply sfx if feature disabled
    if (loaded && Mod::get_mode() == ModMode::Overhaul) {
        uint32_t weaponid_R = Game::right_hand_weapon().value_or(-1);
        uint32_t weaponid_L = Game::left_hand_weapon().value_or(-1);

        //There appears to be some bug here where both the effects cannot be applied simultaneously
        //But it just alternates which is applied, so it works ok
        if (isOccult(weaponid_R))
        {
            set_rally_sfx_rhand();
        }

        if (isOccult(weaponid_L))
        {
            set_rally_sfx_lhand();
        }
    }

    return true;
}
