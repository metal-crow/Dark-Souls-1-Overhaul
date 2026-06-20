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

//Game's layout for BulletIns_FollowupBullet list allocations (verified via ghidra on
//ChrMultiSfxSlot_Process_, ChrStatueDeadSlot_Process_, and Destruct_ChrConditionSfxSeSlot):
//    raw = internal_heap_3->malloc(count * sizeof(BulletIns_FollowupBullet) + 0x10, 0x10)
//    raw[0x0] = raw                    //self pointer used on free
//    raw[0x8] = (uint64_t)count        //element count
//    list    = (BulletIns_FollowupBullet*)(raw + 0x10)
//    free    = Get_HeapAllocator_For_allocation(list)->free( *(void**)(((uint8_t*)list) - 0x10) )
static const size_t BULLETIN_FOLLOWUPBULLET_LIST_HEADER_SIZE = 0x10;

BulletIns_FollowupBullet* alloc_BulletIns_FollowupBullet_List(int16_t count, StateTarget target)
{
    if (count <= 0)
    {
        return NULL;
    }
    if (target == StateTarget::ToGame)
    {
        uint8_t* raw = (uint8_t*)Game::game_malloc(
            sizeof(BulletIns_FollowupBullet) * (size_t)count + BULLETIN_FOLLOWUPBULLET_LIST_HEADER_SIZE,
            0x10,
            *(uint64_t*)Game::internal_heap_3);
        *(void**)(raw + 0x0) = raw;
        *(uint64_t*)(raw + 0x8) = (uint64_t)count;
        return (BulletIns_FollowupBullet*)(raw + BULLETIN_FOLLOWUPBULLET_LIST_HEADER_SIZE);
    }
    return (BulletIns_FollowupBullet*)malloc_(sizeof(BulletIns_FollowupBullet) * (size_t)count);
}

void free_BulletIns_FollowupBullet_List(BulletIns_FollowupBullet** list, int16_t* len, StateTarget target)
{
    if (*list == NULL)
    {
        *len = 0;
        return;
    }
    if (target == StateTarget::ToGame)
    {
        void* raw = *(void**)(((uint8_t*)(*list)) - BULLETIN_FOLLOWUPBULLET_LIST_HEADER_SIZE);
        Game::game_free_alt(raw);
    }
    else
    {
        free(*list);
    }
    *list = NULL;
    *len = 0;
}

