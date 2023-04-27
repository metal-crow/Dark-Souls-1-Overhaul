#include "PadManipulatorStructFunctions.h"
#include "Rollback.h"

void copy_PadManipulator(PadManipulator* to, PadManipulator* from)
{
    memcpy(to, from, sizeof(to));
}

PadManipulator* init_PadManipulator()
{
    PadManipulator* local_PadManipulator = (PadManipulator*)malloc_(sizeof(PadManipulator));

    return local_PadManipulator;
}

void free_PadManipulator(PadManipulator* to)
{
    free(to);
}
