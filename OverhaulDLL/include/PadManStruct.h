#pragma once
#ifndef PADMAN_STRUCT_H
#define PADMAN_STRUCT_H

//NOTE: Anything marked "data_x" is a bunch of non-pointer data that can be saved/loaded without really knowing what it is.
// See the ghidra repo for more specific info about what is in that blob, if it's even known

#include <stddef.h>
#include <stdint.h>

typedef struct PadMan PadMan;
typedef struct PadDevice PadDevice;
typedef struct PadDevice_UserInput PadDevice_UserInput;
typedef struct MouseDevice MouseDevice;
typedef struct KeyboardDevice KeyboardDevice;
typedef struct VirtualMultiDevice VirtualMultiDevice;
typedef struct DLUserInputDeviceImpl DLUserInputDeviceImpl;
typedef struct DLUserInputDevice DLUserInputDevice;
typedef struct VirtualInputData VirtualInputData;
typedef struct VirtualAnalogKeyInfo_float VirtualAnalogKeyInfo_float;
typedef struct DynamicBitset DynamicBitset;
typedef struct FlattenedPadManObj FlattenedPadManObj;
typedef struct FlattenedDLUserInputDeviceImplObj FlattenedDLUserInputDeviceImplObj;

#pragma pack(push, 1)

struct FlattenedDLUserInputDeviceImplObj
{
    float a[0x80];
    uint32_t b[32];
    float c[0x80];
    float d[0x80];
    float e[0x80];
    uint32_t f[32];
};

struct FlattenedPadManObj
{
    struct
    {
        FlattenedDLUserInputDeviceImplObj base;
    } VirtMultiDevice;
    struct
    {
        FlattenedDLUserInputDeviceImplObj base;
        uint8_t data_0[24];
        uint8_t data_1[0x1f8];
        uint64_t data_2;
    } padDevice_UserInput;
    struct
    {
        FlattenedDLUserInputDeviceImplObj base;
        uint8_t data_0[20];
        uint8_t data_1[12];
    } mouseDevice;
    struct
    {
        FlattenedDLUserInputDeviceImplObj base;
        uint8_t data_0[256];
    } keyboardDevice;
};

#pragma pack(pop)

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
    uint8_t padding_0[0x60];
    VirtualAnalogKeyInfo_float VirtAnalogKeyInfo_1;
    VirtualAnalogKeyInfo_float VirtAnalogKeyInfo_2;
    uint8_t padding_1[32];
    VirtualInputData VirtInputData;
};

static_assert(offsetof(DLUserInputDeviceImpl, VirtAnalogKeyInfo_1) == 0xe8);
static_assert(offsetof(DLUserInputDeviceImpl, VirtAnalogKeyInfo_2) == 0x110);
static_assert(offsetof(DLUserInputDeviceImpl, VirtInputData) == 0x158);
static_assert(sizeof(DLUserInputDeviceImpl) == 0x1a8);

struct VirtualMultiDevice
{
    DLUserInputDeviceImpl base;
    uint8_t padding_0[32];
};

static_assert(sizeof(VirtualMultiDevice) == 0x1c8);

struct PadDevice_UserInput
{
    DLUserInputDeviceImpl base;
    uint8_t data_0[24];
    uint64_t padding_0[2];
    uint8_t padding_1[0x40];
    uint8_t data_1[0x1f8];
    uint64_t padding_2[5];
    uint64_t data_2;
};

static_assert(offsetof(PadDevice_UserInput, data_0) == 0x1a8);
static_assert(offsetof(PadDevice_UserInput, data_1) == 0x210);
static_assert(offsetof(PadDevice_UserInput, data_2) == 0x430);
static_assert(sizeof(PadDevice_UserInput) == 0x438);

struct MouseDevice
{
    DLUserInputDeviceImpl base;
    uint64_t padding_0[2];
    uint8_t data_0[20];
    uint32_t padding_1;
    uint8_t data_1[12];
};

static_assert(offsetof(MouseDevice, data_0) == 0x1b8);
static_assert(offsetof(MouseDevice, data_1) == 0x1d0);
static_assert(sizeof(MouseDevice) == 0x1E0);

struct KeyboardDevice
{
    DLUserInputDeviceImpl base;
    uint64_t padding_0[2];
    uint8_t data_0[256];
    uint8_t padding_1[2];
};

static_assert(offsetof(KeyboardDevice, data_0) == 0x1b8);
static_assert(sizeof(KeyboardDevice) == 0x2c0);

struct PadDevice
{
    uint64_t padding_0;
    VirtualMultiDevice* VirtMultiDevice;
    PadDevice_UserInput* padDevice_UserInput;
    MouseDevice* mouseDevice;
    KeyboardDevice* keyboardDevice;
    uint8_t padding_2[0x20];
};

static_assert(offsetof(PadDevice, VirtMultiDevice) == 0x8);
static_assert(offsetof(PadDevice, padDevice_UserInput) == 0x10);
static_assert(offsetof(PadDevice, mouseDevice) == 0x18);
static_assert(offsetof(PadDevice, keyboardDevice) == 0x20);
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
