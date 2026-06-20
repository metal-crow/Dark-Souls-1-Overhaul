#include "FrpgHavokManImpStructFunctions.h"
#include "GameData.h"
#include "StateSerializer.h"
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
    // Even though in theory we don't need to save these, it crashes without them
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
 * We only save hkpSimpleShapePhantom types
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

        // Check the vtable (hkpSimpleShapePhantom::vftable)
        if ((uint64_t)phantomPtr->vtable != 0x14145ccc8) continue;

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
    FreeHkpWorldSnapshotRefs(snap);

    // --- Entities ---
    std::vector<hkpEntity*> worldEntities;
    CollectWorldEntities(world, worldEntities);

    snap->entities.reserve(worldEntities.size());
    for (hkpEntity* e : worldEntities)
    {
        //don't add if there's no associated shape
        if (e->m_collidable.base.shape != NULL)
        {
            SavedEntityState s = {};
            s.ptr = e;
            s.shapePtr = e->m_collidable.base.shape;
            memcpy(&s.motionData, &e->m_motion, sizeof(hkpMotion));
            copy_hkpShape(&s.shapeData, e->m_collidable.base.shape);
            snap->entities.push_back(s);
            // Take a ref so Havok can't free this entity if it's removed before restore
            hk_ref((void*)e);
            hk_ref(e->m_collidable.base.shape);
        }
    }

    // --- Phantoms ---
    std::vector<hkpSimpleShapePhantom*> worldPhantoms;
    CollectWorldPhantoms(world, worldPhantoms);

    snap->phantoms.reserve(worldPhantoms.size());
    for (hkpSimpleShapePhantom* p : worldPhantoms)
    {
        //don't add if there's no associated shape
        if (p->base.m_collidable.base.shape != NULL)
        {
            SavedPhantomState s = {};
            s.ptr = p;
            s.shapePtr = p->base.m_collidable.base.shape;
            memcpy(&s.motionStateData, &p->m_motionState, sizeof(hkMotionState));
            copy_hkpShape(&s.shapeData, p->base.m_collidable.base.shape);
            snap->phantoms.push_back(s);
            hk_ref((void*)p);
            hk_ref(p->base.m_collidable.base.shape);
        }
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
    if (world->m_criticalOperationsLockCount != 0 || world->m_criticalOperationsLockCountForPhantoms != 0)
    {
        FATALERROR("RestoreHkpWorldSnapshot: world is locked (entity=%d phantom=%d)",
            world->m_criticalOperationsLockCount,
            world->m_criticalOperationsLockCountForPhantoms);
    }

    // --- Collect current world state ---
    std::vector<hkpEntity*> currentEntities;
    CollectWorldEntities(world, currentEntities);

    std::vector<hkpSimpleShapePhantom*> currentPhantoms;
    CollectWorldPhantoms(world, currentPhantoms);

    // --- Remove all existing entities ---
    hk_removeEntities(world, currentEntities.data(), (int)currentEntities.size()); //this derefs each entity and it's shape

    // --- Re-add entities from the snapshot  ---
    for (auto& s : snap->entities)
    {
        memcpy(&s.ptr->m_motion, &s.motionData, sizeof(hkpMotion));

        //check if something happened to the object's shape in between saving and now
        void* old_shape = s.ptr->m_collidable.base.shape;
        if (old_shape != s.shapePtr)
        {
            ConsoleWrite("Shape mismatch! %p", old_shape);
            if (old_shape != NULL)
            {
                hk_deref(old_shape);
            }
            s.ptr->m_collidable.base.shape = s.shapePtr;
            hk_ref(s.shapePtr); //ref it for the game, since the game would ref it when it gets added
        }
        copy_hkpShape(s.shapePtr, (void*)(&s.shapeData));

        // 1 = activate. This refs the entity which we want since it's now in the world as well
        // This also rebuilds the agents/manifolds for the island
        hk_addEntity(world, s.ptr, 1);
    }

    // --- Remove all existing phantoms ---
    for (hkpSimpleShapePhantom* p : currentPhantoms)
    {
        hk_removePhantom(world, (void*)p); //this derefs the entity and it's shape
    }

    // --- Re-add phantoms from the snapshot ---
    for (auto& s : snap->phantoms)
    {
        memcpy(&s.ptr->m_motionState, &s.motionStateData, sizeof(hkMotionState));

        void* old_shape = s.ptr->base.m_collidable.base.shape;
        if (old_shape != s.shapePtr)
        {
            ConsoleWrite("Shape mismatch! %p", old_shape);
            if (old_shape != NULL)
            {
                hk_deref(old_shape);
            }
            s.ptr->base.m_collidable.base.shape = s.shapePtr;
            hk_ref(s.shapePtr);
        }
        copy_hkpShape(s.shapePtr, (void*)(&s.shapeData));

        hk_addPhantom(world, (void*)s.ptr); //This refs the entity
    }

    // NOTE: After restoring positions, the broadphase AABBs, overlap pairs,
    // and simulation islands are stale. The next normal game step will rebuild these.
    // Use hkpWorld_stepDeltaTime(0) here to force an immediate rebuild, but this seems to cause some sort of corruption/crashing.
    //hkpWorld_stepDeltaTime(world, 0.0f);
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
        new_s.shapePtr = s.shapePtr;
        memcpy(new_s.motionData, s.motionData, sizeof(hkpMotion));
        copy_hkpShape(&new_s.shapeData, (void*)(&s.shapeData));
        dst->entities.push_back(new_s);
        hk_ref((void*)s.ptr);
        hk_ref(s.shapePtr);
    }

    for (auto& s : src->phantoms)
    {
        SavedPhantomState new_s;
        new_s.ptr = s.ptr;
        new_s.shapePtr = s.shapePtr;
        memcpy(new_s.motionStateData, s.motionStateData, sizeof(hkMotionState));
        copy_hkpShape(&new_s.shapeData, (void*)(&s.shapeData));
        dst->phantoms.push_back(new_s);
        hk_ref((void*)s.ptr);
        hk_ref(s.shapePtr);
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
            hk_deref(s.shapePtr);
            hk_deref((void*)s.ptr);
        }
    }
    snap->entities.clear();

    for (auto& s : snap->phantoms)
    {
        if (s.ptr)
        {
            hk_deref(s.shapePtr);
            hk_deref((void*)s.ptr);
        }
    }
    snap->phantoms.clear();
}


