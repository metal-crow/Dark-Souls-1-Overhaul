#include "FrpgHavokManImpStructFunctions.h"

/* ---------------- HAVOK MAN ------------------ */

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
    copy_hkpWorld(to->_hkpWorld, from->_hkpWorld, target);
}

FrpgPhysWorld* init_FrpgPhysWorld()
{
    FrpgPhysWorld* local = (FrpgPhysWorld*)malloc_(sizeof(FrpgPhysWorld));

    local->_hkpWorld = init_hkpWorld();

    return local;
}

void free_FrpgPhysWorld(FrpgPhysWorld* to)
{
    free_hkpWorld(to->_hkpWorld);
    free(to);
}

void copy_hkpWorld(hkpWorld* to, const hkpWorld* from, StateTarget target)
{
    copy_hkp3AxisSweep(to->m_broadPhase, from->m_broadPhase, target);
    //don't need to actually save/load the simulation islands themselves, those should be stable for the lifetime of this session
    //copy_hkpSimulationIsland(to->m_activeSimulationIslands, to->m_activeSimulationIslands_size, from->m_activeSimulationIslands, from->m_activeSimulationIslands_size, target);
    //copy_hkpSimulationIsland(to->m_inactiveSimulationIslands, to->m_inactiveSimulationIslands_size, from->m_inactiveSimulationIslands, from->m_inactiveSimulationIslands_size, target);
    //copy_hkpSimulationIsland(to->m_dirtySimulationIslands, to->m_dirtySimulationIslands_size, from->m_dirtySimulationIslands, from->m_dirtySimulationIslands_size, target);
    to->m_phantoms_size = from->m_phantoms_size;
    if (to->m_phantoms_cap < from->m_phantoms_size)
    {
        switch (target)
        {
        case StateTarget::ToGame:
            increase_list_size(Game::MemHeapAllocator, &to->m_phantoms, 0x8);
            break;
        case StateTarget::ToLocal:
        case StateTarget::Copy:
            to->m_phantoms = (void**)realloc_(to->m_phantoms, from->m_phantoms_cap * sizeof(void*));
            to->m_phantoms_cap = from->m_phantoms_cap;
            break;
        }
    }
    std::unordered_map<uint64_t, size_t> m_phantoms_tracker = {};
    for (size_t i = 0; i < from->m_phantoms_size; i++)
    {
        //there may be duplicate pointers in this list which we need to support
        if (m_phantoms_tracker.contains((uint64_t)(from->m_phantoms[i])))
        {
            //if we need to clear the existing ptr at this entry
            if (to->m_phantoms[i] != NULL && to->m_phantoms[i] != to->m_phantoms[m_phantoms_tracker[(uint64_t)(from->m_phantoms[i])]])
            {
                free_hkpPhantom(to->m_phantoms[i], target);
            }
            to->m_phantoms[i] = to->m_phantoms[m_phantoms_tracker[(uint64_t)(from->m_phantoms[i])]];
        }
        else
        {
            copy_hkpPhantom(&to->m_phantoms[i], from->m_phantoms[i], target);
            m_phantoms_tracker[(uint64_t)(from->m_phantoms[i])] = i;
        }
    }
}

hkpWorld* init_hkpWorld()
{
    hkpWorld* local = (hkpWorld*)malloc_(sizeof(hkpWorld));
    
    local->m_activeSimulationIslands = NULL;
    local->m_inactiveSimulationIslands = NULL;
    local->m_dirtySimulationIslands = NULL;
    local->m_broadPhase = init_hkp3AxisSweep();
    local->m_phantoms = NULL;

    return local;
}

void free_hkpWorld(hkpWorld* to)
{
    //free_hkpSimulationIsland(to->m_activeSimulationIslands);
    //free_hkpSimulationIsland(to->m_inactiveSimulationIslands);
    //free_hkpSimulationIsland(to->m_dirtySimulationIslands);
    free_hkp3AxisSweep(to->m_broadPhase);
    for (size_t i = 0; i < to->m_phantoms_cap; i++)
    {
        if (to->m_phantoms[i] != NULL)
        {
            free_hkpPhantom(to->m_phantoms[i], StateTarget::ToLocal);
        }
    }
    free(to->m_phantoms);
    free(to);
}

