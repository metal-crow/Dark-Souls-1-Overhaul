#include "DmgHitRecordManImpStructFunctions.h"
#include "StateSerializer.h"

static const size_t max_preallocated_DmgHitRecordManImp_field0x10Elem = 96;
static const size_t max_preallocated_DmgHitRecordManImp_field0x28Elem = 96;

//Every list in DmgHitRecordManImp threads through its two 96-record pools, which live as long as the manager, so a link is copied as its
//offset into its pool. When a pool is empty the game heap-allocates a record instead (DmgHitRecordManImp_Retrieve_Free_0x10Slot/_0x28Slot)
//and counts it in field0x10_heap_count/field0x28_heap_count. Rollback does not restore heap records, so it stops rather than dropping one.
template <typename T>
static T* copy_DmgHitRecord_link(T* ptr, const T* from_start, T* to_start, size_t count, const char* what)
{
    if (ptr == NULL)
    {
        return NULL;
    }
    const ptrdiff_t offset = (const char*)ptr - (const char*)from_start;
    if (offset < 0 || offset >= (ptrdiff_t)(count * sizeof(T)) || offset % (ptrdiff_t)sizeof(T) != 0)
    {
        FATALERROR("copy_DmgHitRecordManImp: %s %p is not a record in its pool", what, ptr);
    }
    return (T*)((char*)to_start + offset);
}

void copy_DmgHitRecordManImp(DmgHitRecordManImp* to, DmgHitRecordManImp* from, StateTarget target)
{
    if (from->field0x10_heap_count != 0 || from->field0x28_heap_count != 0)
    {
        FATALERROR("copy_DmgHitRecordManImp: %u field0x10 and %u field0x28 hit records are heap-allocated because a pool ran out, and rollback cannot restore them",
            from->field0x10_heap_count, from->field0x28_heap_count);
    }
    const size_t n10 = max_preallocated_DmgHitRecordManImp_field0x10Elem;
    const size_t n28 = max_preallocated_DmgHitRecordManImp_field0x28Elem;

    to->field0x10_array = copy_DmgHitRecord_link(from->field0x10_array, from->field0x10_array_start, to->field0x10_array_start, n10, "field0x10 free list head");
    to->timed_list = copy_DmgHitRecord_link(from->timed_list, from->field0x10_array_start, to->field0x10_array_start, n10, "timed list head");
    for (size_t i = 0; i < n10; i++)
    {
        DmgHitRecordManImp_field0x10Elem* f = &from->field0x10_array_start[i];
        DmgHitRecordManImp_field0x10Elem* t = &to->field0x10_array_start[i];
        copy_DmgHitRecordManImp_field0x10Elem(t, f, target);
        t->attached = copy_DmgHitRecord_link(f->attached, from->field0x28_array_start, to->field0x28_array_start, n28, "field0x10 record attached");
        t->next = copy_DmgHitRecord_link(f->next, from->field0x10_array_start, to->field0x10_array_start, n10, "field0x10 record next");
    }

    to->field0x28_array = copy_DmgHitRecord_link(from->field0x28_array, from->field0x28_array_start, to->field0x28_array_start, n28, "field0x28 free list head");
    for (size_t i = 0; i < n28; i++)
    {
        DmgHitRecordManImp_field0x28Elem* f = &from->field0x28_array_start[i];
        DmgHitRecordManImp_field0x28Elem* t = &to->field0x28_array_start[i];
        copy_DmgHitRecordManImp_field0x28Elem(t, f, target);
        t->next = copy_DmgHitRecord_link(f->next, from->field0x28_array_start, to->field0x28_array_start, n28, "field0x28 record next");
    }

    to->field0x10_array_len = from->field0x10_array_len;
    to->field0x10_heap_count = from->field0x10_heap_count;
    to->field0x28_array_len = from->field0x28_array_len;
    to->field0x28_heap_count = from->field0x28_heap_count;
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
    v.padding("unk_c", &e->unk_c, sizeof(e->unk_c));     // alignment before next @0x10
    v.ptr_index("next", e->next, base0x10, sizeof(DmgHitRecordManImp_field0x10Elem));
    v.field("unk_18", e->unk_18);
    v.padding("unk_1c", &e->unk_1c, sizeof(e->unk_1c));  // trailing alignment to 0x20
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
    v.padding("unk_14", &e->unk_14, sizeof(e->unk_14));  // trailing alignment to 0x18
    v.end();
}

void serialize_DmgHitRecordManImp(StateVisitor& v, DmgHitRecordManImp* d)
{
    v.begin("DmgHitRecordManImp");

    // Head pointers, captured as signed byte offsets from their array starts so
    // the representation is deterministic regardless of copy_X's offset math.
    // List heads, as record indices into their pools
    v.ptr_index("field0x10_head", d->field0x10_array, d->field0x10_array_start, sizeof(DmgHitRecordManImp_field0x10Elem));
    v.ptr_index("timed_list", d->timed_list, d->field0x10_array_start, sizeof(DmgHitRecordManImp_field0x10Elem));
    v.ptr_index("field0x28_head", d->field0x28_array, d->field0x28_array_start, sizeof(DmgHitRecordManImp_field0x28Elem));

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
    v.field("field0x10_heap_count", d->field0x10_heap_count);
    v.field("field0x28_array_len", d->field0x28_array_len);
    v.field("field0x28_heap_count", d->field0x28_heap_count);

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
