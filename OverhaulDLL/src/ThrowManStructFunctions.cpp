#include "ThrowManStructFunctions.h"

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
