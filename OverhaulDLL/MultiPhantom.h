/*
Authors:

Metal-Crow	-	Reverse engineering, CE, LUA, etc
Sean Pesce	-	C++ conversion

*/

#pragma once

#include "SP_SysUtils.hpp"
#include "SP_AsmInject_x86.hpp"
#include "SP_AoB_Scan.hpp"


extern void *ds1_base;


uint32_t forceshowsigns_returnhere;
uint32_t sucessful_phantomfix;
uint32_t summon_char_types_returnhere;
uint32_t summon_char_types_callproc;
uint32_t players_connected_array_offsets_callproc;
uint32_t players_connected_array_offsets_returnhere;


void apply_multiphantom_patch();

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

