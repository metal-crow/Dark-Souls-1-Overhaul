#include "DamageManStructFunctions.h"
#include "PlayerInsStructFunctions.h"
#include "FrpgHavokManImpStructFunctions.h"

//the game preallocates the DamageMan active_damage_entries_list to be 128 elements long. Instead of dynamically trying to resize it, lets just never save above the 128 max
static const size_t max_preallocated_DamageEntry = 128;

void copy_DamageMan(DamageMan* to, DamageMan* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target)
{
    Game::SuspendThreads();

    uint64_t active_damage_entries_list_offset = (uint64_t)from->active_damage_entries_list - (uint64_t)from->all_damage_entries_list_start;
    to->active_damage_entries_list = (DamageEntry*)((uint64_t)to->all_damage_entries_list_start + active_damage_entries_list_offset);

    uint64_t all_damage_entries_list_cur_offset = (uint64_t)from->all_damage_entries_list_cur - (uint64_t)from->all_damage_entries_list_start;
    to->all_damage_entries_list_cur = (DamageEntry*)((uint64_t)to->all_damage_entries_list_start + all_damage_entries_list_cur_offset);

    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        DamageEntry* from_DamageEntry = &from->all_damage_entries_list_start[i];
        DamageEntry* to_DamageEntry = &to->all_damage_entries_list_start[i];

        copy_DamageEntry(to_DamageEntry, from_DamageEntry, to_world, from_world, target);

        if (from_DamageEntry->next != NULL)
        {
            uint64_t from_DamageEntry_nextoffset = (uint64_t)from_DamageEntry->next - (uint64_t)from->all_damage_entries_list_start;
            to_DamageEntry->next = (DamageEntry*)((uint64_t)to->all_damage_entries_list_start + from_DamageEntry_nextoffset);
        }
        else
        {
            to_DamageEntry->next = NULL;
        }

        //handle the followup ptrs, since they are also in this list
        if (from_DamageEntry->followup_a != NULL)
        {
            uint64_t a_nextoffset = (uint64_t)from_DamageEntry->followup_a - (uint64_t)from->all_damage_entries_list_start;
            to_DamageEntry->followup_a = (DamageEntry*)((uint64_t)to->all_damage_entries_list_start + a_nextoffset);
        }
        else
        {
            to_DamageEntry->followup_a = NULL;
        }
        if (from_DamageEntry->followup_b != NULL)
        {
            uint64_t a_nextoffset = (uint64_t)from_DamageEntry->followup_b - (uint64_t)from->all_damage_entries_list_start;
            to_DamageEntry->followup_b = (DamageEntry*)((uint64_t)to->all_damage_entries_list_start + a_nextoffset);
        }
        else
        {
            to_DamageEntry->followup_b = NULL;
        }
        if (from_DamageEntry->followup_c != NULL)
        {
            uint64_t a_nextoffset = (uint64_t)from_DamageEntry->followup_c - (uint64_t)from->all_damage_entries_list_start;
            to_DamageEntry->followup_c = (DamageEntry*)((uint64_t)to->all_damage_entries_list_start + a_nextoffset);
        }
        else
        {
            to_DamageEntry->followup_c = NULL;
        }
    }

    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));

    Game::ResumeThreads();
}

//Since the game has pre-init'd all of the DamageEntries already, we don't need to ever init for the game, we can copy in place. So this and all child init's are just for the mod memory
DamageMan* init_DamageMan()
{
    DamageMan* local_DamageMan = (DamageMan*)malloc_(sizeof(DamageMan));

    local_DamageMan->active_damage_entries_list = NULL;
    local_DamageMan->all_damage_entries_list_cur = NULL;
    local_DamageMan->all_damage_entries_list_start = NULL;

    local_DamageMan->all_damage_entries_list_start = (DamageEntry*)malloc_(sizeof(DamageEntry)*max_preallocated_DamageEntry);
    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        DamageEntry* local_DamageEntry = init_DamageEntry();
        local_DamageMan->all_damage_entries_list_start[i] = *local_DamageEntry;
        free(local_DamageEntry);
    }

    return local_DamageMan;
}

