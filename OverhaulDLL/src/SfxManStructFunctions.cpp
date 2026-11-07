#include "GameData.h"
#include "SfxManStructFunctions.h"
#include "StateSerializer.h"
#include "SP/memory/injection/asm/x64.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
 * SfxMan rollback: reconcile the FX system with the restored state instead of saving it.
 *
 * Effects (FxBehaviorNode trees) are cosmetic, aged only by Step_SfxMan in live frames, and own render and sound objects, so
 * they are not saved. What rollback must keep consistent are the FX handles (SfxHandle) inside the structs it
 * does save -- bullets and attach slots. A handle points at an effect's node (node, or follow_node once its root
 * is gone), and every node's body keeps a doubly linked list of the handles pointing at it, which the game walks and writes
 * through when the node dies. A handle restored by plain copy leaves that list naming memory that no longer holds the handle.
 *
 *  - Every node is registered when it is created and forgotten when it is freed. This is done by FrpgFxAdapterBase vtable
 *    slot 10 Init_FxBehaviorNode, and 11 Destruct_FxBehaviorNode. We replace these with wrappers
 *    (Init_FxBehaviorNode_hook and Destruct_FxBehaviorNode_hook) which register the nodes
 *    with a serial number, the frame and session it was spawned in, and whether a rolled-back simulation step spawned it.
 * 
 *  - Saving a handle records the serials of the nodes it is linked to. To load a handle, we first takes the game's handle out of
 *    its node's list, then links it to the saved node if that node still exists (same address, same serial, still alive),
 *    or leaves it empty, as the game leaves a handle whose effect ended.
 * 
 *  - When we load a frame, we have to kill the effects the rolled-back simulation (Step_Chr, Step_Bullet, Step_DamageMan, sfx_sim_step_depth)
 *    spawned in the frame(s) we discard. The re-simulation spawns them again if they still happen. Otherwise we could have effects
 *    sticking around that aren't attached to anything.
 * 
 *  - Stops (deleting an effect) issued by the game (such as SfxHandle_KillAndClear) wait until their frame is confirmed, to
 *    prevent a rollback that causes the stop to never trigger making the effect to end prematurly.
 *    Until the confirm we hide the effect (header flags1 bit sfx_hide_bit). Then either a load that discards the frame
 *    un-hides it and the restored handles link back to it, or the confirm deletes it for good.
 *
 * A handle is in exactly one list: node's when that is set
 */

// ---- game functions ----

typedef void FXManager_Node_FUNC(FXManager* mgr, FxBehaviorNode* node);
//FUN_140f0b320: kill one node. It is unlinked from its tree (its children move up to its parent, or become roots), its body is
//torn down at once (detaching every handle), and the node is queued for the destruct list
static FXManager_Node_FUNC* FXManager_KillNode = (FXManager_Node_FUNC*)0x140f0b320;
//FUN_140f0b760: kill every descendant of a node, leaves first
static FXManager_Node_FUNC* FXManager_KillDescendants = (FXManager_Node_FUNC*)0x140f0b760;

typedef void FXManager_NodeFlag_FUNC(FXManager* mgr, FxBehaviorNode* node, uint32_t bit);
//clear a flags1 bit on a node and its descendants (hides them)
static FXManager_NodeFlag_FUNC* FXManager_HideNode = (FXManager_NodeFlag_FUNC*)0x140f0c420;
//Unclear the above
static FXManager_NodeFlag_FUNC* FXManager_ShowNode = (FXManager_NodeFlag_FUNC*)0x140f0d7f0;

//FUN_140fdc690: stop the handle's effect emitting (it fades out by itself). Hooked, see sfx_defer_handle_soft_stop
typedef void SfxHandle_SoftStop_FUNC(SfxHandle* h);
static SfxHandle_SoftStop_FUNC* SfxHandle_SoftStop = (SfxHandle_SoftStop_FUNC*)0x140fdc690;

