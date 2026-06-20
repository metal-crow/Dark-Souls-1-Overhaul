#include "DmgHitRecordManImpStructFunctions.h"
#include "StateSerializer.h"

static const size_t max_preallocated_DmgHitRecordManImp_field0x10Elem = 96;
static const size_t max_preallocated_DmgHitRecordManImp_field0x28Elem = 96;

void copy_DmgHitRecordManImp(DmgHitRecordManImp* to, DmgHitRecordManImp* from, StateTarget target)
{
    uint64_t offset0x10 = ((uint64_t)from->field0x10_array) - ((uint64_t)from->field0x10_array_start);
    to->field0x10_array = (DmgHitRecordManImp_field0x10Elem*)(((uint64_t)to->field0x10_array_start) + offset0x10);
    for (size_t i = 0; i < max_preallocated_DmgHitRecordManImp_field0x10Elem; i++)
    {
        copy_DmgHitRecordManImp_field0x10Elem(&to->field0x10_array_start[i], &from->field0x10_array_start[i], target);

        if (from->field0x10_array_start[i].attached != NULL)
        {
            uint64_t attached_offset = ((uint64)from->field0x10_array_start[i].attached) - ((uint64)from->field0x28_array_start);
            to->field0x10_array_start[i].attached = (DmgHitRecordManImp_field0x28Elem*)(((uint64)to->field0x28_array_start) + attached_offset);
        }
        else
        {
            to->field0x10_array_start[i].attached = NULL;
        }

        if (from->field0x10_array_start[i].next != NULL)
        {
            uint64_t next_offset = ((uint64)from->field0x10_array_start[i].next) - ((uint64)from->field0x10_array_start);
            to->field0x10_array_start[i].next = (DmgHitRecordManImp_field0x10Elem*)(((uint64)to->field0x10_array_start) + next_offset);
        }
        else
        {
            to->field0x10_array_start[i].next = NULL;
        }
    }

    uint64_t offset0x28 = ((uint64_t)from->field0x28_array_start) - ((uint64_t)from->field0x28_array);
    to->field0x28_array = (DmgHitRecordManImp_field0x28Elem*)(((uint64_t)to->field0x28_array_start) + offset0x28);
    for (size_t i = 0; i < max_preallocated_DmgHitRecordManImp_field0x28Elem; i++)
    {
        copy_DmgHitRecordManImp_field0x28Elem(&to->field0x28_array_start[i], &from->field0x28_array_start[i], target);

        if (from->field0x28_array_start[i].next != NULL)
        {
            uint64_t next_offset = ((uint64)from->field0x28_array_start[i].next) - ((uint64)from->field0x28_array_start);
            to->field0x28_array_start[i].next = (DmgHitRecordManImp_field0x28Elem*)(((uint64)to->field0x28_array_start) + next_offset);
        }
        else
        {
            to->field0x28_array_start[i].next = NULL;
        }
    }

    to->field0x10_array_len = from->field0x10_array_len;
    to->data_0 = from->data_0;
    to->field0x28_array_len = from->field0x28_array_len;
    to->data_1 = from->data_1;
}

DmgHitRecordManImp* init_DmgHitRecordManImp()
{
    DmgHitRecordManImp* local = (DmgHitRecordManImp*)malloc_(sizeof(DmgHitRecordManImp));

    local->field0x10_array_start = (DmgHitRecordManImp_field0x10Elem*)malloc_(sizeof(DmgHitRecordManImp_field0x10Elem) * max_preallocated_DmgHitRecordManImp_field0x10Elem);
    local->field0x28_array_start = (DmgHitRecordManImp_field0x28Elem*)malloc_(sizeof(DmgHitRecordManImp_field0x28Elem) * max_preallocated_DmgHitRecordManImp_field0x28Elem);

    return local;
}

void free_DmgHitRecordManImp(DmgHitRecordManImp* to)
{
    free(to->field0x10_array_start);
    free(to->field0x28_array_start);
    free(to);
}

void copy_DmgHitRecordManImp_field0x10Elem(DmgHitRecordManImp_field0x10Elem* to, DmgHitRecordManImp_field0x10Elem* from, StateTarget target)
{
    to->unk_8 = from->unk_8;
    to->unk_c = from->unk_c;
    to->unk_18 = from->unk_18;
    to->unk_1c = from->unk_1c;
}

