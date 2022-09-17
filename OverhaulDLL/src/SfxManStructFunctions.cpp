#include "SfxManStructFunctions.h"
#include "Rollback.h"

void copy_SfxMan(SfxMan* to, SfxMan* from, bool to_game)
{
    copy_frpgFxManagerBase(to->FrpgFxManagerBase, from->FrpgFxManagerBase, to_game);
}

SfxMan* init_SfxMan()
{
    SfxMan* local_SfxMan = (SfxMan*)malloc_(sizeof(SfxMan));

    local_SfxMan->FrpgFxManagerBase = init_frpgFxManagerBase();

    return local_SfxMan;
}

void free_SfxMan(SfxMan* to)
{
    free_frpgFxManagerBase(to->FrpgFxManagerBase);

    free(to);
}

void copy_frpgFxManagerBase(frpgFxManagerBase* to, frpgFxManagerBase* from, bool to_game)
{
    copy_FXHGManagerBase(&to->base, &from->base, to_game);
}

frpgFxManagerBase* init_frpgFxManagerBase()
{
    frpgFxManagerBase* local_frpgFxManagerBase = (frpgFxManagerBase*)malloc_(sizeof(frpgFxManagerBase));

    FXHGManagerBase* local_FXHGManagerBase = init_FXHGManagerBase();
    local_frpgFxManagerBase->base = *local_FXHGManagerBase;
    free(local_FXHGManagerBase);

    return local_frpgFxManagerBase;
}

void free_frpgFxManagerBase(frpgFxManagerBase* to)
{
    free_FXHGManagerBase(&to->base, false);

    free(to);
}

void copy_FXHGManagerBase(FXHGManagerBase* to, FXHGManagerBase* from, bool to_game)
{
    copy_class_1415002c8(to->Class_1415002c8, from->Class_1415002c8, to_game);
}

FXHGManagerBase* init_FXHGManagerBase()
{
    FXHGManagerBase* local_FXHGManagerBase = (FXHGManagerBase*)malloc_(sizeof(FXHGManagerBase));

    local_FXHGManagerBase->Class_1415002c8 = init_class_1415002c8();

    return local_FXHGManagerBase;
}

void free_FXHGManagerBase(FXHGManagerBase* to, bool freeself)
{
    free_class_1415002c8(to->Class_1415002c8);

    if (freeself)
    {
        free(to);
    }
}

void copy_class_1415002c8(class_1415002c8* to, class_1415002c8* from, bool to_game)
{
    copy_class_14152d360(to->Class_14152d360, from->Class_14152d360, to_game);
}

class_1415002c8* init_class_1415002c8()
{
    class_1415002c8* local_class_1415002c8 = (class_1415002c8*)malloc_(sizeof(class_1415002c8));

    local_class_1415002c8->Class_14152d360 = init_class_14152d360();

    return local_class_1415002c8;
}

void free_class_1415002c8(class_1415002c8* to)
{
    free_class_14152d360(to->Class_14152d360);

    free(to);
}

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
            //TODO copy

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
            //TODO copy

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

class_14152d360* init_class_14152d360()
{
    //this is a linked list, so pre-allocate a max of 256 for the classes
    class_14152d360* local_class_14152d360 = (class_14152d360*)malloc_(sizeof(class_14152d360)*max_preallocated_class_14152d360);

    return local_class_14152d360;
}

void free_class_14152d360(class_14152d360* to)
{
    free(to);
}
