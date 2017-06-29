/*
	Authors:

	Metal-Crow	-	Reverse engineering, CE, LUA, etc
	Sean Pesce	-	C++ conversion

*/

#include "MultiPhantom.h"
#include "PreprocessorArithmetic.h"

// Applies the multiphantom patch, which increases the maximum number of simultaneous phantoms allowed in a session
void apply_multiphantom_patch()
{

	max_allowed_summons_ptr = SpPointer((void *)0x0137D82C, { 0x08C });

	// (Desired num of phantoms * 0x20)
	uint8_t patch1[5] = { 0x68, 0x20, 0x01, 0x00, 0x00 }; // push 0x120
	void *write_address = (uint8_t*)ds1_base + 0xA63CA6;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch1, 5);

	write_address = (uint8_t*)ds1_base + 0xA63CBA;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch1, 5);

	
	uint8_t patch2[6] = { 0xC7, 0x07, 0x09, 0x00, 0x00, 0x00 }; // mov DWORD PTR [edi],0x9		// Number of character slots
	write_address = (uint8_t*)ds1_base + 0xA63CC1;
	set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch2, 6);


	// One of these governs red signs, another white (Probably some other around governs dragon)
	uint8_t patch3[2] = { 0x6A, 0x09 }; // push 0x9		// Max number of summons before signs disappear
	write_address = (uint8_t*)ds1_base + 0x9B51B7;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2);

	write_address = (uint8_t*)ds1_base + 0x9B51EB;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2);


	// Check that sums up all phantoms
	uint8_t patch4[3] = { 0x83, 0xFA, 0x3F }; // cmp edx,0x3F		// This is more than number of phantoms, but not sure by how much (num * 7 ?)
	write_address = (uint8_t*)ds1_base + 0x9B11BE;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch4, 3);


	// Not 100% on these ones
	uint8_t patch5[4] = { 0x83, 0x78, 0x10, 0x09 }; // cmp DWORD PTR [eax+0x10],0x9
	write_address = (uint8_t*)ds1_base + 0x92A47C;
	set_mem_protection(write_address, 4, MEM_PROTECT_RWX);
	memcpy_s(write_address, 4, patch5, 4);

	uint8_t patch6[7] = { 0x83, 0xB8, 0x90, 0x00, 0x00, 0x00, 0x09 }; // cmp DWORD PTR [eax+0x90],0x9
	write_address = (uint8_t*)ds1_base + 0x92A465;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch6, 7);

	
	// "I dunno"
	// - Metal-Crow, 2017
	write_address = (uint8_t*)ds1_base + 0x9B1271;
	set_mem_protection(write_address, 10, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &forceshowsigns_returnhere, 1, forceshowsigns_newmem);


	// Allocation of the summon_chars_data array put in the info_about_summons struct
	uint8_t patch7[5] = { 0x68, 0xD0, 0x2A, 0x00, 0x00 }; // push 0x2AD0		// 16 + 1216*number of character slots
	write_address = (uint8_t*)ds1_base + 0x806848;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch7, 5);


	// Counter limit for initing all items in summon_chars_data array
	uint8_t patch8[3] = { 0x83, 0xFE, 0x09 }; // cmp esi,0x9
	write_address = (uint8_t*)ds1_base + 0x8068A4;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch8, 3);

	uint8_t patch9[6] = { 0x81, 0xFB, 0xC0, 0x2A, 0x00, 0x00 }; // cmp ebx,0x2AC0		// This is not index, but byte offset. So 1216*number of character slots
	write_address = (uint8_t*)ds1_base + 0x806ADE;
	set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch9, 6);


	// Believe this is length of summon_chars_data array, stored in struct
	uint8_t patch10[7] = { 0xC7, 0x41, 0xFC, 0x09, 0x00, 0x00, 0x00 }; // mov DWORD PTR [ecx-0x4],0x9
	write_address = (uint8_t*)ds1_base + 0x8068A9;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch10, 7);


	// Bounds checks
	uint8_t patch11[3] = { 0x83, 0xFA, 0x09 }; // cmp edx,0x9
	write_address = (uint8_t*)ds1_base + 0x8037A7;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch11, 3);

	uint8_t patch12[6] = { 0x81, 0xF9, 0xC0, 0x2A, 0x00, 0x00 }; // cmp ecx,0x2AC0		// 1216*number of chars
	write_address = (uint8_t*)ds1_base + 0x8037BF;
	set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch12, 6);

	uint8_t patch13[3] = { 0x83, 0xF8, 0x09 }; // cmp eax,0x9
	write_address = (uint8_t*)ds1_base + 0x802CC0;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch13, 3);


	// Size of summon_char_types array in info_about_summons struct
	write_address = (uint8_t*)ds1_base + 0x8068C3; 
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2); // push 0x9


	// Write to a variable to alert us if inject was done in time
	sucessful_phantomfix = 0; // Set success marker to false
	summon_char_types_callproc = (uint32_t)((uint8_t*)ds1_base + 0x1629C0);

	// Init rest of the summon_char_types array to zero
	write_address = (uint8_t*)ds1_base + 0x8068DF;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &summon_char_types_returnhere, 3, summon_char_types_newmem);



	// Hardcoded limits for summon_char_types length (some are <=)
	uint8_t patch14[3] = { 0x83, 0xF8, 0x09 }; // cmp eax,0x09
	write_address = (uint8_t*)ds1_base + 0x802CB7;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch14, 3);

	uint8_t patch15[3] = { 0x83, 0xF9, 0x08 }; // cmp ecx,0x08
	write_address = (uint8_t*)ds1_base + 0x802C80;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch15, 3);

	uint8_t patch16[3] = { 0x83, 0xFB, 0x08 }; // cmp ebx,0x08
	write_address = (uint8_t*)ds1_base + 0x92E7A9;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch16, 3);

	write_address = (uint8_t*)ds1_base + 0x92E6AF; // cmp ebx,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch16, 3);

	uint8_t patch17[3] = { 0x83, 0xFE, 0x08 }; // cmp esi,0x08
	write_address = (uint8_t*)ds1_base + 0x92FB73;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch17, 3);

	uint8_t patch18[3] = { 0x83, 0xFF, 0x08 }; // cmp edi,0x08
	write_address = (uint8_t*)ds1_base + 0x92FDF1;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch18, 3);

	uint8_t patch19[3] = { 0x83, 0xF8, 0x08 }; // cmp eax,0x08
	write_address = (uint8_t*)ds1_base + 0x930431;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch19, 3);

	write_address = (uint8_t*)ds1_base + 0x931C66; // cmp esi,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch17, 3);

	write_address = (uint8_t*)ds1_base + 0x928135; // cmp eax,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch19, 3);

	write_address = (uint8_t*)ds1_base + 0x92B19A; // cmp edi,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch18, 3);

	write_address = (uint8_t*)ds1_base + 0xAE2B8A; // cmp eax,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch19, 3);



	// Size to init an array of char pointers to
	write_address = (uint8_t*)ds1_base + 0x322A00;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2); // push 0x9


	/*
	// Number of black signs visible
	// This aob is in memory for the defaults dark souls normally sets. Will not work with we change
	//  - BlackSos.[First/Second/Third]LevelRange[Max/Min]
	void *BlackSosNum = aob_scan("05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00");
	while (BlackSosNum == NULL)
	{
		Sleep(100);
		BlackSosNum = aob_scan("05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00");
	}
	set_mem_protection(BlackSosNum, 4, MEM_PROTECT_RWX);
	*(uint32_t*)BlackSosNum = 0x08000000;


	// Something to do with number of allowed invaders? just adding to be safe
	// This aob is in memory for the defaults dark souls normally sets. Will not work with we change
	// - Invade.[First/Second/Third]LevelRange[Max/Min]
	void *InvadeNum = aob_scan("05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00");
	while (InvadeNum == NULL)
	{
		Sleep(100);
		InvadeNum = aob_scan("05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00");
	}
	set_mem_protection(InvadeNum, 4, MEM_PROTECT_RWX);
	*(uint32_t*)InvadeNum = 0x08000000;
	*/

	// Max connection tickets
	// Alt AOB: 05 00 00 00 00 00 90 41 00 00 10 42 00 00 A0 40 00 00 20 41 00 00 A0 41
	write_address = (uint8_t*)ds1_base + 0xEDE2CC;
	set_mem_protection(write_address, 4, MEM_PROTECT_RWX);
	*(uint32_t*)write_address = 0x08000000;


	// Lobby Access Max Count
	uint8_t patch20[7] = { 0xC7, 0x86, 0xD4, 0x00, 0x00, 0x00, 0x08 }; // mov DWORD PTR [esi+0xd4],0x8
	write_address = (uint8_t*)ds1_base + 0x7E9AD1;
	set_mem_protection(write_address, 10, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch20, 7);


	// In initing players_connected_array, number of chars to init
	write_address = (uint8_t*)ds1_base + 0xAA24AA;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2); // push 0x9


	// For mallocing players_connected_array: 24+20*(number of characters)+36
	uint8_t patch21[5] = { 0x68, 0xF0, 0x00, 0x00, 0x00 }; // push 0xF0
	write_address = (uint8_t*)ds1_base + 0xABB55D;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch21, 5);


	// In initing players_connected_array, offsets after newly updated number of chars to init
	players_connected_array_offsets_callproc = (uint32_t)((uint8_t*)ds1_base + 0xAA2320);
	players_connected_array_offsets_returnhere = (uint32_t)((uint8_t*)ds1_base + 0xAA24FF);

	write_address = (uint8_t*)ds1_base + 0xAA24D2;
	set_mem_protection(write_address, 128, MEM_PROTECT_RWX);
	uint32_t dummy_ret;
	inject_jmp_5b((uint8_t*)write_address, &dummy_ret, 4, players_connected_array_offsets);



	// The number of char slots iterated through the players_connected_array (functions that reference it)

	uint8_t patch22[7] = { 0xC7, 0x44, 0x24, 0x10, 0x09, 0x00, 0x00 }; // mov DWORD PTR [esp+0x10],0x00000009
	write_address = (uint8_t*)ds1_base + 0xAA1A86;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch22, 7);

	uint8_t patch23[7] = { 0xC7, 0x44, 0x24, 0x14, 0x09, 0x00, 0x00 }; // mov DWORD PTR [esp+0x14],0x00000009
	write_address = (uint8_t*)ds1_base + 0xAA2FDE;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch23, 7);

	uint8_t patch24[3] = { 0x83, 0xFA, 0x09 }; // cmp edx,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2554;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch24, 3);

	uint8_t patch25[3] = { 0x83, 0xFB, 0x09 }; // cmp ebx,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2569;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch25, 3);

	uint8_t patch26[3] = { 0x83, 0xFE, 0x09 }; // cmp esi,0x09
	write_address = (uint8_t*)ds1_base + 0xC12C71;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch26, 3);

	uint8_t patch27[7] = { 0xC7, 0x44, 0x24, 0x14, 0x09, 0x00, 0x00 }; // mov DWORD PTR [esp+0x14],0x00000009
	write_address = (uint8_t*)ds1_base + 0xAA2932;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch27, 7);

	uint8_t patch28[7] = { 0xC7, 0x44, 0x24, 0x14, 0x09, 0x00, 0x00 }; // mov DWORD PTR [esp+0x14],0x00000009
	write_address = (uint8_t*)ds1_base + 0xAA2C8E;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch28, 7);

	uint8_t patch29[3] = { 0x83, 0xF8, 0x09 }; // cmp eax,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2A2C;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch29, 3);

	uint8_t patch30[3] = { 0x83, 0xFF, 0x09 }; // cmp edi,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2C06;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch30, 3);

	uint8_t patch31[3] = { 0x83, 0xF8, 0x09 }; // cmp eax,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2C18;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch31, 3);

	//^ but number of phantom slots
	uint8_t patch32[3] = { 0x83, 0xF9, 0x08 }; // cmp ecx,0x08
	write_address = (uint8_t*)ds1_base + 0xAA1788;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch32, 3);

	uint8_t patch33[5] = { 0xB9, 0x08, 0x00, 0x00, 0x00 }; // mov ecx,0x00000008
	write_address = (uint8_t*)ds1_base + 0xAA1798;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch33, 5);

	uint8_t patch34[3] = { 0x8D, 0x5F, 0x08 }; // lea ebx,[edi+0x08]
	write_address = (uint8_t*)ds1_base + 0xAA2BFF;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch34, 3);



	// Where the variables after the inline character array in the players_connected_array
	// whose offsets have changed, are referenced
	// OFFSET+=num additional chars*0x20
	write_address = (uint8_t*)ds1_base + 0xA6615A;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret1, 1, pca_off1);

	write_address = (uint8_t*)ds1_base + 0xA5B900;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp2 = (uint32_t)((uint8_t*)ds1_base + 0xA5B939);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret2, 1, pca_off2);

	write_address = (uint8_t*)ds1_base + 0x9C83DB;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret3, 2, pca_off3);

	write_address = (uint8_t*)ds1_base + 0x9C6BBE;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp4 = (uint32_t)((uint8_t*)ds1_base + 0x9C6BDB);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret4, 1, pca_off4);

	write_address = (uint8_t*)ds1_base + 0x92A44A;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp5 = (uint32_t)((uint8_t*)ds1_base + 0x92A453);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret5, 1, pca_off5);

	write_address = (uint8_t*)ds1_base + 0x9B53FB;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp6 = (uint32_t)((uint8_t*)ds1_base + 0x9B5405);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret6, 1, pca_off6);

	write_address = (uint8_t*)ds1_base + 0x93FEEE;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret7, 4, pca_off7);

	write_address = (uint8_t*)ds1_base + 0xAA1771;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret8, 1, pca_off8);

	write_address = (uint8_t*)ds1_base + 0xAA1791;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret9, 2, pca_off9);

	write_address = (uint8_t*)ds1_base + 0xAA17AD;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret10, 1, pca_off10);

	write_address = (uint8_t*)ds1_base + 0xAA29A9;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret11, 2, pca_off11);

	write_address = (uint8_t*)ds1_base + 0xAA3141;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret12, 1, pca_off12);

	write_address = (uint8_t*)ds1_base + 0xAA3268;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret13, 1, pca_off13);

	write_address = (uint8_t*)ds1_base + 0xAA18E2;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret14, 0, pca_off14);

	write_address = (uint8_t*)ds1_base + 0xAA18E7;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret15, 1, pca_off15);

	write_address = (uint8_t*)ds1_base + 0xAA2748;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp16 = (uint32_t)((uint8_t*)ds1_base + 0xAA27A9);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret16, 1, pca_off16);

	write_address = (uint8_t*)ds1_base + 0xAA27A1;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret17, 2, pca_off17);

	write_address = (uint8_t*)ds1_base + 0xAA2740;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret18, 1, pca_off18);


	uint8_t patch35[6] = { 0x8B, 0x8F, 0xE4, 0x00, 0x00, 0x00 }; // mov ecx, DWORD PTR [edi+0xe4]
	write_address = (uint8_t*)ds1_base + 0xAA277A;
	set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch35, 6);
}

