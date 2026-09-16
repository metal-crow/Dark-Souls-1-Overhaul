#include "FrpgHavokManImpStructFunctions.h"
#include "DamageManStructFunctions.h"
#include "GameData.h"
#include "StateSerializer.h"
#include <cstddef>
#include <vector>

// The only shapes whose contents rollback saves
static const uint64_t hkpSphereShape_vtable = 0x14141c200;
static const uint64_t hkpCapsuleShape_vtable = 0x14141bf58;

// hkpEntityActivation::HK_ENTITY_ACTIVATION_DO_ACTIVATE, what FrpgPhysIns passes to hkpRigidBody::setMotionType (FUN_1402a9670)
static const uint32_t hkpEntityActivation_DO_ACTIVATE = 1;

/* ============================================================
 * Helpers
 * ============================================================ */

static hkpWorld* get_hkpWorld()
{
    FrpgHavokManImp* havokman = *(FrpgHavokManImp**)Game::frpg_havok_man_imp;
    if (havokman == NULL || havokman->physWorld == NULL || havokman->physWorld->_hkpWorld == NULL)
    {
        FATALERROR("havok rollback: the game has no hkpWorld");
    }
    return havokman->physWorld->_hkpWorld;
}

// Add, remove and broadphase updates on a locked world are queued instead of run, pointing at our stack
static void check_hkpWorld_unlocked(const hkpWorld* world, const char* what)
{
    if (world->m_criticalOperationsLockCount != 0 || world->m_criticalOperationsLockCountForPhantoms != 0)
    {
        FATALERROR("%s: world is locked (entity=%d phantom=%d)", what,
            world->m_criticalOperationsLockCount, world->m_criticalOperationsLockCountForPhantoms);
    }
}

static PlayerIns* connected_player(uint32_t i)
{
    auto player_o = Game::get_connected_player(i);
    if (!player_o.has_value() || player_o.value() == 0)
    {
        FATALERROR("havok rollback: unable to get playerins %d", i);
    }
    return (PlayerIns*)player_o.value();
}

static ChrCtrl* chr_ctrl(PlayerIns* player)
{
    if (player->chrins.playerCtrl == NULL)
    {
        return NULL;
    }
    return &player->chrins.playerCtrl->chrCtrl;
}

static hkpSimpleShapePhantom* phantom_of(const FrpgPhysShapePhantomIns* wrapper)
{
    return wrapper != NULL ? wrapper->base._hkpSimpleShapePhantom : NULL;
}

// A load logs what it adjusted, but only the first few times
static bool log_budget(uint32_t& counter)
{
    if (counter >= 20)
    {
        return false;
    }
    counter++;
    return true;
}

/* ============================================================
 * Havok values, saved by their owner
 * ============================================================ */

void copy_hkpShape(SavedHavokShape* to, const hkpShape* from)
{
    memset(to, 0, sizeof(SavedHavokShape));
    if (from == NULL)
    {
        return;
    }
    const uint64_t vtable = (uint64_t)from->vtable;
    if (vtable == hkpSphereShape_vtable)
    {
        to->vtable = vtable;
        to->radius = ((const hkpSphereShape*)from)->m_radius;
    }
    else if (vtable == hkpCapsuleShape_vtable)
    {
        const hkpCapsuleShape* capsule = (const hkpCapsuleShape*)from;
        to->vtable = vtable;
        to->radius = capsule->m_radius;
        memcpy(to->vertexA, capsule->vertexA, sizeof(to->vertexA));
        memcpy(to->vertexB, capsule->vertexB, sizeof(to->vertexB));
    }
}

void copy_hkpShape(hkpShape* to, const SavedHavokShape* from)
{
    if (to == NULL || from->vtable == 0)
    {
        return;
    }
    if ((uint64_t)to->vtable != from->vtable)
    {
        FATALERROR("copy_hkpShape: shape %p changed kind since the save (vtable %llx, saved %llx)", to, (uint64_t)to->vtable, from->vtable);
    }
    if (from->vtable == hkpSphereShape_vtable)
    {
        ((hkpSphereShape*)to)->m_radius = from->radius;
    }
    else
    {
        hkpCapsuleShape* capsule = (hkpCapsuleShape*)to;
        capsule->m_radius = from->radius;
        memcpy(capsule->vertexA, from->vertexA, sizeof(capsule->vertexA));
        memcpy(capsule->vertexB, from->vertexB, sizeof(capsule->vertexB));
    }
}