void copy_hkp3AxisSweep(hkp3AxisSweep* to, const hkp3AxisSweep* from, StateTarget target)
{
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    if ((to->m_nodes_cap & 0x3fffffff) < from->m_nodes_len)
    {
        to->m_nodes_len = from->m_nodes_len;
        if (target == StateTarget::ToGame)
        {
            increase_list_size(Game::MemHeapAllocator, &to->m_nodes, 0x18);
        }
        else
        {
            to->m_nodes = (hkpBpNode*)realloc_(to->m_nodes, (to->m_nodes_len + 1) * sizeof(hkpBpNode));
            to->m_nodes_cap = to->m_nodes_cap + 1;
        }
    }
    for (size_t i = 0; i < to->m_nodes_len; i++)
    {
        copy_hkpBpNode(&to->m_nodes[i], &from->m_nodes[i], target);
    }
    copy_hkpBpAxis(&to->m_axis[0], &from->m_axis[0], target);
    copy_hkpBpAxis(&to->m_axis[1], &from->m_axis[1], target);
    copy_hkpBpAxis(&to->m_axis[2], &from->m_axis[2], target);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

hkp3AxisSweep* init_hkp3AxisSweep()
{
    hkp3AxisSweep* local = (hkp3AxisSweep*)malloc_(sizeof(hkp3AxisSweep));

    //unknown size, init on copy
    local->m_nodes = NULL;
    local->m_axis[0].arry = NULL;
    local->m_axis[1].arry = NULL;
    local->m_axis[2].arry = NULL;
    //always null
    local->m_markers = NULL;

    return local;
}

void free_hkp3AxisSweep(hkp3AxisSweep* to)
{
    if (to->m_nodes != NULL)
    {
        for (size_t i = 0; i < to->m_nodes_cap; i++)
        {
            free(to->m_nodes[i].index_in_array);
        }
        free(to->m_nodes);
    }
    if (to->m_axis[0].arry != NULL)
    {
        free(to->m_axis[0].arry);
    }
    if (to->m_axis[1].arry != NULL)
    {
        free(to->m_axis[1].arry);
    }
    if (to->m_axis[2].arry != NULL)
    {
        free(to->m_axis[2].arry);
    }
    free(to);
}

void copy_hkpBpNode(hkpBpNode* to, const hkpBpNode* from, StateTarget target)
{
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    if (from->index_in_array == NULL)
    {
        to->index_in_array = NULL;
    }
    else
    {
        if (to->index_in_array == NULL)
        {
            if (target == StateTarget::ToGame)
            {
                FATALERROR("to->index_in_array is NULL");
            }
            else
            {
                to->index_in_array = (uint32_t*)malloc_(sizeof(int));
            }
        }
        *to->index_in_array = *from->index_in_array;
    }
}

void copy_hkpBpAxis(hkpBpAxis* to, const hkpBpAxis* from, StateTarget target)
{
    to->len = from->len;
    if ((to->capacity & 0x3fffffff) < from->len)
    {
        if (target == StateTarget::ToGame)
        {
            increase_list_size(Game::MemHeapAllocator, &to->arry, 0x8);
        }
        else
        {
            to->arry = (hkpBpEndPoint*)realloc_(to->arry, (to->len + 1) * sizeof(hkpBpEndPoint));
            to->capacity = to->len + 1;
        }
    }
    for (size_t i = 0; i < from->len; i++)
    {
        copy_hkpBpEndPoint(&to->arry[i], &from->arry[i], target);
    }
}

void copy_hkpBpEndPoint(hkpBpEndPoint* to, const hkpBpEndPoint* from, StateTarget target)
{
    to->data_1 = from->data_1;
}

PhantomType hkpPhantom_getType(void* to)
{
    if (to == NULL)
    {
        return PhantomType::PhantomNull;
    }
    if (*(uint64_t*)(to) == 0x14145cb08) //hkpAabbPhantom::vftable (ragdoll)
    {
        return PhantomType::Aabb;
    }
    if (*(uint64_t*)(to) == 0x14145ccc8) //hkpSimpleShapePhantom::vftable
    {
        return PhantomType::SimpleShape;
    }
    return PhantomType::InvalidPhantom;
}

void copy_hkpPhantom(void** to, void* from, StateTarget target)
{
    PhantomType type = hkpPhantom_getType(from);
    switch (type)
    {
    case PhantomType::SimpleShape:
        //destroy the target if not the right class
        if (*to != NULL)
        {
            PhantomType totype = hkpPhantom_getType(*to);
            if (totype != PhantomType::SimpleShape)
            {
                free_hkpPhantom((*to), target);
                *to = NULL;
            }
        }
        //init the target if needed
        if (*to == NULL)
        {
            *to = init_hkpSimpleShapePhantom(target);
        }
        copy_hkpSimpleShapePhantom((hkpSimpleShapePhantom*)(*to), (const hkpSimpleShapePhantom*)from, target);
        break;
    case PhantomType::Aabb:
        //don't need to handle copying, this is just for ragdolls
        if (target == StateTarget::ToGame)
        {
            free_hkpPhantom((*to), target);
            *to = NULL;
        }
        else
        {
            if (*to == NULL)
            {
                *to = malloc_(sizeof(uint64_t));
            }
            *(uint64_t*)(*to) = (uint64_t)0x14145cb08;
        }
        break;
    case PhantomType::PhantomNull:
        free_hkpPhantom(*to, target);
        *to = NULL;
        return;
    case PhantomType::InvalidPhantom:
        FATALERROR("%p is not a valid phantom for copy (vtable %llx)", from, *(uint64_t*)from);
        break;
    }
}

void free_hkpPhantom(void* to, StateTarget target)
{
    PhantomType type = hkpPhantom_getType(to);
    switch (type)
    {
    case PhantomType::SimpleShape:
        free_hkpSimpleShapePhantom((hkpSimpleShapePhantom*)to, target);
        break;
    case PhantomType::Aabb:
        if (target == StateTarget::ToGame)
        {
            hkReferencedObject_deref(to);
        }
        else
        {
            free(to);
        }
        break;
    case PhantomType::PhantomNull:
        break;
    case PhantomType::InvalidPhantom:
        FATALERROR("%p is not a valid phantom for free (vtable %llx)", to, *(uint64_t*)to);
        break;
    }
}

std::string print_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to)
{
    std::string out = "hkpSimpleShapePhantom\n";
    return out;
}

