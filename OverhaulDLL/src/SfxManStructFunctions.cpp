#include "GameData.h"
#include "SfxManStructFunctions.h"
#include <unordered_map>

/* ============================================================
 * Graveyard management
 * ============================================================ */

//these objects are not ref counted, so we must do so manually
static std::unordered_map<void*, uint32_t> g_sfxGraveyard;
static std::unordered_map<void*, uint32_t> g_nodeGraveyard;


void SfxEntryRef(void* SFXEntry)
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

void SfxEntryDeref(void* SFXEntry)
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

void FxBehaviorNodeRef(void* FxBehaviorNode)
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

void FxBehaviorNodeDeref(void* FxBehaviorNode)
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
        Save_SFXEntryList(to->saved_entries, from);
        break;
    case StateTarget::ToGame:
        Restore_SFXEntryList(to, from->saved_entries);
        break;
    case StateTarget::Copy:
        Copy_SFXEntryList(to->saved_entries, from->saved_entries);
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
    for (auto e : to->saved_entries) {
        SfxEntryDeref(e.game_addr);
        for (auto n : e.nodes)
        {
            FxBehaviorNodeDeref(n);
        }
    }
    free(to);
}

void Save_SFXEntryList(std::vector<SavedSFXEntry> to, FXManager* from)
{
    to.clear();

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

        to.push_back(e);
        head = head->base.next;
    }
}

void Restore_SFXEntryList(FXManager* to, std::vector<SavedSFXEntry> from)
{

}

void Copy_SFXEntryList(std::vector<SavedSFXEntry> to, std::vector<SavedSFXEntry> from)
{

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
