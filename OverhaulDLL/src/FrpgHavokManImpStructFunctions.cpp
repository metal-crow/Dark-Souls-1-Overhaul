#include "FrpgHavokManImpStructFunctions.h"
#include <unordered_set>

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
    //don't need to actually save/load the simulation islands themselves, those should be stable for the lifetime of this session

    //m_phantoms is complicated by the fact that from and to can have multiple elemnts pointed to the same location.
    //clear out the to array
    std::unordered_set<void*> to_freed_tracker = {};
    for (size_t i = 0; i < to->m_phantoms_size; i++)
    {
        //duplicate pointer, don't double free
        if (to_freed_tracker.contains(to->m_phantoms[i]))
        {
            to->m_phantoms[i] = NULL;
        }
        else
        {
            to_freed_tracker.insert(to->m_phantoms[i]);
            free_hkpPhantom(to->m_phantoms[i], target);
            to->m_phantoms[i] = NULL;
        }
    }
    //resize the to array
    to->m_phantoms_size = from->m_phantoms_size;
    if (to->m_phantoms_cap < from->m_phantoms_size)
    {
        //normally this would be a thread malloc'd object game-side but we've disabled that
        to->m_phantoms = (void**)realloc_(to->m_phantoms, from->m_phantoms_cap * sizeof(void*));
        to->m_phantoms_cap = from->m_phantoms_cap;
    }
    //copy in the from array
    std::unordered_map<void*, size_t> from_dupe_tracker = {};
    for (size_t i = 0; i < from->m_phantoms_size; i++)
    {
        //this is a duplicate pointer, link it up to the to side
        if (from_dupe_tracker.contains(from->m_phantoms[i]))
        {
            to->m_phantoms[i] = to->m_phantoms[from_dupe_tracker[from->m_phantoms[i]]];
        }
        else
        {
            copy_hkpPhantom(&to->m_phantoms[i], from->m_phantoms[i], target);
            from_dupe_tracker[from->m_phantoms[i]] = i;
        }
    }
    //now that the phantoms are copied, link in the collisions
    for (size_t i = 0; i < from->m_phantoms_size; i++)
    {
        PhantomType type = hkpPhantom_getType(from->m_phantoms[i]);
        switch (type)
        {
        case PhantomType::SimpleShape:
            copy_hkpSimpleShapePhantom_collisionDetails((hkpSimpleShapePhantom*)(to->m_phantoms[i]), (const hkpSimpleShapePhantom*)(from->m_phantoms[i]), to, from, target);
            break;
        case PhantomType::Aabb:
            copy_hkpAabbPhantom_collisionDetails((hkpAabbPhantom*)(to->m_phantoms[i]), (const hkpAabbPhantom*)(from->m_phantoms[i]), to, from, target);
            break;
        case PhantomType::PhantomNull:
        case PhantomType::InvalidPhantom:
            break;
        }
    }
    //this can only be done after the m_phantoms are all copied
    copy_hkp3AxisSweep(to->m_broadPhase, from->m_broadPhase, to, from, target);
}

hkpWorld* init_hkpWorld()
{
    hkpWorld* local = (hkpWorld*)malloc_(sizeof(hkpWorld));

    local->m_fixedIsland = NULL;
    local->m_fixedRigidBody = NULL;
    local->m_activeSimulationIslands = NULL;
    local->m_inactiveSimulationIslands = NULL;
    local->m_dirtySimulationIslands = NULL;
    local->m_broadPhase = init_hkp3AxisSweep();
    local->m_phantoms = NULL;

    return local;
}

void free_hkpWorld(hkpWorld* to)
{
    free_hkp3AxisSweep(to->m_broadPhase);
    // copy_hkpWorld may store the same pointer in multiple slots when the source has
    // duplicate entries, so track what we've already freed to avoid double-free.
    std::unordered_set<void*> freed;
    for (size_t i = 0; i < to->m_phantoms_cap; i++)
    {
        if (to->m_phantoms[i] != NULL && !freed.count(to->m_phantoms[i]))
        {
            freed.insert(to->m_phantoms[i]);
            free_hkpPhantom(to->m_phantoms[i], StateTarget::ToLocal);
        }
    }
    free(to->m_phantoms);
    free(to);
}

