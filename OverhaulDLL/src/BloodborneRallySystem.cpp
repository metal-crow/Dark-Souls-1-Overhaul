/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Metal-Crow    -    Reverse engineering, Bloodborne rally system, C++

*/

#include "BloodborneRallySystem.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"

#define DS1_BB_RALLY_SYS_OCCULT_MAT_ID 700
const uint64_t MAX_RALLY_RECOVERY_TIME_MS = 5000;

// HP of the player before last hit
uint32_t  beforehit_hp = UINT16_MAX;

// Tracker for the number of hits the user has received
uint64_t  gothit = 0;

extern "C" {
    // Time (in milliseonds) player was last hit
    uint64_t  beforehit_time = 0;

    uint64_t weapon_toggle_injection_return;
    void weapon_toggle_injection();

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

    uint64_t lua_SetEventSpecialEffect_2 = 0x1404a6160;
}

void BloodborneRally::start() {
    global::cmd_out << Mod::output_prefix << "Enabling Bloodborne Rally System...\n";
    Mod::startup_messages.push_back("Enabling Bloodborne Rally System...");

    // Inject function to clear rally on weapon toggle
    uint8_t *write_address = (uint8_t*)(BloodborneRally::weapon_toggle_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &weapon_toggle_injection_return, 5, &weapon_toggle_injection);

    // Inject function to control the timer for the current ui bar
    write_address = (uint8_t*)(BloodborneRally::control_timer_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &control_timer_injection_return, 1, &control_timer_injection);

    // Inject function to perform the main rally code
    write_address = (uint8_t*)(BloodborneRally::main_rally_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &main_rally_injection_return, 2, &main_rally_injection, true);
    main_rally_injection_return = 0x140320848; //use as the jmp we're overwriting
}

static DWORD WINAPI Apply_rally_capable_sfx_and_starting_hp(void*);

void BloodborneRally::on_char_load() {
    // Start new thread dedicated to applying rally-capable weapon sfx and setting the starting HP
    CreateThread(NULL, 0, Apply_rally_capable_sfx_and_starting_hp, NULL, 0, NULL);
    //BloodborneRally::set_weapon_faith_requirements();
}

