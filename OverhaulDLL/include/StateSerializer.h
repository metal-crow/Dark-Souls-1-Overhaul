#pragma once
#ifndef STATE_SERIALIZER_H
#define STATE_SERIALIZER_H

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <initializer_list>
#include <type_traits>

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
 *
 * WRITE / APPLY modes (session-start state handshake, RollbackStartSync): WRITE emits the same
 * canonical values as a compact byte stream (no padding, pointers as null/non-null flags);
 * APPLY walks the RECEIVER's saved-state tree with the same serialize_X and writes the stream's
 * values into it. Pointers, padding, lengths and list counts are never written -- they are only
 * checked, so the receiver keeps its own heap objects. Anything APPLY could not write shows up
 * when the receiver re-WRITEs the patched tree and compares it with the stream it was sent.
 * Every op carries a tag, a hash of its name and its payload length, so the moment the two
 * traversals diverge (a different list count, a null sub-struct) APPLY notices, skips the rest of
 * that struct in both the stream and the traversal, and carries on after it -- it never writes
 * misaligned bytes. A serializer can instead rebuild the receiver's side to the sender's shape
 * with applying() / apply_count() / apply_value() (lists of different length, looked-up pointers).
 *
 * Padding rule: alignment gaps and trailing padding are never written by the
 * game, so they hold whatever the allocator handed out -- per-process heap
 * garbage. Hashing them produces a false desync on frame 0 of every run, on
 * every subsystem that has them (this is exactly what the 2026-09-07 in-game
 * run hit). Use padding() for those bytes: copy_X still memcpys them, we just
 * refuse to *compare* them.
 */

// Set to 1 to fold padding bytes back into the digest. Useful to double-check a
// field marked padding that you suspect is actually live state -- a field that
// is genuinely live will still agree across instances when they are in sync.
#ifndef STATEHASH_HASH_PADDING
#define STATEHASH_HASH_PADDING 0
#endif
class StateVisitor
{
public:
    enum class Mode { Hash, Print, Write, Apply };

    explicit StateVisitor(Mode m) : mode(m), _hash(FNV_OFFSET), _depth(0) {}

    // APPLY mode: `stream` is what a WRITE produced on the other machine. Not owned.
    StateVisitor(Mode m, const uint8_t* stream, size_t len) : mode(m), _hash(FNV_OFFSET), _depth(0), _in(stream), _in_len(len) {}

    // ---- scalars -------------------------------------------------------
    // By reference so APPLY can write the value back. A call that passes a temporary (a cast,
    // a computed value) is simply not applied; the post-apply comparison names it.
    void field(const char* n, const bool&     v) { uint8_t b = v ? 1 : 0; emit_int(n, 'b', &b, 1, (uint64_t)b, false); apply_bool(n, v); }
    void field(const char* n, const int8_t&   v) { emit_int(n, 'i', &v, 1, (uint64_t)(int64_t)v, true); apply_raw(n, 'i', &v, 1); }
    void field(const char* n, const uint8_t&  v) { emit_int(n, 'u', &v, 1, (uint64_t)v, false); apply_raw(n, 'u', &v, 1); }
    void field(const char* n, const int16_t&  v) { emit_int(n, 'i', &v, 2, (uint64_t)(int64_t)v, true); apply_raw(n, 'i', &v, 2); }
    void field(const char* n, const uint16_t& v) { emit_int(n, 'u', &v, 2, (uint64_t)v, false); apply_raw(n, 'u', &v, 2); }
    void field(const char* n, const int32_t&  v) { emit_int(n, 'i', &v, 4, (uint64_t)(int64_t)v, true); apply_raw(n, 'i', &v, 4); }
    void field(const char* n, const uint32_t& v) { emit_int(n, 'u', &v, 4, (uint64_t)v, false); apply_raw(n, 'u', &v, 4); }
    void field(const char* n, const int64_t&  v) { emit_int(n, 'i', &v, 8, (uint64_t)v, true); apply_raw(n, 'i', &v, 8); }
    void field(const char* n, const uint64_t& v) { emit_int(n, 'u', &v, 8, v, false); apply_raw(n, 'u', &v, 8); }
    void field(const char* n, const float&    v) { uint32_t b; memcpy(&b, &v, 4); emit_real(n, &b, 4, (double)v); apply_raw(n, 'f', &v, 4); }
    void field(const char* n, const double&   v) { uint64_t b; memcpy(&b, &v, 8); emit_real(n, &b, 8, v); apply_raw(n, 'f', &v, 8); }