//The bit a pending kill clears to hide its effect. The game uses bits 0 and 31 for this; 29 and 30 mean other things.
static const uint32_t sfx_hide_bit = 28;

// ---- hooks ----

//The adapter vtables whose slot 10 is Init_FxBehaviorNode (140fde7b0) and slot 11 Destruct_FxBehaviorNode (140fdf530):
//FrpgFxAdapterBase's own, and the derived class SfxMan's FXManager actually uses (the object's vtable at runtime)
static const uint64_t FxAdapter_vtable_offsets[] = { 0x15193f0, 0x136a9e0 };
static const uint64_t Init_FxBehaviorNode_offset = 0xfde7b0;
static const uint64_t Destruct_FxBehaviorNode_offset = 0xfdf530;

//MoveMapStep_Step_13's calls to the steps the re-simulation repeats (Game::Step_GameSimulation)
//Instrument these so that our hooks know we're inside rollback frames
static const uint64_t MoveMapStep_Step_Chr_offset = 0x24fb07;        //movzx r8d,r12b / movaps xmm1,xmm7 / mov rcx,rsi / call Step_Chr
static const uint64_t MoveMapStep_Step_Bullet_offset = 0x24fc48;     //movaps xmm1,xmm7 / mov rcx,[BulletMan] / call Step_Bullet
static const uint64_t MoveMapStep_Step_DamageMan_offset = 0x2500ce;  //movaps xmm1,xmm7 / mov rcx,[DamageMan] / call Step_DamageMan

//The handle stop functions. Each starts push reg / sub rsp,20h / mov reg,rcx / mov rcx|rdx,[rcx+..] / test
static const uint64_t SfxHandle_Kill_offset = 0xfdc930;      //FUN_140fdc930: kill node, follow its first child
static const uint64_t SfxHandle_Reset_offset = 0xfdc760;     //SfxHandle_KillAndClear: kill the node and its next siblings, clear
static const uint64_t SfxHandle_SoftStop_offset = 0xfdc690;  //FUN_140fdc690

extern "C" {
    int32_t sfx_sim_step_depth = 0;

    uint64_t SfxSimStep_Chr_return;
    void SfxSimStep_Chr_injection();
    uint64_t SfxSimStep_Bullet_return;
    void SfxSimStep_Bullet_injection();
    uint64_t SfxSimStep_DamageMan_return;
    void SfxSimStep_DamageMan_injection();

    uint64_t SfxHandleKill_return;
    void SfxHandleKill_injection();
    bool sfx_defer_handle_kill(SfxHandle* h);
    uint64_t SfxHandleReset_return;
    void SfxHandleReset_injection();
    bool sfx_defer_handle_reset(SfxHandle* h);
    uint64_t SfxHandleSoftStop_return;
    void SfxHandleSoftStop_injection();
    bool sfx_defer_handle_soft_stop(SfxHandle* h);
}

// ---- node registry ----

struct SfxNodeInfo
{
    uint64_t serial;
    int32_t spawn_frame;   //sfx_sim_frame when it was created
    uint32_t session;      //sfx_session when it was created, 0 for one registered on sight
    bool sim_spawned;      //created inside a rolled-back simulation step
};

static std::unordered_map<FxBehaviorNode*, SfxNodeInfo> sfx_nodes;
static SRWLOCK sfx_nodes_lock = SRWLOCK_INIT;
static uint64_t sfx_next_serial = 1;

static uint32_t sfx_session = 0;
static bool sfx_session_active = false;
//sfx_sim_step_depth counts the main thread's steps: an FX call from another thread is never inside one
static DWORD sfx_main_thread = 0;
//The GGPO frame being simulated: the last frame saved or loaded. -1 until the session's first save.
static int32_t sfx_sim_frame = -1;

//What the session did, logged when it ends
static struct
{
    uint64_t handles_relinked;     //a load linked a handle back to its effect
    uint64_t handles_emptied;      //a load found a handle's effect gone
    uint64_t spawns_killed;        //effects the discarded frames spawned
    uint64_t stops_deferred;
    uint64_t stops_executed;       //after their frame was confirmed
    uint64_t stops_cancelled;      //a load discarded the frame that issued them
} sfx_stats;

