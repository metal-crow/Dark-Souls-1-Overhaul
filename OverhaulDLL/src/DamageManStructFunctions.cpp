#include "DamageManStructFunctions.h"
#include "PlayerInsStructFunctions.h"
#include "FrpgHavokManImpStructFunctions.h"

/* ============================================================
 * Dynamic object management
 * ============================================================ */

bool DamageEntry_isDynamicAlloc(DamageEntry* to)
{
    return to->id >= 0x800000; //the high byte is 0x80 when dynamically alloc'd
}

//these objects are not ref counted, so we must do so manually
static std::unordered_map<void*, int32_t> g_DamageEntryGraveyard;

void DamageEntry_ref(DamageEntry* DamageEntry)
{
    auto search = g_DamageEntryGraveyard.find(DamageEntry);
    if (search != g_DamageEntryGraveyard.end())
    {
        g_DamageEntryGraveyard[DamageEntry] += 1;
    }
    else
    {
        g_DamageEntryGraveyard.insert({ DamageEntry, 1 });
    }
}

void DamageEntry_deref(DamageEntry* DamageEntry)
{
    auto search = g_DamageEntryGraveyard.find(DamageEntry);
    if (search != g_DamageEntryGraveyard.end())
    {
        g_DamageEntryGraveyard[DamageEntry] -= 1;
        if (g_DamageEntryGraveyard[DamageEntry] == 0)
        {
            Destruct_DamageEntry(DamageEntry);
            Game::game_free_alt(DamageEntry); //the destruct call doesn't free it
            g_DamageEntryGraveyard.erase(DamageEntry);
        }
    }
    else if (search == g_DamageEntryGraveyard.end())
    {
        Destruct_DamageEntry(DamageEntry);
        Game::game_free_alt(DamageEntry);
    }
}

//manually perform the destruct operations here, if needed
void OnDamageEntryDestruct(void* DamageEntry)
{
    auto search = g_DamageEntryGraveyard.find(DamageEntry);
    if (search == g_DamageEntryGraveyard.end())
    {
        //nothing keeping this alive, let it be destroyed
        Destruct_DamageEntry(DamageEntry);
        Game::game_free_alt(DamageEntry);
        return;
    }
    //count a game-attempted destruction as a deref
    int32_t refcount = search->second;
    refcount -= 1;
    g_DamageEntryGraveyard[DamageEntry] = refcount;
    if (refcount <= 0)
    {
        g_DamageEntryGraveyard.erase(DamageEntry);
        Destruct_DamageEntry(DamageEntry);
        Game::game_free_alt(DamageEntry);
        return;
    }
    return;
}

/* ============================================================ */

//the game allocates the DamageMan active_damage_entries_list to be 128 elements long. Instead of having a dynamic local-side array, lets just prealloc enough to fit the 128 max
static const size_t max_preallocated_DamageEntry = 128;

void copy_DamageMan(DamageMan* to, DamageMan* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target)
{
    Game::SuspendThreads();

    //go through the active_damage_entries_list and save it, along with the data of any dynamic entries. Any static ones are contained in the all_damage_entries_list
    if (target == StateTarget::ToLocal)
    {
        free_SavedDamageEntryList(&to->saved_active_damage_entries);
        DamageEntry* head = from->active_damage_entries_list;
        while (head != NULL)
        {
            SavedDamageEntry e;
            e.game_addr = head;
            e.is_dynamic = false;
            e.data = NULL;
            if (DamageEntry_isDynamicAlloc(head))
            {
                DamageEntry_ref(head); //ref because the entry is stored game-side
                DamageEntry_ref(head); //ref for the storage local-side
                e.data = init_DamageEntry();
                copy_DamageEntry(e.data, head, to_world, from_world, target);
                e.is_dynamic = true;
            }
            to->saved_active_damage_entries.push_back(e);
            head = head->next;
        }
    }
    else if (target == StateTarget::ToGame)
    {
        //clear out the existing list first
        //this is needed in case we need to destroy any dynamic entries in it
        //this may also mean we remove and put back the same element but since those are still ref'd by the saved side it's safe
        DamageEntry* oldhead = to->active_damage_entries_list;
        while (oldhead != NULL)
        {
            DamageEntry* next = oldhead->next;
            if (DamageEntry_isDynamicAlloc(oldhead))
            {
                DamageEntry_deref(oldhead);
            }
            oldhead = next;
        }

        //push back the correct pointers into the active_damage_entries_list
        to->active_damage_entries_list = NULL;
        DamageEntry** head = &to->active_damage_entries_list;
        for (auto e : from->saved_active_damage_entries)
        {
            *head = e.game_addr;
            if (e.is_dynamic)
            {
                DamageEntry_ref(*head);
                copy_DamageEntry(*head, e.data, to_world, from_world, target);
            }
            head = &((*head)->next);
        }
    }
    else if (target == StateTarget::Copy)
    {
        free_SavedDamageEntryList(&to->saved_active_damage_entries);
        for (auto e : from->saved_active_damage_entries)
        {
            SavedDamageEntry new_e;
            new_e.game_addr = e.game_addr;
            new_e.is_dynamic = e.is_dynamic;
            new_e.data = NULL;
            if (e.is_dynamic)
            {
                DamageEntry_ref(new_e.game_addr);
                new_e.data = init_DamageEntry();
                copy_DamageEntry(new_e.data, e.data, to_world, from_world, target);
            }
            to->saved_active_damage_entries.push_back(new_e);
        }

    }

    to->all_damage_entries_list_cur = from->all_damage_entries_list_cur;

    //go through the all_damage_entries_list. This is all static allocations
    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        DamageEntry* from_DamageEntry = &from->all_damage_entries_list_start[i];
        DamageEntry* to_DamageEntry = &to->all_damage_entries_list_start[i];

        copy_DamageEntry(to_DamageEntry, from_DamageEntry, to_world, from_world, target);
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
    free_SavedDamageEntryList(&to->saved_active_damage_entries);
    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        free_DamageEntry(&to->all_damage_entries_list_start[i], false);
    }
    free(to->all_damage_entries_list_start);

    free(to);
}

