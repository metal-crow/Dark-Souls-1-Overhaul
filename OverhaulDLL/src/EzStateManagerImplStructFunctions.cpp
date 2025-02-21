#include "EzStateManagerImplStructFunctions.h"
#include "Rollback.h"

void copy_EzStateManagerImpl_field0x20(EzStateManagerImpl_field0x20* to, EzStateManagerImpl_field0x20* from, bool to_game)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
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
void copy_EzStateManagerImpl(EzStateManagerImpl* to, EzStateManagerImpl* from, bool to_game)
{
    copy_EzStateManagerImpl_field0x20(to->field0x20, from->field0x20, to_game);
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