void copy_hkp3AxisSweep(hkp3AxisSweep* to, const hkp3AxisSweep* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target)
{
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    if ((from->m_nodes_cap & 0x3fffffff) < from->m_nodes_len)
    {
        FATALERROR("m_nodes_cap %x m_nodes_len %x", from->m_nodes_cap, from->m_nodes_len);
    }
    //update the array object and capacity
    if ((to->m_nodes_cap & 0x3fffffff) < from->m_nodes_len)
    {
        uint32_t old_len = to->m_nodes_len;
        //normally this would be a thread malloc'd object game-side but we've disabled that
        to->m_nodes = (hkpBpNode*)realloc_(to->m_nodes, (from->m_nodes_cap & 0x3fffffff) * sizeof(hkpBpNode));
        to->m_nodes_cap = from->m_nodes_cap;
        // Zero out new nodes
        for (size_t i = old_len; i < (to->m_nodes_cap & 0x3fffffff); i++)
        {
            memset(&to->m_nodes[i], 0, sizeof(hkpBpNode));
        }
    }
    to->m_nodes_len = from->m_nodes_len;
    for (size_t i = 0; i < to->m_nodes_len; i++)
    {
        copy_hkpBpNode(&to->m_nodes[i], &from->m_nodes[i], target);
    }
    //need to correct the pointer address itself to point to the entity's id. We need to point at the id directly since havok uses *(index_in_array+4) to get the m_type
    if (target == StateTarget::ToGame)
    {
        //check the phantoms list to link up any phantom nodes 
        for (size_t i = 0; i < to_world->m_phantoms_size; i++)
        {
            void* phantom = to_world->m_phantoms[i];
            switch (hkpPhantom_getType(phantom))
            {
            case PhantomType::SimpleShape:
            {
                hkpSimpleShapePhantom* ssp = (hkpSimpleShapePhantom*)phantom;
                uint32_t ssid = ssp->m_collidable.base.m_broadPhaseHandle.m_id;
                to->m_nodes[ssid].index_in_array = &ssp->m_collidable.base.m_broadPhaseHandle.m_id;
            }
            break;
            case PhantomType::Aabb:
            {
                hkpAabbPhantom* aap = (hkpAabbPhantom*)phantom;
                uint32_t aaid = aap->m_collidable.base.m_broadPhaseHandle.m_id;
                to->m_nodes[aaid].index_in_array = &aap->m_collidable.base.m_broadPhaseHandle.m_id;
            }
            break;
            default:
                break;
            }
        }

        //check the fixed entities list
        //since this list is fixed and thus we don't save it, if this is a fixed entity just copy the pointer to the game-side data
        for (size_t i = 0; i < to_world->m_fixedIsland->m_entities_size; i++)
        {
            void* elem_p = to_world->m_fixedIsland->m_entities[i];
            uint32_t* elem_id_ptr = (uint32_t*)((uint64_t)elem_p + 0x44);
            to->m_nodes[*elem_id_ptr].index_in_array = elem_id_ptr;
        }

        //check the active entities list
        //same deal as fixed entities list, it should be stable for our session
        for (size_t sim_i = 0; sim_i < to_world->m_activeSimulationIslands_size; sim_i++)
        {
            for (size_t i = 0; i < to_world->m_activeSimulationIslands[sim_i]->m_entities_size; i++)
            {
                void* elem_p = to_world->m_activeSimulationIslands[sim_i]->m_entities[i];
                uint32_t* elem_id_ptr = (uint32_t*)((uint64_t)elem_p + 0x44);
                to->m_nodes[*elem_id_ptr].index_in_array = elem_id_ptr;
            }
        }

        for (size_t sim_i = 0; sim_i < to_world->m_inactiveSimulationIslands_size; sim_i++)
        {
            for (size_t i = 0; i < to_world->m_inactiveSimulationIslands[sim_i]->m_entities_size; i++)
            {
                void* elem_p = to_world->m_inactiveSimulationIslands[sim_i]->m_entities[i];
                uint32_t* elem_id_ptr = (uint32_t*)((uint64_t)elem_p + 0x44);
                to->m_nodes[*elem_id_ptr].index_in_array = elem_id_ptr;
            }
        }

        //skip checking first element
        for (size_t i = 1; i < to->m_nodes_len; i++)
        {
            if (to->m_nodes[i].index_in_array == NULL)
            {
                FATALERROR("ToGame hkp3AxisSweep->m_nodes[%d] (%p) has null index_in_array", i, to->m_nodes[i].index_in_array);
            }
        }
    }
    if (target == StateTarget::ToLocal)
    {
        //skip checking first element
        for (size_t i = 1; i < from->m_nodes_len; i++)
        {
            if (from->m_nodes[i].index_in_array == NULL)
            {
                FATALERROR("ToLocal hkp3AxisSweep->m_nodes[%d] (%p) has null index_in_array", i, to->m_nodes[i].index_in_array);
            }
        }
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
        // index_in_array is a pointer to a different entity, not a allocation — do not free it
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
    to->index_in_array = NULL; //set actual value of index_to_array in the caller
}

void copy_hkpBpAxis(hkpBpAxis* to, const hkpBpAxis* from, StateTarget target)
{
    if ((from->capacity & 0x3fffffff) < from->len)
    {
        FATALERROR("hkpBpAxis cap %x hkpBpAxis len %x", from->capacity, from->len);
    }
    if ((to->capacity & 0x3fffffff) < from->len)
    {
        uint32_t old_len = to->len;
        //normally this would be a thread malloc'd object game-side but we've disabled that
        to->arry = (hkpBpEndPoint*)realloc_(to->arry, (from->capacity & 0x3fffffff) * sizeof(hkpBpEndPoint));
        to->capacity = from->capacity;
        // Zero out new nodes
        for (size_t i = old_len; i < (to->capacity & 0x3fffffff); i++)
        {
            memset(&to->arry[i], 0, sizeof(hkpBpEndPoint));
        }
    }
    to->len = from->len;
    for (size_t i = 0; i < from->len; i++)
    {
        copy_hkpBpEndPoint(&to->arry[i], &from->arry[i], target);
    }
}

void copy_hkpBpEndPoint(hkpBpEndPoint* to, const hkpBpEndPoint* from, StateTarget target)
{
    to->m_value = from->m_value;
    to->m_nodeIndex = from->m_nodeIndex;
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
    if (*to != NULL)
    {
        FATALERROR("Called copy_hkpPhantom with non-null to");
    }

    PhantomType type = hkpPhantom_getType(from);
    switch (type)
    {
    case PhantomType::SimpleShape:
        *to = init_hkpSimpleShapePhantom(target);
        copy_hkpSimpleShapePhantom((hkpSimpleShapePhantom*)(*to), (const hkpSimpleShapePhantom*)from, target);
        break;
    case PhantomType::Aabb:
        *to = init_hkpAabbPhantom(target);
        copy_hkpAabbPhantom((hkpAabbPhantom*)(*to), (const hkpAabbPhantom*)from, target);
        break;
    case PhantomType::PhantomNull:
        *to = NULL;
        break;
    case PhantomType::InvalidPhantom:
        FATALERROR("%p is not a valid phantom for copy (vtable %llx)", from, *(uint64_t*)from);
        break;
    }
    return;
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
        free_hkpAabbPhantom((hkpAabbPhantom*)to, target);
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

    if ((from->m_properties_cap & 0x3fffffff) < from->m_properties_len)
    {
        FATALERROR("m_properties_cap %x m_properties_len %x", from->m_properties_cap, from->m_properties_len);
    }
    if ((to->m_properties_cap & 0x3fffffff) < from->m_properties_len)
    {
        uint32_t old_len = to->m_properties_len;
        //normally this would be a thread malloc'd object game-side but we've disabled that
        to->m_properties = (hkpProperty*)realloc_(to->m_properties, (from->m_properties_cap & 0x3fffffff) * sizeof(hkpProperty));
        to->m_properties_cap = (from->m_properties_cap & 0x3fffffff);

        for (size_t i = old_len; i < (from->m_properties_cap & 0x3fffffff); i++)
        {
            init_hkpProperty(&to->m_properties[i], target);
        }
    }
    to->m_properties_len = from->m_properties_len;
    for (size_t i = 0; i < from->m_properties_len; i++)
    {
        copy_hkpProperty(&to->m_properties[i], &from->m_properties[i], target);
    }

    copy_hkMotionState(&to->m_motionState, &from->m_motionState);

    if ((from->m_collisionDetails_cap & 0x3fffffff) < from->m_collisionDetails_len)
    {
        FATALERROR("m_collisionDetails_cap %x m_collisionDetails_len %x", from->m_collisionDetails_cap, from->m_collisionDetails_len);
    }
    if ((to->m_collisionDetails_cap & 0x3fffffff) < from->m_collisionDetails_len)
    {
        //normally this would be a thread malloc'd object game-side but we've disabled that
        to->m_collisionDetails = (hkpCollidable**)realloc_(to->m_collisionDetails, (from->m_collisionDetails_cap & 0x3fffffff) * sizeof(hkpCollidable*));
        to->m_collisionDetails_cap = (from->m_collisionDetails_cap & 0x3fffffff);
    }
    to->m_collisionDetails_len = from->m_collisionDetails_len;
    //we can't copy the collision details here, since it may rely on all the other phantoms existing. Split out into a seperate function to call later when ready

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}

void copy_hkpSimpleShapePhantom_collisionDetails(hkpSimpleShapePhantom* to, const hkpSimpleShapePhantom* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target)
{
    for (size_t i = 0; i < from->m_collisionDetails_len; i++)
    {
        //this collision points to the colliding entities's m_collidable
        //we either already copy it as part of the other entity's data, or the entity is stable for our lifetime. Just locate it and use the pointer here
        hkpCollidable* collision = from->m_collisionDetails[i];
        if (collision == NULL)
        {
            to->m_collisionDetails[i] = NULL;
            continue;
        }
        bool found_colliding = false;

        //check the phantoms list to see if this is a phantom collision, and get the offset into the 'from' array for it
        //then link it up with the 'to' side phantom's collision
        //do this since we save the m_phantoms list, and it's order may change
        size_t j = 0;
        while (j < from_world->m_phantoms_size && !found_colliding)
        {
            void* elem_p = from_world->m_phantoms[j];
            PhantomType type = hkpPhantom_getType(elem_p);
            switch (type)
            {
            case PhantomType::SimpleShape:
            case PhantomType::Aabb:
                //linked collidable in hkpPhantom is +0x20
                if (((uint64_t)elem_p + 0x20) == (uint64_t)collision)
                {
                    //get the associated phantom pointer in the target array
                    void* colliding_phantom_to_addr = to_world->m_phantoms[j];
                    if (colliding_phantom_to_addr != NULL)
                    {
                        hkpCollidable* colliding_phantom_collision_to_addr = (hkpCollidable*)((uint64_t)colliding_phantom_to_addr + 0x20);
                        to->m_collisionDetails[i] = colliding_phantom_collision_to_addr;
                    }
                    else
                    {
                        FATALERROR("Found colliding phantom at %d but it was null on the 'to' end: %p(%d) %p", j, collision, i, from);
                    }
                    found_colliding = true;
                }
                break;
            case PhantomType::PhantomNull:
                break;
            case PhantomType::InvalidPhantom:
                FATALERROR("Got invalid phantom for %p", elem_p);
                break;
            }
            j++;
        }
        if (found_colliding)
        {
            continue;
        }

        //check the fixed entities list
        //since this list is fixed and thus we don't save it, if this is a fixed entity just copy the pointer to the game-side data
        j = 0;
        hkpSimulationIsland* fixedIslandtarget = NULL;
        switch (target)
        {
        case StateTarget::ToGame:
            fixedIslandtarget = to_world->m_fixedIsland;
            break;
        case StateTarget::ToLocal:
            fixedIslandtarget = from_world->m_fixedIsland;
            break;
        case StateTarget::Copy:
            //no way to actual verify if this is a fixed entity here, so just copy it
            to->m_collisionDetails[i] = from->m_collisionDetails[i];
            found_colliding = true;
            break;
        }
        while (fixedIslandtarget != NULL && j < fixedIslandtarget->m_entities_size && !found_colliding)
        {
            void* elem_p = fixedIslandtarget->m_entities[j];
            if (((uint64_t)elem_p + 0x20) == (uint64_t)collision)
            {
                to->m_collisionDetails[i] = from->m_collisionDetails[i];
                found_colliding = true;
            }
            j++;
        }
        if (found_colliding)
        {
            continue;
        }

        //check the active entities list
        //same deal as fixed entities list, it should be stable for our session
        size_t sim_i = 0;
        const hkpWorld* activeWorldtarget = NULL;
        switch (target)
        {
        case StateTarget::ToGame:
            activeWorldtarget = to_world;
            break;
        case StateTarget::ToLocal:
            activeWorldtarget = from_world;
            break;
        case StateTarget::Copy:
            //no way to actual verify if this is a active entity here, so just copy it
            to->m_collisionDetails[i] = from->m_collisionDetails[i];
            found_colliding = true;
            break;
        }
        while (activeWorldtarget != NULL && sim_i < activeWorldtarget->m_activeSimulationIslands_size && !found_colliding)
        {
            j = 0;
            while (activeWorldtarget != NULL && j < activeWorldtarget->m_activeSimulationIslands[sim_i]->m_entities_size && !found_colliding)
            {
                void* elem_p = activeWorldtarget->m_activeSimulationIslands[sim_i]->m_entities[j];
                if (((uint64_t)elem_p + 0x20) == (uint64_t)collision)
                {
                    to->m_collisionDetails[i] = from->m_collisionDetails[i];
                    found_colliding = true;
                }
                j++;
            }
            sim_i++;
        }
        if (found_colliding)
        {
            continue;
        }

        if (!found_colliding)
        {
            FATALERROR("Unable to find phantom for collision %p %p", collision, from);
        }
    }
}

//all of it's children arrays are unknown sized, so should be init'd on copy
hkpSimpleShapePhantom* init_hkpSimpleShapePhantom(StateTarget target)
{
    hkpSimpleShapePhantom* local_hkpSimpleShapePhantom = NULL;
    //normally this would be a thread malloc'd object game-side but we've disabled that
    local_hkpSimpleShapePhantom = (hkpSimpleShapePhantom*)malloc_(sizeof(hkpSimpleShapePhantom));
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
        free(to->m_collisionDetails);
        free(to);
    }
}

