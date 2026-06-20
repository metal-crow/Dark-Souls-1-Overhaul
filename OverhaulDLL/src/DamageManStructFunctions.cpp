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

    to->unk_18 = from->unk_18;
    to->put_out_sparks = from->put_out_sparks;
    to->damage_from_weapon = from->damage_from_weapon;
    to->damage_to_occur = from->damage_to_occur;
    memcpy(to->unk_34, from->unk_34, sizeof(to->unk_34));

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
    to->unk_4 = from->unk_4;

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

    to->attackerHandle = from->attackerHandle;
    memcpy(to->attackInfo, from->attackInfo, sizeof(to->attackInfo));
    to->unk_114 = from->unk_114;
    copy_DamageEntryField0x118(&to->field0x118, &from->field0x118, target);
    to->life = from->life;
    to->unk_124 = from->unk_124;
    memcpy(to->unk_128, from->unk_128, sizeof(to->unk_128));
    memcpy(to->field0x130, from->field0x130, sizeof(to->field0x130));
    to->unk_170 = from->unk_170;
    to->unk_174 = from->unk_174;
    to->unk_178 = from->unk_178;
    to->unk_17c = from->unk_17c;
    to->unk_180 = from->unk_180;
    to->unk_184 = from->unk_184;
    to->unk_188 = from->unk_188;
    to->unk_18c = from->unk_18c;
    to->unk_190 = from->unk_190;
    to->unk_194 = from->unk_194;
    to->unk_196 = from->unk_196;
    to->unk_198 = from->unk_198;
    to->unk_19c = from->unk_19c;
    to->unk_1a0 = from->unk_1a0;
    to->unk_1a4 = from->unk_1a4;
    to->unk_1a8 = from->unk_1a8;
    to->unk_1ac = from->unk_1ac;
    to->unk_1b0 = from->unk_1b0;
    to->isSweetSpot = from->isSweetSpot;
    to->isWeakSpot = from->isWeakSpot;
    to->unk_1b6 = from->unk_1b6;
    to->unk_1b8 = from->unk_1b8;
    to->physical_magnification = from->physical_magnification;
    to->magic_magnification = from->magic_magnification;
    to->fire_magnification = from->fire_magnification;
    to->lightning_magnification = from->lightning_magnification;
    to->stamina_dmg_magnification = from->stamina_dmg_magnification;
    to->knockback_percent = from->knockback_percent;
    to->unk_1d4 = from->unk_1d4;
    to->DmgHitRecordManImp_field0x10Elem = from->DmgHitRecordManImp_field0x10Elem;
    to->physWorld = from->physWorld;
    to->followup_a = from->followup_a;
    to->followup_b = from->followup_b;
    to->followup_c = from->followup_c;
    to->num_hits = from->num_hits;
    to->unk_214 = from->unk_214;
    memcpy(to->unk_218, from->unk_218, sizeof(to->unk_218));
    to->next = from->next;
    to->unk_228 = from->unk_228;
    to->unk_22c = from->unk_22c;
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

void copy_FrpgPhysIns(FrpgPhysIns* to, FrpgPhysIns* from, StateTarget target)
{
    to->vtable = from->vtable;
    to->data_0 = from->data_0;
    to->damageEntry = from->damageEntry;
    to->physWorld = from->physWorld;
}

void copy_FrpgPhysPhantomIns(FrpgPhysPhantomIns* to, FrpgPhysPhantomIns* from, StateTarget target)
{
    copy_FrpgPhysIns(&to->base, &from->base, target);

    if (from->_hkpSimpleShapePhantom == NULL)
    {
        FATALERROR("SimpleShapePhantom ptr for %p is NULL", from);
    }
    //since the phantoms (and the associated shape) are never destroyed due to the FrpgHavok rollback code, it's safe to just use the raw pointer. it should always be valid
    to->_hkpSimpleShapePhantom = from->_hkpSimpleShapePhantom;

    to->self = to;
    to->data_1 = from->data_1;
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
        copy_FrpgPhysPhantomIns(&(*to)->base, &(*from)->base, target);
        //the capsule/sphere shape is never destroyed, so the raw pointer is safe
        (*to)->_shape = (*from)->_shape;
    }
}

FrpgPhysShapePhantomIns* init_FrpgPhysShapePhantomIns(bool is_sphere)
{
    FrpgPhysShapePhantomIns* local = (FrpgPhysShapePhantomIns*)malloc_(sizeof(FrpgPhysShapePhantomIns));

    local->base._hkpSimpleShapePhantom = NULL;
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
        memcpy((*to)->unk_0, (*from)->unk_0, sizeof((*to)->unk_0));
        (*to)->PlayerHandle = (*from)->PlayerHandle;
        memcpy((*to)->unk_14, (*from)->unk_14, sizeof((*to)->unk_14));
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
