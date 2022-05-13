#include "Rollback.h"

void Rollback::GameInputSave()
{

}

void Rollback::GameInputLoad()
{

}

void copy_PadMan(PadMan* to, PadMan* from)
{
    copy_PadDevice(to->PadDevice_0, from->PadDevice_0);
}

PadMan* init_PadMan()
{
    PadMan* local_PadMan = (PadMan*)malloc_(sizeof(PadMan));

    local_PadMan->PadDevice_0 = init_PadDevice();

    return local_PadMan;
}

void free_PadMan(PadMan* to)
{
    free_PadDevice(to->PadDevice_0);

    free(to);
}

void copy_PadDevice(PadDevice* to, PadDevice* from)
{
    copy_VirtualMultiDevice(to->VirtMultiDevice, from->VirtMultiDevice);
    copy_KeyboardDevice(to->KeybrdDevice, from->KeybrdDevice);
}

PadDevice* init_PadDevice()
{
    PadDevice* local_PadDevice = (PadDevice*)malloc_(sizeof(PadDevice));

    local_PadDevice->VirtMultiDevice = init_VirtualMultiDevice();
    local_PadDevice->KeybrdDevice = init_KeyboardDevice();

    return local_PadDevice;
}
void free_PadDevice(PadDevice* to)
{
    free_VirtualMultiDevice(to->VirtMultiDevice);
    free_KeyboardDevice(to->KeybrdDevice);

    free(to);
}

void copy_KeyboardDevice(KeyboardDevice* to, KeyboardDevice* from)
{
    memcpy(to->key_states, from->key_states, sizeof(to->key_states));
}

KeyboardDevice* init_KeyboardDevice()
{
    KeyboardDevice* local_KeyboardDevice = (KeyboardDevice*)malloc_(sizeof(KeyboardDevice));

    return local_KeyboardDevice;
}

void free_KeyboardDevice(KeyboardDevice* to)
{
    free(to);
}

void copy_VirtualMultiDevice(VirtualMultiDevice* to, VirtualMultiDevice* from)
{
    copy_DLUserInputDeviceImpl(&to->base, &from->base);
}

VirtualMultiDevice* init_VirtualMultiDevice()
{
    VirtualMultiDevice* local_VirtualMultiDevice = (VirtualMultiDevice*)malloc_(sizeof(VirtualMultiDevice));

    DLUserInputDeviceImpl* local_DLUserInputDeviceImpl = init_DLUserInputDeviceImpl();
    local_VirtualMultiDevice->base = *local_DLUserInputDeviceImpl;
    free(local_DLUserInputDeviceImpl);

    return local_VirtualMultiDevice;
}

void free_VirtualMultiDevice(VirtualMultiDevice* to)
{
    free_DLUserInputDeviceImpl(&to->base, false);

    free(to);
}

void copy_DLUserInputDeviceImpl(DLUserInputDeviceImpl* to, DLUserInputDeviceImpl* from)
{
    copy_DLUserInputDevice(&to->base, &from->base);
    copy_VirtualAnalogKeyInfo_float(&to->VirtAnalogKeyInfo_1, &from->VirtAnalogKeyInfo_1);
    copy_VirtualAnalogKeyInfo_float(&to->VirtAnalogKeyInfo_2, &from->VirtAnalogKeyInfo_2);
    copy_VirtualInputData(&to->VirtInputData, &from->VirtInputData);
}

DLUserInputDeviceImpl* init_DLUserInputDeviceImpl()
{
    DLUserInputDeviceImpl* local_DLUserInputDeviceImpl = (DLUserInputDeviceImpl*)malloc_(sizeof(DLUserInputDeviceImpl));

    DLUserInputDevice* local_DLUserInputDevice = init_DLUserInputDevice();
    local_DLUserInputDeviceImpl->base = *local_DLUserInputDevice;
    free(local_DLUserInputDevice);

    VirtualAnalogKeyInfo_float* local_VirtualAnalogKeyInfo_float1 = init_VirtualAnalogKeyInfo_float();
    local_DLUserInputDeviceImpl->VirtAnalogKeyInfo_1 = *local_VirtualAnalogKeyInfo_float1;
    free(local_VirtualAnalogKeyInfo_float1);

    VirtualAnalogKeyInfo_float* local_VirtualAnalogKeyInfo_float2 = init_VirtualAnalogKeyInfo_float();
    local_DLUserInputDeviceImpl->VirtAnalogKeyInfo_2 = *local_VirtualAnalogKeyInfo_float2;
    free(local_VirtualAnalogKeyInfo_float2);

    VirtualInputData* local_VirtualInputData = init_VirtualInputData();
    local_DLUserInputDeviceImpl->VirtInputData = *local_VirtualInputData;
    free(local_VirtualInputData);

    return local_DLUserInputDeviceImpl;

}
void free_DLUserInputDeviceImpl(DLUserInputDeviceImpl* to, bool freeself)
{
    free_DLUserInputDevice(&to->base, false);
    free_VirtualAnalogKeyInfo_float(&to->VirtAnalogKeyInfo_1, false);
    free_VirtualAnalogKeyInfo_float(&to->VirtAnalogKeyInfo_2, false);
    free_VirtualInputData(&to->VirtInputData, false);

    if (freeself)
    {
        free(to);
    }
}