void copy_hkpMotion(hkpMotion* to, const hkpMotion* from)
{
    to->m_deactivationIntegrateCounter = from->m_deactivationIntegrateCounter;
    memcpy(to->m_deactivationNumInactiveFrames, from->m_deactivationNumInactiveFrames, sizeof(to->m_deactivationNumInactiveFrames));
    memcpy(to->unk_16, from->unk_16, sizeof(to->unk_16));
    to->m_motionState = from->m_motionState;
    memcpy(to->m_inertiaAndMassInv, from->m_inertiaAndMassInv, sizeof(to->m_inertiaAndMassInv));
    memcpy(to->m_linearVelocity, from->m_linearVelocity, sizeof(to->m_linearVelocity));
    memcpy(to->m_angularVelocity, from->m_angularVelocity, sizeof(to->m_angularVelocity));
    memcpy(to->m_deactivationRefPosition, from->m_deactivationRefPosition, sizeof(to->m_deactivationRefPosition));
    memcpy(to->m_deactivationRefOrientation, from->m_deactivationRefOrientation, sizeof(to->m_deactivationRefOrientation));
    to->m_savedQualityTypeIndex = from->m_savedQualityTypeIndex;
    to->m_gravityFactor = from->m_gravityFactor;
    memcpy(to->unk_134, from->unk_134, sizeof(to->unk_134));
}

void copy_hkpEntity_motion(hkpEntity* to, const hkpMotion* from)
{
    static uint32_t motion_type_logs = 0;
    if (to->m_motion.m_type != from->m_type)
    {
        //a ragdoll that went limp or was keyframed again since the save. Switch it back the way FrpgPhysIns does (FUN_1402a9670)
        if (log_budget(motion_type_logs))
        {
            ConsoleWrite("havok rollback: body %p motion type %d -> %d", to, to->m_motion.m_type, from->m_type);
        }
        hk_setMotionType(to, from->m_type, hkpEntityActivation_DO_ACTIVATE);
        if (to->m_motion.m_type != from->m_type)
        {
            FATALERROR("copy_hkpEntity_motion: body %p did not take motion type %d", to, from->m_type);
        }
    }
    copy_hkpMotion(&to->m_motion, from);
}

void copy_SavedHavokPhantom(SavedHavokPhantom* to, const FrpgPhysShapePhantomIns* from)
{
    const hkpSimpleShapePhantom* phantom = phantom_of(from);
    to->valid = phantom != NULL;
    if (phantom == NULL)
    {
        return;
    }
    to->motionState = phantom->m_motionState;
    copy_hkpShape(&to->shape, (const hkpShape*)phantom->base.m_collidable.base.shape);
}

void copy_SavedHavokPhantom(FrpgPhysShapePhantomIns* to, const SavedHavokPhantom* from)
{
    hkpSimpleShapePhantom* phantom = phantom_of(to);
    if ((phantom != NULL) != from->valid)
    {
        FATALERROR("copy_SavedHavokPhantom: the phantom %s since the save", from->valid ? "no longer exists" : "was created");
    }
    if (phantom == NULL)
    {
        return;
    }
    phantom->m_motionState = from->motionState;
    copy_hkpShape((hkpShape*)phantom->base.m_collidable.base.shape, &from->shape);
}

/* ============================================================
 * The world side of a load
 * ============================================================ */

// Recompute the phantom's AABB from its restored transform and move its broadphase entry there, which also updates its overlapping
// pairs and collision details. The same sequence hkpShapePhantom::setPosition uses (Set_Player_NextStep_Coords @1409c7230).
static void refresh_phantom_broadphase(hkpWorld* world, hkpSimpleShapePhantom* phantom)
{
    hkpShape* shape = (hkpShape*)phantom->base.m_collidable.base.shape;
    if (shape == NULL || world->m_collisionInput == NULL || phantom->base.hkpWorldPtr == NULL)
    {
        return;
    }
    const float tolerance = world->m_collisionInput->m_tolerance * 0.5f;
    alignas(16) float aabb[8];
    shape->vtable->getAabb(shape, &phantom->m_motionState, tolerance, aabb);
    hk_updatePhantomBroadPhase((hkpPhantom*)phantom, aabb);
}

static bool phantom_overlaps(const hkpSimpleShapePhantom* phantom, const hkpCollidable* collidable)
{
    for (uint32_t i = 0; i < phantom->m_collisionDetails_len; i++)
    {
        if (phantom->m_collisionDetails[i] == collidable)
        {
            return true;
        }
    }
    return false;
}