typedef void Init_FxBehaviorNode_FUNC(void* adapter, FxBehaviorNodeBody* parent_body, uint8_t p3, uint8_t p4, void* descriptor, FxBehaviorNode** node_out, FxBehaviorNodeBody** body_out);
typedef void Destruct_FxBehaviorNode_FUNC(void* adapter, FxBehaviorNode* node, uint8_t p3, uint8_t p4);
static Init_FxBehaviorNode_FUNC* Init_FxBehaviorNode_original = NULL;
static Destruct_FxBehaviorNode_FUNC* Destruct_FxBehaviorNode_original = NULL;

static void Init_FxBehaviorNode_hook(void* adapter, FxBehaviorNodeBody* parent_body, uint8_t p3, uint8_t p4, void* descriptor, FxBehaviorNode** node_out, FxBehaviorNodeBody** body_out)
{
    Init_FxBehaviorNode_original(adapter, parent_body, p3, p4, descriptor, node_out, body_out);
    FxBehaviorNode* node = *node_out;
    if (node == NULL)
    {
        return;
    }
    AcquireSRWLockExclusive(&sfx_nodes_lock);
    //a new node, whatever an entry at this address said before
    const bool sim_spawned = sfx_sim_step_depth > 0 && GetCurrentThreadId() == sfx_main_thread;
    sfx_nodes[node] = { sfx_next_serial++, sfx_sim_frame, sfx_session, sim_spawned };
    ReleaseSRWLockExclusive(&sfx_nodes_lock);
}

static void Destruct_FxBehaviorNode_hook(void* adapter, FxBehaviorNode* node, uint8_t p3, uint8_t p4)
{
    AcquireSRWLockExclusive(&sfx_nodes_lock);
    sfx_nodes.erase(node);
    ReleaseSRWLockExclusive(&sfx_nodes_lock);
    Destruct_FxBehaviorNode_original(adapter, node, p3, p4);
}

static bool sfx_node_alive(const FxBehaviorNode* n)
{
    return (n->flags1 & FxBehaviorNode_flags1_alive) != 0;
}

//Sanity check function. Check if a registered node still has valid pointers. Defensive check for freed/reused memory
static void sfx_check_node(const FxBehaviorNode* n)
{
    const FxBehaviorNodeBody* body = (const FxBehaviorNodeBody*)((const uint8_t*)n + sizeof(FxBehaviorNode));
    if (n->body != body || body->self != n)
    {
        FATALERROR("SfxMan rollback: registered FX node %p is no longer a node (body=%p, expected %p, body->self=%p)",
            n, n->body, body, body->self);
    }
}

//Return the serial number for the given node, if present in the nodes list. Otherwise return 0.
static uint64_t sfx_node_serial(FxBehaviorNode* n)
{
    if (n == NULL)
    {
        return 0;
    }
    uint64_t serial = 0;
    AcquireSRWLockShared(&sfx_nodes_lock);
    auto it = sfx_nodes.find(n);
    if (it != sfx_nodes.end())
    {
        serial = it->second.serial;
    }
    ReleaseSRWLockShared(&sfx_nodes_lock);
    if (serial != 0)
    {
        sfx_check_node(n);
    }
    return serial;
}

//Check if the given node and serial match and the node is alive
static bool sfx_node_is(FxBehaviorNode* n, uint64_t serial)
{
    if (n == NULL || serial == 0)
    {
        return false;
    }
    AcquireSRWLockShared(&sfx_nodes_lock);
    auto it = sfx_nodes.find(n);
    const bool same = it != sfx_nodes.end() && it->second.serial == serial;
    ReleaseSRWLockShared(&sfx_nodes_lock);
    if (!same)
    {
        return false;
    }
    sfx_check_node(n);
    return sfx_node_alive(n);
}

// ---- handle lists ----