void free_SavedDamageEntryList(std::vector<SavedDamageEntry>* to)
{
    for (auto e : *to)
    {
        if (e.is_dynamic)
        {
            DamageEntry_deref(e.game_addr);
            if (e.data)
            {
                free_DamageEntry(e.data, true);
            }
        }
    }
    to->clear();
}

void copy_DamageEntry(DamageEntry* to, DamageEntry* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target)
{
    to->id = from->id;
    to->data_0 = from->data_0;

    if (from->FrpgPhysShapePhantomIns_Sphere == NULL || from->FrpgPhysShapePhantomIns_Capsule == NULL)
    {
        FATALERROR("FrpgPhysShapePhantomIns can be null??? from=%p sphere=%p cap=%p", from, from->FrpgPhysShapePhantomIns_Sphere, from->FrpgPhysShapePhantomIns_Capsule);
    }
    copy_FrpgPhysShapePhantomIns(&to->FrpgPhysShapePhantomIns_Sphere, &from->FrpgPhysShapePhantomIns_Sphere, true, to_world, from_world, target);
    copy_FrpgPhysShapePhantomIns(&to->FrpgPhysShapePhantomIns_Capsule, &from->FrpgPhysShapePhantomIns_Capsule, false, to_world, from_world, target);

    //these are all static pointers to the already handled shapes
    to->PhysShapePhantomIns1 = from->PhysShapePhantomIns1;
    to->hkpSphereShape1 = from->hkpSphereShape1;
    to->hkpCapsuleShape1 = from->hkpCapsuleShape1;
    to->PhysShapePhantomIns1_altPtr_A = from->PhysShapePhantomIns1_altPtr_A;
    to->PhysShapePhantomIns1_altPtr_B = from->PhysShapePhantomIns1_altPtr_B;

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_DamageEntryField0x118(&to->field0x118, &from->field0x118, target);
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    to->DmgHitRecordManImp_field0x10Elem = from->DmgHitRecordManImp_field0x10Elem;
    to->physWorld = from->physWorld;
    to->followup_a = from->followup_a;
    to->followup_b = from->followup_b;
    to->followup_c = from->followup_c;
    memcpy(to->data_6, from->data_6, sizeof(to->data_6));
    to->next = from->next;
    to->data_7 = from->data_7;
}

DamageEntry* init_DamageEntry()
{
    DamageEntry* local_DamageEntry = (DamageEntry*)malloc_(sizeof(DamageEntry));

    local_DamageEntry->FrpgPhysShapePhantomIns_Sphere = init_FrpgPhysShapePhantomIns(true);
    local_DamageEntry->FrpgPhysShapePhantomIns_Capsule = init_FrpgPhysShapePhantomIns(false);
    local_DamageEntry->hkpSphereShape1 = local_DamageEntry->FrpgPhysShapePhantomIns_Sphere->_hkpSphereShape;
    local_DamageEntry->hkpCapsuleShape1 = local_DamageEntry->FrpgPhysShapePhantomIns_Capsule->_hkpCapsuleShape;
    local_DamageEntry->field0x118 = init_DamageEntryField0x118(StateTarget::ToLocal);

    return local_DamageEntry;
}

void free_DamageEntry(DamageEntry* to, bool freeself)
{
    free_FrpgPhysShapePhantomIns(to->FrpgPhysShapePhantomIns_Sphere);
    free_FrpgPhysShapePhantomIns(to->FrpgPhysShapePhantomIns_Capsule);
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
            FATALERROR("SimpleShapePhantom ptr for %p is NULL", (*from));
        }
        //since the phantoms (and the associated shape) are never destroyed due to the FrpgHavok rollback code, it's safe to just use the raw pointer. it should always be valid
        (*to)->_hkpSimpleShapePhantom = (*from)->_hkpSimpleShapePhantom;
        (*to)->_shape = (*from)->_shape;

        (*to)->self = (*to);
        (*to)->data_1 = (*from)->data_1;
    }
}

FrpgPhysShapePhantomIns* init_FrpgPhysShapePhantomIns(bool is_sphere)
{
    FrpgPhysShapePhantomIns* local = (FrpgPhysShapePhantomIns*)malloc_(sizeof(FrpgPhysShapePhantomIns));

    local->_hkpSimpleShapePhantom = NULL;
    local->_shape = NULL;

    return local;
}

void free_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns* to)
{
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
