#pragma once
#ifndef WORLD_FREEZE_H
#define WORLD_FREEZE_H

#include <cstddef>
#include <cstdint>

/*
 * Rollback covers the players only (NPCs, map objects and world physics are out of scope and not saved). So while a rollback
 * session runs, the world is taken out of play, so it cannot interfere with the players or diverge between re-simulated frames:
 *
 *   characters    every character in the world blocks (enemies, NPCs) is disabled with the game's own SetDisable (FUN_140325b20,
 *                 what event scripts use): not stepped, collision moved to the no-collision layer, attachments cleared
 *   objects       every ObjIns gets the game's disable-break flag (what SetObjDisableBreak sets), and the object steps run with a
 *                 frame time of 0 (the injections in RollbackASM.asm), so nothing animates, opens, moves or counts down
 *   bodies        every rigid body in the Havok world that can move and does not belong to a session player is switched to fixed,
 *                 so props, corpses and platforms stay where they are. The session players' own bodies (hurtboxes, weapons) are
 *                 kept from deactivating instead: island sleep is world state, and live and re-simulated frames put them to sleep
 *                 on different frames
 *   damage        damage entries owned by the world are retired when the session starts, and while it runs only the session
 *                 players can create new ones (objects and event scripts kept spawning some in live frames only)
 *
 * freeze() runs when the session starts, tick() at the start of every live frame (things loaded since), unfreeze() when it ends.
 * unfreeze() only undoes what freeze()/tick() changed, and only on characters, objects and bodies still in the world.
 * The harness "freeze on|off" command runs the same freeze without a GGPO session (tick() then comes from the harness pump).
 */
namespace WorldFreeze
{
    void freeze();
    void tick();
    void unfreeze();
    bool frozen();

    struct Status
    {
        bool frozen;
        size_t chrs, objs, bodies, awake;   // what is currently held disabled / unbreakable / fixed / awake
        uint32_t blocked_damage;            // world damage entries refused since the freeze started
        size_t world_fixed, world_movable;  // every body in the Havok world right now, fixed and not (compare before/after a freeze)
    };
    Status status();
}

//read by the object step injections in RollbackASM.asm
extern "C" uint8_t rollback_world_frozen;
//the damage entry count check (FUN_1403ca770) asks this while the world is frozen: false makes the game create no entry
extern "C" bool rollback_damage_attacker_allowed(void* attacker);

#endif