void copy_hkpShape(void* to, void* from)
{
    switch (*(uint64_t*)from)
    {
    //sphere
    case 0x14141c200:
        copy_hkpSphereShape((hkpSphereShape*)to, (hkpSphereShape*)from);
        break;
    //capsule
    case 0x14141bf58:
        copy_hkpCapsuleShape((hkpCapsuleShape*)to, (hkpCapsuleShape*)from);
        break;
    default:
        //not going to bother to copy every possible shape data, most of them are not used for player-important details
        //FATALERROR("Unknown shape type: %llx %p", *(uint64_t*)from, from);
        break;
    }
}


void copy_hkpSphereShape(hkpSphereShape* to, hkpSphereShape* from)
{
    to->vtable = from->vtable;
    to->m_userData = from->m_userData;
    to->m_radius = from->m_radius;
    to->unk_24 = from->unk_24;
    memcpy(to->m_pad, from->m_pad, sizeof(to->m_pad));
    to->unk_34 = from->unk_34;
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


void copy_hkpCapsuleShape(hkpCapsuleShape* to, hkpCapsuleShape* from)
{
    (to)->vtable = (from)->vtable;
    to->m_userData = from->m_userData;
    to->m_radius = from->m_radius;
    to->unk_24 = from->unk_24;
    memcpy(to->unk_28, from->unk_28, sizeof(to->unk_28));
    memcpy(to->vertexA, from->vertexA, sizeof(to->vertexA));
    memcpy(to->vertexB, from->vertexB, sizeof(to->vertexB));
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

// ---- serializer (mirrors the HkpWorldSnapshot that copy_FrpgPhysWorld saves) -
//
// The rollback state for havok is the HkpWorldSnapshot: per-entity hkpMotion and
// per-phantom hkMotionState (memcpy'd), plus a copy of each body's shape.
// Notes on what is/isn't hashed:
//  - vtables are fixed code addresses (the collect code compares vtable to a
//    hardcoded constant), so they are identical across instances; skipped as they
//    carry no per-frame state.
//  - entity/shape pointers are heap addresses (non-deterministic across
//    instances) -> ptr_flag only.
//  - gap/padding bytes are kept as blobs to mirror copy's whole-struct memcpy; if
//    a havok divergence localizes to one of these, downgrade/drop it.
//  - CAVEAT: entities/phantoms are serialized in snapshot (world-traversal) order.
//    If that order ever differs across instances without a physics difference,
//    sort by a stable id (e.g. hkpEntity::m_uid via s.ptr) before hashing.

static void serialize_hkMotionState(StateVisitor& v, const hkMotionState* m)
{
    v.begin("hkMotionState");
    for (int i = 0; i < 4; i++) v.field("transform_rotation0", m->m_transform_rotation0[i]);
    for (int i = 0; i < 4; i++) v.field("transform_rotation1", m->m_transform_rotation1[i]);
    for (int i = 0; i < 4; i++) v.field("transform_rotation2", m->m_transform_rotation2[i]);
    for (int i = 0; i < 4; i++) v.field("transform_translation", m->m_transform_translation[i]);
    for (int i = 0; i < 4; i++) v.field("swept_centerOfMass0", m->m_sweptTransform_centerOfMass0[i]);
    for (int i = 0; i < 4; i++) v.field("swept_centerOfMass1", m->m_sweptTransform_centerOfMass1[i]);
    for (int i = 0; i < 4; i++) v.field("swept_rotation0", m->m_sweptTransform_rotation0[i]);
    for (int i = 0; i < 4; i++) v.field("swept_rotation1", m->m_sweptTransform_rotation1[i]);
    for (int i = 0; i < 4; i++) v.field("swept_centerOfMassLocal", m->m_sweptTransform_centerOfMassLocal[i]);
    for (int i = 0; i < 4; i++) v.field("m_deltaAngle", m->m_deltaAngle[i]);
    v.field("m_objectRadius", m->m_objectRadius);
    v.field("m_linearDamping", m->m_linearDamping);
    v.field("m_angularDamping", m->m_angularDamping);
    v.field("m_timeFactor", m->m_timeFactor);
    v.field("m_maxLinearVelocity", m->m_maxLinearVelocity);
    v.field("m_maxAngularVelocity", m->m_maxAngularVelocity);
    v.field("m_deactivationClass", m->m_deactivationClass);
    v.blob("padding", m->padding, sizeof(m->padding));
    v.end();
}

static void serialize_hkpMotion(StateVisitor& v, const hkpMotion* m)
{
    v.begin("hkpMotion");
    // vtable skipped (fixed code addr). memSizeAndFlags/referenceCount are frozen
    // at save time; kept for now (drop if they prove noisy cross-instance).
    v.field("m_memSizeAndFlags", m->m_memSizeAndFlags);
    v.field("m_referenceCount", m->m_referenceCount);
    v.blob("unk_c", &m->unk_c, sizeof(m->unk_c));
    v.field("m_type", m->m_type);
    v.field("m_deactivationIntegrateCounter", m->m_deactivationIntegrateCounter);
    for (int i = 0; i < 2; i++) v.field("m_deactivationNumInactiveFrames", m->m_deactivationNumInactiveFrames[i]);
    v.blob("unk_16", m->unk_16, sizeof(m->unk_16));
    serialize_hkMotionState(v, &m->m_motionState);
    for (int i = 0; i < 4; i++) v.field("m_inertiaAndMassInv", m->m_inertiaAndMassInv[i]);
    for (int i = 0; i < 4; i++) v.field("m_linearVelocity", m->m_linearVelocity[i]);
    for (int i = 0; i < 4; i++) v.field("m_angularVelocity", m->m_angularVelocity[i]);
    for (int i = 0; i < 8; i++) v.field("m_deactivationRefPosition", m->m_deactivationRefPosition[i]);
    for (int i = 0; i < 2; i++) v.field("m_deactivationRefOrientation", m->m_deactivationRefOrientation[i]);
    v.ptr_flag("m_savedMotion", m->m_savedMotion);   // heap ptr
    v.field("m_savedQualityTypeIndex", m->m_savedQualityTypeIndex);
    v.field("m_gravityFactor", m->m_gravityFactor);
    v.blob("unk_134", m->unk_134, sizeof(m->unk_134));
    v.end();
}

static void serialize_hkpSphereShape(StateVisitor& v, const hkpSphereShape* s)
{
    v.begin("hkpSphereShape");
    v.ptr_flag("m_userData", s->m_userData);
    v.field("m_radius", s->m_radius);
    v.field("unk_24", s->unk_24);
    for (int i = 0; i < 3; i++) v.field("m_pad", s->m_pad[i]);
    v.field("unk_34", s->unk_34);
    v.end();
}

static void serialize_hkpCapsuleShape(StateVisitor& v, const hkpCapsuleShape* s)
{
    v.begin("hkpCapsuleShape");
    v.ptr_flag("m_userData", s->m_userData);
    v.field("m_radius", s->m_radius);
    v.field("unk_24", s->unk_24);
    v.blob("unk_28", s->unk_28, sizeof(s->unk_28));
    for (int i = 0; i < 4; i++) v.field("vertexA", s->vertexA[i]);
    for (int i = 0; i < 4; i++) v.field("vertexB", s->vertexB[i]);
    v.end();
}

// shapeData is a union discriminated by the vtable stored at its start (same as
// copy_hkpShape). Unknown shapes are left zero by copy, so the tag reads 0.
static void serialize_hkpShapeData(StateVisitor& v, const void* shapeData)
{
    uint64_t tag = *(const uint64_t*)shapeData;
    v.field("shape_vtable_tag", tag);   // fixed code addr; identifies the shape kind
    switch (tag)
    {
    case 0x14141c200: serialize_hkpSphereShape(v, (const hkpSphereShape*)shapeData); break;
    case 0x14141bf58: serialize_hkpCapsuleShape(v, (const hkpCapsuleShape*)shapeData); break;
    default: break;   // copy_hkpShape doesn't copy other shapes; nothing to hash
    }
}

static void serialize_SavedEntityState(StateVisitor& v, const SavedEntityState* s)
{
    v.begin("SavedEntityState");
    v.ptr_flag("ptr", s->ptr);
    v.ptr_flag("shapePtr", s->shapePtr);
    serialize_hkpMotion(v, (const hkpMotion*)s->motionData);
    serialize_hkpShapeData(v, &s->shapeData);
    v.end();
}

static void serialize_SavedPhantomState(StateVisitor& v, const SavedPhantomState* s)
{
    v.begin("SavedPhantomState");
    v.ptr_flag("ptr", s->ptr);
    v.ptr_flag("shapePtr", s->shapePtr);
    serialize_hkMotionState(v, (const hkMotionState*)s->motionStateData);
    serialize_hkpShapeData(v, &s->shapeData);
    v.end();
}

void serialize_FrpgHavokManImp(StateVisitor& v, FrpgHavokManImp* h)
{
    v.begin("FrpgHavokManImp");
    HkpWorldSnapshot* snap = h->physWorld->snapshot;

    v.count("entities", snap->entities.size());
    for (SavedEntityState& s : snap->entities)
    {
        serialize_SavedEntityState(v, &s);
    }

    v.count("phantoms", snap->phantoms.size());
    for (SavedPhantomState& s : snap->phantoms)
    {
        serialize_SavedPhantomState(v, &s);
    }
    v.end();
}

std::string print_FrpgHavokManImp(FrpgHavokManImp* h)
{
    StateVisitor v(StateVisitor::Mode::Print);
    serialize_FrpgHavokManImp(v, h);
    return v.text();
}

uint64_t hash_FrpgHavokManImp(FrpgHavokManImp* h)
{
    StateVisitor v(StateVisitor::Mode::Hash);
    serialize_FrpgHavokManImp(v, h);
    return v.digest();
}
