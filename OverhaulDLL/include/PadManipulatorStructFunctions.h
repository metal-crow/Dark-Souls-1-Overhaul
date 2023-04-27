#pragma once
#ifndef PadManipulatorSTRUCTFUNCTIONS_H
#define PadManipulatorSTRUCTFUNCTIONS_H

#include <stdint.h>
#include "PadManipulatorStruct.h"

void copy_PadManipulator(PadManipulator* to, PadManipulator* from);
PadManipulator* init_PadManipulator();
void free_PadManipulator(PadManipulator* to);

#endif
