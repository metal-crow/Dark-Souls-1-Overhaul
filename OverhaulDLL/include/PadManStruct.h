#pragma once
#ifndef PADMAN_STRUCT_H
#define PADMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct PadMan PadMan;
typedef struct PadDevice PadDevice;
typedef struct VirtualMultiDevice VirtualMultiDevice;
typedef struct DLUserInputDeviceImpl DLUserInputDeviceImpl;
typedef struct DLUserInputDevice DLUserInputDevice;
typedef struct VirtualInputData VirtualInputData;
typedef struct DynamicBitset DynamicBitset;

struct DynamicBitset
{
    uint64_t padding_0[2];
    uint32_t* inputKeys;
    uint64_t padding_1;
};

static_assert(offsetof(DynamicBitset, inputKeys) == 0x10);
static_assert(sizeof(DynamicBitset) == 0x20);

struct VirtualInputData
{
    uint8_t padding_0[0x18];
    float* analogSticksAndPad;
    uint8_t padding_1[0x10];
    DynamicBitset keys;
};

static_assert(offsetof(VirtualInputData, analogSticksAndPad) == 0x18);
static_assert(offsetof(VirtualInputData, keys) == 0x30);
static_assert(sizeof(VirtualInputData) == 0x50);

struct DLUserInputDevice
{
    uint64_t padding_0[2];
    VirtualInputData VirtInputData;
    uint8_t padding_1[0x28];
};

static_assert(offsetof(DLUserInputDevice, VirtInputData) == 0x10);
static_assert(sizeof(DLUserInputDevice) == 0x88);

struct DLUserInputDeviceImpl
{
    DLUserInputDevice base;
    uint8_t padding_0[0x120];
};

static_assert(sizeof(DLUserInputDeviceImpl) == 0x1a8);

struct VirtualMultiDevice
{
    DLUserInputDeviceImpl base;
    uint8_t padding_0[32];
};

static_assert(sizeof(VirtualMultiDevice) == 0x1c8);

struct PadDevice
{
    uint64_t padding_0;
    VirtualMultiDevice* VirtMultiDevice;
    uint8_t padding_1[0x38];
};

static_assert(offsetof(PadDevice, VirtMultiDevice) == 0x8);
static_assert(sizeof(PadDevice) == 0x48);

struct PadMan
{
    uint64_t padding_0[2];
    PadDevice* PadDevice_0;
    uint8_t padding_1[0x90];
};

static_assert(offsetof(PadMan, PadDevice_0) == 0x10);
static_assert(sizeof(PadMan) == 0xa8);

#endif