void copy_DLUserInputDevice(DLUserInputDevice* to, DLUserInputDevice* from)
{
    copy_VirtualInputData(&to->VirtInputData, &from->VirtInputData);
}

DLUserInputDevice* init_DLUserInputDevice()
{
    DLUserInputDevice* local_DLUserInputDevice = (DLUserInputDevice*)malloc_(sizeof(DLUserInputDevice));

    VirtualInputData* local_VirtualInputData = init_VirtualInputData();
    local_DLUserInputDevice->VirtInputData = *local_VirtualInputData;
    free(local_VirtualInputData);

    return local_DLUserInputDevice;
}

void free_DLUserInputDevice(DLUserInputDevice* to, bool freeself)
{
    free_VirtualInputData(&to->VirtInputData, false);

    if (freeself)
    {
        free(to);
    }
}

void copy_VirtualInputData(VirtualInputData* to, VirtualInputData* from)
{
    copy_VirtualAnalogKeyInfo_float(&to->VirAnalogKeyInfo, &from->VirAnalogKeyInfo);
    copy_DynamicBitset(&to->keys, &from->keys);
}

VirtualInputData* init_VirtualInputData()
{
    VirtualInputData* local_VirtualInputData = (VirtualInputData*)malloc_(sizeof(VirtualInputData));

    VirtualAnalogKeyInfo_float* local_VirtualAnalogKeyInfo_float = init_VirtualAnalogKeyInfo_float();
    local_VirtualInputData->VirAnalogKeyInfo = *local_VirtualAnalogKeyInfo_float;
    free(local_VirtualAnalogKeyInfo_float);

    DynamicBitset* local_DynamicBitset = init_DynamicBitset();
    local_VirtualInputData->keys = *local_DynamicBitset;
    free(local_DynamicBitset);

    return local_VirtualInputData;

}
void free_VirtualInputData(VirtualInputData* to, bool freeself)
{
    free_VirtualAnalogKeyInfo_float(&to->VirAnalogKeyInfo, false);
    free_DynamicBitset(&to->keys, false);

    if (freeself)
    {
        free(to);
    }
}

void copy_VirtualAnalogKeyInfo_float(VirtualAnalogKeyInfo_float* to, VirtualAnalogKeyInfo_float* from)
{
    memcpy(to->analogSticksAndPad, from->analogSticksAndPad, sizeof(float) * 0x80);
}

VirtualAnalogKeyInfo_float* init_VirtualAnalogKeyInfo_float()
{
    VirtualAnalogKeyInfo_float* local_VirtualAnalogKeyInfo_float = (VirtualAnalogKeyInfo_float*)malloc_(sizeof(VirtualAnalogKeyInfo_float));

    local_VirtualAnalogKeyInfo_float->analogSticksAndPad = (float*)malloc_(sizeof(float) * 0x80);

    return local_VirtualAnalogKeyInfo_float;
}

void free_VirtualAnalogKeyInfo_float(VirtualAnalogKeyInfo_float* to, bool freeself)
{
    free(to->analogSticksAndPad);

    if (freeself)
    {
        free(to);
    }
}


void copy_DynamicBitset(DynamicBitset* to, DynamicBitset* from)
{
    memcpy(to->inputKeys, from->inputKeys, sizeof(uint32_t) * 32);
}

DynamicBitset* init_DynamicBitset()
{
    DynamicBitset* local_DynamicBitset = (DynamicBitset*)malloc_(sizeof(DynamicBitset));

    local_DynamicBitset->inputKeys = (uint32_t*)malloc_(sizeof(uint32_t) * 32);

    return local_DynamicBitset;
}

void free_DynamicBitset(DynamicBitset* to, bool freeself)
{
    free(to->inputKeys);

    if (freeself)
    {
        free(to);
    }
}
