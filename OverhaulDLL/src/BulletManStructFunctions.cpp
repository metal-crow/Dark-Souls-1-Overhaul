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

    size_t field0x78_len = (from->field0x78_end - from->field0x78)/8;
    if (field0x78_len > 3)
    {
        FATALERROR("BulletMan->field0x78 array is longer then 3 elements. %d", field0x78_len);
    }
    for (size_t i = 0; i < field0x78_len; i++)
    {
        copy_BulletMan_field0x78Elem(to->field0x78[i], from->field0x78[i], to_game);
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
    for (size_t i = 0; i < 40; i++)
    {
        free_BulletMan_Field0x20(&to->field0x20[i], false);
    }
    for (size_t i = 0; i < 4; i++)
    {
        free_BulletMan_Field0x40(&to->field0x40[i], false);
    }
    for (size_t i = 0; i < 3; i++)
    {
        free_BulletMan_field0x78Elem(to->field0x78[i]);
    }
    free(to->field0x78);

}
