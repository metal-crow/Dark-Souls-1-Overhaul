#include "GameData.h"
#include "SfxManStructFunctions.h"
#include "Rollback.h"

std::string print_FXManager(FXManager* to)
{
    std::string out = "FXManager\n";
    if (to == NULL)
    {
        return out;
    }

    out += print_SFXEntryList(to->SFXEntryList);

    return out;
}

void copy_FXManager(FXManager* to, FXManager* from, bool to_game)
{
    Game::SuspendThreads();

    copy_SFXEntryList(to->SFXEntryList, from->SFXEntryList, to_game, to, from);
    ConsoleWrite("--------------------------------------------");

    Game::ResumeThreads();
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

static uint64_t* HeapPtr = (uint64_t*)(0x0141B67450 + 8);

std::string print_SFXEntryList(SFXEntry* to)
{
    std::string out;

    while (to)
    {
        out += "SFXEntry\n";
        
    }

    return out;
}

void copy_SFXEntryList(SFXEntry* to, SFXEntry* from, bool to_game, FXManager* to_parent, FXManager* from_parent)
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

            //if we reach the tail here, we're at the last valid entry. The next ptr should be null anyway, but just to be safe
            if (from == from_parent->SFXEntryList_tail)
            {
                to->next = NULL;
                break;
            }

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
        //pre-process the list. If the target has too few or too many entries, need to correct that
        SFXEntry* from_pre = from;
        size_t from_len = 0;
        while (from_pre)
        {
            from_len++;
            from_pre = from_pre->next;
        }

        SFXEntry* to_pre = to;
        size_t to_len = 0;
        while (to_pre)
        {
            to_len++;
            to_pre = to_pre->next;
        }

        //handle if the game's list is too long, and we need to free it's extra slots
        if (from_len < to_len)
        {
            ConsoleWrite("Game SFXEntryList too long. Free");
            for (from_len; from_len < to_len; from_len++)
            {
                SFXEntry* entry_to_free = to_parent->SFXEntryList;
                to_parent->SFXEntryList = to_parent->SFXEntryList->next;
                smallObject_internal_dealloc(*HeapPtr, entry_to_free, sizeof(SFXEntry), 0x10);
            }
        }
        //handle if the game's list isn't long enough, and we need to alloc more slots
        else if (from_len > to_len)
        {
            ConsoleWrite("Game SFXEntryList too short. Alloc");
            for (to_len; to_len < from_len; to_len++)
            {
                SFXEntry* newEntry = (SFXEntry*)Game::game_smallObject_malloc(*HeapPtr, sizeof(SFXEntry), 0x10);
                newEntry->field0x48_head = NULL;
                newEntry->field0x48_tail = NULL;
                newEntry->field0xe0 = NULL;
                newEntry->field0xf0 = NULL;
                newEntry->next = to_parent->SFXEntryList;

                to_parent->SFXEntryList = newEntry;
            }
        }

        to = to_parent->SFXEntryList;
        while (from)
        {
            copy_SFXEntry(to, from, to_game);
            to->parent = to_parent;

            // last valid entry failsafe
            if (to == to_parent->SFXEntryList_tail && !(to->next == NULL && from->next == NULL))
            {
                ConsoleWrite("Warning: early abort at last valid entry. Some SFX lost");
                to->next = NULL;
                break;
            }

            from = from->next;
            to = to->next;
        }
    }
}

void copy_SFXEntry(SFXEntry* to, SFXEntry* from, bool to_game)
{
    to->vtable = 0x14151c278;
    to->field0x8 = NULL;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->parent = NULL;
    to->unk1 = NULL;
    to->unk2 = NULL; //TODO
    //leave next ptr alone
    if (from->field0x48_head != NULL)
    {
        if (to->field0x48_head == NULL)
        {
            if (to_game)
            {
                to->field0x48_head = (FXEntry_Substruct*)Game::game_smallObject_malloc(*HeapPtr, sizeof(FXEntry_Substruct), 8);
            }
            else
            {
                to->field0x48_head = init_FXEntry_Substruct();
            }
        }
        copy_FXEntry_Substruct(to->field0x48_head, from->field0x48_head, to_game, to);
        FXEntry_Substruct* tail = to->field0x48_head;
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
            smallObject_internal_dealloc(*HeapPtr, to->field0x48_head, sizeof(FXEntry_Substruct), 8);
        }
        else
        {
            free_FXEntry_Substruct(to->field0x48_head);
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
        free_FXEntry_Substruct(to->field0x48_head);
        head = (SFXEntry*)((uint64_t)(head)+sizeof(SFXEntry));
    }
    free(to);
}

