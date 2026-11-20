#include "BulletManStructFunctions.h"
#include "SfxManStructFunctions.h"
#include "Rollback.h"
#include "StateSerializer.h"
#include <cstring>

static const size_t ChrExFollowCam_size = 0x340;
static const size_t ChrAimCam_size = 0x1d0;
static const size_t FallDieCam_size = 0xb0;
static const size_t BallistaAimCam_size = 0xe0;

// ---- SfxHandle lists ----

//Game's layout for SfxHandle list allocations (verified via ghidra on
//ChrMultiSfxSlot_Process_, ChrStatueDeadSlot_Process_, and Destruct_ChrConditionSfxSeSlot):
//    raw = internal_heap_3->malloc(count * sizeof(SfxHandle) + 0x10, 0x10)
//    raw[0x0] = raw                    //self pointer used on free
//    raw[0x8] = (uint64_t)count        //element count
//    list    = (SfxHandle*)(raw + 0x10)
//    free    = Get_HeapAllocator_For_allocation(list)->free( *(void**)(((uint8_t*)list) - 0x10) )
static const size_t SFXHANDLE_LIST_HEADER_SIZE = 0x10;

SfxHandle* alloc_SfxHandle_List(int16_t count, StateTarget target)
{
    if (count <= 0)
    {
        return NULL;
    }
    if (target == StateTarget::ToGame)
    {
        uint8_t* raw = (uint8_t*)Game::game_malloc(
            sizeof(SfxHandle) * (size_t)count + SFXHANDLE_LIST_HEADER_SIZE,
            0x10,
            *(uint64_t*)Game::internal_heap_3);
        *(void**)(raw + 0x0) = raw;
        *(uint64_t*)(raw + 0x8) = (uint64_t)count;
        return (SfxHandle*)(raw + SFXHANDLE_LIST_HEADER_SIZE);
    }
    return (SfxHandle*)malloc_(sizeof(SfxHandle) * (size_t)count);
}

void free_SfxHandle_List(SfxHandle** list, int16_t* len, StateTarget target)
{
    if (*list == NULL)
    {
        *len = 0;
        return;
    }
    if (target == StateTarget::ToGame)
    {
        for (int16_t i = 0; i < *len; i++)
        {
            release_SfxHandle(&(*list)[i]);
        }
        void* raw = *(void**)(((uint8_t*)(*list)) - SFXHANDLE_LIST_HEADER_SIZE);
        Game::game_free_alt(raw);
    }
    else
    {
        free(*list);
    }
    *list = NULL;
    *len = 0;
}

void copy_SfxHandle_List(SfxHandle** to_list, int16_t* to_len, SfxHandle** from_list, int16_t* from_len, StateTarget target)
{
    //must realloc on any size change because the header has a build in count
    if (*to_len != *from_len)
    {
        free_SfxHandle_List(to_list, to_len, target);
        *to_list = alloc_SfxHandle_List(*from_len, target);
    }
    for (int16_t i = 0; i < *from_len; i++)
    {
        copy_SfxHandle(&(*to_list)[i], &(*from_list)[i], target);
    }
    *to_len = *from_len;
}

// ---- BulletIns ----

void copy_BulletIns(BulletIns* to, BulletIns* from, StateTarget target)
{
    to->vtable = from->vtable;
    to->bullet_number = from->bullet_number;
    to->unk_9 = from->unk_9;
    to->unk_a = from->unk_a;
    memcpy(to->unk_c, from->unk_c, sizeof(to->unk_c));
    memcpy(to->player_position, from->player_position, sizeof(to->player_position));
    to->orbit_radius_X = from->orbit_radius_X;
    to->orbit_radius_Y = from->orbit_radius_Y;
    to->orbit_radius_Z = from->orbit_radius_Z;
    to->orbit_radius_W = from->orbit_radius_W;
    to->unk_30 = from->unk_30;
    to->unk_34 = from->unk_34;
    to->unk_38 = from->unk_38;
    to->unk_3c = from->unk_3c;
    to->unk_40 = from->unk_40;
    to->unk_44 = from->unk_44;
    to->unk_48 = from->unk_48;
    to->unk_4c = from->unk_4c;
    to->unk_50 = from->unk_50;
    to->unk_54 = from->unk_54;

    copy_SfxHandle(&to->sfx, &from->sfx, target);

    to->bulletClassType = from->bulletClassType;
    to->unk_8c = from->unk_8c;

    //owner contains entity IDs, attack data, position vectors - all safe to memcpy
    memcpy(&to->owner, &from->owner, sizeof(to->owner));

    to->bullet_distance = from->bullet_distance;
    to->bullet_life = from->bullet_life;
    to->unk_1f8 = from->unk_1f8;
    to->unk_1fc = from->unk_1fc;
    to->unk_200 = from->unk_200;
    to->unk_204 = from->unk_204;

    //bulletTargetingSystemOwner contains parent ptr, vtable, entityId, NpcThinkParam - raw pointers are fine
    memcpy(&to->bulletTargetingSystemOwner, &from->bulletTargetingSystemOwner, sizeof(to->bulletTargetingSystemOwner));

    //targetingSystemBase contains vtable, owner ptr, targeting data - raw pointers are fine
    memcpy(&to->targetingSystemBase, &from->targetingSystemBase, sizeof(to->targetingSystemBase));
    //TODO setting the BulletTargetingSystemOwner to null for now, we may have to save/restore it
    to->targetingSystemBase.owner_1 = NULL;

    to->unk_288 = from->unk_288;
    to->unk_28c = from->unk_28c;
    memcpy(to->target, from->target, sizeof(to->target));
    memcpy(to->unk_2c1, from->unk_2c1, sizeof(to->unk_2c1));
    to->unk_2c4 = from->unk_2c4;
    to->unk_2c8 = from->unk_2c8;
    to->unk_2cc = from->unk_2cc;
    to->unk_2d0 = from->unk_2d0;
    memcpy(to->unk_2d1, from->unk_2d1, sizeof(to->unk_2d1));

    memcpy(&to->bulletState, &from->bulletState, sizeof(to->bulletState));
    to->unk_2f8 = from->unk_2f8;
    to->unk_2fc = from->unk_2fc;
    memcpy(&to->bulletFlyState, &from->bulletFlyState, sizeof(to->bulletFlyState));
    memcpy(&to->bulletExplosionState, &from->bulletExplosionState, sizeof(to->bulletExplosionState));

    //previous_bullet_in_use is handled by copy_BulletMan after all elements are copied
    to->unk_358 = from->unk_358;
    to->unk_35c = from->unk_35c;
}

