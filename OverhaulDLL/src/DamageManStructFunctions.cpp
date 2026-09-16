#include "DamageManStructFunctions.h"
#include "PlayerInsStructFunctions.h"
#include "FrpgHavokManImpStructFunctions.h"
#include "StateSerializer.h"
#include <memory>
#include <new>
#include <unordered_map>

//the game allocates the DamageMan all_damage_entries_list to be 128 elements long, and so does every local copy
static const size_t max_preallocated_DamageEntry = 128;

//Init_DamageEntry puts the pool slot in the id's high word, or 0x80 for an entry DamageMan_PopHead_DamageEntry heap-allocated because
//the pool was empty. DamageMan_PushHead_DamageEntry reads it back the same way.
static const int32_t DamageEntry_heap_slot = 0x80;

bool DamageEntry_isDynamicAlloc(const DamageEntry* entry)
{
    return (entry->id >> 16) == (uint32_t)DamageEntry_heap_slot;
}

//Map objects and hazards put entries in the DamageMan too, which we should ignore
//TODO need to verify that this works with spells and bullets
static bool DamageEntry_is_player_owned(const DamageEntry* entry)
{
    for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        auto player_o = Game::get_connected_player(i);
        if (player_o.has_value() && player_o.value() != 0 && entry->attackerHandle == (uint32_t)((PlayerIns*)player_o.value())->chrins.handle)
        {
            return true;
        }
    }
    return false;
}

/* ============================================================
 * Heap-allocated entries
 *
 * A heap entry is saved whole. A load restores it into the live heap entry with the same id, or into a new one built the way
 * DamageMan_PopHead_DamageEntry builds one. Live heap entries the saved frame did not have are torn down the way Step_DamageMan
 * retires an entry. A rebuilt entry has its own phantoms and shapes at new addresses, so pointers to them and links to the entry
 * are remapped after the copy.
 * ============================================================ */

static void free_DamageMan_saved_heap_entries(DamageMan* d)
{
    for (SavedHeapDamageEntry& h : d->saved_heap_entries)
    {
        free_DamageEntry(h.entry, true);
    }
    d->saved_heap_entries.clear();
}

//ToLocal
static void copy_SavedHeapDamageEntry(SavedHeapDamageEntry* to, DamageEntry* from)
{
    to->entry = init_DamageEntry();
    copy_DamageEntry(to->entry, from, StateTarget::ToLocal);
    to->game_address = (uint64_t)from;
    to->game_sphere = (uint64_t)from->FrpgPhysShapePhantomIns_Sphere;
    to->game_capsule = (uint64_t)from->FrpgPhysShapePhantomIns_Capsule;
    copy_hkpShape(&to->shapes[0], from->FrpgPhysShapePhantomIns_Sphere->_hkpShape);
    copy_hkpShape(&to->shapes[1], from->FrpgPhysShapePhantomIns_Capsule->_hkpShape);
}

//Copy
static void copy_SavedHeapDamageEntry(SavedHeapDamageEntry* to, const SavedHeapDamageEntry* from)
{
    *to = *from;
    to->entry = init_DamageEntry();
    copy_DamageEntry(to->entry, from->entry, StateTarget::Copy);
}