// copy_PlayerIns restored m_manifold from the saved frame, and its contacts hold raw hkpCdBody pointers. The world outside the players
// is not rolled back, so a contact can name a body that has since left the world or been freed. Keep only contacts with a body that
// overlaps the restored phantom now; the proxy finds the others again on its next integrate. Returns how many were dropped.
static uint32_t filter_proxy_manifold(hkpCharacterProxy* proxy)
{
    const hkpSimpleShapePhantom* phantom = (const hkpSimpleShapePhantom*)proxy->HkpSimpleShapePhantom;
    if (phantom == NULL || proxy->m_manifold == NULL)
    {
        return 0;
    }
    int32_t kept = 0;
    for (int32_t i = 0; i < proxy->m_manifold_len; i++)
    {
        const hkpRootCdPoint point = proxy->m_manifold[i];
        if (point.m_rootCollidableB != NULL && !phantom_overlaps(phantom, (const hkpCollidable*)point.m_rootCollidableB))
        {
            continue;
        }
        proxy->m_manifold[kept++] = point;
    }
    const uint32_t dropped = (uint32_t)(proxy->m_manifold_len - kept);
    proxy->m_manifold_len = kept;
    return dropped;
}

// A damage phantom is in the world when its FrpgPhysIns::physWorld is set (Type1/Type2_Compute_PhysShape_in_HavokManPhysWorld).
// copy_DamageMan has just restored that flag and the active list, and built or torn down heap entries with their phantoms. So add or
// remove each pool phantom, and each phantom of a heap entry in the active list, until the world agrees, in one batch each.
static void restore_damage_phantom_membership(hkpWorld* world, DamageMan* damageman)
{
    std::vector<hkpSimpleShapePhantom*> to_add;
    std::vector<hkpSimpleShapePhantom*> to_remove;

    auto reconcile = [&](const FrpgPhysShapePhantomIns* wrapper)
    {
        hkpSimpleShapePhantom* phantom = phantom_of(wrapper);
        if (phantom == NULL)
        {
            return;
        }
        const bool wanted = wrapper->base.base.physWorld != NULL;
        const bool in_world = phantom->base.hkpWorldPtr != NULL;
        if (wanted && !in_world)
        {
            to_add.push_back(phantom);
        }
        else if (!wanted && in_world)
        {
            to_remove.push_back(phantom);
        }
    };

    for (uint32_t i = 0; i < HAVOK_ROLLBACK_DAMAGE_POOL_SIZE; i++)
    {
        const DamageEntry* entry = &damageman->all_damage_entries_list_start[i];
        reconcile(entry->FrpgPhysShapePhantomIns_Sphere);
        reconcile(entry->FrpgPhysShapePhantomIns_Capsule);
    }
    for (const DamageEntry* entry = damageman->active_damage_entries_list; entry != NULL; entry = entry->next)
    {
        if (DamageEntry_isDynamicAlloc(entry))
        {
            reconcile(entry->FrpgPhysShapePhantomIns_Sphere);
            reconcile(entry->FrpgPhysShapePhantomIns_Capsule);
        }
    }

    if (!to_remove.empty())
    {
        hk_removePhantomBatch(world, to_remove.data(), (uint32_t)to_remove.size());
    }
    if (!to_add.empty())
    {
        hk_addPhantomBatch(world, to_add.data(), (uint32_t)to_add.size());
    }
}

// copy_FrpgRagdollIns wrote the saved motions into the bodies. Their AABBs are stale, so invalidate each one
// (hkpCollidable::BoundingVolumeData::invalidate) and let updateEntityBP recompute it from the restored transform.
static void refresh_ragdoll_broadphase(hkpWorld* world, const FrpgRagdollIns* ragdoll)
{
    if (ragdoll == NULL || ragdoll->ragdollInstance == NULL)
    {
        return;
    }
    for (uint32_t i = 0; i < ragdoll->ragdollInstance->m_rigidBodies_len; i++)
    {
        hkpEntity* body = ragdoll->ragdollInstance->m_rigidBodies[i];
        if (body == NULL || body->_hkpWorld == NULL)
        {
            continue;
        }
        body->m_collidable.base.m_boundingVolumeData.base.m_min[0] = 1;
        body->m_collidable.base.m_boundingVolumeData.base.m_max[0] = 0;
        hk_updateEntityBP(world, body);
    }
}

