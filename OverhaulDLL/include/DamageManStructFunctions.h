#pragma once
#ifndef DAMAGEMANSTRUCTFUNCTIONS_H
#define DAMAGEMANSTRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>
#include "DamageManStruct.h"
#include "Rollback.h"

class StateVisitor;

void copy_DamageMan(DamageMan* to, DamageMan* from, StateTarget target);
// if DamageMan_PopHead_DamageEntry heap-allocated the entry because the pool was empty
bool DamageEntry_isDynamicAlloc(const DamageEntry* entry);
DamageMan* init_DamageMan();
void free_DamageMan(DamageMan* to);
void serialize_DamageMan(StateVisitor& v, DamageMan* d);
std::string print_DamageMan(DamageMan* d);
uint64_t hash_DamageMan(DamageMan* d);

//The saved DamageMan that damage entry ids held elsewhere (ChrIns' current entries) are resolved against while serializing. An id
//names a pool slot and that slot's reuse count, both of which depend on world-owned entries rollback does not cover, so a reference
//is compared as WHICH player-owned entry it names. Set by DamageEntryRefContext for the duration of one state's serialization.
extern const DamageMan* serialize_damage_context;
struct DamageEntryRefContext
{
    const DamageMan* prev;
    explicit DamageEntryRefContext(const DamageMan* d) : prev(serialize_damage_context) { serialize_damage_context = d; }
    ~DamageEntryRefContext() { serialize_damage_context = prev; }
};
//A damage entry id: -1 for none, else the index among the player-owned active entries of serialize_damage_context, -2 if it names
//no active entry, -3 if it names a world-owned one. The raw id is printed alongside.
void serialize_DamageEntry_ref(StateVisitor& v, const char* name, uint32_t id);

void copy_DamageEntry(DamageEntry* to, DamageEntry* from, StateTarget target);
DamageEntry* init_DamageEntry();
void free_DamageEntry(DamageEntry* to, bool freeself);

void copy_FrpgPhysIns(FrpgPhysIns* to, FrpgPhysIns* from, StateTarget target);
void copy_FrpgPhysPhantomIns(FrpgPhysPhantomIns* to, FrpgPhysPhantomIns* from, StateTarget target);
void copy_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns** to, FrpgPhysShapePhantomIns** from, StateTarget target);
FrpgPhysShapePhantomIns* init_FrpgPhysShapePhantomIns(bool is_sphere);
void free_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns* to);

void copy_DamageEntryField0x118(DamageEntryField0x118** to, DamageEntryField0x118** from, StateTarget target);
DamageEntryField0x118* init_DamageEntryField0x118(StateTarget target);
void free_DamageEntryField0x118(DamageEntryField0x118* to, StateTarget target);

#endif
