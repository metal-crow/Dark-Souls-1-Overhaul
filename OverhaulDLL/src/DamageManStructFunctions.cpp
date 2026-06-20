#include "DamageManStructFunctions.h"
#include "PlayerInsStructFunctions.h"
#include "FrpgHavokManImpStructFunctions.h"
#include "StateSerializer.h"

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

// ---- serializer (mirrors copy_DamageMan / copy_DamageEntry) ------------------
//
// Pointer handling: in the snapshot, the DamageEntry link/followup pointers and
// the shape/phantom pointers hold GAME addresses (copy_DamageEntry copies the raw
// game value), which are non-deterministic across instances and cannot be indexed
// against the snapshot's own pool -> ptr_flag (null/non-null). The per-entry data
// (id, attackInfo, life, magnifications, etc.) carries the signal. vtables/data_x
// "non-pointer" words are at fixed addresses / are plain data -> hashed directly.

static void serialize_FrpgPhysIns(StateVisitor& v, const FrpgPhysIns* p)
{
    v.begin("FrpgPhysIns");
    v.field("vtable", p->vtable);          // fixed code addr (deterministic)
    v.field("data_0", p->data_0);          // non-pointer data
    v.ptr_flag("damageEntry", (void*)p->damageEntry);
    v.ptr_flag("physWorld", p->physWorld);
    v.end();
}

static void serialize_FrpgPhysPhantomIns(StateVisitor& v, const FrpgPhysPhantomIns* p)
{
    v.begin("FrpgPhysPhantomIns");
    serialize_FrpgPhysIns(v, &p->base);
    v.ptr_flag("_hkpSimpleShapePhantom", p->_hkpSimpleShapePhantom);
    v.ptr_flag("self", p->self);
    v.field("data_1", p->data_1);
    v.end();
}

static void serialize_FrpgPhysShapePhantomIns(StateVisitor& v, const FrpgPhysShapePhantomIns* p)
{
    v.begin("FrpgPhysShapePhantomIns");
    serialize_FrpgPhysPhantomIns(v, &p->base);
    v.ptr_flag("_shape", p->_shape);
    v.end();
}

static void serialize_DamageEntryField0x118(StateVisitor& v, const DamageEntryField0x118* f)
{
    v.begin("DamageEntryField0x118");
    for (int i = 0; i < 4; i++) v.field("unk_0", f->unk_0[i]);
    v.field("PlayerHandle", f->PlayerHandle);
    v.blob("unk_14", f->unk_14, sizeof(f->unk_14));
    v.end();
}