void apply_multiphantom_secondary_patch()
{
	// Number of black signs visible
	// This aob is in memory for the defaults dark souls normally sets. Will not work with we change
	//  - BlackSos.[First/Second/Third]LevelRange[Max/Min]
	BlackSosNum = aob_scan("05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00");
	while (BlackSosNum == NULL)
	{
		Sleep(100);
		BlackSosNum = aob_scan("05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00");
	}
	set_mem_protection(BlackSosNum, 4, MEM_PROTECT_RWX);
	*(uint32_t*)BlackSosNum = 0x08000000;


	// Something to do with number of allowed invaders? just adding to be safe
	// This aob is in memory for the defaults dark souls normally sets. Will not work with we change
	// - Invade.[First/Second/Third]LevelRange[Max/Min]
	InvadeNum = aob_scan("05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00");
	while (InvadeNum == NULL)
	{
		Sleep(100);
		InvadeNum = aob_scan("05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00");
	}
	set_mem_protection(InvadeNum, 4, MEM_PROTECT_RWX);
	*(uint32_t*)InvadeNum = 0x08000000;
}


/*
	summon_char_types_newmem:
	push eax
	push ebx  // Copy of base ptr
	mov  eax, 2 // Counter
	startloop:
	cmp  eax, 9 // MAXCHARACTERS
	je  endloop
	mov  ebx, edx
	add  ebx, eax
	mov  [ebx], bl
	inc  eax
	jmp  startloop
	endloop:
	pop  ebx
	pop  eax

	mov[sucessful_phantomfix], 1	// Success marker
	call summon_char_types_callproc
	jmp summon_char_types_returnhere
*/
void __declspec(naked) __stdcall summon_char_types_newmem()
{
	__asm
	{
		/*
		// Old version:
		mov [edx + 0x02], bl
		mov [edx + 0x03], bl
		mov [edx + 0x04], bl
		mov [edx + 0x05], bl
		mov [edx + 0x06], bl
		mov [edx + 0x07], bl
		mov [edx + 0x08], bl // Must write for each offset up until max_allowed_phantoms-1
		mov[sucessful_phantomfix], 1	// Success marker
		call summon_char_types_callproc
		jmp summon_char_types_returnhere
		*/

		push eax
		push ebx  // Copy of base ptr
		mov  eax, 2 // Counter
		startloop:
		//cmp  eax, 9 // MAXCHARACTERS
		cmp  eax, max_allowed_summons32_final
		je  endloop
		mov  ebx, edx
		add  ebx, eax
		mov[ebx], bl
		inc  eax
		jmp  startloop
		endloop:
		pop  ebx
		pop  eax

		mov [sucessful_phantomfix], 1	// Success marker
		call summon_char_types_callproc
		jmp summon_char_types_returnhere
	}
}