void copy_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, const hkpSimpleShapePhantom* from, StateTarget target)
{
    to->vtable = from->vtable;
    to->data_0 = from->data_0;
    to->hkpWorldPtr = from->hkpWorldPtr;
    to->m_userData = from->m_userData;
    copy_hkpLinkedCollidable(&to->m_collidable, &from->m_collidable, &to->m_motionState, target);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->unk_0 = from->unk_0;

    if ((to->m_properties_cap & 0x3fffffff) < from->m_properties_len)
    {
        uint32_t old_len = to->m_properties_len;
        to->m_properties_len = from->m_properties_len;
        if (target == StateTarget::ToGame)
        {
            increase_list_size(Game::MemHeapAllocator, &to->m_properties, 0x10);
        }
        else
        {
            to->m_properties = (hkpProperty*)realloc_(to->m_properties, to->m_properties_len * sizeof(hkpProperty));
            to->m_properties_cap = to->m_properties_len;
        }
        for (size_t i = old_len; i < to->m_properties_len; i++)
        {
            init_hkpProperty(&to->m_properties[i], target);
        }
    }
    for (size_t i = 0; i < from->m_properties_len; i++)
    {
        copy_hkpProperty(&to->m_properties[i], &from->m_properties[i], target);
    }

    copy_hkMotionState(&to->m_motionState, &from->m_motionState);

    if ((to->m_collisionDetails_cap & 0x3fffffff) < from->m_collisionDetails_len)
    {
        uint32_t old_len = to->m_collisionDetails_len;
        to->m_collisionDetails_len = from->m_collisionDetails_len;
        if (target == StateTarget::ToGame)
        {
            increase_list_size(Game::MemHeapAllocator, &to->m_collisionDetails, 0x8);
        }
        else
        {
            to->m_collisionDetails = (hkpCollidable**)realloc_(to->m_collisionDetails, to->m_collisionDetails_len * sizeof(hkpCollidable*));
            to->m_collisionDetails_cap = to->m_collisionDetails_len;
        }
        for (size_t i = old_len; i < to->m_collisionDetails_len; i++)
        {
            init_hkpCollidable(&to->m_collisionDetails[i], target);
        }
    }
    for (size_t i = 0; i < from->m_collisionDetails_len; i++)
    {
        copy_hkpCollidable(to->m_collisionDetails[i], from->m_collisionDetails[i], &to->m_motionState, target);
    }

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}


//all of it's children arrays are unknown sized, so should be init'd on copy
hkpSimpleShapePhantom* init_hkpSimpleShapePhantom(StateTarget target)
{
    hkpSimpleShapePhantom* local_hkpSimpleShapePhantom = NULL;
    if (target == StateTarget::ToGame)
    {
        local_hkpSimpleShapePhantom = (hkpSimpleShapePhantom*)Game::thread_malloc(sizeof(hkpSimpleShapePhantom));
    }
    else
    {
        local_hkpSimpleShapePhantom = (hkpSimpleShapePhantom*)malloc_(sizeof(hkpSimpleShapePhantom));
    }
    local_hkpSimpleShapePhantom->vtable = (void*)0x14145ccc8;
    //don't know if we need a cap or a sphere, so init on copy
    local_hkpSimpleShapePhantom->m_collidable.base.shape = NULL;

    //we don't know how big these should be, so they are init'd on copy
    local_hkpSimpleShapePhantom->m_collidable.base.m_boundingVolumeData.m_childShapeAabbs = NULL;
    local_hkpSimpleShapePhantom->m_collidable.base.m_boundingVolumeData.m_childShapeKeys = NULL;
    local_hkpSimpleShapePhantom->m_collidable.base.m_boundingVolumeData.m_numChildShapeAabbs = 0;
    local_hkpSimpleShapePhantom->m_collidable.base.m_boundingVolumeData.m_capacityChildShapeAabbs = 0;

    local_hkpSimpleShapePhantom->m_collidable.m_collisionEntries = NULL;
    local_hkpSimpleShapePhantom->m_collidable.m_collisionEntries_len = 0;
    local_hkpSimpleShapePhantom->m_collidable.m_collisionEntries_cap = 0;

    local_hkpSimpleShapePhantom->m_properties = NULL;
    local_hkpSimpleShapePhantom->m_properties_len = 0;
    local_hkpSimpleShapePhantom->m_properties_cap = 0;

    local_hkpSimpleShapePhantom->m_collisionDetails = NULL;
    local_hkpSimpleShapePhantom->m_collisionDetails_len = 0;
    local_hkpSimpleShapePhantom->m_collisionDetails_cap = 0;

    return local_hkpSimpleShapePhantom;
}

