#include "EzStateManagerImplStructFunctions.h"

void copy_EzStateManagerImpl_field0x20(EzStateManagerImpl_field0x20* to, EzStateManagerImpl_field0x20* from, StateTarget target)
{
    //per-frame scratch space (not needed for rollback)
    //current/heap are pointers copied raw - harmless here since this is scratch.
    memcpy(to->queries, from->queries, sizeof(to->queries));
    memcpy(to->registers, from->registers, sizeof(to->registers));
    to->current = from->current;
    to->heap = from->heap;
}

EzStateManagerImpl_field0x20* init_EzStateManagerImpl_field0x20()
{
    EzStateManagerImpl_field0x20* local = (EzStateManagerImpl_field0x20*)malloc_(sizeof(EzStateManagerImpl_field0x20));
    return local;
}

void free_EzStateManagerImpl_field0x20(EzStateManagerImpl_field0x20* to)
{
    free(to);
}

//This isn't needed for rollback. The game only uses this as per-frame scratch space, it is cleared every frame
void copy_EzStateManagerImpl(EzStateManagerImpl* to, EzStateManagerImpl* from, StateTarget target)
{
    copy_EzStateManagerImpl_field0x20(to->field0x20, from->field0x20, target);
}

EzStateManagerImpl* init_EzStateManagerImpl()
{
    EzStateManagerImpl* local = (EzStateManagerImpl*)malloc_(sizeof(EzStateManagerImpl));

    local->field0x20 = init_EzStateManagerImpl_field0x20();

    return local;
}
void free_EzStateManagerImpl(EzStateManagerImpl* to)
{
    free_EzStateManagerImpl_field0x20(to->field0x20);
    free(to);
}
