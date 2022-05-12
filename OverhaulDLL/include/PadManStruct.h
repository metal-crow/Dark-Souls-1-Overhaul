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
typedef struct VirtualAnalogKeyInfo_float VirtualAnalogKeyInfo_float;
typedef struct DynamicBitset DynamicBitset;

struct DynamicBitset
{
    uint64_t padding_0[2];
    uint32_t* inputKeys;
    uint64_t padding_1;
};

static_assert(offsetof(DynamicBitset, inputKeys) == 0x10);
static_assert(sizeof(DynamicBitset) == 0x20);

struct VirtualAnalogKeyInfo_float
{
    uint64_t padding_0[2];
    float* analogSticksAndPad;
    uint64_t padding_1[2];
};

static_assert(offsetof(VirtualAnalogKeyInfo_float, analogSticksAndPad) == 0x10);
static_assert(sizeof(VirtualAnalogKeyInfo_float) == 0x28);

struct VirtualInputData
{
    uint64_t padding_0;
    VirtualAnalogKeyInfo_float VirAnalogKeyInfo;
    DynamicBitset keys;
};

static_assert(offsetof(VirtualInputData, VirAnalogKeyInfo) == 0x8);
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
    uint8_t padding_0[0xd0];
    VirtualInputData VirtInputData;
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
