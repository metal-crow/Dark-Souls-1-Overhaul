#pragma once
#ifndef HAVOK_TRACE_H
#define HAVOK_TRACE_H

//Havok object-lifetime tracer, used to design a by-value havok rollback.
//Only active when built with /DHAVOK_TRACE=1 (normally together with /DGGPO_SYNCTEST=1); otherwise every entry point is an
//empty inline. It writes dsoverhaul_havoktrace.jsonl next to the exe, one JSON object per line:
//  add / remove   a phantom or entity entered or left the hkpWorld, with the step that did it: "live" (a live frame),
//                 "load" (our restore), "resim" (a re-simulated frame) or "gap" (between two bracketed points)
//  shape          an object in the world changed its shape pointer
//  free           an object or shape that was in the world was freed through the hkThreadMemory hooks
//  chara          a player's HavokChara wrappers, proxy phantom, capsules or their world flags changed
//  sys            a player-owned FrpgPhysSysIns changed its world flag, flags, bodies in the world or pending bits
//  dmg_flag       a pool DamageEntry phantom's FrpgPhysIns.physWorld flag disagrees with the phantom being in the world
//  dmg_dynamic    heap-allocated DamageEntries on the active list
//  pending_at_save  queued physics commands, pending hkpWorld operations or locks when GGPO saved
//  census         object counts by class (first sample, then every 600 frames)
//Objects are classified by owner: **(obj->m_userData) is the game wrapper (FrpgPhysIns / FrpgPhysSysIns), whose +0x8 short is
//its type and +0x10 its owner (FUN_1402a9cc0, FUN_1403c6850).

#include <cstdint>
#include <intrin.h>

namespace HavokTrace
{
    enum class Stage : uint8_t { None, LiveBegin, LiveEnd, Save, LoadBegin, LoadEnd, ResimBegin, ResimEnd };
}

#ifndef HAVOK_TRACE

namespace HavokTrace
{
    inline void sample(Stage) {}
    inline void on_alloc(void*, uint32_t, void*) {}
    inline void on_free(void*, uint32_t, void*) {}
}

#else

#include <windows.h>
#include <share.h>
#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Rollback.h"
#include "GameData.h"
#include "PlayerInsStruct.h"
#include "DamageManStruct.h"
#include "FrpgHavokManImpStruct.h"

#define HT_HEX(x) ((unsigned long long)(uintptr_t)(x))

namespace HavokTrace
{
    static const char* trace_filename = "dsoverhaul_havoktrace.jsonl";
    static const int dmg_pool_size = 128;

    inline const char* stage_name(Stage s)
    {
        switch (s)
        {
        case Stage::LiveBegin: return "live_begin";
        case Stage::LiveEnd: return "live_end";
        case Stage::Save: return "save";
        case Stage::LoadBegin: return "load_begin";
        case Stage::LoadEnd: return "load_end";
        case Stage::ResimBegin: return "resim_begin";
        case Stage::ResimEnd: return "resim_end";
        default: return "none";
        }
    }

    //the step that ran between two consecutive samples
    inline const char* step_name(Stage from, Stage to)
    {
        if (from == Stage::LiveBegin && to == Stage::LiveEnd) return "live";
        if (from == Stage::LoadBegin && to == Stage::LoadEnd) return "load";
        if (from == Stage::ResimBegin && to == Stage::ResimEnd) return "resim";
        return "gap";
    }