void free_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        hkReferencedObject_deref(to);
    }
    else
    {
        free(to->m_collidable.base.shape);
        free(to->m_collidable.base.m_boundingVolumeData.m_childShapeAabbs);
        free(to->m_collidable.base.m_boundingVolumeData.m_childShapeKeys);
        free(to->m_collidable.m_collisionEntries);
        for (size_t i = 0; i < to->m_properties_len; i++)
        {
            free_hkpProperty(&to->m_properties[i]);
        }
        free(to->m_properties);
        for (size_t i = 0; i < to->m_collisionDetails_len; i++)
        {
            free_hkpCollidable(to->m_collisionDetails[i]);
        }
        free(to->m_collisionDetails);
        free(to);
    }
}

hkpSimpleShapePhantom* get_hkpSimpleShapePhantom(hkpWorld* world, FrpgPhysShapePhantomIns* parent)
{
    auto ret = find_hkpSimpleShapePhantom(world, parent);
    if (ret == NULL)
    {
        FATALERROR("Could not find shape phantom for parent %p", parent);
    }
    return ret;
}

hkpSimpleShapePhantom* find_hkpSimpleShapePhantom(hkpWorld* world, FrpgPhysShapePhantomIns* parent)
{
    void** array = world->m_phantoms;
    for (size_t i = 0; i < world->m_phantoms_size; i++)
    {
        void* elem = array[i];
        PhantomType type = hkpPhantom_getType(elem);
        if (type == PhantomType::SimpleShape)
        {
            hkpSimpleShapePhantom* phantom = (hkpSimpleShapePhantom*)elem;
            if (*(phantom->m_userData) == parent)
            {
                return phantom;
            }
        }
    }
    return NULL;
}

/* ---------------- CHRCTRL + DAMAGE MAN ------------------ */

void copy_hkpLinkedCollidable(hkpLinkedCollidable* to, const hkpLinkedCollidable* from, hkMotionState* motion, StateTarget target)
{
    copy_hkpCollidable(&to->base, &from->base, motion, target);
    if (from->m_collisionEntries_len > 0)
    {
        FATALERROR("hkpLinkedCollidable->m_collisionEntries_len > 0");
    }
    //if ((to->m_collisionEntries_cap & 0x3fffffff) < from->m_collisionEntries_len)
    //{
    //    uint32_t old_len = to->m_collisionEntries_len;
    //    to->m_collisionEntries_len = from->m_collisionEntries_len;
    //    if (target)
    //    {
    //        increase_list_size(Game::MemHeapAllocator, &to->m_collisionEntries, 0x10);
    //    }
    //    else
    //    {
    //        to->m_collisionEntries = (CollisionEntry*)realloc_(to->m_collisionEntries, to->m_collisionEntries_len * sizeof(CollisionEntry));
    //        to->m_collisionEntries_cap = to->m_collisionEntries_len + 1;
    //    }
    //    for (size_t i = old_len; i < to->m_collisionEntries_len; i++)
    //    {
    //        init_CollisionEntry(&to->m_collisionEntries[i], target);
    //    }
    //}
    //for (size_t i = 0; i < from->m_collisionEntries_len; i++)
    //{
    //    copy_CollisionEntry(&to->m_collisionEntries[i], &from->m_collisionEntries[i], to);
    //}
}



void copy_hkpProperty(hkpProperty* to, hkpProperty* from, StateTarget target)
{
    to->key = from->key;
    to->padding = from->padding;
    to->data = from->data;
    //copy_hkpCharacterProxy(to->data, from->data, target);
}

void init_hkpProperty(hkpProperty* to, StateTarget target)
{
    //to->data = init_hkpCharacterProxy(target);
}

void free_hkpProperty(hkpProperty* to)
{
    //free_hkpCharacterProxy(to->data);
}

void copy_hkMotionState(hkMotionState* to, const hkMotionState* from)
{
    memcpy(to, from, sizeof(hkMotionState));
}


void copy_hkpCollidable(hkpCollidable* to, const hkpCollidable* from, hkMotionState* motion, StateTarget target)
{
    copy_hkpShape(&to->shape, from->shape, target);
    to->m_shapeKey = from->m_shapeKey;
    to->m_motion = (void*)motion;
    if (from->m_parent != NULL)
    {
        FATALERROR("%p collidable has non null parent. Never seen before", from);
    }
    to->m_parent = NULL;
    to->m_ownerOffset = from->m_ownerOffset;
    to->m_forceCollideOntoPpu = from->m_forceCollideOntoPpu;
    to->m_shapeSizeOnSpu = from->m_shapeSizeOnSpu;
    copy_hkpTypedBroadPhaseHandle(&to->m_broadPhaseHandle, &from->m_broadPhaseHandle);
    copy_BoundingVolumeData(&to->m_boundingVolumeData, &from->m_boundingVolumeData, target);
    to->m_allowedPenetrationDepth = from->m_allowedPenetrationDepth;
}

