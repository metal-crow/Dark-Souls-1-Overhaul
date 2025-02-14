#pragma once
#ifndef PadManipulatorSTRUCTFUNCTIONS_H
#define PadManipulatorSTRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "PadManipulatorStruct.h"

std::string print_PadManipulatorPacked(PadManipulatorPacked* to);
std::string print_PadManipulator(PadManipulator* pad);
void PadManipulator_to_PadManipulatorPacked(PadManipulatorPacked* to, PadManipulator* from);
void PadManipulatorPacked_to_PadManipulator(PadManipulator* to, PadManipulatorPacked* from, bool networkedPc);

void copy_PadManipulator(PadManipulator* to, PadManipulator* from);
PadManipulator* init_PadManipulator();
void free_PadManipulator(PadManipulator* to);

#endif
