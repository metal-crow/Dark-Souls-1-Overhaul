#include "SfxManStructFunctions.h"
#include "Rollback.h"

static const size_t max_preallocated_class_14152d360 = 256;

typedef uint64_t FUN_140f5f6c0_FUNC(uint64_t p);
FUN_140f5f6c0_FUNC* FUN_140f5f6c0 = (FUN_140f5f6c0_FUNC*)0x140f5f6c0;

typedef void* smallObject_internal_malloc_FUNC(uint64_t heap, uint64_t size, uint64_t align);
smallObject_internal_malloc_FUNC* smallObject_internal_malloc = (smallObject_internal_malloc_FUNC*)0x140cbe4f0;

typedef void* smallObject_internal_dealloc_FUNC(uint64_t heap, void* obj, uint64_t size, uint64_t align);
smallObject_internal_dealloc_FUNC* smallObject_internal_dealloc = (smallObject_internal_dealloc_FUNC*)0x140cbe790;

void copy_class_14152d360(class_14152d360* to, class_14152d360* from, bool to_game)
{
    if (!to_game)
    {
        size_t to_index = 0;
        while (from)
        {
            if (to_index >= max_preallocated_class_14152d360)
            {
                ConsoleWrite("Unable to recursivly copy class_14152d360 from the game. Out of space.");
                break;
            }
            copy_class_14152d360_asObj(to, from, to_game);

            if (from->next != NULL)
            {
                to->next = (class_14152d360*)((uint64_t)(to)+sizeof(class_14152d360));
            }
            else
            {
                to->next = NULL;
            }

            from = from->next;
            to = (class_14152d360*)((uint64_t)(to)+sizeof(class_14152d360));
            to_index += 1;
        }
    }
    else
    {
        while (from)
        {
            copy_class_14152d360_asObj(to, from, to_game);

            //handle if the game's list isn't long enough, and we need to alloc more slots
            if (from->next != NULL && to->next == NULL)
            {
                uint64_t heap = FUN_140f5f6c0(0x141c03470);
                to->next = (class_14152d360*)smallObject_internal_malloc(heap, sizeof(class_14152d360), 0x10);
                to->next->next = NULL;
            }

            //handle if the game's list is too long, and we need to free it's extra slots
            if (from->next == NULL && to->next != NULL)
            {
                class_14152d360* entry_to_free = to->next;
                to->next = NULL;
                while (entry_to_free)
                {
                    class_14152d360* next = entry_to_free->next;
                    uint64_t heap = FUN_140f5f6c0(0x141c03470);
                    smallObject_internal_dealloc(heap, entry_to_free, sizeof(class_14152d360), 0x10);
                    entry_to_free = next;
                }
                break;
            }

            from = from->next;
            to = to->next;
        }
    }
}

void copy_class_14152d360_asObj(class_14152d360* to, class_14152d360* from, bool to_game)
{
    to->vtable = 0x14152d360;
    to->field0x8 = NULL;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->parent = NULL;
    to->unk1 = NULL;
    to->unk2 = NULL;
    if (from->field0x48 != NULL)
    {
        if (to->field0x48 == NULL)
        {
            if (to_game)
            {
                uint64_t heap = FUN_140f5f6c0(0x141c03470);
                to->field0x48 = (class_14150b808_field0x48*)smallObject_internal_malloc(heap, sizeof(class_14150b808_field0x48), 8);
            }
            else
            {
                to->field0x48 = init_class_14150b808_field0x48();
            }
        }
        copy_class_14150b808_field0x48(to->field0x48, from->field0x48, to_game);
    }
    else if (from->field0x48 == NULL && to->field0x48 != NULL)
    {
        if (to_game)
        {
            uint64_t heap = FUN_140f5f6c0(0x141c03470);
            smallObject_internal_dealloc(heap, to->field0x48, sizeof(class_14150b808_field0x48), 8);
        }
        else
        {
            free_class_14150b808_field0x48(to->field0x48);
        }
        to->field0x48 = NULL;
    }
    to->unk3 = NULL;
    to->unk4 = NULL;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->unk5 = NULL;
    to->data_2 = from->data_2;
    to->field0xf0 = NULL;
    to->data_3 = from->data_3;
}

class_14152d360* init_class_14152d360()
{
    //this is a linked list, so pre-allocate a max of 256 for the classes
    class_14152d360* local_class_14152d360 = init_class_14152d360_asObj();

    class_14152d360* head = local_class_14152d360;
    for (int i = 1; i < max_preallocated_class_14152d360; i++)
    {
        head->next = init_class_14152d360_asObj();
        head = head->next;
    }

    return local_class_14152d360;
}

class_14152d360* init_class_14152d360_asObj()
{
    class_14152d360* local_class_14152d360 = (class_14152d360*)malloc_(sizeof(class_14152d360));

    local_class_14152d360->field0x48 = NULL; //alloc'd as needed
    local_class_14152d360->next = NULL;

    return local_class_14152d360;
}

void free_class_14152d360(class_14152d360* to)
{
    if (to == NULL)
    {
        return;
    }
    free_class_14152d360(to->next);
    free_class_14150b808_field0x48(to->field0x48);
    free(to);
}

void copy_class_14150b808_field0x48(class_14150b808_field0x48* to, class_14150b808_field0x48* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->self_class_1415262e0 = (uint64_t)to + 0xe0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->unk1 = from->unk1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    to->unk2 = NULL;
    to->unk3 = NULL;
    to->unk4 = from->unk4;
    to->unk5 = from->unk5;
    memset(to->padding_0, 0, sizeof(to->padding_0));
    //leave parent ptr alone
    to->unk6 = NULL;
    to->vtable = 0x1415262e0;
    to->unk7 = from->unk7;
    to->unk8 = NULL;
    to->unk9 = NULL;
    to->unk10 = NULL;
    to->unk11 = NULL;
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

class_14150b808_field0x48* init_class_14150b808_field0x48()
{
    class_14150b808_field0x48* local_class_14150b808_field0x48 = (class_14150b808_field0x48*)malloc_(sizeof(class_14150b808_field0x48));

    return local_class_14150b808_field0x48;
}

void free_class_14150b808_field0x48(class_14150b808_field0x48* to)
{
    free(to);
}