BulletIns* init_BulletIns()
{
    BulletIns* local = (BulletIns*)malloc_(sizeof(BulletIns));
    return local;
}

void free_BulletIns(BulletIns* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}

// ---- BulletMan_Field0x20 ----

void copy_BulletMan_Field0x20(BulletMan_Field0x20* to, BulletMan_Field0x20* from, StateTarget target)
{
    to->unk_0 = from->unk_0;
    memcpy(to->field0x4Elem, from->field0x4Elem, sizeof(to->field0x4Elem));
    memcpy(to->unk_184, from->unk_184, sizeof(to->unk_184));
    memcpy(to->unk_18c, from->unk_18c, sizeof(to->unk_18c));
    to->unk_194 = from->unk_194;
    to->unk_195 = from->unk_195;
    memcpy(to->unk_196, from->unk_196, sizeof(to->unk_196));

    //BulletParamInfo is game-allocated and never freed. Since this object is linked to the lifetime of BulletMan itself, it's stable
    to->BulletParamInfo = from->BulletParamInfo;

    memcpy(&to->field0x1a0, &from->field0x1a0, sizeof(to->field0x1a0));
    to->unk_300 = from->unk_300;
    to->unk_304 = from->unk_304;

    //next_in_use is a linked list pointer within the field0x20 array - handled by copy_BulletMan
    //chrCam is a raw pointer to the game ChrCam
    to->chrCam = from->chrCam;
    //dbgNode is a raw pointer
    to->dbgNode = from->dbgNode;
}

BulletMan_Field0x20* init_BulletMan_Field0x20()
{
    BulletMan_Field0x20* local = (BulletMan_Field0x20*)malloc_(sizeof(BulletMan_Field0x20));
    return local;
}

void free_BulletMan_Field0x20(BulletMan_Field0x20* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}

// ---- BulletMan_Field0x40 ----

void copy_BulletMan_Field0x40(BulletMan_Field0x40* to, BulletMan_Field0x40* from, StateTarget target)
{
    to->bulletOwnerEntityId = from->bulletOwnerEntityId;
    to->padding_0 = from->padding_0;

    for (size_t i = 0; i < 16; i++)
    {
        copy_SfxHandle(&to->arry[i], &from->arry[i], target);
    }

    to->unk_308 = from->unk_308;
    to->unk_30c = from->unk_30c;
    to->next_in_use = from->next_in_use;
}

BulletMan_Field0x40* init_BulletMan_Field0x40()
{
    BulletMan_Field0x40* local = (BulletMan_Field0x40*)malloc_(sizeof(BulletMan_Field0x40));
    return local;
}

void free_BulletMan_Field0x40(BulletMan_Field0x40* to, bool freeself)
{
    if (freeself)
    {
        free(to);
    }
}

// ---- ChrCam ----

void copy_ChrCam(ChrCam* to, ChrCam* from, StateTarget target)
{
    //Copy the FrpgPersCam base data
    memcpy(to->FrpgPersCam_base, from->FrpgPersCam_base, sizeof(to->FrpgPersCam_base));

    //Deep copy sub-camera data. The sub-cameras are allocated once by the game and persist,
    //so for our local copy we have our own allocations. Copy the data at the pointers.
    if (from->ChrExFollowCam != NULL && to->ChrExFollowCam != NULL)
    {
        memcpy(to->ChrExFollowCam, from->ChrExFollowCam, ChrExFollowCam_size);
    }
    if (from->ChrAimCam != NULL && to->ChrAimCam != NULL)
    {
        memcpy(to->ChrAimCam, from->ChrAimCam, ChrAimCam_size);
    }
    if (from->FallDieCam != NULL && to->FallDieCam != NULL)
    {
        memcpy(to->FallDieCam, from->FallDieCam, FallDieCam_size);
    }
    if (from->BallistaAimCam != NULL && to->BallistaAimCam != NULL)
    {
        memcpy(to->BallistaAimCam, from->BallistaAimCam, BallistaAimCam_size);
    }

    //Copy remaining ChrCam state (flags, floats, etc.)
    to->unk_80 = from->unk_80;
    memcpy(to->unk_81, from->unk_81, sizeof(to->unk_81));
    to->unk_84 = from->unk_84;
    to->unk_88 = from->unk_88;
    to->unk_8c = from->unk_8c;
    memcpy(to->unk_90, from->unk_90, sizeof(to->unk_90));
    to->unk_9c = from->unk_9c;
    to->unk_a0 = from->unk_a0;
    to->unk_a1 = from->unk_a1;
    to->unk_a2 = from->unk_a2;
    to->UseBallistaAimCam = from->UseBallistaAimCam;
    memcpy(to->unk_a4, from->unk_a4, sizeof(to->unk_a4));
    to->unk_c0 = from->unk_c0;
    memcpy(to->unk_c1, from->unk_c1, sizeof(to->unk_c1));
    to->DbgNode = from->DbgNode;
    memcpy(to->unk_c9, from->unk_c9, sizeof(to->unk_c9));
}

