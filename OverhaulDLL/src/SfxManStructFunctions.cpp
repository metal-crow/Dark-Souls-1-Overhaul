#include "GameData.h"
#include "SfxManStructFunctions.h"
#include <unordered_map>

/* ============================================================
 * Graveyard management
 * ============================================================ */

//these objects are not ref counted, so we must do so manually
static std::unordered_map<void*, uint32_t> g_sfxGraveyard;
static std::unordered_map<void*, uint32_t> g_nodeGraveyard;


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
    uint32_t refcount = search->second;
    return refcount > 0;
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
            Destruct_FxBehaviorNode(FxBehaviorNode, 1);
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
    uint32_t refcount = search->second;
    return refcount > 0;
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

    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    to->FxBehaviorNode_destructlist_head = NULL;
    to->FxBehaviorNode_destructlist_tail = NULL;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->FXDrawEntityHandler_list, from->FXDrawEntityHandler_list, sizeof(to->FXDrawEntityHandler_list));
    memcpy(to->FxBehaviorNode_staging_list, from->FxBehaviorNode_staging_list, sizeof(to->FxBehaviorNode_staging_list));
    to->FxBehaviorNode_destruction_queue = (void*)1;
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
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
        SavedSFXEntry e;
        e.game_addr = head;
        SfxEntryRef(head);
        copy_SFXEntry(&e.data, head, StateTarget::ToLocal);

        FxBehaviorNode* node_head = head->base.behaviour_list;
        while (node_head != NULL)
        {
            e.nodes.push_back(node_head);
            FxBehaviorNodeRef(node_head);
            node_head = node_head->next;
        }

        to->push_back(e);
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
        FxBehaviorNode* node_head = head->base.behaviour_list;
        while (node_head != NULL)
        {
            FxBehaviorNode* node_head_next = node_head->next;
            FxBehaviorNodeDeref(node_head);
            node_head = node_head_next;
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
        SfxEntryRef(new_head);

        for (auto ni = s.nodes.rbegin(); ni != s.nodes.rend(); ni++)
        {
            FxBehaviorNode* new_node_head = *ni;
            FxBehaviorNode* old_node_head = new_head->base.behaviour_list;
            //update next ptr
            new_node_head->next = old_node_head;
            //insert
            new_head->base.behaviour_list = new_node_head;
            //ref count
            FxBehaviorNodeRef(new_node_head);
        }
        new_head->base.behaviour_list_end = s.nodes.back();
    }
    to->SFXEntryList_tail = from->back().game_addr;
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
        for (auto& n : s.nodes)
        {
            new_s.nodes.push_back(n);
            FxBehaviorNodeRef(n);
        }
        to->push_back(new_s);
    }
}

void Clear_SFXEntryList(std::vector<SavedSFXEntry>* to)
{
    for (auto e : *to)
    {
        SfxEntryDeref(e.game_addr);
        for (auto n : e.nodes)
        {
            FxBehaviorNodeDeref(n);
        }
        e.nodes.clear();
    }
    to->clear();
}

void copy_SFXEntry(SFXEntry* to, SFXEntry* from, StateTarget target)
{
    memcpy(to->base.data_0, from->base.data_0, sizeof(to->base.data_0));
    to->base.fxParent = from->base.fxParent;
    //TODO dunno if i need to save any of these pointers i've commented out
    //vec
    //list
    to->base.next = from->base.next; //safe to copy since we are graveyarding the pointers
    //ignore behaviour_list in this function
    memcpy(to->base.data_1, from->base.data_1, sizeof(to->base.data_1));
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    //field_0xe0
    //field_0xf0
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}
