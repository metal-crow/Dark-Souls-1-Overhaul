#include "ThrowManStructFunctions.h"
#include "StateSerializer.h"

static const size_t ThrowRequestQueueCapacity = 5;

void copy_ThrowMan(ThrowMan* to, ThrowMan* from, StateTarget target)
{
    size_t from_len = from->throw_request_queue_cur - from->throw_request_queue_start;
    size_t to_len = to->throw_request_queue_cur - to->throw_request_queue_start;

    if (target != StateTarget::ToGame)
    {
        if (from_len > ThrowRequestQueueCapacity)
        {
            FATALERROR("ThrowRequestQueueCapacity exceeded");
        }
        else
        {
            //we know that our rollback array actually has this much space
            to_len = from_len;
        }
    }
    else
    {
        if (to_len < from_len)
        {
            //this function already checks the end ptr for us
            Vector_IncreaseSize((void*)(((uint64_t)to) + 0x20), from_len - to_len);
            for (size_t i = to_len; i < from_len; i++)
            {
                to->throw_request_queue_start[i] = (ThrowRequestedEntry*)Game::game_malloc(sizeof(ThrowRequestedEntry), 0x8, *(uint64_t*)Game::internal_heap_3);
            }
        }
        to_len = from_len;
        //don't need to worry about the > case since we'll set the cur ptr at the end
    }


    for(size_t i=0;i<to_len;i++)
    {
        copy_ThrowRequestedEntry(to->throw_request_queue_start[i], from->throw_request_queue_start[i], target);
    }
    to->throw_request_queue_cur = &to->throw_request_queue_start[to_len];

    to->unk_80 = from->unk_80;
    to->unk_81 = from->unk_81;
    memcpy(to->unk_82, from->unk_82, sizeof(to->unk_82));
    to->timeoutVal = from->timeoutVal;
    to->throwEscape_remainingTypeToDecayWeighting = from->throwEscape_remainingTypeToDecayWeighting;
    to->throwEscape_IncreaseValueOfGoalWeighting = from->throwEscape_IncreaseValueOfGoalWeighting;
    to->throwEscape_NowIncreaseWeighting = from->throwEscape_NowIncreaseWeighting;
    to->throwEscape_goalWeightLossVals = from->throwEscape_goalWeightLossVals;
    to->throwEscape_curWeightLossVals = from->throwEscape_curWeightLossVals;
    to->attacking_chr = from->attacking_chr;
    to->defending_chr = from->defending_chr;
    to->allDrawing = from->allDrawing;
    memcpy(to->unk_c1, from->unk_c1, sizeof(to->unk_c1));
    to->unk_c4 = from->unk_c4;
    to->animPlaySpeed = from->animPlaySpeed;
    to->unk_cc = from->unk_cc;
    memcpy(to->unk_cd, from->unk_cd, sizeof(to->unk_cd));
    to->unk_d0 = from->unk_d0;
    memcpy(to->unk_d4, from->unk_d4, sizeof(to->unk_d4));
    memcpy(to->vec_e0, from->vec_e0, sizeof(to->vec_e0));
    memcpy(to->vec_100, from->vec_100, sizeof(to->vec_100));
    memcpy(to->unk_120, from->unk_120, sizeof(to->unk_120));
    memcpy(to->unk_128, from->unk_128, sizeof(to->unk_128));
    memcpy(to->unk_12a, from->unk_12a, sizeof(to->unk_12a));
    to->unk_12c = from->unk_12c;
}

ThrowMan* init_ThrowMan()
{
    ThrowMan* local_ThrowMan = (ThrowMan*)malloc_(sizeof(ThrowMan));

    ThrowRequestedEntry** throw_request_queue = (ThrowRequestedEntry**)malloc_(sizeof(ThrowRequestedEntry*) * ThrowRequestQueueCapacity);
    local_ThrowMan->throw_request_queue_start = throw_request_queue;
    local_ThrowMan->throw_request_queue_cur = throw_request_queue;
    local_ThrowMan->throw_request_queue_end = &throw_request_queue[ThrowRequestQueueCapacity];
    for (size_t i = 0; i < ThrowRequestQueueCapacity; i++)
    {
        throw_request_queue[i] = init_ThrowRequestedEntry();
    }

    return local_ThrowMan;
}

void free_ThrowMan(ThrowMan* to)
{
    for (size_t i = 0; i < ThrowRequestQueueCapacity; i++)
    {
        free_ThrowRequestedEntry(to->throw_request_queue_start[i]);
    }
    free(to->throw_request_queue_start);
    free(to);
}