void copy_BulletIns_FollowupBullet_List(BulletIns_FollowupBullet** to_list, int16_t* to_len, BulletIns_FollowupBullet** from_list, int16_t* from_len, StateTarget target)
{
    //must realloc on any size change because the header has a build in count
    if (*to_len != *from_len)
    {
        free_BulletIns_FollowupBullet_List(to_list, to_len, target);
        *to_list = alloc_BulletIns_FollowupBullet_List(*from_len, target);
    }
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
    to->bullet_number = from->bullet_number;
    to->unk_9 = from->unk_9;
    to->unk_a = from->unk_a;
    memcpy(to->unk_c, from->unk_c, sizeof(to->unk_c));
    memcpy(to->player_position, from->player_position, sizeof(to->player_position));
    to->orbit_radius_X = from->orbit_radius_X;
    to->orbit_radius_Y = from->orbit_radius_Y;
    to->orbit_radius_Z = from->orbit_radius_Z;
    to->orbit_radius_W = from->orbit_radius_W;
    to->unk_30 = from->unk_30;
    to->unk_34 = from->unk_34;
    to->unk_38 = from->unk_38;
    to->unk_3c = from->unk_3c;
    to->unk_40 = from->unk_40;
    to->unk_44 = from->unk_44;
    to->unk_48 = from->unk_48;
    to->unk_4c = from->unk_4c;
    to->unk_50 = from->unk_50;
    to->unk_54 = from->unk_54;

    copy_BulletIns_FollowupBullet(&to->FollowupBullet, &from->FollowupBullet, target);

    to->bulletClassType = from->bulletClassType;
    to->unk_8c = from->unk_8c;

    //owner contains entity IDs, attack data, position vectors - all safe to memcpy
    memcpy(&to->owner, &from->owner, sizeof(to->owner));

    to->bullet_distance = from->bullet_distance;
    to->bullet_life = from->bullet_life;
    to->unk_1f8 = from->unk_1f8;
    to->unk_1fc = from->unk_1fc;
    to->unk_200 = from->unk_200;
    to->unk_204 = from->unk_204;

    //bulletTargetingSystemOwner contains parent ptr, vtable, entityId, NpcThinkParam - raw pointers are fine
    memcpy(&to->bulletTargetingSystemOwner, &from->bulletTargetingSystemOwner, sizeof(to->bulletTargetingSystemOwner));

    //targetingSystemBase contains vtable, owner ptr, targeting data - raw pointers are fine
    memcpy(&to->targetingSystemBase, &from->targetingSystemBase, sizeof(to->targetingSystemBase));
    //TODO setting the BulletTargetingSystemOwner to null for now, we may have to save/restore it
    to->targetingSystemBase.owner_1 = NULL;

    to->unk_288 = from->unk_288;
    to->unk_28c = from->unk_28c;
    memcpy(to->target, from->target, sizeof(to->target));
    memcpy(to->unk_2c1, from->unk_2c1, sizeof(to->unk_2c1));
    to->unk_2c4 = from->unk_2c4;
    to->unk_2c8 = from->unk_2c8;
    to->unk_2cc = from->unk_2cc;
    to->unk_2d0 = from->unk_2d0;
    memcpy(to->unk_2d1, from->unk_2d1, sizeof(to->unk_2d1));

    memcpy(&to->bulletState, &from->bulletState, sizeof(to->bulletState));
    to->unk_2f8 = from->unk_2f8;
    to->unk_2fc = from->unk_2fc;
    memcpy(&to->bulletFlyState, &from->bulletFlyState, sizeof(to->bulletFlyState));
    memcpy(&to->bulletExplosionState, &from->bulletExplosionState, sizeof(to->bulletExplosionState));

    //previous_bullet_in_use is handled by copy_BulletMan after all elements are copied
    to->unk_358 = from->unk_358;
    to->unk_35c = from->unk_35c;
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
    to->unk_0 = from->unk_0;
    memcpy(to->field0x4Elem, from->field0x4Elem, sizeof(to->field0x4Elem));
    memcpy(to->unk_184, from->unk_184, sizeof(to->unk_184));
    memcpy(to->unk_18c, from->unk_18c, sizeof(to->unk_18c));
    to->unk_194 = from->unk_194;
    to->unk_195 = from->unk_195;
    memcpy(to->unk_196, from->unk_196, sizeof(to->unk_196));

    //BulletParamInfo is game-allocated and never freed. Since this object is linked to the lifetime of BulletMan itself, it's stable
    to->BulletParamInfo = from->BulletParamInfo;

    memcpy(&to->field0x1a0, &from->field0x1a0, sizeof(to->field0x1a0));
    to->unk_300 = from->unk_300;
    to->unk_304 = from->unk_304;

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

    to->unk_308 = from->unk_308;
    to->unk_30c = from->unk_30c;
    to->unk_310 = from->unk_310;
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
    memcpy(to->FrpgPersCam_base, from->FrpgPersCam_base, sizeof(to->FrpgPersCam_base));

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
    to->unk_80 = from->unk_80;
    memcpy(to->unk_81, from->unk_81, sizeof(to->unk_81));
    to->unk_84 = from->unk_84;
    to->unk_88 = from->unk_88;
    to->unk_8c = from->unk_8c;
    memcpy(to->unk_90, from->unk_90, sizeof(to->unk_90));
    to->unk_9c = from->unk_9c;
    to->unk_a0 = from->unk_a0;
    to->unk_a1 = from->unk_a1;
    to->unk_a2 = from->unk_a2;
    to->UseBallistaAimCam = from->UseBallistaAimCam;
    memcpy(to->unk_a4, from->unk_a4, sizeof(to->unk_a4));
    to->unk_c0 = from->unk_c0;
    memcpy(to->unk_c1, from->unk_c1, sizeof(to->unk_c1));
    to->DbgNode = from->DbgNode;
    memcpy(to->unk_c9, from->unk_c9, sizeof(to->unk_c9));
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

    to->end_of_bullets_in_use = from->end_of_bullets_in_use;
    to->start_of_unused_bullets = from->start_of_unused_bullets;
    to->bullet_count_1 = from->bullet_count_1;
    to->bullet_count_2 = from->bullet_count_2;

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

    to->end_of_bullets_in_use_field0x20 = from->end_of_bullets_in_use_field0x20;
    to->start_of_unused_bullets_field0x20 = from->start_of_unused_bullets_field0x20;
    to->unk_38 = from->unk_38;
    to->unk_3c = from->unk_3c;

    for (size_t i = 0; i < 4; i++)
    {
        copy_BulletMan_Field0x40(&to->field0x40[i], &from->field0x40[i], target);
    }

    to->end_of_bullets_in_use_field0x40 = from->end_of_bullets_in_use_field0x40;
    to->start_of_unused_bullets_field0x40 = from->start_of_unused_bullets_field0x40;
    to->unk_58 = from->unk_58;
    to->unk_5c = from->unk_5c;

    copy_ChrCam(to->chrCam, from->chrCam, target);

    to->attach_system_disabled = from->attach_system_disabled;
    memcpy(to->unk_69, from->unk_69, sizeof(to->unk_69));

    to->bullet_direction_drawing = from->bullet_direction_drawing;
    memcpy(to->unk_a9, from->unk_a9, sizeof(to->unk_a9));
    to->speed_attenuation_rate = from->speed_attenuation_rate;
    to->last_addition_rate = from->last_addition_rate;
    to->external_force = from->external_force;

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