    // The length or capacity of a buffer the struct owns. Hashed and printed exactly like field(),
    // but APPLY only checks it: writing another machine's length over this one's buffer could run
    // past what the buffer holds. A difference skips the rest of the enclosing struct.
    template <typename T> void length(const char* n, const T& v)
    {
        static_assert(std::is_integral<T>::value, "length() is for integer lengths");
        const char t = std::is_signed<T>::value ? 'i' : 'u';
        emit_int(n, t, &v, sizeof(T), std::is_signed<T>::value ? (uint64_t)(int64_t)v : (uint64_t)v, std::is_signed<T>::value);
        if (mode == Mode::Apply) check_raw(n, t, &v, sizeof(T), true);
    }

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
        else if (mode == Mode::Write) { op('B', n, 4 + len); uint32_t l = (uint32_t)len; put(&l, sizeof(l)); put(p, len); }
        else if (mode == Mode::Apply)
        {
            if (!expect('B', n)) return;
            uint32_t l = 0;
            if (!take(&l, sizeof(l))) return;
            if (l != len)
            {
                take(NULL, l);
                mismatch(n, "blob length " + std::to_string(l) + " vs " + std::to_string(len), 0);
                return;
            }
            take((void*)p, len);
        }
    }

    // Alignment gap / trailing padding. The length is folded (so a layout change
    // is still caught) but the CONTENTS are not: they are uninitialised bytes
    // and can never match across two processes. Printed without a value so the
    // Tier-1 dump diff stays quiet about them too.
    void padding(const char* n, const void* p, size_t len)
    {
        tag('_'); fold(&len, sizeof(len));
#if STATEHASH_HASH_PADDING
        fold(p, len);
#else
        (void)p;
#endif
        if (mode == Mode::Print)
        {
            line_begin(n);
            _out += "<pad "; _out += std::to_string(len); _out += "B>\n";
        }
    }

    // A blob with holes: `holes` lists byte ranges (offset, length, kind) inside it that are either
    // padding (Pad: never written by the game, per the Ghidra layout -- only the length is folded) or
    // a heap pointer (Ptr: 8 bytes, folded as null/non-null like ptr_flag). Everything else is hashed
    // byte for byte like blob(). The ranges must be sorted and inside the blob. Printed as hex with
    // "__" for padding bytes and "pp" for pointer bytes, so the dump diff stays quiet about them too.
    enum class Hole { Pad, Ptr };
    struct HoleRange { size_t off; size_t len; Hole kind; };
    void blob_holes(const char* n, const void* p, size_t len, std::initializer_list<HoleRange> holes)
    {
        const uint8_t* b = (const uint8_t*)p;
        tag('H'); fold(&len, sizeof(len));
        if (mode == Mode::Write)
        {
            // payload: length, then the plain bytes and one flag byte per pointer hole, in order
            size_t payload = 4, at = 0;
            for (const HoleRange& h : holes)
            {
                size_t off = h.off < len ? h.off : len;
                if (off > at) payload += off - at;
                size_t hl = h.len;
                if (h.off + hl > len) hl = h.off < len ? len - h.off : 0;
                if (h.kind == Hole::Ptr) payload += 1;
                at = h.off + hl;
            }
            if (len > at) payload += len - at;
            op('H', n, payload);
            uint32_t l = (uint32_t)len; put(&l, sizeof(l));
        }
        if (mode == Mode::Apply)
        {
            if (!expect('H', n)) return;
            uint32_t l = 0;
            if (!take(&l, sizeof(l))) return;
            if (l != len)
            {
                take(NULL, _cur_payload_left);
                mismatch(n, "blob length " + std::to_string(l) + " vs " + std::to_string(len), 0);
                return;
            }
        }
        size_t at = 0;
        std::string hex;
        char tmp[4];
        auto plain = [&](size_t upto)
        {
            if (upto > len) upto = len;
            if (upto > at)
            {
                fold(b + at, upto - at);
                if (mode == Mode::Write) put(b + at, upto - at);
                else if (mode == Mode::Apply) take((void*)(b + at), upto - at);
            }
            if (mode == Mode::Print) for (size_t i = at; i < upto; i++) { snprintf(tmp, sizeof(tmp), "%02x", b[i]); hex += tmp; }
            if (upto > at) at = upto;
        };
        for (const HoleRange& h : holes)
        {
            plain(h.off);
            size_t hl = h.len;
            if (h.off + hl > len) hl = h.off < len ? len - h.off : 0;
            if (h.kind == Hole::Ptr)
            {
                uint64_t v = 0;
                memcpy(&v, b + h.off, hl < 8 ? hl : 8);
                uint8_t f = v ? 1 : 0;
                tag('p'); fold(&f, 1);
                if (mode == Mode::Write) put(&f, 1);
                else if (mode == Mode::Apply) check_flag(n, f);
                if (mode == Mode::Print) for (size_t i = 0; i < hl; i++) hex += "pp";
            }
            else
            {
                tag('_'); fold(&hl, sizeof(hl));
#if STATEHASH_HASH_PADDING
                fold(b + h.off, hl);
#endif
                if (mode == Mode::Print) for (size_t i = 0; i < hl; i++) hex += "__";
            }
            at = h.off + hl;
        }
        plain(len);
        if (mode == Mode::Print) { line_begin(n); _out += hex; _out += "\n"; }
    }

    // Real, live state that is deliberately OUT OF SCOPE for the comparison, and so is
    // expected to differ between the two instances. Distinct from padding(): padding is
    // bytes the game never writes, this is state the game does write but that we have
    // chosen not to synchronise (yet). Same treatment -- fold the length so a layout
    // change is still caught, but not the contents -- with its own tag and print text so a
    // dump diff says which of the two reasons applies.
    void excluded(const char* n, size_t len)
    {
        tag('x'); fold(&len, sizeof(len));
        if (mode == Mode::Print)
        {
            line_begin(n);
            _out += "<excluded "; _out += std::to_string(len); _out += "B>\n";
        }
    }

    // Print-only annotation. Never folded into the digest, so it can describe state that is deliberately not compared.
    void note(const char* n, const std::string& text)
    {
        if (mode == Mode::Print) { line_begin(n); _out += text; _out += "\n"; }
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
        else if (mode == Mode::Write) { op('P', n, sizeof(idx)); put(&idx, sizeof(idx)); }
        else if (mode == Mode::Apply) check_raw(n, 'P', &idx, sizeof(idx), false);
    }

    // Pointer we can't canonicalize -> determinism-safe null/non-null only.
    void ptr_flag(const char* n, const void* p)
    {
        uint8_t f = p ? 1 : 0;
        tag('p'); fold(&f, 1);
        if (mode == Mode::Print) { line_begin(n); _out += (f ? "set\n" : "null\n"); }
        else if (mode == Mode::Write) { op('p', n, 1); put(&f, 1); }
        else if (mode == Mode::Apply) { if (expect('p', n)) check_flag(n, f); }
    }

    // ---- structure -----------------------------------------------------
    void begin(const char* n)
    {
        tag('{');
        if (mode == Mode::Print) { indent(); _out += n; _out += "\n"; _depth++; }
        else if (mode == Mode::Write) { op('{', n, 0); _path.push_back(n); }
        else if (mode == Mode::Apply)
        {
            expect('{', n);
            _path.push_back(n);
            _depth++;
        }
    }
    void end()
    {
        tag('}');
        if (mode == Mode::Print && _depth > 0) _depth--;
        else if (mode == Mode::Write) { op('}', "}", 0); if (!_path.empty()) _path.pop_back(); }
        else if (mode == Mode::Apply)
        {
            expect('}', "}");
            if (!_path.empty()) _path.pop_back();
            _depth--;
            if (_skip_depth >= 0 && _depth < _skip_depth) _skip_depth = -1;   // back in step with the stream
        }
    }
    // List length. Folded into the hash so a structural change (e.g. an element
    // appearing/disappearing) can't be masked by an offsetting value change.
    void count(const char* n, size_t c)
    {
        tag('#'); fold(&c, sizeof(c));
        if (mode == Mode::Print) { line_begin(n); _out += "count="; _out += std::to_string(c); _out += "\n"; }
        else if (mode == Mode::Write) { op('#', n, 8); uint64_t c64 = c; put(&c64, sizeof(c64)); }
        else if (mode == Mode::Apply) { uint64_t c64 = c; check_raw(n, '#', &c64, sizeof(c64), true); }
    }

    uint64_t digest() const { return _hash; }
    const std::string& text() const { return _out; }

    // ---- custom APPLY support --------------------------------------------
    // For a serializer that rebuilds part of the receiver's saved tree instead of only writing values into it
    // (a list whose length can differ, a pointer that has to be looked up again): true while the stream and
    // the traversal are in step.
    bool applying() const { return mode == Mode::Apply && !_aborted && _skip_depth < 0; }
    // APPLY: read the next op, a count() of name `n`, and return the SENDER's value. The caller then resizes
    // what it traverses to match, so no mismatch is recorded here.
    bool apply_count(const char* n, size_t* theirs)
    {
        if (!applying() || !expect('#', n)) return false;
        uint64_t c64 = 0;
        if (!take(&c64, sizeof(c64))) return false;
        *theirs = (size_t)c64;
        return true;
    }
    // APPLY: read the next op, a field() of name `n` of the same type as *out, into *out (not into the tree)
    template <typename T> bool apply_value(const char* n, T* out)
    {
        static_assert(std::is_arithmetic<T>::value, "apply_value() is for scalars");
        const char t = std::is_floating_point<T>::value ? 'f' : (std::is_same<T, bool>::value ? 'b' : (std::is_signed<T>::value ? 'i' : 'u'));
        if (!applying() || !expect(t, n)) return false;
        return take(out, sizeof(T));
    }
    // APPLY: something a custom apply could not do
    void apply_problem(const char* n, const std::string& why) { _problems.push_back("VALUE " + path_of(n) + ": " + why); }

    // ---- WRITE / APPLY results -------------------------------------------
    const std::vector<uint8_t>& stream() const { return _stream; }
    // WRITE with record_paths(true): the stream offset each op starts at, with its path, for naming a mismatch
    struct OpAt { size_t off; std::string path; };
    const std::vector<OpAt>& ops() const { return _ops; }
    void record_paths(bool on) { _record = on; }
    // APPLY: what could not be applied. POINTER / VALUE problems are listed and APPLY goes on. A STRUCTURE
    // one (a different op, list count or length) skips the rest of the struct it is in, on both sides, and
    // carries on after it; only one outside every struct stops the whole APPLY.
    const std::vector<std::string>& problems() const { return _problems; }
    size_t structure_problems() const { return _structural; }
    bool aborted() const { return _aborted; }
    bool fully_consumed() const { return !_aborted && _pos == _in_len; }

    Mode mode;