    //some pointers we follow (wrappers, physics systems) can be stale; never crash the game over a trace
    inline bool safe_read(const void* addr, void* out, size_t n)
    {
        __try
        {
            memcpy(out, addr, n);
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }

    template <class T>
    inline T rd(const void* base, size_t off, T def = T())
    {
        T v;
        if (base == NULL || !safe_read((const char*)base + off, &v, sizeof(T)))
        {
            return def;
        }
        return v;
    }

    struct AllocRec
    {
        uint32_t size = 0;
        int32_t frame = -1;
        Stage stage = Stage::None;
        uint8_t nstk = 0;
        void* stk[10] = {};
    };

    struct Ent
    {
        void* p;
        void* shape;
        char kind; //'p' phantom, 'e' entity
    };

    struct ObjInfo
    {
        char kind = '?';
        uint64_t vtable = 0;
        uint16_t msf = 0;
        int16_t refc = 0;
        void* shape = NULL;
        uint64_t shape_vt = 0;
        uint16_t shape_msf = 0;
        void* wrapper = NULL;
        int16_t wtype = -1;
        uint16_t wflags = 0;
        void* owner = NULL;
        int16_t elem_index = -1;
        uint16_t elem_flags = 0;
        std::string cls;
        bool has_alloc = false;
        AllocRec alloc;
        bool has_shape_alloc = false;
        AllocRec shape_alloc;
        int32_t first_frame = -1;
        Stage first_stage = Stage::None;
    };

    struct SysState
    {
        uint64_t vtable = 0;
        void* owner = NULL;
        std::string last;
        bool gone = false;
        int pending_bits = 0;
    };

    struct State
    {
        FILE* fp = NULL;
        bool open_failed = false;
        std::atomic<uint8_t> cur_stage{ 0 };
        std::atomic<int32_t> cur_frame{ -1 };

        //the alloc hooks run on havok worker threads too
        std::mutex mu;
        std::unordered_map<void*, AllocRec> allocs;
        std::unordered_set<void*> watched;
        std::unordered_set<void*> freed;
        std::unordered_set<void*> sysbodies; //rigid bodies allocated under Construct_FrpgPhysSysIns
        std::vector<std::string> free_lines;

        //game thread only
        std::vector<Ent> prev;
        Stage prev_stage = Stage::None;
        bool have_prev = false;
        std::unordered_map<void*, ObjInfo> info;
        std::string chara_last[GGPO_MAX_PLAYERS];
        std::unordered_map<void*, SysState> sys;
        uint64_t samples = 0, events = 0, frees = 0, dmg_flag_mismatches = 0, dmg_dynamic = 0, pending_saves = 0;
    };

    inline State& S()
    {
        static State s;
        return s;
    }

    inline bool open_trace()
    {
        State& s = S();
        if (s.fp != NULL) return true;
        if (s.open_failed) return false;
        s.fp = _fsopen(trace_filename, "w", _SH_DENYWR);
        if (s.fp == NULL)
        {
            s.open_failed = true;
            ConsoleWrite("HAVOKTRACE: could not open %s", trace_filename);
            return false;
        }
        ConsoleWrite("HAVOKTRACE: writing %s", trace_filename);
        return true;
    }

    inline void emit(const char* fmt, ...)
    {
        State& s = S();
        if (s.fp == NULL) return;
        va_list args;
        va_start(args, fmt);
        vfprintf(s.fp, fmt, args);
        va_end(args);
        fputc('\n', s.fp);
        s.events++;
    }

    inline std::string alloc_json(const AllocRec& a)
    {
        std::string out;
        char buf[96];
        snprintf(buf, sizeof(buf), "{\"size\":%u,\"f\":%d,\"st\":\"%s\",\"stk\":[", a.size, a.frame, stage_name(a.stage));
        out += buf;
        for (uint8_t i = 0; i < a.nstk; i++)
        {
            snprintf(buf, sizeof(buf), "%s\"0x%llx\"", i ? "," : "", HT_HEX(a.stk[i]));
            out += buf;
        }
        out += "]}";
        return out;
    }

    // ---- allocation hooks (called from the hkThreadMemory replacements in Rollback.cpp) ----

    inline void on_alloc(void* p, uint32_t size, void* ret)
    {
        if (p == NULL) return;
        State& s = S();
        AllocRec r;
        r.size = size;
        r.frame = s.cur_frame.load();
        r.stage = (Stage)s.cur_stage.load();
        //objects (phantoms 0x1c0, entities, shapes 0x38/0x50) get a stack; small blocks only their caller
        if (size >= 0x100 || size == 0x38 || size == 0x50)
        {
            r.nstk = (uint8_t)RtlCaptureStackBackTrace(0, 10, r.stk, NULL);
        }
        else
        {
            r.stk[0] = ret;
            r.nstk = 1;
        }
        bool sysbody = false;
        if (size == 0x2c0)
        {
            for (uint8_t k = 0; k < r.nstk; k++)
            {
                //return address inside Construct_FrpgPhysSysIns @1402aede0
                if ((uint64_t)(uintptr_t)r.stk[k] == 0x1402aee46)
                {
                    sysbody = true;
                    break;
                }
            }
        }
        std::lock_guard<std::mutex> g(s.mu);
        s.allocs[p] = r;
        if (sysbody)
        {
            s.sysbodies.insert(p);
            char buf[96];
            snprintf(buf, sizeof(buf), "{\"ev\":\"sysbody_alloc\",\"p\":\"0x%llx\",\"by\":", HT_HEX(p));
            s.free_lines.push_back(std::string(buf) + alloc_json(r) + "}");
        }
    }

    inline void on_free(void* p, uint32_t size, void* ret)
    {
        if (p == NULL) return;
        State& s = S();
        std::lock_guard<std::mutex> g(s.mu);
        s.allocs.erase(p);
        const bool was_sysbody = s.sysbodies.erase(p) != 0;
        const bool was_watched = s.watched.erase(p) != 0;
        if (!was_sysbody && !was_watched) return;
        AllocRec r;
        r.size = size;
        r.frame = s.cur_frame.load();
        r.stage = (Stage)s.cur_stage.load();
        r.nstk = (uint8_t)RtlCaptureStackBackTrace(0, 10, r.stk, NULL);
        char buf[128];
        snprintf(buf, sizeof(buf), "{\"ev\":\"%s\",\"p\":\"0x%llx\",\"ret\":\"0x%llx\",\"by\":", was_watched ? "free" : "sysbody_free", HT_HEX(p), HT_HEX(ret));
        s.free_lines.push_back(std::string(buf) + alloc_json(r) + "}");
        if (was_watched) s.freed.insert(p);
    }

    // ---- world sampling ----

    inline hkpWorld* world()
    {
        FrpgHavokManImp* hm = *(FrpgHavokManImp**)Game::frpg_havok_man_imp;
        if (hm == NULL || hm->physWorld == NULL) return NULL;
        return hm->physWorld->_hkpWorld;
    }

    inline void collect(hkpWorld* w, std::vector<Ent>& out)
    {
        out.clear();
        auto island = [&out](hkpSimulationIsland* isl)
        {
            if (isl == NULL) return;
            for (uint32_t j = 0; j < isl->m_entities_size; j++)
            {
                hkpEntity* e = isl->m_entities[j];
                if (e != NULL) out.push_back({ (void*)e, e->m_collidable.base.shape, 'e' });
            }
        };
        island(w->m_fixedIsland);
        for (uint32_t i = 0; i < w->m_activeSimulationIslands_size; i++) island(w->m_activeSimulationIslands[i]);
        for (uint32_t i = 0; i < w->m_inactiveSimulationIslands_size; i++) island(w->m_inactiveSimulationIslands[i]);
        for (uint32_t i = 0; i < w->m_phantoms_size; i++)
        {
            hkpPhantom* ph = w->m_phantoms[i];
            if (ph != NULL) out.push_back({ (void*)ph, ph->m_collidable.base.shape, 'p' });
        }
        std::sort(out.begin(), out.end(), [](const Ent& a, const Ent& b) { return a.p < b.p; });
    }

    inline bool in_world(const std::vector<Ent>& v, void* p)
    {
        auto it = std::lower_bound(v.begin(), v.end(), p, [](const Ent& a, void* x) { return a.p < x; });
        return it != v.end() && it->p == p;
    }

    inline PlayerIns* player(uint32_t i)
    {
        auto po = Game::get_connected_player(i);
        if (!po.has_value() || po.value() == 0) return NULL;
        return (PlayerIns*)po.value();
    }

    inline HavokChara* havok_chara(PlayerIns* pl)
    {
        if (pl == NULL || pl->chrins.playerCtrl == NULL) return NULL;
        return pl->chrins.playerCtrl->chrCtrl.havokChara;
    }

    inline std::string classify(const Ent& e, const ObjInfo& i, hkpWorld* w)
    {
        char buf[64];
        hkpBroadPhaseBorder* border = w->m_broadPhaseBorder;
        if (border != NULL && e.kind == 'p')
        {
            void* ph[6] = { border->phantom1, border->phantom2, border->phantom3, border->phantom4, border->phantom5, border->phantom6 };
            for (void* b : ph)
            {
                if (b == e.p) return "border";
            }
        }
        for (uint32_t pi = 0; pi < (uint32_t)Rollback::ggpoCurrentPlayerCount && pi < GGPO_MAX_PLAYERS; pi++)
        {
            PlayerIns* pl = player(pi);
            if (pl == NULL) continue;
            const char* sub = NULL;
            HavokChara* hc = havok_chara(pl);
            if (hc != NULL)
            {
                if (hc->char_proxy != NULL && e.p == hc->char_proxy->HkpSimpleShapePhantom) sub = "proxy";
                else if (i.wrapper != NULL && i.wrapper == (void*)hc->physShapePhantomIns_1) sub = "hc1";
                else if (i.wrapper != NULL && i.wrapper == (void*)hc->physShapePhantomIns_2) sub = "hc2";
            }
            if (sub != NULL || (i.owner != NULL && (i.owner == (void*)pl || i.owner == (void*)&pl->chrins)))
            {
                snprintf(buf, sizeof(buf), "p%u.%s.t%d", pi, sub != NULL ? sub : (e.kind == 'e' ? "body" : "ph"), i.wtype);
                return buf;
            }
        }
        DamageMan* dm = *(DamageMan**)Game::damage_man;
        if (dm != NULL && i.owner != NULL && i.wtype == 3)
        {
            DamageEntry* start = dm->all_damage_entries_list_start;
            ptrdiff_t off = (char*)i.owner - (char*)start;
            if (start != NULL && off >= 0 && off < (ptrdiff_t)(dmg_pool_size * sizeof(DamageEntry)) && off % sizeof(DamageEntry) == 0)
            {
                DamageEntry* de = (DamageEntry*)i.owner;
                const char* part = i.wrapper == (void*)de->FrpgPhysShapePhantomIns_Sphere ? "s" : i.wrapper == (void*)de->FrpgPhysShapePhantomIns_Capsule ? "c" : "?";
                snprintf(buf, sizeof(buf), "dmg%d%s", (int)(off / sizeof(DamageEntry)), part);
                return buf;
            }
            snprintf(buf, sizeof(buf), "dmgdyn.id%x", rd<uint32_t>(i.owner, 0));
            return buf;
        }
        snprintf(buf, sizeof(buf), "w.t%d", i.wtype);
        return buf;
    }

    inline std::string family(const std::string& cls)
    {
        if (cls.compare(0, 6, "dmgdyn") == 0) return "dmgdyn";
        if (cls.compare(0, 3, "dmg") == 0) return "dmg";
        return cls;
    }

    inline ObjInfo make_info(const Ent& e, hkpWorld* w, int32_t frame, Stage st)
    {
        State& s = S();
        ObjInfo i;
        i.kind = e.kind;
        i.vtable = rd<uint64_t>(e.p, 0);
        i.msf = rd<uint16_t>(e.p, 8);
        i.refc = rd<int16_t>(e.p, 0xa);
        i.shape = e.shape;
        if (e.shape != NULL)
        {
            i.shape_vt = rd<uint64_t>(e.shape, 0);
            i.shape_msf = rd<uint16_t>(e.shape, 8);
        }
        //hkpWorldObject::m_userData
        void* ud = rd<void*>(e.p, 0x18);
        if (ud != NULL)
        {
            i.wrapper = rd<void*>(ud, 0);
            if (e.kind == 'e')
            {
                //FrpgPhysSysIns per-body element: {sys, int16 index, uint16 flags}
                i.elem_index = rd<int16_t>(ud, 8);
                i.elem_flags = rd<uint16_t>(ud, 0xa);
            }
        }
        if (i.wrapper != NULL)
        {
            i.wtype = rd<int16_t>(i.wrapper, 8);
            i.wflags = rd<uint16_t>(i.wrapper, 0xa);
            i.owner = rd<void*>(i.wrapper, 0x10);
        }
        i.cls = classify(e, i, w);
        i.first_frame = frame;
        i.first_stage = st;
        std::lock_guard<std::mutex> g(s.mu);
        auto it = s.allocs.find(e.p);
        if (it != s.allocs.end())
        {
            i.has_alloc = true;
            i.alloc = it->second;
        }
        if (e.shape != NULL)
        {
            auto it2 = s.allocs.find(e.shape);
            if (it2 != s.allocs.end())
            {
                i.has_shape_alloc = true;
                i.shape_alloc = it2->second;
            }
            s.watched.insert(e.shape);
        }
        s.watched.insert(e.p);
        s.freed.erase(e.p);
        return i;
    }

    inline std::string info_json(void* p, const ObjInfo& i)
    {
        char buf[640];
        snprintf(buf, sizeof(buf),
            "\"p\":\"0x%llx\",\"k\":\"%c\",\"cls\":\"%s\",\"vt\":\"0x%llx\",\"msf\":%u,\"rc\":%d,\"sh\":\"0x%llx\",\"shvt\":\"0x%llx\",\"shmsf\":%u,"
            "\"wr\":\"0x%llx\",\"wt\":%d,\"wf\":%u,\"own\":\"0x%llx\",\"ei\":%d,\"ef\":%u,\"first\":[%d,\"%s\"]",
            HT_HEX(p), i.kind, i.cls.c_str(), (unsigned long long)i.vtable, i.msf, i.refc, HT_HEX(i.shape), (unsigned long long)i.shape_vt, i.shape_msf,
            HT_HEX(i.wrapper), i.wtype, i.wflags, HT_HEX(i.owner), i.elem_index, i.elem_flags, i.first_frame, stage_name(i.first_stage));
        std::string out = buf;
        if (i.has_alloc) out += ",\"alloc\":" + alloc_json(i.alloc);
        if (i.has_shape_alloc) out += ",\"shalloc\":" + alloc_json(i.shape_alloc);
        return out;
    }

    inline void census(const std::vector<Ent>& now, hkpWorld* w, int32_t frame, Stage st, bool first)
    {
        State& s = S();
        std::map<std::string, int> counts;
        for (const Ent& e : now)
        {
            auto it = s.info.find(e.p);
            if (first || it == s.info.end())
            {
                s.info[e.p] = make_info(e, w, frame, st);
                it = s.info.find(e.p);
                //individual lines for everything that is not ambient world
                if (first && it->second.cls.compare(0, 2, "w.") != 0 && it->second.cls != "border")
                {
                    emit("{\"f\":%d,\"at\":\"%s\",\"ev\":\"present\",%s}", frame, stage_name(st), info_json(e.p, it->second).c_str());
                }
            }
            counts[family(it->second.cls) + (e.kind == 'e' ? "/e" : "/p")]++;
        }
        std::string body;
        char buf[96];
        for (auto& kv : counts)
        {
            snprintf(buf, sizeof(buf), "%s\"%s\":%d", body.empty() ? "" : ",", kv.first.c_str(), kv.second);
            body += buf;
        }
        size_t nallocs;
        {
            std::lock_guard<std::mutex> g(s.mu);
            nallocs = s.allocs.size();
        }
        emit("{\"f\":%d,\"at\":\"%s\",\"ev\":\"census\",\"total\":%zu,\"allocs_live\":%zu,\"events\":%llu,\"frees\":%llu,\"dmg_flag_mismatches\":%llu,"
             "\"dmg_dynamic\":%llu,\"pending_saves\":%llu,\"counts\":{%s}}",
            frame, stage_name(st), now.size(), nallocs, s.events, s.frees, s.dmg_flag_mismatches, s.dmg_dynamic, s.pending_saves, body.c_str());
        ConsoleWrite("HAVOKTRACE f=%d objs=%zu events=%llu frees=%llu dmg_flag=%llu dmg_dyn=%llu pending_saves=%llu",
            frame, now.size(), s.events, s.frees, s.dmg_flag_mismatches, s.dmg_dynamic, s.pending_saves);
    }

    inline void diff(const std::vector<Ent>& before, const std::vector<Ent>& after, hkpWorld* w, int32_t frame, Stage from, Stage to)
    {
        State& s = S();
        const char* step = step_name(from, to);
        size_t a = 0, b = 0;
        while (a < before.size() || b < after.size())
        {
            if (b >= after.size() || (a < before.size() && before[a].p < after[b].p))
            {
                const Ent& e = before[a++];
                bool freed;
                {
                    std::lock_guard<std::mutex> g(s.mu);
                    freed = s.freed.count(e.p) != 0;
                }
                auto it = s.info.find(e.p);
                if (it != s.info.end())
                {
                    emit("{\"f\":%d,\"step\":\"%s\",\"at\":\"%s\",\"ev\":\"remove\",\"freed\":%s,%s}",
                        frame, step, stage_name(to), freed ? "true" : "false", info_json(e.p, it->second).c_str());
                }
                else
                {
                    emit("{\"f\":%d,\"step\":\"%s\",\"at\":\"%s\",\"ev\":\"remove\",\"freed\":%s,\"p\":\"0x%llx\",\"k\":\"%c\",\"cls\":\"unknown\"}",
                        frame, step, stage_name(to), freed ? "true" : "false", HT_HEX(e.p), e.kind);
                }
            }
            else if (a >= before.size() || after[b].p < before[a].p)
            {
                const Ent& e = after[b++];
                ObjInfo i = make_info(e, w, frame, to);
                auto it = s.info.find(e.p);
                if (it != s.info.end())
                {
                    //re-added: keep when it was first seen
                    i.first_frame = it->second.first_frame;
                    i.first_stage = it->second.first_stage;
                }
                s.info[e.p] = i;
                emit("{\"f\":%d,\"step\":\"%s\",\"at\":\"%s\",\"ev\":\"add\",%s}", frame, step, stage_name(to), info_json(e.p, i).c_str());
            }
            else
            {
                const Ent& e0 = before[a++];
                const Ent& e1 = after[b++];
                if (e0.shape == e1.shape) continue;
                auto it = s.info.find(e1.p);
                std::string cls = it != s.info.end() ? it->second.cls : "unknown";
                uint64_t vt = rd<uint64_t>(e1.shape, 0);
                uint16_t msf = rd<uint16_t>(e1.shape, 8);
                AllocRec r;
                bool has = false;
                {
                    std::lock_guard<std::mutex> g(s.mu);
                    auto f = s.allocs.find(e1.shape);
                    if (f != s.allocs.end())
                    {
                        r = f->second;
                        has = true;
                    }
                    if (e1.shape != NULL) s.watched.insert(e1.shape);
                }
                std::string alloc = has ? ",\"alloc\":" + alloc_json(r) : std::string();
                emit("{\"f\":%d,\"step\":\"%s\",\"at\":\"%s\",\"ev\":\"shape\",\"p\":\"0x%llx\",\"cls\":\"%s\",\"old\":\"0x%llx\",\"new\":\"0x%llx\",\"newvt\":\"0x%llx\",\"newmsf\":%u%s}",
                    frame, step, stage_name(to), HT_HEX(e1.p), cls.c_str(), HT_HEX(e0.shape), HT_HEX(e1.shape), (unsigned long long)vt, msf, alloc.c_str());
                if (it != s.info.end())
                {
                    it->second.shape = e1.shape;
                    it->second.shape_vt = vt;
                    it->second.shape_msf = msf;
                }
            }
        }
    }

    inline void chara_track(const std::vector<Ent>& now, int32_t frame, Stage st)
    {
        State& s = S();
        for (uint32_t pi = 0; pi < (uint32_t)Rollback::ggpoCurrentPlayerCount && pi < GGPO_MAX_PLAYERS; pi++)
        {
            HavokChara* hc = havok_chara(player(pi));
            if (hc == NULL) continue;
            void* proxy_ph = hc->char_proxy != NULL ? hc->char_proxy->HkpSimpleShapePhantom : NULL;
            void* w1 = hc->physShapePhantomIns_1;
            void* w2 = hc->physShapePhantomIns_2;
            void* ph1 = rd<void*>(w1, 0x20);
            void* ph2 = rd<void*>(w2, 0x20);
            char buf[768];
            snprintf(buf, sizeof(buf),
                "\"hc\":\"0x%llx\",\"proxy_ph\":\"0x%llx\",\"proxy_ph_in\":%d,\"proxy_ph_shape\":\"0x%llx\",\"cap1\":\"0x%llx\",\"cap2\":\"0x%llx\","
                "\"w1\":\"0x%llx\",\"w1_type\":%d,\"w1_ph\":\"0x%llx\",\"w1_flag\":%d,\"w1_in\":%d,\"w1_shape\":\"0x%llx\","
                "\"w2\":\"0x%llx\",\"w2_type\":%d,\"w2_ph\":\"0x%llx\",\"w2_flag\":%d,\"w2_in\":%d,\"w2_shape\":\"0x%llx\"",
                HT_HEX(hc), HT_HEX(proxy_ph), proxy_ph != NULL && in_world(now, proxy_ph), HT_HEX(rd<void*>(proxy_ph, 0x20)),
                HT_HEX(hc->capsule_shape_1), HT_HEX(hc->capsule_shape_2),
                HT_HEX(w1), rd<int16_t>(w1, 8, -1), HT_HEX(ph1), rd<void*>(w1, 0x18) != NULL, ph1 != NULL && in_world(now, ph1), HT_HEX(rd<void*>(ph1, 0x20)),
                HT_HEX(w2), rd<int16_t>(w2, 8, -1), HT_HEX(ph2), rd<void*>(w2, 0x18) != NULL, ph2 != NULL && in_world(now, ph2), HT_HEX(rd<void*>(ph2, 0x20)));
            if (s.chara_last[pi] != buf)
            {
                emit("{\"f\":%d,\"at\":\"%s\",\"ev\":\"chara\",\"player\":%u,%s}", frame, stage_name(st), pi, buf);
                s.chara_last[pi] = buf;
            }
        }
    }

    //player-owned FrpgPhysSysIns, discovered through their bodies
    inline void sys_track(const std::vector<Ent>& now, int32_t frame, Stage from, Stage st)
    {
        State& s = S();
        std::unordered_map<void*, int> in_world_count;
        for (const Ent& e : now)
        {
            if (e.kind != 'e') continue;
            auto it = s.info.find(e.p);
            if (it == s.info.end() || it->second.cls.empty() || it->second.cls[0] != 'p' || it->second.wrapper == NULL) continue;
            in_world_count[it->second.wrapper]++;
            if (s.sys.find(it->second.wrapper) == s.sys.end())
            {
                SysState ss;
                ss.vtable = rd<uint64_t>(it->second.wrapper, 0);
                ss.owner = it->second.owner;
                s.sys[it->second.wrapper] = ss;
            }
        }
        for (auto& kv : s.sys)
        {
            SysState& ss = kv.second;
            if (ss.gone) continue;
            void* sys = kv.first;
            if (rd<uint64_t>(sys, 0) != ss.vtable)
            {
                ss.gone = true;
                emit("{\"f\":%d,\"at\":\"%s\",\"ev\":\"sys_gone\",\"sys\":\"0x%llx\"}", frame, stage_name(st), HT_HEX(sys));
                continue;
            }
            void* physsys = rd<void*>(sys, 0x28);
            int32_t total = rd<int32_t>(physsys, 0x18);
            const bool valid = total >= 0 && total <= 256;
            if (!valid) total = -1;
            void* elems = rd<void*>(sys, 0x30);
            int pending = 0;
            for (int32_t k = 0; valid && k < total; k++)
            {
                if (rd<uint16_t>(elems, (size_t)k * 0x10 + 0xa) & 1) pending++;
            }
            ss.pending_bits = pending;
            auto iw = in_world_count.find(sys);
            char buf[256];
            snprintf(buf, sizeof(buf), "\"world_flag\":%d,\"flags\":%u,\"type\":%d,\"bodies\":%d,\"in_world\":%d,\"pending_bits\":%d",
                rd<void*>(sys, 0x18) != NULL, rd<uint16_t>(sys, 0xa), rd<int16_t>(sys, 8, -1), total, iw != in_world_count.end() ? iw->second : 0, pending);
            if (ss.last != buf)
            {
                emit("{\"f\":%d,\"step\":\"%s\",\"at\":\"%s\",\"ev\":\"sys\",\"sys\":\"0x%llx\",\"owner\":\"0x%llx\",%s}",
                    frame, step_name(from, st), stage_name(st), HT_HEX(sys), HT_HEX(ss.owner), buf);
                ss.last = buf;
            }
        }
    }

    inline void dmg_check(const std::vector<Ent>& now, int32_t frame, Stage st)
    {
        State& s = S();
        DamageMan* dm = *(DamageMan**)Game::damage_man;
        if (dm == NULL || dm->all_damage_entries_list_start == NULL) return;
        int mismatches = 0;
        for (int idx = 0; idx < dmg_pool_size; idx++)
        {
            DamageEntry* de = &dm->all_damage_entries_list_start[idx];
            FrpgPhysShapePhantomIns* wr[2] = { de->FrpgPhysShapePhantomIns_Sphere, de->FrpgPhysShapePhantomIns_Capsule };
            for (int k = 0; k < 2; k++)
            {
                if (wr[k] == NULL) continue;
                void* ph = wr[k]->base._hkpSimpleShapePhantom;
                bool flag = wr[k]->base.base.physWorld != NULL;
                bool inw = ph != NULL && in_world(now, ph);
                if (flag == inw) continue;
                mismatches++;
                if (mismatches <= 4)
                {
                    emit("{\"f\":%d,\"at\":\"%s\",\"ev\":\"dmg_flag\",\"idx\":%d,\"part\":\"%c\",\"flag\":%d,\"in_world\":%d,\"id\":%u,\"attacker\":%u}",
                        frame, stage_name(st), idx, k ? 'c' : 's', flag, inw, de->id, de->attackerHandle);
                }
            }
        }
        s.dmg_flag_mismatches += mismatches;
        int n = 0, dyn = 0;
        for (DamageEntry* e = dm->active_damage_entries_list; e != NULL && n < 512; e = e->next, n++)
        {
            if (((e->id >> 16) & 0xffff) == 0x80) dyn++;
        }
        if (dyn > 0)
        {
            s.dmg_dynamic += dyn;
            emit("{\"f\":%d,\"at\":\"%s\",\"ev\":\"dmg_dynamic\",\"count\":%d}", frame, stage_name(st), dyn);
        }
    }

    inline void pending_check(hkpWorld* w, int32_t frame)
    {
        State& s = S();
        FrpgHavokManImp* hm = *(FrpgHavokManImp**)Game::frpg_havok_man_imp;
        void* pw = hm->physWorld;
        //FrpgPhysWorld+0x18/+0x20: the queued physics command vector FUN_1402af900 flushes
        uint64_t cb = rd<uint64_t>(pw, 0x18), ce = rd<uint64_t>(pw, 0x20);
        long long cmds = ce >= cb ? (long long)((ce - cb) / 8) : -1;
        int ops = rd<int32_t>(w, 0xE0), body_ops = rd<int32_t>(w, 0xE4), lock = rd<int32_t>(w, 0xE8), lock_ph = rd<int32_t>(w, 0xEC), queues = rd<int32_t>(w, 0x100);
        int sys_pending = 0;
        for (auto& kv : s.sys)
        {
            if (!kv.second.gone && kv.second.pending_bits != 0) sys_pending++;
        }
        if (cmds == 0 && ops == 0 && body_ops == 0 && lock == 0 && lock_ph == 0 && queues == 0 && sys_pending == 0) return;
        s.pending_saves++;
        emit("{\"f\":%d,\"ev\":\"pending_at_save\",\"cmds\":%lld,\"ops\":%d,\"body_ops\":%d,\"lock\":%d,\"lock_ph\":%d,\"queues\":%d,\"player_sys_pending\":%d}",
            frame, cmds, ops, body_ops, lock, lock_ph, queues, sys_pending);
    }

    inline void sample(Stage st)
    {
        State& s = S();
        if (!Rollback::ggpoStarted || Rollback::ggpo == NULL) return;
        hkpWorld* w = world();
        if (w == NULL || !open_trace()) return;
        int frame = 0, confirmed = 0;
        ggpo_get_frame_info(Rollback::ggpo, &frame, &confirmed);
        s.cur_frame = frame;

        //frees the alloc hooks saw since the last sample
        std::vector<std::string> frees;
        {
            std::lock_guard<std::mutex> g(s.mu);
            frees.swap(s.free_lines);
        }
        for (const std::string& line : frees)
        {
            emit("%s", line.c_str());
            s.frees++;
        }

        std::vector<Ent> now;
        now.reserve(s.prev.size() + 16);
        collect(w, now);
        if (!s.have_prev)
        {
            census(now, w, frame, st, true);
        }
        else
        {
            diff(s.prev, now, w, frame, s.prev_stage, st);
        }
        chara_track(now, frame, st);
        sys_track(now, frame, s.prev_stage, st);
        if (st == Stage::LiveEnd || st == Stage::ResimEnd || st == Stage::LoadEnd || st == Stage::Save)
        {
            dmg_check(now, frame, st);
        }
        if (st == Stage::Save)
        {
            pending_check(w, frame);
        }
        s.prev.swap(now);
        s.prev_stage = st;
        s.have_prev = true;
        s.cur_stage = (uint8_t)st;
        s.samples++;
        if (st == Stage::LiveEnd)
        {
            if (frame % 600 == 0)
            {
                census(s.prev, w, frame, st, false);
            }
            fflush(s.fp);
        }
    }
}

#endif //HAVOK_TRACE
#endif //HAVOK_TRACE_H