//ToGame, into the live entry init_heap_DamageEntries picked. copy_DamageEntry writes the saved entry's game addresses, so point them at the
//phantoms and shapes of the entry it was restored into, and restore those shapes.
static void copy_SavedHeapDamageEntry(DamageEntry* to, const SavedHeapDamageEntry* from)
{
    copy_DamageEntry(to, from->entry, StateTarget::ToGame);

    FrpgPhysShapePhantomIns* sphere = to->FrpgPhysShapePhantomIns_Sphere;
    FrpgPhysShapePhantomIns* capsule = to->FrpgPhysShapePhantomIns_Capsule;
    auto own_phantom = [&](void* p) -> void*
    {
        if (p == NULL)
        {
            return NULL;
        }
        if ((uint64_t)p == from->game_sphere)
        {
            return sphere;
        }
        if ((uint64_t)p != from->game_capsule)
        {
            FATALERROR("copy_DamageMan: heap DamageEntry id=%x refers to phantom %p, which is not one of its own", to->id, p);
        }
        return capsule;
    };
    to->PhysShapePhantomIns1 = own_phantom(to->PhysShapePhantomIns1);
    to->PhysShapePhantomIns1_altPtr_A = own_phantom(to->PhysShapePhantomIns1_altPtr_A);
    to->PhysShapePhantomIns1_altPtr_B = own_phantom(to->PhysShapePhantomIns1_altPtr_B);
    to->hkpSphereShape1 = sphere->_shape;
    to->hkpCapsuleShape1 = capsule->_shape;
    //the phantoms' owner (Construct_FrpgPhysShapePhantomIns), which copy_FrpgPhysIns just set to the saved entry's address
    sphere->base.base.owner = to;
    capsule->base.base.owner = to;
    copy_hkpShape(sphere->_hkpShape, &from->shapes[0]);
    copy_hkpShape(capsule->_hkpShape, &from->shapes[1]);
}

//Step_DamageMan retires an entry with Clear_DamageEntry, DamageEntry_DestructDbgNode and DamageMan_PushHead_DamageEntry, which destructs
//and frees a heap entry. Its phantoms are taken out of the world first. Its DmgHitRecordManImp references are dropped rather than
//released: the load restores DmgHitRecordManImp after DamageMan, counts included.
static void free_heap_DamageEntry(DamageMan* damageman, DamageEntry* entry)
{
    free_DamageEntry_phantoms(entry);
    entry->PhysShapePhantomIns1 = NULL;
    entry->PhysShapePhantomIns1_altPtr_A = NULL;
    entry->PhysShapePhantomIns1_altPtr_B = NULL;
    entry->DmgHitRecordManImp_field0x10Elem = NULL;
    entry->DmgHitRecordManImp_field0x10Elem_b = NULL;
    entry->DmgHitRecordManImp_field0x10Elem_c = NULL;
    if (entry->field0x118 != NULL)
    {
        free_DamageEntryField0x118(entry->field0x118, StateTarget::ToGame);
        entry->field0x118 = NULL;
    }
    DamageEntry_DestructDbgNode(entry);
    entry->next = NULL;
    DamageMan_PushHead_DamageEntry(damageman, entry);
}

//Allocated from the heap DamageMan_PopHead_DamageEntry uses. Its phantoms are not in the world.
static DamageEntry* init_heap_DamageEntry()
{
    DamageEntry* entry = (DamageEntry*)Game::game_malloc(sizeof(DamageEntry), 0x10, *(uint64_t*)Game::internal_heap_3);
    Init_DamageEntry(entry, DamageEntry_heap_slot);
    entry->next = NULL;
    return entry;
}

//The live entry each saved heap entry is restored into: the live heap entry with its id, or a new one. Live heap entries left over are
//torn down.
static std::vector<DamageEntry*> init_heap_DamageEntries(DamageMan* game, const DamageMan* saved)
{
    std::vector<DamageEntry*> live;
    for (DamageEntry* e = game->active_damage_entries_list; e != NULL; e = e->next)
    {
        if (DamageEntry_isDynamicAlloc(e))
        {
            live.push_back(e);
        }
    }

    std::vector<DamageEntry*> targets(saved->saved_heap_entries.size(), NULL);
    for (size_t i = 0; i < targets.size(); i++)
    {
        for (DamageEntry*& e : live)
        {
            if (e != NULL && e->id == saved->saved_heap_entries[i].entry->id)
            {
                targets[i] = e;
                e = NULL;
                break;
            }
        }
    }
    for (DamageEntry* e : live)
    {
        if (e != NULL)
        {
            free_heap_DamageEntry(game, e);
        }
    }
    for (DamageEntry*& t : targets)
    {
        if (t == NULL)
        {
            t = init_heap_DamageEntry();
        }
    }
    return targets;
}

