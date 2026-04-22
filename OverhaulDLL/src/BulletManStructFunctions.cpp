#include "BulletManStructFunctions.h"
#include "SfxManStructFunctions.h"
#include "Rollback.h"

static const size_t ChrExFollowCam_size = 0x340;
static const size_t ChrAimCam_size = 0x1d0;
static const size_t FallDieCam_size = 0xb0;
static const size_t BallistaAimCam_size = 0xe0;

// ---- BulletIns_FollowupBullet ----

void copy_BulletIns_FollowupBullet_Data(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, StateTarget target)
{
    to->vtable = from->vtable;
    to->FXManager = from->FXManager;
    to->fxentry_a = NULL;// from->fxentry_a;
    to->fxentry_b = NULL;// from->fxentry_b;
}

void copy_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, StateTarget target)
{
    copy_BulletIns_FollowupBullet_Data(to, from, target);
    //next/prev are linked list pointers into the SFX system or other FollowupBullets.
    //Since the SFX system is saved/restored independently and game objects are at fixed addresses,
    //copying the raw pointer values is correct for rollback.
    to->next = from->next;
    to->prev = from->prev;
}

void copy_BulletIns_FollowupBullet_List(BulletIns_FollowupBullet** to_list, int16_t* to_len, BulletIns_FollowupBullet** from_list, int16_t* from_len, StateTarget target)
{
    *to_list = (BulletIns_FollowupBullet*)realloc_(*to_list, sizeof(BulletIns_FollowupBullet) * (*from_len));
    for (int16_t i = 0; i < *from_len; i++)
    {
        copy_BulletIns_FollowupBullet_Data(&(*to_list)[i], &(*from_list)[i], target);
        //translate next/prev pointers within the list
        if ((*from_list)[i].next != NULL)
        {
            size_t from_next_offset = ((uint64_t)(*from_list)[i].next) - ((uint64_t)(*from_list));
            (*to_list)[i].next = (BulletIns_FollowupBullet*)(((uint64_t)(*to_list)) + from_next_offset);
        }
        else
        {
            (*to_list)[i].next = NULL;
        }
        if ((*from_list)[i].prev != NULL)
        {
            size_t from_prev_offset = ((uint64_t)(*from_list)[i].prev) - ((uint64_t)(*from_list));
            (*to_list)[i].prev = (BulletIns_FollowupBullet*)(((uint64_t)(*to_list)) + from_prev_offset);
        }
        else
        {
            (*to_list)[i].prev = NULL;
        }
    }
    *to_len = *from_len;
}

// ---- BulletIns ----

void copy_BulletIns(BulletIns* to, BulletIns* from, StateTarget target)
{
    to->vtable = from->vtable;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    copy_BulletIns_FollowupBullet(&to->FollowupBullet, &from->FollowupBullet, target);

    to->data_1 = from->data_1;

    //owner contains entity IDs, attack data, position vectors - all safe to memcpy
    memcpy(&to->owner, &from->owner, sizeof(to->owner));

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));

    //bulletTargetingSystemOwner contains parent ptr, vtable, entityId, NpcThinkParam - raw pointers are fine
    memcpy(&to->bulletTargetingSystemOwner, &from->bulletTargetingSystemOwner, sizeof(to->bulletTargetingSystemOwner));

    //targetingSystemBase contains vtable, owner ptr, targeting data - raw pointers are fine
    memcpy(&to->targetingSystemBase, &from->targetingSystemBase, sizeof(to->targetingSystemBase));

    memcpy(to->data_3, from->data_3, sizeof(to->data_3));

    memcpy(&to->bulletState, &from->bulletState, sizeof(to->bulletState));
    to->data_4 = from->data_4;
    memcpy(&to->bulletFlyState, &from->bulletFlyState, sizeof(to->bulletFlyState));
    memcpy(&to->bulletExplosionState, &from->bulletExplosionState, sizeof(to->bulletExplosionState));

    //previous_bullet_in_use is handled by copy_BulletMan after all elements are copied
    to->data_5 = from->data_5;
}

BulletIns* init_BulletIns()
{
    BulletIns* local = (BulletIns*)malloc_(sizeof(BulletIns));
    return local;
}

void free_BulletIns(BulletIns* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}

// ---- BulletMan_Field0x20 ----

