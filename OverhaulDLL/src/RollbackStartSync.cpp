#include "RollbackStartSync.h"
#include "DarkSoulsOverhaulMod.h"
#include "GameData.h"
#include "ModNetworking.h"
#include "Rollback.h"
#include "StateSerializer.h"
#include "PlayerInsStructFunctions.h"

#include <cstring>
#include <map>
#include <vector>

namespace
{
    const int SYNC_CHANNEL = 2;   // GGPO uses 1, the game 0
    const char SYNC_MAGIC[8] = { 'D', 'S', 'R', 'S', 'Y', 'N', 'C', '1' };
    // Log a reminder every this many frames while a peer's snapshot is outstanding
    const uint32_t WAIT_LOG_FRAMES = 120;
    const size_t MAX_LOGGED_PROBLEMS = 40;

#pragma pack(push, 1)
    struct SyncHeader
    {
        char magic[8];
        uint64_t sender_steam_id;
        uint32_t stream_len;
    };
#pragma pack(pop)

    enum class Phase { Idle, Waiting, Done, Failed };

    Phase phase = Phase::Idle;
    // The local character as it was when its snapshot was sent. The peers apply exactly this, so
    // it is also what our own character is put back to before frame 0.
    PlayerIns* own_snapshot = NULL;
    uint32_t waited_frames = 0;
    std::map<uint64_t, std::vector<uint8_t>> received;   // sender steam id -> stream
    size_t sent_bytes = 0;
    std::vector<std::string> problems;
    size_t unapplied_ops = 0;

    // The character proxy's contact manifold names world bodies at per-process addresses, and nothing can
    // translate them. Both sides start frame 0 without contacts; the proxy finds them again on its next step,
    // from positions that are now identical.
    void clear_contacts(PlayerIns* tree)
    {
        if (tree->chrins.playerCtrl != NULL && tree->chrins.playerCtrl->chrCtrl.havokChara != NULL
            && tree->chrins.playerCtrl->chrCtrl.havokChara->char_proxy != NULL)
        {
            tree->chrins.playerCtrl->chrCtrl.havokChara->char_proxy->m_manifold_len = 0;
        }
    }

    // FUN_14032d410 (the per-frame floor step) latches once, the first time the character is landed: it sets ChrIns+0x2a5
    // bit 7 and links hitins_2 to the map part the character stands on (hitins_1). The bit travels in the applied ChrIns
    // values, but a map-part pointer cannot, so a copy that had not landed yet on this machine would carry the sender's
    // "already landed" bit with no part and never link one. Link it the way the game would have: to the current part.
    void complete_first_landing_latch(ChrIns* chr)
    {
        const uint8_t latch = *((uint8_t*)chr + 0x2a5);
        if ((latch & 0x80) != 0 && chr->hitins_2 == NULL && chr->hitins_1 != NULL)
        {
            chr->hitins_2 = chr->hitins_1;
            ConsoleWrite("START SYNC: linked a character's first-landing map part (it had the latch bit from its owner but no part)");
        }
    }

    uint64_t local_steam_id()
    {
        return ModNetworking::SteamUser != NULL ? ModNetworking::SteamUser->GetSteamID().ConvertToUint64() : 0;
    }

    // The steam id GGPO connects to for connected player i (i > 0)
    uint64_t remote_steam_id(uint32_t i)
    {
        auto p = Game::get_connected_player(i);
        if (!p.has_value() || p.value() == NULL) return 0;
        PlayerIns* pi = (PlayerIns*)p.value();
        if (pi->steamPlayerData == NULL || pi->steamPlayerData->steamOnlineIDData == NULL) return 0;
        return pi->steamPlayerData->steamOnlineIDData->steam_id;
    }

    void drain_channel(bool keep)
    {
        if (ModNetworking::SteamNetMessages == NULL) return;
        SteamNetworkingMessage_t* msgs[8];
        int n;
        while ((n = ModNetworking::SteamNetMessages->ReceiveMessagesOnChannel(SYNC_CHANNEL, msgs, 8)) > 0)
        {
            for (int i = 0; i < n; i++)
            {
                SteamNetworkingMessage_t* m = msgs[i];
                const uint8_t* data = (const uint8_t*)m->GetData();
                const size_t size = (size_t)m->GetSize();
                const uint64_t from = m->m_identityPeer.GetSteamID64();
                if (keep)
                {
                    SyncHeader h;
                    if (size < sizeof(h))
                    {
                        ConsoleWrite("START SYNC: dropped a %zu-byte message from %llx (too short)", size, from);
                    }
                    else
                    {
                        memcpy(&h, data, sizeof(h));
                        if (memcmp(h.magic, SYNC_MAGIC, sizeof(SYNC_MAGIC)) != 0 || sizeof(h) + h.stream_len != size || h.sender_steam_id != from)
                        {
                            ConsoleWrite("START SYNC: dropped a malformed %zu-byte message from %llx (a different DLL build?)", size, from);
                        }
                        else
                        {
                            received[from].assign(data + sizeof(h), data + size);
                            ConsoleWrite("START SYNC: received %u bytes from %llx", h.stream_len, from);
                        }
                    }
                }
                m->Release();
            }
        }
    }

