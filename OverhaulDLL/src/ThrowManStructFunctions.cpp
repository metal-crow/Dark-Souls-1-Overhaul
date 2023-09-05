#include "ThrowManStructFunctions.h"
#include "Rollback.h"

static const size_t ThrowRequestQueueCapacity = 5;

void copy_ThrowMan(ThrowMan* to, ThrowMan* from, bool to_game)
{
    size_t from_len = from->throw_request_queue_cur - from->throw_request_queue_start;
    size_t to_len = to->throw_request_queue_cur - to->throw_request_queue_start;

    if (!to_game)
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
        copy_ThrowRequestedEntry(to->throw_request_queue_start[i], from->throw_request_queue_start[i], to_game);
    }
    to->throw_request_queue_cur = &to->throw_request_queue_start[to_len];

    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
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

void copy_ThrowRequestedEntry(ThrowRequestedEntry* to, ThrowRequestedEntry* from, bool to_game)
{
    to->attacker = from->attacker;
    to->defender = from->defender;
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
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