// For all offsets, do 20*(number_of_characters - 4)+orignal_offset		(original found in DISABLE script)
void __declspec(naked) __stdcall players_connected_array_offsets()
{
	__asm
	{
		// Disable:
		// mov [esi + 0x74],ebx
		// mov[esi + 0x00000088], ebx

		// Full original code:
		/*
			mov     [esi+0x74], ebx
			mov     [esi+0x88], ebx
			or      edi, 0x0FFFFFFFF
			push    ecx
			mov     [esi+0x78], ebx
			mov     [esi+0x7C], edi
			mov     [esi+0x80], edi
			mov     [esi+0x84], edi
			call    sub_EA2320
			mov     [esi+0x68], edi
			mov     [esi+0x6C], edi
			mov     [esi+0x70], edi
		*/
		push eax
		mov eax, pca_offset_add
		add eax, 0x74
		add eax, esi
		mov [eax], ebx

		mov eax, pca_offset_add
		add eax, 0x88
		add eax, esi
		mov [eax], ebx
		pop eax

		or edi, -0x01
		push ecx

		push eax
		mov eax, pca_offset_add
		add eax, 0x78
		add eax, esi
		mov [eax], ebx

		mov eax, pca_offset_add
		add eax, 0x7C
		add eax, esi
		mov [eax], edi

		mov eax, pca_offset_add
		add eax, 0x80
		add eax, esi
		mov [eax], edi

		mov eax, pca_offset_add
		add eax, 0x84
		add eax, esi
		mov [eax], edi
		pop eax

		call players_connected_array_offsets_callproc

		push eax
		mov eax, pca_offset_add
		add eax, 0x68
		add eax, esi
		mov [eax], edi

		mov eax, pca_offset_add
		add eax, 0x6C
		add eax, esi
		mov [eax], edi

		mov eax, pca_offset_add
		add eax, 0x70
		add eax, esi
		mov [eax], edi
		pop eax

		jmp players_connected_array_offsets_returnhere
		

	}
}