void init_hkpCollidable(hkpCollidable** to, StateTarget target)
{
    if (target != StateTarget::ToGame)
    {
        *to = (hkpCollidable*)malloc_(sizeof(hkpCollidable));
    }
    else
    {
        //TODO how does the game alloc this?
        FATALERROR("Need to alloc hkpCollidable in game");
    }
    //unknown, init on copy
    (*to)->shape = NULL;
    (*to)->m_parent = NULL;
    init_BoundingVolumeData(&(*to)->m_boundingVolumeData, target);
}

void free_hkpCollidable(hkpCollidable* to)
{
    free_hkpShape(to->shape, StateTarget::ToLocal);
    free_BoundingVolumeData(&to->m_boundingVolumeData, StateTarget::ToLocal);
    free(to);
}


//void copy_CollisionEntry(CollisionEntry* to, CollisionEntry* from, hkpLinkedCollidable* parent)
//{
//    copy_hkpAgentNnEntry(to->m_agentEntry, from->m_agentEntry);
//    to->m_partner = parent;
//}


void copy_hkpTypedBroadPhaseHandle(hkpTypedBroadPhaseHandle* to, const hkpTypedBroadPhaseHandle* from)
{
    memcpy(to, from, sizeof(hkpTypedBroadPhaseHandle));
}


void copy_BoundingVolumeData(BoundingVolumeData* to, const BoundingVolumeData* from, StateTarget target)
{
    memcpy(&to->base, &from->base, sizeof(to->base));

    if (from->m_numChildShapeAabbs > 0)
    {
        FATALERROR("testing BoundingVolumeData");
    }

    //if ((to->m_capacityChildShapeAabbs & 0x3fffffff) < from->m_numChildShapeAabbs)
    //{
    //    to->m_numChildShapeAabbs = from->m_numChildShapeAabbs;
    //    if (target)
    //    {
    //        //alloc;
    //    }
    //    else
    //    {
    //        to->m_childShapeAabbs = (hkAabbUint32*)realloc_(to->m_childShapeAabbs, (to->m_numChildShapeAabbs + 1) * sizeof(hkAabbUint32));
    //        to->m_childShapeKeys = (uint32_t*)realloc_(to->m_childShapeKeys, (to->m_numChildShapeAabbs + 1) * sizeof(uint32_t));
    //        to->m_capacityChildShapeAabbs = to->m_numChildShapeAabbs + 1;
    //    }
    //}
    //for (size_t i = 0; i < from->m_numChildShapeAabbs; i++)
    //{
    //    memcpy(&to->m_childShapeAabbs[i], &from->m_childShapeAabbs[i], sizeof(hkAabbUint32));
    //    to->m_childShapeKeys[i] = from->m_childShapeKeys[i];
    //}
}

void init_BoundingVolumeData(BoundingVolumeData* to, StateTarget target)
{
    if (target != StateTarget::ToGame)
    {
        //init on copy
    }
    else
    {
        FATALERROR("In-game alloc of BoundingVolumeData");
    }
}

void free_BoundingVolumeData(BoundingVolumeData* to, StateTarget target)
{
    if (target != StateTarget::ToGame)
    {
        free(to->m_childShapeAabbs);
        free(to->m_childShapeKeys);
    }
    else
    {
        FATALERROR("In-game free of BoundingVolumeData");
    }
}

//void copy_hkpAgentNnEntry(hkpAgentNnEntry* to, hkpAgentNnEntry* from)
//{
//
//}


ShapeType hkpShape_getType(void* to)
{
    if (to == NULL)
    {
        return ShapeType::ShapeNull;
    }
    if (*(uint64_t*)(to) == 0x14141c200) //hkpSphereShape::vftable
    {
        return ShapeType::Sphere;
    }
    if (*(uint64_t*)(to) == 0x14141bf58) //hkpCapsuleShape::vftable
    {
        return ShapeType::Capsule;
    }
    if (*(uint64_t*)(to) == 0x14141bd68) //hkpMoppBvTreeShape::vftable
    {
        return ShapeType::MoppBvTree;
    }
    if (*(uint64_t*)(to) == 0x14141c0c0) //hkpConvexVerticesShape::vftable
    {
        return ShapeType::ConvexVertices;
    }
    if (*(uint64_t*)(to) == 0x14141bff8) //hkpConvexTranslateShape::vftable
    {
        return ShapeType::ConvexTranslate;
    }
    if (*(uint64_t*)(to) == 0x14141be18) //hkpBoxShape::vftable
    {
        return ShapeType::Box;
    }
    return ShapeType::InvalidShape;
}