static const size_t sfx_handle_list_max = 4096;

//Check if the sfx list for the node contains this handle
static bool sfx_list_contains(FxBehaviorNodeBody* body, SfxHandle* h)
{
    size_t n = 0;
    for (SfxHandle* p = body->linked_sfxHandle; p != NULL; p = p->list_prev)
    {
        if (p == h)
        {
            return true;
        }
        if (++n > sfx_handle_list_max)
        {
            FATALERROR("SfxMan rollback: the handle list of FX node %p is longer than %zu (corrupt)", body->self, sfx_handle_list_max);
        }
    }
    return false;
}

//Replicated code for FxBehaviorNodeBody_Remove_SfxHandle
static void sfx_list_unlink(FxBehaviorNodeBody* body, SfxHandle* h)
{
    if (h->list_next != NULL)
    {
        h->list_next->list_prev = h->list_prev;
    }
    if (h->list_prev != NULL)
    {
        h->list_prev->list_next = h->list_next;
    }
    if (body->linked_sfxHandle == h)
    {
        body->linked_sfxHandle = h->list_prev;
    }
    h->list_next = NULL;
    h->list_prev = NULL;
}

//Replicated code for FxBehaviorNodeBody_PushBack_SfxHandle
static void sfx_list_push(FxBehaviorNodeBody* body, SfxHandle* h)
{
    h->list_next = NULL;
    h->list_prev = body->linked_sfxHandle;
    if (body->linked_sfxHandle != NULL)
    {
        body->linked_sfxHandle->list_next = h;
    }
    body->linked_sfxHandle = h;
}

//The node a game handle is really linked to, or NULL. A handle the game destructed keeps its pointers, so make sure it's alive.
static FxBehaviorNode* sfx_handle_linked_node(SfxHandle* h)
{
    FxBehaviorNode* n = h->node != NULL ? h->node : h->follow_node;
    if (sfx_node_serial(n) != 0 && sfx_list_contains(n->body, h))
    {
        return n;
    }
    return NULL;
}

void release_SfxHandle(SfxHandle* h)
{
    //check both: a stale handle can name a node it is not in, and must not touch that node's list
    FxBehaviorNode* nodes[2] = { h->node, h->follow_node };
    for (FxBehaviorNode* n : nodes)
    {
        if (sfx_node_serial(n) != 0 && sfx_list_contains(n->body, h))
        {
            sfx_list_unlink(n->body, h);
        }
    }
    clear_SfxHandle(h);
}

void clear_SfxHandle(SfxHandle* h)
{
    h->fx_manager = NULL;
    h->node = NULL;
    h->list_next = NULL;
    h->list_prev = NULL;
    h->follow_node = NULL;
}

// ---- saving handles ----

static int32_t sfx_node_sfx_id(const FxBehaviorNode* n)
{
    return (n != NULL && n->parent != NULL) ? (int32_t)n->parent->base.sfxId : -1;
}

//A local handle keeps the game's pointer to the node it is linked to (node, or follow_node once node has died; never both).
//list_next means nothing outside the game, so it holds that node's serial.
static uint64_t sfx_saved_serial(const SfxHandle* h) { return (uint64_t)h->list_next; }

