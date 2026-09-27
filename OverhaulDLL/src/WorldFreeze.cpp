#include "WorldFreeze.h"
#include "DarkSoulsOverhaulMod.h"
#include "GameData.h"
#include "Rollback.h"
#include "PlayerInsStruct.h"
#include "FrpgHavokManImpStruct.h"
#include "DamageManStructFunctions.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

extern "C" uint8_t rollback_world_frozen = 0;
static uint32_t blocked_damage = 0;

namespace
{
    // ---- game layout (DarkSoulsRemastered 1.3.1.0, Ghidra) ----
    // WorldChrManImp: +0x28 uint number of WorldBlockChr, +0x30 WorldBlockChr* (0xe8 each). A WorldBlockChr's +0x48 is a WorldBlock
    // {uint32 number_of_chrs; PlayerNodeDetails* list at +0x8}, 0x38 per character, the ChrIns* first (get_Chr_From_WorldBlock).
    // The players are not in the world blocks but in WorldChrManImp's own host/network player lists.
    const uint64_t WorldObjManImp_global = 0x141c75dd8;
    // WorldObjManImp: +0x28 int number of WorldBlockObj, +0x30 WorldBlockObj* (0x160 each); a block's +0x48 int and +0x50 ObjIns array
    // (0x120 each) (FUN_14030c290)
    // ObjIns+0x45 bit 1: breaking disabled (what SetObjDisableBreak, FUN_140490810, sets)
    const uint8_t ObjIns_disable_break = 0x2;
    // ChrIns SetDisable (FUN_140325b20, called by LUA_SetDisable); the state lives in bit 0 of the character's slot +0x20
    typedef void ChrIns_SetDisable_FUNC(void* chr, uint8_t disable);
    ChrIns_SetDisable_FUNC* ChrIns_SetDisable = (ChrIns_SetDisable_FUNC*)0x140325b20;
    const uint32_t hkpMotion_FIXED = 5;
    // hkpEntityActivation: 0 leaves the body's activation alone, 1 activates it (what FrpgPhysIns passes when restoring a type)
    const uint32_t hkpEntityActivation_DO_NOT_ACTIVATE = 0;
    const uint32_t hkpEntityActivation_DO_ACTIVATE = 1;

    // hkpRigidBody::enableDeactivation(body, enable): false writes 0xff to hkpMotion::m_deactivationIntegrateCounter ("never")
    typedef void hkpRigidBody_enableDeactivation_FUNC(hkpEntity* body, bool enable);
    hkpRigidBody_enableDeactivation_FUNC* hk_enableDeactivation = (hkpRigidBody_enableDeactivation_FUNC*)0x1409c64e0;
    const uint8_t hkpMotion_NEVER_DEACTIVATE = 0xff;

    // A body's address and wrapper can both come back on a different body (a warp or map load frees bodies and makes new ones, and
    // map collision has no wrapper), so the world's m_uid, assigned when a body is added, is what identifies it
    struct BodyId
    {
        void* wrapper;
        uint32_t uid;
        bool operator==(const BodyId& o) const { return wrapper == o.wrapper && uid == o.uid; }
    };

    struct FrozenBody
    {
        BodyId id;
        uint8_t motion_type;
    };

    std::unordered_set<void*> disabled_chrs;          // characters we disabled (not ones that already were)
    std::unordered_set<void*> unbreakable_objs;       // objects whose disable-break bit we set
    std::unordered_map<hkpEntity*, FrozenBody> frozen_bodies;
    std::unordered_map<hkpEntity*, BodyId> awake_player_bodies;   // player bodies we stopped from deactivating

    template <typename T> T rd(const void* p, size_t off) { return *(const T*)((const uint8_t*)p + off); }

    bool is_session_player(const void* p)
    {
        if (p == NULL)
        {
            return false;
        }
        for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
        {
            auto player_o = Game::get_connected_player(i);
            PlayerIns* player = player_o.has_value() ? (PlayerIns*)player_o.value() : NULL;
            if (player != NULL && (p == (void*)player || p == (void*)&player->chrins))
            {
                return true;
            }
        }
        return false;
    }

    //apply the given callback function to each world chr
    template <typename F> void for_each_world_chr(F f)
    {
        void* wcm = *(void**)Game::world_chr_man_imp;
        if (wcm == NULL)
        {
            return;
        }
        const uint32_t nblocks = rd<uint32_t>(wcm, 0x28);
        const uint8_t* blocks = rd<const uint8_t*>(wcm, 0x30);
        if (blocks == NULL || nblocks > 64)
        {
            return;
        }
        for (uint32_t b = 0; b < nblocks; b++)
        {
            const uint8_t* block = blocks + b * 0xe8 + 0x48;
            const uint32_t nchrs = rd<uint32_t>(block, 0);
            const uint8_t* list = rd<const uint8_t*>(block, 8);
            if (list == NULL)
            {
                continue;
            }
            for (uint32_t c = 0; c < nchrs; c++)
            {
                const uint8_t* node = list + c * 0x38;
                void* chr = rd<void*>(node, 0);
                if (chr != NULL && !is_session_player(chr))
                {
                    f(chr, rd<uint8_t>(node, 0x20));
                }
            }
        }
    }