hkpSimpleShapePhantom* get_hkpSimpleShapePhantom(const hkpWorld* world, FrpgPhysShapePhantomIns* parent)
{
    auto ret = find_hkpSimpleShapePhantom(world, parent);
    if (ret == NULL)
    {
        FATALERROR("Could not find shape phantom for parent %p", parent);
    }
    return ret;
}

hkpSimpleShapePhantom* find_hkpSimpleShapePhantom(const hkpWorld* world, FrpgPhysShapePhantomIns* parent)
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

bool world_contains_phantom(const hkpWorld* world, void* phantom)
{
    void** array = world->m_phantoms;
    for (size_t i = 0; i < world->m_phantoms_size; i++)
    {
        void* elem = array[i];
        if (elem == phantom){
            return true;
        }
    }
    return false;
}


void copy_hkpAabbPhantom(hkpAabbPhantom* to, const hkpAabbPhantom* from, StateTarget target)
{
    to->vtable = from->vtable;
    to->data_0 = from->data_0;
    to->hkpWorldPtr = from->hkpWorldPtr;
    to->m_userData = from->m_userData;
    copy_hkpLinkedCollidable(&to->m_collidable, &from->m_collidable, NULL, target);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->unk_0 = from->unk_0;

    if ((from->m_properties_cap & 0x3fffffff) < from->m_properties_len)
    {
        FATALERROR("m_properties_cap %x m_properties_len %x", from->m_properties_cap, from->m_properties_len);
    }
    if ((to->m_properties_cap & 0x3fffffff) < from->m_properties_len)
    {
        uint32_t old_len = to->m_properties_len;
        //normally this would be a thread malloc'd object game-side but we've disabled that
        to->m_properties = (hkpProperty*)realloc_(to->m_properties, (from->m_properties_cap & 0x3fffffff) * sizeof(hkpProperty));
        to->m_properties_cap = (from->m_properties_cap & 0x3fffffff);

        for (size_t i = old_len; i < (from->m_properties_cap & 0x3fffffff); i++)
        {
            init_hkpProperty(&to->m_properties[i], target);
        }
    }
    to->m_properties_len = from->m_properties_len;
    for (size_t i = 0; i < from->m_properties_len; i++)
    {
        copy_hkpProperty(&to->m_properties[i], &from->m_properties[i], target);
    }

    to->vtable2 = from->vtable2;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));

    if ((from->m_overlappingCollidables_cap & 0x3fffffff) < from->m_overlappingCollidables_len)
    {
        FATALERROR("m_overlappingCollidables_cap %x m_overlappingCollidables_len %x", from->m_overlappingCollidables_cap, from->m_overlappingCollidables_len);
    }
    if ((to->m_overlappingCollidables_cap & 0x3fffffff) < from->m_overlappingCollidables_len)
    {
        //normally this would be a thread malloc'd object game-side but we've disabled that
        to->m_overlappingCollidables = (hkpCollidable**)realloc_(to->m_overlappingCollidables, (from->m_overlappingCollidables_cap & 0x3fffffff) * sizeof(hkpCollidable*));
        to->m_overlappingCollidables_cap = (from->m_overlappingCollidables_cap & 0x3fffffff);
    }
    to->m_overlappingCollidables_len = from->m_overlappingCollidables_len;
    //we can't copy the collision details here, since it may rely on all the other phantoms existing. Split out into a seperate function to call later when ready
}