ChrCam* init_ChrCam()
{
    ChrCam* local = (ChrCam*)malloc_(sizeof(ChrCam));

    local->ChrExFollowCam = malloc_(ChrExFollowCam_size);
    local->ChrAimCam = malloc_(ChrAimCam_size);
    local->FallDieCam = malloc_(FallDieCam_size);
    local->BallistaAimCam = malloc_(BallistaAimCam_size);

    return local;
}

void free_ChrCam(ChrCam* to)
{
    if (to->ChrExFollowCam != NULL)
    {
        free(to->ChrExFollowCam);
    }
    if (to->ChrAimCam != NULL)
    {
        free(to->ChrAimCam);
    }
    if (to->FallDieCam != NULL)
    {
        free(to->FallDieCam);
    }
    if (to->BallistaAimCam != NULL)
    {
        free(to->BallistaAimCam);
    }
    free(to);
}

// ---- serializer (mirrors copy_BulletMan and friends) ------------------------
//
// Pointer handling:
//  - intra-array links that copy fixes up as offsets (previous_bullet_in_use,
//    next_in_use) -> ptr_index.
//  - FX handles (BulletIns::sfx, field0x40.arry) -> serialize_SfxHandle: only the
//    vtable, which effect they point at is the sfx column's.
//  - cursor pointers (end_of_bullets_in_use, etc.) hold GAME addresses in the
//    snapshot -> ptr_flag.
//  - sub-structs that copy memcpy's wholesale but which embed a pointer
//    (owner.unk_148, BulletState.param_info's BulletParam*, targetingSystemBase
//    .owner_1, bulletTargetingSystemOwner.parent) are serialized field-by-field
//    with ptr_flag for the pointer, so a heap address can't false-desync Tier-0.
//  - large opaque blobs (ChrCam sub-cameras, field0x4Elem, AttackData, etc.) are
//    hashed raw to mirror copy; if the bullet column shows persistent divergence,
//    the ChrCam sub-camera blobs are the first thing to exclude.

// NS_FRPG_BulletIns_Field0x90_Field0x90 (FUN_140423680): entity handle, pad, u64, two Vector4s, a flag byte
static void serialize_BulletTarget(StateVisitor& v, const char* n, const uint8_t* t)
{
    v.begin(n);
    uint32_t handle;
    memcpy(&handle, t, sizeof(handle));
    serialize_handle(v, "entity_id", handle);
    v.padding("unk_4", t + 4, 4);
    v.blob("rest", t + 8, 49 - 8);
    v.end();
}

static void serialize_BulletIns_Field0x90_Field0x1a0(StateVisitor& v, const BulletIns_Field0x90_Field0x1a0* o)
{
    v.begin("Field0x90_Field0x1a0");
    v.field("unk_0", o->unk_0);
    v.field("bullet_id", o->bullet_id);
    v.field("unk_8", o->unk_8);
    serialize_handle(v, "owner_entityId", o->owner_entityId);
    v.field("unk_10", o->unk_10);
    v.field("unk_14", o->unk_14);
    v.field("goodsId", o->goodsId);
    v.padding("unk_1c", &o->unk_1c, sizeof(o->unk_1c));            // alignment before AttackData @0x20
    v.blob("AttackData", o->AttackData, sizeof(o->AttackData));
    v.blob("unk_88", o->unk_88, sizeof(o->unk_88));
    serialize_BulletTarget(v, "target0x90", o->target0x90);
    // 0xc1-0xcf: the rest of the 0x40-byte target struct at 0x90 (FUN_140423680 initialises it up to 0x30)
    v.padding("unk_c1", o->unk_c1, sizeof(o->unk_c1));
    v.padding("unk_c8", &o->unk_c8, sizeof(o->unk_c8));
    v.field("unk_d0", o->unk_d0);
    v.field("bowDist", o->bowDist);
    v.padding("unk_d8", o->unk_d8, sizeof(o->unk_d8));             // alignment before the 16-aligned 0xe0
    v.blob("fieldE0_0", o->fieldE0_0, sizeof(o->fieldE0_0));
    v.blob("fieldE0_1", o->fieldE0_1, sizeof(o->fieldE0_1));
    v.field("unk_140", o->unk_140);
    v.padding("unk_144", o->unk_144, sizeof(o->unk_144));         // alignment before the pointer
    v.ptr_flag("unk_148", o->unk_148);                             // DmgHitRecordManImp_field0x10Elem* (refcounted hit record)
    v.field("unk_150", o->unk_150);
    v.field("unk_154", o->unk_154);
    v.field("unk_158", o->unk_158);
    v.field("unk_15c", o->unk_15c);
    v.end();
}

static void serialize_BulletTargetingSystemOwner(StateVisitor& v, const BulletTargetingSystemOwner* o)
{
    v.begin("BulletTargetingSystemOwner");
    v.field("vtable", o->vtable);
    v.ptr_flag("parent", o->parent);
    serialize_handle(v, "ownerEntityId", o->ownerEntityId);
    v.padding("unk_14", &o->unk_14, sizeof(o->unk_14));            // alignment before the 8-aligned param @0x18
    // NpcThinkParamInfo: rowId, pad, NpcThinkParam*, currentBattleGoalId, currentLogicId
    v.blob_holes("autoSearchNPCThinkParam", o->autoSearchNPCThinkParam, sizeof(o->autoSearchNPCThinkParam),
        { { 4, 4, StateVisitor::Hole::Pad }, { 8, 8, StateVisitor::Hole::Ptr } });
    v.end();
}

