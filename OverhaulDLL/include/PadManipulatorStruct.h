#pragma once
#ifndef PADMANIPULATOR_STRUCT_H
#define PADMANIPULATOR_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

#pragma pack(push, 1)

typedef struct PadManipulator PadManipulator;
typedef struct ChrManipulator ChrManipulator;
typedef struct PadManipulatorPacked PadManipulatorPacked;

struct PadManipulatorPacked
{
    uint8_t const1;
    //this is the sum of the data components in PadManipulator
    uint8_t data_0[0x228];
    uint32_t data_0a;
    uint8_t data_1[0x100];
};

struct ChrManipulator
{
    void* padding_0;
    uint8_t data_0[0x228]; //fill this out once we have it understood in ghidra
};

static_assert(sizeof(ChrManipulator) == 0x230);

struct PadManipulator
{
    ChrManipulator chrManipulator;
    uint8_t padding_0[12];
    uint32_t data_0;
    uint64_t padding_1[4];
    uint8_t data_1[0x100]; //fill this out once we have it understood in ghidra
};

static_assert(offsetof(PadManipulator, padding_0) == 0x230);
static_assert(sizeof(PadManipulator) == 0x360);

#pragma pack(pop)

#endif