void copy_hkpShape(void** to, void* from, StateTarget target)
{
    ShapeType fromshape = hkpShape_getType(from);
    ShapeType toshape = hkpShape_getType(*to);

    switch (fromshape)
    {
    case ShapeType::Sphere:
        if (toshape != fromshape)
        {
            free_hkpShape(*to, target);
            *to = init_hkpSphereShape(target);
        }
        copy_hkpSphereShape((hkpSphereShape**)to, (hkpSphereShape*)from, target);
        break;
    case ShapeType::Capsule:
        if (toshape != fromshape)
        {
            free_hkpShape(*to, target);
            *to = init_hkpCapsuleShape(target);
        }
        copy_hkpCapsuleShape((hkpCapsuleShape**)to, (hkpCapsuleShape*)from, target);
        break;
    case ShapeType::MoppBvTree:
        if (toshape != fromshape)
        {
            free_hkpShape(*to, target);
            *to = init_hkpMoppBvTreeShape(target);
        }
        copy_hkpMoppBvTreeShape((hkpMoppBvTreeShape**)to, (hkpMoppBvTreeShape*)from, target);
        break;
    case ShapeType::ConvexVertices:
        FATALERROR("ConvexVertices shape detected %p", from);
        if (toshape != fromshape)
        {
            free_hkpShape(*to, target);
            *to = init_hkpConvexVerticesShape(target);
        }
        copy_hkpConvexVerticesShape((hkpConvexVerticesShape**)to, (hkpConvexVerticesShape*)from, target);
        break;
    case ShapeType::ConvexTranslate:
        if (toshape != fromshape)
        {
            free_hkpShape(*to, target);
            *to = init_hkpConvexTranslateShape(target);
        }
        copy_hkpConvexTranslateShape((hkpConvexTranslateShape**)to, (hkpConvexTranslateShape*)from, target);
        break;
    case ShapeType::Box:
        if (toshape != fromshape)
        {
            free_hkpShape(*to, target);
            *to = init_hkpBoxShape(target);
        }
        copy_hkpBoxShape((hkpBoxShape**)to, (hkpBoxShape*)from, target);
        break;
    case ShapeType::ShapeNull:
        free_hkpShape(*to, target);
        *to = NULL;
        break;
    case ShapeType::InvalidShape:
        FATALERROR("%p is not a valid shape (vtable %llx)", from, *(uint64_t*)from);
        break;
    }
}

void free_hkpShape(void* to, StateTarget target)
{
    switch (hkpShape_getType(to))
    {
    case ShapeType::Sphere:
        free_hkpSphereShape((hkpSphereShape*)to, target);
        break;
    case ShapeType::Capsule:
        free_hkpCapsuleShape((hkpCapsuleShape*)to, target);
        break;
    case ShapeType::MoppBvTree:
        free_hkpMoppBvTreeShape((hkpMoppBvTreeShape*)to, target);
        break;
    case ShapeType::ConvexVertices:
        free_hkpConvexVerticesShape((hkpConvexVerticesShape*)to, target);
        break;
    case ShapeType::ConvexTranslate:
        free_hkpConvexTranslateShape((hkpConvexTranslateShape*)to, target);
        break;
    case ShapeType::Box:
        free_hkpBoxShape((hkpBoxShape*)to, target);
        break;
    case ShapeType::ShapeNull:
        break;
    case ShapeType::InvalidShape:
        FATALERROR("Unable to free invalid shape %p (vtable %llx)", to, *(uint64_t*)to);
        break;
    }
}


void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape* from, StateTarget target)
{
    if (*to == NULL && from != NULL)
    {
        *to = init_hkpSphereShape(target);
    }
    if (*to != NULL && from == NULL)
    {
        free_hkpSphereShape(*to, target);
        *to = NULL;
    }
    if (*to != NULL && from != NULL)
    {
        (*to)->vtable = (from)->vtable;
        memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
        memcpy((*to)->data_1, (from)->data_1, sizeof((*to)->data_1));
        if ((from)->m_userData != 0)
        {
            FATALERROR("hkpSphereShape->m_userData is non-0, value %x", (from)->m_userData);
        }
    }
}

hkpSphereShape* init_hkpSphereShape(StateTarget target)
{
    hkpSphereShape* local;
    if (target == StateTarget::ToGame)
    {
        local = (hkpSphereShape*)Game::thread_malloc(sizeof(hkpSphereShape));
    }
    else
    {
        local = (hkpSphereShape*)malloc_(sizeof(hkpSphereShape));
    }
    local->vtable = 0x14141c200;
    local->m_userData = NULL;
    return local;
}

void free_hkpSphereShape(hkpSphereShape* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        hkReferencedObject_deref((void*)to);
    }
    else
    {
        free(to);
    }
}


void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape* from, StateTarget target)
{
    if (*to == NULL && from != NULL)
    {
        *to = init_hkpCapsuleShape(target);
    }
    if (*to != NULL && from == NULL)
    {
        free_hkpCapsuleShape(*to, target);
        *to = NULL;
    }
    if (*to != NULL && from != NULL)
    {
        (*to)->vtable = (from)->vtable;
        memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
        memcpy((*to)->data_1, (from)->data_1, sizeof((*to)->data_1));
        if ((from)->m_userData != 0)
        {
            FATALERROR("hkpCapsuleShape->m_userData is non-0, value %x", (from)->m_userData);
        }
    }
}

