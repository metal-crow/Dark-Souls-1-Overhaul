#include "FrpgHavokManImpStructFunctions.h"
#include "GameData.h"
#include <unordered_set>

void copy_FrpgHavokManImp(FrpgHavokManImp* to, const FrpgHavokManImp* from, StateTarget target)
{
    copy_FrpgPhysWorld(to->physWorld, from->physWorld, target);
}

//all the Havok Man inits are only for dll memory, game side they always exist
FrpgHavokManImp* init_FrpgHavokManImp()
{
    FrpgHavokManImp* local = (FrpgHavokManImp*)malloc_(sizeof(FrpgHavokManImp));

    local->physWorld = init_FrpgPhysWorld();

    return local;
}

void free_FrpgHavokManImp(FrpgHavokManImp* to)
{
    free_FrpgPhysWorld(to->physWorld);
    free(to);
}

void copy_FrpgPhysWorld(FrpgPhysWorld* to, const FrpgPhysWorld* from, StateTarget target)
{
    if (target == StateTarget::ToLocal)
    {
        // from is game-side, to is our local snapshot
        hkpWorld* world = from->_hkpWorld;
        to->_hkpWorld = world; // store reference to game world (used by DamageMan)
        SaveHkpWorldSnapshot(to->snapshot, world);
    }
    else if (target == StateTarget::ToGame)
    {
        // from is our local snapshot, to is game-side
        hkpWorld* world = to->_hkpWorld;
        RestoreHkpWorldSnapshot(from->snapshot, world);
    }
    else // StateTarget::Copy
    {
        // Copy between two local snapshots
        to->_hkpWorld = from->_hkpWorld;
        CopyHkpWorldSnapshot(to->snapshot, from->snapshot);
    }
}

FrpgPhysWorld* init_FrpgPhysWorld()
{
    FrpgPhysWorld* local = (FrpgPhysWorld*)malloc_(sizeof(FrpgPhysWorld));
    local->snapshot = new HkpWorldSnapshot();
    return local;
}

void free_FrpgPhysWorld(FrpgPhysWorld* to)
{
    if (to->snapshot)
    {
        FreeHkpWorldSnapshotRefs(to->snapshot);
        delete to->snapshot;
    }
    free(to);
}

/* ============================================================
 * Helper: check if a phantom is one of the 6 broadphase border phantoms
 * ============================================================ */
static bool IsBorderPhantom(hkpWorld* world, void* phantomPtr)
{
    hkpBroadPhaseBorder* border = world->m_broadPhaseBorder;
    if (!border) return false;

    return phantomPtr == border->phantom1
        || phantomPtr == border->phantom2
        || phantomPtr == border->phantom3
        || phantomPtr == border->phantom4
        || phantomPtr == border->phantom5
        || phantomPtr == border->phantom6;
}

/* ============================================================
 * Helper: collect all entities from the world's simulation islands
 * ============================================================ */
static void CollectWorldEntities(hkpWorld* world, std::vector<hkpEntity*>& out)
{
    out.clear();

    // Fixed island (static/keyframed bodies)
    if (world->m_fixedIsland)
    {
        hkpSimulationIsland* island = world->m_fixedIsland;
        for (uint32_t j = 0; j < island->m_entities_size; j++)
        {
            out.push_back(island->m_entities[j]);
        }
    }

    // Active simulation islands
    for (uint32_t i = 0; i < world->m_activeSimulationIslands_size; i++)
    {
        hkpSimulationIsland* island = world->m_activeSimulationIslands[i];
        for (uint32_t j = 0; j < island->m_entities_size; j++)
        {
            out.push_back(island->m_entities[j]);
        }
    }

    // Inactive simulation islands
    for (uint32_t i = 0; i < world->m_inactiveSimulationIslands_size; i++)
    {
        hkpSimulationIsland* island = world->m_inactiveSimulationIslands[i];
        for (uint32_t j = 0; j < island->m_entities_size; j++)
        {
            out.push_back(island->m_entities[j]);
        }
    }
}

/* ============================================================
 * Helper: collect all non-border phantoms from the world
 * We only save hkpSimpleShapePhantom types (broadPhaseHandle.m_type != 3)
 * ============================================================ */
