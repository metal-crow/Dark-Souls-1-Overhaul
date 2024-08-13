#include "DmgHitRecordManImpStructFunctions.h"
#include "Rollback.h"

static const size_t max_preallocated_DmgHitRecordManImp_field0x10Elem = 96;
static const size_t max_preallocated_DmgHitRecordManImp_field0x28Elem = 96;

void copy_DmgHitRecordManImp(DmgHitRecordManImp* to, DmgHitRecordManImp* from, bool to_game)
{
    Game::SuspendThreads();

    uint64_t offset0x10 = ((uint64_t)from->field0x10_array) - ((uint64_t)from->field0x10_array_start);
    to->field0x10_array = (DmgHitRecordManImp_field0x10Elem*)(((uint64_t)to->field0x10_array_start) + offset0x10);
    for (size_t i = 0; i < max_preallocated_DmgHitRecordManImp_field0x10Elem; i++)
    {
        copy_DmgHitRecordManImp_field0x10Elem(&to->field0x10_array_start[i], &from->field0x10_array_start[i], to_game);

        if (from->field0x10_array_start[i].attached != NULL)
        {
            uint64_t attached_offset = ((uint64)from->field0x10_array_start[i].attached) - ((uint64)from->field0x28_array_start);
            to->field0x10_array_start[i].attached = (DmgHitRecordManImp_field0x28Elem*)(((uint64)to->field0x28_array_start) + attached_offset);
        }
        else
        {
            to->field0x10_array_start[i].attached = NULL;
        }

        if (from->field0x10_array_start[i].next != NULL)
        {
            uint64_t next_offset = ((uint64)from->field0x10_array_start[i].next) - ((uint64)from->field0x10_array_start);
            to->field0x10_array_start[i].next = (DmgHitRecordManImp_field0x10Elem*)(((uint64)to->field0x10_array_start) + next_offset);
        }
        else
        {
            to->field0x10_array_start[i].next = NULL;
        }
    }

    uint64_t offset0x28 = ((uint64_t)from->field0x28_array_start) - ((uint64_t)from->field0x28_array);
    to->field0x28_array = (DmgHitRecordManImp_field0x28Elem*)(((uint64_t)to->field0x28_array_start) + offset0x28);
    for (size_t i = 0; i < max_preallocated_DmgHitRecordManImp_field0x28Elem; i++)
    {
        copy_DmgHitRecordManImp_field0x28Elem(&to->field0x28_array_start[i], &from->field0x28_array_start[i], to_game);

        if (from->field0x28_array_start[i].next != NULL)
        {
            uint64_t next_offset = ((uint64)from->field0x28_array_start[i].next) - ((uint64)from->field0x28_array_start);
            to->field0x28_array_start[i].next = (DmgHitRecordManImp_field0x28Elem*)(((uint64)to->field0x28_array_start) + next_offset);
        }
        else
        {
            to->field0x28_array_start[i].next = NULL;
        }
    }

    to->field0x10_array_len = from->field0x10_array_len;
    to->data_0 = from->data_0;
    to->field0x28_array_len = from->field0x28_array_len;
    to->data_1 = from->data_1;

    Game::ResumeThreads();
}

DmgHitRecordManImp* init_DmgHitRecordManImp()
{
    DmgHitRecordManImp* local = (DmgHitRecordManImp*)malloc_(sizeof(DmgHitRecordManImp));

    local->field0x10_array_start = (DmgHitRecordManImp_field0x10Elem*)malloc_(sizeof(DmgHitRecordManImp_field0x10Elem) * max_preallocated_DmgHitRecordManImp_field0x10Elem);
    local->field0x28_array_start = (DmgHitRecordManImp_field0x28Elem*)malloc_(sizeof(DmgHitRecordManImp_field0x28Elem) * max_preallocated_DmgHitRecordManImp_field0x28Elem);

    return local;
}

void free_DmgHitRecordManImp(DmgHitRecordManImp* to)
{
    free(to->field0x10_array_start);
    free(to->field0x28_array_start);
    free(to);
}

void copy_DmgHitRecordManImp_field0x10Elem(DmgHitRecordManImp_field0x10Elem* to, DmgHitRecordManImp_field0x10Elem* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->data_1 = from->data_1;
}

void copy_DmgHitRecordManImp_field0x28Elem(DmgHitRecordManImp_field0x28Elem* to, DmgHitRecordManImp_field0x28Elem* from, bool to_game)
{
    to->data_0 = from->data_0;
    to->data_1 = from->data_1;
}
