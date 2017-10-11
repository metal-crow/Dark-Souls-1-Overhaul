/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Sean Pesce	-	C++


	GameParamIDs.h

		Param definition data utilized by the mod.

*/

#pragma once

#ifndef _DS1_OVERHAUL_GAME_PARAM_DATA_H_
	#define _DS1_OVERHAUL_GAME_PARAM_DATA_H_


// Protector param definition file
Game::ParamDefFile Game::protector_params = Game::ParamDefFile(NULL, 0x30, 324, PROTECTION_PARAM_SIZE,
										"80 48 28 00 60 31 01 00 9B 45 01 00 68 4C 28 00 48 32 01 00 AA 45 01 00 50 50 28 00 30 33 01 00");





/*
    Obtains the address of the ProtectorParam struct with the given sort ID (or NULL if the specified
      sort ID is invalid) and stores the address in the supplied parameter pointer.

    Returns ERROR_SUCCESS if successful, otherwise returns a non-zero error code.
*/
#define PROT_PARAM_LAST_UNIQUE_SORT_ID ((int32_t)243)
uint32_t Game::get_protector_param_by_sort_id(Game::ProtectorParam** param, int32_t sort_id)
{
	if(param == NULL)
		return ERROR_INVALID_ADDRESS;

	Game::protector_params.init(true);

	int i = 0;
	(*param) = NULL;
	
	if (sort_id < -1)
		return ERROR_INVALID_PARAMETER;

	// Get first element in data array
	(*param) = (Game::ProtectorParam*)Game::protector_params.data();

	// Iterate through array of protector parameters
	while ((*param)->sortId != sort_id && i < (int)Game::protector_params.param_count)
	{
		(*param)++;
		i++;
	}

	if((*param)->sortId == sort_id)
		return ERROR_SUCCESS;

	(*param) = NULL;
	return ERROR_FILE_NOT_FOUND;


	#ifdef _DS1_OVERHAUL_MOD_DBG_
		
		// Additional debug build definitions
		#include "GameParamDataDebug.h"

	#endif // _DS1_OVERHAUL_MOD_DBG_
}





#endif // _DS1_OVERHAUL_GAME_PARAM_DATA_H_