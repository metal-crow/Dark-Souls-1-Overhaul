/*
    DARK SOULS GAME FILE C++ LIBRARY

    Contributors to this file:
        Sean Pesce  -  C++, reverse engineering of savegame files (.sl2)
        Tarvitz     -  Initial reverse engineering of savegame files (.sl2) (GitHub: https://github.com/tarvitz )


    Save/Sl2.h

    Contains data about SL2 (.sl2) savegame files, as well as functions and
    structures that facilitate working with this filetype.


    References:
        https://github.com/tarvitz/dsfp
        https://github.com/tarvitz/dsfp/blob/master/docs/datasheet.rst


*/

#pragma once

#ifndef _DS1_FILE_LIB_SL2_SAVE_FILE_H_
    #define _DS1_FILE_LIB_SL2_SAVE_FILE_H_

#include "Archive/Bnd4.h"
#include "GameEnum.h"
#include "FileUtil.h"
#include "Save/Enum.h"

#include <vector>


typedef struct RgbaColor {
    float r;
    float g;
    float b;
    float a;
} Ds1Color;

typedef struct GameCoordinates {
    float x;
    float y;
    float z;
} Ds1Coords;

typedef struct WorldPosition {
    Ds1Coords location;
    float rotation;
} Ds1Pos;

/*
    ///////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// SL2 FILE OVERVIEW //////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

        @TODO

*/



