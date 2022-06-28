#include "BulletManStructFunctions.h"
#include "Rollback.h"

typedef void* falloc(uint64_t, uint64_t, uint32_t);

void copy_BulletMan(BulletMan* to, BulletMan* from, bool to_game)
{
    for (size_t i = 0; i < 128; i++)
    {
        copy_BulletIns(&to->bulletins_arry[i], &from->bulletins_arry[i], to_game);
    }

    memcpy(to->data_0, from->data_0, sizeof(to->data_0));

    for (size_t i = 0; i < 40; i++)
    {
        copy_BulletMan_Field0x20(&to->field0x20[i], &from->field0x20[i], to_game);
    }

    memcpy(to->data_1, from->data_1, sizeof(to->data_1));

    for (size_t i = 0; i < 4; i++)
    {
        copy_BulletMan_Field0x40(&to->field0x40[i], &from->field0x40[i], to_game);
    }

    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
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
            copy_BulletMan_field0x78Elem(to->field0x78[i], from->field0x78[i], to_game);
        }
    }
    else
    {
        if (to_game)
        {
            to->field0x78 = NULL;
        }
    }
    to->field0x78_next = from->field0x78_next;
    to->field0x78_end = from->field0x78_end;

    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
}

BulletMan* init_BulletMan()
{
    BulletMan* local_BulletMan = (BulletMan*)malloc_(sizeof(BulletMan));

    local_BulletMan->bulletins_arry = (BulletIns*)malloc(sizeof(BulletIns) * 128);
    for (size_t i = 0; i < 128; i++)
    {
        BulletIns* local_bulletins = init_BulletIns();
        local_BulletMan->bulletins_arry[i] = *local_bulletins;
        free(local_bulletins);
    }

    local_BulletMan->field0x20 = (BulletMan_Field0x20*)malloc(sizeof(BulletMan_Field0x20) * 40);
    for (size_t i = 0; i < 40; i++)
    {
        BulletMan_Field0x20* local_BulletMan_Field0x20 = init_BulletMan_Field0x20();
        local_BulletMan->field0x20[i] = *local_BulletMan_Field0x20;
        free(local_BulletMan_Field0x20);
    }

    local_BulletMan->field0x40 = (BulletMan_Field0x40*)malloc(sizeof(BulletMan_Field0x40) * 4);
    for (size_t i = 0; i < 4; i++)
    {
        BulletMan_Field0x40* local_BulletMan_Field0x40 = init_BulletMan_Field0x40();
        local_BulletMan->field0x40[i] = *local_BulletMan_Field0x40;
        free(local_BulletMan_Field0x40);
    }

    local_BulletMan->field0x78 = (BulletMan_field0x78Elem**)malloc(sizeof(BulletMan_field0x78Elem*) * 3);
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
    free(to->field0x78);

    free(to);
}

void copy_BulletIns(BulletIns* to, BulletIns* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletIns_FollowupBullet(&to->FollowupBullet, &from->FollowupBullet, to_game);
    to->data_1 = from->data_1;
    copy_BulletIns_Field0x90_Field0x1a0(&to->owner, &from->owner, to_game);
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    copy_BulletTargetingSystemOwner(&to->bulletTargetingSystemOwner, &from->bulletTargetingSystemOwner, to_game);
    copy_TargetingSystemBase(&to->targetingSystemBase, &from->targetingSystemBase, to_game);
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    copy_BulletState(&to->bulletState, &from->bulletState, to_game);
    to->data_4 = from->data_4;
    copy_BulletFlyState(&to->bulletFlyState, &from->bulletFlyState, to_game);
    copy_BulletState(&to->bulletExplosionState, &from->bulletExplosionState, to_game);
    to->data_5 = from->data_5;
}

BulletIns* init_BulletIns()
{
    BulletIns* local_BulletIns = (BulletIns*)malloc_(sizeof(BulletIns));

    //none of the substruct have pointers, so can save some code here

    return local_BulletIns;
}

void free_BulletIns(BulletIns* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}

void copy_BulletIns_FollowupBullet(BulletIns_FollowupBullet* to, BulletIns_FollowupBullet* from, bool to_game)
{
    return;
}

void copy_BulletIns_Field0x90_Field0x1a0(BulletIns_Field0x90_Field0x1a0* to, BulletIns_Field0x90_Field0x1a0* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
    memcpy(to->data_4, from->data_4, sizeof(to->data_4));
    memcpy(to->data_5, from->data_5, sizeof(to->data_5));
}

void copy_BulletTargetingSystemOwner(BulletTargetingSystemOwner* to, BulletTargetingSystemOwner* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_TargetingSystemBase(TargetingSystemBase* to, TargetingSystemBase* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->data_1 = from->data_1;
}

void copy_BulletState(BulletState* to, BulletState* from, bool to_game)
{
    copy_BulletParamInfo(&to->paramInfo, &from->paramInfo, to_game);
    to->data_0 = from->data_0;
}

void copy_BulletFlyState(BulletFlyState* to, BulletFlyState* from, bool to_game)
{
    copy_BulletState(&to->state, &from->state, to_game);
    to->data_0 = from->data_0;
}

void copy_BulletParamInfo(BulletParamInfo* to, BulletParamInfo* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}

void copy_BulletMan_Field0x20(BulletMan_Field0x20* to, BulletMan_Field0x20* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    copy_BulletParamInfo(to->bulletParamInfo, from->bulletParamInfo, to_game);
    copy_BulletIns_Field0x90_Field0x1a0(&to->field0x1a0, &from->field0x1a0, to_game);
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

void copy_BulletMan_Field0x40(BulletMan_Field0x40* to, BulletMan_Field0x40* from, bool to_game)
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

void copy_BulletMan_field0x78Elem(BulletMan_field0x78Elem* to, BulletMan_field0x78Elem* from, bool to_game)
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