static void serialize_TargetingSystemBase(StateVisitor& v, const TargetingSystemBase* t)
{
    v.begin("TargetingSystemBase");
    v.field("vtable", t->vtable);
    v.ptr_flag("owner_1", t->owner_1);   // forced NULL by copy_BulletIns
    // TargetingSystemBase_sub0x10 (FUN_14061fba0): owner_2 (the slot's own BulletTargetingSystemOwner), target struct pointer, 2x u64
    v.blob_holes("sub0x10", t->sub0x10, sizeof(t->sub0x10),
        { { 0, 8, StateVisitor::Hole::Ptr }, { 8, 8, StateVisitor::Hole::Ptr } });
    v.field("unk_30", t->unk_30);
    v.field("unk_34", t->unk_34);
    v.field("unk_35", t->unk_35);
    v.padding("unk_36", t->unk_36, sizeof(t->unk_36));             // alignment before unk_38
    v.field("unk_38", t->unk_38);
    v.padding("unk_3c", &t->unk_3c, sizeof(t->unk_3c));            // alignment before unk_40 (8-aligned)
    v.field("unk_40", t->unk_40);
    v.field("unk_48", t->unk_48);
    v.padding("unk_4a", t->unk_4a, sizeof(t->unk_4a));             // trailing alignment
    v.end();
}

static void serialize_BulletState(StateVisitor& v, const BulletState* s)
{
    v.begin("BulletState");
    v.field("vtable", s->vtable);
    // param_info = bullet_id(u32) + pad(u32) + BulletParam*(ptr)
    v.blob_holes("param_info", s->param_info, sizeof(s->param_info),
        { { 4, 4, StateVisitor::Hole::Pad }, { 8, 8, StateVisitor::Hole::Ptr } });
    v.field("frametime", s->frametime);
    v.field("unk_1c", s->unk_1c);
    v.end();
}

static void serialize_BulletFlyState(StateVisitor& v, const BulletFlyState* s)
{
    v.begin("BulletFlyState");
    serialize_BulletState(v, &s->base);
    v.field("unk_20", s->unk_20);
    v.padding("unk_22", s->unk_22, sizeof(s->unk_22));             // trailing alignment
    v.end();
}

static void serialize_BulletIns(StateVisitor& v, const BulletIns* b)
{
    v.begin("BulletIns");
    v.field("vtable", b->vtable);
    v.field("bullet_number", b->bullet_number);
    v.field("unk_9", b->unk_9);
    v.field("unk_a", b->unk_a);
    v.padding("unk_c", b->unk_c, sizeof(b->unk_c));                // alignment before the 16-aligned position
    for (int i = 0; i < 4; i++) v.field("player_position", b->player_position[i]);
    v.field("orbit_radius_X", b->orbit_radius_X);
    v.field("orbit_radius_Y", b->orbit_radius_Y);
    v.field("orbit_radius_Z", b->orbit_radius_Z);
    v.field("orbit_radius_W", b->orbit_radius_W);
    v.field("unk_30", b->unk_30);
    v.field("unk_34", b->unk_34);
    v.field("unk_38", b->unk_38);
    v.field("unk_3c", b->unk_3c);
    v.field("unk_40", b->unk_40);
    v.field("unk_44", b->unk_44);
    v.field("unk_48", b->unk_48);
    v.field("unk_4c", b->unk_4c);
    v.field("unk_50", b->unk_50);
    serialize_DamageEntry_ref(v, "unk_54", b->unk_54);             // the bullet's DamageEntry id (-1 if none; Init sets -1)

    serialize_SfxHandle(v, "sfx", &b->sfx);

    v.field("bulletClassType", b->bulletClassType);
    v.field("unk_8c", b->unk_8c);
    serialize_BulletIns_Field0x90_Field0x1a0(v, &b->owner);
    v.field("bullet_distance", b->bullet_distance);
    v.field("bullet_life", b->bullet_life);
    v.field("unk_1f8", b->unk_1f8);
    v.field("unk_1fc", b->unk_1fc);
    v.field("unk_200", b->unk_200);
    v.field("unk_204", b->unk_204);
    serialize_BulletTargetingSystemOwner(v, &b->bulletTargetingSystemOwner);
    serialize_TargetingSystemBase(v, &b->targetingSystemBase);
    v.field("unk_288", b->unk_288);
    v.field("unk_28c", b->unk_28c);
    serialize_BulletTarget(v, "target", b->target);
    // 0x2c1-0x2cf: the rest of the 0x40-byte target struct at 0x290 (FUN_140423680 initialises it up to 0x30)
    v.padding("unk_2c1", b->unk_2c1, sizeof(b->unk_2c1));
    v.padding("unk_2c4", &b->unk_2c4, sizeof(b->unk_2c4));
    v.padding("unk_2c8", &b->unk_2c8, sizeof(b->unk_2c8));
    v.padding("unk_2cc", &b->unk_2cc, sizeof(b->unk_2cc));
    v.field("unk_2d0", b->unk_2d0);
    v.padding("unk_2d1", b->unk_2d1, sizeof(b->unk_2d1));          // alignment before bulletState
    serialize_BulletState(v, &b->bulletState);
    v.field("unk_2f8", b->unk_2f8);                                // BulletWaitState's float at +0x20 (Init_BulletWaitState)
    v.padding("unk_2fc", &b->unk_2fc, sizeof(b->unk_2fc));         // the wait state is 0x24 bytes; alignment before bulletFlyState
    serialize_BulletFlyState(v, &b->bulletFlyState);
    serialize_BulletState(v, &b->bulletExplosionState);
    // previous_bullet_in_use handled by parent (ptr_index); padding_1 not copied
    v.field("unk_358", b->unk_358);
    v.field("unk_35c", b->unk_35c);
    v.end();
}

