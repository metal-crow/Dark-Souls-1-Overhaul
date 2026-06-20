#pragma once
#ifndef STATE_SERIALIZER_H
#define STATE_SERIALIZER_H

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

/*
 * Single-traversal state serializer shared by the rollback determinism oracle.
 *
 *   - HASH  mode folds canonical bytes into an FNV-1a-64 digest. This is the
 *           cheap, always-on Tier-0 detector: hash the saved state every frame,
 *           tag it with the GGPO frame number, and compare the two instances'
 *           digests for the same confirmed frame. A mismatch is a desync and
 *           tells you the exact frame + subsystem.
 *   - PRINT mode emits a canonical, line-oriented text dump. This is the Tier-1
 *           localizer: diff the two instances' dumps at the offending frame to
 *           find the exact field that differs.
 *
 * Both modes are driven by ONE serialize_X(StateVisitor&, X*) per struct, so a
 * hash mismatch is *guaranteed* to correspond to a textual diff -- the two can
 * never disagree about what "the state" is. Each serialize_X must mirror the
 * matching copy_X, because copy_X defines the canonical rollback state.
 *
 * Pointer rule: raw pointers differ across processes (ASLR) and must NEVER be
 * hashed directly, or they produce a false desync every frame. Use ptr_index()
 * when the pointer indexes a known array (fully canonical) or ptr_flag() when
 * it does not (records only null/non-null).
 */
class StateVisitor
{
public:
    enum class Mode { Hash, Print };

    explicit StateVisitor(Mode m) : mode(m), _hash(FNV_OFFSET), _depth(0) {}

    // ---- scalars -------------------------------------------------------
    void field(const char* n, bool     v) { uint8_t b = v ? 1 : 0; emit_int(n, 'b', &b, 1, (uint64_t)b, false); }
    void field(const char* n, int8_t   v) { emit_int(n, 'i', &v, 1, (uint64_t)(int64_t)v, true); }
    void field(const char* n, uint8_t  v) { emit_int(n, 'u', &v, 1, (uint64_t)v, false); }
    void field(const char* n, int16_t  v) { emit_int(n, 'i', &v, 2, (uint64_t)(int64_t)v, true); }
    void field(const char* n, uint16_t v) { emit_int(n, 'u', &v, 2, (uint64_t)v, false); }
    void field(const char* n, int32_t  v) { emit_int(n, 'i', &v, 4, (uint64_t)(int64_t)v, true); }
    void field(const char* n, uint32_t v) { emit_int(n, 'u', &v, 4, (uint64_t)v, false); }
    void field(const char* n, int64_t  v) { emit_int(n, 'i', &v, 8, (uint64_t)v, true); }
    void field(const char* n, uint64_t v) { emit_int(n, 'u', &v, 8, v, false); }
    void field(const char* n, float    v) { uint32_t b; memcpy(&b, &v, 4); emit_real(n, &b, 4, (double)v); }
    void field(const char* n, double   v) { uint64_t b; memcpy(&b, &v, 8); emit_real(n, &b, 8, v); }

    // Opaque non-pointer blob (the "data_x" fields whose internal layout is
    // unknown). Hashed byte-for-byte; printed as hex.
    void blob(const char* n, const void* p, size_t len)
    {
        tag('B'); fold(&len, sizeof(len)); fold(p, len);
        if (mode == Mode::Print)
        {
            line_begin(n);
            const uint8_t* b = (const uint8_t*)p;
            char tmp[4];
            for (size_t i = 0; i < len; i++) { snprintf(tmp, sizeof(tmp), "%02x", b[i]); _out += tmp; }
            _out += "\n";
        }
    }

    // Pointer that indexes a known array -> canonical element index.
    void ptr_index(const char* n, const void* p, const void* base, size_t stride)
    {
        uint64_t idx = NULL_INDEX;
        if (p && base && p >= base)
            idx = (uint64_t)(((const uint8_t*)p - (const uint8_t*)base) / stride);
        tag('P'); fold(&idx, sizeof(idx));
        if (mode == Mode::Print)
        {
            line_begin(n);
            if (idx == NULL_INDEX) _out += "null\n";
            else { _out += "#"; _out += std::to_string(idx); _out += "\n"; }
        }
    }

    // Pointer we can't canonicalize -> determinism-safe null/non-null only.
    void ptr_flag(const char* n, const void* p)
    {
        uint8_t f = p ? 1 : 0;
        tag('p'); fold(&f, 1);
        if (mode == Mode::Print) { line_begin(n); _out += (f ? "set\n" : "null\n"); }
    }

    // ---- structure -----------------------------------------------------
    void begin(const char* n)
    {
        tag('{');
        if (mode == Mode::Print) { indent(); _out += n; _out += "\n"; _depth++; }
    }
    void end()
    {
        tag('}');
        if (mode == Mode::Print && _depth > 0) _depth--;
    }
    // List length. Folded into the hash so a structural change (e.g. an element
    // appearing/disappearing) can't be masked by an offsetting value change.
    void count(const char* n, size_t c)
    {
        tag('#'); fold(&c, sizeof(c));
        if (mode == Mode::Print) { line_begin(n); _out += "count="; _out += std::to_string(c); _out += "\n"; }
    }

    uint64_t digest() const { return _hash; }
    const std::string& text() const { return _out; }

    Mode mode;

private:
    static const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    static const uint64_t FNV_PRIME  = 1099511628211ULL;
    static const uint64_t NULL_INDEX = ~0ULL;

    void fold(const void* p, size_t n) { const uint8_t* b = (const uint8_t*)p; for (size_t i = 0; i < n; i++) { _hash ^= b[i]; _hash *= FNV_PRIME; } }
    void tag(char t) { uint8_t b = (uint8_t)t; _hash ^= b; _hash *= FNV_PRIME; }
    void indent() { for (int i = 0; i < _depth; i++) _out += "  "; }
    void line_begin(const char* n) { indent(); _out += n; _out += ":"; }

    void emit_int(const char* n, char t, const void* p, size_t len, uint64_t pretty, bool sgn)
    {
        tag(t); fold(p, len);
        if (mode == Mode::Print) { line_begin(n); _out += (sgn ? std::to_string((int64_t)pretty) : std::to_string(pretty)); _out += "\n"; }
    }
    void emit_real(const char* n, const void* bits, size_t len, double pretty)
    {
        tag('f'); fold(bits, len);   // hash the raw IEEE-754 bits, not the text
        if (mode == Mode::Print)
        {
            line_begin(n);
            char tmp[40];
            snprintf(tmp, sizeof(tmp), "%.9g", pretty);
            _out += tmp; _out += "\n";
        }
    }

    uint64_t _hash;
    std::string _out;
    int _depth;
};

#endif // STATE_SERIALIZER_H