static void CollectWorldPhantoms(hkpWorld* world, std::vector<hkpSimpleShapePhantom*>& out)
{
    out.clear();

    for (uint32_t i = 0; i < world->m_phantoms_size; i++)
    {
        hkpPhantom* phantomPtr = world->m_phantoms[i];
        if (!phantomPtr) continue;

        // Skip broadphase border phantoms
        if (IsBorderPhantom(world, (void*)phantomPtr)) continue;

        // Check the broadPhaseHandle type: type 3 = border/aabb infrastructure phantom
        uint8_t bpType = phantomPtr->m_collidable.base.m_broadPhaseHandle.m_type;
        if (bpType == 3) continue; // skip AABB/border phantoms

        out.push_back((hkpSimpleShapePhantom*)phantomPtr);
    }
}

/* ============================================================
 * SaveHkpWorldSnapshot
 *
 * Scans the world for all entities/phantoms, memcpy's their
 * mutable motion state, and takes a ref on each to prevent
 * Havok from freeing them if they're removed before restore.
 * ============================================================ */
void SaveHkpWorldSnapshot(HkpWorldSnapshot* snap, hkpWorld* world)
{
    snap->entities.clear();
    snap->phantoms.clear();

    // --- Entities ---
    std::vector<hkpEntity*> worldEntities;
    CollectWorldEntities(world, worldEntities);

    snap->entities.reserve(worldEntities.size());
    for (hkpEntity* e : worldEntities)
    {
        SavedEntityState s;
        s.ptr = e;
        memcpy(s.motionData, &e->m_motion, sizeof(hkpMotion));
        snap->entities.push_back(s);
        // Take a ref so Havok can't free this entity if it's removed before restore
        hk_ref((void*)e);
    }

    // --- Phantoms ---
    std::vector<hkpSimpleShapePhantom*> worldPhantoms;
    CollectWorldPhantoms(world, worldPhantoms);

    snap->phantoms.reserve(worldPhantoms.size());
    for (hkpSimpleShapePhantom* p : worldPhantoms)
    {
        SavedPhantomState s;
        s.ptr = p;
        memcpy(s.motionStateData, &p->m_motionState, sizeof(hkMotionState));
        snap->phantoms.push_back(s);

        hk_ref((void*)p);
    }
}

/* ============================================================
 * RestoreHkpWorldSnapshot
 *
 * Makes the current world match the snapshot:
 * 1. Remove entities/phantoms not in the snapshot
 * 2. Re-add entities/phantoms that are in the snapshot but missing from the world
 * 3. memcpy motion states back
 * ============================================================ */
void RestoreHkpWorldSnapshot(const HkpWorldSnapshot* snap, hkpWorld* world)
{
    // --- Build target sets for O(1) lookup when we remove entities ---
    std::unordered_set<hkpEntity*> targetEntities;
    for (auto& s : snap->entities)
    {
        targetEntities.insert(s.ptr);
    }
    std::unordered_set<hkpSimpleShapePhantom*> targetPhantoms;
    for (auto& s : snap->phantoms)
    {
        targetPhantoms.insert(s.ptr);
    }

    // --- Collect current world state ---
    std::vector<hkpEntity*> currentEntities;
    CollectWorldEntities(world, currentEntities);

    std::vector<hkpSimpleShapePhantom*> currentPhantoms;
    CollectWorldPhantoms(world, currentPhantoms);

    std::unordered_set<hkpEntity*> currentEntitySet(currentEntities.begin(), currentEntities.end());
    std::unordered_set<hkpSimpleShapePhantom*> currentPhantomSet(currentPhantoms.begin(), currentPhantoms.end());

    // --- Remove entities that shouldn't exist at this frame ---
    for (hkpEntity* e : currentEntities)
    {
        if (targetEntities.find(e) == targetEntities.end())
        {
            hk_removeEntities(world, &e, 1); //this derefs the entity
        }
    }

    // --- Re-add entities from the snapshot that are not currently in the world ---
    for (auto& s : snap->entities)
    {
        if (currentEntitySet.find(s.ptr) == currentEntitySet.end())
        {
            hk_addEntity(world, s.ptr, 1); // 1 = activate. This refs the entity which we want since it's now in the world as well
        }
    }

    // --- Remove phantoms that shouldn't exist ---
    for (hkpSimpleShapePhantom* p : currentPhantoms)
    {
        if (targetPhantoms.find(p) == targetPhantoms.end())
        {
            hk_removePhantom(world, (void*)p); //this derefs the entity
        }
    }

    // --- Re-add phantoms from the snapshot ---
    for (auto& s : snap->phantoms)
    {
        if (currentPhantomSet.find(s.ptr) == currentPhantomSet.end())
        {
            hk_addPhantom(world, (void*)s.ptr); //This refs the entity
        }
    }

    // --- Restore motion states via memcpy ---
    for (auto& s : snap->entities)
    {
        memcpy(&s.ptr->m_motion, s.motionData, sizeof(hkpMotion));
    }
    for (auto& s : snap->phantoms)
    {
        memcpy(&s.ptr->m_motionState, s.motionStateData, sizeof(hkMotionState));
    }

    // NOTE: After restoring positions, the broadphase AABBs, overlap pairs,
    // and simulation islands are stale. The next normal game step will rebuild these.
    // I attemped to use hkpWorld_stepDeltaTime(0) here to force an immediate rebuild but that causes crashing
}