void copy_SfxHandle(SfxHandle* to, const SfxHandle* from, StateTarget target)
{
    switch (target)
    {
    case StateTarget::ToLocal:
    {
        SfxHandle* game = (SfxHandle*)from;
        to->vtable = game->vtable;
        clear_SfxHandle(to);
        FxBehaviorNode* linked = sfx_handle_linked_node(game);
        if (linked == NULL)
        {
            break;
        }
        to->fx_manager = game->fx_manager;
        to->node = game->node;
        to->follow_node = game->node != NULL ? NULL : game->follow_node;
        to->list_next = (SfxHandle*)sfx_node_serial(linked);
        break;
    }
    case StateTarget::ToGame:
    {
        release_SfxHandle(to);
        to->vtable = from->vtable;
        FxBehaviorNode* a = sfx_node_is(from->node, sfx_saved_serial(from)) ? from->node : NULL;
        //the game zeroes a handle whose node dies, rather than moving it on to follow_node
        FxBehaviorNode* b = (from->node == NULL && sfx_node_is(from->follow_node, sfx_saved_serial(from))) ? from->follow_node : NULL;
        if (a == NULL && b == NULL)
        {
            if (from->node != NULL || from->follow_node != NULL)
            {
                sfx_stats.handles_emptied++;
            }
            break;
        }
        sfx_stats.handles_relinked++;
        to->fx_manager = from->fx_manager;
        to->node = a;
        to->follow_node = b;
        sfx_list_push((a != NULL ? a : b)->body, to);
        break;
    }
    case StateTarget::Copy:
        *to = *from;
        break;
    }
}

void serialize_SfxHandle(StateVisitor& v, const char* name, const SfxHandle* h)
{
    v.begin(name);
    v.field("vtable", h->vtable);
    //which effect it points at, and whether that effect is still alive, is FX state, not compared. Effects only age in live
    //frames, so this differs between a live and a re-simulated frame.
    v.excluded("fx_link", sizeof(SfxHandle) - sizeof(h->vtable));
    v.end();
}

// ---- deferred stops ----

enum class SfxStopKind : uint8_t
{
    KillRoot,   //FUN_140fdc930: the node dies, its children carry on as roots
    KillTree,   //SfxHandle_KillAndClear: the node and all its descendants die
    SoftStop,   //FUN_140fdc690: the effect stops emitting
};

struct SfxPendingStop
{
    SfxStopKind kind;
    int32_t frame;
    FxBehaviorNode* node;
    uint64_t serial;
    FXManager* mgr;
    uint64_t handle_vtable;   //SoftStop: for the handle it is replayed through
};

//list of sfx that are queued to be stopped, and are currently hidden
static std::vector<SfxPendingStop> sfx_pending;
static bool sfx_bypass_defer = false;

static bool sfx_defer_now()
{
    return sfx_session_active && sfx_sim_frame >= 0 && sfx_sim_step_depth > 0 && !sfx_bypass_defer && GetCurrentThreadId() == sfx_main_thread;
}

static void sfx_hide_pending_kills()
{
    for (const SfxPendingStop& p : sfx_pending)
    {
        if (p.kind != SfxStopKind::SoftStop && sfx_node_is(p.node, p.serial))
        {
            FXManager_HideNode(p.mgr, p.node, sfx_hide_bit);
        }
    }
}

static void sfx_execute_stop(const SfxPendingStop& p)
{
    if (!sfx_node_is(p.node, p.serial))
    {
        return;   //ended (or was killed) meanwhile
    }
    sfx_stats.stops_executed++;
    switch (p.kind)
    {
    case SfxStopKind::KillRoot:
        //its children outlive it, so they must not stay hidden
        FXManager_ShowNode(p.mgr, p.node, sfx_hide_bit);
        FXManager_KillNode(p.mgr, p.node);
        break;
    case SfxStopKind::KillTree:
        FXManager_KillDescendants(p.mgr, p.node);
        FXManager_KillNode(p.mgr, p.node);
        break;
    case SfxStopKind::SoftStop:
    {
        SfxHandle h = {};
        h.vtable = p.handle_vtable;
        h.fx_manager = p.mgr;
        h.node = p.node;
        sfx_bypass_defer = true;
        SfxHandle_SoftStop(&h);   //reads the handle only: it is never linked
        sfx_bypass_defer = false;
        break;
    }
    }
}

//Run the stops issued in frames <= last (all of them for INT32_MAX), oldest first
static void sfx_execute_stops_through(int32_t last)
{
    if (sfx_pending.empty())
    {
        return;
    }
    std::vector<SfxPendingStop> due;
    std::vector<SfxPendingStop> rest;
    for (const SfxPendingStop& p : sfx_pending)
    {
        (p.frame <= last ? due : rest).push_back(p);
    }
    if (due.empty())
    {
        return;
    }
    sfx_pending.swap(rest);
    for (const SfxPendingStop& p : due)
    {
        sfx_execute_stop(p);
    }
    //showing a killed root showed its whole subtree, which may include nodes other pending kills hide
    sfx_hide_pending_kills();
}