static void serialize_BulletMan_Field0x20(StateVisitor& v, const BulletMan_Field0x20* f)
{
    v.begin("BulletMan_Field0x20");
    v.field("unk_0", f->unk_0);
    v.blob("field0x4Elem", f->field0x4Elem, sizeof(f->field0x4Elem));
    v.blob("unk_184", f->unk_184, sizeof(f->unk_184));
    v.blob("unk_18c", f->unk_18c, sizeof(f->unk_18c));
    v.field("unk_194", f->unk_194);
    v.field("unk_195", f->unk_195);
    v.padding("unk_196", f->unk_196, sizeof(f->unk_196));          // alignment before BulletParamInfo ptr
    v.ptr_flag("BulletParamInfo", f->BulletParamInfo);
    serialize_BulletIns_Field0x90_Field0x1a0(v, &f->field0x1a0);
    v.field("unk_300", f->unk_300);
    v.field("unk_304", f->unk_304);
    // next_in_use handled by parent (ptr_index)
    v.ptr_flag("chrCam", f->chrCam);
    v.ptr_flag("dbgNode", f->dbgNode);
    v.end();
}

static void serialize_BulletMan_Field0x40(StateVisitor& v, const BulletMan_Field0x40* f)
{
    v.begin("BulletMan_Field0x40");
    serialize_handle(v, "bulletOwnerEntityId", *(const uint32_t*)&f->bulletOwnerEntityId);
    v.padding("padding_0", &f->padding_0, sizeof(f->padding_0));
    v.count("arry", 16);
    for (size_t i = 0; i < 16; i++)
    {
        serialize_SfxHandle(v, "sfx", &f->arry[i]);
    }
    v.field("unk_308", f->unk_308);
    v.padding("unk_30c", &f->unk_30c, sizeof(f->unk_30c)); // gap
    v.ptr_flag("next_in_use", f->next_in_use);
    v.end();
}

static void serialize_ChrCam(StateVisitor& v, const ChrCam* c)
{
    v.begin("ChrCam");
    v.blob("FrpgPersCam_base", c->FrpgPersCam_base, sizeof(c->FrpgPersCam_base));
    // sub-camera data is deep-copied by copy_ChrCam; opaque blobs (may embed ptrs)
    if (c->ChrExFollowCam) v.blob("ChrExFollowCam", c->ChrExFollowCam, ChrExFollowCam_size);
    if (c->ChrAimCam)      v.blob("ChrAimCam", c->ChrAimCam, ChrAimCam_size);
    if (c->FallDieCam)     v.blob("FallDieCam", c->FallDieCam, FallDieCam_size);
    if (c->BallistaAimCam) v.blob("BallistaAimCam", c->BallistaAimCam, BallistaAimCam_size);
    v.field("unk_80", c->unk_80);
    v.blob("unk_81", c->unk_81, sizeof(c->unk_81));
    v.field("unk_84", c->unk_84);
    v.field("unk_88", c->unk_88);
    v.field("unk_8c", c->unk_8c);
    v.blob("unk_90", c->unk_90, sizeof(c->unk_90));
    v.field("unk_9c", c->unk_9c);
    v.field("unk_a0", c->unk_a0);
    v.field("unk_a1", c->unk_a1);
    v.field("unk_a2", c->unk_a2);
    v.field("UseBallistaAimCam", c->UseBallistaAimCam);
    v.blob("unk_a4", c->unk_a4, sizeof(c->unk_a4));
    v.field("unk_c0", c->unk_c0);
    v.blob("unk_c1", c->unk_c1, sizeof(c->unk_c1));
    v.field("DbgNode", c->DbgNode);
    v.blob("unk_c9", c->unk_c9, sizeof(c->unk_c9));
    v.end();
}

void serialize_BulletMan(StateVisitor& v, BulletMan* m)
{
    v.begin("BulletMan");

    // only the pool entries actually IN USE are hashed.
    // ignore free slots since they will have garbage data
    size_t n_bullets = 0;
    for (const BulletIns* b = m->end_of_bullets_in_use; b != NULL; b = b->previous_bullet_in_use)
    {
        if (++n_bullets >= 128) break;
    }
    v.count("bullets_in_use", n_bullets);
    {
        size_t i = 0;
        for (const BulletIns* b = m->end_of_bullets_in_use; b != NULL && i < n_bullets;
             b = b->previous_bullet_in_use, i++)
        {
            v.ptr_index("slot", b, m->bulletins_arry, sizeof(BulletIns));
            serialize_BulletIns(v, b);
        }
    }
    v.ptr_flag("end_of_bullets_in_use", m->end_of_bullets_in_use);
    v.ptr_flag("start_of_unused_bullets", m->start_of_unused_bullets);
    v.field("bullet_count_1", m->bullet_count_1);
    v.field("bullet_count_2", m->bullet_count_2);

    size_t n_f20 = 0;
    for (const BulletMan_Field0x20* f = m->end_of_bullets_in_use_field0x20; f != NULL; f = f->next_in_use)
    {
        if (++n_f20 >= 64) break;
    }
    v.count("field0x20_in_use", n_f20);
    {
        size_t i = 0;
        for (const BulletMan_Field0x20* f = m->end_of_bullets_in_use_field0x20; f != NULL && i < n_f20;
             f = f->next_in_use, i++)
        {
            v.ptr_index("slot", f, m->field0x20, sizeof(BulletMan_Field0x20));
            serialize_BulletMan_Field0x20(v, f);
        }
    }
    v.ptr_flag("end_of_bullets_in_use_field0x20", m->end_of_bullets_in_use_field0x20);
    v.ptr_flag("start_of_unused_bullets_field0x20", m->start_of_unused_bullets_field0x20);
    v.field("unk_38", m->unk_38);
    v.field("unk_3c", m->unk_3c);

    v.count("field0x40", 4);
    for (size_t i = 0; i < 4; i++)
    {
        serialize_BulletMan_Field0x40(v, &m->field0x40[i]);
    }
    v.ptr_flag("end_of_bullets_in_use_field0x40", m->end_of_bullets_in_use_field0x40);
    v.ptr_flag("start_of_unused_bullets_field0x40", m->start_of_unused_bullets_field0x40);
    v.field("unk_58", m->unk_58);
    v.field("unk_5c", m->unk_5c);

    // ChrCam is NOT hashed. it is saved/restored, but the two instances are two different players
    // looking in different directions
    v.ptr_flag("chrCam", m->chrCam);

    v.field("attach_system_disabled", m->attach_system_disabled);
    v.padding("unk_69", m->unk_69, sizeof(m->unk_69));             // gap
    v.field("bullet_direction_drawing", m->bullet_direction_drawing);
    v.padding("unk_a9", m->unk_a9, sizeof(m->unk_a9));             // gap
    v.field("speed_attenuation_rate", m->speed_attenuation_rate);
    v.field("last_addition_rate", m->last_addition_rate);
    v.field("external_force", m->external_force);

    v.end();
}