/* ============================================================
 * CopyHkpWorldSnapshot
 *
 * Deep-copies snapshot data and refs all entities/phantoms
 * in the new copy (since it now also holds pointers to them).
 * ============================================================ */
void CopyHkpWorldSnapshot(HkpWorldSnapshot* dst, const HkpWorldSnapshot* src)
{
    // Clean the dst vectors
    FreeHkpWorldSnapshotRefs(dst);

    // Deep copy the vectors, and Ref everything in the new copy
    for (auto& s : src->entities)
    {
        SavedEntityState new_s;
        new_s.ptr = s.ptr;
        memcpy(&new_s.motionData, &s.motionData, sizeof(hkpMotion));
        dst->entities.push_back(new_s);
        hk_ref((void*)s.ptr);
    }

    for (auto& s : src->phantoms)
    {
        SavedPhantomState new_s;
        new_s.ptr = s.ptr;
        memcpy(&new_s.motionStateData, &s.motionStateData, sizeof(hkMotionState));
        dst->phantoms.push_back(new_s);
        hk_ref((void*)s.ptr);
    }
}

/* ============================================================
 * FreeHkpWorldSnapshotRefs
 *
 * Releases the refs we took when saving/copying.
 * If an entity was removed from the world and this was the last
 * ref, Havok will free it.
 * ============================================================ */
void FreeHkpWorldSnapshotRefs(HkpWorldSnapshot* snap)
{
    for (auto& s : snap->entities)
    {
        if (s.ptr)
        {
            hk_deref((void*)s.ptr);
        }
    }
    snap->entities.clear();

    for (auto& s : snap->phantoms)
    {
        if (s.ptr)
        {
            hk_deref((void*)s.ptr);
        }
    }
    snap->phantoms.clear();
}

/* ============================================================
 * The following are used for DamageMan entries.
 * ============================================================ */
void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape* from, StateTarget target)
{
    (*to)->vtable = (from)->vtable;
    memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
    memcpy((*to)->data_1, (from)->data_1, sizeof((*to)->data_1));
    if ((from)->m_userData != 0)
    {
        FATALERROR("hkpSphereShape->m_userData is non-0, value %x", (from)->m_userData);
    }
}

hkpSphereShape* init_hkpSphereShape(StateTarget target)
{
    hkpSphereShape* local;
    //normally this would be a thread malloc'd object game-side but we've disabled that
    local = (hkpSphereShape*)malloc_(sizeof(hkpSphereShape));
    local->vtable = 0x14141c200;
    local->m_userData = NULL;
    return local;
}

void free_hkpSphereShape(hkpSphereShape* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        hk_deref((void*)to);
    }
    else
    {
        free(to);
    }
}


void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape* from, StateTarget target)
{
    (*to)->vtable = (from)->vtable;
    memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
    memcpy((*to)->data_1, (from)->data_1, sizeof((*to)->data_1));
    if ((from)->m_userData != 0)
    {
        FATALERROR("hkpCapsuleShape->m_userData is non-0, value %x", (from)->m_userData);
    }
}

hkpCapsuleShape* init_hkpCapsuleShape(StateTarget target)
{
    hkpCapsuleShape* local;
    //normally this would be a thread malloc'd object game-side but we've disabled that
    local = (hkpCapsuleShape*)malloc_(sizeof(hkpCapsuleShape));
    local->vtable = 0x14141bf58;
    local->m_userData = NULL;
    return local;
}

void free_hkpCapsuleShape(hkpCapsuleShape* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        hk_deref((void*)to);
    }
    else
    {
        free(to);
    }
}
