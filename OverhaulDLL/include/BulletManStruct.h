#pragma once
#ifndef BULLETMAN_STRUCT_H
#define BULLETMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct BulletMan BulletMan;
typedef struct BulletIns BulletIns;
typedef struct BulletMan_Field0x20 BulletMan_Field0x20;
typedef struct BulletMan_Field0x40 BulletMan_Field0x40;
typedef struct BulletMan_field0x78Elem BulletMan_field0x78Elem;
typedef struct BulletIns_Field0x58 BulletIns_Field0x58;
typedef struct BulletMan_Field0x20 BulletMan_Field0x20;
typedef struct BulletParamInfo BulletParamInfo;
typedef struct BulletIns_Field0x90_Field0x1a0 BulletIns_Field0x90_Field0x1a0;

struct BulletIns
{
    uint64_t padding_0;
    uint8_t data_0[0x50];
    uint64_t padding_1;

};
static_assert(offsetof(BulletIns, data_0) == 0x8);

static_assert(sizeof(BulletIns) == 0x360);

struct BulletIns_Field0x90_Field0x1a0
{
    uint8_t data_0[0x20];
    uint64_t unknown1; //unknown
    uint8_t data_1[0x18];
    uint64_t unknown2; //unknown
    uint8_t data_2[0x50];
    uint64_t unknown3; //unknown
    uint8_t data_3[0x20+0x18];
    uint64_t unknown4; //unknown
    uint8_t data_4[0x30+0x30+8];
    void* unknown5; //unknown
    uint8_t data_5[0x10];
};
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, data_1) == 0x28);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, unknown2) == 0x40);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, data_2) == 0x48);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, unknown3) == 0x90+8);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, data_3) == 0x90+0x10);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, unknown4) == 0xd8);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, data_4) == 0xe0);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, unknown5) == 0x148);
static_assert(offsetof(BulletIns_Field0x90_Field0x1a0, data_5) == 0x150);
static_assert(sizeof(BulletIns_Field0x90_Field0x1a0) == 0x160);

struct BulletParamInfo
{
    uint8_t data_0[0x10];
};
static_assert(sizeof(BulletParamInfo) == 0x10);

struct BulletMan_Field0x20
{
    uint8_t data_0[0x198];
    BulletParamInfo* bulletParamInfo;
    BulletIns_Field0x90_Field0x1a0 field0x1a0;
    uint64_t data_1[2]; //this includes a pointer to a saved entitu
    uint64_t padding_1[2];
};
static_assert(offsetof(BulletMan_Field0x20, bulletParamInfo) == 0x198);
static_assert(offsetof(BulletMan_Field0x20, field0x1a0) == 0x1a0);
static_assert(offsetof(BulletMan_Field0x20, data_1) == 0x300);
static_assert(sizeof(BulletMan_Field0x20) == 0x320);

struct BulletIns_Field0x58
{
    uint64_t padding_0;
    uint64_t padding_1[5]; //unknown
};
static_assert(sizeof(BulletIns_Field0x58) == 0x30);

struct BulletMan_Field0x40
{
    uint64_t data_0;
    BulletIns_Field0x58 arry[16];
    uint64_t data_1[2];
};
static_assert(offsetof(BulletMan_Field0x40, data_1) == 0x308);
static_assert(sizeof(BulletMan_Field0x40) == 0x318);

struct BulletMan_field0x78Elem
{
    uint8_t data_0[0x30];
};
static_assert(sizeof(BulletMan_field0x78Elem) == 0x30);

struct Bulletman
{
    BulletIns* bulletins_arry; //length == 128
    uint64_t data_0[3]; //includes pointers to offsets in above
    BulletMan_Field0x20* field0x20; //length == 40
    uint64_t data_1[3]; //includes pointers to offsets in above
    BulletMan_Field0x40* field0x40; //length == 4
    uint64_t data_2[3]; //includes pointers to offsets in above
    uint64_t padding_0;
    uint64_t data_3;
    uint64_t padding_1;
    BulletMan_field0x78Elem** field0x78; //variable length
    uint64_t data_4[2]; //includes pointers to offsets in above
    uint64_t padding_2[3];
    uint64_t data_5[2];
};
static_assert(offsetof(Bulletman, bulletins_arry) == 0);
static_assert(offsetof(Bulletman, data_0) == 8);
static_assert(offsetof(Bulletman, field0x20) == 0x20);
static_assert(offsetof(Bulletman, data_1) == 0x28);
static_assert(offsetof(Bulletman, field0x40) == 0x40);
static_assert(offsetof(Bulletman, data_2) == 0x48);
static_assert(offsetof(Bulletman, padding_0) == 0x60);
static_assert(offsetof(Bulletman, data_3) == 0x68);
static_assert(offsetof(Bulletman, padding_1) == 0x70);
static_assert(offsetof(Bulletman, field0x78) == 0x78);
static_assert(offsetof(Bulletman, data_4) == 0x80);
static_assert(offsetof(Bulletman, padding_2) == 0x90);
static_assert(offsetof(Bulletman, data_5) == 0xa8);
static_assert(sizeof(Bulletman) == 0xb8);

#endif
