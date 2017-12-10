/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
        Metal-Crow  -  Reverse engineering, Phantom Limit patch
		Sean Pesce  -  C++

*/

#pragma once

#ifndef _DS1_OVERHAUL_PHANTOM_LIMIT_H_
	#define _DS1_OVERHAUL_PHANTOM_LIMIT_H_


#include "SpPointer.h"


// Maximum number of phantoms allowed in the player's world
#define _MAX_SUMMONS_ 9



uint8_t max_allowed_summons8 = _MAX_SUMMONS_;
uint8_t max_allowed_summons32_arr[4] = { max_allowed_summons8, 0x0, 0x0, 0x0 }; // Use an array of 4 bytes to represent a DWORD for easy conversion between big/little endian
uint32_t *max_allowed_summons32 = (uint32_t*)max_allowed_summons32_arr;

uint32_t max_allowed_summons32_final = _MAX_SUMMONS_;

uint32_t pca_offset_add = 20 * (_MAX_SUMMONS_ - 4);  // For all pca_off offsets, do 20*(number_of_characters - 4)+orignal_offset

SpPointer max_allowed_summons_ptr;
uint32_t forceshowsigns_returnhere;
uint32_t sucessful_phantomfix;
uint32_t summon_char_types_returnhere;
uint32_t summon_char_types_callproc;
uint32_t players_connected_array_offsets_callproc;
uint32_t players_connected_array_offsets_returnhere;
void *BlackSosNum;
void *InvadeNum;



void __stdcall forceshowsigns_newmem();
void __stdcall summon_char_types_newmem();
void __stdcall players_connected_array_offsets();


uint32_t pca_off_ret1;
void __stdcall pca_off1();
uint32_t pca_off_ret2;
uint32_t pca_off_jmp2;
void __stdcall pca_off2();
uint32_t pca_off_ret3;
void __stdcall pca_off3();
uint32_t pca_off_ret4;
uint32_t pca_off_jmp4;
void __stdcall pca_off4();
uint32_t pca_off_ret5;
uint32_t pca_off_jmp5;
void __stdcall pca_off5();
uint32_t pca_off_ret6;
uint32_t pca_off_jmp6;
void __stdcall pca_off6();
uint32_t pca_off_ret7;
void __stdcall pca_off7();
uint32_t pca_off_ret8;
void __stdcall pca_off8();
uint32_t pca_off_ret9;
void __stdcall pca_off9();
uint32_t pca_off_ret10;
void __stdcall pca_off10();
uint32_t pca_off_ret11;
void __stdcall pca_off11();
uint32_t pca_off_ret12;
void __stdcall pca_off12();
uint32_t pca_off_ret13;
void __stdcall pca_off13();
uint32_t pca_off_ret14;
void __stdcall pca_off14();
uint32_t pca_off_ret15;
void __stdcall pca_off15();
uint32_t pca_off_ret16;
uint32_t pca_off_jmp16;
void __stdcall pca_off16();
uint32_t pca_off_ret17;
void __stdcall pca_off17();
uint32_t pca_off_ret18;
void __stdcall pca_off18();

#endif // _DS1_OVERHAUL_PHANTOM_LIMIT_H_