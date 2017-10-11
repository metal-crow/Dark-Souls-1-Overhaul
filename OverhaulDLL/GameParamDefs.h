/*
	DARK SOULS OVERHAUL

	Contributors to this file:
	Sean Pesce	-	C++


	GameParamDefs.h

	Declares data types, constants, variables, and functions used to parse
	or manipulate Dark Souls game param files.

	NOTE: This file is included inside of the Game class, so anything defined
	in this header must be externally referenced with the "Game::" prefix.


	References:
		- (?Author?) ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1KukblWL61We64-gNIyaAShga9h8RTXYmyFs98eQhY4E/

		- Kushisushi's ParamDefs Sheet
		  https://docs.google.com/spreadsheets/d/1QX6i93H4_jGZ0So9mArDECnuv1lfkaJQhXOzkDuxdkE/edit#gid=2133439974

*/

#pragma once

#ifndef _DS1_OVERHAUL_GAME_PARAM_STRUCTS_H_
	#define _DS1_OVERHAUL_GAME_PARAM_STRUCTS_H_



				/////////////////////////////////////////////////////
				///////////////// CONSTANTS & ENUMS /////////////////
				/////////////////////////////////////////////////////

enum DS1_PARAMETER_SIZE {
	PROTECTION_PARAM_SIZE = sizeof(ProtectorParam)
	// @TODO: Fill in other param data struct sizes
};
					
					
					
				/////////////////////////////////////////////////////
				///////////////// STRUCTS & CLASSES /////////////////
				/////////////////////////////////////////////////////

	
// Ancestor data struct for all Dark Souls game parameter data structure types
typedef struct DS1_PARAMETER {} Parameter;


// Represents a Dark Souls parameter definitions file
class ParamDefFile {
public:
	ParamDefFile::ParamDefFile(void *new_base, int32_t new_off, size_t new_element_count, DS1_PARAMETER_SIZE new_element_size, std::string aob)
	{
		base = new_base;
		data_offset = new_off;
		param_count = new_element_count;
		param_size = new_element_size;
		unique_pattern = aob;
	}
	ParamDefFile::ParamDefFile(){}
	ParamDefFile::~ParamDefFile(){}
	void *base = NULL;   // Starting address of the parameters file
	int32_t data_offset = 0; // Offset (relative to base address) to the start of the param structure array
	size_t param_count = 0; // Number of parameter structures in the data array
	DS1_PARAMETER_SIZE param_size; // Size of a single parameter structure from the data array
	std::string unique_pattern; // Unique array of bytes that can be used to find the param file // @TODO: Find better technique
	
	// Initialize the Protector Params definition file base pointer. Returns ERROR_SUCCESS on success, and a non-zero error code otherwise
	int ParamDefFile::init(bool print_result = false, bool re_init = false)
	{
		if(base != NULL && !re_init)
			return ERROR_SUCCESS;

		if (print_result)
			print_console(std::string("Searching for param def file...").c_str());

		base = aob_scan(unique_pattern.c_str());

		if (base != NULL && print_result)
		{
			std::stringstream hex_stream;
			hex_stream << std::hex << (int)base;
			print_console(std::string("    Located param def file (start: 0x").append(hex_stream.str()).append(")").c_str());
		}
		else if (print_result)
		{
			print_console(std::string("    Failed to locate parameter data.").c_str());
		}

		if (base == NULL)
			return ERROR_FILE_NOT_FOUND;

		return ERROR_SUCCESS;
	}

	// Returns the start of the parameter file data array (array element 0)
	Parameter *ParamDefFile::data()
	{ 
		return (Parameter*)(base == NULL ? NULL : (((int8_t*)base) + data_offset)); 
	}

	// Returns param struct at the specified index in the data array
	Parameter *ParamDefFile::get_by_index(int index)
	{
		if (index < 0 || (size_t)index >= param_count || data() == NULL)
			return NULL;
		return (Parameter *)(((int8_t*)data()) + (param_size * index));
	}
};