void copy_DmgHitRecordManImp_field0x28Elem(DmgHitRecordManImp_field0x28Elem* to, DmgHitRecordManImp_field0x28Elem* from, StateTarget target)
{
    to->targetHandle = from->targetHandle;
    to->unk_4 = from->unk_4;
    to->unk_10 = from->unk_10;
    to->unk_14 = from->unk_14;
}

// ---- serializer (mirrors copy_DmgHitRecordManImp; drives print_ and hash_) --
// The two fixed 96-elem arrays are used as intrusive linked lists. copy_X
// recomputes attached/next as offsets within the snapshot's own arrays; here we
// canonicalize those offsets to element indices (ptr_index). Captures exactly
// the state copy_X preserves (scalars + the two intra-array link pointers).

static void serialize_DmgHitRecordManImp_field0x10Elem(StateVisitor& v, DmgHitRecordManImp_field0x10Elem* e,
                                                       const void* base0x10, const void* base0x28)
{
    v.begin("field0x10Elem");
    v.ptr_index("attached", e->attached, base0x28, sizeof(DmgHitRecordManImp_field0x28Elem));
    v.field("unk_8", e->unk_8);
    v.field("unk_c", e->unk_c);
    v.ptr_index("next", e->next, base0x10, sizeof(DmgHitRecordManImp_field0x10Elem));
    v.field("unk_18", e->unk_18);
    v.field("unk_1c", e->unk_1c);
    v.end();
}

static void serialize_DmgHitRecordManImp_field0x28Elem(StateVisitor& v, DmgHitRecordManImp_field0x28Elem* e,
                                                       const void* base0x28)
{
    v.begin("field0x28Elem");
    v.field("targetHandle", e->targetHandle);
    v.field("unk_4", e->unk_4);
    v.ptr_index("next", e->next, base0x28, sizeof(DmgHitRecordManImp_field0x28Elem));
    v.field("unk_10", e->unk_10);
    v.field("unk_14", e->unk_14);
    v.end();
}

void serialize_DmgHitRecordManImp(StateVisitor& v, DmgHitRecordManImp* d)
{
    v.begin("DmgHitRecordManImp");

    // Head pointers, captured as signed byte offsets from their array starts so
    // the representation is deterministic regardless of copy_X's offset math.
    v.field("field0x10_head_off", (int64_t)((const uint8_t*)d->field0x10_array - (const uint8_t*)d->field0x10_array_start));
    v.field("field0x28_head_off", (int64_t)((const uint8_t*)d->field0x28_array - (const uint8_t*)d->field0x28_array_start));

    v.count("field0x10_array", max_preallocated_DmgHitRecordManImp_field0x10Elem);
    for (size_t i = 0; i < max_preallocated_DmgHitRecordManImp_field0x10Elem; i++)
    {
        serialize_DmgHitRecordManImp_field0x10Elem(v, &d->field0x10_array_start[i],
                                                   d->field0x10_array_start, d->field0x28_array_start);
    }

    v.count("field0x28_array", max_preallocated_DmgHitRecordManImp_field0x28Elem);
    for (size_t i = 0; i < max_preallocated_DmgHitRecordManImp_field0x28Elem; i++)
    {
        serialize_DmgHitRecordManImp_field0x28Elem(v, &d->field0x28_array_start[i], d->field0x28_array_start);
    }

    v.field("field0x10_array_len", d->field0x10_array_len);
    v.field("data_0", d->data_0);
    v.field("field0x28_array_len", d->field0x28_array_len);
    v.field("data_1", d->data_1);

    v.end();
}

std::string print_DmgHitRecordManImp(DmgHitRecordManImp* d)
{
    StateVisitor v(StateVisitor::Mode::Print);
    serialize_DmgHitRecordManImp(v, d);
    return v.text();
}

uint64_t hash_DmgHitRecordManImp(DmgHitRecordManImp* d)
{
    StateVisitor v(StateVisitor::Mode::Hash);
    serialize_DmgHitRecordManImp(v, d);
    return v.digest();
}
