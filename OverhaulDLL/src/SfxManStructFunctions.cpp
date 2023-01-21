#include "GameData.h"
#include "SfxManStructFunctions.h"
#include "Rollback.h"

void copy_FXManager(FXManager* to, FXManager* from, bool to_game)
{
    copy_SFXEntryList(to->SFXEntryList, from->SFXEntryList, to_game);
}

FXManager* init_FXManager()
{
    FXManager* local_FXManager = (FXManager*)malloc_(sizeof(FXManager));

    local_FXManager->SFXEntryList = init_SFXEntryList();

    return local_FXManager;
}

void free_FXManager(FXManager* to)
{
    free_SFXEntryList(to->SFXEntryList);
    free(to);
}

static const size_t max_preallocated_SFXEntries = 256;

void copy_SFXEntryList(SFXEntry* to, SFXEntry* from, bool to_game)
{
    if (!to_game)
    {
        size_t to_index = 0;
        while (from)
        {
            if (to_index >= max_preallocated_SFXEntries)
            {
                ConsoleWrite("Unable to recursivly copy SFXEntry from the game. Out of space.");
                break;
            }
            copy_SFXEntry(to, from, to_game);

            if (from->next != NULL)
            {
                to->next = (SFXEntry*)((uint64_t)(to)+sizeof(SFXEntry));
            }
            else
            {
                to->next = NULL;
            }

            from = from->next;
            to = (SFXEntry*)((uint64_t)(to)+sizeof(SFXEntry));
            to_index += 1;
        }
    }
    else
    {
        while (from)
        {
            copy_SFXEntry(to, from, to_game);

            //handle if the game's list isn't long enough, and we need to alloc more slots
            if (from->next != NULL && to->next == NULL)
            {
                to->next = (SFXEntry*)smallObject_internal_malloc(0, sizeof(SFXEntry), 0x10); //TODO
                to->next->next = NULL;
            }

            //handle if the game's list is too long, and we need to free it's extra slots
            if (from->next == NULL && to->next != NULL)
            {
                SFXEntry* entry_to_free = to->next;
                to->next = NULL;
                while (entry_to_free)
                {
                    SFXEntry* next = entry_to_free->next;
                    smallObject_internal_dealloc(0, entry_to_free, sizeof(SFXEntry), 0x10); //TODO
                    entry_to_free = next;
                }
                break;
            }

            from = from->next;
            to = to->next;
        }
    }
}

void copy_SFXEntry(SFXEntry* to, SFXEntry* from, bool to_game)
{
    to->vtable = 0x14152d360;
    to->field0x8 = NULL;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->parent = NULL;
    to->unk1 = NULL;
    to->unk2 = NULL;
    if (from->field0x48_head != NULL)
    {
        if (to->field0x48_head == NULL)
        {
            if (to_game)
            {
                to->field0x48_head = (class_14150b808_field0x48*)smallObject_internal_malloc(0, sizeof(class_14150b808_field0x48), 8); //TODO
            }
            else
            {
                to->field0x48_head = init_class_14150b808_field0x48();
            }
        }
        copy_class_14150b808_field0x48(to->field0x48_head, from->field0x48_head, to_game);
        class_14150b808_field0x48* tail = to->field0x48_head;
        while (tail->next != NULL)
        {
            tail = tail->next;
        }
        to->field0x48_tail = tail;
    }
    else if (from->field0x48_head == NULL && to->field0x48_head != NULL)
    {
        if (to_game)
        {
            smallObject_internal_dealloc(0, to->field0x48_head, sizeof(class_14150b808_field0x48), 8); //TODO
        }
        else
        {
            free_class_14150b808_field0x48(to->field0x48_head);
        }
        to->field0x48_head = NULL;
        to->field0x48_tail = NULL;
    }

    to->unk4 = NULL;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->field0xe0 = NULL; //TODO
    to->data_2 = from->data_2;
    to->field0xf0 = NULL; //TODO
    to->data_3 = from->data_3;
}

SFXEntry* init_SFXEntryList()
{
    //this is a linked list, so pre-allocate a max of 256 for the classes
    SFXEntry* local_SFXEntry = (SFXEntry*)malloc_(sizeof(SFXEntry)*max_preallocated_SFXEntries);

    //field0x48 must be dynamically alloc'd, since it can be null

    return local_SFXEntry;
}

void free_SFXEntryList(SFXEntry* to)
{
    SFXEntry* head = to;
    for (size_t i = 0; i < max_preallocated_SFXEntries; i++)
    {
        free_class_14150b808_field0x48(to->field0x48_head);
        head = (SFXEntry*)((uint64_t)(head)+sizeof(SFXEntry));
    }
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
    to->next = NULL;
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
