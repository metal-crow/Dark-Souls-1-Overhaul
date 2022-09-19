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

struct class_14152d360
{
    //inline class_14150b808 here since this class is a linked list array and we can't handle inheritance in our basic copy functions
        uint64_t padding_0;
        uint64_t unk1;
        uint8_t data_0[24];
        uint64_t padding_1;
        uint64_t unk2[2];
        class_14152d360* next;
        uint64_t unk2a[3];
    //end inline
    uint8_t data_1[0x80];
    uint64_t unk3;
    uint64_t data_2;
    uint64_t unk4;
    uint64_t data_3;
};
static_assert(offsetof(class_14152d360, next) == 0x40);
static_assert(offsetof(class_14152d360, data_1) == 0x60);
static_assert(offsetof(class_14152d360, data_2) == 0xe8);
static_assert(sizeof(class_14152d360) == 0x100);

struct class_1415002c8
{
    uint64_t padding_0;
    uint64_t unk1;
    uint64_t padding_1;
    class_14152d360* Class_14152d360;

    uint8_t unk2[0x210 - 0x20];
};
static_assert(offsetof(class_1415002c8, Class_14152d360) == 0x18);
static_assert(sizeof(class_1415002c8) == 0x210);

struct FXHGManagerBase
{
    uint64_t padding_0;
    class_1415002c8* Class_1415002c8;
    uint8_t unk1[0x30];
};
static_assert(offsetof(FXHGManagerBase, Class_1415002c8) == 8);
static_assert(sizeof(FXHGManagerBase) == 0x40);

struct frpgFxManagerBase
{
    FXHGManagerBase base;
    uint8_t unk1[0xa8 - 0x40];
};
static_assert(sizeof(frpgFxManagerBase) == 0xa8);

struct SfxMan
{
    uint8_t padding_0[8];
    uint64_t unk1;
    frpgFxManagerBase* FrpgFxManagerBase;
    uint64_t padding_1;
    uint8_t unk2[0x340];
};
static_assert(offsetof(SfxMan, FrpgFxManagerBase) == 0x10);
static_assert(sizeof(SfxMan) == 0x360);

#endif