    bool send_own_snapshot()
    {
        auto pc = Game::get_connected_player(0);
        if (!pc.has_value() || pc.value() == NULL)
        {
            FATALERROR("START SYNC: no local player");
        }
        own_snapshot = init_PlayerIns();
        copy_PlayerIns(own_snapshot, (PlayerIns*)pc.value(), StateTarget::ToLocal);
        clear_contacts(own_snapshot);

        StateVisitor w(StateVisitor::Mode::Write);
        serialize_PlayerIns(w, own_snapshot);
        const std::vector<uint8_t>& stream = w.stream();

        SyncHeader h;
        memcpy(h.magic, SYNC_MAGIC, sizeof(SYNC_MAGIC));
        h.sender_steam_id = local_steam_id();
        h.stream_len = (uint32_t)stream.size();
        std::vector<uint8_t> msg(sizeof(h) + stream.size());
        memcpy(msg.data(), &h, sizeof(h));
        memcpy(msg.data() + sizeof(h), stream.data(), stream.size());
        sent_bytes = stream.size();

        for (uint32_t i = 1; i < Rollback::ggpoCurrentPlayerCount; i++)
        {
            const uint64_t to = remote_steam_id(i);
            if (to == 0)
            {
                FATALERROR("START SYNC: no steam id for player %u", i);
            }
            SteamNetworkingIdentity target{};
            target.SetSteamID64(to);
            EResult res = ModNetworking::SteamNetMessages->SendMessageToUser(target, msg.data(), (uint32)msg.size(), k_nSteamNetworkingSend_Reliable, SYNC_CHANNEL);
            if (res != k_EResultOK)
            {
                FATALERROR("START SYNC: sending %zu bytes to %llx failed (EResult %d)", msg.size(), to, (int)res);
            }
        }
        ConsoleWrite("START SYNC: sent own character (%zu bytes) to %u peer(s)", stream.size(), (unsigned)(Rollback::ggpoCurrentPlayerCount - 1));
        return true;
    }

    // Apply `stream` onto `tree` (a saved copy of that peer's character on this machine). Returns false if the two
    // did not have the same shape, in which case the op-by-op check in verify_peer would be meaningless.
    bool apply_peer(PlayerIns* tree, const std::vector<uint8_t>& stream, uint64_t from)
    {
        clear_contacts(tree);
        StateVisitor a(StateVisitor::Mode::Apply, stream.data(), stream.size());
        serialize_PlayerIns(a, tree);
        for (const std::string& p : a.problems())
        {
            problems.push_back(p);
        }
        if (!a.aborted() && !a.fully_consumed())
        {
            problems.push_back("STRUCTURE stream from " + std::to_string(from) + " has trailing bytes");
        }
        return a.structure_problems() == 0;
    }

    // After the load: save that peer's character from the game again and compare it op by op with what the peer sent,
    // so anything the apply or the load did not carry over is named
    void verify_peer(PlayerIns* game_player, const std::vector<uint8_t>& stream)
    {
        PlayerIns* tree = init_PlayerIns();
        copy_PlayerIns(tree, game_player, StateTarget::ToLocal);
        StateVisitor w(StateVisitor::Mode::Write);
        w.record_paths(true);
        serialize_PlayerIns(w, tree);
        const std::vector<uint8_t>& mine = w.stream();
        const std::vector<StateVisitor::OpAt>& ops = w.ops();
        size_t differing = 0;
        for (size_t k = 0; k < ops.size(); k++)
        {
            const size_t start = ops[k].off;
            const size_t stop = (k + 1 < ops.size()) ? ops[k + 1].off : mine.size();
            const bool same = stop <= stream.size() && memcmp(mine.data() + start, stream.data() + start, stop - start) == 0;
            if (!same)
            {
                differing++;
                if (differing <= MAX_LOGGED_PROBLEMS)
                {
                    problems.push_back("UNAPPLIED " + ops[k].path);
                }
            }
            if (stop > stream.size())
            {
                break;   // the traversals diverged in shape; already reported as STRUCTURE
            }
        }
        if (mine.size() != stream.size())
        {
            problems.push_back("STRUCTURE re-written size " + std::to_string(mine.size()) + " vs sent " + std::to_string(stream.size()));
        }
        unapplied_ops += differing;
        free_PlayerIns(tree);
    }