void copy_ThrowRequestedEntry(ThrowRequestedEntry* to, ThrowRequestedEntry* from, StateTarget target)
{
    to->attacker = from->attacker;
    to->defender = from->defender;
    to->throwId = from->throwId;
    to->throwTimeout = from->throwTimeout;
    to->unk_18 = from->unk_18;
    to->unk1 = from->unk1;
    memcpy(to->unk_1a, from->unk_1a, sizeof(to->unk_1a));
}

ThrowRequestedEntry* init_ThrowRequestedEntry()
{
    ThrowRequestedEntry* local_ThrowRequestedEntry = (ThrowRequestedEntry*)malloc_(sizeof(ThrowRequestedEntry));
    return local_ThrowRequestedEntry;
}

void free_ThrowRequestedEntry(ThrowRequestedEntry* to)
{
    free(to);
}

// ---- serializer (mirrors copy_ThrowMan; drives both print_ and hash_) -------

void serialize_ThrowRequestedEntry(StateVisitor& v, ThrowRequestedEntry* e)
{
    v.begin("ThrowRequestedEntry");
    // attacker/defender point into a PlayerIns' throw_animation_info; the raw
    // pointer varies across instances, so record only null/non-null. Upgrade to
    // a stable entity id if throw state ever shows persistent cross-instance drift.
    v.ptr_flag("attacker", e->attacker);
    v.ptr_flag("defender", e->defender);
    v.field("throwId", e->throwId);
    v.field("throwTimeout", e->throwTimeout);
    v.field("unk_18", e->unk_18);
    v.field("unk1", e->unk1);
    v.blob("unk_1a", e->unk_1a, sizeof(e->unk_1a));
    v.end();
}

void serialize_ThrowMan(StateVisitor& v, ThrowMan* t)
{
    v.begin("ThrowMan");

    // dynamic throw-request queue: length = cur - start (mirrors copy_ThrowMan)
    size_t len = (size_t)(t->throw_request_queue_cur - t->throw_request_queue_start);
    v.count("throw_request_queue", len);
    for (size_t i = 0; i < len; i++)
    {
        serialize_ThrowRequestedEntry(v, t->throw_request_queue_start[i]);
    }

    v.field("unk_80", t->unk_80);
    v.field("unk_81", t->unk_81);
    v.blob("unk_82", t->unk_82, sizeof(t->unk_82));
    v.field("timeoutVal", t->timeoutVal);
    v.field("throwEscape_remainingTypeToDecayWeighting", t->throwEscape_remainingTypeToDecayWeighting);
    v.field("throwEscape_IncreaseValueOfGoalWeighting", t->throwEscape_IncreaseValueOfGoalWeighting);
    v.field("throwEscape_NowIncreaseWeighting", t->throwEscape_NowIncreaseWeighting);
    v.field("throwEscape_goalWeightLossVals", t->throwEscape_goalWeightLossVals);
    v.field("throwEscape_curWeightLossVals", t->throwEscape_curWeightLossVals);
    v.field("attacking_chr", t->attacking_chr);
    v.field("defending_chr", t->defending_chr);
    v.field("allDrawing", t->allDrawing);
    v.blob("unk_c1", t->unk_c1, sizeof(t->unk_c1));
    v.field("unk_c4", t->unk_c4);
    v.field("animPlaySpeed", t->animPlaySpeed);
    v.field("unk_cc", t->unk_cc);
    v.blob("unk_cd", t->unk_cd, sizeof(t->unk_cd));
    v.field("unk_d0", t->unk_d0);
    v.blob("unk_d4", t->unk_d4, sizeof(t->unk_d4));
    for (int i = 0; i < 8; i++) v.field("vec_e0", t->vec_e0[i]);
    for (int i = 0; i < 8; i++) v.field("vec_100", t->vec_100[i]);
    for (int i = 0; i < 2; i++) v.field("unk_120", t->unk_120[i]);
    v.blob("unk_128", t->unk_128, sizeof(t->unk_128));
    v.blob("unk_12a", t->unk_12a, sizeof(t->unk_12a));
    v.field("unk_12c", t->unk_12c);

    v.end();
}

std::string print_ThrowMan(ThrowMan* t)
{
    StateVisitor v(StateVisitor::Mode::Print);
    serialize_ThrowMan(v, t);
    return v.text();
}

uint64_t hash_ThrowMan(ThrowMan* t)
{
    StateVisitor v(StateVisitor::Mode::Hash);
    serialize_ThrowMan(v, t);
    return v.digest();
}