// Unused in dynamic version
void __declspec(naked) __stdcall forceshowsigns_newmem()
{
	__asm
	{
		cmp eax, 0x9
		jmp forceshowsigns_returnhere
	}
}



// For all pca_off offsets, do 20*(number_of_characters - 4)+orignal_offset		(original found in DISABLE script)


// Where the variables after the inline character array in the players_connected_array
// whose offsets have changed, are referenced
// OFFSET+=num additional chars*0x20
void __declspec(naked) __stdcall pca_off1()
{
	// Disable:
	/*
		cmp [eax+0x7C], esi
	*/

	__asm
	{
		// For all pca_off offsets, do 20*(number_of_characters - 4)+orignal_offset		(original found in DISABLE script)
		/*
		Original:

		cmp dword ptr [eax + 0xE0], esi
		setne al
		jmp pca_off_ret1
		*/
		push edx
		mov edx, pca_offset_add
		add edx, 0x7C
		add edx, eax
		cmp dword ptr[edx], esi
		pop edx
		setne al
		jmp pca_off_ret1
	}
}
void __declspec(naked) __stdcall pca_off2()
{
	// Disable:
	/*
		cmp dword ptr [eax+0x7C], -0x01
	*/
	__asm
	{
		/*
			Original function:

			cmp dword ptr [eax + 0xE0], -0x01
			je pca_off_jmp2
			jmp pca_off_ret2

			Unfortunately inline ASM is extremely finnicky with the JE/JNE instructions

		*/

		/*
		cmp dword ptr[eax + 0xE0], -0x01
		je $+0x7
		jmp pca_off_ret2
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		jmp pca_off_jmp2
		*/
		push edx
		mov edx, pca_offset_add
		add edx, 0x7C
		add edx, eax
		cmp dword ptr[edx], -0x01
		pop edx
		je $ + 0x7
		jmp pca_off_ret2
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		jmp pca_off_jmp2
	}
}
void __declspec(naked) __stdcall pca_off3()
{
	// Disable:
	/*
		cmp dword ptr [eax+0x7C], -0x01
	*/
	__asm
	{
		push edx
		mov edx, pca_offset_add
		add edx, 0x7C
		add edx, eax
		cmp dword ptr [edx], -0x01
		pop edx
		setne al
		jmp pca_off_ret3
	}
}
void __declspec(naked) __stdcall pca_off4()
{
	// Disable:
	/*
		cmp dword ptr [eax+0x7C], -0x01
	*/
	__asm
	{
		/*
			Original function:

			cmp dword ptr [eax + 0xE0], -0x01
			jne pca_off_jmp4
			jmp pca_off_ret4

		*/
		push edx
		mov edx, pca_offset_add
		add edx, 0x7C
		add edx, eax
		cmp dword ptr[edx], -0x01
		pop edx
		jne $+0x7
		jmp pca_off_ret4
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		jmp pca_off_jmp4
		
	}
}
void __declspec(naked) __stdcall pca_off5()
{
	// Disable:
	/*
		cmp dword ptr [edx+0x7C], -0x01
	*/
	__asm
	{
		/*
			Original function:

			cmp dword ptr [edx + 0xE0], -0x01
			jne pca_off_jmp5
			jmp pca_off_ret5

		*/
		push eax
		mov eax, pca_offset_add
		add eax, 0x7C
		add eax, edx
		cmp dword ptr[eax], -0x01
		pop eax
		jne $+0x7
		jmp pca_off_ret5
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		jmp pca_off_jmp5
	}
}
void __declspec(naked) __stdcall pca_off6()
{
	// Disable:
	/*
		cmp dword ptr [eax+0x7C], -0x01
	*/
	__asm
	{
		/*
			Original function:

			cmp dword ptr [eax + 0xE0], -0x01
			jne pca_off_jmp6
			jmp pca_off_ret6

		*/
		push edx
		mov edx, pca_offset_add
		add edx, 0x7C
		add edx, eax
		cmp dword ptr[edx], -0x01
		pop edx
		jne $+0x7
		jmp pca_off_ret6
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		jmp pca_off_jmp6
	}
}
void __declspec(naked) __stdcall pca_off7()
{
	// Disable:
	/*
		cmp dword ptr [ecx+0x7C], -0x01
	*/
	__asm
	{
		push edx
		mov edx, pca_offset_add
		add edx, 0x7C
		add edx, ecx
		cmp dword ptr [edx], -0x01
		pop edx
		mov byte ptr [esp + 0x0E], 0x00
		jmp pca_off_ret7
	}
}
void __declspec(naked) __stdcall pca_off8()
{
	// Disable:
	/*
		lea eax, [edx+0x68]
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x68
		add ecx, edx
		lea eax, [ecx]
		pop ecx
		push esi
		xor bl, bl
		jmp pca_off_ret8
	}
}
void __declspec(naked) __stdcall pca_off9()
{
	// Disable:
	/*
		mov [edx+0x74], 0x00000000
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x74
		add ecx, edx
		mov dword ptr [ecx], 0x00000000
		pop ecx
		jmp pca_off_ret9
	}
}
void __declspec(naked) __stdcall pca_off10()
{
	// Disable:
	/*
		inc [edx+0x74]
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x74
		add ecx, edx
		inc [ecx]
		pop ecx
		add eax, 0x04
		jmp pca_off_ret10
	}
}
void __declspec(naked) __stdcall pca_off11()
{
	// Disable:
	/*
		mov edx, [ebx+0x70]
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x70
		add ecx, ebx
		mov edx, [ecx]
		pop ecx
		mov dword ptr [esp + 0x24], ecx
		jmp pca_off_ret11
	}
}
void __declspec(naked) __stdcall pca_off12()
{
	// Disable:
	/*
		mov edi, [eax+0x7C]
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x7C
		add ecx, eax
		mov edi, [ecx]
		pop ecx
		cmp edi, -0x01
		jmp pca_off_ret12
	}
}
void __declspec(naked) __stdcall pca_off13()
{
	// Disable:
	/*
		mov edi, [eax+0x7C]
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x7C
		add ecx, eax
		mov edi, [ecx]
		pop ecx
		cmp edi, -0x01
		jmp pca_off_ret13
	}
}
void __declspec(naked) __stdcall pca_off14()
{
	// Disable:
	/*
		mov eax, [esi+0x74]
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x74
		add ecx, esi
		mov eax, [ecx]
		pop ecx
		jmp pca_off_ret14
	}
}
void __declspec(naked) __stdcall pca_off15()
{
	// Disable:
	/*
		mov edx, [esi+0x78]
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x78
		add ecx, esi
		mov edx, [ecx]
		pop ecx
		cmp edx, 0x01
		jmp pca_off_ret15
	}
}
void __declspec(naked) __stdcall pca_off16()
{
	// Disable:
	/*
		cmp dword ptr [edi+0x78], 0x02
	*/
	__asm
	{
		/*
			Original function:

			cmp dword ptr [edi + 0xDC], 0x02
			jne pca_off_jmp16
			jmp pca_off_ret16

		*/
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x78
		add ecx, edi
		cmp dword ptr[ecx], 0x02
		pop ecx
		jne $+0x7
		jmp pca_off_ret16
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		jmp pca_off_jmp16
	}
}
void __declspec(naked) __stdcall pca_off17()
{
	// Disable:
	/*
		mov [edi+0x78], 0x00000003
	*/
	__asm
	{
		push ecx
		mov ecx, pca_offset_add
		add ecx, 0x78
		add ecx, edi
		mov dword ptr [ecx], 0x00000003
		pop ecx
		jmp pca_off_ret17
	}
}
void __declspec(naked) __stdcall pca_off18()
{
	// Disable:
	/*
		mov ecx, [edi+0x7C]
	*/
	__asm
	{
		push eax
		mov eax, pca_offset_add
		add eax, 0x7C
		add eax, edi
		mov ecx, [eax]
		pop eax
		cmp ecx, -0x01
		jmp pca_off_ret18
	}
}




