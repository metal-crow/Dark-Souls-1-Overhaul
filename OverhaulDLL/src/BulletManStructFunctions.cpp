#include "BulletManStructFunctions.h"
#include "SfxManStructFunctions.h"
#include "Rollback.h"

void copy_BulletMan(BulletMan* to, BulletMan* from, StateTarget target)
{
    Game::SuspendThreads();

    for (size_t i = 0; i < 128; i++)
    {
        copy_BulletIns(&to->bulletins_arry[i], &from->bulletins_arry[i], target);
    }

    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    for (size_t i = 0; i < 40; i++)
    {
        copy_BulletMan_Field0x20(&to->field0x20[i], &from->field0x20[i], target);
    }

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));

    for (size_t i = 0; i < 4; i++)
    {
        copy_BulletMan_Field0x40(&to->field0x40[i], &from->field0x40[i], target);
    }

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));

    copy_ChrCam(to->chrCam, from->chrCam, target);

    to->data_3 = from->data_3;

    if (from->field0x78 != NULL && from->field0x78_end != NULL)
    {
        size_t field0x78_len = (from->field0x78_end - (uint64_t)from->field0x78) / 8;
        if (field0x78_len > 3)
        {
            FATALERROR("BulletMan->field0x78 array is longer then 3 elements. end=%x start=%x len=%d", from->field0x78_end, from->field0x78, field0x78_len);
        }
        for (size_t i = 0; i < field0x78_len; i++)
        {
            copy_BulletMan_field0x78Elem(to->field0x78[i], from->field0x78[i], target);
        }
    }
    else
    {
        if (target == StateTarget::ToGame)
        {
            to->field0x78 = NULL;
        }
    }
    to->field0x78_next = (uint64_t)(to->field0x78) + (from->field0x78_next - (uint64_t)from->field0x78); //convert to offset and then to pointer
    to->field0x78_end = (uint64_t)(to->field0x78) + (from->field0x78_end - (uint64_t)from->field0x78);

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

    local_BulletMan->field0x20 = (BulletMan_Field0x20*)malloc_(sizeof(BulletMan_Field0x20) * 40);
    for (size_t i = 0; i < 40; i++)
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

    local_BulletMan->field0x78 = (BulletMan_field0x78Elem**)malloc_(sizeof(BulletMan_field0x78Elem*) * 3);
    for (size_t i = 0; i < 3; i++)
    {
        local_BulletMan->field0x78[i] = init_BulletMan_field0x78Elem();
    }

    return local_BulletMan;
}

void free_BulletMan(BulletMan* to)
{
    for (size_t i = 0; i < 128; i++)
    {
        free_BulletIns(&to->bulletins_arry[i], false);
    }
    free(to->bulletins_arry);
    for (size_t i = 0; i < 40; i++)
    {
        free_BulletMan_Field0x20(&to->field0x20[i], false);
    }
    free(to->field0x20);
    for (size_t i = 0; i < 4; i++)
    {
        free_BulletMan_Field0x40(&to->field0x40[i], false);
    }
    free(to->field0x40);
    for (size_t i = 0; i < 3; i++)
    {
        free_BulletMan_field0x78Elem(to->field0x78[i]);
    }
    free_ChrCam(to->chrCam);
    free(to->field0x78);

    free(to);
}


void copy_ChrCam(ChrCam* to, ChrCam* from, StateTarget target)
{
    copy_ChrExFollowCam(to->chrExFollowCam, from->chrExFollowCam, target);
}

ChrCam* init_ChrCam()
{
    ChrCam* local_ChrCam = (ChrCam*)malloc_(sizeof(ChrCam));
    local_ChrCam->chrExFollowCam = init_ChrExFollowCam();
    return local_ChrCam;
}

void free_ChrCam(ChrCam* to)
{
    free_ChrExFollowCam(to->chrExFollowCam);
    free(to);
}


