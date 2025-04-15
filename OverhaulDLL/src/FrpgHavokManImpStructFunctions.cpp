#include "FrpgHavokManImpStructFunctions.h"
#include "Rollback.h"

/* ---------------- GENERAL + DAMAGE MAN ------------------ */

std::string print_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to)
{
    std::string out = "hkpSimpleShapePhantom\n";

    out += print_hkpSimpleShapePhantom_field0x30(to->field0x30);

    return out;
}

void copy_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to, const hkpSimpleShapePhantom* from)
{
    copy_hkpSimpleShapePhantom_field0x30(to->field0x30, from->field0x30);
}

hkpSimpleShapePhantom* init_hkpSimpleShapePhantom()
{
    hkpSimpleShapePhantom* local_hkpSimpleShapePhantom = (hkpSimpleShapePhantom*)malloc_(sizeof(hkpSimpleShapePhantom));

    local_hkpSimpleShapePhantom->field0x30 = init_hkpSimpleShapePhantom_field0x30();

    return local_hkpSimpleShapePhantom;
}

void free_hkpSimpleShapePhantom(hkpSimpleShapePhantom* to)
{
    free_hkpSimpleShapePhantom_field0x30(to->field0x30);
    free(to);
}


std::string print_hkpSimpleShapePhantom_field0x30(hkpSimpleShapePhantom_field0x30* to)
{
    std::string out = "hkpSimpleShapePhantom_field0x30\n";

    out += "Unknown position:";
    for (size_t i = 0; i < sizeof(to->position) / 4; i++)
    {
        out += std::to_string(to->position[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_hkpSimpleShapePhantom_field0x30(hkpSimpleShapePhantom_field0x30* to, const hkpSimpleShapePhantom_field0x30* from)
{
    memcpy(to->position, from->position, sizeof(to->position));
}

hkpSimpleShapePhantom_field0x30* init_hkpSimpleShapePhantom_field0x30()
{
    hkpSimpleShapePhantom_field0x30* local_hkpSimpleShapePhantom_field0x30 = (hkpSimpleShapePhantom_field0x30*)malloc_(sizeof(hkpSimpleShapePhantom_field0x30));

    return local_hkpSimpleShapePhantom_field0x30;
}

void free_hkpSimpleShapePhantom_field0x30(hkpSimpleShapePhantom_field0x30* to)
{
    free(to);
}


void copy_hkpSphereShape(hkpSphereShape** to, hkpSphereShape** from, bool to_game)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_hkpSphereShape(to_game);
    }
    if (*to != NULL && *from == NULL)
    {
        free_hkpSphereShape(*to, to_game);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        memcpy((*to)->data_0, (*from)->data_0, sizeof((*to)->data_0));
    }
}

hkpSphereShape* init_hkpSphereShape(bool to_game)
{
    hkpSphereShape* local;
    if (to_game)
    {
        local = (hkpSphereShape*)Game::thread_malloc(sizeof(hkpSphereShape));
    }
    else
    {
        local = (hkpSphereShape*)malloc_(sizeof(hkpSphereShape));
    }
    return local;
}

void free_hkpSphereShape(hkpSphereShape* to, bool to_game)
{
    if (to_game)
    {
        Game::thread_free(to, sizeof(hkpSphereShape));
    }
    else
    {
        free(to);
    }
}


void copy_hkpCapsuleShape(hkpCapsuleShape** to, hkpCapsuleShape** from, bool to_game)
{
    if (*to == NULL && *from != NULL)
    {
        *to = init_hkpCapsuleShape(to_game);
    }
    if (*to != NULL && *from == NULL)
    {
        free_hkpCapsuleShape(*to, to_game);
        *to = NULL;
    }
    if (*to != NULL && *from != NULL)
    {
        memcpy((*to)->data_0, (*from)->data_0, sizeof((*to)->data_0));
    }
}

hkpCapsuleShape* init_hkpCapsuleShape(bool to_game)
{
    hkpCapsuleShape* local;
    if (to_game)
    {
        local = (hkpCapsuleShape*)Game::thread_malloc(sizeof(hkpCapsuleShape));
    }
    else
    {
        local = (hkpCapsuleShape*)malloc_(sizeof(hkpCapsuleShape));
    }
    return local;
}

void free_hkpCapsuleShape(hkpCapsuleShape* to, bool to_game)
{
    if (to_game)
    {
        Game::thread_free(to, sizeof(hkpCapsuleShape));
    }
    else
    {
        free(to);
    }
}


/* ---------------- CHRCTRL ------------------ */

std::string print_hkpCharacterProxy(hkpCharacterProxy* to)
{
    std::string out = "hkpCharacterProxy\n";

    out += "data_0: " + std::to_string(to->data_0) + "\n";

    out += "Unknown data_1:";
    for (size_t i = 0; i < sizeof(to->data_1); i++)
    {
        out += std::to_string(to->data_1[i]);
        out += " ";
    }
    out += "\n";

    out += print_hkpSimpleShapePhantom(to->HkpSimpleShapePhantom);

    out += "Unknown data_2:";
    for (size_t i = 0; i < sizeof(to->data_2); i++)
    {
        out += std::to_string(to->data_2[i]);
        out += " ";
    }
    out += "\n";

    out += "Unknown data_3:";
    for (size_t i = 0; i < sizeof(to->data_3); i++)
    {
        out += std::to_string(to->data_3[i]);
        out += " ";
    }
    out += "\n";

    return out;
}

void copy_hkpCharacterProxy(hkpCharacterProxy* to, const hkpCharacterProxy* from)
{
    to->data_0 = from->data_0;
    memcpy(to->data_1, from->data_1, sizeof(to->data_1));
    copy_hkpSimpleShapePhantom(to->HkpSimpleShapePhantom, from->HkpSimpleShapePhantom);
    memcpy(to->data_2, from->data_2, sizeof(to->data_2));
    memcpy(to->data_3, from->data_3, sizeof(to->data_3));
}

hkpCharacterProxy* init_hkpCharacterProxy()
{
    hkpCharacterProxy* local_hkpCharacterProxy = (hkpCharacterProxy*)malloc_(sizeof(hkpCharacterProxy));

    local_hkpCharacterProxy->HkpSimpleShapePhantom = init_hkpSimpleShapePhantom();

    return local_hkpCharacterProxy;
}

void free_hkpCharacterProxy(hkpCharacterProxy* to)
{
    free_hkpSimpleShapePhantom(to->HkpSimpleShapePhantom);
    free(to);
}