void free_DamageEntry_phantoms(DamageEntry* entry)
{
    hkpWorld* world = get_hkpWorld();
    check_hkpWorld_unlocked(world, "free_DamageEntry_phantoms");
    hkpSimpleShapePhantom* in_world[2];
    uint32_t n = 0;
    for (FrpgPhysShapePhantomIns* wrapper : { entry->FrpgPhysShapePhantomIns_Sphere, entry->FrpgPhysShapePhantomIns_Capsule })
    {
        hkpSimpleShapePhantom* phantom = phantom_of(wrapper);
        if (phantom != NULL && phantom->base.hkpWorldPtr != NULL)
        {
            in_world[n++] = phantom;
        }
        if (wrapper != NULL)
        {
            wrapper->base.base.physWorld = NULL;
        }
    }
    if (n > 0)
    {
        hk_removePhantomBatch(world, in_world, n);
    }
}

/* ============================================================
 * FrpgHavokManImp
 * ============================================================ */

static HavokRollbackState* init_HavokRollbackState()
{
    return (HavokRollbackState*)malloc_(sizeof(HavokRollbackState));
}

static void free_HavokRollbackState(HavokRollbackState* to)
{
    free(to);
}

static void copy_HavokRollbackState(HavokRollbackState* to, const hkpWorld* from)
{
    to->simulation_clock = from->m_simulation->m_clock;
    to->dynamics_step_info = from->m_dynamicsStepInfo;
}

static void copy_hkpWorld(hkpWorld* to, const HavokRollbackState* from)
{
    static uint32_t manifold_logs = 0;
    check_hkpWorld_unlocked(to, "copy_hkpWorld");

    //the next world step has to start from the saved clock, or every body it integrates is stamped with a later time
    to->m_simulation->m_clock = from->simulation_clock;
    to->m_dynamicsStepInfo = from->dynamics_step_info;

    restore_damage_phantom_membership(to, *(DamageMan**)Game::damage_man);

    for (uint32_t i = 0; i < (uint32_t)Rollback::ggpoCurrentPlayerCount; i++)
    {
        ChrCtrl* chrctrl = chr_ctrl(connected_player(i));
        if (chrctrl == NULL)
        {
            continue;
        }
        HavokChara* chara = chrctrl->havokChara;
        if (chara != NULL)
        {
            refresh_phantom_broadphase(to, phantom_of(chara->physShapePhantomIns_1));
            refresh_phantom_broadphase(to, phantom_of(chara->physShapePhantomIns_2));
            if (chara->char_proxy != NULL)
            {
                const uint32_t dropped = filter_proxy_manifold(chara->char_proxy);
                if (dropped > 0 && log_budget(manifold_logs))
                {
                    ConsoleWrite("havok rollback: dropped %u character proxy contacts of player %d whose body no longer overlaps", dropped, i);
                }
            }
        }
        refresh_ragdoll_broadphase(to, chrctrl->ragdollIns);
    }
}

void copy_FrpgPhysWorld(FrpgPhysWorld* to, const FrpgPhysWorld* from, StateTarget target)
{
    if (target == StateTarget::ToLocal)
    {
        copy_HavokRollbackState(to->rollbackState, from->_hkpWorld);
    }
    else if (target == StateTarget::ToGame)
    {
        copy_hkpWorld(to->_hkpWorld, from->rollbackState);
    }
    else
    {
        *to->rollbackState = *from->rollbackState;
    }
}

//all the Havok Man inits are only for dll memory, game side they always exist
FrpgPhysWorld* init_FrpgPhysWorld()
{
    FrpgPhysWorld* local = (FrpgPhysWorld*)malloc_(sizeof(FrpgPhysWorld));
    local->rollbackState = init_HavokRollbackState();
    return local;
}

void free_FrpgPhysWorld(FrpgPhysWorld* to)
{
    free_HavokRollbackState(to->rollbackState);
    free(to);
}

void copy_FrpgHavokManImp(FrpgHavokManImp* to, const FrpgHavokManImp* from, StateTarget target)
{
    copy_FrpgPhysWorld(to->physWorld, from->physWorld, target);
}

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

// ---- serializers ------------------------------------------------------------
//
// The havok values themselves are hashed by their owner: a character's phantoms and ragdoll bodies with PlayerIns, a damage entry's
// shapes with DamageMan. Only vtables are hashed as data here; they are fixed code addresses and identify a shape's kind.