hkpCapsuleShape* init_hkpCapsuleShape(StateTarget target)
{
    hkpCapsuleShape* local;
    if (target == StateTarget::ToGame)
    {
        local = (hkpCapsuleShape*)Game::thread_malloc(sizeof(hkpCapsuleShape));
    }
    else
    {
        local = (hkpCapsuleShape*)malloc_(sizeof(hkpCapsuleShape));
    }
    local->vtable = 0x14141bf58;
    local->m_userData = NULL;
    return local;
}

void free_hkpCapsuleShape(hkpCapsuleShape* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        hkReferencedObject_deref((void*)to);
    }
    else
    {
        free(to);
    }
}


void copy_hkpMoppBvTreeShape(hkpMoppBvTreeShape** to, hkpMoppBvTreeShape* from, StateTarget target)
{
    if (*to == NULL && from != NULL)
    {
        *to = init_hkpMoppBvTreeShape(target);
    }
    if (*to != NULL && from == NULL)
    {
        free_hkpMoppBvTreeShape(*to, target);
        *to = NULL;
    }
    if (*to != NULL && from != NULL)
    {
        (*to)->vtable1 = (from)->vtable1;
        memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
        if ((from)->m_userData != 0)
        {
            FATALERROR("hkpMoppBvTreeShape->m_userData is non-0, value %x", (from)->m_userData);
        }
        (*to)->data_1 = (from)->data_1;
        //refobject1
        (*to)->unk2 = (from)->unk2;
        memcpy((*to)->data_2, (from)->data_2, sizeof((*to)->data_2));
        (*to)->vtable2 = (from)->vtable2;
        //refobject2
        memcpy((*to)->data_3, (from)->data_3, sizeof((*to)->data_3));
    }
}

hkpMoppBvTreeShape* init_hkpMoppBvTreeShape(StateTarget target)
{
    hkpMoppBvTreeShape* local;
    if (target == StateTarget::ToGame)
    {
        local = (hkpMoppBvTreeShape*)Game::thread_malloc(sizeof(hkpMoppBvTreeShape));
    }
    else
    {
        local = (hkpMoppBvTreeShape*)malloc_(sizeof(hkpMoppBvTreeShape));
    }
    local->vtable1 = 0x14141bd68;
    local->m_userData = NULL;
    return local;
}

void free_hkpMoppBvTreeShape(hkpMoppBvTreeShape* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        hkReferencedObject_deref(to->refObject1);
        hkReferencedObject_deref(to->refObject2);
    }
    else
    {
        free(to);
    }
}


void copy_hkpConvexVerticesShape(hkpConvexVerticesShape** to, hkpConvexVerticesShape* from, StateTarget target)
{
    FATALERROR("TODO");
    if (*to == NULL && from != NULL)
    {
        *to = init_hkpConvexVerticesShape(target);
    }
    if (*to != NULL && from == NULL)
    {
        free_hkpConvexVerticesShape(*to, target);
        *to = NULL;
    }
    if (*to != NULL && from != NULL)
    {
        (*to)->vtable = (from)->vtable;
        memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
        if ((from)->m_userData != 0)
        {
            FATALERROR("hkpConvexVerticesShape->m_userData is non-0, value %x", (from)->m_userData);
        }
    }
}

hkpConvexVerticesShape* init_hkpConvexVerticesShape(StateTarget target)
{
    FATALERROR("TODO");
    hkpConvexVerticesShape* local;
    if (target == StateTarget::ToGame)
    {
        local = (hkpConvexVerticesShape*)Game::thread_malloc(sizeof(hkpConvexVerticesShape));
    }
    else
    {
        local = (hkpConvexVerticesShape*)malloc_(sizeof(hkpConvexVerticesShape));
    }
    local->vtable = 0x14141c0c0;
    local->m_userData = NULL;
    return local;
}

void free_hkpConvexVerticesShape(hkpConvexVerticesShape* to, StateTarget target)
{
    FATALERROR("TODO");
    if (target == StateTarget::ToGame)
    {
        hkReferencedObject_deref(to);
    }
    else
    {

        free(to);
    }
}

void copy_hkpConvexVerticesConnectivity(hkpConvexVerticesConnectivity** to, hkpConvexVerticesConnectivity* from, StateTarget target)
{
    FATALERROR("TODO");
}

hkpConvexVerticesConnectivity* init_hkpConvexVerticesConnectivity(StateTarget target)
{
    FATALERROR("TODO");
}

void free_hkpConvexVerticesConnectivity(hkpConvexVerticesConnectivity* to, StateTarget target)
{
    FATALERROR("TODO");
}


