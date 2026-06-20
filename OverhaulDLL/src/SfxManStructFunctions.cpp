#include "GameData.h"
#include "SfxManStructFunctions.h"
#include <unordered_map>

/* ============================================================
 * Graveyard management
 * ============================================================ */

//these objects are not ref counted, so we must do so manually
static std::unordered_map<void*, int32_t> g_sfxGraveyard;
static std::unordered_map<void*, int32_t> g_nodeGraveyard;


void SfxEntryRef(SFXEntry* SFXEntry)
{
    auto search = g_sfxGraveyard.find(SFXEntry);
    if (search != g_sfxGraveyard.end())
    {
        g_sfxGraveyard[SFXEntry] += 1;
    }
    else
    {
        g_sfxGraveyard.insert({ SFXEntry, 1 });
    }
}

void SfxEntryDeref(SFXEntry* SFXEntry)
{
    auto search = g_sfxGraveyard.find(SFXEntry);
    if (search != g_sfxGraveyard.end())
    {
        g_sfxGraveyard[SFXEntry] -= 1;
        if (g_sfxGraveyard[SFXEntry] == 0)
        {
            Destruct_SfxEntry(SFXEntry, 1);
        }
    }
}

bool OnSfxEntryDestruct(void* SFXEntry)
{
    auto search = g_sfxGraveyard.find(SFXEntry);
    if (search == g_sfxGraveyard.end())
    {
        //nothing keeping this alive, let it be destroyed
        return false;
    }
    //count a game-attempted destruction as a deref
    int32_t refcount = search->second;
    refcount -= 1;
    g_sfxGraveyard[SFXEntry] = refcount;
    if (refcount <= 0)
    {
        g_sfxGraveyard.erase(SFXEntry);
        return false;
    }
    return true;
}

void FxBehaviorNodeRef(FxBehaviorNode* FxBehaviorNode)
{
    auto search = g_nodeGraveyard.find(FxBehaviorNode);
    if (search != g_nodeGraveyard.end())
    {
        g_nodeGraveyard[FxBehaviorNode] += 1;
    }
    else
    {
        g_nodeGraveyard.insert({ FxBehaviorNode, 1 });
    }
}

void FxBehaviorNodeDeref(FxBehaviorNode* FxBehaviorNode)
{
    auto search = g_nodeGraveyard.find(FxBehaviorNode);
    if (search != g_nodeGraveyard.end())
    {
        g_nodeGraveyard[FxBehaviorNode] -= 1;
        if (g_nodeGraveyard[FxBehaviorNode] == 0)
        {
            Destruct_FxBehaviorNode(NULL, FxBehaviorNode, FxBehaviorNode_GetSize_Type1(FxBehaviorNode), FxBehaviorNode_GetSize_Type2(FxBehaviorNode));
        }
    }
}

bool OnFxBehaviorNodeDealloc(void* unused, void* FxBehaviorNode)
{
    auto search = g_nodeGraveyard.find(FxBehaviorNode);
    if (search == g_nodeGraveyard.end())
    {
        //nothing keeping this alive, let it be destroyed
        return false;
    }
    //count a game-attempted destruction as a deref
    int32_t refcount = search->second;
    refcount -= 1;
    g_nodeGraveyard[FxBehaviorNode] = refcount;
    if (refcount <= 0)
    {
        g_nodeGraveyard.erase(FxBehaviorNode);
        return false;
    }
    return true;
}


/* ============================================================ */


void copy_SfxMan(SfxMan* to, SfxMan* from, StateTarget target)
{
    copy_frpgFxManagerBase(to->FrpgFxManagerBase, from->FrpgFxManagerBase, target);
}

SfxMan* init_SfxMan()
{
    SfxMan* local = (SfxMan*)malloc_(sizeof(SfxMan));

    local->FrpgFxManagerBase = init_frpgFxManagerBase();

    return local;
}

void free_SfxMan(SfxMan* to)
{
    free_frpgFxManagerBase(to->FrpgFxManagerBase);
    free(to);
}


void copy_frpgFxManagerBase(frpgFxManagerBase* to, frpgFxManagerBase* from, StateTarget target)
{
    copy_FXManager(to->base.fXManager, from->base.fXManager, target);
}

