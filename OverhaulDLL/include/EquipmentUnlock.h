#pragma once

#ifndef _DS1_OVERHAUL_EQUIPMENT_UNLOCK_H_
#define _DS1_OVERHAUL_EQUIPMENT_UNLOCK_H_

#include <cstdint>

class EquipmentUnlock
{
public:
    static void start();
private:
    static const uint64_t menu_entry_allowed_check_injection_offset = 0x630d37;
    static const uint64_t disable_remove_equipment_check_injection_offset = 0x630c89;
    static const uint64_t equipment_change_unlock_injection_offset = 0x630df1;
};

#endif