//Followup links that name a heap entry now restored at another address. Pool entries keep theirs.
static void copy_heap_DamageEntry_links(DamageMan* to, const DamageMan* from, const std::vector<DamageEntry*>& targets)
{
    std::unordered_map<uint64_t, DamageEntry*> moved;
    for (size_t i = 0; i < targets.size(); i++)
    {
        moved[from->saved_heap_entries[i].game_address] = targets[i];
    }
    if (moved.empty())
    {
        return;
    }
    auto remap = [&moved](DamageEntry* e)
    {
        for (DamageEntry** link : { &e->followup_a, &e->followup_b, &e->followup_c })
        {
            if (*link == NULL)
            {
                continue;
            }
            auto it = moved.find((uint64_t)*link);
            if (it != moved.end())
            {
                *link = it->second;
            }
        }
    };
    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        remap(&to->all_damage_entries_list_start[i]);
    }
    for (DamageEntry* t : targets)
    {
        remap(t);
    }
}

/* ============================================================ */

void copy_DamageMan(DamageMan* to, DamageMan* from, StateTarget target)
{
    Game::SuspendThreads();

    //ToGame: the live entry each saved heap entry is restored into
    std::vector<DamageEntry*> heap_targets;

    if (target == StateTarget::ToLocal)
    {
        to->saved_active_damage_entries.clear();
        free_DamageMan_saved_heap_entries(to);
        for (DamageEntry* head = from->active_damage_entries_list; head != NULL; head = head->next)
        {
            SavedDamageEntry e;
            e.pool_index = -1;
            e.heap_index = -1;
            memset(e.shapes, 0, sizeof(e.shapes));
            e.player_owned = DamageEntry_is_player_owned(head);
            const ptrdiff_t idx = head - from->all_damage_entries_list_start;
            if (DamageEntry_isDynamicAlloc(head))
            {
                to->saved_heap_entries.emplace_back();
                copy_SavedHeapDamageEntry(&to->saved_heap_entries.back(), head);
                e.heap_index = (int32_t)to->saved_heap_entries.size() - 1;
            }
            else if (idx >= 0 && idx < (ptrdiff_t)max_preallocated_DamageEntry)
            {
                e.pool_index = (int32_t)idx;
                //the phantoms and shapes belong to the entry, so its shapes are saved with it
                copy_hkpShape(&e.shapes[0], head->FrpgPhysShapePhantomIns_Sphere->_hkpShape);
                copy_hkpShape(&e.shapes[1], head->FrpgPhysShapePhantomIns_Capsule->_hkpShape);
            }
            else
            {
                FATALERROR("copy_DamageMan: active DamageEntry %p id=%x is neither a pool slot nor a heap entry", head, head->id);
            }
            to->saved_active_damage_entries.push_back(e);
        }
    }
    else if (target == StateTarget::ToGame)
    {
        heap_targets = init_heap_DamageEntries(to, from);
    }
    else if (target == StateTarget::Copy)
    {
        to->saved_active_damage_entries = from->saved_active_damage_entries;
        free_DamageMan_saved_heap_entries(to);
        to->saved_heap_entries.resize(from->saved_heap_entries.size());
        for (size_t i = 0; i < from->saved_heap_entries.size(); i++)
        {
            copy_SavedHeapDamageEntry(&to->saved_heap_entries[i], &from->saved_heap_entries[i]);
        }
    }

    to->all_damage_entries_list_cur = from->all_damage_entries_list_cur;

    //go through the all_damage_entries_list. This is all static allocations
    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        DamageEntry* from_DamageEntry = &from->all_damage_entries_list_start[i];
        DamageEntry* to_DamageEntry = &to->all_damage_entries_list_start[i];

        copy_DamageEntry(to_DamageEntry, from_DamageEntry, target);
    }

    if (target == StateTarget::ToGame)
    {
        for (size_t i = 0; i < heap_targets.size(); i++)
        {
            copy_SavedHeapDamageEntry(heap_targets[i], &from->saved_heap_entries[i]);
        }
        copy_heap_DamageEntry_links(to, from, heap_targets);

        //a pool entry keeps its phantoms and shapes, so only their contents are restored (a heap entry's came with it)
        for (const SavedDamageEntry& e : from->saved_active_damage_entries)
        {
            if (e.pool_index < 0)
            {
                continue;
            }
            DamageEntry* entry = &to->all_damage_entries_list_start[e.pool_index];
            copy_hkpShape(entry->FrpgPhysShapePhantomIns_Sphere->_hkpShape, &e.shapes[0]);
            copy_hkpShape(entry->FrpgPhysShapePhantomIns_Capsule->_hkpShape, &e.shapes[1]);
        }

        //relink the active list in its saved order
        DamageEntry** link = &to->active_damage_entries_list;
        for (const SavedDamageEntry& e : from->saved_active_damage_entries)
        {
            DamageEntry* entry = e.pool_index >= 0 ? &to->all_damage_entries_list_start[e.pool_index] : heap_targets[e.heap_index];
            *link = entry;
            link = &entry->next;
        }
        *link = NULL;
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
    //malloc_ hands back raw memory, so the local-only containers are constructed in place
    new (&local_DamageMan->saved_active_damage_entries) std::vector<SavedDamageEntry>();
    new (&local_DamageMan->saved_heap_entries) std::vector<SavedHeapDamageEntry>();

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
    free_DamageMan_saved_heap_entries(to);
    //the struct is malloc'd, so its containers are destroyed by hand
    std::destroy_at(&to->saved_active_damage_entries);
    std::destroy_at(&to->saved_heap_entries);
    for (size_t i = 0; i < max_preallocated_DamageEntry; i++)
    {
        free_DamageEntry(&to->all_damage_entries_list_start[i], false);
    }
    free(to->all_damage_entries_list_start);

    free(to);
}