std::string print_FXEntry_Substruct(FXEntry_Substruct* to)
{
    std::string out = "FXEntry_Substruct\n";
    if (to == NULL)
    {
        return out;
    }

    out += "Data 0:";
    for (size_t i = 0; i < sizeof(to->data_0); i++)
    {
        out += std::to_string(to->data_0[i]);
        out += " ";
    }
    out += "\n";

    out += "Data 1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += "Unk1:" + std::to_string((uint64_t)to->unk1) + "\n";

    out += "Data 2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "Unk5:" + std::to_string((uint64_t)to->unk5) + "\n";

    out += "Str len:" + std::to_string(to->str.len) + "\n";

    out += "Data 3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_FXEntry_Substruct(FXEntry_Substruct* to, FXEntry_Substruct* from, bool to_game, SFXEntry* parent)
{
    copy_FXEntry_Substruct_Obj(to, from, to_game, parent);

    if (from->linked != NULL)
    {
        if (to->linked == NULL)
        {
            if (to_game)
            {
                to->linked = (FXEntry_Substruct*)Game::game_smallObject_malloc(*HeapPtr, sizeof(FXEntry_Substruct), 8);
            }
            else
            {
                to->linked = init_FXEntry_Substruct();
            }
        }
        copy_FXEntry_Substruct(to->linked, from->linked, to_game, parent);
    }
    if (from->linked == NULL)
    {
        if (to->linked != NULL)
        {
            if (to_game)
            {
                FXEntry_Substruct_internal_dealloc(*HeapPtr, to->linked, sizeof(FXEntry_Substruct));
            }
            else
            {
                free_FXEntry_Substruct(to->linked);
            }
            to->linked = NULL;
        }
    }

    if (from->next != NULL)
    {
        if (to->next == NULL)
        {
            if (to_game)
            {
                to->next = (FXEntry_Substruct*)Game::game_smallObject_malloc(*HeapPtr, sizeof(FXEntry_Substruct), 8);
            }
            else
            {
                to->next = init_FXEntry_Substruct();
            }
        }
        copy_FXEntry_Substruct(to->next, from->next, to_game, parent);
    }
    if (from->next == NULL)
    {
        if (to->next != NULL)
        {
            if (to_game)
            {
                FXEntry_Substruct_internal_dealloc(*HeapPtr, to->next, sizeof(FXEntry_Substruct));
            }
            else
            {
                free_FXEntry_Substruct(to->next);
            }
            to->next = NULL;
        }
    }
}

void copy_FXEntry_Substruct_Obj(FXEntry_Substruct* to, FXEntry_Substruct* from, bool to_game, SFXEntry* parent)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->self_substruct2 = (uint64_t)to + 0xe0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    to->unk1 = from->unk1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    to->unk2 = NULL;
    //leave next ptr alone
    //leaved linked ptr alone
    to->unk5 = from->unk5;
    memcpy(&to->str, &from->str, sizeof(to->str));
    to->parent = parent;
    to->unk6 = NULL;
    to->vtable = 0x141519DA0;
    to->self = to;
    to->unk8 = NULL;
    to->unk9 = NULL;
    to->unk10 = NULL;
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));

#if 0
    FXEntry_Substruct* printer;
    const size_t len = 4092;
    size_t ofst = 0;
    char buf[len];
    if (!to_game)
    {
        printer = from;
    }
    else
    {
        printer = to;
    }

    ofst += snprintf(buf+ofst, len,"parent sfxid=%d addr=%p ", *(uint32_t*)(((uint64_t)parent) + 0x14), printer);
    for (size_t i = 0; i < sizeof(printer->data_0); i+=4)
    {
        ofst += snprintf(buf + ofst, len, "%x ", *(uint32_t*)(&printer->data_0[i]));
    }
    ofst += snprintf(buf + ofst, len, "self_substruct2 offset=%llx ", (uint64_t)printer->self_substruct2-(uint64_t)printer);
    for (size_t i = 0; i < sizeof(printer->data_1); i += 2)
    {
        ofst += snprintf(buf + ofst, len, "%x ", *(uint16_t*)(&printer->data_1[i]));
    }
    ofst += snprintf(buf + ofst, len, "%llx ", (uint64_t)printer->unk1);
    for (size_t i = 0; i < sizeof(printer->data_2); i += 2)
    {
        ofst += snprintf(buf + ofst, len, "%x ", *(uint16_t*)(&printer->data_2[i]));
    }
    ofst += snprintf(buf + ofst, len, "next=%llx ", (uint64_t)printer->next);
    ofst += snprintf(buf + ofst, len, "linked=%llx ", (uint64_t)printer->linked);
    ofst += snprintf(buf + ofst, len, "%llx ", (uint64_t)printer->unk5);
    ofst += snprintf(buf + ofst, len, "strptr=%llx ", (uint64_t)printer->str.ptr);
    ofst += snprintf(buf + ofst, len, "parent=%llx ", (uint64_t)printer->parent);
    for (size_t i = 0; i < sizeof(printer->data_3); i += 4)
    {
        ofst += snprintf(buf + ofst, len, "%x ", *(uint32_t*)(&printer->data_3[i]));
    }
    ConsoleWrite(buf);
#endif
}

FXEntry_Substruct* init_FXEntry_Substruct()
{
    FXEntry_Substruct* local_class_14150b808_field0x48 = (FXEntry_Substruct*)malloc_(sizeof(FXEntry_Substruct));

    return local_class_14150b808_field0x48;
}

void free_FXEntry_Substruct(FXEntry_Substruct* to)
{
    //This will be a real issue if there's a circular or duplicate link somewhere
    if (to->linked != NULL)
    {
        free_FXEntry_Substruct(to->linked);
        to->linked = NULL;
    }
    if (to->next != NULL)
    {
        free_FXEntry_Substruct(to->next);
        to->next = NULL;
    }
    free(to);
}