//FUN_140fdc930 (h): if node is alive, kill it and follow its first child with follow_node
bool sfx_defer_handle_kill(SfxHandle* h)
{
    if (!sfx_defer_now())
    {
        return false;
    }
    FxBehaviorNode* a = h->node;
    FXManager* mgr = h->fx_manager;
    if (mgr == NULL || a == NULL || !sfx_node_alive(a) || a->body == NULL)
    {
        return false;   //the original only clears the handle, or does nothing
    }
    const uint64_t serial = sfx_node_serial(a);
    if (serial == 0)
    {
        return false;
    }
    //leave the handle as the original does
    FxBehaviorNode* child = a->child_node;
    if (sfx_list_contains(a->body, h))
    {
        sfx_list_unlink(a->body, h);
    }
    clear_SfxHandle(h);
    if (child != NULL)
    {
        h->fx_manager = mgr;
        h->follow_node = child;
        sfx_list_push(child->body, h);
    }
    FXManager_HideNode(mgr, a, sfx_hide_bit);
    sfx_pending.push_back({ SfxStopKind::KillRoot, sfx_sim_frame, a, serial, mgr, 0 });
    sfx_stats.stops_deferred++;
    return true;
}

//SfxHandle_KillAndClear (h): kill the live one of node / follow_node and every node after it in its sibling list, then clear h
bool sfx_defer_handle_reset(SfxHandle* h)
{
    if (!sfx_defer_now())
    {
        return false;
    }
    FXManager* mgr = h->fx_manager;
    if (mgr == NULL)
    {
        return false;
    }
    FxBehaviorNode* first = NULL;
    if (h->node != NULL && sfx_node_alive(h->node))
    {
        first = h->node;
    }
    else if (h->follow_node != NULL && sfx_node_alive(h->follow_node))
    {
        first = h->follow_node;
    }
    if (first == NULL)
    {
        return false;   //the original only clears the handle
    }
    std::vector<std::pair<FxBehaviorNode*, uint64_t>> nodes;
    for (FxBehaviorNode* n = first; n != NULL; n = n->next_node)
    {
        const uint64_t serial = sfx_node_serial(n);
        if (serial == 0)
        {
            return false;
        }
        nodes.push_back({ n, serial });
    }
    release_SfxHandle(h);
    for (const auto& n : nodes)
    {
        FXManager_HideNode(mgr, n.first, sfx_hide_bit);
        sfx_pending.push_back({ SfxStopKind::KillTree, sfx_sim_frame, n.first, n.second, mgr, 0 });
        sfx_stats.stops_deferred++;
    }
    return true;
}

//FUN_140fdc690 (h): the effect stops emitting. The handle is not changed.
bool sfx_defer_handle_soft_stop(SfxHandle* h)
{
    if (!sfx_defer_now())
    {
        return false;
    }
    FxBehaviorNode* a = h->node;
    if (a == NULL || h->fx_manager == NULL || !sfx_node_alive(a) || a->body == NULL)
    {
        return false;
    }
    const uint64_t serial = sfx_node_serial(a);
    if (serial == 0)
    {
        return false;
    }
    sfx_pending.push_back({ SfxStopKind::SoftStop, sfx_sim_frame, a, serial, h->fx_manager, (uint64_t)h->vtable });
    sfx_stats.stops_deferred++;
    return true;
}

// ---- SfxManState ----

SfxManState* init_SfxMan()
{
    SfxManState* local = new SfxManState();
    local->frame = SfxManState_frame_current;
    return local;
}

void free_SfxMan(SfxManState* to)
{
    delete to;
}

void copy_SfxMan(SfxManState* to, const SfxManState* from)
{
    *to = *from;
}