void copy_hkpConvexTranslateShape(hkpConvexTranslateShape** to, hkpConvexTranslateShape* from, StateTarget target)
{
    if (*to == NULL && from != NULL)
    {
        *to = init_hkpConvexTranslateShape(target);
    }
    if (*to != NULL && from == NULL)
    {
        free_hkpConvexTranslateShape(*to, target);
        *to = NULL;
    }
    if (*to != NULL && from != NULL)
    {
        (*to)->vtable = (from)->vtable;
        memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
        (*to)->data_1 = from->data_1;
        (*to)->vtable2 = (from)->vtable2;
        copy_hkpShape(&(*to)->m_childShape, from->m_childShape, target);
        memcpy((*to)->data_2, (from)->data_2, sizeof((*to)->data_2));
        if ((from)->m_userData != 0)
        {
            FATALERROR("hkpConvexTranslateShape->m_userData is non-0, value %x", (from)->m_userData);
        }
    }
}

hkpConvexTranslateShape* init_hkpConvexTranslateShape(StateTarget target)
{
    hkpConvexTranslateShape* local;
    if (target == StateTarget::ToGame)
    {
        local = (hkpConvexTranslateShape*)Game::thread_malloc(sizeof(hkpConvexTranslateShape));
    }
    else
    {
        local = (hkpConvexTranslateShape*)malloc_(sizeof(hkpConvexTranslateShape));
    }
    local->vtable = 0x14141bff8;
    local->m_userData = NULL;
    local->m_childShape = NULL; //Init on copy
    return local;
}

void free_hkpConvexTranslateShape(hkpConvexTranslateShape* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        if (to->m_childShape != NULL)
        {
            hkReferencedObject_deref((void*)to->m_childShape);
        }
        hkReferencedObject_deref((void*)to);
    }
    else
    {
        free(to);
    }
}


void copy_hkpBoxShape(hkpBoxShape** to, hkpBoxShape* from, StateTarget target)
{
    if (*to == NULL && from != NULL)
    {
        *to = init_hkpBoxShape(target);
    }
    if (*to != NULL && from == NULL)
    {
        free_hkpBoxShape(*to, target);
        *to = NULL;
    }
    if (*to != NULL && from != NULL)
    {
        (*to)->vtable = (from)->vtable;
        memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
        memcpy((*to)->data_1, (from)->data_1, sizeof((*to)->data_1));
        if ((from)->m_userData != 0)
        {
            FATALERROR("hkpBoxShape->m_userData is non-0, value %x", (from)->m_userData);
        }
    }
}

hkpBoxShape* init_hkpBoxShape(StateTarget target)
{
    hkpBoxShape* local;
    if (target == StateTarget::ToGame)
    {
        local = (hkpBoxShape*)Game::thread_malloc(sizeof(hkpBoxShape));
    }
    else
    {
        local = (hkpBoxShape*)malloc_(sizeof(hkpBoxShape));
    }
    local->vtable = 0x14141bff8;
    local->m_userData = NULL;
    return local;
}

void free_hkpBoxShape(hkpBoxShape* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        hkReferencedObject_deref((void*)to);
    }
    else
    {
        free(to);
    }
}

/* ---------------- CHRCTRL ------------------ */

std::string print_hkpCharacterProxy(hkpCharacterProxy* to)
{
    std::string out = "hkpCharacterProxy\n";

    out += "data_0: " + std::to_string(to->data_0) + "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += print_hkpSimpleShapePhantom(to->HkpSimpleShapePhantom);

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

//CharacterProxy is not in hkpWorld, it's handled by game code. So we can save/restore it from the Chr
void copy_hkpCharacterProxy(hkpCharacterProxy* to, const hkpCharacterProxy* from, hkpWorld* world, StateTarget target)
{
    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    size_t index = -1;
    switch (target)
    {
    case StateTarget::ToGame:
        //use the value (actually an index) to get the right shape phantom
        to->HkpSimpleShapePhantom = (hkpSimpleShapePhantom*)world->m_phantoms[(size_t)(from->HkpSimpleShapePhantom)];
        break;
    case StateTarget::ToLocal:
        //save the phantom's index in the m_phantoms list
        for (size_t i = 0; i < world->m_phantoms_size; i++)
        {
            if (world->m_phantoms[i] == from->HkpSimpleShapePhantom)
            {
                index = i;
                break;
            }
        }
        if (index == -1)
        {
            FATALERROR("Unable to locate hkpCharacterProxy's shapePhantom (%p) in m_phantoms", from->HkpSimpleShapePhantom);
        }
        to->HkpSimpleShapePhantom = (hkpSimpleShapePhantom*)index;
        break;
    case StateTarget::Copy:
        to->HkpSimpleShapePhantom = from->HkpSimpleShapePhantom;
        break;
    }
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

hkpCharacterProxy* init_hkpCharacterProxy(StateTarget target)
{
    if (target != StateTarget::ToGame)
    {
        hkpCharacterProxy* local_hkpCharacterProxy = (hkpCharacterProxy*)malloc_(sizeof(hkpCharacterProxy));
        return local_hkpCharacterProxy;
    }
    else
    {
        FATALERROR("in-game alloc hkpCharacterProxy");
    }
}

void free_hkpCharacterProxy(hkpCharacterProxy* to)
{
    free(to);
}
