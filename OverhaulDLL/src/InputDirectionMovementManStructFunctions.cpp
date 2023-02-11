#include "InputDirectionMovementManStructFunctions.h"
#include "Rollback.h"

void copy_InputDirectionMovementMan(InputDirectionMovementMan* to, InputDirectionMovementMan* from)
{
    memcpy(to->data_0, from->data_0, sizeof(to->data_0));
    for (size_t i = 0; i < sizeof(to->data_1) / sizeof(to->data_1[1]); i++)
    {
        *to->data_1[i] = *from->data_1[i];
    }
}

InputDirectionMovementMan* init_InputDirectionMovementMan()
{
    InputDirectionMovementMan* local_InputDirectionMovementMan = (InputDirectionMovementMan*)malloc_(sizeof(InputDirectionMovementMan));

    for (size_t i = 0; i < sizeof(local_InputDirectionMovementMan->data_1) / sizeof(local_InputDirectionMovementMan->data_1[1]); i++)
    {
        local_InputDirectionMovementMan->data_1[i] = (float*)malloc_(sizeof(float));
    }

    return local_InputDirectionMovementMan;

}

void free_InputDirectionMovementMan(InputDirectionMovementMan* to)
{
    for (size_t i = 0; i < sizeof(to->data_1) / sizeof(to->data_1[1]); i++)
    {
        free(to->data_1[i]);
    }

    free(to);
}