// Note: The immediate values in the struct below are bit field definitions (number of bits each variable consists of)
typedef struct DS1_PROTECTOR_PARAM_FLAGS {
		
	uint64_t isDeposit:    1, // "Will it be deposited in the warehouse?" / Denotes if item can be put into bottomless box?
			    headEquip:    1, // "Head equipment?"
			    bodyEquip:    1, // "Is it equipped with a cylinder?" / Body equipment
			    armEquip:     1, // "Arm armor?" / Hands/gloves armor
			    legEquip:     1, // "Is it a leg fitting?" / Pants/Leggings/Boots/Shoes/Foot armor
			    useFaceScale: 1, // "Do you use face scales?"
			
			

		/*
			Key for invisibleFlag functions (incomplete):

			These flags toggle rendering of specific body parts.

			0="Bangs ahead", 1="Bottom of bangs", 2="Sideburns", 3="Head top", 4="Head top", 5="Back hair" (back of head?), 6="Behind the back", 7="?", 8="?",
			9="", 10="Collar", 11="Around the collar", 12="?", 13="?", 14="?", 15="Hem/no hem", 16="?", 17="?", 18="?", 19="?", 20="Sleeve A", 21="Sleeve B",
			22="?", 23="?", 24="?", 25="Arm", 26="?", 27="?", 28="?", 29="?", 30="Belt", 31="?", 32="?", 33="?", 34="?", 35="?", 36="?", 37="?", 38="?",
			39="?", 40="?", 41="?", ="?", 42="?", 43="?", 44="?", 45="?", 46="?", 47="?"
		*/ 
			
		// Unfortunately C++ doesn't support bit fields in an array, so this will have to do
		invisibleFlag0:  1, invisibleFlag1:  1, invisibleFlag2:  1, invisibleFlag3:  1, invisibleFlag4:  1, invisibleFlag5:  1, invisibleFlag6:  1, invisibleFlag7:  1, invisibleFlag8:  1,
		invisibleFlag9:  1, invisibleFlag10: 1, invisibleFlag11: 1, invisibleFlag12: 1, invisibleFlag13: 1, invisibleFlag14: 1, invisibleFlag15: 1, invisibleFlag16: 1, invisibleFlag17: 1,
		invisibleFlag18: 1, invisibleFlag19: 1, invisibleFlag20: 1, invisibleFlag21: 1, invisibleFlag22: 1, invisibleFlag23: 1, invisibleFlag24: 1, invisibleFlag25: 1, invisibleFlag26: 1,
		invisibleFlag27: 1, invisibleFlag28: 1, invisibleFlag29: 1, invisibleFlag30: 1, invisibleFlag31: 1, invisibleFlag32: 1, invisibleFlag33: 1, invisibleFlag34: 1, invisibleFlag35: 1,
		invisibleFlag36: 1, invisibleFlag37: 1, invisibleFlag38: 1, invisibleFlag39: 1, invisibleFlag40: 1, invisibleFlag41: 1, invisibleFlag42: 1, invisibleFlag43: 1, invisibleFlag44: 1,
		invisibleFlag45: 1, invisibleFlag46: 1, invisibleFlag47: 1,
			
		disableMultiDropShare: 1, // "Does multi-drop sharing prohibited?" / "Disable Multiplayer Drop Share" (?)
		simpleModelForDlc:     1, // "Does a simple model exist for DLC?"
		pad:                   8; // Unused padding bits
} ProtectorParamFlags;