void free_DamageMan(DamageMan* to)
{
    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        free_DamageEntry(&to->all_damage_entries_list_start[i], false);
    }
    free(to->all_damage_entries_list_start);

    free(to);
}

void copy_DamageEntry(DamageEntry* to, DamageEntry* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target)
{
    to->data_0 = from->data_0;
    //these need to be run first
    if (from->FrpgPhysShapePhantomIns_Sphere == NULL || from->FrpgPhysShapePhantomIns_Capsule == NULL)
    {
        FATALERROR("FrpgPhysShapePhantomIns can be null??? from=%p sphere=%p cap=%p", from, from->FrpgPhysShapePhantomIns_Sphere, from->FrpgPhysShapePhantomIns_Capsule);
    }
    copy_FrpgPhysShapePhantomIns(&to->FrpgPhysShapePhantomIns_Sphere, &from->FrpgPhysShapePhantomIns_Sphere, true, to_world, from_world, target);
    copy_FrpgPhysShapePhantomIns(&to->FrpgPhysShapePhantomIns_Capsule, &from->FrpgPhysShapePhantomIns_Capsule, false, to_world, from_world, target);

    //these are handled by the above FrpgPhysShapePhantomIns
    if (from->hkpSphereShape1 != from->FrpgPhysShapePhantomIns_Sphere->_hkpSphereShape)
    {
        FATALERROR("hkpSphereShape1 %p not pointing to FrpgPhysShapePhantomIns_Sphere->_hkpSphereShape %p",
            from->hkpSphereShape1, from->FrpgPhysShapePhantomIns_Sphere->_hkpSphereShape);
    }
    to->hkpSphereShape1 = to->FrpgPhysShapePhantomIns_Sphere->_hkpSphereShape;
    if (from->hkpCapsuleShape1 != from->FrpgPhysShapePhantomIns_Capsule->_hkpCapsuleShape)
    {
        FATALERROR("hkpCapsuleShape1 %p not pointing to FrpgPhysShapePhantomIns_Capsule->_hkpCapsuleShape %p",
            from->hkpCapsuleShape1, from->FrpgPhysShapePhantomIns_Capsule->_hkpCapsuleShape);
    }
    to->hkpCapsuleShape1 = to->FrpgPhysShapePhantomIns_Capsule->_hkpCapsuleShape;

    //these always points to either the sphere or the capsule
    if (from->PhysShapePhantomIns1 == from->FrpgPhysShapePhantomIns_Sphere)
    {
        to->PhysShapePhantomIns1 = to->FrpgPhysShapePhantomIns_Sphere;
    }
    else if (from->PhysShapePhantomIns1 == from->FrpgPhysShapePhantomIns_Capsule)
    {
        to->PhysShapePhantomIns1 = to->FrpgPhysShapePhantomIns_Capsule;
    }
    else if (from->PhysShapePhantomIns1 == NULL)
    {
        to->PhysShapePhantomIns1 = NULL;
    }
    else
    {
        FATALERROR("PhysShapePhantomIns1 %p FrpgPhysShapePhantomIns_Sphere %p FrpgPhysShapePhantomIns_Capsule %p",
            to->PhysShapePhantomIns1, to->FrpgPhysShapePhantomIns_Sphere, to->FrpgPhysShapePhantomIns_Capsule);
    }

    if (from->PhysShapePhantomIns1_altPtr_A == from->FrpgPhysShapePhantomIns_Sphere)
    {
        to->PhysShapePhantomIns1_altPtr_A = to->FrpgPhysShapePhantomIns_Sphere;
    }
    else if (from->PhysShapePhantomIns1_altPtr_A == from->FrpgPhysShapePhantomIns_Capsule)
    {
        to->PhysShapePhantomIns1_altPtr_A = to->FrpgPhysShapePhantomIns_Capsule;
    }
    else if (from->PhysShapePhantomIns1_altPtr_A == NULL)
    {
        to->PhysShapePhantomIns1_altPtr_A = NULL;
    }
    else
    {
        FATALERROR("PhysShapePhantomIns1_altPtr_A %p FrpgPhysShapePhantomIns_Sphere %p FrpgPhysShapePhantomIns_Capsule %p",
            to->PhysShapePhantomIns1_altPtr_A, to->FrpgPhysShapePhantomIns_Sphere, to->FrpgPhysShapePhantomIns_Capsule);
    }

    if (from->PhysShapePhantomIns1_altPtr_B == from->FrpgPhysShapePhantomIns_Sphere)
    {
        to->PhysShapePhantomIns1_altPtr_B = to->FrpgPhysShapePhantomIns_Sphere;
    }
    else if (from->PhysShapePhantomIns1_altPtr_B == from->FrpgPhysShapePhantomIns_Capsule)
    {
        to->PhysShapePhantomIns1_altPtr_B = to->FrpgPhysShapePhantomIns_Capsule;
    }
    else if (from->PhysShapePhantomIns1_altPtr_B == NULL)
    {
        to->PhysShapePhantomIns1_altPtr_B = NULL;
    }
    else
    {
        FATALERROR("PhysShapePhantomIns1_altPtr_B %p FrpgPhysShapePhantomIns_Sphere %p FrpgPhysShapePhantomIns_Capsule %p",
            to->PhysShapePhantomIns1_altPtr_B, to->FrpgPhysShapePhantomIns_Sphere, to->FrpgPhysShapePhantomIns_Capsule);
    }

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_DamageEntryField0x118(&to->field0x118, &from->field0x118, target);
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    to->DmgHitRecordManImp_field0x10Elem = from->DmgHitRecordManImp_field0x10Elem;
    to->physWorld = from->physWorld;
    //ignore followup, that's handled by the caller
    memcpy(to->data_6, from->data_6, sizeof(to->data_6));
    //ignore next, that's handled by the caller
    to->data_7 = from->data_7;
}