void serialize_hkMotionState(StateVisitor& v, const hkMotionState* m)
{
    v.begin("hkMotionState");
    for (int i = 0; i < 4; i++) v.field("transform_rotation0", m->m_transform_rotation0[i]);
    for (int i = 0; i < 4; i++) v.field("transform_rotation1", m->m_transform_rotation1[i]);
    for (int i = 0; i < 4; i++) v.field("transform_rotation2", m->m_transform_rotation2[i]);
    for (int i = 0; i < 4; i++) v.field("transform_translation", m->m_transform_translation[i]);
    for (int i = 0; i < 3; i++) v.field("swept_centerOfMass0", m->m_sweptTransform_centerOfMass0[i]);
    // .w is the world clock at the last integrate. Not compared: two instances started their clocks at different times
    v.excluded("swept_time0", sizeof(m->m_sweptTransform_centerOfMass0[3]));
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

// Mirrors copy_hkpMotion: the object header and m_savedMotion are not state
void serialize_hkpMotion(StateVisitor& v, const hkpMotion* m)
{
    v.begin("hkpMotion");
    v.field("m_type", m->m_type);
    // Havok's sleep bookkeeping: both counters are driven by the body's simulation island (its check timers and split counter), and
    // islands are world state rollback does not restore, so they drift from the live frame and differ between two instances anyway.
    // They are still restored, they are just not compared.
    v.excluded("m_deactivationIntegrateCounter", sizeof(m->m_deactivationIntegrateCounter));
    v.excluded("m_deactivationNumInactiveFrames", sizeof(m->m_deactivationNumInactiveFrames));
    v.blob("unk_16", m->unk_16, sizeof(m->unk_16));
    serialize_hkMotionState(v, &m->m_motionState);
    for (int i = 0; i < 4; i++) v.field("m_inertiaAndMassInv", m->m_inertiaAndMassInv[i]);
    for (int i = 0; i < 4; i++) v.field("m_linearVelocity", m->m_linearVelocity[i]);
    for (int i = 0; i < 4; i++) v.field("m_angularVelocity", m->m_angularVelocity[i]);
    for (int i = 0; i < 8; i++) v.field("m_deactivationRefPosition", m->m_deactivationRefPosition[i]);
    for (int i = 0; i < 2; i++) v.field("m_deactivationRefOrientation", m->m_deactivationRefOrientation[i]);
    v.field("m_savedQualityTypeIndex", m->m_savedQualityTypeIndex);
    v.field("m_gravityFactor", m->m_gravityFactor);
    v.blob("unk_134", m->unk_134, sizeof(m->unk_134));
    v.end();
}

void serialize_SavedHavokShape(StateVisitor& v, const char* name, const SavedHavokShape* s)
{
    v.begin(name);
    v.field("vtable", s->vtable);   // fixed code addr; identifies the shape kind, 0 = not saved
    v.field("radius", s->radius);
    for (int i = 0; i < 4; i++) v.field("vertexA", s->vertexA[i]);
    for (int i = 0; i < 4; i++) v.field("vertexB", s->vertexB[i]);
    v.end();
}

void serialize_SavedHavokPhantom(StateVisitor& v, const char* name, const SavedHavokPhantom* p)
{
    v.begin(name);
    v.field("valid", p->valid);
    if (p->valid)
    {
        serialize_hkMotionState(v, &p->motionState);
        serialize_SavedHavokShape(v, "shape", &p->shape);
    }
    v.end();
}

void serialize_FrpgHavokManImp(StateVisitor& v, const FrpgHavokManImp* h, const uint32_t* order, uint32_t nplayers)
{
    (void)order;
    (void)nplayers;
    const HavokRollbackState* s = h->physWorld->rollbackState;
    v.begin("FrpgHavokManImp");
    // Restored, but not compared: both are clock-derived, and the two instances started their clocks at different times.
    v.excluded("simulation_clock", sizeof(s->simulation_clock));
    v.excluded("dynamics_step_info", sizeof(s->dynamics_step_info));
    v.note("player_physics_hashed_by", "PlayerIns (character phantoms, ragdoll bodies) and DamageMan (entry shapes)");
    v.end();
}

std::string print_FrpgHavokManImp(const FrpgHavokManImp* h, const uint32_t* order, uint32_t nplayers)
{
    StateVisitor v(StateVisitor::Mode::Print);
    serialize_FrpgHavokManImp(v, h, order, nplayers);
    return v.text();
}

uint64_t hash_FrpgHavokManImp(const FrpgHavokManImp* h, const uint32_t* order, uint32_t nplayers)
{
    StateVisitor v(StateVisitor::Mode::Hash);
    serialize_FrpgHavokManImp(v, h, order, nplayers);
    return v.digest();
}