/*
    Class containing data and functions for reading and/or manipulating SL2 archive files (.sl2)
*/
typedef class Sl2SaveFile : Bnd4Archive {
public:
    // Default number of save slots (only 10 are usable)
    static const uint32_t SLOT_COUNT_DEFAULT = 11;

    // Default slot title prefix
    static const char    *SLOT_TITLE_PREFIX_DEFAULT;   //  "USER_DATA" (char)
    static const wchar_t *SLOT_TITLE_PREFIX_DEFAULT_W; // L"USER_DATA" (wide char)

    // SL2 file extension
    static const char    *FILE_EXT;   //  ".sl2" (char)
    static const wchar_t *FILE_EXT_W; // L".sl2" (wide char)

    // Default filename for Dark Souls save files
    static const char    *FILE_NAME_DEFAULT;   //  "DRAKS0005" (char)
    static const wchar_t *FILE_NAME_DEFAULT_W; // L"DRAKS0005" (wide char)

    // Header checksum value when all save slots are empty
    static const uint8_t CHECKSUM_HEADER_DEFAULT[16];
    // Footer checksum value when all save slots are empty
    static const uint8_t CHECKSUM_FOOTER_DEFAULT[16];

    static const uint32_t SAVE_SLOT_SIZE_DEFAULT = 393248;
    static const uint32_t SAVE_FILE_SIZE_DEFAULT = 4326432;

    // Maximum number of attunement slots in unmodified game (10 from maxed Attunement stat + 1 each from Darkmoon & White Seance Rings)
    static const uint32_t ATTUNEMENT_MAX_DEFAULT = 12;

    // File header structure
    typedef struct ArchiveHeader : Bnd4::Header {
        // Magic identifier bytes
//      uint32_t magic32_0 = 0x34444E42; // "BND4"
        uint32_t magic32_1 = 0x00000000;
        uint32_t magic32_2 = 0x00010000; // Tarvitz speculated that this was a revision number
        uint32_t slot_count = Sl2SaveFile::SLOT_COUNT_DEFAULT; // Including the main menu data slot, so number of save slots is (slot_count - 1)
        const uint32_t const32b_0 = 0x00000040;
        const uint32_t const32b_1 = 0x00000000;
        // Next 8 bytes are possibly a version string (Note: Not null-terminated)
        const uint32_t const32b_2 = 0x30303030; // "0000"
        const uint32_t const32b_3 = 0x31303030; // "0001"
        uint32_t slot_header_size = 0x00000020;
        const uint32_t const32b_4 = 0x00000000;
        uint32_t size = 0x000002C0; // Size of the file header in bytes (Including slot header array and title arrays)
        const uint32_t const32b_5 = 0x00000000;
        const uint32_t const32b_6 = 0x00002001;
        const uint8_t padding[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    } Header;


    typedef struct SaveSlotHeader {
        const uint32_t const32b_0 = 0x00000050;
        const uint32_t const32b_1 = 0xFFFFFFFF;
        uint32_t slot_size = 0x00060014; // Size of save slot (not including 12 trailing 0 bytes); generally the same default size
        const uint32_t const32b_2 = 0x00000000;
        uint32_t offset = 0; // Offset (from start of file) of save slot struct
        uint32_t title_offset = 0; // Offset of title string (not the same as character name)
        uint32_t padding_size = 0x0000000C; // Length of trailing padding between the current slot and the following slot
        const uint32_t const32b_3 = 0x00000000;
    } SaveHeader;

    /*
        Note: Save slot title is not the same as character name.
        
        Slot titles are null-terminated wide-character strings following a preset format:
            "USER_DATA***", where "***" is the save slot index ("000" to "999")

        Total size: 26 bytes (0x1A)
    */
    typedef struct SaveSlotTitle {
        wchar_t prefix[9] = { L'U', L'S', L'E', L'R', L'_', L'D', L'A', L'T', L'A' };
        wchar_t index[3] = { L'0', L'0', L'0' };
        wchar_t terminator = 0;
    } SaveTitle;



    typedef class SaveSlot {
    public:

        // pragma pack() turns off implicit struct padding, which was causing issues
        #pragma pack(push, 1)
        typedef struct SaveSlotStartData {
            // Starting data is a fixed length, so addressing is easy if we keep it in a packed struct
            typedef struct PlayerStats {
                // "Poise? I think poise is inferred from item use etc." - Tarvitz
                typedef struct StatPoints {
                    // NOTE: This may not be correct. Tarvis made this comment: "is the middle one total or current?"
                    uint32_t current = 0;
                    uint32_t total = 0;
                    uint32_t total2 = 0;
                } Points;
                typedef struct HitPoints : StatPoints {} Hp;
                typedef struct HitPoints Health;
                typedef struct UnknownStatPoints : StatPoints {} UnknownPoints; // Unused MP/mana stat?
                typedef struct Stamina : StatPoints {} Stam;

                typedef struct CharacterStats {
                    uint64_t vitality = 1;
                    uint64_t attunement = 1;
                    uint64_t endurance = 1;
                    uint64_t strength = 1;
                    uint64_t dexterity = 1;
                    uint64_t intelligence = 1;
                    uint64_t faith = 1;
                    uint64_t unknown0 = 1;
                    uint32_t unknown1 = 1;
                    uint32_t humanity = 0;
                    uint64_t resistance = 1; // Why would this be 64 bit... it's probably not
                    uint32_t level = 1;
                    uint32_t souls = 0;
                    uint64_t earned_souls = 0; // Soul memory?
                    uint32_t unknown2 = 1;
                    int32_t status = PLAYER_STATUS::HUMAN; // Human = 0, hollow = 8, etc
                } CharStats;

                
                Hp hp;
                UnknownStatPoints unknown_stat0;
                uint32_t unknown_stat1 = 0;
                Stamina stamina;
                uint32_t unknown_stat2 = 0;
                CharStats stats;
                wchar_t name[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // UTF-8
                uint8_t unknown0[6] = { 0, 0, 0, 0, 0, 0 }; // Possibly extension of character name?
                uint8_t gender = GENDER::MALE; // Male = 1, Female = 0
                uint8_t unknown1 = 0;
                uint8_t unknown2[2] = { 0, 0 };
                uint8_t class_ = ENUM_CLASS_TYPE::Warrior;
                uint8_t body_type = BODY_TYPE::average;
                uint8_t gift = GIFT::no_gift;
                uint8_t unknown3 = 0;
                uint8_t unknown4[34];
                uint32_t sin = 0;
                uint8_t unknown5[8];
                uint32_t resist_poison = 0;
                uint32_t resist_bleed = 0;
                uint32_t resist_toxic = 0;
                uint32_t resist_curse = 0;
                uint8_t unknown6[3];
                uint8_t covenant = ENUM_COVENANT_TYPE::NoCovenant;
                uint8_t face = HOMELAND::COMMONER;
                uint8_t hair = HAIR_STYLE::shaved;
                uint8_t hair_color = HAIR_COLOR::black; // Enum, not RGBA (Note: This is technically hair and eye color, which is a single choice in char creation menu)
                uint8_t unknown7 = 0;
            } Stats;

            typedef struct InventoryItemSlot {
                uint32_t type = 0;
                uint32_t id = 0;
                uint32_t quantity = 0;
                uint32_t index : 12;
                uint32_t sort_index : 20;
                uint32_t filled = 0; // Whether this slot has an item in it or not (seems to be 0 or 1) (??)
                uint32_t durability = 0;
                uint32_t durability_hits = 0;
            } InventorySlot;

            typedef struct StorageItemSlot {
                uint32_t unknown0 = 0;
                uint32_t type_and_item = 0;
                uint32_t unknown1 = 0;
                uint32_t unknown2 = 0;
                uint32_t durability = 0;
                uint32_t unknown3 = 0;
                uint32_t unknown4 = 0;
                uint32_t unknown5 = 0;
            } StorageSlot;

            typedef struct CharacterEquipment {
                // Equipment slots hold the equipment ID if something is equipped, or 0xFFFFFFFF if empty (except head/torso/hand/legs)
                uint8_t unknown0[168];
                uint32_t weapon_equip[4] = { 64, 64, 64, 64 }; // ?? (Not item IDs?)
                uint32_t arrow_left_slot = ITEM::empty;
                uint32_t bolt_left_slot = ITEM::empty;
                uint32_t arrow_right_slot = ITEM::empty;
                uint32_t bolt_right_slot = ITEM::empty;
                uint32_t armor_equip[4] = { 81, 80, 78, 79 }; // ?? (Not item IDs?)
                uint32_t unknown1 = ITEM::empty;
                // Not sure what the difference is between the non-slot vars and *_slot vars
                uint32_t ring_left_slot = ITEM::empty;
                uint32_t ring_right_slot = ITEM::empty;
                uint32_t belt_quantities[5] = { ITEM::empty, ITEM::empty, ITEM::empty, ITEM::empty, ITEM::empty }; // Belt item quantities
                uint32_t wield_type = 1; // One-hand (1) vs 2-hand (3) 
                // Next 3 were labelled as "switches" seemed to only be on (1) or off (0)
                uint32_t hand_left = 1; // ??
                uint32_t hand_right = 1; // ??
                uint32_t unknown_switches[4] = { 0, 0, 0, 0 }; // ??
                // Item slots
                uint32_t hand_left_primary = ITEM::Fists_nothing_equipped;
                uint32_t hand_left_secondary = ITEM::Fists_nothing_equipped;
                uint32_t hand_right_primary = ITEM::Fists_nothing_equipped;
                uint32_t hand_right_secondary = ITEM::Fists_nothing_equipped;
                uint32_t arrow_left_item_slot = ITEM::empty;
                uint32_t bolt_left_item_slot = ITEM::empty;
                uint32_t arrow_right_item_slot = ITEM::empty;
                uint32_t bolt_right_item_slot = ITEM::empty;
                uint32_t head = ITEM::Head_nothing_equipped;
                uint32_t torso = ITEM::Chest_nothing_equipped;
                uint32_t hands = ITEM::Arms_nothing_equipped;
                uint32_t legs = ITEM::Legs_nothing_equipped;
                uint32_t hairstyle = 1200;
                uint32_t ring_left = ITEM::empty;
                uint32_t ring_right = ITEM::empty;
                uint32_t belt[5] = { ITEM::empty, ITEM::empty, ITEM::empty, ITEM::empty, ITEM::empty }; // Belt item IDs
            } Equipment;

            typedef struct AttunementSlot {
                uint32_t spell_id = 0; // Unique ID of equipped spell (or 0xFFFFFFFF if nothing is equipped)
                uint32_t remaining = 0; // Number of remaining casts (??)
            } Attunement;

            struct UnknownSlot {
                // Possibly covenant-related?
                uint16_t index = 0;
                uint16_t id = 0;
            };

            typedef struct CharacterCreationData {
                uint8_t face_geo_data[50];
                uint8_t skin_tone = 127;
                uint8_t face_tex_data[49];
                uint8_t unknown[8];
            } CreationData;

            typedef struct GestureData {
                UnknownSlot unknown0[10];
                UnknownSlot learned_gestures[4];
                UnknownSlot unknown1[4]; // unknown1[3] might be a bit mask with each bit representing a gesture being locked/unlocked
                uint32_t padding[16];
            } Gestures;

            typedef struct NewGamePlusData {
                uint16_t unknown0[72];
                uint16_t ng_level = 0; // NG+ level (0 for first playthrough, 1 for NG+, etc)
                uint32_t padding[2];
            } NewGamePlus;

            struct ZeroNegativeOne {
                // Section of data that alternates between 0x00000000 and 0xFFFFFFFF (if save slot is used)
                uint32_t zero = 0;
                uint32_t negative_one = 0xFFFFFFFF;
            };

            struct UnknownCounters {
                uint32_t unknown0[2];
                uint16_t unknown_short = 0;
                uint32_t unknown1[4];
                uint8_t  unknown2[28];
                uint32_t death_count = 0;
                uint32_t padding = 0; // 0xFFFFFFFF if save slot is used
            };

            typedef struct BonfiresVisitedData {
                // If a bonfire has been rested at, the corresponding bit is set to 1
                uint32_t undead_parish : 1; // Undead Parish
                uint32_t bonfire1 : 1;
                uint32_t bonfire2 : 1;
                uint32_t bonfire3 : 1;
                uint32_t bonfire4 : 1;
                uint32_t bonfire5 : 1;
                uint32_t bonfire6 : 1;
                uint32_t bonfire7 : 1;
                uint32_t princess_chamber : 1; // Chamber of the Princess
                uint32_t bonfire9 : 1;
                uint32_t sunlight_altar : 1; // Sunlight Altar
                uint32_t bonfire11 : 1;
                uint32_t bonfire12 : 1;
                uint32_t bonfire13 : 1;
                uint32_t bonfire14 : 1;
                uint32_t firelink_shrine : 1; // Firelink Shrine
                uint32_t bonfire16 : 1;
                uint32_t bonfire17 : 1;
                uint32_t bonfire18 : 1;
                uint32_t bonfire19 : 1;
                uint32_t bonfire20 : 1;
                uint32_t bonfire21 : 1;
                uint32_t bonfire22 : 1;
                uint32_t bonfire23 : 1;
                uint32_t bonfire24 : 1;
                uint32_t bonfire25 : 1;
                uint32_t bonfire26 : 1;
                uint32_t bonfire27 : 1;
                uint32_t bonfire28 : 1;
                uint32_t bonfire29 : 1;
                uint32_t bonfire30 : 1;
                uint32_t bonfire31 : 1;
            } BonfiresVisited;

            typedef struct BonfireDataSection {
                uint32_t unknown0[4];
                uint8_t  unknown1[18];
                BonfiresVisited visited;
                uint8_t unknown2[214];
                uint8_t padding0[788];
                uint8_t unknown3[224];
                uint8_t padding1[16];
            } BonfireSection;

            typedef struct UnknownDataSection {
                uint32_t unknown0 = 0;
                uint32_t unknown1 = 0;
                uint32_t unknown2 = 0;
                uint32_t index = 0;
                uint8_t markers[112];
                uint8_t unknown3[1152];
            } UnknownSection;

            // Header is an MD5 checksum of the bytes starting at byte 16 (0x10), to byte (16+size+4) for a total of (this->size+4) bytes, but only AFTER footer hash is calculated
            uint8_t checksum_header[16];
            uint32_t size = 0x00000000; // Size of data starting after this value and ending at the end of the footer checksum
            // NOTE: All data between size and footer checksum can be zeroed-out in an empty save slot
            uint32_t unknown0[2] = { 0x00000000, 0x00000000 }; // unknown[0] seems to be version info in the last (unused) save slot
            uint32_t play_time = 0x00000000; // Total in-game playtime in ??
            uint32_t unknown_stats[19]; // In end (unused) slot, bytes [8:18] are either 1 or 0, and signify which slots have a character (1=slot has char)
            Stats stats;
            Equipment equipment;
            uint32_t inventory_item_count = 0;
            uint32_t inventory_key_count = 0;
            uint32_t inventory_size = 0; // Total slots (used or unused) in entire inventory; seems to always be 2048 (util_box.length + item_box.length)
            InventorySlot util_box[64]; // Key items?
            InventorySlot item_box[1984]; // Non-key items?
            uint32_t last_index = 0;
            Attunement spells[Sl2SaveFile::ATTUNEMENT_MAX_DEFAULT];
            uint32_t unknown_stat0 = 0x00000000;
            uint32_t belt_quantities[5] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }; // Remaining amount of each belt item (0xFFFFFFFF for no item) (Tarvitz had this as "equippedItems")
            UnknownSlot unknown_slot[10];
            RgbaColor hair_color = { 0.235f, 0.235f, 0.196f, 1.0f };
            RgbaColor eye_color = { 0.235f, 0.235f, 0.196f, 1.0f };
            CharacterCreationData character_creation_data; // CharInitParams?
            StorageSlot bottomless_box[2048];
            GestureData gestures;
            NewGamePlusData ng_data;
            StorageSlot unknown_item_data0[64];
            ZeroNegativeOne unknown_data[120];
            UnknownCounters counters; // Contains death count
            BonfireSection bonfires;  // Contains flags for whether each bonfire has been visited
            UnknownSection unknown_sections[72];
            uint8_t padding[10];
        } StartData;
        #pragma pack(pop)

        // Middle section contains online data, which varies in size
        typedef class OnlineData {
        public:
            static const uint32_t trailing_data_length = 88; // Trailing data always seems to be the same length (88 bytes)
            
            class OnlineContainer {
            public:
                uint32_t length; // Empty containers have length == 8
                uint8_t *data;   // Array of bytes with length specified by length
                uint32_t *index;

                void *start() {
                    return (void*)(((uint32_t)data) - sizeof(uint32_t));
                }

                // Returns address of next container in OnlineContainer array
                OnlineContainer *next_container() {
                    return (OnlineContainer*)(((uint32_t)index) + sizeof(uint32_t));
                }

                // Constructor
                OnlineContainer(void *new_start) : length(*(uint32_t*)new_start), data((uint8_t*)((uint32_t)new_start + sizeof(uint32_t))), index((uint32_t*)(((uint32_t)data) + length)) {}
                OnlineContainer() {}
                // Destructor
                ~OnlineContainer() {}
            };

            class SoapstoneMessage {
            public:
                uint32_t *start_flag; // Usually 1; maybe 0 if player is the author of the message?
                uint32_t *size; // 52 or 56 (maybe other values but I haven't seen any)
                // Steam64IDs can be searched using this site: https://steamid.io/lookup
                char (*steam_id)[16]; // Steam64ID (not null-terminated) or Steam name (null-terminated) (if Steam name is < 16 chars)
                uint8_t *unknown_data; // Unknown data; usually 12 bytes but if size is 56 then unknown_data will be 16 bytes
                Ds1Pos  *position; // XYZ coordinates of the message (not necessarily in that order; order is unknown) and rotation
                uint16_t (*msg_id)[2]; // IDs of soapstone message substrings
                uint16_t *positive; // Total number of positive rates (upvotes)
                uint16_t *negative; // Total number of negative rates (downvotes)

                int unknown_data_length;

                uint32_t next_message() {
                    return (uint32_t)(negative + 1);
                }

                // Constructor
                SoapstoneMessage(void *new_start) : start_flag((uint32_t*)new_start), size(start_flag + 1), steam_id((char(*)[16])(size + 1)), unknown_data((uint8_t*)(steam_id + 1))
                {
                    unknown_data_length = (*size) - ((sizeof(char) * 16) + sizeof(Ds1Pos) + (sizeof(uint16_t) * 4));
                    position = (Ds1Pos*)(unknown_data + unknown_data_length);
                    msg_id = (uint16_t(*)[2])(position + 1);
                    positive = (uint16_t*)(msg_id + 1);
                    negative = (uint16_t*)(positive + 1);
                }
                SoapstoneMessage() {}
                // Destructor
                ~SoapstoneMessage() {}
            };

            // Soapstone messages; index = 7
            typedef class SoapstoneMessageContainer : public OnlineContainer {
            public:
                std::vector<SoapstoneMessage> messages = std::vector<SoapstoneMessage>();
                uint32_t *msgs_end_flag; // = 0x0000FFFF;
                uint32_t *padding; // = 0x00000000

                SoapstoneMessage get(int unsigned i) {
                    return messages[i];
                }

                int message_count() {
                    return messages.size();
                }

                // Returns Steam ID (Steam64ID or Steam name, depending on the message) of the author of the message at index i
                const char *get_author(int unsigned i, std::string &out_str) {
                    out_str.clear();
                    if ((int)i >= messages.size()) {
                        return NULL;
                    }
                    if ((*messages[i].steam_id)[15] == 0)
                    {
                        out_str.append(*(messages[i].steam_id));
                    }
                    else
                    {
                        // Steam64 ID strings are not null-terminated; create a null-terminated copy by inserting each char into the given string
                        for (int c = 0; c < 16; c++) {
                            out_str += (*messages[i].steam_id)[c];
                        }
                    }
                    return out_str.c_str();
                }

                // Returns soapstone message unknown data bytes as a string representation of a hexadecimal byte array
                const char *get_unknown_msg_data_as_hex_string(int unsigned i, std::string &out_str) {
                    out_str.clear();
                    if ((int)i >= messages.size()) {
                        return NULL;
                    }
                    std::string tmp;
                    for (int b = 0; b < messages[i].unknown_data_length; b++) {
                        out_str.append(FileUtil::to_hex_string(messages[i].unknown_data[b], tmp) + std::string(" "));
                    }
                    return out_str.c_str();
                }

                // Returns SoapstoneMessagesContainer as a formatted string
                const char *to_string(std::string &out_str) {
                    out_str.clear();
                    out_str.append("Container length: " + std::to_string(length) + " bytes\nMessages: " + std::to_string(messages.size()) + "\n");
                    std::string tmp;
                    for (int i = 0; i < (int)messages.size(); i++) {
                        tmp.clear();
                        out_str.append("    [Message #" + std::to_string(i) + "]\n    Size: " + std::to_string(*messages[i].size) + " bytes\n    Author: "
                            + get_author(i, tmp) + "\n    Unknown 12 bytes: " + get_unknown_msg_data_as_hex_string(i, tmp)
                            + "\n    XYZ Coordinates: \n        " + std::to_string(messages[i].position->location.x)
                            + "\n        " + std::to_string(messages[i].position->location.y) + "\n        " + std::to_string(messages[i].position->location.z)
                            + "\n    Rotation: \n        " + std::to_string(messages[i].position->rotation) + "\n    Message IDs: "
                            + std::to_string((*messages[i].msg_id)[0]) + "    " + std::to_string((*messages[i].msg_id)[1])
                            + "\n    Ratings: +" + std::to_string(*messages[i].positive) + ", -" + std::to_string(*messages[i].negative)
                            + "  (Net=" + std::to_string((*messages[i].positive) - (*messages[i].negative)) + ", Blind net="
                            + std::to_string((*messages[i].positive) + (*messages[i].negative)) + ")\n\n");
                    }
                    return out_str.c_str();
                }

                // Constructor
                SoapstoneMessageContainer(void *new_start) : OnlineContainer(new_start)
                {
                    padding = (uint32_t*)(index - 1);
                    msgs_end_flag = (uint32_t*)(padding - 1);
                    SoapstoneMessage msg = SoapstoneMessage((void*)data);

                    while ((*msg.start_flag) != 0x0000FFFF) {
                        messages.push_back(SoapstoneMessage(msg.start_flag));
                        msg = SoapstoneMessage((void*)(((uint32_t)msg.steam_id) + (*msg.size)));
                    }
                }
                SoapstoneMessageContainer() {}
                // Destructor
                ~SoapstoneMessageContainer() {}
            } SoapstoneMessages;


            typedef class Bloodstain {
            public:
                int32_t *start_flag; // = 0x00000001;  // I've never seen a value other than 1 here
                uint32_t *length;
                char (*steam_id)[16]; // Steam64ID (Not null-terminated) or Steam name (null-terminated) (if Steam name is < 16 chars) 
                uint8_t (*unknown_data)[12]; // Unknown data (12 bytes)
                Ds1Pos  *position;
                uint8_t *animation_data; // Unknown anmation data specifies animations played, player rotations, timings, etc in the player's death sequence

                uint32_t animation_data_length = 0;

                Bloodstain *next_bloodstain() {
                    return (Bloodstain*)&animation_data[animation_data_length];
                }

                Bloodstain(void *new_start) :
                    start_flag((int32_t*)new_start),
                    length((uint32_t*)(start_flag + 1)),
                    steam_id((char(*)[16])(length + 1)),
                    unknown_data((uint8_t(*)[12])(steam_id + 1)),
                    position((Ds1Pos*)(unknown_data + 1)),
                    animation_data((uint8_t*)(position + 1))
                {
                    uint32_t unknown_data_length_pad = 0;
                    uint8_t *unknown_data_length_test = (uint8_t*)(((uint32_t)animation_data) + ((*length) - (16 + 12 + 16)) + unknown_data_length_pad);
                    while (*unknown_data_length_test == 0) {
                        unknown_data_length_pad++;
                        unknown_data_length_test = (uint8_t*)(((uint32_t)animation_data) + ((*length) - (16 + 12 + 16)) + unknown_data_length_pad);
                    }
                    animation_data_length = ((*length) - (16 + 12 + 16)) + unknown_data_length_pad;
                }
                Bloodstain() {}
                ~Bloodstain() {}
            } PlayerGhost;

            // Bloodstains; index = 8
            typedef class BloodstainContainer : public OnlineContainer {
            public:
                Bloodstain *bloodstains_data; // Array of bloodstain structures
                uint32_t *bloodstains_end_flag; // = 0x0000FFFF; should be (&length + length)
                uint32_t *padding; // Another 4 null bytes
                std::vector<Bloodstain> bloodstains = std::vector<Bloodstain>();

                Bloodstain get(int unsigned i) {
                    return bloodstains.at(i);
                }

                int bloodstain_count() {
                    return bloodstains.size();
                }

                // Constructor
                BloodstainContainer(void *new_start) : OnlineContainer(new_start)
                {
                    bloodstains_data = (Bloodstain*)data;
                    bloodstains_end_flag = (uint32_t*)((((uint32_t)bloodstains_data) + length) - (sizeof(uint32_t) * 2));
                    padding = (uint32_t*)((((uint32_t)bloodstains_data) + length) - sizeof(uint32_t));
                    Bloodstain bloodstain = Bloodstain(bloodstains_data);
                    while ((*bloodstain.start_flag) != 0x0000FFFF && ((uint32_t)bloodstain.start_flag) < (((uint32_t)(data)) + length)) { // This might be better: while ((*bloodstain.start_flag) != 0x0000FFFF && ((uint32_t)bloodstain.start_flag) < (((uint32_t)(data)) + length))
                        bloodstains.push_back(Bloodstain(bloodstain.start_flag));
                        bloodstain = Bloodstain(bloodstain.next_bloodstain());
                    }
                }
                BloodstainContainer() {}
                // Destructor
                ~BloodstainContainer() {}
            } Bloodstains;
            typedef BloodstainContainer PlayerGhostContainer;
            typedef Bloodstains PlayerGhosts;

            typedef class SummonSign {
            public:
                int32_t *start_flag; // Should be 1 (or 0?)
                uint32_t *length;
                char(*steam_id)[16]; // Steam64ID (Not null-terminated) or Steam name (null-terminated) (if Steam name is < 16 chars) 
                uint8_t(*unknown_data)[12]; // Unknown data (12 bytes)
                Ds1Pos  *position;
                uint32_t *padding;

                SummonSign(void *new_start) :
                    start_flag((int32_t*)new_start),
                    length((uint32_t*)(((uint32_t)new_start) + sizeof(uint32_t))),
                    steam_id((char(*)[16])(((uint32_t)new_start) + (sizeof(uint32_t) * 2))),
                    unknown_data(((uint8_t(*)[12])steam_id) + 16),
                    position((Ds1Pos*)(unknown_data + 1)),
                    padding((uint32_t*)(((uint32_t)length) + (*length)))
                {}
                SummonSign() {}
                ~SummonSign() {}
            } PlayerSign;

            // Index 9 and 10; Tarvitz speculated that these last 2 non-empty containers had data about summon signs (one container for white and one for red).
            //     This seems likely considering the type data that was in the other containers.
            typedef class SummonSignContainer : public OnlineContainer {
            public:
                uint32_t *end_flag;
                uint32_t *padding;
                std::vector<SummonSign> signs = std::vector<SummonSign>();

                int sign_count() {
                    return signs.size();
                }

                // Constructor
                SummonSignContainer(void *new_start) : OnlineContainer(new_start)
                {
                    SummonSign s = SummonSign(data);
                    while ((*s.start_flag) != 0x0000FFFF && ((uint32_t)s.start_flag) < (((uint32_t)data) + length)) {
                        signs.push_back(s);
                        s = SummonSign(s.padding + 1);
                    }
                    end_flag = index - 2;
                    padding = index - 1;
                }
                SummonSignContainer() {}
                // Destructor
                ~SummonSignContainer() {}
            } SummonSigns;
            
            // "CHR " header contains data about which enemies are already dead in the host world
            uint32_t *chr_header_length;
            uint8_t *chr_header; // Array of bytes with length = char_header_length
            // "OBJ " header contains data about broken environment objects
            uint32_t *obj_header_length;
            uint8_t *obj_header; // Array of bytes with length = obj_header_length
            uint8_t *trailing_data; // This seems to be a constant size of 88 bytes
            OnlineContainer *containers_start; // Array of (empty) OnlineContainers

            // Somewhere in containers array is a SoapstoneMessageContainer (index 7) and a BloodstainContainer (index 8)
            SoapstoneMessages soapstone_messages;
            Bloodstains bloodstains;
            SummonSigns summon_signs[2];

            // Vector of OnlineContainer pointers to allow access to individual containers
            std::vector<OnlineContainer> empty_containers = std::vector<OnlineContainer>(); // When a container is found with index == 0x0000FFFF, the last container has been found

            uint32_t start() {
                return (uint32_t)chr_header_length;
            }

            // Constructor
            OnlineData(void *new_start) : chr_header_length((uint32_t*)new_start)
            {
                chr_header = (uint8_t*)(this->start() + sizeof(uint32_t)); // Address of chr_header_length + 4
                obj_header_length = (uint32_t*)(this->start() + sizeof(uint32_t) + *chr_header_length);// Address of chr_header + chr_header_length
                obj_header = (uint8_t*)(((uint32_t)obj_header_length) + sizeof(uint32_t)); // Address of obj_header_length + 4
                trailing_data = (uint8_t*)(((uint32_t)obj_header) + *obj_header_length);  // Address of obj_header + obj_header_length
                containers_start = (OnlineContainer*)(((uint32_t)trailing_data) + OnlineData::trailing_data_length);
                OnlineContainer container = OnlineContainer(containers_start);

                bool containers_initialized = false;
                while (!containers_initialized) {
                    
                    switch (*(container.index)) {
                        case 0:
                            // Empty save slot
                            break;
                        case 7:
                            // Soapstone message container
                            soapstone_messages = SoapstoneMessages(container.start());
                            break;
                        case 8:
                            // Bloodstain container
                            bloodstains = Bloodstains(container.start());
                            break;
                        //case 9:
                            // Unknown container (more signs?)
                            //summon_signs[0] = SummonSigns(container.start());
                            //break;
                        case 10:
                            // Summon sign container
                            if (container.length > 8) {
                                //print_console("initializing signs container...");
                                summon_signs[1] = SummonSigns(container.start());
                            }
                            break;
                        default:
                            // Empty container
                            empty_containers.push_back(OnlineContainer(container.start()));
                            break;
                    }
                    
                    if ((*(container.index)) == 0x0000FFFF || (*(container.index)) == 0) {
                        // Last container found
                        containers_initialized = true;
                    }
                    else
                    {
                        // Get next multiplayer data container
                        container = OnlineContainer(container.next_container());
                    }
                }
            }
            OnlineData() {}

            // Destructor
            ~OnlineData(){}
        } MultiplayerData;
        typedef MultiplayerData MpData;


        #pragma pack(push, 1)
        typedef struct SaveSlotEndData {
            // Footer hash is an MD5 checksum calculated from byte 20 (0x14) to byte (20+size-16), for a total of (this->size() - 16) bytes
            uint8_t checksum[16];
            uint8_t padding[12];
        } EndData;
        #pragma pack(pop)
        

        StartData *start_data;
        MultiplayerData multiplayer_data; // Multiplayer data varies in size

        // Between multiplayer_data and end_data there is a varying amount of zeroed-out bytes

        EndData *end_data;

        SaveHeader *header;
        wchar_t *title;
        void *file_start;

        // Returns true if this is an unused save slot
        bool is_empty() {
            // @TODO: Use MainMenu slot flags instead
            return start_data->stats.hp.total == 0;
        }

        uint32_t file_start_val() {
            return (uint32_t)file_start;
        }

        uint32_t size() {
            return header->slot_size;
        }

        uint32_t size_with_padding() {
            return (header->slot_size + 12); // Or (header->slot_size + header->padding_size) ?
        }

        uint32_t calculate_checksums(std::vector<uint8_t> &store_primary, std::vector<uint8_t> &store_secondary) {
            
            uint32_t return_val = FileUtil::calculate_md5_hash(((uint8_t*)(start_data->checksum_header)) + 20, size() - 36, 16, store_secondary);
            if (return_val)
                return return_val;

            // @TODO: Write secondary checksum before calculating primary?

            return FileUtil::calculate_md5_hash(((uint8_t*)(start_data->checksum_header)) + 16, size() - 16, 16, store_primary);
        }

        void calculate_footer_checksum(std::vector<uint8_t> &buffer) {
            buffer.clear();
            FileUtil::calculate_md5_hash(((uint8_t*)(start_data->checksum_header)) + 20, size() - 36, 16, buffer);
        }

        void calculate_header_checksum(std::vector<uint8_t> &buffer) {
            buffer.clear();
            FileUtil::calculate_md5_hash(((uint8_t*)(start_data->checksum_header)) + 16, size() - 16, 16, buffer);
        }

        bool verify_checksums() {
            std::vector<uint8_t> calculated_primary = std::vector<uint8_t>();
            std::vector<uint8_t> calculated_secondary = std::vector<uint8_t>();
            
            // Calculate footer checksum
            FileUtil::calculate_md5_hash(((uint8_t*)(start_data->checksum_header)) + 20, size() - 36, 16, calculated_secondary);
            for (int b = 0; b < (int)sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT); b++) {
                if (calculated_secondary[b] != end_data->checksum[b]) {
                    return false;
                }
            }

            // Calculate header checksum
            FileUtil::calculate_md5_hash(((uint8_t*)(start_data->checksum_header)) + 16, size() - 16, 16, calculated_primary);
            for (int b = 0; b < (int)sizeof(Sl2SaveFile::CHECKSUM_HEADER_DEFAULT); b++) {
                if (calculated_primary[b] != start_data->checksum_header[b]) {
                    return false;
                }
            }


            std::string check_prim, check_sec, calc_prim, calc_sec, tmp;
            for (int b = 0; b < 16; b++) {
                check_prim += FileUtil::to_hex_string(start_data->checksum_header[b], tmp);
                check_sec += FileUtil::to_hex_string(end_data->checksum[b], tmp);
                calc_prim += FileUtil::to_hex_string(calculated_primary[b], tmp);
                calc_sec += FileUtil::to_hex_string(calculated_secondary[b], tmp);
            }

            return true;
        }

        // Constructor
        SaveSlot(void *new_file_start, SaveHeader *new_header) : file_start(new_file_start), header(new_header)
        {
            start_data = (StartData*)(file_start_val() + header->offset);
            title = (wchar_t*)(file_start_val() + header->title_offset);
            multiplayer_data = MultiplayerData((void*)(file_start_val() + header->offset + sizeof(StartData)));
            end_data = (EndData*)((((uint32_t)start_data) + header->slot_size) - 16);
        }
        SaveSlot() {}

        // Destructor
        ~SaveSlot()
        {
        }
    } Save;


    typedef class MainMenuSlot {
    public:

        #pragma pack(push, 1)
        struct StartData {
            uint8_t checksum_header[16];
            uint32_t length;
            uint32_t version; // Should be 19 (0x13), but some other values also work (Invalid value yields error message with string ID = 45001)
            uint16_t unknown_data0[3];
            uint8_t unknown_data1; // Thought this was # of slots, but not sure anymore
            uint8_t unknown_flags0[8];
            uint8_t unknown_data2;
            uint8_t slot_occupied[10]; // Each flag (1 or 0) denotes whether a saved character resides in the corresponding save slot (Array length = Sl2Header.slot_count - 1)
            uint8_t unknown_flags1[6];
        };
        #pragma pack(pop)

        #pragma pack(push, 1)
        typedef struct CharacterData {

            typedef struct CharacterPreview {
                uint32_t unknown_data0[6];
                RgbaColor hair_color;
                RgbaColor eye_color;
                uint8_t face_geo_data[50];
                uint8_t skin_tone;
                uint8_t face_tex_data[49];
                uint8_t unknown_data1[60];
                // Equipment IDs
                uint32_t L_hand_primary;
                uint32_t R_hand_primary;
                uint32_t L_hand_secondary;
                uint32_t R_hand_secondary;
                uint32_t L_arrows;
                uint32_t L_bolts;
                uint32_t R_arrows;
                uint32_t R_bolts;
                uint32_t head;
                uint32_t torso;
                uint32_t hands;
                uint32_t legs;
                uint32_t hairstyle;
                uint32_t L_ring;
                uint32_t R_ring;
                uint32_t belt[5]; // belt[0] is ID of left-most belt (Quickslot) item
                uint32_t unknown_data2[2];
            } CharPreview;


            wchar_t name[14];
            uint32_t unknown_data0[2];
            uint32_t level; // Soul level
            uint32_t playtime = 0; // Total playtime in seconds; overflows at 999:59:59 (3599999)
            uint32_t earned_souls = 0; // Unconfirmed?
            uint16_t unknown_data1[2];
            uint32_t location; // Game world location string ID from .fmg file in msg/<LANGUAGE>/item.msgbnd.dcx
            CharPreview char_preview;
            uint16_t unknown_data2[4];

        } CharData;
        #pragma pack(pop)

        StartData *start_data;
        CharData *char_previews; // Array of character previews
        uint8_t (*checksum_footer)[16];
        uint8_t *padding; // Array with length = Sl2Header.slot_headers[Sl2Header.slot_count - 1]->padding_size


        // Instance data
        void *file_start;
        SaveHeader *header;
        wchar_t *title;

        uint32_t file_start_val() {
            return (uint32_t)file_start;
        }

        int unsigned slot_count() {
            return ((Sl2SaveFile::Header*)file_start)->slot_count - 1;
        }

        // Constructor
        MainMenuSlot(void *new_file_start, SaveHeader *new_header) : file_start(new_file_start), header(new_header)
        {
            title = (wchar_t*)(file_start_val() + header->title_offset);
            
            start_data = (StartData*)(file_start_val() + header->offset);
            char_previews = (CharData*)(start_data + 1);
            checksum_footer = (uint8_t(*)[16])((((uint32_t)start_data) + header->slot_size) - 16);
            padding = (uint8_t*)(checksum_footer + 1);
        }
        MainMenuSlot() {}

        // Destructor
        ~MainMenuSlot()
        {}

    } MainMenuData;

private:



public:

    Header *header;
    SaveHeader *slot_headers; // Array
    SaveTitle  *slot_titles;  // Array

    // Between slot titles and slot structures is zeroed-out padding to align save slots to 16 bytes
    // uint8_t *slot_titles_pad;

    std::vector<SaveSlot> saves = std::vector<SaveSlot>();
    MainMenuData main_menu;

    Sl2SaveFile(void *new_start): header((Header*)new_start), slot_headers((SaveHeader*)(&header[1])), slot_titles((SaveTitle*)(((uint32_t)new_start) + slot_headers[0].title_offset))
    {
        for (int i = 0; i < (int)(header->slot_count - 1); i++) {
            saves.push_back(SaveSlot(new_start, &slot_headers[i]));
        }
        main_menu = MainMenuData(new_start, &slot_headers[header->slot_count - 1]);
    };

    Sl2SaveFile() {}

    ~Sl2SaveFile() {}

    uint32_t total_size() {
        return slot_headers[header->slot_count - 1].offset + slot_headers[header->slot_count - 1].slot_size + slot_headers[header->slot_count - 1].padding_size;
    }

    bool write_to_file(const char *new_file_name) {
        return (FileUtil::dump_file(header, total_size(), new_file_name) == ERROR_SUCCESS);
    }



    /*
        Creates an empty save file
    */
    static uint32_t generate_empty_save_file(const char *new_filename = NULL, bool overwrite = false) {
        std::string base_filename, filename;
        if (new_filename == NULL) {
            base_filename = Sl2SaveFile::FILE_NAME_DEFAULT;
        } else {
            base_filename = new_filename;
        }
        filename = base_filename;

        if (!overwrite) {
            int unsigned file_count = 1;
            while (FileUtil::file_exists(filename.c_str()) && file_count < 100) {
                if (file_count < 10) {
                    filename = base_filename + "_0" + std::to_string(file_count);
                } else {
                    filename = base_filename + "_" + std::to_string(file_count);
                }
                file_count++;
            }
            if (file_count >= 100 && FileUtil::file_exists(filename.c_str())) {
                // Too many save files; don't create a new one
                return ERROR_FILE_EXISTS;
            }
        }

        void *buff = CoTaskMemAlloc(Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT);
        if (buff == NULL) {
            return ERROR_OUTOFMEMORY;
        }
        memset(buff, 0, Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT);

        Sl2SaveFile::Header tmp_header;
        Sl2SaveFile::SaveSlotHeader tmp_save_header;
        tmp_save_header.offset = tmp_header.size;
        tmp_save_header.title_offset = sizeof(Sl2SaveFile::Header) + (sizeof(Sl2SaveFile::SaveSlotHeader) * Sl2SaveFile::SLOT_COUNT_DEFAULT);
        Sl2SaveFile::SaveSlotTitle tmp_save_title;
        memcpy_s(buff, Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT, &tmp_header, sizeof(Sl2SaveFile::Header));
        uint32_t tmp_offset = sizeof(Sl2SaveFile::Header);
        // Write non-zero data
        for (int i = 0; i < Sl2SaveFile::SLOT_COUNT_DEFAULT; i++) {
            // Write save slot header data
            if (i > 0) {
                tmp_save_header.offset += Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT;
                tmp_save_header.title_offset += sizeof(Sl2SaveFile::SaveSlotTitle);
                memcpy_s((void*)(((uint32_t)buff) + (tmp_save_header.offset - (12 + sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT)))), Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT - tmp_save_header.offset, Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT, sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT));
            }
            memcpy_s((void*)(((uint32_t)buff) + tmp_offset), Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT - tmp_offset, &tmp_save_header, sizeof(Sl2SaveFile::SaveSlotHeader));
            tmp_offset += sizeof(Sl2SaveFile::SaveSlotHeader);
            
            // Write save slot title data
            std::wstring index_str = L"0";
            if (i < 10) {
                index_str += L"0";
            }
            index_str += std::to_wstring(i);
            tmp_save_title.index[1] = index_str.c_str()[1];
            tmp_save_title.index[2] = index_str.c_str()[2];
            memcpy_s((void*)(((uint32_t)buff) + tmp_save_header.title_offset), Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT - tmp_save_header.title_offset, &tmp_save_title, sizeof(Sl2SaveFile::SaveSlotTitle));
        
            memcpy_s((void*)(((uint32_t)buff) + tmp_save_header.offset), Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT - tmp_save_header.offset, Sl2SaveFile::CHECKSUM_HEADER_DEFAULT, sizeof(Sl2SaveFile::CHECKSUM_HEADER_DEFAULT));
            *(uint32_t*)(((uint32_t)buff) + tmp_save_header.offset + sizeof(Sl2SaveFile::CHECKSUM_HEADER_DEFAULT)) = (Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT - (sizeof(Sl2SaveFile::CHECKSUM_HEADER_DEFAULT) + sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT)));
        }

        // Write MainMenuSlot footer checksum
        memcpy_s((void*)(((uint32_t)buff) + ((tmp_save_header.offset + Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT) - (12 + sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT)))), Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT - tmp_save_header.offset, Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT, sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT));

        FileUtil::dump_file(buff, Sl2SaveFile::SAVE_FILE_SIZE_DEFAULT, filename.c_str(), overwrite);
        CoTaskMemFree(buff);
        return ERROR_SUCCESS;
    }


    /*
        EXPERIMENTAL FUNCTION (UNFINISHED)

        Generates an empty save slot in the given buffer. If buffer is not NULL, it is assumed to be at least 393,248 bytes,
        which is the amount of memory this function uses to create an empty save slot. If buffer is NULL, 393,248 bytes of
        memory are allocated using CoTaskMemAlloc, and must be manually freed by the caller with CoTaskMemFree.

        If size is not NULL, the size (in bytes) of the generated save slot is stored.
    */
    static void *generate_empty_save_slot(void *buffer, uint32_t *size = NULL) {
        if (buffer == NULL) {
            buffer = CoTaskMemAlloc(Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT);
        }
        if (buffer == NULL) {
            // Failed to allocate memory
            SetLastError(ERROR_OUTOFMEMORY);
            if (size != NULL)
                (*size) = 0;
            return NULL;
        }

        // Zero-out the block of memory
        memset(buffer, 0, Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT);
        // Insert the header checksum for an empty save slot
        memcpy_s(buffer, Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT, Sl2SaveFile::CHECKSUM_HEADER_DEFAULT, sizeof(Sl2SaveFile::CHECKSUM_HEADER_DEFAULT));
        // Insert the footer checksum for an empty save slot
        memcpy_s((void*)(((uint32_t)buffer) + (Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT - (sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT) + 12))),
            (sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT) + 12), Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT, sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT));

        // Insert block length (length of data between checksums)
        *(uint32_t*)(((uint32_t)buffer) + sizeof(Sl2SaveFile::CHECKSUM_HEADER_DEFAULT)) = Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT - (sizeof(Sl2SaveFile::CHECKSUM_HEADER_DEFAULT) + sizeof(Sl2SaveFile::CHECKSUM_FOOTER_DEFAULT));



        if (size != NULL)
            (*size) = Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT;

        return buffer;
    }


    /*
        EXPERIMENTAL FUNCTION (UNFINISHED)
    */
    bool insert_new_save_slot(const char *new_file_name) {
        if (new_file_name == NULL) {
            SetLastError(ERROR_INVALID_ADDRESS);
            return false;
        }
        
        if (header->slot_count >= 999) {
            SetLastError(ERROR_TOO_MANY_NAMES);
            return false;
        }

        // Calculate offset necessary to align each save slot to 16 bytes
        uint32_t align_offset = 16 - ((header->size + sizeof(SaveSlotHeader) + sizeof(SaveSlotTitle)) % 16);

        // Allocate memory for new save file buffer
        uint32_t new_size = total_size() + sizeof(SaveSlotHeader) + sizeof(SaveSlotTitle) + Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT + align_offset;
        void *buffer = CoTaskMemAlloc(new_size);
        if (buffer == NULL) {
            SetLastError(ERROR_OUTOFMEMORY);
            return false;
        }

        // Calculate pointers to new save slot header/title blocks
        SaveSlotHeader *new_slot_header = &((SaveSlotHeader*)(((uint32_t)buffer) + sizeof(Header)))[header->slot_count - 1];
        SaveSlotHeader *new_end_slot_header = &((SaveSlotHeader*)(((uint32_t)buffer) + sizeof(Header)))[header->slot_count]; // New header for unusable trailing slot
        SaveSlotTitle *new_slot_title = (SaveSlotTitle*)(((uint32_t)new_end_slot_header) + sizeof(SaveSlotHeader) + (sizeof(SaveSlotTitle) * header->slot_count));

        // Zero-out new memory
        memset(buffer, 0, new_size);

        // Copy and update main header data
        memcpy_s(buffer, new_size, header, sizeof(Header) + (sizeof(SaveSlotHeader) * header->slot_count));
        ((Header*)buffer)->slot_count = header->slot_count + 1;
        ((Header*)buffer)->size += sizeof(SaveSlotHeader) + sizeof(SaveSlotTitle) + align_offset;
        
        // Copy and update save slot header data
        memcpy_s(new_end_slot_header, sizeof(SaveSlotHeader), &(slot_headers[header->slot_count - 1]), sizeof(SaveSlotHeader));
        new_slot_header->slot_size = Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT - 12;
        new_end_slot_header->offset = total_size() + sizeof(SaveSlotHeader) + sizeof(SaveSlotTitle) + align_offset + ((Sl2SaveFile::SAVE_SLOT_SIZE_DEFAULT - 12) - slot_headers[header->slot_count - 1].slot_size);
        new_end_slot_header->title_offset += sizeof(SaveSlotHeader) + sizeof(SaveSlotTitle);
        for (int i = 0; i < (int)header->slot_count; i++) {
            ((SaveSlotHeader*)(((uint32_t)buffer) + sizeof(Header)))[i].title_offset += sizeof(SaveSlotHeader);
            ((SaveSlotHeader*)(((uint32_t)buffer) + sizeof(Header)))[i].offset += sizeof(SaveSlotHeader) + sizeof(SaveSlotTitle) + align_offset;
        }

        // Copy and update save slot title data
        memcpy_s(new_end_slot_header + 1, sizeof(SaveSlotTitle) * header->slot_count, slot_titles, sizeof(SaveSlotTitle) * header->slot_count);
        
        // Build new save slot title string
        std::wstring new_slot_title_string = Sl2SaveFile::SLOT_TITLE_PREFIX_DEFAULT_W;
        if (header->slot_count + 1 < 100) {
            new_slot_title_string += L'0';
        }
        if (header->slot_count + 1 < 10) {
            new_slot_title_string += L'0';
        }
        new_slot_title_string += std::to_wstring(header->slot_count);
        memcpy_s(new_slot_title, sizeof(SaveSlotTitle), new_slot_title_string.c_str(), sizeof(SaveSlotTitle));

        // Copy existing save slots (but not last unusable slot)
        memcpy_s((void*)(((uint32_t)buffer) + ((Header*)buffer)->size), total_size() - slot_headers[0].offset, (void*)(((uint32_t)header) + slot_headers[0].offset), total_size() - slot_headers[0].offset - (slot_headers[header->slot_count - 1].slot_size + slot_headers[header->slot_count - 1].padding_size));


        // Generate new empty save slot
        uint32_t new_slot_size;
        Sl2SaveFile::generate_empty_save_slot((void*)(((uint32_t)buffer) + new_slot_header->offset), &new_slot_size);

        // Copy last unusable save slot
        memcpy_s((void*)(((uint32_t)buffer) + new_end_slot_header->offset), (new_end_slot_header->slot_size + new_end_slot_header->padding_size), (void*)(((uint32_t)header) + slot_headers[header->slot_count - 1].offset), (new_end_slot_header->slot_size + new_end_slot_header->padding_size));

        // Write new data to file
        FileUtil::dump_file(buffer, new_size, new_file_name);

        /*Sl2SaveFile file = Sl2SaveFile(buffer);
        for (int i = 0; i < (int)file.header->slot_count; i++) {
            print_console("Verifying checksums for save[" + std::to_string(i) + "]...");
            bool checksums_verified = file.saves[i].verify_checksums();
            if (checksums_verified)
                print_console("Success.");
            else
                print_console("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nChecksums did not match; verification failed\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            print_console("");
        }*/

        CoTaskMemFree(buffer);
        return true;
    }

} SaveFile;
typedef Sl2SaveFile Sl2;







#endif // _DS1_FILE_LIB_SL2_SAVE_FILE_H_
