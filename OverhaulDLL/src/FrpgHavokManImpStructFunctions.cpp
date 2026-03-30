#include "FrpgHavokManImpStructFunctions.h"
#include <unordered_set>

void copy_FrpgHavokManImp(FrpgHavokManImp* to, const FrpgHavokManImp* from, StateTarget target)
{
    copy_FrpgPhysWorld(to->physWorld, from->physWorld, target);
}

//all the Havok Man inits are only for dll memory, game side they always exist
FrpgHavokManImp* init_FrpgHavokManImp()
{
    FrpgHavokManImp* local = (FrpgHavokManImp*)malloc_(sizeof(FrpgHavokManImp));

    local->physWorld = init_FrpgPhysWorld();

    return local;
}

void free_FrpgHavokManImp(FrpgHavokManImp* to)
{
    free_FrpgPhysWorld(to->physWorld);
    free(to);
}

void copy_FrpgPhysWorld(FrpgPhysWorld* to, const FrpgPhysWorld* from, StateTarget target)
{
    copy_hkpWorld(to->_hkpWorld, from->_hkpWorld, target);
}

FrpgPhysWorld* init_FrpgPhysWorld()
{
    FrpgPhysWorld* local = (FrpgPhysWorld*)malloc_(sizeof(FrpgPhysWorld));

    //local->_hkpWorld = init_hkpWorld();

    return local;
}

void free_FrpgPhysWorld(FrpgPhysWorld* to)
{
    //free_hkpWorld(to->_hkpWorld);
    free(to);
}

void copy_hkpWorld(hkpWorld* to, const hkpWorld* from, StateTarget target)
{
    
}