    //apply the given callback function to each obj
    template <typename F> void for_each_obj(F f)
    {
        void* wom = *(void**)WorldObjManImp_global;
        if (wom == NULL)
        {
            return;
        }
        const int32_t nblocks = rd<int32_t>(wom, 0x28);
        uint8_t* blocks = rd<uint8_t*>(wom, 0x30);
        if (blocks == NULL || nblocks < 0 || nblocks > 256)
        {
            return;
        }
        for (int32_t b = 0; b < nblocks; b++)
        {
            uint8_t* block = blocks + b * 0x160;
            const int32_t nobjs = rd<int32_t>(block, 0x48);
            uint8_t* objs = rd<uint8_t*>(block, 0x50);
            if (objs == NULL || nobjs < 0)
            {
                continue;
            }
            for (int32_t o = 0; o < nobjs; o++)
            {
                uint8_t* obj = objs + o * 0x120;
                if (rd<void*>(obj, 0) != NULL)   // constructed (has its vtable)
                {
                    f(obj);
                }
            }
        }
    }

    hkpWorld* world()
    {
        FrpgHavokManImp* hm = *(FrpgHavokManImp**)Game::frpg_havok_man_imp;
        if (hm == NULL || hm->physWorld == NULL)
        {
            return NULL;
        }
        return hm->physWorld->_hkpWorld;
    }

    // Every body in the world that can move (the fixed island only holds fixed ones)
    void movable_bodies(hkpWorld* w, std::vector<hkpEntity*>& out)
    {
        out.clear();
        auto island = [&out](hkpSimulationIsland* isl)
        {
            if (isl == NULL)
            {
                return;
            }
            for (uint32_t j = 0; j < isl->m_entities_size; j++)
            {
                if (isl->m_entities[j] != NULL)
                {
                    out.push_back(isl->m_entities[j]);
                }
            }
        };
        for (uint32_t i = 0; i < w->m_activeSimulationIslands_size; i++) island(w->m_activeSimulationIslands[i]);
        for (uint32_t i = 0; i < w->m_inactiveSimulationIslands_size; i++) island(w->m_inactiveSimulationIslands[i]);
    }

    // hkpWorldObject::m_userData -> the game wrapper (FrpgPhysIns / FrpgPhysSysIns), whose +0x10 is its owner
    void* wrapper_of(const hkpEntity* e)
    {
        return e->m_userData != NULL ? rd<void*>(e->m_userData, 0) : NULL;
    }

    BodyId id_of(const hkpEntity* e)
    {
        return { wrapper_of(e), e->m_uid };
    }

    bool player_owned(const hkpEntity* e)
    {
        void* wrapper = wrapper_of(e);
        return wrapper != NULL && is_session_player(rd<void*>(wrapper, 0x10));
    }

    bool world_unlocked(const hkpWorld* w)
    {
        return w->m_criticalOperationsLockCount == 0 && w->m_criticalOperationsLockCountForPhantoms == 0;
    }

    struct Counts { uint32_t chrs = 0, objs = 0, bodies = 0, damage = 0, awake = 0; };

    Counts apply()
    {
        Counts n;
        for_each_world_chr([&n](void* chr, uint8_t slot_flags)
        {
            if ((slot_flags & 1) == 0)
            {
                ChrIns_SetDisable(chr, 1);
                disabled_chrs.insert(chr);
                n.chrs++;
            }
        });
        for_each_obj([&n](uint8_t* obj)
        {
            if ((obj[0x45] & ObjIns_disable_break) == 0)
            {
                obj[0x45] |= ObjIns_disable_break;
                unbreakable_objs.insert(obj);
                n.objs++;
            }
        });
        hkpWorld* w = world();
        if (w != NULL && world_unlocked(w))
        {
            //collect first: a motion type change moves the body to another island
            std::vector<hkpEntity*> bodies;
            movable_bodies(w, bodies);
            for (hkpEntity* e : bodies)
            {
                const uint8_t type = e->m_motion.m_type;
                if (type == hkpMotion_FIXED)
                {
                    continue;
                }
                if (player_owned(e))
                {
                    //A player's hurtboxes and weapon bodies must never go to sleep: when Havok deactivates an island it zeroes the
                    //velocities and freezes the swept transform, and island sleep is world state rollback does not restore, so live
                    //and re-simulated frames put them to sleep on different frames (synctest, idle, 2026-09-27)
                    if (e->m_motion.m_deactivationIntegrateCounter != hkpMotion_NEVER_DEACTIVATE)
                    {
                        hk_enableDeactivation(e, false);
                        awake_player_bodies[e] = id_of(e);
                        n.awake++;
                    }
                    continue;
                }
                frozen_bodies[e] = { id_of(e), type };
                hk_setMotionType(e, hkpMotion_FIXED, hkpEntityActivation_DO_NOT_ACTIVATE);
                n.bodies++;
            }
        }
        return n;
    }
}