void copy_BulletMan_Field0x20(BulletMan_Field0x20* to, BulletMan_Field0x20* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));

    //BulletParamInfo is game-allocated and never freed. Since this object is linked to the lifetime of BulletMan itself, it's stable
    to->BulletParamInfo = from->BulletParamInfo;

    memcpy(&to->field0x1a0, &from->field0x1a0, sizeof(to->field0x1a0));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));

    //next_in_use is a linked list pointer within the field0x20 array - handled by copy_BulletMan
    //chrCam is a raw pointer to the game ChrCam
    to->chrCam = from->chrCam;
    //dbgNode is a raw pointer
    to->dbgNode = from->dbgNode;
}

BulletMan_Field0x20* init_BulletMan_Field0x20()
{
    BulletMan_Field0x20* local = (BulletMan_Field0x20*)malloc_(sizeof(BulletMan_Field0x20));
    return local;
}

void free_BulletMan_Field0x20(BulletMan_Field0x20* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}

// ---- BulletMan_Field0x40 ----

void copy_BulletMan_Field0x40(BulletMan_Field0x40* to, BulletMan_Field0x40* from, StateTarget target)
{
    to->bulletOwnerEntityId = from->bulletOwnerEntityId;
    to->padding_0 = from->padding_0;

    for (size_t i = 0; i < 16; i++)
    {
        copy_BulletIns_FollowupBullet_Data(&to->arry[i], &from->arry[i], target);
        //translate next/prev pointers within the arry
        if (from->arry[i].next != NULL)
        {
            size_t from_next_offset = ((uint64_t)from->arry[i].next) - ((uint64_t)(from->arry));
            to->arry[i].next = (BulletIns_FollowupBullet*)(((uint64_t)(to->arry)) + from_next_offset);
        }
        else
        {
            to->arry[i].next = NULL;
        }
        if (from->arry[i].prev != NULL)
        {
            size_t from_prev_offset = ((uint64_t)from->arry[i].prev) - ((uint64_t)(from->arry));
            to->arry[i].prev = (BulletIns_FollowupBullet*)(((uint64_t)(to->arry)) + from_prev_offset);
        }
        else
        {
            to->arry[i].prev = NULL;
        }
    }

    to->data_0 = from->data_0;
    to->data_1 = from->data_1;
}

BulletMan_Field0x40* init_BulletMan_Field0x40()
{
    BulletMan_Field0x40* local = (BulletMan_Field0x40*)malloc_(sizeof(BulletMan_Field0x40));
    return local;
}

void free_BulletMan_Field0x40(BulletMan_Field0x40* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}

// ---- ChrCam ----