std::string print_BulletMan(BulletMan* m)
{
    StateVisitor v(StateVisitor::Mode::Print);
    serialize_BulletMan(v, m);
    return v.text();
}

uint64_t hash_BulletMan(BulletMan* m)
{
    StateVisitor v(StateVisitor::Mode::Hash);
    serialize_BulletMan(v, m);
    return v.digest();
}

// ---- session start: free pool slots ----
//BulletMan_AddBullet (0x14042a880) hands out a free slot as it is, and whoever spawns the bullet writes only some of its fields, so
//the rest keeps whatever the slot's earlier bullets (or the uninitialised heap behind Init_BulletMan) left there. That history differs
//between the machines, so a session start rebuilds every free slot with the game's own constructors, giving the same bytes on every
//machine from then on. Kept: the slot's id bytes, its effect handle (SfxMan's lists may still name it), the free list link, the debug
//menu item, and the two references the slot does not own (refcounted hit record, targeting target).
typedef BulletIns* Init_BulletIns_FUNC(BulletIns* bullet, uint64_t array_index, uint32_t param_3);
static Init_BulletIns_FUNC* const Game_Init_BulletIns = (Init_BulletIns_FUNC*)0x140423f10;
typedef BulletIns_Field0x90_Field0x1a0* Init_BulletOwner_FUNC(BulletIns_Field0x90_Field0x1a0* owner);
static Init_BulletOwner_FUNC* const Game_Init_BulletOwner = (Init_BulletOwner_FUNC*)0x140423070;

template <typename T>
static void mark_BulletMan_pool_in_use(const T* head, T* T::* link, const T* pool, size_t count, bool* in_use, const char* what)
{
    size_t n = 0;
    for (const T* e = head; e != NULL; e = e->*link)
    {
        if (e < pool || e >= pool + count)
        {
            FATALERROR("canonicalize_BulletMan_free_slots: %s %p is not in its %zu-element pool", what, e, count);
        }
        if (++n > count)
        {
            FATALERROR("canonicalize_BulletMan_free_slots: %s list is longer than its %zu-element pool", what, count);
        }
        in_use[e - pool] = true;
    }
}

void canonicalize_BulletMan_free_slots(BulletMan* m)
{
    if (m == NULL || m->bulletins_arry == NULL || m->field0x20 == NULL || m->field0x40 == NULL)
    {
        FATALERROR("canonicalize_BulletMan_free_slots: BulletMan %p is not set up", m);
    }
    size_t rebuilt[3] = {};

    bool bullet_in_use[128] = {};
    mark_BulletMan_pool_in_use(m->end_of_bullets_in_use, &BulletIns::previous_bullet_in_use, m->bulletins_arry, 128, bullet_in_use, "BulletIns in use");
    for (size_t i = 0; i < 128; i++)
    {
        if (bullet_in_use[i])
        {
            continue;
        }
        BulletIns* b = &m->bulletins_arry[i];
        BulletIns saved;
        memcpy(&saved, b, sizeof(saved));
        memset(b, 0, sizeof(*b));
        Game_Init_BulletIns(b, i, 0);
        b->bullet_number = saved.bullet_number;
        b->unk_9 = saved.unk_9;
        b->unk_a = saved.unk_a;
        memcpy(&b->sfx, &saved.sfx, sizeof(b->sfx));
        b->previous_bullet_in_use = saved.previous_bullet_in_use;
        b->padding_1 = saved.padding_1;
        b->owner.unk_148 = saved.owner.unk_148;
        memcpy(b->targetingSystemBase.sub0x10 + 8, saved.targetingSystemBase.sub0x10 + 8, 8);
        rebuilt[0]++;
    }

    bool f20_in_use[64] = {};
    mark_BulletMan_pool_in_use(m->end_of_bullets_in_use_field0x20, &BulletMan_Field0x20::next_in_use, m->field0x20, 64, f20_in_use, "Field0x20 in use");
    for (size_t i = 0; i < 64; i++)
    {
        if (f20_in_use[i])
        {
            continue;
        }
        //Init_BulletMan_Field0x20 allocates its BulletParamInfo, so only the parts it builds without allocating are rebuilt; the
        //release (FUN_140420e00) already resets the rest
        BulletMan_Field0x20* f = &m->field0x20[i];
        void* hit_record = f->field0x1a0.unk_148;
        memset(&f->field0x1a0, 0, sizeof(f->field0x1a0));
        Game_Init_BulletOwner(&f->field0x1a0);
        f->field0x1a0.unk_148 = hit_record;
        memset(f->unk_196, 0, sizeof(f->unk_196));
        f->unk_300 = 0;
        f->unk_304 = 0;
        rebuilt[1]++;
    }

    bool f40_in_use[4] = {};
    mark_BulletMan_pool_in_use(m->end_of_bullets_in_use_field0x40, &BulletMan_Field0x40::next_in_use, m->field0x40, 4, f40_in_use, "Field0x40 in use");
    for (size_t i = 0; i < 4; i++)
    {
        if (f40_in_use[i])
        {
            continue;
        }
        //Init_BulletMan_field0x40, without its effect handles
        BulletMan_Field0x40* f = &m->field0x40[i];
        f->bulletOwnerEntityId = -1;
        f->padding_0 = 0;
        f->unk_308 = 0;
        f->unk_30c = 0;
        rebuilt[2]++;
    }

    ConsoleWrite("BulletMan: rebuilt %zu free bullet slots, %zu free Field0x20 and %zu free Field0x40 entries for the session start",
        rebuilt[0], rebuilt[1], rebuilt[2]);
}