DamageEntry* init_DamageEntry()
{
    DamageEntry* local_DamageEntry = (DamageEntry*)malloc_(sizeof(DamageEntry));

    local_DamageEntry->FrpgPhysShapePhantomIns_Sphere = init_FrpgPhysShapePhantomIns(true, StateTarget::ToLocal);
    local_DamageEntry->FrpgPhysShapePhantomIns_Capsule = init_FrpgPhysShapePhantomIns(false, StateTarget::ToLocal);
    local_DamageEntry->hkpSphereShape1 = local_DamageEntry->FrpgPhysShapePhantomIns_Sphere->_hkpSphereShape;
    local_DamageEntry->hkpCapsuleShape1 = local_DamageEntry->FrpgPhysShapePhantomIns_Capsule->_hkpCapsuleShape;
    local_DamageEntry->field0x118 = init_DamageEntryField0x118(StateTarget::ToLocal);

    return local_DamageEntry;
}

void free_DamageEntry(DamageEntry* to, bool freeself)
{
    free_FrpgPhysShapePhantomIns(to->FrpgPhysShapePhantomIns_Sphere, true);
    free_FrpgPhysShapePhantomIns(to->FrpgPhysShapePhantomIns_Capsule, false);
    to->hkpSphereShape1 = NULL;
    to->hkpCapsuleShape1 = NULL;
    free_DamageEntryField0x118(to->field0x118, StateTarget::ToLocal);

    if (freeself)
    {
        free(to);
    }
}