void apply_multiphantom_patch_dynamic()
{

	max_allowed_summons_ptr = SpPointer((void *)0x0137D82C, { 0x08C });

	// (Desired num of phantoms * 0x20)
	*max_allowed_summons32 = (uint32_t)(max_allowed_summons8 * 0x20);
	uint8_t patch1[5] = { 0x68, max_allowed_summons32_arr[0], max_allowed_summons32_arr[1], max_allowed_summons32_arr[2], max_allowed_summons32_arr[3] }; // push 0x120	(when max_phantoms = 9)
	*max_allowed_summons32 = max_allowed_summons8; // Reset max_allowed_summons32 back to original value
	void *write_address = (uint8_t*)ds1_base + 0xA63CA6;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch1, 5);

	write_address = (uint8_t*)ds1_base + 0xA63CBA;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch1, 5);


	uint8_t patch2[6] = { 0xC7, 0x07, max_allowed_summons8, 0x00, 0x00, 0x00 }; // mov DWORD PTR [edi],0x9		// Number of character slots
	write_address = (uint8_t*)ds1_base + 0xA63CC1;
	set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch2, 6);


	// One of these governs red signs, another white (Probably some other around governs dragon)
	uint8_t patch3[2] = { 0x6A, max_allowed_summons8 }; // push 0x9		// Max number of summons before signs disappear
	write_address = (uint8_t*)ds1_base + 0x9B51B7;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2);

	write_address = (uint8_t*)ds1_base + 0x9B51EB;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2);


	// Check that sums up all phantoms
	uint8_t patch4[3] = { 0x83, 0xFA, (uint8_t)(max_allowed_summons8 * 7) }; // cmp edx,0x3F		// This is more than number of phantoms, but not sure by how much (num * 7 ?)
	write_address = (uint8_t*)ds1_base + 0x9B11BE;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch4, 3);


	// Not 100% on these ones
	uint8_t patch5[4] = { 0x83, 0x78, 0x10, max_allowed_summons8 }; // cmp DWORD PTR [eax+0x10],0x9
	write_address = (uint8_t*)ds1_base + 0x92A47C;
	set_mem_protection(write_address, 4, MEM_PROTECT_RWX);
	memcpy_s(write_address, 4, patch5, 4);

	uint8_t patch6[7] = { 0x83, 0xB8, 0x90, 0x00, 0x00, 0x00, max_allowed_summons8 }; // cmp DWORD PTR [eax+0x90],0x9
	write_address = (uint8_t*)ds1_base + 0x92A465;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch6, 7);


	// "I dunno"
	// - Metal-Crow, 2017
	uint8_t patch6_2[6] = { 0x83, 0xF8, max_allowed_summons8, 0x90, 0x90, 0x90 }; // cmp eax,0x9; nop; nop; nop
	write_address = (uint8_t*)ds1_base + 0x9B1271;
	set_mem_protection(write_address, 10, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch6_2, 6);
	//inject_jmp_5b((uint8_t*)write_address, &forceshowsigns_returnhere, 1, forceshowsigns_newmem);


	// Allocation of the summon_chars_data array put in the info_about_summons struct
	*max_allowed_summons32 = (uint32_t)(16 + (max_allowed_summons8 * 1216));
	uint8_t patch7[5] = { 0x68, max_allowed_summons32_arr[0], max_allowed_summons32_arr[1], max_allowed_summons32_arr[2], max_allowed_summons32_arr[3] }; // push 0x2AD0		// 16 + 1216*number of character slots
	*max_allowed_summons32 = max_allowed_summons8; // Reset max_allowed_summons32 back to original value
	write_address = (uint8_t*)ds1_base + 0x806848;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch7, 5);


	// Counter limit for initing all items in summon_chars_data array
	uint8_t patch8[3] = { 0x83, 0xFE, max_allowed_summons8 }; // cmp esi,0x9
	write_address = (uint8_t*)ds1_base + 0x8068A4;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch8, 3);

	*max_allowed_summons32 = (uint32_t)(max_allowed_summons8 * 1216);
	uint8_t patch9[6] = { 0x81, 0xFB, max_allowed_summons32_arr[0], max_allowed_summons32_arr[1], max_allowed_summons32_arr[2], max_allowed_summons32_arr[3] }; // cmp ebx,0x2AC0		// This is not index, but byte offset. So 1216*number of character slots
	*max_allowed_summons32 = max_allowed_summons8; // Reset max_allowed_summons32 back to original value
	write_address = (uint8_t*)ds1_base + 0x806ADE;
	set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch9, 6);


	// Believe this is length of summon_chars_data array, stored in struct
	uint8_t patch10[7] = { 0xC7, 0x41, 0xFC, max_allowed_summons8, 0x00, 0x00, 0x00 }; // mov DWORD PTR [ecx-0x4],0x9
	write_address = (uint8_t*)ds1_base + 0x8068A9;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch10, 7);


	// Bounds checks
	uint8_t patch11[3] = { 0x83, 0xFA, max_allowed_summons8 }; // cmp edx,0x9
	write_address = (uint8_t*)ds1_base + 0x8037A7;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch11, 3);

	*max_allowed_summons32 = (uint32_t)(max_allowed_summons8 * 1216);
	uint8_t patch12[6] = { 0x81, 0xF9, max_allowed_summons32_arr[0], max_allowed_summons32_arr[1], max_allowed_summons32_arr[2], max_allowed_summons32_arr[3] }; // cmp ecx,0x2AC0		// 1216*number of chars
	*max_allowed_summons32 = max_allowed_summons8; // Reset max_allowed_summons32 back to original value
	write_address = (uint8_t*)ds1_base + 0x8037BF;
	set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch12, 6);

	uint8_t patch13[3] = { 0x83, 0xF8, max_allowed_summons8 }; // cmp eax,0x9
	write_address = (uint8_t*)ds1_base + 0x802CC0;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch13, 3);


	// Size of summon_char_types array in info_about_summons struct
	write_address = (uint8_t*)ds1_base + 0x8068C3;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2); // push 0x9


	// Write to a variable to alert us if inject was done in time
	sucessful_phantomfix = 0; // Set success marker to false
	summon_char_types_callproc = (uint32_t)((uint8_t*)ds1_base + 0x1629C0);

	// Init rest of the summon_char_types array to zero
	write_address = (uint8_t*)ds1_base + 0x8068DF;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &summon_char_types_returnhere, 3, summon_char_types_newmem);



	// Hardcoded limits for summon_char_types length (some are <=)
	uint8_t patch14[3] = { 0x83, 0xF8, max_allowed_summons8 }; // cmp eax,0x09
	write_address = (uint8_t*)ds1_base + 0x802CB7;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch14, 3);

	uint8_t patch15[3] = { 0x83, 0xF9, (uint8_t)(max_allowed_summons8 - 1) }; // cmp ecx,0x08
	write_address = (uint8_t*)ds1_base + 0x802C80;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch15, 3);

	uint8_t patch16[3] = { 0x83, 0xFB, (uint8_t)(max_allowed_summons8 - 1) }; // cmp ebx,0x08
	write_address = (uint8_t*)ds1_base + 0x92E7A9;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch16, 3);

	write_address = (uint8_t*)ds1_base + 0x92E6AF; // cmp ebx,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch16, 3);

	uint8_t patch17[3] = { 0x83, 0xFE, (uint8_t)(max_allowed_summons8 - 1) }; // cmp esi,0x08
	write_address = (uint8_t*)ds1_base + 0x92FB73;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch17, 3);

	uint8_t patch18[3] = { 0x83, 0xFF, (uint8_t)(max_allowed_summons8 - 1) }; // cmp edi,0x08
	write_address = (uint8_t*)ds1_base + 0x92FDF1;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch18, 3);

	uint8_t patch19[3] = { 0x83, 0xF8, (uint8_t)(max_allowed_summons8 - 1) }; // cmp eax,0x08
	write_address = (uint8_t*)ds1_base + 0x930431;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch19, 3);

	write_address = (uint8_t*)ds1_base + 0x931C66; // cmp esi,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch17, 3);

	write_address = (uint8_t*)ds1_base + 0x928135; // cmp eax,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch19, 3);

	write_address = (uint8_t*)ds1_base + 0x92B19A; // cmp edi,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch18, 3);

	write_address = (uint8_t*)ds1_base + 0xAE2B8A; // cmp eax,0x08
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch19, 3);



	// Size to init an array of char pointers to
	write_address = (uint8_t*)ds1_base + 0x322A00;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2); // push 0x9


	/*
	// Number of black signs visible
	// This aob is in memory for the defaults dark souls normally sets. Will not work with we change
	//  - BlackSos.[First/Second/Third]LevelRange[Max/Min]
	void *BlackSosNum = aob_scan("05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00");
	while (BlackSosNum == NULL)
	{
		Sleep(100);
		BlackSosNum = aob_scan("05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00");
	}
	set_mem_protection(BlackSosNum, 4, MEM_PROTECT_RWX);
	*(uint32_t*)BlackSosNum = 0x08000000;


	// Something to do with number of allowed invaders? just adding to be safe
	// This aob is in memory for the defaults dark souls normally sets. Will not work with we change
	// - Invade.[First/Second/Third]LevelRange[Max/Min]
	void *InvadeNum = aob_scan("05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00");
	while (InvadeNum == NULL)
	{
		Sleep(100);
		InvadeNum = aob_scan("05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00");
	}
	set_mem_protection(InvadeNum, 4, MEM_PROTECT_RWX);
	*(uint32_t*)InvadeNum = 0x08000000;
	*/

	// Max connection tickets
	// Alt AOB: 05 00 00 00 00 00 90 41 00 00 10 42 00 00 A0 40 00 00 20 41 00 00 A0 41
	write_address = (uint8_t*)ds1_base + 0xEDE2CC;
	set_mem_protection(write_address, 4, MEM_PROTECT_RWX);
	*(uint32_t*)write_address = 0x08000000;


	// Lobby Access Max Count
	uint8_t patch20[7] = { 0xC7, 0x86, 0xD4, 0x00, 0x00, 0x00, (uint8_t)(max_allowed_summons8 - 1) }; // mov DWORD PTR [esi+0xd4],0x8
	write_address = (uint8_t*)ds1_base + 0x7E9AD1;
	set_mem_protection(write_address, 10, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch20, 7);


	// In initing players_connected_array, number of chars to init
	write_address = (uint8_t*)ds1_base + 0xAA24AA;
	set_mem_protection(write_address, 2, MEM_PROTECT_RWX);
	memcpy_s(write_address, 2, patch3, 2); // push 0x9


	// For mallocing players_connected_array: 24+20*(number of characters)+36
	*max_allowed_summons32 = (uint32_t)(24 + (max_allowed_summons8 * 20) + 36);
	uint8_t patch21[5] = { 0x68, max_allowed_summons32_arr[0], max_allowed_summons32_arr[1], max_allowed_summons32_arr[2], max_allowed_summons32_arr[3] }; // push 0xF0
	*max_allowed_summons32 = max_allowed_summons8; // Reset max_allowed_summons32 back to original value
	write_address = (uint8_t*)ds1_base + 0xABB55D;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch21, 5);


	// In initing players_connected_array, offsets after newly updated number of chars to init
	// @TODO: Is this affected by max_allowed_summons value?
	players_connected_array_offsets_callproc = (uint32_t)((uint8_t*)ds1_base + 0xAA2320);
	players_connected_array_offsets_returnhere = (uint32_t)((uint8_t*)ds1_base + 0xAA24FF);

	write_address = (uint8_t*)ds1_base + 0xAA24D2;
	set_mem_protection(write_address, 128, MEM_PROTECT_RWX);
	uint32_t dummy_ret;
	inject_jmp_5b((uint8_t*)write_address, &dummy_ret, 4, players_connected_array_offsets);



	// The number of char slots iterated through the players_connected_array (functions that reference it)

	uint8_t patch22[7] = { 0xC7, 0x44, 0x24, 0x10, max_allowed_summons8, 0x00, 0x00 }; // mov DWORD PTR [esp+0x10],0x00000009
	write_address = (uint8_t*)ds1_base + 0xAA1A86;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch22, 7);

	uint8_t patch23[7] = { 0xC7, 0x44, 0x24, 0x14, max_allowed_summons8, 0x00, 0x00 }; // mov DWORD PTR [esp+0x14],0x00000009
	write_address = (uint8_t*)ds1_base + 0xAA2FDE;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch23, 7);

	uint8_t patch24[3] = { 0x83, 0xFA, max_allowed_summons8 }; // cmp edx,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2554;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch24, 3);

	uint8_t patch25[3] = { 0x83, 0xFB, max_allowed_summons8 }; // cmp ebx,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2569;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch25, 3);

	uint8_t patch26[3] = { 0x83, 0xFE, max_allowed_summons8 }; // cmp esi,0x09
	write_address = (uint8_t*)ds1_base + 0xC12C71;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch26, 3);

	uint8_t patch27[7] = { 0xC7, 0x44, 0x24, 0x14, max_allowed_summons8, 0x00, 0x00 }; // mov DWORD PTR [esp+0x14],0x00000009
	write_address = (uint8_t*)ds1_base + 0xAA2932;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch27, 7);

	uint8_t patch28[7] = { 0xC7, 0x44, 0x24, 0x14, max_allowed_summons8, 0x00, 0x00 }; // mov DWORD PTR [esp+0x14],0x00000009
	write_address = (uint8_t*)ds1_base + 0xAA2C8E;
	set_mem_protection(write_address, 7, MEM_PROTECT_RWX);
	memcpy_s(write_address, 7, patch28, 7);

	uint8_t patch29[3] = { 0x83, 0xF8, max_allowed_summons8 }; // cmp eax,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2A2C;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch29, 3);

	uint8_t patch30[3] = { 0x83, 0xFF, max_allowed_summons8 }; // cmp edi,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2C06;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch30, 3);

	uint8_t patch31[3] = { 0x83, 0xF8, max_allowed_summons8 }; // cmp eax,0x09
	write_address = (uint8_t*)ds1_base + 0xAA2C18;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch31, 3);

	//^ but number of phantom slots
	uint8_t patch32[3] = { 0x83, 0xF9, (uint8_t)(max_allowed_summons8 - 1) }; // cmp ecx,0x08
	write_address = (uint8_t*)ds1_base + 0xAA1788;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch32, 3);

	uint8_t patch33[5] = { 0xB9, (uint8_t)(max_allowed_summons8 - 1), 0x00, 0x00, 0x00 }; // mov ecx,0x00000008
	write_address = (uint8_t*)ds1_base + 0xAA1798;
	set_mem_protection(write_address, 5, MEM_PROTECT_RWX);
	memcpy_s(write_address, 5, patch33, 5);

	uint8_t patch34[3] = { 0x8D, 0x5F, (uint8_t)(max_allowed_summons8 - 1) }; // lea ebx,[edi+0x08]
	write_address = (uint8_t*)ds1_base + 0xAA2BFF;
	set_mem_protection(write_address, 3, MEM_PROTECT_RWX);
	memcpy_s(write_address, 3, patch34, 3);



	// Where the variables after the inline character array in the players_connected_array
	// whose offsets have changed, are referenced
	// OFFSET+=num additional chars*0x20
	write_address = (uint8_t*)ds1_base + 0xA6615A;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret1, 1, pca_off1);

	write_address = (uint8_t*)ds1_base + 0xA5B900;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp2 = (uint32_t)((uint8_t*)ds1_base + 0xA5B939);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret2, 1, pca_off2);

	write_address = (uint8_t*)ds1_base + 0x9C83DB;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret3, 2, pca_off3);

	write_address = (uint8_t*)ds1_base + 0x9C6BBE;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp4 = (uint32_t)((uint8_t*)ds1_base + 0x9C6BDB);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret4, 1, pca_off4);

	write_address = (uint8_t*)ds1_base + 0x92A44A;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp5 = (uint32_t)((uint8_t*)ds1_base + 0x92A453);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret5, 1, pca_off5);

	write_address = (uint8_t*)ds1_base + 0x9B53FB;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp6 = (uint32_t)((uint8_t*)ds1_base + 0x9B5405);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret6, 1, pca_off6);

	write_address = (uint8_t*)ds1_base + 0x93FEEE;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret7, 4, pca_off7);

	write_address = (uint8_t*)ds1_base + 0xAA1771;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret8, 1, pca_off8);

	write_address = (uint8_t*)ds1_base + 0xAA1791;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret9, 2, pca_off9);

	write_address = (uint8_t*)ds1_base + 0xAA17AD;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret10, 1, pca_off10);

	write_address = (uint8_t*)ds1_base + 0xAA29A9;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret11, 2, pca_off11);

	write_address = (uint8_t*)ds1_base + 0xAA3141;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret12, 1, pca_off12);

	write_address = (uint8_t*)ds1_base + 0xAA3268;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret13, 1, pca_off13);

	write_address = (uint8_t*)ds1_base + 0xAA18E2;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret14, 0, pca_off14);

	write_address = (uint8_t*)ds1_base + 0xAA18E7;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret15, 1, pca_off15);

	write_address = (uint8_t*)ds1_base + 0xAA2748;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	pca_off_jmp16 = (uint32_t)((uint8_t*)ds1_base + 0xAA27A9);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret16, 1, pca_off16);

	write_address = (uint8_t*)ds1_base + 0xAA27A1;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret17, 2, pca_off17);

	write_address = (uint8_t*)ds1_base + 0xAA2740;
	set_mem_protection(write_address, 16, MEM_PROTECT_RWX);
	inject_jmp_5b((uint8_t*)write_address, &pca_off_ret18, 1, pca_off18);


	uint8_t patch35[6] = { 0x8B, 0x8F, 0xE4, 0x00, 0x00, 0x00 }; // mov ecx, DWORD PTR [edi+0xe4]
	write_address = (uint8_t*)ds1_base + 0xAA277A;
	set_mem_protection(write_address, 6, MEM_PROTECT_RWX);
	memcpy_s(write_address, 6, patch35, 6);
}

