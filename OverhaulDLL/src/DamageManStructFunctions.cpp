#include "DamageManStructFunctions.h"
#include "PlayerInsStructFunctions.h"
#include "Rollback.h"

static const size_t max_preallocated_DamageEntry = 128;

void copy_DamageMan(DamageMan* to, DamageMan* from, bool to_game)
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

        copy_DamageEntry(to_DamageEntry, from_DamageEntry, to_game);

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

    free(to);
}

void copy_DamageEntry(DamageEntry* to, DamageEntry* from, bool to_game)
{
    to->data_0 = from->data_0;
    //these need to be run first
    copy_FrpgPhysShapePhantomIns(&to->FrpgPhysShapePhantomIns_Sphere, &from->FrpgPhysShapePhantomIns_Sphere, true, to_game);
    copy_FrpgPhysShapePhantomIns(&to->FrpgPhysShapePhantomIns_Capsule, &from->FrpgPhysShapePhantomIns_Capsule, false, to_game);
    //these are handled by the above FrpgPhysShapePhantomIns
    to->hkpSphereShape1 = to->FrpgPhysShapePhantomIns_Sphere->_hkpSphereShape;
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
    copy_DamageEntryField0x118(&to->field0x118, &from->field0x118, to_game);
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

    local_DamageEntry->FrpgPhysShapePhantomIns_Sphere = init_FrpgPhysShapePhantomIns(true, false);
    local_DamageEntry->FrpgPhysShapePhantomIns_Capsule = init_FrpgPhysShapePhantomIns(false, false);
    local_DamageEntry->hkpSphereShape1 = local_DamageEntry->FrpgPhysShapePhantomIns_Sphere->_hkpSphereShape;
    local_DamageEntry->hkpCapsuleShape1 = local_DamageEntry->FrpgPhysShapePhantomIns_Capsule->_hkpCapsuleShape;
    local_DamageEntry->field0x118 = init_DamageEntryField0x118(false);

    return local_DamageEntry;
}

void free_DamageEntry(DamageEntry* to, bool freeself)
{
    free_FrpgPhysShapePhantomIns(to->FrpgPhysShapePhantomIns_Sphere, true, false);
    free_FrpgPhysShapePhantomIns(to->FrpgPhysShapePhantomIns_Capsule, false, false);
    to->hkpSphereShape1 = NULL;
    to->hkpCapsuleShape1 = NULL;
    free_DamageEntryField0x118(to->field0x118, false);

    if (freeself)
    {
        free(to);
    }
}

void copy_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns** to, FrpgPhysShapePhantomIns** from, bool is_sphere, bool to_game)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_FrpgPhysShapePhantomIns(is_sphere, to_game);
    }
    if (*to != NULL && *from == NULL)
    {
        free_FrpgPhysShapePhantomIns(*to, is_sphere, to_game);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        (*to)->data_0 = (*from)->data_0;
        (*to)->damageEntry = (*from)->damageEntry;
        (*to)->physWorld = (*from)->physWorld;
        copy_hkpSimpleShapePhantom((*to)->_hkpSimpleShapePhantom, (*from)->_hkpSimpleShapePhantom);
        (*to)->self = (*to);
        (*to)->data_1 = (*from)->data_1;
        if (is_sphere)
        {
            copy_hkpSphereShape(&(*to)->_hkpSphereShape, &(*from)->_hkpSphereShape, to_game);
        }
        else
        {
            copy_hkpCapsuleShape(&(*to)->_hkpCapsuleShape, &(*from)->_hkpCapsuleShape, to_game);
        }
    }
}