void copy_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns** to, FrpgPhysShapePhantomIns** from, bool is_sphere, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target)
{
    if (*to == NULL && *from != NULL)
    {
        FATALERROR("WARNING: I shouldn't have to init the FrpgPhysShapePhantomIns, it should be pre-init'ed either by me or the game");
    }
    if (*to != NULL && *from == NULL)
    {
        FATALERROR("WARNING: I shouldn't have to free the FrpgPhysShapePhantomIns, it should always exist");
    }
    if (*to != NULL && *from != NULL)
    {
        (*to)->data_0 = (*from)->data_0;
        (*to)->damageEntry = (*from)->damageEntry;
        (*to)->physWorld = (*from)->physWorld;

        if ((*from)->_hkpSimpleShapePhantom == NULL)
        {
            FATALERROR("ToGame: SimpleShapePhantom ptr for %p is NULL", (*from));
        }
        //the hkpSimpleShapePhantom has only been added to the hkpWorld if physWorld is non-null. If not we have to manually manage it
        if ((*from)->physWorld == NULL)
        {
            hk_ref((*from)->_hkpSimpleShapePhantom);
        }
        //since the phantoms are never destroyed due to us keeping a ref to it, it's safe to just use the raw pointer. it should always be valid
        (*to)->_hkpSimpleShapePhantom = (*from)->_hkpSimpleShapePhantom;

        (*to)->self = (*to);
        (*to)->data_1 = (*from)->data_1;
        if (is_sphere)
        {
            copy_hkpSphereShape(&(*to)->_hkpSphereShape, (*from)->_hkpSphereShape, target);
        }
        else
        {
            copy_hkpCapsuleShape(&(*to)->_hkpCapsuleShape, (*from)->_hkpCapsuleShape, target);
        }
    }
}

FrpgPhysShapePhantomIns* init_FrpgPhysShapePhantomIns(bool is_sphere, StateTarget target)
{
    FrpgPhysShapePhantomIns* local;
    if (target == StateTarget::ToGame)
    {
        local = (FrpgPhysShapePhantomIns*)Game::game_malloc(sizeof(FrpgPhysShapePhantomIns), 8, *(uint64_t*)Game::internal_heap_2);
    }
    else
    {
        local = (FrpgPhysShapePhantomIns*)malloc_(sizeof(FrpgPhysShapePhantomIns));
    }

    local->_hkpSimpleShapePhantom = NULL;

    if (is_sphere)
    {
        local->_hkpSphereShape = init_hkpSphereShape(target);
    }
    else
    {
        local->_hkpCapsuleShape = init_hkpCapsuleShape(target);
    }
    return local;
}

void free_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns* to, bool is_sphere)
{
    if (is_sphere)
    {
        free_hkpSphereShape(to->_hkpSphereShape, StateTarget::ToLocal);
    }
    else
    {
        free_hkpCapsuleShape(to->_hkpCapsuleShape, StateTarget::ToLocal);
    }
    if (to->physWorld == NULL)
    {
        //now that we're freeing the rollback obj, deref the manually saved phantom
        hk_deref(to->_hkpSimpleShapePhantom);
    }
    free(to);
}

void copy_DamageEntryField0x118(DamageEntryField0x118** to, DamageEntryField0x118** from, StateTarget target)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_DamageEntryField0x118(target);
    }
    if (*to != NULL && *from == NULL)
    {
        free_DamageEntryField0x118(*to, target);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        memcpy((*to)->data_0, (*from)->data_0, sizeof((*to)->data_0));
    }
}

DamageEntryField0x118* init_DamageEntryField0x118(StateTarget target)
{
    DamageEntryField0x118* local;
    if (target == StateTarget::ToGame)
    {
        local = (DamageEntryField0x118*)Game::game_malloc(sizeof(DamageEntryField0x118), 16, *(uint64_t*)Game::internal_heap_3);
    }
    else
    {
        local = (DamageEntryField0x118*)malloc_(sizeof(DamageEntryField0x118));
    }
    return local;
}

void free_DamageEntryField0x118(DamageEntryField0x118* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        Game::game_free_alt(to, *(void**)Game::internal_heap_3);
    }
    else
    {
        free(to);
    }
}