void copy_DamageEntry(DamageEntry* to, DamageEntry* from, StateTarget target)
{
    to->id = from->id;
    to->unk_4 = from->unk_4;

    if (from->FrpgPhysShapePhantomIns_Sphere == NULL || from->FrpgPhysShapePhantomIns_Capsule == NULL)
    {
        FATALERROR("FrpgPhysShapePhantomIns can be null??? from=%p sphere=%p cap=%p", from, from->FrpgPhysShapePhantomIns_Sphere, from->FrpgPhysShapePhantomIns_Capsule);
    }
    copy_FrpgPhysShapePhantomIns(&to->FrpgPhysShapePhantomIns_Sphere, &from->FrpgPhysShapePhantomIns_Sphere, target);
    copy_FrpgPhysShapePhantomIns(&to->FrpgPhysShapePhantomIns_Capsule, &from->FrpgPhysShapePhantomIns_Capsule, target);

    //these all point at the entry's own phantoms and shapes (copy_DamageMan remaps them for a rebuilt heap entry)
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
    to->DmgHitRecordManImp_field0x10Elem_b = from->DmgHitRecordManImp_field0x10Elem_b;
    to->DmgHitRecordManImp_field0x10Elem_c = from->DmgHitRecordManImp_field0x10Elem_c;
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
    to->type = from->type;
    to->unk_a = from->unk_a;
    to->unk_c = from->unk_c;
    to->owner = from->owner;
    to->physWorld = from->physWorld;
}

void copy_FrpgPhysPhantomIns(FrpgPhysPhantomIns* to, FrpgPhysPhantomIns* from, StateTarget target)
{
    copy_FrpgPhysIns(&to->base, &from->base, target);

    if (from->_hkpSimpleShapePhantom == NULL)
    {
        FATALERROR("SimpleShapePhantom ptr for %p is NULL", from);
    }
    //A phantom is created with its DamageEntry and lives as long as the entry, so a load leaves the live pointer alone.
    //The snapshot keeps it only for the serializer's null check.
    if (target != StateTarget::ToGame)
    {
        to->_hkpSimpleShapePhantom = from->_hkpSimpleShapePhantom;
    }

    to->self = to;
    to->data_1 = from->data_1;
}