void save_SfxMan(SfxManState* to)
{
    to->frame = SfxManState_frame_current;
}

//Kill the effects the rolled-back simulation spawned in frames >= first_discarded of this session
static void sfx_kill_discarded_spawns(int32_t first_discarded)
{
    std::unordered_set<FxBehaviorNode*> discarded;
    AcquireSRWLockShared(&sfx_nodes_lock);
    for (const auto& e : sfx_nodes)
    {
        const SfxNodeInfo& info = e.second;
        if (info.sim_spawned && info.session == sfx_session && info.spawn_frame >= first_discarded)
        {
            discarded.insert(e.first);
        }
    }
    ReleaseSRWLockShared(&sfx_nodes_lock);

    std::vector<FxBehaviorNode*> tops;
    for (FxBehaviorNode* n : discarded)
    {
        sfx_check_node(n);
        if (sfx_node_alive(n) && discarded.find(n->parent_node) == discarded.end())
        {
            tops.push_back(n);
        }
    }
    for (FxBehaviorNode* n : tops)
    {
        if (!sfx_node_alive(n) || n->parent == NULL || n->parent->base.fxParent == NULL)
        {
            continue;
        }
        FXManager* mgr = n->parent->base.fxParent;
        FXManager_KillDescendants(mgr, n);
        FXManager_KillNode(mgr, n);
        sfx_stats.spawns_killed++;
    }
}

void load_SfxMan(const SfxManState* from)
{
    if (from->frame == SfxManState_frame_current)
    {
        return;   //a capture of the current frame: nothing is discarded
    }
    const int32_t first_discarded = from->frame;
    sfx_sim_frame = first_discarded;

    //Undo any stop requests from the discarded frames, and un-hide those nodes
    std::vector<SfxPendingStop> kept;
    for (const SfxPendingStop& p : sfx_pending)
    {
        //this stop is still valid since it was before the undone frames
        if (p.frame < first_discarded)
        {
            kept.push_back(p);
            continue;
        }
        sfx_stats.stops_cancelled++;
        if (p.kind != SfxStopKind::SoftStop && sfx_node_is(p.node, p.serial))
        {
            FXManager_ShowNode(p.mgr, p.node, sfx_hide_bit);
        }
    }
    sfx_pending.swap(kept);

    //also kill any effects spawned during the discarded frames. Killing them also detaches any handle owners have to them.
    sfx_kill_discarded_spawns(first_discarded);
    sfx_hide_pending_kills();
}

//Every handle in every node's list points back at that node, and the list is doubly linked. Catches a handle that some
//copy wrote raw over a linked one, which would otherwise corrupt memory when the node dies.
static void sfx_check_handle_lists()
{
    AcquireSRWLockShared(&sfx_nodes_lock);
    for (const auto& e : sfx_nodes)
    {
        FxBehaviorNode* node = e.first;
        sfx_check_node(node);
        size_t n = 0;
        SfxHandle* expected_next = NULL;
        for (SfxHandle* h = node->body->linked_sfxHandle; h != NULL; h = h->list_prev)
        {
            const bool points_here = h->node == node || (h->node == NULL && h->follow_node == node);
            if (!points_here || h->list_next != expected_next || ++n > sfx_handle_list_max)
            {
                FATALERROR("SfxMan rollback: FX node %p (sfx %d) lists handle %p (a=%p b=%p list_next=%p, expected list_next %p), which is not linked to it",
                    node, sfx_node_sfx_id(node), h, h->node, h->follow_node, h->list_next, expected_next);
            }
            expected_next = h;
        }
    }
    ReleaseSRWLockShared(&sfx_nodes_lock);
}

//Now that the sfxhandles have been put back by the other load_* functions, validate all our saved nodes are still correct
void load_SfxMan_finish()
{
    sfx_check_handle_lists();
}

// ---- session / frames ----