typedef struct DS1_PROTECTOR_PARAM : public Parameter { // Armor
	int32_t  sortId = -1;
	uint32_t wanderingEquipId = 950000; // "Replacement equipment ID for the wandering ghost."
	int32_t  vagrantItemLotId = -1; // "-1: no beigrant 0: no drawing 1: ~ lottery"
	int32_t  vagrantBonusEneDropItemLotId = -1; // "Vagrant Bonus Enemy Drop Item Lot ID" / "-1: No drop 0: no drawing 1: to be selected"
	int32_t  vagrantItemEneDropItemLotId = -1;  // "Vagrant Item  Enemy Drop Item Lot ID" / "-1: No drop 0: no drawing 1: to be selected"
	int32_t  repairPrice;
	int32_t  buyPrice; // "Basic price"
	int32_t  sellValue;
	float    weight; // kg
	// "Resident Special Effect Ids" are not in an array to avoid confusion because indexing starts at 1 (really FromSoft???)
	int32_t  residentSpEffectId  = -1;
	int32_t  residentSpEffectId2 = -1;
	int32_t  residentSpEffectId3 = -1;
	int32_t  materialSetId = -1; // "Material parameter ID required to strengthen weapons"
	float    partsDamageRate = 1.0; // "Site damage rate"
	float    corectSARecover; // "Correct Super Armor Recover" (Note: "Correct" is spelled wrong) / "Super armor recovery time correction value"
	// Key for Origin Equip Pro values:
	// 0="Enhancement source armor ID", 1="Uses reinforcement armor ID 1", 2="Uses reinforcement armor ID 2", 3="Uses reinforcement armor ID 3",
	// 4="Uses reinforcement armor ID 4", 5="Uses reinforcement armor ID 5", 6="Uses reinforcement armor ID 6", 7="Uses reinforcement armor ID 7",
	// 8="Uses reinforcement armor ID 8", 9="Uses reinforcement armor ID 9", 10="Uses reinforcement armor ID 10", 11="Uses reinforcement armor ID 11",
	// 12="Uses reinforcement armor ID 12", 13="Uses reinforcement armor ID 13", 14="Uses reinforcement armor ID 14", 15="Uses reinforcement armor ID 15"
	int32_t  originEquipPro[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	float    faceScaleM_ScaleX;     // Male
	float    faceScaleM_ScaleZ;     // Male
	float    faceScaleM_MaxX = 1.0; // Male
	float    faceScaleM_MaxZ = 1.0; // Male
	float    faceScaleF_ScaleX;     // Female
	float    faceScaleF_ScaleZ;     // Female
	float    faceScaleF_MaxX = 1.0; // Female
	float    faceScaleF_MaxZ = 1.0; // Female
	int32_t  qwcId = -1; // "QWC parameter ID"
	uint16_t equipModelId = 1;
	uint16_t iconIdM; // "Male menu icon ID"
	uint16_t iconIdF; // "Female menu icon ID"
	uint16_t knockBack; // "Decrease value of knockback"
	uint16_t knockbackBounceRate; // "Knock back rebound rate"
	uint16_t durability; // "Initial durability"
	uint16_t durabilityMax; // "New article durability"
	int16_t  saDurability; // "Super armor endurance"
	uint16_t defenseFlickPower; // "Used to judge the rebound of enemy's attack"
	uint16_t defensePhysical; // Physical defense
	uint16_t defenseMagic; // Magic defense
	uint16_t defenseFire; // Fire defense
	uint16_t defenseThunder; // Lightning defense
	int16_t  defenseSlash; // Slash defense
	int16_t  defenseBlow; // Strike defense
	int16_t  defenseThrust; // Thrust defense
	uint16_t resistPoison; // Poison resistance
	uint16_t resistDisease; // Toxic resistance
	uint16_t resistBlood; // Bleed resistance
	uint16_t resistCurse; // Curse resistance
	int16_t  reinforceTypeId; // "Enhanced Type ID"
	int16_t  trophySGradeId = -1; // "Related to the trophy system?" / Something to do with achievements (?)
	int16_t  shopLv = -1; // "Level that can be sold at a store" / "Shop level" (?)
	uint8_t  knockbackParamId = 1; // "ID of parameter used for knockback"
	uint8_t  flickDamageCutRate = 255; // "Used for damage attenuation rate at repelling"
	uint8_t  equipModelCategory = 5; // "Type of equipment model"
	uint8_t  equipModelGender = 1; // "Gender of the equipment model"
	uint8_t  protectorCategory; // "Category of armor"
	uint8_t  defenseMaterial = 59; // "For SE for movement/defense" (SE = sfx/sound effects? Zullie armor sounds toggle script works by setting this to 0)
	uint8_t  defenseMaterialSfx = 59; // "Defense Material Sound Effects" / "For SFX when moving / defending"
	uint8_t  partsDmgType; // "Parts Damage Type" / "Set the attack type to perform site damage judgment"
	uint8_t  defenseMaterial_Weak = 29; // "For Weak Points Damaged SE" (SE = sfx/sound effects? Zullie armor sounds toggle script works by setting this to 0)
	uint8_t  defenseMaterialSfx_Weak = 29; // "Defense Material Sound Effects (Weak)" / "For Weak Points Damaged SE"
	ProtectorParamFlags flags; // See "ProtectorParamFlags" structure definition (above)
	int16_t  oldSortId = -1; // "Old sort ID (-1 = not collected)"
	uint8_t  pad[6] = { 0, 0, 0, 0, 0, 0 }; // Unused padding bits
} ProtectorParam;



					/////////////////////////////////////////////////
					///////////////// VARIABLE DATA /////////////////
					/////////////////////////////////////////////////


// Dark Souls ProtectorParams file (contains armor parameters)
static ParamDefFile protector_params;



						/////////////////////////////////////////////
						///////////////// FUNCTIONS /////////////////
						/////////////////////////////////////////////



/*
	Obtains the address of the ProtectorParam struct with the given sort ID (or NULL if the specified
		sort ID is invalid) and stores the address in the supplied parameter pointer.

	Returns ERROR_SUCCESS if successful, otherwise returns a non-zero error code.
*/
static uint32_t get_protector_param_by_sort_id(Game::ProtectorParam** param, int32_t sort_id);




#endif // _DS1_OVERHAUL_GAME_PARAM_STRUCTS_H_