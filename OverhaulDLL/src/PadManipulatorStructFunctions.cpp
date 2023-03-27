#include "PadManipulatorStructFunctions.h"
#include "Rollback.h"

void copy_PadManipulator(PadManipulator* to, PadManipulator* from)
{
    copy_ChrManipulator(&to->chrManipulator, &from->chrManipulator);
    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
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

void copy_ChrManipulator(ChrManipulator* to, ChrManipulator* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}