void copy_ChrCam(ChrCam* to, ChrCam* from, StateTarget target)
{
    //Copy the FrpgPersCam base data
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    //Deep copy sub-camera data. The sub-cameras are allocated once by the game and persist,
    //so for our local copy we have our own allocations. Copy the data at the pointers.
    if (from->ChrExFollowCam != NULL && to->ChrExFollowCam != NULL)
    {
        memcpy(to->ChrExFollowCam, from->ChrExFollowCam, ChrExFollowCam_size);
    }
    if (from->ChrAimCam != NULL && to->ChrAimCam != NULL)
    {
        memcpy(to->ChrAimCam, from->ChrAimCam, ChrAimCam_size);
    }
    if (from->FallDieCam != NULL && to->FallDieCam != NULL)
    {
        memcpy(to->FallDieCam, from->FallDieCam, FallDieCam_size);
    }
    if (from->BallistaAimCam != NULL && to->BallistaAimCam != NULL)
    {
        memcpy(to->BallistaAimCam, from->BallistaAimCam, BallistaAimCam_size);
    }

    //Copy remaining ChrCam state (flags, floats, etc.)
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

ChrCam* init_ChrCam()
{
    ChrCam* local = (ChrCam*)malloc_(sizeof(ChrCam));

    local->ChrExFollowCam = malloc_(ChrExFollowCam_size);
    local->ChrAimCam = malloc_(ChrAimCam_size);
    local->FallDieCam = malloc_(FallDieCam_size);
    local->BallistaAimCam = malloc_(BallistaAimCam_size);

    return local;
}

void free_ChrCam(ChrCam* to)
{
    if (to->ChrExFollowCam != NULL)
    {
        free(to->ChrExFollowCam);
    }
    if (to->ChrAimCam != NULL)
    {
        free(to->ChrAimCam);
    }
    if (to->FallDieCam != NULL)
    {
        free(to->FallDieCam);
    }
    if (to->BallistaAimCam != NULL)
    {
        free(to->BallistaAimCam);
    }
    free(to);
}

// ---- BulletMan ----

void copy_BulletMan(BulletMan* to, BulletMan* from, StateTarget target)
{
    Game::SuspendThreads();

    for (size_t i = 0; i < 128; i++)
    {
        copy_BulletIns(&to->bulletins_arry[i], &from->bulletins_arry[i], target);
    }
    //Fix up previous_bullet_in_use linked list pointers within the bulletins_arry.
    for (size_t i = 0; i < 128; i++)
    {
        if (from->bulletins_arry[i].previous_bullet_in_use != NULL)
        {
            uint64_t from_offset = (uint64_t)from->bulletins_arry[i].previous_bullet_in_use - (uint64_t)from->bulletins_arry;
            to->bulletins_arry[i].previous_bullet_in_use = (BulletIns*)((uint64_t)to->bulletins_arry + from_offset);
        }
        else
        {
            to->bulletins_arry[i].previous_bullet_in_use = NULL;
        }
    }

    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    for (size_t i = 0; i < 64; i++)
    {
        copy_BulletMan_Field0x20(&to->field0x20[i], &from->field0x20[i], target);
    }
    //Fix up next_in_use linked list pointers within the field0x20 array
    for (size_t i = 0; i < 64; i++)
    {
        if (from->field0x20[i].next_in_use != NULL)
        {
            uint64_t from_offset = (uint64_t)from->field0x20[i].next_in_use - (uint64_t)from->field0x20;
            to->field0x20[i].next_in_use = (BulletMan_Field0x20*)((uint64_t)to->field0x20 + from_offset);
        }
        else
        {
            to->field0x20[i].next_in_use = NULL;
        }
    }

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));

    for (size_t i = 0; i < 4; i++)
    {
        copy_BulletMan_Field0x40(&to->field0x40[i], &from->field0x40[i], target);
    }

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));

    copy_ChrCam(to->chrCam, from->chrCam, target);

    to->data_3 = from->data_3;

    memcpy(to->data_5, from->data_5, sizeof(to->data_5));

    Game::ResumeThreads();
}

BulletMan* init_BulletMan()
{
    BulletMan* local_BulletMan = (BulletMan*)malloc_(sizeof(BulletMan));

    local_BulletMan->bulletins_arry = (BulletIns*)malloc_(sizeof(BulletIns) * 128);
    for (size_t i = 0; i < 128; i++)
    {
        BulletIns* local_bulletins = init_BulletIns();
        local_BulletMan->bulletins_arry[i] = *local_bulletins;
        free(local_bulletins);
    }

    local_BulletMan->field0x20 = (BulletMan_Field0x20*)malloc_(sizeof(BulletMan_Field0x20) * 64);
    for (size_t i = 0; i < 64; i++)
    {
        BulletMan_Field0x20* local_BulletMan_Field0x20 = init_BulletMan_Field0x20();
        local_BulletMan->field0x20[i] = *local_BulletMan_Field0x20;
        free(local_BulletMan_Field0x20);
    }

    local_BulletMan->field0x40 = (BulletMan_Field0x40*)malloc_(sizeof(BulletMan_Field0x40) * 4);
    for (size_t i = 0; i < 4; i++)
    {
        BulletMan_Field0x40* local_BulletMan_Field0x40 = init_BulletMan_Field0x40();
        local_BulletMan->field0x40[i] = *local_BulletMan_Field0x40;
        free(local_BulletMan_Field0x40);
    }

    local_BulletMan->chrCam = init_ChrCam();

    return local_BulletMan;
}

void free_BulletMan(BulletMan* to)
{
    for (size_t i = 0; i < 128; i++)
    {
        free_BulletIns(&to->bulletins_arry[i], false);
    }
    free(to->bulletins_arry);
    for (size_t i = 0; i < 64; i++)
    {
        free_BulletMan_Field0x20(&to->field0x20[i], false);
    }
    free(to->field0x20);
    for (size_t i = 0; i < 4; i++)
    {
        free_BulletMan_Field0x40(&to->field0x40[i], false);
    }
    free(to->field0x40);
    free_ChrCam(to->chrCam);

    free(to);
}
