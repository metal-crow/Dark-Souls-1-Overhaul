#pragma once
#ifndef SFXMAN_STRUCT_H
#define SFXMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct SfxMan SfxMan;
typedef struct frpgFxManagerBase frpgFxManagerBase;
typedef struct FXHGManagerBase FXHGManagerBase;
typedef struct class_1415002c8 class_1415002c8;
typedef struct class_14152d360 class_14152d360;
typedef struct class_14150b808_field0x48 class_14150b808_field0x48;
typedef struct class_14152d360_field0xe0 class_14152d360_field0xe0;
typedef struct class_14150b808_field0xf0 class_14150b808_field0xf0;

struct class_14150b808_field0x48
{
    uint8_t data_0[0x58];
    uint64_t self_class_1415262e0; //&self + 0xe0
    uint8_t data_1[16];
    void* unk1;
    uint8_t data_2[16];
    void* unk2; //can be nulled without error
    class_14150b808_field0x48* next; //this is p much always null, probably safe to ignore
    void* unk4;
    void* unk5;
    uint8_t padding_0[0x28]; //wstring, set to null
    class_14152d360* parent;
    uint64_t unk6; //can be nulled without error
    uint64_t vtable; //0x1415262e0
    void* unk7;
    void* unk8; //can be nulled without error
    void* unk9; //can be nulled without error
    void* unk10; //can be nulled without error
    void* unk11; //can be nulled without error
    uint8_t data_3[28];
};
static_assert(offsetof(class_14150b808_field0x48, data_1) == 0x60);
static_assert(offsetof(class_14150b808_field0x48, unk1) == 0x70);
static_assert(offsetof(class_14150b808_field0x48, data_2) == 0x78);
static_assert(offsetof(class_14150b808_field0x48, unk2) == 0x88);
static_assert(offsetof(class_14150b808_field0x48, next) == 0x90);
static_assert(offsetof(class_14150b808_field0x48, unk4) == 0x98);
static_assert(offsetof(class_14150b808_field0x48, unk5) == 0xa0);
static_assert(offsetof(class_14150b808_field0x48, parent) == 0xd0);
static_assert(offsetof(class_14150b808_field0x48, unk6) == 0xd8);
static_assert(offsetof(class_14150b808_field0x48, unk7) == 0xe0+8);
static_assert(offsetof(class_14150b808_field0x48, data_3) == 0xe0+0x30);
static_assert(sizeof(class_14150b808_field0x48) == 0x130);

struct class_14152d360
{
    //inline class_14150b808 here since this class is a linked list array and we can't handle inheritance in our basic copy functions
        uint64_t vtable;
        void* field0x8; //can be nulled without error
        uint8_t data_0[24];
        void* parent;
        uint64_t unk1; //always null
        uint64_t unk2; //can be nulled without error
        class_14152d360* next;
        class_14150b808_field0x48* field0x48_head;
        class_14150b808_field0x48* field0x48_tail;
        uint64_t unk4; //always null
    //end inline
    uint8_t data_1[0x80];
    class_14152d360_field0xe0* field0xe0;
    uint64_t data_2;
    class_14150b808_field0xf0* field0xf0;
    uint64_t data_3;
};
static_assert(offsetof(class_14152d360, field0x8) == 0x8);
static_assert(offsetof(class_14152d360, data_0) == 0x10);
static_assert(offsetof(class_14152d360, unk1) == 0x30);
static_assert(offsetof(class_14152d360, unk2) == 0x38);
static_assert(offsetof(class_14152d360, next) == 0x40);
static_assert(offsetof(class_14152d360, field0x48_head) == 0x48);
static_assert(offsetof(class_14152d360, field0x48_tail) == 0x50);
static_assert(offsetof(class_14152d360, unk4) == 0x58);
static_assert(offsetof(class_14152d360, data_1) == 0x60);
static_assert(offsetof(class_14152d360, field0xe0) == 0xe0);
static_assert(offsetof(class_14152d360, data_2) == 0xe8);
static_assert(offsetof(class_14152d360, field0xf0) == 0xf0);
static_assert(sizeof(class_14152d360) == 0x100);

struct class_1415002c8
{
    uint64_t padding_0[3];
    class_14152d360* Class_14152d360;
};
static_assert(offsetof(class_1415002c8, Class_14152d360) == 0x18);

struct FXHGManagerBase
{
    uint64_t padding_0;
    class_1415002c8* Class_1415002c8;
};
static_assert(offsetof(FXHGManagerBase, Class_1415002c8) == 8);

struct frpgFxManagerBase
{
    FXHGManagerBase base;
};

struct SfxMan
{
    uint8_t padding_0[16];
    frpgFxManagerBase* FrpgFxManagerBase;
};
static_assert(offsetof(SfxMan, FrpgFxManagerBase) == 0x10);

#endif