frpgFxManagerBase* init_frpgFxManagerBase()
{
    frpgFxManagerBase* local = (frpgFxManagerBase*)malloc_(sizeof(frpgFxManagerBase));

    local->base.fXManager = init_FXManager();

    return local;
}

void free_frpgFxManagerBase(frpgFxManagerBase* to)
{
    free_FXManager(to->base.fXManager);
    free(to);
}


void copy_FXManager(FXManager* to, FXManager* from, StateTarget target)
{
    Game::SuspendThreads();

    to->vtable = from->vtable;
    to->parent = from->parent;
    to->heap = from->heap;
    to->FxBehaviorNode_destructlist_head = NULL;
    to->FxBehaviorNode_destructlist_tail = NULL;
    to->unk_38 = from->unk_38;
    to->unk_3c = from->unk_3c;
    memcpy(to->FXDrawEntityHandler_list, from->FXDrawEntityHandler_list, sizeof(to->FXDrawEntityHandler_list));
    memcpy(to->FxBehaviorNode_staging_list, from->FxBehaviorNode_staging_list, sizeof(to->FxBehaviorNode_staging_list));
    to->FxBehaviorNode_destruction_queue = (void*)1;
    memcpy(to->unk_88, from->unk_88, sizeof(to->unk_88));
    memcpy(to->unk_98, from->unk_98, sizeof(to->unk_98));
    memcpy(to->unk_a0, from->unk_a0, sizeof(to->unk_a0));
    to->unk_b0 = from->unk_b0;
    memcpy(to->unk_b1, from->unk_b1, sizeof(to->unk_b1));
    memcpy(to->FXSettingOptions, from->FXSettingOptions, sizeof(to->FXSettingOptions));
    memcpy(to->unk_17c, from->unk_17c, sizeof(to->unk_17c));
    memcpy(to->FXSettingScales, from->FXSettingScales, sizeof(to->FXSettingScales));
    to->FrpgFxAdapterBase = from->FrpgFxAdapterBase;

    switch (target)
    {
    case StateTarget::ToLocal:
        Save_SFXEntryList(&to->saved_entries, from);
        break;
    case StateTarget::ToGame:
        Restore_SFXEntryList(to, &from->saved_entries);
        break;
    case StateTarget::Copy:
        Copy_SFXEntryList(&to->saved_entries, &from->saved_entries);
        break;
    }
    Game::ResumeThreads();
}

FXManager* init_FXManager()
{
    FXManager* local_FXManager = (FXManager*)malloc_(sizeof(FXManager));

    return local_FXManager;
}

void free_FXManager(FXManager* to)
{
    Clear_SFXEntryList(&to->saved_entries);
    free(to);
}


void Save_SFXEntryList(std::vector<SavedSFXEntry>* to, FXManager* from)
{
    to->clear();

    SFXEntry* head = from->SFXEntryList;
    while (head != NULL)
    {
        SavedSFXEntry ss;
        ss.game_addr = head;
        SfxEntryRef(head); //ref for the game-side storage
        SfxEntryRef(head); //ref for the local-side storage
        copy_SFXEntry(&ss.data, head, StateTarget::ToLocal);

        FxBehaviorNode* node_head = head->base.behaviour_list;
        if (node_head != NULL)
        {
            ss.behaviour_list_head = new SavedFxBehaviorNode();
            Save_SavedFxBehaviorNodeGraph(ss.behaviour_list_head, node_head);
        }

        to->push_back(ss);
        head = head->base.next;
    }
}

