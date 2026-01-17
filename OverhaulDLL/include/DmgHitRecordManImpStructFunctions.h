#pragma once
#ifndef DMGHITRECORD_FUNCTIONS_H
#define DMGHITRECORD_FUNCTIONS_H

#include <stdint.h>
#include <string>
#include "DmgHitRecordManImpStruct.h"
#include "Rollback.h"

void copy_DmgHitRecordManImp(DmgHitRecordManImp* to, DmgHitRecordManImp* from, StateTarget target);
DmgHitRecordManImp* init_DmgHitRecordManImp();
void free_DmgHitRecordManImp(DmgHitRecordManImp* to);

void copy_DmgHitRecordManImp_field0x10Elem(DmgHitRecordManImp_field0x10Elem* to, DmgHitRecordManImp_field0x10Elem* from, StateTarget target);

void copy_DmgHitRecordManImp_field0x28Elem(DmgHitRecordManImp_field0x28Elem* to, DmgHitRecordManImp_field0x28Elem* from, StateTarget target);

#endif