void copy_FrpgPhysShapePhantomIns(FrpgPhysShapePhantomIns** to, FrpgPhysShapePhantomIns** from, StateTarget target)
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
        //likewise the shape; its radius and vertices are restored by copy_FrpgHavokManImp, or for a heap entry by copy_DamageMan
        if (target != StateTarget::ToGame)
        {
            (*to)->_shape = (*from)->_shape;
        }
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
    v.field("type", p->type);
    v.field("unk_a", p->unk_a);
    v.field("unk_c", p->unk_c);
    v.ptr_flag("owner", p->owner);
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
    v.ptr_flag("DmgHitRecordManImp_field0x10Elem_b", e->DmgHitRecordManImp_field0x10Elem_b);
    v.ptr_flag("DmgHitRecordManImp_field0x10Elem_c", e->DmgHitRecordManImp_field0x10Elem_c);
    v.ptr_flag("followup_a", e->followup_a);
    v.ptr_flag("followup_b", e->followup_b);
    v.ptr_flag("followup_c", e->followup_c);
    v.field("num_hits", e->num_hits);
    v.field("unk_214", e->unk_214);
    v.blob("unk_218", e->unk_218, sizeof(e->unk_218));
    // the active list's link. Its order is already the order of saved_active_damage_entries, and whether it is set
    // also depends on world-owned entries after this one, which are not compared
    v.excluded("next", sizeof(e->next));
    v.field("unk_228", e->unk_228);
    v.field("unk_22c", e->unk_22c);
    v.end();
}

// An active damage entry, identified by its pool slot, or as a heap entry. A pool entry's contents are hashed from the snapshot's own
// copy of all_damage_entries_list_start, from the slot it occupies; a heap entry's from its saved copy.
static void serialize_SavedDamageEntry(StateVisitor& v, const SavedDamageEntry* e, const DamageMan* d)
{
    v.begin("SavedDamageEntry");
    v.field("pool_index", e->pool_index);
    const SavedHavokShape* shapes = e->shapes;
    if (e->pool_index >= 0 && e->pool_index < (int32_t)max_preallocated_DamageEntry && d->all_damage_entries_list_start != NULL)
    {
        serialize_DamageEntry(v, &d->all_damage_entries_list_start[e->pool_index]);
    }
    else if (e->heap_index >= 0 && (size_t)e->heap_index < d->saved_heap_entries.size())
    {
        serialize_DamageEntry(v, d->saved_heap_entries[e->heap_index].entry);
        shapes = d->saved_heap_entries[e->heap_index].shapes;
    }
    serialize_SavedHavokShape(v, "sphere_shape", &shapes[0]);
    serialize_SavedHavokShape(v, "capsule_shape", &shapes[1]);
    v.end();
}

void serialize_DamageMan(StateVisitor& v, DamageMan* d)
{
    v.begin("DamageMan");

    // only the ACTIVE, player-owned damage entries are hashed
    size_t player_owned = 0;
    for (const SavedDamageEntry& e : d->saved_active_damage_entries)
    {
        if (e.player_owned) player_owned++;
    }
    v.count("saved_active_damage_entries", player_owned);
    for (const SavedDamageEntry& e : d->saved_active_damage_entries)
    {
        if (e.player_owned)
        {
            serialize_SavedDamageEntry(v, &e, d);
        }
    }
    v.note("world_owned_entries_not_compared", std::to_string(d->saved_active_damage_entries.size() - player_owned));

    // all_damage_entries_list_cur is a GAME pool address in the snapshot; can't be
    // indexed against the snapshot's own pool -> null/non-null only. A real cursor
    // divergence also shows up as differing pool contents below.
    v.ptr_flag("all_damage_entries_list_cur", d->all_damage_entries_list_cur);

    // The pool itself is NOT walked: an entry that is not in the active list is a
    // free slot holding uninitialised memory.

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