// ---- BulletMan ----

//Init_BulletMan allocates each pool (128 BulletIns, 64 Field0x20, 4 Field0x40) as one array whose elements start on its free list.
//BulletMan_AddBullet heap-allocates a BulletIns when the free list is empty. copy_BulletMan copies the arrays only, so a list that
//reaches outside its array stops the save rather than losing the entry.
template <typename T>
static void check_BulletMan_pool_list(const T* head, T* T::* link, const T* pool, size_t count, const char* what)
{
    size_t n = 0;
    for (const T* e = head; e != NULL; e = e->*link)
    {
        if (e < pool || e >= pool + count)
        {
            FATALERROR("copy_BulletMan: %s %p is not in its %zu-element pool; heap-allocated bullets cannot be rolled back", what, e, count);
        }
        if (++n > count)
        {
            FATALERROR("copy_BulletMan: %s list is longer than its %zu-element pool", what, count);
        }
    }
}

//A list link as the same element of the other copy's pool, so a local copy's lists stay inside its own arrays
template <typename T>
static T* copy_BulletMan_link(T* ptr, const T* from_pool, T* to_pool, size_t count, const char* what)
{
    if (ptr == NULL)
    {
        return NULL;
    }
    const uint64_t offset = (uint64_t)ptr - (uint64_t)from_pool;
    if (offset >= count * sizeof(T) || offset % sizeof(T) != 0)
    {
        FATALERROR("copy_BulletMan: %s %p is not an element of its %zu-element pool", what, ptr, count);
    }
    return (T*)((uint64_t)to_pool + offset);
}

