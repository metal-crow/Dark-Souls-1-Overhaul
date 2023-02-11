#pragma once
#ifndef InputDirectionMovementMan_STRUCTFUNCTIONS_H
#define InputDirectionMovementMan_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "InputDirectionMovementManStruct.h"

void copy_InputDirectionMovementMan(InputDirectionMovementMan* to, InputDirectionMovementMan* from);
InputDirectionMovementMan* init_InputDirectionMovementMan();
void free_InputDirectionMovementMan(InputDirectionMovementMan* to);

#endif
