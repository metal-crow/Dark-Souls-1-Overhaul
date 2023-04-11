#pragma once
#ifndef PadManipulatorSTRUCTFUNCTIONS_H
#define PadManipulatorSTRUCTFUNCTIONS_H

#include <stdint.h>
#include "PadManipulatorStruct.h"

void PadManipulator_to_PadManipulatorPacked(PadManipulatorPacked* to, PadManipulator* from);
void PadManipulatorPacked_to_PadManipulator(PadManipulator* to, PadManipulatorPacked* from);

void copy_PadManipulator(PadManipulator* to, PadManipulator* from);

#endif