void copy_hkpAabbPhantom_collisionDetails(hkpAabbPhantom* to, const hkpAabbPhantom* from, const hkpWorld* to_world, const hkpWorld* from_world, StateTarget target)
{
    for (size_t i = 0; i < from->m_overlappingCollidables_len; i++)
    {
        //this collision points to the colliding entities's m_collidable
        //we either already copy it as part of the other entity's data, or the entity is stable for our lifetime. Just locate it and use the pointer here
        hkpCollidable* collision = from->m_overlappingCollidables[i];
        if (collision == NULL)
        {
            to->m_overlappingCollidables[i] = NULL;
            continue;
        }
        bool found_colliding = false;

        //check the phantoms list to see if this is a phantom collision, and get the offset into the 'from' array for it
        //then link it up with the 'to' side phantom's collision
        //do this since we save the m_phantoms list, and it's order may change
        size_t j = 0;
        while (j < from_world->m_phantoms_size && !found_colliding)
        {
            void* elem_p = from_world->m_phantoms[j];
            PhantomType type = hkpPhantom_getType(elem_p);
            switch (type)
            {
            case PhantomType::SimpleShape:
            case PhantomType::Aabb:
                //linked collidable in hkpPhantom is +0x20
                if (((uint64_t)elem_p + 0x20) == (uint64_t)collision)
                {
                    //get the associated phantom pointer in the target array
                    void* colliding_phantom_to_addr = to_world->m_phantoms[j];
                    if (colliding_phantom_to_addr != NULL)
                    {
                        hkpCollidable* colliding_phantom_collision_to_addr = (hkpCollidable*)((uint64_t)colliding_phantom_to_addr + 0x20);
                        to->m_overlappingCollidables[i] = colliding_phantom_collision_to_addr;
                    }
                    else
                    {
                        FATALERROR("Found colliding phantom at %d but it was null on the 'to' end: %p(%d) %p", j, collision, i, from);
                    }
                    found_colliding = true;
                }
                break;
            case PhantomType::PhantomNull:
                break;
            case PhantomType::InvalidPhantom:
                FATALERROR("Got invalid phantom for %p", elem_p);
                break;
            }
            j++;
        }
        if (found_colliding)
        {
            continue;
        }

        //check the fixed entities list
        //since this list is fixed and thus we don't save it, if this is a fixed entity just copy the pointer to the game-side data
        j = 0;
        hkpSimulationIsland* fixedIslandtarget = NULL;
        switch (target)
        {
        case StateTarget::ToGame:
            fixedIslandtarget = to_world->m_fixedIsland;
            break;
        case StateTarget::ToLocal:
            fixedIslandtarget = from_world->m_fixedIsland;
            break;
        case StateTarget::Copy:
            //no way to actual verify if this is a fixed entity here, so just copy it
            to->m_overlappingCollidables[i] = from->m_overlappingCollidables[i];
            found_colliding = true;
            break;
        }
        while (fixedIslandtarget != NULL && j < fixedIslandtarget->m_entities_size && !found_colliding)
        {
            void* elem_p = fixedIslandtarget->m_entities[j];
            if (((uint64_t)elem_p + 0x20) == (uint64_t)collision)
            {
                to->m_overlappingCollidables[i] = from->m_overlappingCollidables[i];
                found_colliding = true;
            }
            j++;
        }
        if (found_colliding)
        {
            continue;
        }

        //check the active entities list
        //same deal as fixed entities list, it should be stable for our session
        size_t sim_i = 0;
        const hkpWorld* activeWorldtarget = NULL;
        switch (target)
        {
        case StateTarget::ToGame:
            activeWorldtarget = to_world;
            break;
        case StateTarget::ToLocal:
            activeWorldtarget = from_world;
            break;
        case StateTarget::Copy:
            //no way to actual verify if this is a active entity here, so just copy it
            to->m_overlappingCollidables[i] = from->m_overlappingCollidables[i];
            found_colliding = true;
            break;
        }
        while (activeWorldtarget != NULL && sim_i < activeWorldtarget->m_activeSimulationIslands_size && !found_colliding)
        {
            j = 0;
            while (activeWorldtarget != NULL && j < activeWorldtarget->m_activeSimulationIslands[sim_i]->m_entities_size && !found_colliding)
            {
                void* elem_p = activeWorldtarget->m_activeSimulationIslands[sim_i]->m_entities[j];
                if (((uint64_t)elem_p + 0x20) == (uint64_t)collision)
                {
                    to->m_overlappingCollidables[i] = from->m_overlappingCollidables[i];
                    found_colliding = true;
                }
                j++;
            }
            sim_i++;
        }
        if (found_colliding)
        {
            continue;
        }

        if (!found_colliding)
        {
            FATALERROR("Unable to find phantom for collision %p %p", collision, from);
        }
    }
}