void copy_ChrExFollowCam(ChrExFollowCam* to, ChrExFollowCam* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

ChrExFollowCam* init_ChrExFollowCam()
{
    ChrExFollowCam* local_ChrExFollowCam = (ChrExFollowCam*)malloc_(sizeof(ChrExFollowCam));
    return local_ChrExFollowCam;
}

void free_ChrExFollowCam(ChrExFollowCam* to)
{
    free(to);
}


void copy_BulletIns(BulletIns* to, BulletIns* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->FollowupBullet, &from->FollowupBullet, target);
    to->data_1 = from->data_1;
    copy_BulletIns_Field0x90_Field0x1a0(&to->owner, &from->owner, target);
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_BulletTargetingSystemOwner(&to->bulletTargetingSystemOwner, &from->bulletTargetingSystemOwner, target);
    copy_TargetingSystemBase(&to->targetingSystemBase, &from->targetingSystemBase, target);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    copy_BulletState(&to->bulletState, &from->bulletState, target);
    to->data_4 = from->data_4;
    copy_BulletFlyState(&to->bulletFlyState, &from->bulletFlyState, target);
    copy_BulletState(&to->bulletExplosionState, &from->bulletExplosionState, target);
    to->padding_previous_bullet_in_use = NULL;
    to->data_5 = from->data_5;
}

BulletIns* init_BulletIns()
{
    BulletIns* local_BulletIns = (BulletIns*)malloc_(sizeof(BulletIns));

    return local_BulletIns;
}

void free_BulletIns(BulletIns* to, bool freeself)
{
    free_BulletIns_FollowupBullet(&to->FollowupBullet, false, true);
    if (freeself)
    {
        free(to);
    }
}


//This only handles the data in the struct. The next/prev ptrs have to be handled by the caller
//This is because otherwise we may allocate a new bullet struct for the next ptr, but the correct answer is to point to an already existing bullet struct
void copy_BulletIns_FollowupBullet_Data(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, StateTarget target)
{
    to->vtable = from->vtable;
    //This is just a const ptr to the FXManager in the SfxMan global
    to->FXManager = from->FXManager;
    //Similar to above for these
    to->FXEntry_Substruct_a = from->FXEntry_Substruct_a;
    to->FXEntry_Substruct_b = from->FXEntry_Substruct_b;

    return;
}

void copy_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, StateTarget target)
{
    copy_BulletIns_FollowupBullet_Data(to, from, target);
    //TODO unsure how to handle these
    to->next = NULL;
    to->prev = NULL;
}

void copy_BulletIns_FollowupBullet_List(
    BulletIns_FollowupBullet** to_followup_bullet_list_ptr, int16_t* to_followup_bullet_list_len_ptr,
    BulletIns_FollowupBullet** from_followup_bullet_list_ptr, int16_t* from_followup_bullet_list_len_ptr,
    StateTarget target)
{
    if (*from_followup_bullet_list_ptr == NULL)
    {
        if (*to_followup_bullet_list_ptr != NULL)
        {
            if (target == StateTarget::ToGame)
            {
                Game::game_free(*to_followup_bullet_list_ptr);
            }
            else
            {
                free(*to_followup_bullet_list_ptr);
            }
        }
        *to_followup_bullet_list_ptr = NULL;
    }
    else
    {
        //Resize the list
        size_t from_list_size = *from_followup_bullet_list_len_ptr;
        size_t to_list_size = *to_followup_bullet_list_len_ptr;
        if (to_list_size != from_list_size)
        {
            if (target == StateTarget::ToGame)
            {
                auto new_followup_bullet_list = (BulletIns_FollowupBullet*)Game::game_malloc(sizeof(BulletIns_FollowupBullet) * from_list_size, 0x10, *(uint64_t*)Game::internal_heap_3);
                if (*to_followup_bullet_list_ptr != NULL)
                {
                    Game::game_free(*to_followup_bullet_list_ptr);
                }
                *to_followup_bullet_list_ptr = new_followup_bullet_list;
            }
            else
            {
                auto new_followup_bullet_list = (BulletIns_FollowupBullet*)malloc_(sizeof(BulletIns_FollowupBullet) * from_list_size);
                if (*to_followup_bullet_list_ptr != NULL)
                {
                    free(*to_followup_bullet_list_ptr);
                }
                *to_followup_bullet_list_ptr = new_followup_bullet_list;
            }
        }

        for (size_t list_i = 0; list_i < *from_followup_bullet_list_len_ptr; list_i++)
        {
            BulletIns_FollowupBullet* to_bullet = &(*to_followup_bullet_list_ptr)[list_i];
            BulletIns_FollowupBullet* from_bullet = &(*from_followup_bullet_list_ptr)[list_i];
            copy_BulletIns_FollowupBullet(to_bullet, from_bullet, target);
        }
    }
    *to_followup_bullet_list_len_ptr = *from_followup_bullet_list_len_ptr;
}