void Restore_SFXEntryList(FXManager* to, std::vector<SavedSFXEntry>* from)
{
    //clear out everything, then insert.
    //this may mean we remove and put back the same element but since those are still ref'd by the saved side it's safe
    SFXEntry* head = to->SFXEntryList;
    while (head != NULL)
    {
        SFXEntry* next = head->base.next;

        std::vector<FxBehaviorNode*> graph_elems;
        if (head->base.behaviour_list != NULL)
        {
            graph_elems.push_back(head->base.behaviour_list);
        }
        while (!graph_elems.empty())
        {
            FxBehaviorNode* node = graph_elems.back();
            graph_elems.pop_back();

            if (node->next_node != NULL)
            {
                graph_elems.push_back(node->next_node);
            }
            if (node->child_node != NULL)
            {
                graph_elems.push_back(node->child_node);
            }
            FxBehaviorNodeDeref(node);
        }
        head->base.behaviour_list = NULL;
        head->base.behaviour_list_end = NULL;

        SfxEntryDeref(head);
        head = next;
    }
    to->SFXEntryList = NULL;
    to->SFXEntryList_tail = NULL;

    //insert elements. Do in reverse order so at the end the first element is put in the head
    for (auto si = from->rbegin(); si != from->rend(); si++)
    {
        SavedSFXEntry s = *si;
        SFXEntry* old_head = to->SFXEntryList;
        //update the next pointer for this element
        SFXEntry* new_head = s.game_addr;
        new_head->base.next = old_head;
        //update the data
        copy_SFXEntry(new_head, &s.data, StateTarget::ToGame);
        //insert into list
        to->SFXEntryList = new_head;
        //ref count now that it's in the game
        SfxEntryRef(s.game_addr);

        if (s.behaviour_list_head != NULL)
        {
            Restore_SavedFxBehaviorNodeGraph(&new_head->base.behaviour_list, s.behaviour_list_head);
        }
        FxBehaviorNode* last_node = new_head->base.behaviour_list;
        while (last_node != NULL)
        {
            new_head->base.behaviour_list_end = last_node;
            last_node = last_node->next_node;
        }
    }
    if (!from->empty())
    {
        to->SFXEntryList_tail = from->back().game_addr;
    }
}

void Copy_SFXEntryList(std::vector<SavedSFXEntry>* to, std::vector<SavedSFXEntry>* from)
{
    // Clean the dst vectors
    Clear_SFXEntryList(to);

    // Deep copy the vectors, and Ref everything in the new copy
    for (auto& s : *from)
    {
        SavedSFXEntry new_s;
        new_s.game_addr = s.game_addr;
        copy_SFXEntry(&new_s.data, &s.data, StateTarget::Copy);
        SfxEntryRef(new_s.game_addr);
        if (s.behaviour_list_head != NULL)
        {
            new_s.behaviour_list_head = new SavedFxBehaviorNode();
            Copy_SavedFxBehaviorNodeGraph(new_s.behaviour_list_head, s.behaviour_list_head);
        }
        else
        {
            new_s.behaviour_list_head = NULL;
        }
        to->push_back(new_s);
    }
}

void Clear_SFXEntryList(std::vector<SavedSFXEntry>* to)
{
    for (auto e : *to)
    {
        SfxEntryDeref(e.game_addr);
        Free_SFXEntry(&e.data, false);
        if (e.behaviour_list_head != NULL)
        {
            Clear_SavedFxBehaviorNodeGraph(e.behaviour_list_head);
            delete e.behaviour_list_head;
        }
    }
    to->clear();
}


void copy_SFXEntry(SFXEntry* to, SFXEntry* from, StateTarget target)
{
    to->base.vtable = from->base.vtable;
    to->base.unk_8 = from->base.unk_8;
    to->base.unk_10 = from->base.unk_10;
    to->base.sfxId = from->base.sfxId;
    to->base.unk_18 = from->base.unk_18;
    to->base.unk_1c = from->base.unk_1c;
    to->base.unk_20 = from->base.unk_20;
    to->base.fxParent = from->base.fxParent;
    //TODO dunno if i need to save any of these pointers i've commented out
    //vec
    //list
    to->base.next = from->base.next; //safe to copy since we are graveyarding the pointers
    //ignore behaviour_list in this function
    memcpy(to->base.data_1, from->base.data_1, sizeof(to->base.data_1));
    to->unk_60 = from->unk_60;
    to->unk_68 = from->unk_68;
    to->unk_6c = from->unk_6c;
    to->unk_70 = from->unk_70;
    to->unk_74 = from->unk_74;
    to->unk_78 = from->unk_78;
    to->unk_7c = from->unk_7c;
    memcpy(to->field0x80, from->field0x80, sizeof(to->field0x80));
    memcpy(to->field0xb0, from->field0xb0, sizeof(to->field0xb0));
    //field_0xe0
    //field_0xf0
    to->unk_f8 = from->unk_f8;
    to->unk_fc = from->unk_fc;
    memcpy(to->unk_fd, from->unk_fd, sizeof(to->unk_fd));
}