FrpgPhysShapePhantomIns* init_FrpgPhysShapePhantomIns(bool is_sphere, bool to_game)
{
    FrpgPhysShapePhantomIns* local;
    if (to_game)
    {
        local = (FrpgPhysShapePhantomIns*)Game::game_malloc(sizeof(FrpgPhysShapePhantomIns), 8, *(uint64_t*)Game::internal_heap_2);
        local->_hkpSimpleShapePhantom = (hkpSimpleShapePhantom*)Game::thread_malloc(sizeof(hkpSimpleShapePhantom));
    }
    else
    {
        local = (FrpgPhysShapePhantomIns*)malloc_(sizeof(FrpgPhysShapePhantomIns));
        local->_hkpSimpleShapePhantom = init_hkpSimpleShapePhantom();
    }

    if (is_sphere)
    {
        local->_hkpSphereShape = init_hkpSphereShape(to_game);
    }
    else
    {
        local->_hkpCapsuleShape = init_hkpCapsuleShape(to_game);
    }
    return local;
}

void free_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns* to, bool is_sphere, bool to_game)
{
    if (to_game)
    {
        Game::thread_free(to->_hkpSimpleShapePhantom, sizeof(hkpSimpleShapePhantom));
    }
    else
    {
        free_hkpSimpleShapePhantom(to->_hkpSimpleShapePhantom);
    }
    if (is_sphere)
    {
        free_hkpSphereShape(to->_hkpSphereShape, to_game);
    }
    else
    {
        free_hkpCapsuleShape(to->_hkpCapsuleShape, to_game);
    }
    if (to_game)
    {
        Game::game_free(to);
    }
    else
    {
        free(to);
    }
}

void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape** from, bool to_game)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_hkpSphereShape(to_game);
    }
    if (*to != NULL && *from == NULL)
    {
        free_hkpSphereShape(*to, to_game);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        memcpy((*to)->data_0, (*from)->data_0, sizeof((*to)->data_0));
    }
}

hkpSphereShape* init_hkpSphereShape(bool to_game)
{
    hkpSphereShape* local;
    if (to_game)
    {
        local = (hkpSphereShape*)Game::thread_malloc(sizeof(hkpSphereShape));
    }
    else
    {
        local = (hkpSphereShape*)malloc_(sizeof(hkpSphereShape));
    }
    return local;
}

void free_hkpSphereShape(hkpSphereShape* to, bool to_game)
{
    if (to_game)
    {
        Game::thread_free(to, sizeof(hkpSphereShape));
    }
    else
    {
        free(to);
    }
}

void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape** from, bool to_game)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_hkpCapsuleShape(to_game);
    }
    if (*to != NULL && *from == NULL)
    {
        free_hkpCapsuleShape(*to, to_game);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        memcpy((*to)->data_0, (*from)->data_0, sizeof((*to)->data_0));
    }
}

hkpCapsuleShape* init_hkpCapsuleShape(bool to_game)
{
    hkpCapsuleShape* local;
    if (to_game)
    {
        local = (hkpCapsuleShape*)Game::thread_malloc(sizeof(hkpCapsuleShape));
    }
    else
    {
        local = (hkpCapsuleShape*)malloc_(sizeof(hkpCapsuleShape));
    }
    return local;
}

void free_hkpCapsuleShape(hkpCapsuleShape* to, bool to_game)
{
    if (to_game)
    {
        Game::thread_free(to, sizeof(hkpCapsuleShape));
    }
    else
    {
        free(to);
    }
}

void copy_DamageEntryField0x118(DamageEntryField0x118** to, DamageEntryField0x118** from, bool to_game)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_DamageEntryField0x118(to_game);
    }
    if (*to != NULL && *from == NULL)
    {
        free_DamageEntryField0x118(*to, to_game);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        memcpy((*to)->data_0, (*from)->data_0, sizeof((*to)->data_0));
    }
}

DamageEntryField0x118* init_DamageEntryField0x118(bool to_game)
{
    DamageEntryField0x118* local;
    if (to_game)
    {
        local = (DamageEntryField0x118*)Game::game_malloc(sizeof(DamageEntryField0x118), 16, *(uint64_t*)Game::internal_heap_3);
    }
    else
    {
        local = (DamageEntryField0x118*)malloc_(sizeof(DamageEntryField0x118));
    }
    return local;
}

void free_DamageEntryField0x118(DamageEntryField0x118* to, bool to_game)
{
    if (to_game)
    {
        Game::game_free_alt(to, *(void**)Game::internal_heap_3);
    }
    else
    {
        free(to);
    }
}