void free_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, bool freeself, bool freenext)
{
    if (freenext && to->next != NULL)
    {
        free_BulletIns_FollowupBullet(to->next, true, freenext);
    }

    if (freeself)
    {
        free(to);
    }
}


void copy_BulletIns_Field0x90_Field0x1a0(BulletIns_Field0x90_Field0x1a0* to, BulletIns_Field0x90_Field0x1a0* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
}


void copy_BulletTargetingSystemOwner(BulletTargetingSystemOwner* to, BulletTargetingSystemOwner* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}


void copy_TargetingSystemBase(TargetingSystemBase* to, TargetingSystemBase* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->data_1 = from->data_1;
}


void copy_BulletState(BulletState* to, BulletState* from, StateTarget target)
{
    copy_BulletParamInfo(&to->paramInfo, &from->paramInfo, target);
    to->data_0 = from->data_0;
}


void copy_BulletFlyState(BulletFlyState* to, BulletFlyState* from, StateTarget target)
{
    copy_BulletState(&to->state, &from->state, target);
    to->data_0 = from->data_0;
}


void copy_BulletParamInfo(BulletParamInfo* to, BulletParamInfo* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}


void copy_BulletMan_Field0x20(BulletMan_Field0x20* to, BulletMan_Field0x20* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletParamInfo(to->bulletParamInfo, from->bulletParamInfo, target);
    copy_BulletIns_Field0x90_Field0x1a0(&to->field0x1a0, &from->field0x1a0, target);
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

BulletMan_Field0x20* init_BulletMan_Field0x20()
{
    BulletMan_Field0x20* local_BulletMan_Field0x20 = (BulletMan_Field0x20*)malloc_(sizeof(BulletMan_Field0x20));

    local_BulletMan_Field0x20->bulletParamInfo = (BulletParamInfo*)malloc_(sizeof(BulletParamInfo));

    return local_BulletMan_Field0x20;

}

void free_BulletMan_Field0x20(BulletMan_Field0x20* to, bool freeself)
{
    free(to->bulletParamInfo);

    if (freeself)
    {
        free(to);
    }
}


void copy_BulletMan_Field0x40(BulletMan_Field0x40* to, BulletMan_Field0x40* from, StateTarget target)
{
    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

BulletMan_Field0x40* init_BulletMan_Field0x40()
{
    BulletMan_Field0x40* local_BulletMan_Field0x40 = (BulletMan_Field0x40*)malloc_(sizeof(BulletMan_Field0x40));

    return local_BulletMan_Field0x40;
}

void free_BulletMan_Field0x40(BulletMan_Field0x40* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}


void copy_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to, BulletMan_field0x78Elem* from, StateTarget target)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

BulletMan_field0x78Elem* init_BulletMan_field0x78Elem()
{
    BulletMan_field0x78Elem* local_BulletMan_field0x78Elem = (BulletMan_field0x78Elem*)malloc_(sizeof(BulletMan_field0x78Elem));

    return local_BulletMan_field0x78Elem;
}

void free_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to)
{
    free(to);
}