#if 0
void BloodborneRally::set_weapon_faith_requirements() {
    SetLastError(ERROR_SUCCESS);
    if (Params::Weapon().base == NULL) {
        // Weapon parameters not loaded yet
        print_console("ERROR: Failed to set faith requirements for Occult weapons (wait for Weapon params to load)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    // @TODO: Un-comment next line when final faith requirement values are determined
    //BloodborneRally::set_default_weapon_faith_requirements();
    for (int i = 0; i < (int)Params::Weapon().param_count; i++) {
        if (Params::Weapon().get(i)->materialSetId == DS1_BB_RALLY_SYS_OCCULT_MAT_ID) {
            // Set faith requirement for possible custom weapons (in case player is using custom game files)
            if (Params::Weapon().get(i)->properFaith <= 0) {
                // Sum of all base damage types (physical, lightning, fire, magic)
                int base_damage_sum = Params::Weapon().get(i)->attackBasePhysics + Params::Weapon().get(i)->attackBaseThunder + Params::Weapon().get(i)->attackBaseFire + Params::Weapon().get(i)->attackBaseMagic;
                // Faith requirement = (base_damage_sum / 12) + 3
                // @TODO: Un-comment next line when final faith requirement values are determined
                //Params::Weapon().get(i)->properFaith = 3 + (int)(base_damage_sum / 12);  // @TODO: Implement better algorithm to calculate a fair faith requirement for each weapon (should take weapon attack speed into consideration)
            }
        }
    }
}

// Sets faith requirements for the default weapons in the base (un-modded) game
void BloodborneRally::set_default_weapon_faith_requirements() {
    SetLastError(ERROR_SUCCESS);
    if (Params::Weapon().base == NULL) {
        // Weapon parameters not loaded yet
        global::cmd_out << ("ERROR: Failed to set faith requirements for Occult weapons (wait for Weapon params to load)\n");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return;
    }
    // Values have been hand-tweaked for balance purposes
    ((WeaponParam*)Params::Weapon().get_by_id(100700))->properFaith = 10;  // Occult Dagger
    ((WeaponParam*)Params::Weapon().get_by_id(101700))->properFaith = 10;  // Occult Parrying Dagger
    ((WeaponParam*)Params::Weapon().get_by_id(103700))->properFaith = 12;  // Occult Knife
    ((WeaponParam*)Params::Weapon().get_by_id(200700))->properFaith = 13;  // Occult Shortsword
    ((WeaponParam*)Params::Weapon().get_by_id(201700))->properFaith = 13;  // Occult Longsword
    ((WeaponParam*)Params::Weapon().get_by_id(202700))->properFaith = 13;  // Occult Broadsword
    ((WeaponParam*)Params::Weapon().get_by_id(204700))->properFaith = 16;  // Occult Balder Side Sword
    ((WeaponParam*)Params::Weapon().get_by_id(210700))->properFaith = 16;  // Occult Darksword
    ((WeaponParam*)Params::Weapon().get_by_id(212700))->properFaith = 6;   // Occult Straight Sword Hilt
    ((WeaponParam*)Params::Weapon().get_by_id(300700))->properFaith = 14;  // Occult Bastard Sword
    ((WeaponParam*)Params::Weapon().get_by_id(301700))->properFaith = 16;  // Occult Claymore
    ((WeaponParam*)Params::Weapon().get_by_id(303700))->properFaith = 15;  // Occult Flamberge
    ((WeaponParam*)Params::Weapon().get_by_id(350700))->properFaith = 13;  // Occult Zweihander
    ((WeaponParam*)Params::Weapon().get_by_id(351700))->properFaith = 13;  // Occult Greatsword
    ((WeaponParam*)Params::Weapon().get_by_id(352700))->properFaith = 12;  // Occult Demon Great Machete
    ((WeaponParam*)Params::Weapon().get_by_id(400700))->properFaith = 18;  // Occult Scimitar
    ((WeaponParam*)Params::Weapon().get_by_id(401700))->properFaith = 19;  // Occult Falchion
    ((WeaponParam*)Params::Weapon().get_by_id(402700))->properFaith = 16;  // Occult Shotel
    ((WeaponParam*)Params::Weapon().get_by_id(450700))->properFaith = 1;   // Occult Server
    ((WeaponParam*)Params::Weapon().get_by_id(451700))->properFaith = 17;  // Occult Murakumo
    ((WeaponParam*)Params::Weapon().get_by_id(500700))->properFaith = 18;  // Occult Uchigatana
    ((WeaponParam*)Params::Weapon().get_by_id(501700))->properFaith = 17;  // Occult Washing Pole
    ((WeaponParam*)Params::Weapon().get_by_id(502700))->properFaith = 18;  // Occult Iaito
    ((WeaponParam*)Params::Weapon().get_by_id(600700))->properFaith = 10;  // Occult Mail Breaker
    ((WeaponParam*)Params::Weapon().get_by_id(601700))->properFaith = 13;  // Occult Rapier
    ((WeaponParam*)Params::Weapon().get_by_id(602700))->properFaith = 18;  // Occult Estoc
    ((WeaponParam*)Params::Weapon().get_by_id(604700))->properFaith = 14;  // Occult Ricard's Rapier
    ((WeaponParam*)Params::Weapon().get_by_id(700700))->properFaith = 10;  // Occult Hand Axe
    ((WeaponParam*)Params::Weapon().get_by_id(701700))->properFaith = 11;  // Occult Battle Axe
    ((WeaponParam*)Params::Weapon().get_by_id(703700))->properFaith = 1;   // Occult Butcher Knife
    ((WeaponParam*)Params::Weapon().get_by_id(705700))->properFaith = 11;  // Occult Gargoyle Tail Axe
    ((WeaponParam*)Params::Weapon().get_by_id(750700))->properFaith = 10;  // Occult Greataxe
    ((WeaponParam*)Params::Weapon().get_by_id(751700))->properFaith = 10;  // Occult Demon's Greataxe
    ((WeaponParam*)Params::Weapon().get_by_id(800700))->properFaith = 10;  // Occult Club
    ((WeaponParam*)Params::Weapon().get_by_id(801700))->properFaith = 11;  // Occult Mace
    ((WeaponParam*)Params::Weapon().get_by_id(802700))->properFaith = 11;  // Occult Morning Star
    ((WeaponParam*)Params::Weapon().get_by_id(803700))->properFaith = 11;  // Occult Warpick
    ((WeaponParam*)Params::Weapon().get_by_id(804700))->properFaith = 10;  // Occult Pickaxe
    ((WeaponParam*)Params::Weapon().get_by_id(809700))->properFaith = 10;  // Occult Reinforced Club
    ((WeaponParam*)Params::Weapon().get_by_id(810700))->properFaith = 10;  // Occult Blacksmith Hammer
    ((WeaponParam*)Params::Weapon().get_by_id(850700))->properFaith = 13;  // Occult Great Club
    ((WeaponParam*)Params::Weapon().get_by_id(855700))->properFaith = 13;  // Occult Large Club
    ((WeaponParam*)Params::Weapon().get_by_id(901700))->properFaith = 10;  // Occult Caestus
    ((WeaponParam*)Params::Weapon().get_by_id(902700))->properFaith = 8;   // Occult Claw
    ((WeaponParam*)Params::Weapon().get_by_id(1000700))->properFaith = 18; // Occult Spear
    ((WeaponParam*)Params::Weapon().get_by_id(1001700))->properFaith = 18; // Occult Winged Spear
    ((WeaponParam*)Params::Weapon().get_by_id(1002700))->properFaith = 18; // Occult Partizan
    ((WeaponParam*)Params::Weapon().get_by_id(1050700))->properFaith = 16; // Occult Pike
    ((WeaponParam*)Params::Weapon().get_by_id(1100700))->properFaith = 12; // Occult Halberd
    ((WeaponParam*)Params::Weapon().get_by_id(1103700))->properFaith = 12; // Occult Gargoyle's Halberd
    ((WeaponParam*)Params::Weapon().get_by_id(1106700))->properFaith = 12; // Occult Lucerne 
    ((WeaponParam*)Params::Weapon().get_by_id(1107700))->properFaith = 1;  // Occult Scythe
    ((WeaponParam*)Params::Weapon().get_by_id(1150700))->properFaith = 1;  // Occult Great Scythe
    ((WeaponParam*)Params::Weapon().get_by_id(1200700))->properFaith = 10; // Occult Short Bow
    ((WeaponParam*)Params::Weapon().get_by_id(1201700))->properFaith = 12; // Occult Longbow
    ((WeaponParam*)Params::Weapon().get_by_id(1202700))->properFaith = 12; // Occult Black Bow of Pharis
    ((WeaponParam*)Params::Weapon().get_by_id(1204700))->properFaith = 12; // Occult Composite Bow
    ((WeaponParam*)Params::Weapon().get_by_id(1600700))->properFaith = 12; // Occult Whip
    ((WeaponParam*)Params::Weapon().get_by_id(1601700))->properFaith = 12; // Occult Notched Whip
}
#endif

uint64_t control_timer_function(uint64_t bar_id, uint64_t orange_bar) {
    //TODO get the other bar ids
    if (bar_id == 0x24) {
        uint32_t curtime = *Game::get_game_time_ms();
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
            float new_orange_bar = (float)beforehit_hp / (float)Game::get_player_char_max_hp();
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
    uint64_t pc_entity_ptr = Game::get_pc_entity_pointer();

    //if target is host
    if (target == pc_entity_ptr) {
        //Save current time and player HP
        uint32_t curhp = *(uint32_t*)(target + 0x3E8);
        //This sometimes gets called multiple times for 1 hit, so check
        if (curhp == new_hp) return;
        beforehit_hp = curhp;
        beforehit_time = *Game::get_game_time_ms();
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
        if (*Game::get_game_time_ms() - beforehit_time < MAX_RALLY_RECOVERY_TIME_MS) {
            //check what weapon is used in attack (check hand in weapon_data)
            int32_t weapon_hand = *(int32_t*)(attack_data + 0x4A4);

            uint32_t weaponid;
            //R hand
            if (weapon_hand == -1) {
                weaponid = Game::right_hand_weapon();
            }
            //L hand
            else if (weapon_hand == -2) {
                weaponid = Game::left_hand_weapon();
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

static DWORD WINAPI Apply_rally_capable_sfx_and_starting_hp(void* unused) {
    int32_t char_status = DS1_PLAYER_STATUS_LOADING;

    uint32_t weaponid_R = 0;
    uint32_t weaponid_L = 0;

    while (true) {
        //prevent first hit having a beforehit hp of zero
        beforehit_hp = UINT16_MAX;

        //only apply the rally sfx if character is loaded
        char_status = Game::get_player_char_status();

        while (char_status != DS1_PLAYER_STATUS_LOADING) {
            weaponid_R = Game::right_hand_weapon();
            weaponid_L = Game::left_hand_weapon();

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

            Sleep(10);
            char_status = Game::get_player_char_status();
        }

        Sleep(500);
    }

    return 0;
}

#undef DS1_BB_RALLY_SYS_OCCULT_MAT_ID