void sfx_rollback_session_start()
{
    sfx_execute_stops_through(INT32_MAX);
    sfx_session++;
    sfx_session_active = true;
    sfx_stats = {};
    sfx_main_thread = GetCurrentThreadId();
    sfx_sim_frame = -1;
}

void sfx_rollback_session_end()
{
    //nothing can be rolled back any more
    sfx_execute_stops_through(INT32_MAX);
    sfx_session_active = false;
    sfx_sim_frame = -1;
    ConsoleWrite("SfxMan rollback: %llu handles relinked, %llu emptied (effect gone), %llu discarded spawns killed, %llu stops deferred: %llu run, %llu cancelled",
        sfx_stats.handles_relinked, sfx_stats.handles_emptied, sfx_stats.spawns_killed, sfx_stats.stops_deferred, sfx_stats.stops_executed, sfx_stats.stops_cancelled);
}

void sfx_rollback_saved_frame(int32_t frame)
{
    sfx_sim_frame = frame;
}

void sfx_rollback_confirmed_frame(int32_t frame)
{
    sfx_execute_stops_through(frame);
}

// ---- install ----

void sfx_rollback_install()
{
    //node registry: slots 10 and 11 of the FX adapter vtables. A node created through an unhooked vtable would be invisible to
    //the registry, and its handles could then never be unlinked.
    Init_FxBehaviorNode_original = (Init_FxBehaviorNode_FUNC*)(Game::ds1_base + Init_FxBehaviorNode_offset);
    Destruct_FxBehaviorNode_original = (Destruct_FxBehaviorNode_FUNC*)(Game::ds1_base + Destruct_FxBehaviorNode_offset);
    for (uint64_t offset : FxAdapter_vtable_offsets)
    {
        uint64_t* vtable = (uint64_t*)(Game::ds1_base + offset);
        if (vtable[10] != (uint64_t)Init_FxBehaviorNode_original || vtable[11] != (uint64_t)Destruct_FxBehaviorNode_original)
        {
            FATALERROR("sfx_rollback_install: the FX adapter vtable at +%llx is not the one this hook was written for", offset);
        }
        DWORD old;
        VirtualProtect(&vtable[10], 2 * sizeof(uint64_t), PAGE_READWRITE, &old);
        vtable[10] = (uint64_t)&Init_FxBehaviorNode_hook;
        vtable[11] = (uint64_t)&Destruct_FxBehaviorNode_hook;
        VirtualProtect(&vtable[10], 2 * sizeof(uint64_t), old, &old);
    }

    //when the game calls these function that we use as part of a frame resim, we inject code to mark the sfx generated
    //this tells us to pay attention and handle any generated sfx as part of rollback state
    sp::mem::code::x64::inject_jmp_14b((void*)(Game::ds1_base + MoveMapStep_Step_Chr_offset), &SfxSimStep_Chr_return, 1, &SfxSimStep_Chr_injection);
    sp::mem::code::x64::inject_jmp_14b((void*)(Game::ds1_base + MoveMapStep_Step_Bullet_offset), &SfxSimStep_Bullet_return, 1, &SfxSimStep_Bullet_injection);
    sp::mem::code::x64::inject_jmp_14b((void*)(Game::ds1_base + MoveMapStep_Step_DamageMan_offset), &SfxSimStep_DamageMan_return, 1, &SfxSimStep_DamageMan_injection);

    //deferred stops, hook the sfx destruct functions to just make them hidden instead
    sp::mem::code::x64::inject_jmp_14b((void*)(Game::ds1_base + SfxHandle_Kill_offset), &SfxHandleKill_return, 2, &SfxHandleKill_injection);
    sp::mem::code::x64::inject_jmp_14b((void*)(Game::ds1_base + SfxHandle_Reset_offset), &SfxHandleReset_return, 2, &SfxHandleReset_injection);
    sp::mem::code::x64::inject_jmp_14b((void*)(Game::ds1_base + SfxHandle_SoftStop_offset), &SfxHandleSoftStop_return, 2, &SfxHandleSoftStop_injection);
}