extern "C" bool rollback_damage_attacker_allowed(void* attacker)
{
    if (is_session_player(attacker))
    {
        return true;
    }
    blocked_damage++;
    return false;
}

bool WorldFreeze::frozen()
{
    return rollback_world_frozen != 0;
}

WorldFreeze::Status WorldFreeze::status()
{
    size_t world_fixed = 0, world_movable = 0;
    hkpWorld* w = world();
    if (w != NULL)
    {
        std::vector<hkpEntity*> bodies;
        movable_bodies(w, bodies);
        world_movable = bodies.size();
        world_fixed = w->m_fixedIsland != NULL ? w->m_fixedIsland->m_entities_size : 0;
    }
    return { frozen(), disabled_chrs.size(), unbreakable_objs.size(), frozen_bodies.size(), awake_player_bodies.size(), blocked_damage,
        world_fixed, world_movable };
}

void WorldFreeze::freeze()
{
    if (frozen())
    {
        return;
    }
    rollback_world_frozen = 1;
    Counts n = apply();
    DamageMan* dm = *(DamageMan**)Game::damage_man;
    if (dm != NULL)
    {
        n.damage = DamageMan_retire_world_entries(dm);
    }
    ConsoleWrite("WorldFreeze: disabled %u characters, %u objects made unbreakable, %u bodies fixed, %u world damage entries retired, "
        "%u player bodies kept awake", n.chrs, n.objs, n.bodies, n.damage, n.awake);
}

void WorldFreeze::tick()
{
    if (!frozen())
    {
        return;
    }
    Counts n = apply();
    if (n.chrs || n.objs || n.bodies || n.awake)
    {
        ConsoleWrite("WorldFreeze: newly loaded: %u characters disabled, %u objects made unbreakable, %u bodies fixed, %u player bodies kept awake",
            n.chrs, n.objs, n.bodies, n.awake);
    }
}

void WorldFreeze::unfreeze()
{
    if (!frozen())
    {
        return;
    }
    rollback_world_frozen = 0;

    uint32_t bodies = 0, chrs = 0, objs = 0;
    hkpWorld* w = world();
    if (w != NULL && world_unlocked(w))
    {
        //only bodies still in the world, and only if they are still the body we froze
        std::vector<hkpEntity*> in_world;
        auto island = [&in_world](hkpSimulationIsland* isl)
        {
            if (isl == NULL) return;
            for (uint32_t j = 0; j < isl->m_entities_size; j++) if (isl->m_entities[j] != NULL) in_world.push_back(isl->m_entities[j]);
        };
        island(w->m_fixedIsland);
        const size_t fixed_count = in_world.size();
        for (uint32_t i = 0; i < w->m_activeSimulationIslands_size; i++) island(w->m_activeSimulationIslands[i]);
        for (uint32_t i = 0; i < w->m_inactiveSimulationIslands_size; i++) island(w->m_inactiveSimulationIslands[i]);
        for (size_t k = fixed_count; k < in_world.size(); k++)
        {
            hkpEntity* e = in_world[k];
            auto it = awake_player_bodies.find(e);
            if (it != awake_player_bodies.end() && it->second == id_of(e))
            {
                hk_enableDeactivation(e, true);
            }
        }
        in_world.resize(fixed_count);
        for (hkpEntity* e : in_world)
        {
            auto it = frozen_bodies.find(e);
            if (it != frozen_bodies.end() && it->second.id == id_of(e) && e->m_motion.m_type == hkpMotion_FIXED)
            {
                hk_setMotionType(e, it->second.motion_type, hkpEntityActivation_DO_ACTIVATE);
                bodies++;
            }
        }
    }
    frozen_bodies.clear();
    awake_player_bodies.clear();

    for_each_obj([&objs](uint8_t* obj)
    {
        if (unbreakable_objs.count(obj))
        {
            obj[0x45] &= (uint8_t)~ObjIns_disable_break;
            objs++;
        }
    });
    unbreakable_objs.clear();

    for_each_world_chr([&chrs](void* chr, uint8_t slot_flags)
    {
        if (disabled_chrs.count(chr) && (slot_flags & 1) != 0)
        {
            ChrIns_SetDisable(chr, 0);
            chrs++;
        }
    });
    disabled_chrs.clear();

    ConsoleWrite("WorldFreeze: restored %u characters, %u objects, %u bodies; blocked %u damage entries the world tried to create", chrs, objs, bodies, blocked_damage);
    blocked_damage = 0;
}
