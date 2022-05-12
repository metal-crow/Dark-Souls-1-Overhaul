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
}

PadDevice* init_PadDevice()
{
    PadDevice* local_PadDevice = (PadDevice*)malloc_(sizeof(PadDevice));

    local_PadDevice->VirtMultiDevice = init_VirtualMultiDevice();

    return local_PadDevice;
}
void free_PadDevice(PadDevice* to)
{
    free_VirtualMultiDevice(to->VirtMultiDevice);

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
}

DLUserInputDeviceImpl* init_DLUserInputDeviceImpl()
{
    DLUserInputDeviceImpl* local_DLUserInputDeviceImpl = (DLUserInputDeviceImpl*)malloc_(sizeof(DLUserInputDeviceImpl));

    DLUserInputDevice* local_DLUserInputDevice = init_DLUserInputDevice();
    local_DLUserInputDeviceImpl->base = *local_DLUserInputDevice;
    free(local_DLUserInputDevice);

    return local_DLUserInputDeviceImpl;

}
void free_DLUserInputDeviceImpl(DLUserInputDeviceImpl* to, bool freeself)
{
    free_DLUserInputDevice(&to->base, false);

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
    for (size_t i = 0; i < 0x80; i++)
    {
        to->analogSticksAndPad[i] = from->analogSticksAndPad[i];
    }
    copy_DynamicBitset(&to->keys, &from->keys);
}

VirtualInputData* init_VirtualInputData()
{
    VirtualInputData* local_VirtualInputData = (VirtualInputData*)malloc_(sizeof(VirtualInputData));

    local_VirtualInputData->analogSticksAndPad = (float*)malloc_(sizeof(float) * 0x80);

    DynamicBitset* local_DynamicBitset = init_DynamicBitset();
    local_VirtualInputData->keys = *local_DynamicBitset;
    free(local_DynamicBitset);

    return local_VirtualInputData;

}
void free_VirtualInputData(VirtualInputData* to, bool freeself)
{
    free(to->analogSticksAndPad);
    free_DynamicBitset(&to->keys, false);

    if (freeself)
    {
        free(to);
    }
}

void copy_DynamicBitset(DynamicBitset* to, DynamicBitset* from)
{
    for (size_t i = 0; i < 32; i++)
    {
        to->inputKeys[i] = from->inputKeys[i];
    }
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
