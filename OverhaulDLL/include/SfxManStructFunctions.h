#pragma once
#ifndef SFXMAN_STRUCTFUNCTIONS_H
#define SFXMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include <string>

#include "SfxManStruct.h"
#include "Rollback.h"

class StateVisitor;

// ---- SfxManState----
SfxManState* init_SfxMan();
void free_SfxMan(SfxManState* to);
//StateTarget::Copy
void copy_SfxMan(SfxManState* to, const SfxManState* from);
//StateTarget::ToLocal: when saving the handle owners (players, BulletMan)
void save_SfxMan(SfxManState* to);
//StateTarget::ToGame: before and after restoring the handle owners
void load_SfxMan(const SfxManState* from);
void load_SfxMan_finish();

// ---- FX handles (SfxHandle) inside rolled-back structs ----
//ToLocal: the handle's links as values the load can check.
//ToGame: unlink the game handle, then link it to the saved effect if that effect still exists, else leave it empty.
//Copy: plain copy.
void copy_SfxHandle(SfxHandle* to, const SfxHandle* from, StateTarget target);
//Take a game handle out of its effect's handle list before its memory is freed or overwritten (the game would destruct it)
void release_SfxHandle(SfxHandle* h);
//A local handle that points at nothing (pool entries not in use)
void clear_SfxHandle(SfxHandle* h);
//A saved handle in its owner's hash: the vtable only. Which effect it points at is FX state, which is not compared.
void serialize_SfxHandle(StateVisitor& v, const char* name, const SfxHandle* h);

// ---- session / frame bookkeeping, called from Rollback.cpp ----
void sfx_rollback_install();              //Rollback::start
void sfx_rollback_session_start();
void sfx_rollback_session_end();
void sfx_rollback_saved_frame(int32_t frame);      //each GGPO save: the frame now being simulated
void sfx_rollback_confirmed_frame(int32_t frame);  //frames up to this one can no longer be rolled back

#ifdef GGPO_SYNCTEST
//Synctest restore check: around each GGPO save, record what every saved handle points at; a re-save of a frame is compared
//with its original (see SfxManStructFunctions.cpp)
void sfx_synctest_capture_begin();
void sfx_synctest_capture_end(int32_t frame);
void sfx_synctest_reset();
std::string sfx_synctest_json();
//Handles saved inside one belong to this attach slot. The check matches them by slot type: the call stack would say how deep
//in the (rotating) slot chain the slot sits. Differences in a cosmetic slot's handles (one the state hash leaves out) are
//counted apart from the errors.
struct SfxSynctestOwner
{
    int64_t slot_type;   //-1: not inside an attach slot
    bool cosmetic;
};
extern SfxSynctestOwner sfx_synctest_owner;
struct SfxSynctestOwnerScope
{
    SfxSynctestOwner prev;
    SfxSynctestOwnerScope(uint32_t slot_type, bool cosmetic) : prev(sfx_synctest_owner) { sfx_synctest_owner = { slot_type, cosmetic }; }
    ~SfxSynctestOwnerScope() { sfx_synctest_owner = prev; }
};
#endif

//When we resimulate a frame using Step_GameSimulation, we inject code to mark the sfx generated
//this tells us to pay attention and handle any generated sfx as part of rollback state
extern "C" int32_t sfx_sim_step_depth;
struct SfxSimStepScope
{
    SfxSimStepScope() { sfx_sim_step_depth++; }
    ~SfxSimStepScope() { sfx_sim_step_depth--; }
};

#endif