private:
    static const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    static const uint64_t FNV_PRIME  = 1099511628211ULL;
    static const uint64_t NULL_INDEX = ~0ULL;
    static const size_t HEADER = 7;   // tag, name hash (2), payload length (4)

    void fold(const void* p, size_t n) { if (mode != Mode::Hash) return; const uint8_t* b = (const uint8_t*)p; for (size_t i = 0; i < n; i++) { _hash ^= b[i]; _hash *= FNV_PRIME; } }
    void tag(char t) { if (mode != Mode::Hash) return; uint8_t b = (uint8_t)t; _hash ^= b; _hash *= FNV_PRIME; }
    void indent() { for (int i = 0; i < _depth; i++) _out += "  "; }
    void line_begin(const char* n) { indent(); _out += n; _out += ":"; }

    void emit_int(const char* n, char t, const void* p, size_t len, uint64_t pretty, bool sgn)
    {
        tag(t); fold(p, len);
        if (mode == Mode::Print) { line_begin(n); _out += (sgn ? std::to_string((int64_t)pretty) : std::to_string(pretty)); _out += "\n"; }
        else if (mode == Mode::Write) { op(t, n, len); put(p, len); }
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
        else if (mode == Mode::Write) { op('f', n, len); put(bits, len); }
    }

    // ---- WRITE / APPLY plumbing -------------------------------------------
    static uint16_t name_hash(const char* n)
    {
        uint32_t h = 2166136261u;
        for (const char* c = n; *c; c++) { h ^= (uint8_t)*c; h *= 16777619u; }
        return (uint16_t)(h ^ (h >> 16));
    }
    std::string path_of(const char* n) const
    {
        std::string s;
        for (const std::string& p : _path) { s += p; s += "/"; }
        return s + n;
    }
    void put(const void* p, size_t n) { const uint8_t* b = (const uint8_t*)p; _stream.insert(_stream.end(), b, b + n); }
    void op(char t, const char* n, size_t payload)
    {
        if (_record) _ops.push_back({ _stream.size(), path_of(n) });
        uint8_t tb = (uint8_t)t; put(&tb, 1);
        uint16_t nh = name_hash(n); put(&nh, 2);
        uint32_t pl = (uint32_t)payload; put(&pl, 4);
    }
    // dst NULL: skip the bytes
    bool take(void* dst, size_t n)
    {
        if (_aborted) return false;
        if (_pos + n > _in_len) { abort_apply("<end>", "stream ran out"); return false; }
        if (dst != NULL) memcpy(dst, _in + _pos, n);
        _pos += n;
        _cur_payload_left = _cur_payload_left > n ? _cur_payload_left - n : 0;
        return true;
    }
    bool read_header(uint8_t* t, uint16_t* nh, uint32_t* pl)
    {
        uint8_t hdr[HEADER];
        if (!take(hdr, HEADER)) return false;
        *t = hdr[0];
        memcpy(nh, hdr + 1, 2);
        memcpy(pl, hdr + 3, 4);
        return true;
    }
    void abort_apply(const char* n, const std::string& why)
    {
        if (_aborted) return;
        _problems.push_back("STRUCTURE " + path_of(n) + ": " + why + " (apply stopped)");
        _structural++;
        _aborted = true;
    }
    // The stream and the traversal disagree about the shape of the struct they are in. `rel` is how deep the
    // stream now is relative to that struct (+1 if it has just opened a sub-struct, -1 if it has just closed
    // this one). Skip the stream to the end of the struct, and the traversal with it (end() leaves the skip).
    void mismatch(const char* n, const std::string& why, int rel)
    {
        _problems.push_back("STRUCTURE " + path_of(n) + ": " + why + " (rest of the struct skipped)");
        _structural++;
        if (_depth <= 0) { abort_apply(n, "outside every struct"); return; }
        while (rel >= 0)
        {
            uint8_t t; uint16_t nh; uint32_t pl;
            if (!read_header(&t, &nh, &pl)) return;
            if (!take(NULL, pl)) return;
            if (t == '{') rel++;
            else if (t == '}') rel--;
        }
        _skip_depth = _depth;
    }
    // Consume one op header and check it is the op this traversal is at
    bool expect(char t, const char* n)
    {
        if (_aborted || _skip_depth >= 0) return false;
        uint8_t st; uint16_t nh; uint32_t pl;
        if (!read_header(&st, &nh, &pl)) return false;
        _cur_payload_left = pl;
        if (st != (uint8_t)t || nh != name_hash(n))
        {
            take(NULL, pl);
            mismatch(n, std::string("stream has op '") + (char)st + "', traversal is at '" + t + "'",
                st == '{' ? 1 : (st == '}' ? -1 : 0));
            return false;
        }
        return true;
    }
    void apply_raw(const char* n, char t, const void* dst, size_t len)
    {
        if (mode != Mode::Apply || !expect(t, n)) return;
        if (_cur_payload_left != len) { take(NULL, _cur_payload_left); mismatch(n, "value size", 0); return; }
        take((void*)dst, len);
    }
    void apply_bool(const char* n, const bool& dst)
    {
        if (mode != Mode::Apply || !expect('b', n)) return;
        uint8_t b = 0;
        if (take(&b, 1)) *(bool*)&dst = (b != 0);
    }
    // APPLY never writes these: report a difference, and skip the struct if it changes the shape of what follows
    void check_raw(const char* n, char t, const void* mine, size_t len, bool shape)
    {
        if (!expect(t, n)) return;
        if (_cur_payload_left != len) { take(NULL, _cur_payload_left); mismatch(n, "value size", 0); return; }
        uint8_t theirs[8] = {};
        if (!take(theirs, len)) return;
        if (memcmp(theirs, mine, len) != 0)
        {
            uint64_t a = 0, b = 0;
            memcpy(&a, theirs, len); memcpy(&b, mine, len);
            if (shape) mismatch(n, "sender " + std::to_string(a) + ", receiver " + std::to_string(b), 0);
            else _problems.push_back("VALUE " + path_of(n) + ": sender " + std::to_string(a) + ", receiver " + std::to_string(b));
        }
    }
    void check_flag(const char* n, uint8_t mine)
    {
        uint8_t theirs = 0;
        if (!take(&theirs, 1)) return;
        if (theirs != mine) _problems.push_back(std::string("POINTER ") + path_of(n) + ": sender " + (theirs ? "set" : "null") + ", receiver " + (mine ? "set" : "null"));
    }

    uint64_t _hash;
    std::string _out;
    int _depth;

    std::vector<uint8_t> _stream;
    std::vector<OpAt> _ops;
    bool _record = false;
    std::vector<std::string> _path;
    const uint8_t* _in = nullptr;
    size_t _in_len = 0;
    size_t _pos = 0;
    size_t _cur_payload_left = 0;
    int _skip_depth = -1;
    bool _aborted = false;
    size_t _structural = 0;
    std::vector<std::string> _problems;
};

#endif // STATE_SERIALIZER_H
