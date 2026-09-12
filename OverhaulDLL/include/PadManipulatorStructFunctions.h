#pragma once
#ifndef PadManipulatorSTRUCTFUNCTIONS_H
#define PadManipulatorSTRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "PadManipulatorStruct.h"
#include "PlayerInsStruct.h"

class StateVisitor;


void copy_PadManipulator(PadManipulator* to, PadManipulator* from);
PadManipulator* init_PadManipulator();
void free_PadManipulator(PadManipulator* to);
void serialize_PadManipulator(StateVisitor& v, const PadManipulator* p);
uint64_t hash_PadManipulator(const PadManipulator* p);
std::string print_PadManipulator(PadManipulator* pad);

#endif