hkpAabbPhantom* init_hkpAabbPhantom(StateTarget target)
{
    hkpAabbPhantom* local_hkpAabbPhantom = NULL;
    //normally this would be a thread malloc'd object game-side but we've disabled that
    local_hkpAabbPhantom = (hkpAabbPhantom*)malloc_(sizeof(hkpAabbPhantom));
    local_hkpAabbPhantom->vtable = (void*)0x14145cb08;
    //don't know if we need a cap or a sphere, so init on copy
    local_hkpAabbPhantom->m_collidable.base.shape = NULL;

    //we don't know how big these should be, so they are init'd on copy
    local_hkpAabbPhantom->m_collidable.base.m_boundingVolumeData.m_childShapeAabbs = NULL;
    local_hkpAabbPhantom->m_collidable.base.m_boundingVolumeData.m_childShapeKeys = NULL;
    local_hkpAabbPhantom->m_collidable.base.m_boundingVolumeData.m_numChildShapeAabbs = 0;
    local_hkpAabbPhantom->m_collidable.base.m_boundingVolumeData.m_capacityChildShapeAabbs = 0;

    local_hkpAabbPhantom->m_collidable.m_collisionEntries = NULL;
    local_hkpAabbPhantom->m_collidable.m_collisionEntries_len = 0;
    local_hkpAabbPhantom->m_collidable.m_collisionEntries_cap = 0;

    local_hkpAabbPhantom->m_properties = NULL;
    local_hkpAabbPhantom->m_properties_len = 0;
    local_hkpAabbPhantom->m_properties_cap = 0;

    local_hkpAabbPhantom->m_overlappingCollidables = NULL;
    local_hkpAabbPhantom->m_overlappingCollidables_len = 0;
    local_hkpAabbPhantom->m_overlappingCollidables_cap = 0;

    return local_hkpAabbPhantom;
}