void copy_BulletMan(BulletMan* to, BulletMan* from, StateTarget target)
{
    Game::SuspendThreads();

    if (target == StateTarget::ToLocal)
    {
        check_BulletMan_pool_list(from->end_of_bullets_in_use, &BulletIns::previous_bullet_in_use, from->bulletins_arry, 128, "BulletIns in use");
        check_BulletMan_pool_list(from->start_of_unused_bullets, &BulletIns::previous_bullet_in_use, from->bulletins_arry, 128, "unused BulletIns");
        check_BulletMan_pool_list(from->end_of_bullets_in_use_field0x20, &BulletMan_Field0x20::next_in_use, from->field0x20, 64, "Field0x20 in use");
        check_BulletMan_pool_list(from->start_of_unused_bullets_field0x20, &BulletMan_Field0x20::next_in_use, from->field0x20, 64, "unused Field0x20");
        check_BulletMan_pool_list(from->end_of_bullets_in_use_field0x40, &BulletMan_Field0x40::next_in_use, from->field0x40, 4, "Field0x40 in use");
        check_BulletMan_pool_list(from->start_of_unused_bullets_field0x40, &BulletMan_Field0x40::next_in_use, from->field0x40, 4, "unused Field0x40");
    }

    for (size_t i = 0; i < 128; i++)
    {
        copy_BulletIns(&to->bulletins_arry[i], &from->bulletins_arry[i], target);
    }
    //Fix up previous_bullet_in_use linked list pointers within the bulletins_arry.
    for (size_t i = 0; i < 128; i++)
    {
        if (from->bulletins_arry[i].previous_bullet_in_use != NULL)
        {
            uint64_t from_offset = (uint64_t)from->bulletins_arry[i].previous_bullet_in_use - (uint64_t)from->bulletins_arry;
            to->bulletins_arry[i].previous_bullet_in_use = (BulletIns*)((uint64_t)to->bulletins_arry + from_offset);
        }
        else
        {
            to->bulletins_arry[i].previous_bullet_in_use = NULL;
        }
    }

    to->end_of_bullets_in_use = copy_BulletMan_link(from->end_of_bullets_in_use, from->bulletins_arry, to->bulletins_arry, 128, "BulletIns in use");
    to->start_of_unused_bullets = copy_BulletMan_link(from->start_of_unused_bullets, from->bulletins_arry, to->bulletins_arry, 128, "unused BulletIns");
    to->bullet_count_1 = from->bullet_count_1;
    to->bullet_count_2 = from->bullet_count_2;

    for (size_t i = 0; i < 64; i++)
    {
        copy_BulletMan_Field0x20(&to->field0x20[i], &from->field0x20[i], target);
    }
    //Fix up next_in_use linked list pointers within the field0x20 array
    for (size_t i = 0; i < 64; i++)
    {
        if (from->field0x20[i].next_in_use != NULL)
        {
            uint64_t from_offset = (uint64_t)from->field0x20[i].next_in_use - (uint64_t)from->field0x20;
            to->field0x20[i].next_in_use = (BulletMan_Field0x20*)((uint64_t)to->field0x20 + from_offset);
        }
        else
        {
            to->field0x20[i].next_in_use = NULL;
        }
    }

    to->end_of_bullets_in_use_field0x20 = copy_BulletMan_link(from->end_of_bullets_in_use_field0x20, from->field0x20, to->field0x20, 64, "Field0x20 in use");
    to->start_of_unused_bullets_field0x20 = copy_BulletMan_link(from->start_of_unused_bullets_field0x20, from->field0x20, to->field0x20, 64, "unused Field0x20");
    to->unk_38 = from->unk_38;
    to->unk_3c = from->unk_3c;

    for (size_t i = 0; i < 4; i++)
    {
        copy_BulletMan_Field0x40(&to->field0x40[i], &from->field0x40[i], target);
    }

    for (size_t i = 0; i < 4; i++)
    {
        to->field0x40[i].next_in_use = copy_BulletMan_link(from->field0x40[i].next_in_use, from->field0x40, to->field0x40, 4, "Field0x40 next");
    }
    to->end_of_bullets_in_use_field0x40 = copy_BulletMan_link(from->end_of_bullets_in_use_field0x40, from->field0x40, to->field0x40, 4, "Field0x40 in use");
    to->start_of_unused_bullets_field0x40 = copy_BulletMan_link(from->start_of_unused_bullets_field0x40, from->field0x40, to->field0x40, 4, "unused Field0x40");
    to->unk_58 = from->unk_58;
    to->unk_5c = from->unk_5c;

    //Clear the sfx handles for any bullets that are no longer in use, otherwise we save sfx that don't exist anymore
    // (since the game doesn't clear them, finished bullets just have their memory left sitting around)
    if (target == StateTarget::ToLocal)
    {
        bool in_use[128] = {};
        for (BulletIns* b = from->end_of_bullets_in_use; b != NULL; b = b->previous_bullet_in_use)
        {
            in_use[b - from->bulletins_arry] = true;
        }
        for (size_t i = 0; i < 128; i++)
        {
            if (!in_use[i])
            {
                clear_SfxHandle(&to->bulletins_arry[i].sfx);
            }
        }
        bool field0x40_in_use[4] = {};
        for (BulletMan_Field0x40* f = from->end_of_bullets_in_use_field0x40; f != NULL; f = f->next_in_use)
        {
            field0x40_in_use[f - from->field0x40] = true;
        }
        for (size_t i = 0; i < 4; i++)
        {
            if (!field0x40_in_use[i])
            {
                for (size_t j = 0; j < 16; j++)
                {
                    clear_SfxHandle(&to->field0x40[i].arry[j]);
                }
            }
        }
    }

    copy_ChrCam(to->chrCam, from->chrCam, target);

    to->attach_system_disabled = from->attach_system_disabled;
    memcpy(to->unk_69, from->unk_69, sizeof(to->unk_69));

    to->bullet_direction_drawing = from->bullet_direction_drawing;
    memcpy(to->unk_a9, from->unk_a9, sizeof(to->unk_a9));
    to->speed_attenuation_rate = from->speed_attenuation_rate;
    to->last_addition_rate = from->last_addition_rate;
    to->external_force = from->external_force;

    Game::ResumeThreads();
}

BulletMan* init_BulletMan()
{
    BulletMan* local_BulletMan = (BulletMan*)malloc_(sizeof(BulletMan));

    local_BulletMan->bulletins_arry = (BulletIns*)malloc_(sizeof(BulletIns) * 128);
    for (size_t i = 0; i < 128; i++)
    {
        BulletIns* local_bulletins = init_BulletIns();
        local_BulletMan->bulletins_arry[i] = *local_bulletins;
        free(local_bulletins);
    }

    local_BulletMan->field0x20 = (BulletMan_Field0x20*)malloc_(sizeof(BulletMan_Field0x20) * 64);
    for (size_t i = 0; i < 64; i++)
    {
        BulletMan_Field0x20* local_BulletMan_Field0x20 = init_BulletMan_Field0x20();
        local_BulletMan->field0x20[i] = *local_BulletMan_Field0x20;
        free(local_BulletMan_Field0x20);
    }

    local_BulletMan->field0x40 = (BulletMan_Field0x40*)malloc_(sizeof(BulletMan_Field0x40) * 4);
    for (size_t i = 0; i < 4; i++)
    {
        BulletMan_Field0x40* local_BulletMan_Field0x40 = init_BulletMan_Field0x40();
        local_BulletMan->field0x40[i] = *local_BulletMan_Field0x40;
        free(local_BulletMan_Field0x40);
    }

    local_BulletMan->chrCam = init_ChrCam();

    return local_BulletMan;
}

void free_BulletMan(BulletMan* to)
{
    for (size_t i = 0; i < 128; i++)
    {
        free_BulletIns(&to->bulletins_arry[i], false);
    }
    free(to->bulletins_arry);
    for (size_t i = 0; i < 64; i++)
    {
        free_BulletMan_Field0x20(&to->field0x20[i], false);
    }
    free(to->field0x20);
    for (size_t i = 0; i < 4; i++)
    {
        free_BulletMan_Field0x40(&to->field0x40[i], false);
    }
    free(to->field0x40);
    free_ChrCam(to->chrCam);

    free(to);
}
