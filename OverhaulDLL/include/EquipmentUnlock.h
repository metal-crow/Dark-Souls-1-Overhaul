#pragma once

#ifndef _DS1_OVERHAUL_EQUIPMENT_UNLOCK_H_
#define _DS1_OVERHAUL_EQUIPMENT_UNLOCK_H_

#include <cstdint>

class EquipmentUnlock
{
public:
    static void start();
private:
    static const uint64_t disable_equipment_unlock_injection_offset = 0x62E437;
    static const uint64_t add_check_equipment_unlock_injection_offset = 0x62E4F1;
};

#endif