static void serialize_DamageEntry(StateVisitor& v, const DamageEntry* e)
{
    v.begin("DamageEntry");
    v.field("id", e->id);
    v.field("unk_4", e->unk_4);
    serialize_FrpgPhysShapePhantomIns(v, e->FrpgPhysShapePhantomIns_Sphere);
    serialize_FrpgPhysShapePhantomIns(v, e->FrpgPhysShapePhantomIns_Capsule);
    v.ptr_flag("PhysShapePhantomIns1", e->PhysShapePhantomIns1);
    v.ptr_flag("hkpSphereShape1", e->hkpSphereShape1);
    v.ptr_flag("hkpCapsuleShape1", e->hkpCapsuleShape1);
    v.ptr_flag("PhysShapePhantomIns1_altPtr_A", e->PhysShapePhantomIns1_altPtr_A);
    v.ptr_flag("PhysShapePhantomIns1_altPtr_B", e->PhysShapePhantomIns1_altPtr_B);
    v.field("attackerHandle", e->attackerHandle);
    v.blob("attackInfo", e->attackInfo, sizeof(e->attackInfo));
    v.field("unk_114", e->unk_114);
    if (e->field0x118)
    {
        v.field("field0x118_present", true);
        serialize_DamageEntryField0x118(v, e->field0x118);
    }
    else
    {
        v.field("field0x118_present", false);
    }
    v.field("life", e->life);
    v.field("unk_124", e->unk_124);
    v.blob("unk_128", e->unk_128, sizeof(e->unk_128));
    v.blob("field0x130", e->field0x130, sizeof(e->field0x130));
    v.field("unk_170", e->unk_170);
    v.field("unk_174", e->unk_174);
    v.field("unk_178", e->unk_178);
    v.field("unk_17c", e->unk_17c);
    v.field("unk_180", e->unk_180);
    v.field("unk_184", e->unk_184);
    v.field("unk_188", e->unk_188);
    v.field("unk_18c", e->unk_18c);
    v.field("unk_190", e->unk_190);
    v.field("unk_194", e->unk_194);
    v.field("unk_196", e->unk_196);
    v.field("unk_198", e->unk_198);
    v.field("unk_19c", e->unk_19c);
    v.field("unk_1a0", e->unk_1a0);
    v.field("unk_1a4", e->unk_1a4);
    v.field("unk_1a8", e->unk_1a8);
    v.field("unk_1ac", e->unk_1ac);
    v.field("unk_1b0", e->unk_1b0);
    v.field("isSweetSpot", e->isSweetSpot);
    v.field("isWeakSpot", e->isWeakSpot);
    v.field("unk_1b6", e->unk_1b6);
    v.field("unk_1b8", e->unk_1b8);
    v.field("physical_magnification", e->physical_magnification);
    v.field("magic_magnification", e->magic_magnification);
    v.field("fire_magnification", e->fire_magnification);
    v.field("lightning_magnification", e->lightning_magnification);
    v.field("stamina_dmg_magnification", e->stamina_dmg_magnification);
    v.field("knockback_percent", e->knockback_percent);
    v.field("unk_1d4", e->unk_1d4);
    v.ptr_flag("DmgHitRecordManImp_field0x10Elem", e->DmgHitRecordManImp_field0x10Elem);
    v.ptr_flag("physWorld", e->physWorld);
    v.ptr_flag("followup_a", e->followup_a);
    v.ptr_flag("followup_b", e->followup_b);
    v.ptr_flag("followup_c", e->followup_c);
    v.field("num_hits", e->num_hits);
    v.field("unk_214", e->unk_214);
    v.blob("unk_218", e->unk_218, sizeof(e->unk_218));
    v.ptr_flag("next", e->next);
    v.field("unk_228", e->unk_228);
    v.field("unk_22c", e->unk_22c);
    v.end();
}

static void serialize_SavedDamageEntry(StateVisitor& v, const SavedDamageEntry* e)
{
    v.begin("SavedDamageEntry");
    v.ptr_flag("game_addr", e->game_addr);   // game pool/heap address
    v.field("is_dynamic", e->is_dynamic);
    if (e->is_dynamic && e->data)
    {
        serialize_DamageEntry(v, e->data);
    }
    v.end();
}

void serialize_DamageMan(StateVisitor& v, DamageMan* d)
{
    v.begin("DamageMan");

    // active list as captured at save time (the linked list walked via ->next)
    v.count("saved_active_damage_entries", d->saved_active_damage_entries.size());
    for (SavedDamageEntry& e : d->saved_active_damage_entries)
    {
        serialize_SavedDamageEntry(v, &e);
    }

    // all_damage_entries_list_cur is a GAME pool address in the snapshot; can't be
    // indexed against the snapshot's own pool -> null/non-null only. A real cursor
    // divergence also shows up as differing pool contents below.
    v.ptr_flag("all_damage_entries_list_cur", d->all_damage_entries_list_cur);

    v.count("all_damage_entries_list", max_preallocated_DamageEntry);
    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        serialize_DamageEntry(v, &d->all_damage_entries_list_start[i]);
    }

    v.field("unk_18", d->unk_18);
    v.field("put_out_sparks", d->put_out_sparks);
    v.field("damage_from_weapon", d->damage_from_weapon);
    v.field("damage_to_occur", d->damage_to_occur);
    v.blob("unk_34", d->unk_34, sizeof(d->unk_34));

    v.end();
}

std::string print_DamageMan(DamageMan* d)
{
    StateVisitor v(StateVisitor::Mode::Print);
    serialize_DamageMan(v, d);
    return v.text();
}

uint64_t hash_DamageMan(DamageMan* d)
{
    StateVisitor v(StateVisitor::Mode::Hash);
    serialize_DamageMan(v, d);
    return v.digest();
}
