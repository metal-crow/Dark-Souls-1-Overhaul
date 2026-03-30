#pragma once
#ifndef FRPGHAVOKMAN_FUNCTIONS_H
#define FRPGHAVOKMAN_FUNCTIONS_H

#include <stdint.h>
#include <string>
#include "FrpgHavokManImpStruct.h"
#include "Rollback.h"

/* ---------------- hkpWorld ------------------ */

void copy_FrpgHavokManImp(FrpgHavokManImp* to, const FrpgHavokManImp* from, StateTarget target);
FrpgHavokManImp* init_FrpgHavokManImp();
void free_FrpgHavokManImp(FrpgHavokManImp* to);

void copy_FrpgPhysWorld(FrpgPhysWorld* to, const FrpgPhysWorld* from, StateTarget target);
FrpgPhysWorld* init_FrpgPhysWorld();
void free_FrpgPhysWorld(FrpgPhysWorld* to);

void copy_hkpWorld(hkpWorld* to, const hkpWorld* from, StateTarget target);


#endif