    void finish()
    {
        RollbackState* state = rollback_capture_state();

        //our own character goes back to exactly what the peers were sent
        free_PlayerIns(state->playerins[0]);
        state->playerins[0] = own_snapshot;
        own_snapshot = NULL;

        bool same_shape[GGPO_MAX_PLAYERS] = {};
        for (uint32_t i = 1; i < Rollback::ggpoCurrentPlayerCount; i++)
        {
            const uint64_t from = remote_steam_id(i);
            same_shape[i] = apply_peer(state->playerins[i], received[from], from);
        }

        rollback_load_game_state_callback((unsigned char*)state, sizeof(RollbackState));
        rollback_free_buffer(state);

        for (uint32_t i = 0; i < Rollback::ggpoCurrentPlayerCount; i++)
        {
            auto p = Game::get_connected_player(i);
            if (p.has_value() && p.value() != NULL)
            {
                complete_first_landing_latch(&((PlayerIns*)p.value())->chrins);
                if (i > 0 && same_shape[i])
                {
                    verify_peer((PlayerIns*)p.value(), received[remote_steam_id(i)]);
                }
            }
        }

        phase = problems.empty() ? Phase::Done : Phase::Failed;
        if (phase == Phase::Done)
        {
            ConsoleWrite("START SYNC: done after %u frame(s); every peer's character applied exactly", waited_frames);
        }
        else
        {
            ConsoleWrite("START SYNC FAILED after %u frame(s): %zu problem(s), %zu op(s) not applied. Frame 0 will differ between machines:",
                waited_frames, problems.size(), unapplied_ops);
            for (size_t k = 0; k < problems.size() && k < MAX_LOGGED_PROBLEMS; k++)
            {
                ConsoleWrite("  %s", problems[k].c_str());
            }
        }
    }

    std::string json_escape(const std::string& s)
    {
        std::string o;
        for (char c : s)
        {
            if (c == '"' || c == '\\') { o += '\\'; o += c; }
            else if ((unsigned char)c < 0x20) { char b[8]; snprintf(b, sizeof(b), "\\u%04x", (unsigned char)c); o += b; }
            else o += c;
        }
        return o;
    }
}

void RollbackStartSync::begin_session()
{
    if (own_snapshot != NULL)
    {
        free_PlayerIns(own_snapshot);
        own_snapshot = NULL;
    }
    received.clear();
    problems.clear();
    unapplied_ops = 0;
    sent_bytes = 0;
    waited_frames = 0;
    phase = Phase::Idle;
    //anything still queued belongs to an earlier session
    drain_channel(false);
}

bool RollbackStartSync::tick()
{
    if (phase == Phase::Done || phase == Phase::Failed)
    {
        return true;
    }
    if (phase == Phase::Idle)
    {
        send_own_snapshot();
        phase = Phase::Waiting;
    }

    drain_channel(true);
    for (uint32_t i = 1; i < Rollback::ggpoCurrentPlayerCount; i++)
    {
        if (received.count(remote_steam_id(i)) == 0)
        {
            waited_frames++;
            if (waited_frames % WAIT_LOG_FRAMES == 0)
            {
                ConsoleWrite("START SYNC: still waiting for %llx after %u frames", remote_steam_id(i), waited_frames);
            }
            return false;
        }
    }

    finish();
    return true;
}

void RollbackStartSync::end_session()
{
    if (own_snapshot != NULL)
    {
        free_PlayerIns(own_snapshot);
        own_snapshot = NULL;
    }
    received.clear();
}

std::string RollbackStartSync::status_json()
{
    const char* names[] = { "idle", "waiting", "done", "failed" };
    std::string s = "{\"phase\":\"" + std::string(names[(int)phase]) + "\"";
    s += ",\"waited_frames\":" + std::to_string(waited_frames);
    s += ",\"sent_bytes\":" + std::to_string(sent_bytes);
    s += ",\"problems\":" + std::to_string(problems.size());
    s += ",\"unapplied_ops\":" + std::to_string(unapplied_ops);
    s += ",\"first_problems\":[";
    for (size_t k = 0; k < problems.size() && k < 10; k++)
    {
        if (k) s += ",";
        s += "\"" + json_escape(problems[k]) + "\"";
    }
    s += "]}";
    return s;
}