void free_hkpAabbPhantom(hkpAabbPhantom* to, StateTarget target)
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
        free(to->m_overlappingCollidables);
        free(to);
    }
}

/* ---------------- CHRCTRL + DAMAGE MAN ------------------ */

void copy_hkpLinkedCollidable(hkpLinkedCollidable* to, const hkpLinkedCollidable* from, hkMotionState* motion, StateTarget target)
{
    copy_hkpCollidable(&to->base, &from->base, motion, target);
    if (from->m_collisionEntries_len > 0)
    {
        FATALERROR("hkpLinkedCollidable->m_collisionEntries_len > 0");
    }
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
    if (to != NULL)
    {
        free_hkpShape(to->shape, StateTarget::ToLocal);
        free_BoundingVolumeData(&to->m_boundingVolumeData, StateTarget::ToLocal);
        free(to);
    }
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
            *to = NULL;
        }
        *to = from;
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
        hkReferencedObject_deref((void*)to);
    }
    else
    {
        free(to);
    }
}


void copy_hkpMoppBvTreeShape(hkpMoppBvTreeShape** to, hkpMoppBvTreeShape* from, StateTarget target)
{
    //if (*to == NULL && from != NULL)
    //{
    //    *to = init_hkpMoppBvTreeShape(target);
    //}
    //if (*to != NULL && from == NULL)
    //{
    //    free_hkpMoppBvTreeShape(*to, target);
    //    *to = NULL;
    //}
    //if (*to != NULL && from != NULL)
    //{
    //    (*to)->vtable1 = (from)->vtable1;
    //    memcpy((*to)->data_0, (from)->data_0, sizeof((*to)->data_0));
    //    if ((from)->m_userData != 0)
    //    {
    //        FATALERROR("hkpMoppBvTreeShape->m_userData is non-0, value %x", (from)->m_userData);
    //    }
    //    (*to)->data_1 = (from)->data_1;
    //    copy_hkpMoppCode((*to)->m_code, (from)->m_code, target);
    //    (*to)->m_moppData = (*to)->m_code->m_data;
    //    (*to)->m_moppData_size = (*to)->m_code->m_data_len;
    //    memcpy((*to)->data_2, (from)->data_2, sizeof((*to)->data_2));
    //    (*to)->vtable2 = (from)->vtable2;
    //    copy_hkpShape(&(*to)->m_childShape, from->m_childShape, target);
    //    memcpy((*to)->data_3, (from)->data_3, sizeof((*to)->data_3));
    //}
}

