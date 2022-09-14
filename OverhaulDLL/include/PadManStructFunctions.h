#pragma once
#ifndef PADMAN_STRUCTFUNCTIONS_H
#define PADMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "PadManStruct.h"

void copy_PadMan(PadMan* to, PadMan* from);
PadMan* init_PadMan();
void free_PadMan(PadMan* to);

void copy_PadDevice(PadDevice* to, PadDevice* from);
PadDevice* init_PadDevice();
void free_PadDevice(PadDevice* to);

void copy_KeyboardDevice(KeyboardDevice* to, KeyboardDevice* from);
KeyboardDevice* init_KeyboardDevice();
void free_KeyboardDevice(KeyboardDevice* to);

void copy_MouseDevice(MouseDevice* to, MouseDevice* from);
MouseDevice* init_MouseDevice();
void free_MouseDevice(MouseDevice* to);

void copy_PadDevice_UserInput(PadDevice_UserInput* to, PadDevice_UserInput* from);
PadDevice_UserInput* init_PadDevice_UserInput();
void free_PadDevice_UserInput(PadDevice_UserInput* to);

void copy_VirtualMultiDevice(VirtualMultiDevice* to, VirtualMultiDevice* from);
VirtualMultiDevice* init_VirtualMultiDevice();
void free_VirtualMultiDevice(VirtualMultiDevice* to);

void copy_DLUserInputDeviceImpl(DLUserInputDeviceImpl* to, DLUserInputDeviceImpl* from);
DLUserInputDeviceImpl* init_DLUserInputDeviceImpl();
void free_DLUserInputDeviceImpl(DLUserInputDeviceImpl* to, bool freeself);

void copy_DLUserInputDevice(DLUserInputDevice* to, DLUserInputDevice* from);
DLUserInputDevice* init_DLUserInputDevice();
void free_DLUserInputDevice(DLUserInputDevice* to, bool freeself);

void copy_VirtualInputData(VirtualInputData* to, VirtualInputData* from);
VirtualInputData* init_VirtualInputData();
void free_VirtualInputData(VirtualInputData* to, bool freeself);

void copy_VirtualAnalogKeyInfo_float(VirtualAnalogKeyInfo_float* to, VirtualAnalogKeyInfo_float* from);
VirtualAnalogKeyInfo_float* init_VirtualAnalogKeyInfo_float();
void free_VirtualAnalogKeyInfo_float(VirtualAnalogKeyInfo_float* to, bool freeself);

void copy_DynamicBitset(DynamicBitset* to, DynamicBitset* from);
DynamicBitset* init_DynamicBitset();
void free_DynamicBitset(DynamicBitset* to, bool freeself);

#endif
