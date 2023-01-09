#include "DamageManStructFunctions.h"
#include "PlayerInsStructFunctions.h"
#include "Rollback.h"

static const size_t max_preallocated_DamageEntry = 128;

void copy_DamageMan(DamageMan* to, DamageMan* from, bool to_game)
{
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
    }

    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
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
    to->PhysShapePhantomIns = from->PhysShapePhantomIns;
    to->FrpgPhysShapePhantomIns_Sphere = from->FrpgPhysShapePhantomIns_Sphere;
    to->FrpgPhysShapePhantomIns_Capsule = from->FrpgPhysShapePhantomIns_Capsule;
    to->hkpSphereShape1 = from->hkpSphereShape1;
    to->hkpCapsuleShape1 = from->hkpCapsuleShape1;
    to->data_1 = from->data_1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    to->hkpCapsuleShape2 = from->hkpCapsuleShape2;
    to->data_3 = from->data_3;
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    to->physWorld = from->physWorld;
    to->simpleShapePhantom = from->simpleShapePhantom;
    to->data_5 = from->data_5;
    to->hkpSphereShape2 = from->hkpSphereShape2;
    to->data_6 = from->data_6;
    to->data_7 = from->data_7;
}

DamageEntry* init_DamageEntry()
{
    DamageEntry* local_DamageEntry = (DamageEntry*)malloc_(sizeof(DamageEntry));

    return local_DamageEntry;
}

void free_DamageEntry(DamageEntry* to, bool freeself)
{


    if (freeself)
    {
        free(to);
    }
}