hkpMoppBvTreeShape* init_hkpMoppBvTreeShape(StateTarget target)
{
    //hkpMoppBvTreeShape* local;
    ////normally this would be a thread malloc'd object game-side but we've disabled that
    //local = (hkpMoppBvTreeShape*)malloc_(sizeof(hkpMoppBvTreeShape));
    //local->vtable1 = 0x14141bd68;
    //local->m_userData = NULL;
    //local->m_code = init_hkpMoppCode(target);
    //local->m_moppData = local->m_code->m_data;
    //local->m_moppData_size = local->m_code->m_data_len;
    //local->m_childShape = NULL; //Init on copy
    //return local;
    return NULL;
}

void free_hkpMoppBvTreeShape(hkpMoppBvTreeShape* to, StateTarget target)
{
    //free_hkpMoppCode(to->m_code, target);
    //free_hkpShape(to->m_childShape, target);
    //if (target == StateTarget::ToGame)
    //{
    //    hkReferencedObject_deref(to);
    //}
    //else
    //{
    //    free(to);
    //}
}

void copy_hkpMoppCode(hkpMoppCode* to, hkpMoppCode* from, StateTarget target)
{
    to->vtable = from->vtable;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    if ((from->m_data_cap & 0x3fffffff) < from->m_data_len)
    {
        FATALERROR("m_data_cap %x m_data_len %x", from->m_data_cap, from->m_data_len);
    }
    if ((to->m_data_cap & 0x3fffffff) < from->m_data_len)
    {
        uint32_t old_len = to->m_data_len;
        to->m_data = (uint8_t*)realloc_(to->m_data, (from->m_data_cap & 0x3fffffff) * sizeof(uint8_t));
        to->m_data_cap = from->m_data_cap;
        // Zero out new values
        for (size_t i = old_len; i < (to->m_data_cap & 0x3fffffff); i++)
        {
            to->m_data[i] = 0;
        }
    }
    to->m_data_len = from->m_data_len;
    for (size_t i = 0; i < to->m_data_len; i++)
    {
        to->m_data[i] = from->m_data[i];
    }
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

hkpMoppCode* init_hkpMoppCode(StateTarget target)
{
    hkpMoppCode* local;
    //normally this would be a thread malloc'd object game-side but we've disabled that
    local = (hkpMoppCode*)malloc_(sizeof(hkpMoppCode));
    local->vtable = 0x141460138;
    local->m_data = NULL; //init on copy
    local->m_data_len = 0;
    local->m_data_cap = 0;
    return local;
}

void free_hkpMoppCode(hkpMoppCode* to, StateTarget target)
{
    if (target == StateTarget::ToGame)
    {
        hkReferencedObject_deref(to);
    }
    else
    {
        free(to->m_data);
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
    //normally this would be a thread malloc'd object game-side but we've disabled that
    local = (hkpConvexVerticesShape*)malloc_(sizeof(hkpConvexVerticesShape));
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
    //normally this would be a thread malloc'd object game-side but we've disabled that
    local = (hkpConvexTranslateShape*)malloc_(sizeof(hkpConvexTranslateShape));
    local->vtable = 0x14141bff8;
    local->m_userData = NULL;
    local->m_childShape = NULL; //Init on copy
    return local;
}

void free_hkpConvexTranslateShape(hkpConvexTranslateShape* to, StateTarget target)
{
    free_hkpShape(to->m_childShape, target);
    if (target == StateTarget::ToGame)
    {
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
    //normally this would be a thread malloc'd object game-side but we've disabled that
    local = (hkpBoxShape*)malloc_(sizeof(hkpBoxShape));
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
