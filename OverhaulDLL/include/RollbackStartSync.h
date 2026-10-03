#pragma once
#ifndef ROLLBACK_START_SYNC_H
#define ROLLBACK_START_SYNC_H

#include <cstdint>
#include <string>

/*
 * Session-start state handshake: makes the two machines' frame 0 the same state.
 *
 * Without it every machine enters the session with its own independently simulated copy of every
 * character: the remote one was only ever placed once by the PlayerInitPacket (position, rotation,
 * HP/SP at lobby join, seconds earlier) and has been animating on its own since, and the guest's own
 * character typically finished loading a few frames before frame 0, so it is mid-spawn-animation
 * with a different elapsed time on each machine. Nothing rollback does afterwards can repair that.
 *
 * Each machine is the authority for its own character. When GGPO reports RUNNING, before the first
 * input is added (GGPO saves frame 0 on the first ggpo_add_local_input), every machine:
 *   1. saves its LOCAL character with the rollback save path and sends the canonical values
 *      (serialize_PlayerIns in WRITE mode) to every peer on Steam channel 2
 *   2. waits, frame by frame, until every peer's snapshot has arrived
 *   3. saves the whole rollback state, replaces its own character with the snapshot it sent (undoing
 *      what it simulated while waiting), APPLYs each peer's values onto that peer's character in the
 *      saved tree (keeping this machine's own pointers), and loads the result with the normal load path
 * then lets frame 0 begin. Afterwards the receiver re-WRITEs each patched character and compares it
 * with what it was sent, so anything the apply could not write is named in the log.
 */
namespace RollbackStartSync
{
    // A new GGPO session is starting: forget the previous handshake and drop stale messages
    void begin_session();
    // Called at the frame head until it returns true; while false, frame 0 must not start
    bool tick();
    void end_session();

    // For the harness: "idle|waiting|done|failed" plus counts and the first problems, as a JSON object
    std::string status_json();
}

#endif