void apply_multiphantom_secondary_patch_dynamic()
{
	uint32_t temp_ind_off = (uint32_t)(max_allowed_summons8 - 1) << 6;

	// Number of black signs visible
	// This aob is in memory for the defaults dark souls normally sets. Will not work with we change
	//  - BlackSos.[First/Second/Third]LevelRange[Max/Min]
	BlackSosNum = aob_scan("05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00");
	while (BlackSosNum == NULL)
	{
		Sleep(100);
		BlackSosNum = aob_scan("05 00 00 00 03 00 00 00 03 00 00 00 05 00 00 00 05 00 00 00 0A 00 00 00 0A 00 00 00");
	}
	set_mem_protection(BlackSosNum, 4, MEM_PROTECT_RWX);
	*(uint32_t*)BlackSosNum = temp_ind_off;


	// Something to do with number of allowed invaders? just adding to be safe
	// This aob is in memory for the defaults dark souls normally sets. Will not work with we change
	// - Invade.[First/Second/Third]LevelRange[Max/Min]
	InvadeNum = aob_scan("05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00");
	while (InvadeNum == NULL)
	{
		Sleep(100);
		InvadeNum = aob_scan("05 00 00 00 FA FF FF FF 08 00 00 00 FC FF FF FF 0A 00 00 00 FE FF FF FF 0C 00 00 00");
	}
	set_mem_protection(InvadeNum, 4, MEM_PROTECT_RWX);
	*(uint32_t*)InvadeNum = temp_ind_off;
}