void Free_SFXEntry(SFXEntry* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}


void Save_SavedFxBehaviorNodeGraph(SavedFxBehaviorNode* to, FxBehaviorNode* from)
{
    to->game_addr = from;
    copy_FxBehaviorNode(&to->data, from, StateTarget::ToLocal);
    FxBehaviorNodeRef(from); //ref for game-side
    FxBehaviorNodeRef(from); //ref for local-side

    if (from->next_node != NULL)
    {
        to->next_node = new SavedFxBehaviorNode();
        Save_SavedFxBehaviorNodeGraph(to->next_node, from->next_node);
    }
    if (from->child_node != NULL)
    {
        to->child_node = new SavedFxBehaviorNode();
        Save_SavedFxBehaviorNodeGraph(to->child_node, from->child_node);
    }
}

void Restore_SavedFxBehaviorNodeGraph(FxBehaviorNode** to, SavedFxBehaviorNode* from)
{
    *to = from->game_addr;
    copy_FxBehaviorNode(*to, &from->data, StateTarget::ToGame);
    FxBehaviorNodeRef(from->game_addr);

    if (from->next_node != NULL)
    {
        Restore_SavedFxBehaviorNodeGraph(&(*to)->next_node, from->next_node);
    }
    else
    {
        (*to)->next_node = NULL;
    }
    if (from->child_node != NULL)
    {
        Restore_SavedFxBehaviorNodeGraph(&(*to)->child_node, from->child_node);
    }
    else
    {
        (*to)->child_node = NULL;
    }
}

void Copy_SavedFxBehaviorNodeGraph(SavedFxBehaviorNode* to, SavedFxBehaviorNode* from)
{
    to->game_addr = from->game_addr;
    copy_FxBehaviorNode(&to->data, &from->data, StateTarget::Copy);
    FxBehaviorNodeRef(to->game_addr);

    if (from->next_node != NULL)
    {
        to->next_node = new SavedFxBehaviorNode();
        Copy_SavedFxBehaviorNodeGraph(to->next_node, from->next_node);
    }
    else
    {
        to->next_node = NULL;
    }
    if (from->child_node != NULL)
    {
        to->child_node = new SavedFxBehaviorNode();
        Copy_SavedFxBehaviorNodeGraph(to->child_node, from->child_node);
    }
    else
    {
        to->child_node = NULL;
    }
}

void Clear_SavedFxBehaviorNodeGraph(SavedFxBehaviorNode* to)
{
    FxBehaviorNodeDeref(to->game_addr);
    Free_FxBehaviorNode(&to->data, false);
    if (to->next_node != NULL)
    {
        Clear_SavedFxBehaviorNodeGraph(to->next_node);
        delete to->next_node;
        to->next_node = NULL;
    }
    if (to->child_node != NULL)
    {
        Clear_SavedFxBehaviorNodeGraph(to->child_node);
        delete to->child_node;
        to->child_node = NULL;
    }
}


void copy_FxBehaviorNode(FxBehaviorNode* to, FxBehaviorNode* from, StateTarget target)
{
    memcpy(to->unk_0, from->unk_0, sizeof(to->unk_0));
    memcpy(to->unk_20, from->unk_20, sizeof(to->unk_20));
    to->unk_1 = from->unk_1;
    //body !!!
    to->body = from->body;
    to->flags1 = from->flags1;
    to->flags2 = from->flags2;
    to->unk_68 = from->unk_68;
    //TODO dunno if i need to save any of these pointers i've commented out
    //unk_2
    to->unk_78 = from->unk_78;
    to->unk_7c = from->unk_7c;
    to->parent_node_updated = from->parent_node_updated;
    memcpy(to->unk_81, from->unk_81, sizeof(to->unk_81));
    to->unk_84 = from->unk_84;
    to->unk_85 = from->unk_85;
    memcpy(to->unk_86, from->unk_86, sizeof(to->unk_86));
    to->parent_node = from->parent_node;
    to->next_node = from->next_node;
    to->child_node = from->child_node;
    //unk_3;
    //unk_4;
    //_0 !!!
    to->parent = from->parent;
    to->destruction_queue_next = from->destruction_queue_next;
}

void Free_FxBehaviorNode(FxBehaviorNode* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}
