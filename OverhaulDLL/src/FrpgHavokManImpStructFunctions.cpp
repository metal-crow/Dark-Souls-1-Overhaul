#include "FrpgHavokManImpStructFunctions.h"
#include "Rollback.h"

/* ---------------- CHRCTRL + DAMAGE MAN ------------------ */

std::string print_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to)
{
    std::string out = "hkpSimpleShapePhantom\n";
    return out;
}

void copy_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, const hkpSimpleShapePhantom* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->hkpWorld = from->hkpWorld;
    to->m_userData = from->m_userData;
    copy_hkpLinkedCollidable(&to->m_collidable, &from->m_collidable, to_game);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->unk_0 = from->unk_0;

    if ((to->m_properties_cap & 0x3fffffff) < from->m_properties_len)
    {
        to->m_properties_len = from->m_properties_len;
        increase_list_size(Game::MemHeapAllocator, &to->m_properties, 0x10);
    }
    for (size_t i = 0; i < from->m_properties_len; i++)
    {
        copy_hkpProperty(&to->m_properties[i], &from->m_properties[i]);
    }
    //i guess we don't have to save the listeners?? maybe?
    //void** m_overlapListeners;
    //uint32_t m_overlapListeners_len;
    //uint32_t m_overlapListeners_cap;
    //void** m_phantomListeners;
    //uint32_t m_phantomListeners_len;
    //uint32_t m_phantomListeners_cap;
    //uint64_t padding_0;
    copy_hkMotionState(&to->m_motionState, &from->m_motionState);
    if ((to->m_collisionDetails_cap & 0x3fffffff) < from->m_collisionDetails_len)
    {
        to->m_collisionDetails_len = from->m_collisionDetails_len;
        increase_list_size(Game::MemHeapAllocator, &to->m_collisionDetails, 0x8);
    }
    for (size_t i = 0; i < from->m_collisionDetails_len; i++)
    {
        copy_hkpCollidable(to->m_collisionDetails[i], from->m_collisionDetails[i]);
    }
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
}


hkpSimpleShapePhantom* init_hkpSimpleShapePhantom()
{
    hkpSimpleShapePhantom* local_hkpSimpleShapePhantom = (hkpSimpleShapePhantom*)malloc_(sizeof(hkpSimpleShapePhantom));


    return local_hkpSimpleShapePhantom;
}

void free_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to)
{
    free(to);
}


void copy_hkpLinkedCollidable(hkpLinkedCollidable* to, const hkpLinkedCollidable* from, bool to_game)
{
    copy_hkpCollidable(&to->base, &from->base, to_game);
    if ((to->m_collisionEntries_cap & 0x3fffffff) < from->m_collisionEntries_len)
    {
        to->m_collisionEntries_len = from->m_collisionEntries_len;
        increase_list_size(Game::MemHeapAllocator, &to->m_collisionEntries, 0x10);
    }
    for (size_t i = 0; i < from->m_collisionEntries_len; i++)
    {
        copy_CollisionEntry(&to->m_collisionEntries[i], &from->m_collisionEntries[i]);
    }
}


void copy_hkpProperty(hkpProperty* to, hkpProperty* from)
{
    to->key = from->key;
    to->padding = from->padding;
    copy_hkpCharacterProxy(to->data, from->data);
}


void copy_hkMotionState(hkMotionState* to, const hkMotionState* from)
{
    memcpy(to, from, sizeof(hkMotionState));
}


void copy_hkpCollidable(hkpCollidable* to, const hkpCollidable* from, bool to_game)
{
    if (hkpShape_isSphere(from->m_sph_shape))
    {
        if (!hkpShape_isSphere(to->m_sph_shape))
        {
            FATALERROR(__FUNCTION__" auuuuuuugh fuck me");
        }
        copy_hkpSphereShape(&to->m_sph_shape, (hkpSphereShape**)&from->m_sph_shape, to_game);
    }
    else
    {
        if (hkpShape_isSphere(to->m_sph_shape))
        {
            FATALERROR(__FUNCTION__" auuuuuuugh fuck me");
        }
        copy_hkpCapsuleShape(&to->m_cap_shape, (hkpCapsuleShape**)&from->m_cap_shape, to_game);
    }
    to->m_shapeKey = from->m_shapeKey;
    void* m_motion;
}


bool hkpShape_isSphere(hkpSphereShape* to)
{
    bool sphere = *((uint32_t*)(((uint64_t)to) + 0x10)) == 0x400;
    bool capsule = *((uint32_t*)(((uint64_t)to) + 0x10)) == 0x404;
    if (!sphere && !capsule)
    {
        FATALERROR("%p is not a shape", to);
    }
    return sphere;
}

void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape** from, bool to_game)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_hkpSphereShape(to_game);
    }
    if (*to != NULL && *from == NULL)
    {
        free_hkpSphereShape(*to, to_game);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        memcpy((*to)->data_0, (*from)->data_0, sizeof((*to)->data_0));
    }
}

hkpSphereShape* init_hkpSphereShape(bool to_game)
{
    hkpSphereShape* local;
    if (to_game)
    {
        local = (hkpSphereShape*)Game::thread_malloc(sizeof(hkpSphereShape));
    }
    else
    {
        local = (hkpSphereShape*)malloc_(sizeof(hkpSphereShape));
    }
    return local;
}

void free_hkpSphereShape(hkpSphereShape* to, bool to_game)
{
    if (to_game)
    {
        hkReferencedObject_deref((void*)to);
    }
    else
    {
        free(to);
    }
}


void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape** from, bool to_game)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_hkpCapsuleShape(to_game);
    }
    if (*to != NULL && *from == NULL)
    {
        free_hkpCapsuleShape(*to, to_game);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        memcpy((*to)->data_0, (*from)->data_0, sizeof((*to)->data_0));
    }
}

hkpCapsuleShape* init_hkpCapsuleShape(bool to_game)
{
    hkpCapsuleShape* local;
    if (to_game)
    {
        local = (hkpCapsuleShape*)Game::thread_malloc(sizeof(hkpCapsuleShape));
    }
    else
    {
        local = (hkpCapsuleShape*)malloc_(sizeof(hkpCapsuleShape));
    }
    return local;
}

void free_hkpCapsuleShape(hkpCapsuleShape* to, bool to_game)
{
    if (to_game)
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

void copy_hkpCharacterProxy(hkpCharacterProxy* to, const hkpCharacterProxy* from)
{
    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    copy_hkpSimpleShapePhantom(to->HkpSimpleShapePhantom, from->HkpSimpleShapePhantom);
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

hkpCharacterProxy* init_hkpCharacterProxy()
{
    hkpCharacterProxy* local_hkpCharacterProxy = (hkpCharacterProxy*)malloc_(sizeof(hkpCharacterProxy));

    local_hkpCharacterProxy->HkpSimpleShapePhantom = init_hkpSimpleShapePhantom();

    return local_hkpCharacterProxy;
}

void free_hkpCharacterProxy(hkpCharacterProxy* to)
{
    free_hkpSimpleShapePhantom(to->HkpSimpleShapePhantom);
    free(to);
}
