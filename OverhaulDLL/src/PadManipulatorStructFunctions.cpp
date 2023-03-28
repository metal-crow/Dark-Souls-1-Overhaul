#include "PadManipulatorStructFunctions.h"
#include "Rollback.h"

void PadManipulator_to_PadManipulatorPacked(PadManipulatorPacked* to, PadManipulator* from)
{
    memcpy(to->data_0, from->chrManipulator.data_0, sizeof(from->chrManipulator.data_0));
    to->data_0a = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(from->data_1));
}

void PadManipulatorPacked_to_PadManipulator(PadManipulator* to, PadManipulatorPacked* from)
{
    memcpy(to->chrManipulator.data_0, from->data_0, sizeof(from->data_0));
    to->data_0 = from->data_0a;
    memcpy(to->data_1, from->data_1, sizeof(from->data_1));
}

void copy_PadManipulator(PadManipulator* to, PadManipulator* from)
{
    copy_ChrManipulator(&to->chrManipulator, &from->chrManipulator);
    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
}

void copy_ChrManipulator(ChrManipulator* to, ChrManipulator* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
}
