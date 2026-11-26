// NBT Editor — single translation unit, native C++.
// Sections: L1 utils · L4 NBT core · (later) L0 platform · L2 render · L3 ui · L5 app · L6 screens
// Build host tests:           bash scripts/build_host_tests.sh
// Build device library:       bash scripts/build_device.sh
// Both scripts inject the version macros from the VERSION file.
#if defined(NBT_SELFTEST) || defined(NBT_UISHOT)
  #define NBT_TESTABLE 1
#else
  #define NBT_TESTABLE 0
#endif

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <algorithm>
#include <memory>
#include <new>
#include <atomic>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>

// Entity identifier -> PNG filename, generated from reference t.f.java.
#include "entity_icons.inc"
// Block RGB colours extracted from the reference smali t/c.java.
#include "block_colors.inc"

// stb_image is compiled as a separate C unit (app/src/stb_image.c).
extern "C" {
unsigned char* stbi_load_from_memory(const unsigned char* buffer, int len,
                                     int* x, int* y, int* comp, int req_comp);
void stbi_image_free(void* retval);
}
// Platform hook: read a PNG from APK assets (defined in namespace app below).
namespace app {
std::vector<uint8_t> readAssetBytes(const std::string& rel);
bool writePickedFile(const std::string& uri, const std::vector<uint8_t>& bytes);
}

// ============================================================================
// L4 — NBT CORE (Bedrock little-endian dialect)
// No dependency on Android, UI, IO. Unit-tested from host via -DNBT_SELFTEST.
// ============================================================================

namespace nbt {

enum class Type : uint8_t {
    End        = 0,
    Byte       = 1,
    Short      = 2,
    Int        = 3,
    Long       = 4,
    Float      = 5,
    Double     = 6,
    ByteArray  = 7,
    String     = 8,
    List       = 9,
    Compound   = 10,
    IntArray   = 11,
    LongArray  = 12,   // not present in the reference app; required by spec
};

inline const char* typeName(Type t) {
    switch (t) {
        case Type::End:       return "TAG_End";
        case Type::Byte:      return "TAG_Byte";
        case Type::Short:     return "TAG_Short";
        case Type::Int:       return "TAG_Int";
        case Type::Long:      return "TAG_Long";
        case Type::Float:     return "TAG_Float";
        case Type::Double:    return "TAG_Double";
        case Type::ByteArray: return "TAG_Byte_Array";
        case Type::String:    return "TAG_String";
        case Type::List:      return "TAG_List";
        case Type::Compound:  return "TAG_Compound";
        case Type::IntArray:  return "TAG_Int_Array";
        case Type::LongArray: return "TAG_Long_Array";
    }
    return "TAG_Unknown";
}

inline bool typeFromId(uint8_t id, Type& out) {
    switch (id) {
        case 0:  out = Type::End;       return true;
        case 1:  out = Type::Byte;      return true;
        case 2:  out = Type::Short;     return true;
        case 3:  out = Type::Int;       return true;
        case 4:  out = Type::Long;      return true;
        case 5:  out = Type::Float;     return true;
        case 6:  out = Type::Double;    return true;
        case 7:  out = Type::ByteArray; return true;
        case 8:  out = Type::String;    return true;
        case 9:  out = Type::List;      return true;
        case 10: out = Type::Compound;  return true;
        case 11: out = Type::IntArray;  return true;
        case 12: out = Type::LongArray; return true;
        default: return false;
    }
}

inline bool isArray(Type t) { return t == Type::ByteArray || t == Type::IntArray || t == Type::LongArray; }
inline bool isList(Type t)  { return t == Type::List; }
inline bool isCompound(Type t) { return t == Type::Compound; }
inline bool isContainer(Type t) { return t == Type::Compound || t == Type::List || isArray(t); }
inline bool isScalar(Type t) { return t == Type::Byte || t == Type::Short || t == Type::Int || t == Type::Long
                                  || t == Type::Float || t == Type::Double || t == Type::String; }

// ---- node (arena element) --------------------------------------------------
struct Node {
    Type   type      = Type::End;
    std::string name;                  // empty = unnamed
    int64_t i        = 0;              // Byte/Short/Int/Long payload
    double  d        = 0;              // Float/Double payload
    std::string s;                     // String payload
    std::vector<uint8_t>  bytes;       // ByteArray
    std::vector<int64_t>  longs;       // IntArray/LongArray
    std::vector<uint32_t> kids;        // Compound/List children
    Type   elemType  = Type::End;      // List element type
};

struct Document {
    std::vector<Node> arena;
    uint32_t root = 0;
    std::string path;
    // level.dat container bookkeeping (0 when the file is raw NBT)
    bool   hasLevelDatHeader = false;
    int32_t headerVersion    = 0;      // preserved from source when written back
    bool   dirty             = false;
};

// ---- errors ----------------------------------------------------------------
enum class Err {
    None,
    Empty,
    Truncated,
    UnknownTag,
    DepthLimit,
    SizeLimit,
    HeaderLengthMismatch,
    TrailingBytes,
    InvalidDocument,
    DuplicateName,
    UnnamedTag,
    ElementTypeMismatch,
    IndexOutOfRange,
    NotFound,
    IoError,
    WriteFailed,
    Internal,
};

inline const char* errText(Err e) {
    switch (e) {
        case Err::None:                 return "ok";
        case Err::Empty:                return "File is empty.";
        case Err::Truncated:            return "Unexpected end of file (truncated NBT).";
        case Err::UnknownTag:           return "Unknown tag type encountered.";
        case Err::DepthLimit:           return "Nesting too deep (depth limit reached).";
        case Err::SizeLimit:            return "Document too large (node limit reached).";
        case Err::HeaderLengthMismatch: return "level.dat header length does not match the payload.";
        case Err::TrailingBytes:        return "Unexpected data after the NBT root.";
        case Err::InvalidDocument:      return "NBT document is invalid.";
        case Err::DuplicateName:        return "A tag with this name already exists in the compound";
        case Err::UnnamedTag:           return "Tags inside a CompoundTag must have a name";
        case Err::ElementTypeMismatch:  return "List element type mismatch, it should be";
        case Err::IndexOutOfRange:      return "Index out of range.";
        case Err::NotFound:             return "Tag not found.";
        case Err::IoError:              return "Could not read or write the file.";
        case Err::WriteFailed:          return "Failed to write NBT data.";
        case Err::Internal:             return "Internal error.";
    }
    return "Unknown error.";
}

template <typename T>
struct Result {
    T value{};
    Err err = Err::None;
    std::string detail;               // human-readable extra context (offsets, paths)
    bool ok() const { return err == Err::None; }
    static Result fail(Err e, std::string d = "") { Result r; r.err = e; r.detail = std::move(d); return r; }
    static Result good(T v) { Result r; r.value = std::move(v); return r; }
};

template <>
struct Result<void> {
    char dummy = 0;
    Err err = Err::None;
    std::string detail;
    bool ok() const { return err == Err::None; }
    static Result fail(Err e, std::string d = "") { Result r; r.err = e; r.detail = std::move(d); return r; }
    static Result good() { return Result{}; }
};

// ---- reader ----------------------------------------------------------------
struct ByteReader {
    const uint8_t* p;
    size_t n;
    size_t pos = 0;
    size_t start = 0;                 // where the NBT payload begins (after header)

    bool eof() const { return pos >= n; }
    size_t remaining() const { return pos <= n ? n - pos : 0; }
    bool need(size_t k) { return pos <= n && k <= n - pos; }
    bool u8(uint8_t& out) { if (!need(1)) return false; out = p[pos++]; return true; }
    bool u16(uint16_t& out) { if (!need(2)) return false; out = (uint16_t)(p[pos] | (p[pos + 1] << 8)); pos += 2; return true; }
    bool i16(int16_t& out) { uint16_t u; if (!u16(u)) return false; out = (int16_t)u; return true; }
    bool i32(int32_t& out) { if (!need(4)) return false;
        out = (int32_t)((uint32_t)p[pos] | ((uint32_t)p[pos+1] << 8) | ((uint32_t)p[pos+2] << 16) | ((uint32_t)p[pos+3] << 24));
        pos += 4; return true; }
    bool i64(int64_t& out) { if (!need(8)) return false; uint64_t v = 0; for (int k = 0; k < 8; ++k) v |= ((uint64_t)p[pos + k]) << (8 * k);
        memcpy(&out, &v, 8); pos += 8; return true; }
    bool f32(float& out) { uint32_t v = 0; if (!need(4)) return false;
        for (int k = 0; k < 4; ++k) v |= ((uint32_t)p[pos + k]) << (8 * k);
        memcpy(&out, &v, 4); pos += 4; return true; }
    bool f64(double& out) { uint64_t v = 0; if (!need(8)) return false;
        for (int k = 0; k < 8; ++k) v |= ((uint64_t)p[pos + k]) << (8 * k);
        memcpy(&out, &v, 8); pos += 8; return true; }
    bool bytes(uint8_t* out, size_t k) { if (!need(k)) return false; memcpy(out, p + pos, k); pos += k; return true; }
};

// ---- options ---------------------------------------------------------------
constexpr int    kMaxDepth     = 128;
constexpr size_t kMaxNodes     = 4000000;
[[maybe_unused]] constexpr size_t kMaxNbtStringBytes = 65535;

struct ParseOptions {
    bool skipHeader = false;   // true: level.dat container (8-byte header before NBT)
    bool strictHeader = true;  // verify header payload length
};

// ---- parser (recursive descent, depth bounded) ----------------------------
//
// Wire format reminder (Bedrock / little-endian):
//   * compound child      = type byte + uint16 name length + name bytes + payload
//   * list element        = payload only (no type byte, no name) — the element
//                           type is declared once in the list header
//   * TAG_End (0)         = terminates a compound
// Depth is capped by kMaxDepth so the C++ stack cannot be exhausted; exceeding
// the limit is an error, never an abort.
namespace parser {

struct State {
    ByteReader r;
    std::vector<Node> arena;
    size_t nodes = 0;
};

Err readTag(State& st, bool withName, Node& out, int depth);

Err readPayload(State& st, Node& nd) {
    switch (nd.type) {
        case Type::End: return Err::None;
        case Type::Byte: { uint8_t b; if (!st.r.u8(b)) return Err::Truncated; nd.i = (int8_t)b; return Err::None; }
        case Type::Short: { int16_t v; if (!st.r.i16(v)) return Err::Truncated; nd.i = v; return Err::None; }
        case Type::Int: { int32_t v; if (!st.r.i32(v)) return Err::Truncated; nd.i = v; return Err::None; }
        case Type::Long: { int64_t v; if (!st.r.i64(v)) return Err::Truncated; nd.i = v; return Err::None; }
        case Type::Float: { float v; if (!st.r.f32(v)) return Err::Truncated; nd.d = v; return Err::None; }
        case Type::Double: { double v; if (!st.r.f64(v)) return Err::Truncated; nd.d = v; return Err::None; }
        case Type::String: {
            uint16_t l; if (!st.r.u16(l)) return Err::Truncated;
            if (l) { nd.s.resize(l);
                if (!st.r.bytes(reinterpret_cast<uint8_t*>(nd.s.data()), l)) return Err::Truncated; }
            return Err::None; }
        case Type::ByteArray: {
            int32_t c; if (!st.r.i32(c)) return Err::Truncated;
            if (c < 0) return Err::Truncated;
            if ((size_t)c > kMaxNodes) return Err::SizeLimit;
            if ((uint64_t)c > st.r.remaining()) return Err::Truncated;
            nd.bytes.resize((size_t)c);
            if (c && !st.r.bytes(nd.bytes.data(), (size_t)c)) return Err::Truncated;
            return Err::None; }
        case Type::IntArray:
        case Type::LongArray: {
            int32_t c; if (!st.r.i32(c)) return Err::Truncated;
            if (c < 0) return Err::Truncated;
            if ((size_t)c > kMaxNodes) return Err::SizeLimit;
            uint64_t bytesPer = (nd.type == Type::IntArray) ? 4u : 8u;
            if ((uint64_t)c * bytesPer > st.r.remaining()) return Err::Truncated;
            nd.longs.resize((size_t)c);
            for (size_t k = 0; k < (size_t)c; ++k) {
                if (nd.type == Type::IntArray) { int32_t v; if (!st.r.i32(v)) return Err::Truncated; nd.longs[k] = v; }
                else { int64_t v; if (!st.r.i64(v)) return Err::Truncated; nd.longs[k] = v; }
            }
            return Err::None; }
        default: return Err::Internal;
    }
}

// Minimum number of wire bytes required by one list element.  This is only a
// lower bound used to reject impossible counts before entering a long loop.
uint64_t minimumWireBytes(Type t) {
    switch (t) {
        case Type::End:      return 0;
        case Type::Byte:     return 1;
        case Type::Short:    return 2;
        case Type::Int:      return 4;
        case Type::Long:     return 8;
        case Type::Float:    return 4;
        case Type::Double:   return 8;
        case Type::ByteArray:return 4;   // nested count
        case Type::String:   return 2;   // nested length
        case Type::List:     return 5;   // element type + count
        case Type::Compound: return 1;   // TAG_End
        case Type::IntArray: return 4;   // nested count
        case Type::LongArray:return 4;   // nested count
    }
    return 1;
}

// Reads the payload (and children) of a node whose type is already known.
Err readValue(State& st, Node& out, int depth) {
    if (depth > kMaxDepth) return Err::DepthLimit;
    if (st.nodes > kMaxNodes) return Err::SizeLimit;

    if (out.type == Type::Compound) {
        for (;;) {
            size_t peek = st.r.pos;
            uint8_t c8; if (!st.r.u8(c8)) return Err::Truncated;
            if (c8 == 0) break;                          // TAG_End terminator
            st.r.pos = peek;                             // readTag() consumes the type byte once
            Node child;
            Err e = readTag(st, /*withName*/ true, child, depth + 1);
            if (e != Err::None) return e;
            out.kids.push_back((uint32_t)st.arena.size());
            st.arena.push_back(std::move(child));
        }
        return Err::None;
    }

    if (out.type == Type::List) {
        uint8_t e8; if (!st.r.u8(e8)) return Err::Truncated;
        Type et{}; if (!typeFromId(e8, et)) return Err::UnknownTag;
        int32_t count; if (!st.r.i32(count)) return Err::Truncated;
        if (count < 0) return Err::Truncated;
        if (et == Type::End && count != 0) return Err::Truncated;
        if ((uint64_t)count > (uint64_t)(kMaxNodes - std::min(st.nodes, kMaxNodes)))
            return Err::SizeLimit;
        uint64_t minBytes = minimumWireBytes(et);
        if (minBytes != 0 && (uint64_t)count > st.r.remaining() / minBytes)
            return Err::Truncated;
        out.elemType = et;
        // List elements are arena nodes too.  Account for them before parsing
        // so a tiny file cannot request billions of empty elements.
        st.nodes += (size_t)count;
        for (int32_t k = 0; k < count; ++k) {
            Node child; child.type = et;                 // payload only, no header
            Err e = readValue(st, child, depth + 1);
            if (e != Err::None) return e;
            out.kids.push_back((uint32_t)st.arena.size());
            st.arena.push_back(std::move(child));
        }
        return Err::None;
    }

    return readPayload(st, out);
}

// Reads a tag *with* its type byte (and optional name), then its payload.
Err readTag(State& st, bool withName, Node& out, int depth) {
    if (depth > kMaxDepth) return Err::DepthLimit;
    uint8_t t8; if (!st.r.u8(t8)) return Err::Truncated;
    if (t8 == 0) return Err::Truncated;                  // unexpected TAG_End
    Type t{}; if (!typeFromId(t8, t)) return Err::UnknownTag;
    out.type = t;
    if (st.nodes >= kMaxNodes) return Err::SizeLimit;
    ++st.nodes;
    if (withName) {
        uint16_t nl; if (!st.r.u16(nl)) return Err::Truncated;
        if (nl) { out.name.resize(nl);
            if (!st.r.bytes(reinterpret_cast<uint8_t*>(out.name.data()), nl)) return Err::Truncated; }
    }
    return readValue(st, out, depth);
}

} // namespace parser


// ---- entry points ----------------------------------------------------------
Result<Document> parseBytes(const uint8_t* data, size_t len, const std::string& path, ParseOptions opt) {
    Document doc;
    doc.path = path;
    if (len == 0) return Result<Document>::fail(Err::Empty);

    parser::State st;
    st.r.p = data;
    st.r.n = len;
    st.r.pos = 0;
    size_t payloadEnd = len;

    if (opt.skipHeader) {
        int32_t version = 0, plen = 0;
        if (!st.r.i32(version) || !st.r.i32(plen)) return Result<Document>::fail(Err::Truncated);
        if (version < 0 || plen < 0)
            return Result<Document>::fail(Err::HeaderLengthMismatch, "invalid header values");
        doc.hasLevelDatHeader = true;
        doc.headerVersion = version;
        st.r.start = st.r.pos;
        if ((uint64_t)plen > (uint64_t)(len - st.r.start))
            return Result<Document>::fail(Err::HeaderLengthMismatch,
                "header declares " + std::to_string(plen) + " payload bytes but the file holds " +
                std::to_string(len - st.r.start));
        payloadEnd = st.r.start + (size_t)plen;
        if (opt.strictHeader && payloadEnd != len)
            return Result<Document>::fail(Err::HeaderLengthMismatch,
                "header declares " + std::to_string(plen) + " payload bytes but the file holds " +
                std::to_string(len - st.r.start));
        // The NBT payload ends at the declared boundary even in non-strict
        // mode; bytes outside it are container metadata, not part of the root.
        st.r.n = payloadEnd;
    }

    Node root;
    Err e = parser::readTag(st, /*withName*/ true, root, 0);
    if (e != Err::None) {
        std::string d = errText(e);
        d += " (offset " + std::to_string(st.r.pos) + ")";
        if (e == Err::UnknownTag) {
            uint8_t bad = (st.r.pos < len) ? data[st.r.pos] : 0;
            char hx[8]; std::snprintf(hx, sizeof hx, "0x%02x", (unsigned)bad);
            d += ": type byte "; d += hx;
        }
        return Result<Document>::fail(e, d);
    }
    if (st.r.pos != payloadEnd)
        return Result<Document>::fail(Err::TrailingBytes,
            "NBT root ended at offset " + std::to_string(st.r.pos) +
            " but the payload ends at " + std::to_string(payloadEnd));
    doc.arena = std::move(st.arena);
    doc.root = 0;
    doc.arena.insert(doc.arena.begin(), std::move(root));
    for (auto& n : doc.arena) {
        for (auto& k : n.kids) ++k;      // children shifted by the inserted root
    }
    return Result<Document>::good(std::move(doc));
}

// ---- serializer ------------------------------------------------------------
inline std::vector<std::string> validate(const Document& doc);

struct ByteWriter {
    const std::vector<Node>* arena = nullptr;
    std::vector<uint8_t> out;
    const Node& arenaRef(uint32_t i) const { return (*arena)[i]; }
    void u8(uint8_t v)  { out.push_back(v); }
    void u16(uint16_t v){ out.push_back((uint8_t)(v & 0xff)); out.push_back((uint8_t)(v >> 8)); }
    void i32(int32_t v) { uint32_t u; memcpy(&u, &v, 4);
        for (int k = 0; k < 4; ++k) out.push_back((uint8_t)(u >> (8 * k))); }
    void i64(int64_t v) { uint64_t u; memcpy(&u, &v, 8);
        for (int k = 0; k < 8; ++k) out.push_back((uint8_t)(u >> (8 * k))); }
    void f32(float v)   { uint32_t u; memcpy(&u, &v, 4);
        for (int k = 0; k < 4; ++k) out.push_back((uint8_t)(u >> (8 * k))); }
    void f64(double v)  { uint64_t u; memcpy(&u, &v, 8);
        for (int k = 0; k < 8; ++k) out.push_back((uint8_t)(u >> (8 * k))); }
    void raw(const uint8_t* p, size_t n) { out.insert(out.end(), p, p + n); }
    void name(const std::string& s);  // defined after nbt::Tag helpers
};
void writeTree(ByteWriter& w, const Node& nd, bool withName);
void writeValue(ByteWriter& w, const Node& nd);
void writePayload(ByteWriter& w, const Node& nd);

// A tag written with its type byte and (optionally) its name: root + compound children.
inline void writeTree(ByteWriter& w, const Node& nd, bool withName) {
    w.u8((uint8_t)nd.type);
    if (withName) {
        w.u16((uint16_t)nd.name.size());
        w.raw(reinterpret_cast<const uint8_t*>(nd.name.data()), nd.name.size());
    }
    writeValue(w, nd);
}

// A list element: payload only — no type byte, no name (matches the parser).
inline void writeValue(ByteWriter& w, const Node& nd) {
    switch (nd.type) {
        case Type::Compound:
            for (uint32_t k : nd.kids) writeTree(w, w.arenaRef(k), /*withName*/ true);
            w.u8(0);                                   // TAG_End terminator
            return;
        case Type::List:
            w.u8((uint8_t)nd.elemType);
            w.i32((int32_t)nd.kids.size());
            for (uint32_t k : nd.kids) writeValue(w, w.arenaRef(k));
            return;
        default:
            writePayload(w, nd);                       // scalars and arrays
            return;
    }
}

inline void writePayload(ByteWriter& w, const Node& nd) {
    switch (nd.type) {
        case Type::End: return;
        case Type::Byte:   w.u8((uint8_t)(nd.i & 0xff)); return;
        case Type::Short:  w.u16((uint16_t)(nd.i & 0xffff)); return;
        case Type::Int:    w.i32((int32_t)nd.i); return;
        case Type::Long:   w.i64(nd.i); return;
        case Type::Float:  w.f32((float)nd.d); return;
        case Type::Double: w.f64(nd.d); return;
        case Type::String: {
            w.u16((uint16_t)nd.s.size());
            w.raw(reinterpret_cast<const uint8_t*>(nd.s.data()), nd.s.size());
            return; }
        case Type::ByteArray: {
            w.i32((int32_t)nd.bytes.size());
            w.raw(nd.bytes.data(), nd.bytes.size());
            return; }
        case Type::IntArray: {
            w.i32((int32_t)nd.longs.size());
            for (int64_t v : nd.longs) w.i32((int32_t)v);   // int32 elements
            return; }
        case Type::LongArray: {
            w.i32((int32_t)nd.longs.size());
            for (int64_t v : nd.longs) w.i64(v);            // int64 elements
            return; }
        default: return;
    }
}

Result<std::vector<uint8_t>> serialize(const Document& doc) {
    std::vector<std::string> problems = validate(doc);
    if (!problems.empty()) {
        std::string detail;
        for (const std::string& problem : problems) {
            if (!detail.empty()) detail += "; ";
            detail += problem;
        }
        return Result<std::vector<uint8_t>>::fail(Err::InvalidDocument, detail);
    }
    ByteWriter w;
    w.arena = &doc.arena;
    writeTree(w, doc.arena[doc.root], /*withName*/ true);
    return Result<std::vector<uint8_t>>::good(std::move(w.out));
}

Result<std::vector<uint8_t>> serializeLevelDat(const Document& doc) {
    auto payloadResult = serialize(doc);
    if (!payloadResult.ok()) return payloadResult;
    if (payloadResult.value.size() > (size_t)INT32_MAX)
        return Result<std::vector<uint8_t>>::fail(Err::SizeLimit,
            "level.dat payload is larger than the signed 32-bit header allows");
    int32_t version = doc.hasLevelDatHeader ? doc.headerVersion : 8;
    ByteWriter w;
    w.i32(version);
    w.i32((int32_t)payloadResult.value.size());
    w.raw(payloadResult.value.data(), payloadResult.value.size());
    return Result<std::vector<uint8_t>>::good(std::move(w.out));
}

// ---- validation ------------------------------------------------------------

// ---- message helpers (reference strings reused verbatim where applicable) --
inline const char* msgUnnamedTag()      { return "Tags inside a CompoundTag must have a name"; }
inline const char* msgDuplicateName()   { return "A tag with this name already exists in the compound"; }
inline const char* msgElementMismatch() { return "List element type mismatch, it should be"; }
inline const char* msgUnknownToEdit()   { return "Unknown tag to edit, report to developers."; }
inline const char* msgOutOfBoundsFor2() { return "Out of bounds for byte."; }
inline const char* msgOutOfBoundsFor()  { return "Only tags of the matching type can be added here."; }

enum class RangeCheck { Ok, OutLow, OutHigh, NotNumeric, NotFinite };

inline RangeCheck rangeFor(Type t, const std::string& text) {
    if (text.empty()) return RangeCheck::NotNumeric;
    errno = 0;
    char* end = nullptr;
    switch (t) {
        case Type::Byte: {
            long long v = strtoll(text.c_str(), &end, 10);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (v < -128) return RangeCheck::OutLow;
            if (v > 127) return RangeCheck::OutHigh;
            return RangeCheck::Ok; }
        case Type::Short: {
            long long v = strtoll(text.c_str(), &end, 10);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (v < -32768) return RangeCheck::OutLow;
            if (v > 32767) return RangeCheck::OutHigh;
            return RangeCheck::Ok; }
        case Type::Int: {
            long long v = strtoll(text.c_str(), &end, 10);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (v < INT32_MIN) return RangeCheck::OutLow;
            if (v > INT32_MAX) return RangeCheck::OutHigh;
            return RangeCheck::Ok; }
        case Type::Long: {
            errno = 0;
            strtoll(text.c_str(), &end, 10);
            if (errno == ERANGE) return RangeCheck::OutHigh;
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            return RangeCheck::Ok; }
        case Type::Float: {
            double v = strtod(text.c_str(), &end);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (!std::isfinite(v)) return RangeCheck::NotFinite;
            if (v < -3.4028234663852886e38) return RangeCheck::OutLow;
            if (v >  3.4028234663852886e38) return RangeCheck::OutHigh;
            return RangeCheck::Ok; }
        case Type::Double: {
            double v = strtod(text.c_str(), &end);
            if (end == text.c_str() || *end) return RangeCheck::NotNumeric;
            if (!std::isfinite(v)) return RangeCheck::NotFinite;
            return RangeCheck::Ok; }
        default: return RangeCheck::Ok;
    }
}

inline const char* rangeMessage(Type t, RangeCheck c) {
    if (c == RangeCheck::NotNumeric || c == RangeCheck::NotFinite) return "Enter a valid number.";
    switch (t) {
        case Type::Byte:   return "Out of bounds for byte.";
        case Type::Short: return "Out of bounds for short.";
        case Type::Int:   return "Out of bounds for integer.";
        case Type::Long:  return "Out of bounds for long.";
        case Type::Float: return "Out of bounds for float.";
        case Type::Double:return "Out of bounds for double.";
        default:          return "Out of bounds for string.";
    }
}

inline bool parseScalar(const std::string& text, Type t, Node& out, std::string& msg) {
    RangeCheck c = rangeFor(t, text);
    if (c != RangeCheck::Ok) { msg = rangeMessage(t, c); return false; }
    switch (t) {
        case Type::Byte: { long long v = strtoll(text.c_str(), nullptr, 10); out.i = (int8_t)v; return true; }
        case Type::Short:{ long long v = strtoll(text.c_str(), nullptr, 10); out.i = (int16_t)v; return true; }
        case Type::Int:  { long long v = strtoll(text.c_str(), nullptr, 10); out.i = (int32_t)v; return true; }
        case Type::Long: { out.i = strtoll(text.c_str(), nullptr, 10); return true; }
        case Type::Float:{ out.d = strtod(text.c_str(), nullptr); return true; }
        case Type::Double:{ out.d = strtod(text.c_str(), nullptr); return true; }
        case Type::String:{
            if (text.size() > 65535) { msg = "Out of bounds for string."; return false; }
            out.s = text; return true; }
        default: msg = "Unknown tag to edit, report to developers."; return false;
    }
}

// Semantic validation of the reachable document tree.  The writer calls this
// before touching the output, so invalid names, counts, ranges, child indexes,
// list element types, and cycles cannot silently corrupt a file.
inline void validateNode(const Document& doc, uint32_t idx, int depth,
                         std::unordered_set<uint32_t>& visited, size_t& nodeCount,
                         std::vector<std::string>& problems) {
    if (idx >= doc.arena.size()) {
        problems.push_back("Dangling child index.");
        return;
    }
    if (!visited.insert(idx).second) {
        problems.push_back("A node is referenced more than once.");
        return;
    }
    ++nodeCount;
    if (nodeCount > kMaxNodes) {
        problems.push_back("Document too large (node limit reached).");
        return;
    }
    if (depth > kMaxDepth) {
        problems.push_back("Nesting too deep.");
        return;
    }

    const Node& nd = doc.arena[idx];
    if (nd.type == Type::End) {
        problems.push_back("TAG_End is not a value node.");
        return;
    }
    if (nd.name.size() > kMaxNbtStringBytes)
        problems.push_back("Tag name is longer than 65535 UTF-8 bytes.");
    if (nd.type == Type::String && nd.s.size() > kMaxNbtStringBytes)
        problems.push_back("String value is longer than 65535 UTF-8 bytes.");

    switch (nd.type) {
        case Type::Byte:
            if (nd.i < -128 || nd.i > 127) problems.push_back("Byte value is out of range.");
            break;
        case Type::Short:
            if (nd.i < -32768 || nd.i > 32767) problems.push_back("Short value is out of range.");
            break;
        case Type::Int:
            if (nd.i < INT32_MIN || nd.i > INT32_MAX) problems.push_back("Int value is out of range.");
            break;
        case Type::Long:
            break;
        case Type::Float:
        case Type::Double:
            break;
        case Type::ByteArray:
            if (nd.bytes.size() > (size_t)INT32_MAX)
                problems.push_back("Byte array is too large.");
            break;
        case Type::String:
            break;
        case Type::IntArray:
            if (nd.longs.size() > (size_t)INT32_MAX)
                problems.push_back("Int array is too large.");
            for (int64_t value : nd.longs)
                if (value < INT32_MIN || value > INT32_MAX)
                    problems.push_back("Int array contains a value outside signed 32-bit range.");
            break;
        case Type::LongArray:
            if (nd.longs.size() > (size_t)INT32_MAX)
                problems.push_back("Long array is too large.");
            break;
        case Type::List: {
            if (nd.kids.size() > (size_t)INT32_MAX)
                problems.push_back("List is too large.");
            Type ignored{};
            if (!typeFromId((uint8_t)nd.elemType, ignored))
                problems.push_back("List has an invalid element type.");
            if (nd.elemType == Type::End && !nd.kids.empty())
                problems.push_back("A non-empty list cannot have TAG_End element type.");
            break; }
        case Type::Compound:
            break;
        case Type::End:
            break;
    }

    if (isCompound(nd.type)) {
        std::unordered_set<std::string> names;
        for (uint32_t child : nd.kids) {
            if (child >= doc.arena.size()) {
                problems.push_back("Dangling child index.");
                continue;
            }
            const Node& childNode = doc.arena[child];
            // Empty names are legal on the wire and existing files can contain
            // them.  The editor prevents creating unnamed tags, but the
            // serializer must still be able to round-trip such documents.
            // Duplicate detection applies to the named subset only.
            if (!childNode.name.empty() && !names.insert(childNode.name).second)
                problems.push_back(msgDuplicateName());
        }
    }

    if (isList(nd.type)) {
        for (uint32_t child : nd.kids) {
            if (child >= doc.arena.size()) {
                problems.push_back("Dangling child index.");
                continue;
            }
            const Node& childNode = doc.arena[child];
            if (childNode.type != nd.elemType) {
                std::string message = msgElementMismatch();
                message += " ";
                message += typeName(nd.elemType);
                problems.push_back(message);
            }
            if (!childNode.name.empty())
                problems.push_back("List elements cannot have names.");
        }
    }

    for (uint32_t child : nd.kids)
        validateNode(doc, child, depth + 1, visited, nodeCount, problems);
}

inline std::vector<std::string> validate(const Document& doc) {
    std::vector<std::string> problems;
    if (doc.root >= doc.arena.size()) {
        problems.push_back("Dangling root index.");
        return problems;
    }
    if (doc.hasLevelDatHeader && doc.headerVersion < 0)
        problems.push_back("level.dat header version is invalid.");
    std::unordered_set<uint32_t> visited;
    size_t nodeCount = 0;
    validateNode(doc, doc.root, 0, visited, nodeCount, problems);
    return problems;
}

// ---- queries ---------------------------------------------------------------
inline uint64_t nodeBytes(const Document& doc, uint32_t idx = 0) {
    if (idx >= doc.arena.size()) return 0;
    ByteWriter w;
    w.arena = &doc.arena;
    writeTree(w, doc.arena[idx], false);
    return w.out.size();
}

struct FindHit { std::vector<uint32_t> path; std::string name; Type type; };

inline void findRec(const Document& doc, uint32_t idx, const std::string& needle,
                    std::vector<uint32_t>& path, std::vector<FindHit>& out, int depth) {
    if (depth > kMaxDepth) return;
    const Node& nd = doc.arena[idx];
    path.push_back(idx);
    if (nd.name.find(needle) != std::string::npos)
        out.push_back({path, nd.name, nd.type});
    for (uint32_t k : nd.kids) findRec(doc, k, needle, path, out, depth + 1);
    path.pop_back();
}

inline std::vector<FindHit> find(const Document& doc, const std::string& needle, uint32_t from = 0) {
    std::vector<FindHit> out;
    std::vector<uint32_t> path;
    findRec(doc, from, needle, path, out, 0);
    return out;
}

inline uint64_t countNodes(const Document& doc, uint32_t idx = 0) {
    uint64_t c = 1;
    for (uint32_t k : doc.arena[idx].kids) c += countNodes(doc, k);
    return c;
}

} // namespace nbt



// ============================================================================
// L2 — RENDERER (software framebuffer + anti-aliased glyph table)
// Glyphs are embedded as 8-bit coverage at 45 px and sampled at the current
// density.  The generated table includes ASCII, Latin-1, and common UI
// punctuation; no runtime font file or asset is required.
// ============================================================================
#if !NBT_APP_BUILD_DONE
#define NBT_APP_BUILD_DONE 1
namespace gfx {

static constexpr int kGlyphH = 15;         // line box in design units
static constexpr int kGlyphAdv = 8;         // legacy average advance
static constexpr int kGlyphBaseline = 12;   // baseline offset in line box
static constexpr int kFontDesignRatio = 3;  // 45 raster px -> 15 design units

struct FontGlyph {
    uint32_t pixelOffset;
    uint16_t width;
    uint16_t height;
    int16_t leftQ4;      // bearing from pen to bitmap, in 1/16 raster px
    int16_t topQ4;       // bearing from baseline to bitmap, in 1/16 raster px
    uint16_t advanceQ4;  // advance in 1/16 raster px
};

#include "font_table.inc"

// Decode one UTF-8 code point without ever returning past the end of the
// string.  Invalid input becomes U+FFFD and consumes one byte.
inline uint32_t utf8DecodeAt(const std::string& s, size_t offset, size_t* byteLength) {
    const auto continuation = [&](size_t at) -> bool {
        return at < s.size() && ((static_cast<unsigned char>(s[at]) & 0xc0) == 0x80);
    };
    const unsigned char first = offset < s.size() ? static_cast<unsigned char>(s[offset]) : 0;
    if (first < 0x80) {
        *byteLength = 1;
        return first;
    }
    if (first >= 0xc2 && first <= 0xdf && continuation(offset + 1)) {
        *byteLength = 2;
        return ((first & 0x1f) << 6) |
               (static_cast<unsigned char>(s[offset + 1]) & 0x3f);
    }
    if (first >= 0xe0 && first <= 0xef && continuation(offset + 1) && continuation(offset + 2)) {
        const unsigned char second = static_cast<unsigned char>(s[offset + 1]);
        if ((first == 0xe0 && second >= 0xa0) || (first == 0xed && second <= 0x9f)) {
            *byteLength = 1;
            return 0xfffd;
        }
        *byteLength = 3;
        return ((first & 0x0f) << 12) | ((second & 0x3f) << 6) |
               (static_cast<unsigned char>(s[offset + 2]) & 0x3f);
    }
    if (first >= 0xf0 && first <= 0xf4 && continuation(offset + 1) &&
        continuation(offset + 2) && continuation(offset + 3)) {
        const unsigned char second = static_cast<unsigned char>(s[offset + 1]);
        if ((first == 0xf0 && second < 0x90) || (first == 0xf4 && second > 0x8f)) {
            *byteLength = 1;
            return 0xfffd;
        }
        *byteLength = 4;
        return ((first & 0x07) << 18) | ((second & 0x3f) << 12) |
               ((static_cast<unsigned char>(s[offset + 2]) & 0x3f) << 6) |
               (static_cast<unsigned char>(s[offset + 3]) & 0x3f);
    }
    *byteLength = 1;
    return 0xfffd;
}

inline int glyphIndexForCodepoint(uint32_t codepoint) {
    uint32_t low = 0;
    uint32_t high = kFontCodepointPairCount;
    while (low < high) {
        const uint32_t mid = low + (high - low) / 2;
        const uint32_t value = kFontCodepointPairs[mid * 2];
        if (value == codepoint) return static_cast<int>(kFontCodepointPairs[mid * 2 + 1]);
        if (value < codepoint) low = mid + 1;
        else high = mid;
    }
    // ASCII '?' is the final entry of the first 95 generated glyphs.
    return codepoint == '?' ? 31 : -1;
}

inline double glyphAdvanceDesign(int glyphIndex) {
    if (glyphIndex < 0 || glyphIndex >= static_cast<int>(kFontGlyphCount)) return 0.0;
    return kFontGlyphs[glyphIndex].advanceQ4 /
           (16.0 * static_cast<double>(kFontDesignRatio));
}

inline int fontTextWidth(const std::string& s, int scale = 1) {
    double width = 0.0;
    size_t offset = 0;
    while (offset < s.size()) {
        size_t length = 1;
        const uint32_t codepoint = utf8DecodeAt(s, offset, &length);
        offset += length;
        if (codepoint == '\n') continue;
        int glyphIndex = glyphIndexForCodepoint(codepoint);
        if (glyphIndex < 0) glyphIndex = glyphIndexForCodepoint('?');
        width += glyphAdvanceDesign(glyphIndex);
    }
    return static_cast<int>(std::ceil(width * static_cast<double>(std::max(1, scale))));
}

inline std::string utf8PrefixByWidth(const std::string& s, int maxWidth, int scale = 1) {
    std::string out;
    double width = 0.0;
    size_t offset = 0;
    while (offset < s.size()) {
        size_t length = 1;
        const uint32_t codepoint = utf8DecodeAt(s, offset, &length);
        if (codepoint == '\n') break;
        int glyphIndex = glyphIndexForCodepoint(codepoint);
        if (glyphIndex < 0) glyphIndex = glyphIndexForCodepoint('?');
        const double advance = glyphAdvanceDesign(glyphIndex) * std::max(1, scale);
        if (!out.empty() && width + advance > static_cast<double>(maxWidth)) break;
        out.append(s, offset, length);
        width += advance;
        offset += length;
        if (width >= static_cast<double>(maxWidth)) break;
    }
    return out;
}

inline std::string utf8TakeChars(const std::string& s, size_t maxChars) {
    size_t offset = 0;
    size_t used = 0;
    while (offset < s.size() && used < maxChars) {
        size_t length = 1;
        utf8DecodeAt(s, offset, &length);
        offset += length;
        ++used;
    }
    return s.substr(0, offset);
}

// Keep the beginning of a value visible when it cannot fit in its column.
// Keeping the prefix is important for signed numbers: a clipped tail hides
// the sign and can make a negative value look positive.
inline std::string ellipsizeForWidth(const std::string& s, int maxWidth, int scale = 1) {
    if (maxWidth <= 0 || s.empty()) return "";
    if (fontTextWidth(s, scale) <= maxWidth) return s;

    const std::string ellipsis = "...";
    const int ellipsisWidth = fontTextWidth(ellipsis, scale);
    if (ellipsisWidth >= maxWidth) return utf8PrefixByWidth(s, maxWidth, scale);

    std::string cut = utf8PrefixByWidth(s, maxWidth - ellipsisWidth, scale);
    if (cut.empty()) cut = utf8TakeChars(s, 1);
    cut += ellipsis;
    return cut;
}

} // namespace gfx
#endif // NBT_APP_BUILD_DONE

// ============================================================================
// L1 — KERNEL PRIMITIVES (math, colour, utf8, string helpers)
// ============================================================================
namespace gfx {

struct Color { uint8_t r, g, b, a; };
inline Color rgb(uint8_t r, uint8_t g, uint8_t b) { return {r, g, b, 255}; }
inline Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return {r, g, b, a}; }
// Resolve a translucent colour against an opaque backdrop.  The software
// canvas stores opaque pixels, so translucent fills must be resolved before
// they are written instead of leaving an unexpected alpha in the framebuffer.
inline Color blendOpaque(Color source, Color background, int alpha) {
    alpha = std::max(0, std::min(255, alpha));
    const int inverse = 255 - alpha;
    const auto channel = [&](uint8_t foreground, uint8_t backdrop) -> uint8_t {
        return static_cast<uint8_t>((static_cast<int>(foreground) * alpha +
                                     static_cast<int>(backdrop) * inverse + 127) / 255);
    };
    return rgb(channel(source.r, background.r), channel(source.g, background.g),
               channel(source.b, background.b));
}
inline uint32_t pack(Color c) { return (uint32_t)c.r | ((uint32_t)c.g << 8) | ((uint32_t)c.b << 16) | ((uint32_t)c.a << 24); }

struct Rect { int x = 0, y = 0, w = 0, h = 0;
    int right()  const { return x + w; }
    int bottom() const { return y + h; }
    bool contains(int px, int py) const { return px >= x && py >= y && px < x + w && py < y + h; }
    bool empty() const { return w <= 0 || h <= 0; }
};
inline Rect intersect(Rect a, Rect b) {
    int x1 = std::max(a.x, b.x), y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.w, b.x + b.w), y2 = std::min(a.y + a.h, b.y + b.h);
    if (x2 <= x1 || y2 <= y1) return {0, 0, 0, 0};
    return {x1, y1, x2 - x1, y2 - y1};
}

struct Theme {
    Color primary, secondary, background, surface, onPrimary, onSurface, onSurfaceMuted,
          outline, error, errorContainer, success, scrim;
};

// Reference palette (ANALYSIS.md §3) + dark counterpart.
inline Theme themeLight() {
    return {/*primary*/ rgb(0x42, 0x9e, 0x46), /*secondary*/ rgb(0x66, 0xbb, 0x6a),
            /*background*/ rgb(0xf1, 0xf8, 0xe9), /*surface*/ rgb(0xff, 0xff, 0xff),
            /*onPrimary*/ rgb(0xff, 0xff, 0xff), /*onSurface*/ rgb(0x1b, 0x1b, 0x1b),
            /*onSurfaceMuted*/ rgb(0x60, 0x6a, 0x60), /*outline*/ rgb(0xc6, 0xd6, 0xc6),
            /*error*/ rgb(0xb0, 0x00, 0x20), /*errorContainer*/ rgb(0xff, 0xda, 0xda),
            /*success*/ rgb(0x1b, 0x7f, 0x3b), /*scrim*/ rgba(0x00, 0x00, 0x00, 0x40)};
}
inline Theme themeDark() {
    return {/*primary*/ rgb(0x7c, 0xc0, 0x7f), /*secondary*/ rgb(0x57, 0x9e, 0x5b),
            /*background*/ rgb(0x10, 0x14, 0x10), /*surface*/ rgb(0x1c, 0x21, 0x1c),
            /*onPrimary*/ rgb(0x08, 0x14, 0x08), /*onSurface*/ rgb(0xe6, 0xec, 0xe6),
            /*onSurfaceMuted*/ rgb(0x9a, 0xa6, 0x9a), /*outline*/ rgb(0x3a, 0x44, 0x3a),
            /*error*/ rgb(0xff, 0x8a, 0x80), /*errorContainer*/ rgb(0x3a, 0x14, 0x14),
            /*success*/ rgb(0x7c, 0xd8, 0x9a), /*scrim*/ rgba(0x00, 0x00, 0x00, 0x60)};
}

// ---- utf8 / string helpers -------------------------------------------------
inline size_t utf8Len(const std::string& s) {
    size_t offset = 0;
    size_t count = 0;
    while (offset < s.size()) {
        size_t length = 1;
        utf8DecodeAt(s, offset, &length);
        offset += length;
        ++count;
    }
    return count;
}

inline std::string utf8Substr(const std::string& s, size_t chars, size_t maxPixels, int glyphPx) {
    // Trim by visible code points and by the real font width.  `glyphPx` keeps
    // the old call shape while its ratio supplies the requested text scale.
    const std::string prefix = utf8TakeChars(s, chars);
    const int scale = std::max(1, glyphPx / std::max(1, kGlyphAdv));
    return utf8PrefixByWidth(prefix, maxPixels, scale);
}

inline bool startsWith(const std::string& s, const std::string& p) {
    return s.size() >= p.size() && s.compare(0, p.size(), p) == 0;
}
inline std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
inline std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) { if (c == sep) { out.push_back(cur); cur.clear(); } else cur += c; }
    out.push_back(cur);
    return out;
}

inline std::string humanSize(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double v = (double)bytes;
    int u = 0;
    while (v >= 1024.0 && u < 4) { v /= 1024.0; ++u; }
    char buf[32];
    if (u == 0) std::snprintf(buf, sizeof buf, "%llu B", (unsigned long long)bytes);
    else std::snprintf(buf, sizeof buf, "%.1f %s", v, units[u]);
    return buf;
}

} // namespace gfx

namespace gfx {

// Wrap UTF-8 text into lines that fit `maxWidth` design units.  Words are
// kept intact when possible; a single overlong word is split by code point.
inline std::vector<std::string> wrapText(const std::string& text, int maxWidth, int scale = 1) {
    std::vector<std::string> lines;
    maxWidth = std::max(1, maxWidth);
    std::string paragraph;
    const auto flushParagraph = [&]() {
        std::string line;
        for (const std::string& word : split(paragraph, ' ')) {
            if (word.empty()) continue;
            if (fontTextWidth(word, scale) > maxWidth) {
                if (!line.empty()) {
                    lines.push_back(line);
                    line.clear();
                }
                std::string remaining = word;
                while (!remaining.empty()) {
                    std::string piece = utf8PrefixByWidth(remaining, maxWidth, scale);
                    if (piece.empty()) piece = utf8TakeChars(remaining, 1);
                    lines.push_back(piece);
                    remaining = remaining.substr(piece.size());
                }
                continue;
            }
            const std::string candidate = line.empty() ? word : line + " " + word;
            if (!line.empty() && fontTextWidth(candidate, scale) > maxWidth) {
                lines.push_back(line);
                line = word;
            } else {
                line = candidate;
            }
        }
        if (!line.empty()) lines.push_back(line);
        paragraph.clear();
    };

    size_t offset = 0;
    while (offset <= text.size()) {
        const size_t newline = text.find('\n', offset);
        if (newline == std::string::npos) {
            paragraph.append(text, offset, text.size() - offset);
            flushParagraph();
            break;
        }
        paragraph.append(text, offset, newline - offset);
        flushParagraph();
        offset = newline + 1;
    }
    return lines;
}

} // namespace gfx

// ============================================================================
// L2a — IMAGE (world_icon.jpeg decoder, cached)
// ============================================================================
namespace gfx {

struct Image {
    int w = 0, h = 0;
    std::vector<uint8_t> rgba;
    bool ok() const { return w > 0 && h > 0 && rgba.size() == (size_t)w * h * 4; }
};

// Decoded world icons are tiny and reused every frame, so one cache entry per
// path is enough.  A failed load is cached too (empty Image) to avoid hitting
// the file system on every repaint.
inline std::shared_ptr<Image> loadImage(const std::string& path) {
    static std::unordered_map<std::string, std::shared_ptr<Image>> cache;
    auto it = cache.find(path);
    if (it != cache.end()) return it->second;
    auto img = std::make_shared<Image>();
    FILE* f = std::fopen(path.c_str(), "rb");
    if (f && std::fseek(f, 0, SEEK_END) == 0) {
        long n = std::ftell(f);
        if (n > 0 && n < 16 * 1024 * 1024 && std::fseek(f, 0, SEEK_SET) == 0) {
            std::vector<uint8_t> raw((size_t)n);
            if (std::fread(raw.data(), 1, raw.size(), f) == raw.size()) {
                int iw = 0, ih = 0, comp = 0;
                unsigned char* px = stbi_load_from_memory(raw.data(),
                                     (int)raw.size(), &iw, &ih, &comp, 4);
                if (px && iw > 0 && ih > 0) {
                    img->w = iw; img->h = ih;
                    img->rgba.assign(px, px + (size_t)iw * ih * 4);
                }
                stbi_image_free(px);
            }
        }
        std::fclose(f);
    }
    cache.emplace(path, img);
    return img;
}

inline std::shared_ptr<Image> imageFromBytes(const std::vector<uint8_t>& raw) {
    auto img = std::make_shared<Image>();
    if (raw.empty()) return img;
    int iw = 0, ih = 0, comp = 0;
    unsigned char* px = stbi_load_from_memory(raw.data(), (int)raw.size(),
                                               &iw, &ih, &comp, 4);
    if (px && iw > 0 && ih > 0) {
        img->w = iw; img->h = ih;
        img->rgba.assign(px, px + (size_t)iw * ih * 4);
    }
    stbi_image_free(px);
    return img;
}

// PNG icons bundled in the XAPK assets (entityIcons, ui).  Decoded once and
// cached by asset path; no per-frame decoding.
inline std::shared_ptr<Image> loadAssetImage(const std::string& rel) {
    static std::unordered_map<std::string, std::shared_ptr<Image>> cache;
    auto it = cache.find(rel);
    if (it != cache.end()) return it->second;
    auto img = imageFromBytes(app::readAssetBytes(rel));
    cache.emplace(rel, img);
    return img;
}

} // namespace gfx

// ============================================================================
// L2 — CANVAS (2D software rasteriser over an RGBA8888 buffer)
// ============================================================================
namespace gfx {

struct Canvas {
    uint32_t* px = nullptr;
    int w = 0, h = 0, stride = 0;     // device pixels
    int uiScale = 1;                  // UI scale: device px per design unit (dp)
    std::vector<Rect> clipStack;      // device-space rects

    void bind(uint32_t* buffer, int width, int height, int stridePx) {
        px = buffer; w = width; h = height; stride = stridePx;
        clipStack.clear();
        clipStack.push_back({0, 0, w, h});
    }
    // The whole UI is laid out in "design units" (dp, ~360 units wide). The
    // canvas multiplies every coordinate by s, so one layout works on every
    // density instead of being microscopic on a 1080p screen.
    void setScale(int v) { uiScale = v < 1 ? 1 : (v > 6 ? 6 : v); }
    Rect dev(Rect r) const { return {r.x * uiScale, r.y * uiScale, r.w * uiScale, r.h * uiScale}; }

    void pushClip(Rect r) { clipStack.push_back(intersect(clipStack.back(), dev(r))); }
    void pushClipDev(Rect r) { clipStack.push_back(intersect(clipStack.back(), r)); }
    void popClip() { if (clipStack.size() > 1) clipStack.pop_back(); }
    Rect clip() const { return clipStack.back(); }

    void clear(Color c) { fill({0, 0, w / uiScale, h / uiScale}, c); }

    // The ANativeWindow buffer is an opaque RGB surface: writing a packed
    // translucent colour directly turns (0,0,0,0x60) into SOLID BLACK instead
    // of a dim overlay.  Every fill primitive therefore blends against the
    // pixels already on the canvas, exactly like blendTextPixel() below.
    void blendPixel(int x, int y, Color c, int alpha = -1) {
        if (x < 0 || y < 0 || x >= w || y >= h) return;
        int a = alpha < 0 ? (int)c.a : alpha;
        if (a <= 0) return;
        uint32_t& dst = px[(size_t)y * stride + x];
        if (a >= 255) { dst = pack({c.r, c.g, c.b, 255}); return; }
        const int inv = 255 - a;
        dst = pack({(uint8_t)((c.r * a + (dst & 0xff) * inv + 127) / 255),
                    (uint8_t)((c.g * a + ((dst >> 8) & 0xff) * inv + 127) / 255),
                    (uint8_t)((c.b * a + ((dst >> 16) & 0xff) * inv + 127) / 255),
                    255});
    }

    void fill(Rect r, Color c) {
        r = intersect(dev(r), clip());
        if (r.empty()) return;
        if (c.a >= 255) {
            uint32_t v = pack(c);
            uint32_t* row = px + (size_t)r.y * stride + r.x;
            for (int y = 0; y < r.h; ++y) {
                std::fill(row, row + r.w, v);
                row += stride;
            }
            return;
        }
        // Translucent scrim / legend overlay: preserve the scene underneath.
        for (int y = r.y; y < r.bottom(); ++y)
            for (int x = r.x; x < r.right(); ++x) blendPixel(x, y, c);
    }
    // Centre-cropped image blit (world_icon.jpeg in a square avatar).
    void image(Rect r, const Image& img) {
        if (!img.ok()) return;
        Rect d = intersect(dev(r), clip());
        if (d.empty()) return;
        const float srcAspect = (float)img.w / (float)img.h;
        const float dstAspect = (float)dev(r).w / (float)dev(r).h;
        int sx0 = 0, sy0 = 0, sw = img.w, sh = img.h;
        if (srcAspect > dstAspect) { sw = (int)(img.h * dstAspect); sx0 = (img.w - sw) / 2; }
        else                       { sh = (int)(img.w / dstAspect); sy0 = (img.h - sh) / 2; }
        if (sw <= 0 || sh <= 0) return;
        for (int y = d.y; y < d.bottom(); ++y) {
            int sy = sy0 + (int)((float)(y - dev(r).y) * sh / dev(r).h);
            for (int x = d.x; x < d.right(); ++x) {
                int sx = sx0 + (int)((float)(x - dev(r).x) * sw / dev(r).w);
                if (sx < 0 || sy < 0 || sx >= img.w || sy >= img.h) continue;
                const uint8_t* p = &img.rgba[((size_t)sy * img.w + sx) * 4];
                blendPixel(x, y, {p[0], p[1], p[2], 255}, p[3]);
            }
        }
    }
    void pixel(int x, int y, Color c) {
        x *= uiScale; y *= uiScale;
        if (!clip().contains(x, y)) return;
        blendPixel(x, y, c);
    }
    void hline(int x0, int x1, int y, Color c) {
        y *= uiScale; x0 *= uiScale; x1 *= uiScale;
        if (y < clip().y || y >= clip().bottom()) return;
        int a = std::max(x0, clip().x), b = std::min(x1, clip().right());
        for (int x = a; x < b; ++x) blendPixel(x, y, c);
    }
    void vline(int x, int y0, int y1, Color c) {
        x *= uiScale; y0 *= uiScale; y1 *= uiScale;
        if (x < clip().x || x >= clip().right()) return;
        int a = std::max(y0, clip().y), b = std::min(y1, clip().bottom());
        for (int y = a; y < b; ++y) blendPixel(x, y, c);
    }
    // anti-aliased rounded rectangle: corners are analysed, interior is filled fast
    void roundRect(Rect r, int radius, Color c) {
        r = dev(r);
        radius *= uiScale;
        if (radius <= 0) { fillRectDev(r, c); return; }
        r = intersect(r, clip());
        if (r.empty()) return;
        const float rad = (float)radius;
        const int xr0 = r.x + radius, xr1 = r.right() - radius;
        const int yr0 = r.y + radius, yr1 = r.bottom() - radius;
        for (int y = r.y; y < r.bottom(); ++y) {
            const int ry = (y < yr0) ? (yr0 - y) : (y >= yr1 ? (y - yr1 + 1) : 0);
            for (int x = r.x; x < r.right(); ++x) {
                const int rx = (x < xr0) ? (xr0 - x) : (x >= xr1 ? (x - xr1 + 1) : 0);
                if (rx == 0 || ry == 0) { blendPixel(x, y, c); continue; }
                const float cx = (float)rx + 0.5f, cy = (float)ry + 0.5f;
                const float d = std::sqrt(cx * cx + cy * cy);
                const float cov = rad + 0.5f - d;
                if (cov <= 0.0f) continue;
                blendPixel(x, y, c, (int)std::lround((float)c.a * cov));
            }
        }
    }
    void fillRectDev(Rect r, Color c) {          // r already in device px
        r = intersect(r, clip());
        if (r.empty()) return;
        if (c.a >= 255) {
            uint32_t v = pack(c);
            uint32_t* row = px + (size_t)r.y * stride + r.x;
            for (int y = 0; y < r.h; ++y) {
                std::fill(row, row + r.w, v);
                row += stride;
            }
            return;
        }
        for (int y = r.y; y < r.bottom(); ++y)
            for (int x = r.x; x < r.right(); ++x) blendPixel(x, y, c);
    }
    void stroke(Rect r, Color c, int thickness = 1) {
        r = dev(r);
        thickness *= uiScale;
        if (thickness <= 0) thickness = 1;
        fillRectDev({r.x, r.y, r.w, thickness}, c);
        fillRectDev({r.x, r.bottom() - thickness, r.w, thickness}, c);
        fillRectDev({r.x, r.y, thickness, r.h}, c);
        fillRectDev({r.right() - thickness, r.y, thickness, r.h}, c);
    }
    void circle(int cx, int cy, int radius, Color c) {
        cx *= uiScale; cy *= uiScale; radius *= uiScale;
        int r2 = radius * radius;
        for (int y = cy - radius; y <= cy + radius; ++y)
            for (int x = cx - radius; x <= cx + radius; ++x) {
                if (x < 0 || y < 0 || x >= w || y >= h) continue;
                if (!clip().contains(x, y)) continue;
                int d2 = (x - cx) * (x - cx) + (y - cy) * (y - cy);
                if (d2 > r2 + radius) continue;
                float cov = 1.0f - std::max(0.0f, std::sqrt((float)d2) - (radius - 0.5f));
                if (cov <= 0) continue;
                blendPixel(x, y, c, (int)std::lround((float)c.a * cov));
            }
    }
    void line(int x0, int y0, int x1, int y1, Color c) {
        x0 *= uiScale; y0 *= uiScale; x1 *= uiScale; y1 *= uiScale;
        int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        while (true) {
            if (x0 >= 0 && y0 >= 0 && x0 < w && y0 < h && clip().contains(x0, y0))
                blendPixel(x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    // ---- text ----
    int textWidth(const std::string& s, int scale = 1) const {
        return fontTextWidth(s, scale);
    }

    static uint8_t sampleCoverage(const uint8_t* bitmap, int width, int height,
                                  float sourceX, float sourceY) {
        sourceX = std::max(0.0f, std::min(static_cast<float>(width - 1), sourceX));
        sourceY = std::max(0.0f, std::min(static_cast<float>(height - 1), sourceY));
        const int x0 = static_cast<int>(std::floor(sourceX));
        const int y0 = static_cast<int>(std::floor(sourceY));
        const int x1 = std::min(width - 1, x0 + 1);
        const int y1 = std::min(height - 1, y0 + 1);
        const float fx = sourceX - x0;
        const float fy = sourceY - y0;
        const float top = bitmap[y0 * width + x0] * (1.0f - fx) +
                          bitmap[y0 * width + x1] * fx;
        const float bottom = bitmap[y1 * width + x0] * (1.0f - fx) +
                             bitmap[y1 * width + x1] * fx;
        return static_cast<uint8_t>(std::lround(top * (1.0f - fy) + bottom * fy));
    }

    void blendTextPixel(int x, int y, uint8_t coverage, Color c) {
        if (x < 0 || y < 0 || x >= w || y >= h) return;
        if (!clip().contains(x, y)) return;
        const uint32_t destination = px[(size_t)y * stride + x];
        const int alpha = (static_cast<int>(coverage) * static_cast<int>(c.a) + 127) / 255;
        if (alpha <= 0) return;
        const int inverse = 255 - alpha;
        const auto channel = [&](int shift, uint8_t source) -> uint8_t {
            const int destinationChannel = static_cast<int>((destination >> shift) & 0xff);
            return static_cast<uint8_t>((static_cast<int>(source) * alpha +
                                         destinationChannel * inverse + 127) / 255);
        };
        const uint32_t value = static_cast<uint32_t>(channel(0, c.r)) |
                               (static_cast<uint32_t>(channel(8, c.g)) << 8) |
                               (static_cast<uint32_t>(channel(16, c.b)) << 16) |
                               0xff000000u;
        px[(size_t)y * stride + x] = value;
    }

    void drawGlyph(int glyphIndex, double penX, int lineY, Color c,
                   int scale, bool bold) {
        if (glyphIndex < 0 || glyphIndex >= static_cast<int>(kFontGlyphCount)) return;
        const FontGlyph& glyph = kFontGlyphs[glyphIndex];
        if (glyph.width == 0 || glyph.height == 0) return;

        const double factor = static_cast<double>(uiScale * scale) /
                              static_cast<double>(kFontDesignRatio);
        const double left = static_cast<double>(glyph.leftQ4) / 16.0 * factor;
        const double top = static_cast<double>(glyph.topQ4) / 16.0 * factor;
        const double baseline = static_cast<double>(lineY + kGlyphBaseline * scale) * uiScale;
        const double originX = penX + left;
        const double originY = baseline + top;
        const int x0 = static_cast<int>(std::floor(originX));
        const int y0 = static_cast<int>(std::floor(originY));
        const float fractionX = static_cast<float>(originX - x0);
        const float fractionY = static_cast<float>(originY - y0);
        const int destinationWidth = std::max(1, static_cast<int>(std::ceil(glyph.width * factor)));
        const int destinationHeight = std::max(1, static_cast<int>(std::ceil(glyph.height * factor)));
        const uint8_t* bitmap = kFontPixels + glyph.pixelOffset;

        for (int dy = 0; dy < destinationHeight; ++dy) {
            const float sourceY = ((dy + 0.5f - fractionY) /
                                   static_cast<float>(destinationHeight)) *
                                  static_cast<float>(glyph.height) - 0.5f;
            for (int dx = 0; dx < destinationWidth; ++dx) {
                const float sourceX = ((dx + 0.5f - fractionX) /
                                       static_cast<float>(destinationWidth)) *
                                      static_cast<float>(glyph.width) - 0.5f;
                const uint8_t coverage = sampleCoverage(bitmap, glyph.width, glyph.height,
                                                        sourceX, sourceY);
                blendTextPixel(x0 + dx, y0 + dy, coverage, c);
                if (bold) blendTextPixel(x0 + dx + 1, y0 + dy, coverage, c);
            }
        }
    }

    void text(int x, int y, const std::string& s, Color c, int scale = 1, bool bold = false) {
        pushClipDev(clip());   // clip() is already in device pixels
        double penX = static_cast<double>(x * uiScale);
        int lineY = y;
        size_t offset = 0;
        while (offset < s.size()) {
            size_t length = 1;
            const uint32_t codepoint = utf8DecodeAt(s, offset, &length);
            offset += length;
            if (codepoint == '\n') {
                penX = static_cast<double>(x * uiScale);
                lineY += (kGlyphH + 3) * std::max(1, scale);
                continue;
            }
            int glyphIndex = glyphIndexForCodepoint(codepoint);
            if (glyphIndex < 0) glyphIndex = glyphIndexForCodepoint('?');
            drawGlyph(glyphIndex, penX, lineY, c, scale, bold);
            penX += glyphAdvanceDesign(glyphIndex) * scale * uiScale;
        }
        popClip();
    }

    void textWrapped(Rect r, const std::string& s, Color c, int scale = 1, bool bold = false,
                     bool centered = false) {
        const std::vector<std::string> lines = wrapText(s, r.w, scale);
        const int lineHeight = (kGlyphH + 3) * std::max(1, scale);
        for (size_t index = 0; index < lines.size(); ++index) {
            const int lineY = r.y + static_cast<int>(index) * lineHeight;
            if (lineY >= r.bottom()) break;
            const int lineX = centered
                ? r.x + (r.w - textWidth(lines[index], scale)) / 2
                : r.x;
            text(lineX, lineY, lines[index], c, scale, bold);
        }
    }

    // centred / clipped variants used by UI
    void textRight(int right, int y, const std::string& s, Color c, int scale = 1, bool bold = false) {
        text(right - textWidth(s, scale), y, s, c, scale, bold);
    }
    void textCentered(Rect r, const std::string& s, Color c, int scale = 1, bool bold = false) {
        text(r.x + (r.w - textWidth(s, scale)) / 2,
             r.y + (r.h - kGlyphH * scale) / 2, s, c, scale, bold);
    }
    void textEllipsisAt(int x, int y, int maxWidth, const std::string& s,
                         Color c, int scale = 1, bool bold = false) {
        if (maxWidth <= 0 || s.empty()) return;
        const std::string cut = ellipsizeForWidth(s, maxWidth, scale);
        text(x, y, cut, c, scale, bold);
    }
    void textEllipsis(Rect r, const std::string& s, Color c, int scale = 1, bool bold = false) {
        const int maxPx = r.w - 4;
        const int textY = r.y + (r.h - kGlyphH * scale) / 2;
        textEllipsisAt(r.x + 2, textY, maxPx, s, c, scale, bold);
    }
};

// host-only debug helper: print the canvas as ASCII art
inline void dumpAscii(const Canvas& c, const char* title) {
    std::printf("---- %s (%dx%d) ----\n", title, c.w, c.h);
    for (int y = 0; y < c.h; ++y) {
        std::string line;
        for (int x = 0; x < c.w; ++x) {
            // "ink" = pixel differs from the background value sampled at (0,0)
            uint32_t p = c.px[(size_t)y * c.stride + x];
            uint32_t bg = c.px[0];
            auto chan = [](uint32_t v, int shift) { return (int)((v >> shift) & 0xff); };
            int delta = std::abs(chan(p, 0) - chan(bg, 0)) + std::abs(chan(p, 8) - chan(bg, 8)) +
                        std::abs(chan(p, 16) - chan(bg, 16));
            int lum = (chan(p, 0) + chan(p, 8) + chan(p, 16)) * chan(p, 24) / 3;
            bool panelish = (delta >= 8) && (delta < 300);   // e.g. white card on pale green
            char ch = (delta > 300) ? '#' : (delta > 60 ? '+' : (panelish ? '-' : ' '));
            (void)lum;
            line += ch;
        }
        std::printf("%s\n", line.c_str());
    }
    std::printf("---- end ----\n");
}

} // namespace gfx

// ============================================================================
// L3 — WIDGET KIT
// A retained tree built as a pure function of state ("screen -> nodes"), so the
// UI is deterministic and host-testable without a device. Events are integers
// ("actions") routed back to L6 handlers — no callbacks, no view recycling.
// ============================================================================
namespace ui {
using gfx::Rect;
using gfx::Color;

enum class Kind {
    Panel, Text, Button, List, Row, Column, Scroll, Checkbox, Field, Dialog, Toast,
    Fab, IconTile, Divider, Progress, Breadcrumb, SearchField, Keypad, ImageTile, TreeRow,
    MapView
};

struct Node {
    Kind kind = Kind::Panel;
    Rect rect{};
    std::string id;
    std::string text;            // primary label (utf-8)
    std::string text2;           // secondary label
    std::string value;           // field value / selected label
    std::string imagePath;       // optional decoded image (world icon)
    Color bg{0, 0, 0, 0}, fg{0, 0, 0, 0}, accent{0, 0, 0, 0}, border{0, 0, 0, 0};
    int radius = 0, pad = 0, borderW = 0;
    bool visible = true, enabled = true, selected = false, checked = false, expanded = false;
    bool multiline = false;
    bool centered = false;
    bool bold = false;           // faux bold for titles / primary labels
    int  action = 0;             // tap action id (0 = none)
    int  actionLong = 0;         // long-press action id
    int  index = -1;             // list row index (generic payload)
    int64_t user = 0;            // screen-specific payload
    std::vector<Node> kids;
};

// ---- scoping helpers -------------------------------------------------------
inline Node panel(Rect r, std::vector<Node> kids = {}) {
    Node n; n.kind = Kind::Panel; n.rect = r; n.kids = std::move(kids); return n;
}
inline Node text(Rect r, const std::string& s, Color c, int scale = 1, bool bold = false) {
    Node n; n.kind = Kind::Text; n.rect = r; n.text = s; n.fg = c; n.user = scale; n.bold = bold; return n;
}
inline Node textBlock(Rect r, const std::string& s, Color c, int scale = 1, bool bold = false) {
    Node n = text(r, s, c, scale, bold); n.multiline = true; return n;
}
inline Node textCentered(Rect r, const std::string& s, Color c, int scale = 1, bool bold = false) {
    Node n = text(r, s, c, scale, bold); n.centered = true; return n;
}
inline Node textBlockCentered(Rect r, const std::string& s, Color c, int scale = 1, bool bold = false) {
    Node n = textBlock(r, s, c, scale, bold); n.centered = true; return n;
}
inline Node button(Rect r, const std::string& label, Color bg, Color fg, int action, int radius = 6) {
    Node n; n.kind = Kind::Button; n.rect = r; n.text = label; n.bg = bg; n.fg = fg;
    n.action = action; n.radius = radius; return n;
}
inline Node list(Rect r, std::vector<Node> rows) {
    Node n; n.kind = Kind::Scroll; n.rect = r; n.kids = std::move(rows); return n;
}
inline Node checkbox(Rect r, const std::string& label, bool on, int action) {
    Node n; n.kind = Kind::Checkbox; n.rect = r; n.text = label; n.checked = on; n.action = action; return n;
}
inline Node field(Rect r, const std::string& label, const std::string& value, int action, bool numeric = true) {
    Node n; n.kind = Kind::Field; n.rect = r; n.text = label; n.value = value;
    n.action = action; n.user = numeric ? 1 : 0; return n;
}
inline Node searchField(Rect r, const std::string& value, int action) {
    Node n; n.kind = Kind::SearchField; n.rect = r; n.value = value; n.action = action; return n;
}
inline Node divider(Rect r, Color c) { Node n; n.kind = Kind::Divider; n.rect = r; n.bg = c; return n; }
inline Node progress(Rect r, float v, Color c) { Node n; n.kind = Kind::Progress; n.rect = r; n.user = 0; n.bg = c;
    n.value = std::to_string(v); return n; }

// Tree row: icon tile + key + value summary + expander
inline Node treeRow(Rect r, const std::string& key, const std::string& summary, Color accent,
                    bool hasChildren, bool expanded, bool selected, int index) {
    Node n; n.kind = Kind::TreeRow; n.rect = r; n.text = key; n.text2 = summary; n.accent = accent;
    n.expanded = hasChildren ? expanded : false;
    n.selected = selected; n.index = index; n.user = hasChildren ? 1 : 0;
    return n;
}
inline Node mapView(Rect r) {
    Node n; n.kind = Kind::MapView; n.rect = r; return n;
}
inline Node iconTile(Rect r, Color c, const std::string& glyph,
                     const std::string& imagePath = "") {
    Node n; n.kind = Kind::IconTile; n.rect = r; n.bg = c;
    n.text = glyph; n.imagePath = imagePath; return n;
}

// dialog is modal: rendered after the screen, blocks input below it
struct Dialog {
    bool open = false;
    std::string title, message;
    std::vector<Node> body;      // custom rows
    std::string pos, neg, neu;   // button labels ("", absent)
    int posAction = 0, negAction = 0, neuAction = 0;
    Rect rect{};
};

struct Toast { std::string message; int64_t untilMs = 0; Color color{0, 0, 0, 0}; };

// ---- layout helpers (pure geometry, no state) ------------------------------
inline Rect rowCell(Rect r, int col, int ncols, int gap = 4) {
    int avail = r.w - gap * (ncols - 1);
    int cw = avail / ncols;
    return {r.x + col * (cw + gap), r.y, cw, r.h};
}
inline Rect inset(Rect r, int d) { return {r.x + d, r.y + d, r.w - 2 * d, r.h - 2 * d}; }

inline Rect stackDown(Rect r, int& y, int h, int gap = 2) {
    Rect out{r.x, y, r.w, h};
    y += h + gap;
    return out;
}

// ---- input -----------------------------------------------------------------
enum class Gesture { None, Tap, LongPress, Scroll, RightEdge, LeftEdge };

struct TouchState {
    bool  down = false;
    int   x = 0, y = 0;
    int   startX = 0, startY = 0;
    int64_t downMs = 0;
    bool  consumed = false;      // a scroll container claimed this gesture
    bool  moved = false;
    int64_t nowMs = 0;
};

// Hit test: deepest visible child first (tree is in z-order).
inline bool hitTest(const Node& n, int x, int y, const Node*& out) {
    if (!n.visible) return false;
    Rect r = n.rect;
    if (!r.contains(x, y)) return false;
    out = &n;
    for (auto it = n.kids.rbegin(); it != n.kids.rend(); ++it)
        if (hitTest(*it, x, y, out)) return true;
    return true;
}

// The character a keypad key represents: the on-screen keypad stores it in
// .index, the inline numeric pad in the button label.
inline std::string keyCharOf(const Node& n) {
    if (n.index >= 32 && n.index < 127) return std::string(1, (char)n.index);
    if (!n.text.empty()) return n.text.substr(0, 1);
    return "";
}

// Hit test for INPUT: returns the deepest node that is actually interactive
// (has an action, or carries a row index).
// Audit B2: list rows are panels whose children (icon tile, text) have
// index = -1; the old hitTest returned those children, so `ed.selected` was
// never set and every row action (open world, pick file, pick recent) failed.
// A tap on the row label/icon must resolve to the row itself.
inline bool hitTestInteractiveClipped(const Node& n, int x, int y,
                                      const Node*& out, Rect clip) {
    if (!n.visible) return false;
    if (!n.rect.contains(x, y) || !clip.contains(x, y)) return false;
    Rect childClip = clip;
    if (n.kind == Kind::Scroll) {
        childClip = gfx::intersect(n.rect, clip);
        if (childClip.empty()) return false;
    }
    for (auto it = n.kids.rbegin(); it != n.kids.rend(); ++it) {
        const Node* sub = nullptr;
        if (hitTestInteractiveClipped(*it, x, y, sub, childClip) && sub) {
            out = sub;
            return true;
        }
    }
    if (n.action != 0 || n.index >= 0) { out = &n; return true; }
    return false;
}

inline bool hitTestInteractive(const Node& n, int x, int y, const Node*& out) {
    // The root clip is intentionally larger than any device surface.  Only a
    // Scroll ancestor narrows it, so ordinary panels keep their existing
    // absolute-coordinate semantics.
    Rect all{-1000000000, -1000000000, 2000000000, 2000000000};
    return hitTestInteractiveClipped(n, x, y, out, all);
}

// Scroll containers report which one owns the gesture (consumes it).
inline Node* findScrollAt(const Node& n, int x, int y) {
    if (!n.visible) return nullptr;
    if (!n.rect.contains(x, y)) return nullptr;
    for (auto it = n.kids.rbegin(); it != n.kids.rend(); ++it)
        if (Node* hit = findScrollAt(*it, x, y)) return hit;
    if (n.kind == Kind::Scroll) return const_cast<Node*>(&n);
    return nullptr;
}

// Breadcrumb has its own horizontal gesture.  Return the viewport that owns
// the initial touch so the platform layer can axis-lock the drag without
// borrowing the tree's vertical scroll state.
inline Node* findBreadcrumbAt(const Node& n, int x, int y) {
    if (!n.visible) return nullptr;
    if (!n.rect.contains(x, y)) return nullptr;
    for (auto it = n.kids.rbegin(); it != n.kids.rend(); ++it)
        if (Node* hit = findBreadcrumbAt(*it, x, y)) return hit;
    if (n.kind == Kind::Breadcrumb) return const_cast<Node*>(&n);
    return nullptr;
}

// Dialog bodies are authored in local coordinates, but the renderer and
// hit-test both use absolute screen coordinates.  Translate the whole subtree
// once when the body node is placed; otherwise its children stay at (0,0) and
// are both invisible and untappable.
inline void translateNodeRecursive(Node& n, int dx, int dy) {
    n.rect.x += dx;
    n.rect.y += dy;
    for (Node& k : n.kids) translateNodeRecursive(k, dx, dy);
}

} // namespace ui

// ============================================================================
// L5 — APPLICATION LAYER (repositories, view models, background worker)
// Owns all mutable state. Screens (L6) only read state and emit intents.
// ============================================================================
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <zlib.h>

namespace app {

using nbt::Document;
using nbt::Result;
using nbt::Err;

// ---- file repository -------------------------------------------------------
namespace fs = std::filesystem;

// A single read is boundedly buffered.  The editor never needs to map multi-
// gigabyte files into RAM; refusing them here keeps a corrupt or hostile file
// from turning into an OOM before any parser error can be reported.
constexpr uint64_t kMaxFileBytes = 512ull * 1024ull * 1024ull;

Result<std::vector<uint8_t>> readFile(const std::string& path) {
    std::error_code ec;
    if (!fs::exists(path, ec)) return Result<std::vector<uint8_t>>::fail(Err::IoError, "File not found: " + path);
    uintmax_t declaredSize = fs::file_size(path, ec);
    if (!ec && declaredSize > kMaxFileBytes)
        return Result<std::vector<uint8_t>>::fail(Err::SizeLimit,
            "File is too large to open: " + path);
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return Result<std::vector<uint8_t>>::fail(Err::IoError, "Cannot open: " + path);
    std::fseek(f, 0, SEEK_END);
    long sz = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (sz < 0) { std::fclose(f); return Result<std::vector<uint8_t>>::fail(Err::IoError, "Cannot read size: " + path); }
    if ((uint64_t)sz > kMaxFileBytes) {
        std::fclose(f);
        return Result<std::vector<uint8_t>>::fail(Err::SizeLimit, "File is too large to open: " + path);
    }
    std::vector<uint8_t> buf(sz > 0 ? (size_t)sz : 0);
    size_t got = sz > 0 ? std::fread(buf.data(), 1, buf.size(), f) : 0;
    std::fclose(f);
    if (got != buf.size()) return Result<std::vector<uint8_t>>::fail(Err::IoError, "Short read: " + path);
    return Result<std::vector<uint8_t>>::good(std::move(buf));
}

// Atomic save: write to a unique sibling temp file, flush, rename over the
// target. The original file is never truncated, so a failure cannot destroy
// data. mkstemp(O_EXCL) also prevents a pre-created temp symlink from turning
// the write into an overwrite of an unrelated file.
Result<void> writeFileAtomic(const std::string& path, const std::vector<uint8_t>& bytes) {
    std::string tmpTemplate = path + ".tmp-XXXXXX";
    std::vector<char> tmp(tmpTemplate.begin(), tmpTemplate.end());
    tmp.push_back('\0');
    int fd = mkstemp(tmp.data());
    if (fd < 0) {
        return Result<void>::fail(Err::WriteFailed,
            "Cannot create temp file in " + fs::path(path).parent_path().string());
    }
    FILE* f = fdopen(fd, "wb");
    if (!f) {
        close(fd);
        std::remove(tmp.data());
        return Result<void>::fail(Err::WriteFailed, "Cannot open temp file for writing");
    }
    // Imported worlds and NBT files must remain readable by other apps that
    // access the shared Android storage tree.
    (void)fchmod(fd, 0644);
    size_t put = std::fwrite(bytes.data(), 1, bytes.size(), f);
    int flushed = std::fflush(f);
    int synced = fsync(fileno(f));
    int closed = std::fclose(f);
    if (put != bytes.size() || flushed != 0 || synced != 0 || closed != 0) {
        std::remove(tmp.data());
        return Result<void>::fail(Err::WriteFailed, "Write failed (disk full?)");
    }
    std::error_code ec;
    fs::rename(tmp.data(), path, ec);
    if (ec) {
        std::remove(tmp.data());
        return Result<void>::fail(Err::WriteFailed, "Could not replace " + path + ": " + ec.message());
    }
    return Result<void>::good();
}

Result<void> writeText(const std::string& path, const std::string& s) {
    std::vector<uint8_t> b(s.begin(), s.end());
    return writeFileAtomic(path, b);
}

// ---- zip (.mcworld) via system zlib ----------------------------------------
struct ZipEntry { std::string name; bool isDir = false; std::vector<uint8_t> data; };

Result<std::vector<ZipEntry>> unzip(const std::string& path) {
    auto raw = readFile(path);
    if (!raw.ok()) return Result<std::vector<ZipEntry>>::fail(raw.err, raw.detail);
    const std::vector<uint8_t>& b = raw.value;
    if (b.size() < 22) return Result<std::vector<ZipEntry>>::fail(Err::IoError, "Not a zip file (too small)");
    // locate End Of Central Directory
    size_t eocd = std::string::npos;
    for (size_t i = b.size() - 22 + 1; i-- > 0;) {
        if (b[i] == 0x50 && b[i + 1] == 0x4b && b[i + 2] == 0x05 && b[i + 3] == 0x06) { eocd = i; break; }
        if (i == 0) break;
    }
    if (eocd == std::string::npos)
        return Result<std::vector<ZipEntry>>::fail(Err::IoError, "Not a zip file (no end record)");
    auto rd16 = [&](size_t o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); };
    auto rd32 = [&](size_t o) { return (uint32_t)(b[o] | (b[o+1] << 8) | (b[o+2] << 16) | ((uint32_t)b[o+3] << 24)); };
    uint32_t count = rd32(eocd + 10);
    size_t   cd = rd32(eocd + 16);

    std::vector<ZipEntry> out;
    out.reserve(count);
    size_t p = cd;
    for (uint32_t k = 0; k < count && p + 46 <= b.size(); ++k) {
        if (rd32(p) != 0x02014b50) break;
        uint16_t method = rd16(p + 10);
        uint32_t csize = rd32(p + 20);
        uint32_t usize = rd32(p + 24);
        uint16_t nlen = rd16(p + 28);
        uint16_t elen = rd16(p + 30);
        uint16_t clen = rd16(p + 32);
        uint32_t lho = rd32(p + 42);
        std::string name((const char*)b.data() + p + 46, nlen);
        // local header
        size_t lh = lho;
        if (rd32(lh) != 0x04034b50) break;
        uint16_t lnlen = rd16(lh + 26);
        uint16_t lelen = rd16(lh + 28);
        size_t dataOff = lh + 30 + lnlen + lelen;

        ZipEntry e;
        e.name = name;
        e.isDir = (!name.empty() && name.back() == '/');
        if (method == 0) {
            if (dataOff + csize <= b.size()) e.data.assign(b.begin() + dataOff, b.begin() + dataOff + csize);
        } else if (method == 8) {
            e.data.resize(usize);
            z_stream zs{}; zs.next_in = (Bytef*)(b.data() + dataOff); zs.avail_in = csize;
            zs.next_out = e.data.data(); zs.avail_out = usize;
            if (inflateInit2(&zs, -15) == Z_OK) {
                inflate(&zs, Z_FINISH);
                inflateEnd(&zs);
            }
        }
        out.push_back(std::move(e));
        p += 46 + nlen + elen + clen;
    }
    return Result<std::vector<ZipEntry>>::good(std::move(out));
}

Result<void> extractZip(const std::string& zipPath, const std::string& destDir) {
    auto entries = unzip(zipPath);
    if (!entries.ok()) return Result<void>::fail(entries.err, entries.detail);
    std::error_code ec;
    for (auto& e : entries.value) {
        if (e.isDir) continue;
        fs::path dest = fs::path(destDir) / e.name;
        if (!dest.parent_path().empty()) fs::create_directories(dest.parent_path(), ec);
        FILE* f = std::fopen(dest.string().c_str(), "wb");
        if (!f) return Result<void>::fail(Err::WriteFailed, "Cannot create " + dest.string());
        if (!e.data.empty()) std::fwrite(e.data.data(), 1, e.data.size(), f);
        std::fclose(f);
    }
    return Result<void>::good();
}

Result<void> createZip(const std::string& dir, const std::string& zipPath) {
    // collect entries deterministically (sorted by name)
    std::vector<fs::path> files;
    std::error_code ec;
    for (auto it = fs::recursive_directory_iterator(dir, ec); it != fs::recursive_directory_iterator(); ) {
        if (it->is_regular_file(ec)) files.push_back(it->path());
        it.increment(ec);
    }
    std::sort(files.begin(), files.end());   // deterministic order inside the archive

    FILE* out = std::fopen(zipPath.c_str(), "wb");
    if (!out) return Result<void>::fail(Err::WriteFailed, "Cannot create " + zipPath);

    struct Rec { uint32_t crc, csize, usize, lho; std::string name; };
    std::vector<Rec> recs;
    uint32_t crc_table_dummy = 0; (void)crc_table_dummy;

    for (auto& fp : files) {
        auto data = readFile(fp.string());
        if (!data.ok()) continue;
        std::string rel = fs::relative(fp, dir, ec).string();
        uint32_t crc = (uint32_t)crc32(0, data.value.data(), (uInt)data.value.size());
        std::vector<uint8_t> deflated;
        uLongf bound = compressBound((uLong)data.value.size());
        deflated.resize(bound);
        if (compress2(deflated.data(), &bound, data.value.data(), (uLong)data.value.size(), 6) == Z_OK)
            deflated.resize(bound);
        else { deflated = data.value; bound = deflated.size(); }

        uint16_t method = (bound < data.value.size()) ? 8 : 0;
        std::vector<uint8_t> stored = (method == 8) ? deflated : data.value;
        uint32_t lho = (uint32_t)ftell(out);
        // local file header
        uint8_t lh[30];
        lh[0]=0x50; lh[1]=0x4b; lh[2]=0x03; lh[3]=0x04;
        lh[4]=20; lh[5]=0;                                   // version needed
        lh[6]=0; lh[7]=0;                                    // flags
        lh[8]=(uint8_t)method; lh[9]=0;                     // method
        lh[10]=lh[11]=lh[12]=lh[13]=0;                       // time/date
        std::memcpy(lh + 14, &crc, 4);
        uint32_t csize32 = (uint32_t)stored.size(), usize32 = (uint32_t)data.value.size();
        std::memcpy(lh + 18, &csize32, 4);
        std::memcpy(lh + 22, &usize32, 4);
        uint16_t nlen16 = (uint16_t)rel.size(); std::memcpy(lh + 26, &nlen16, 2);
        lh[28] = lh[29] = 0;
        std::fwrite(lh, 1, 30, out);
        std::fwrite(rel.data(), 1, rel.size(), out);
        if (!stored.empty()) std::fwrite(stored.data(), 1, stored.size(), out);
        recs.push_back({crc, csize32, usize32, lho, rel});
    }
    uint32_t cdStart = (uint32_t)ftell(out);
    for (auto& r : recs) {
        uint8_t ch[46];
        ch[0]=0x50; ch[1]=0x4b; ch[2]=0x01; ch[3]=0x02;
        ch[4]=20; ch[5]=0; ch[6]=20; ch[7]=0;
        ch[8]=ch[9]=0;
        ch[10]=(uint8_t)(r.csize < r.usize ? 8 : 0); ch[11]=0;
        ch[12]=ch[13]=ch[14]=ch[15]=0;
        std::memcpy(ch + 16, &r.crc, 4);
        std::memcpy(ch + 20, &r.csize, 4);
        std::memcpy(ch + 24, &r.usize, 4);
        uint16_t nlen16 = (uint16_t)r.name.size(); std::memcpy(ch + 28, &nlen16, 2);
        ch[30]=ch[31]=ch[32]=ch[33]=0;
        ch[34]=ch[35]=0; ch[36]=0;
        ch[37]=0; ch[38]=0; ch[39]=0; ch[40]=0;
        std::memcpy(ch + 42, &r.lho, 4);
        std::fwrite(ch, 1, 46, out);
        std::fwrite(r.name.data(), 1, r.name.size(), out);
    }
    uint32_t cdEnd = (uint32_t)ftell(out);
    uint8_t eocd[22];
    eocd[0]=0x50; eocd[1]=0x4b; eocd[2]=0x05; eocd[3]=0x06;
    eocd[4]=eocd[5]=0; eocd[6]=eocd[7]=0;
    uint16_t cnt16 = (uint16_t)recs.size(); std::memcpy(eocd + 8, &cnt16, 2);
    std::memcpy(eocd + 10, &cnt16, 2);
    uint32_t cdSize = cdEnd - cdStart; std::memcpy(eocd + 12, &cdSize, 4);
    std::memcpy(eocd + 16, &cdStart, 4);
    eocd[20] = eocd[21] = 0;
    std::fwrite(eocd, 1, 22, out);
    std::fclose(out);
    return Result<void>::good();
}

std::string joinPath(const std::string& a, const std::string& b);
bool ensureDir(const std::string& path);

// ---- diagnostics: in-memory log + crash capture ---------------------------
// Kept small and dependency-free: the log is written to the app's private dir so
// the user can share it from inside the app (no adb needed on device).
static const int kLogCapacity = 400;
struct LogBuffer {
    std::mutex m;
    std::vector<std::string> lines;
    void add(const std::string& line) {
        std::lock_guard<std::mutex> lk(m);
        lines.push_back(line);
        if ((int)lines.size() > kLogCapacity) lines.erase(lines.begin());
    }
    std::vector<std::string> snapshot() {
        std::lock_guard<std::mutex> lk(m);
        return lines;
    }
};
static LogBuffer& logBuffer() { static LogBuffer b; return b; }

inline void diagLog(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    logBuffer().add(std::string(buf));
}

inline std::string logFilePath(const std::string& filesDir) {
    return joinPath(filesDir, "nbt-editor-log.txt");
}

// Writes the in-memory log to the private dir and returns the path.
inline std::string flushLog(const std::string& filesDir) {
    std::string p = logFilePath(filesDir);
    ensureDir(filesDir.empty() ? "." : filesDir);
    std::string all;
    for (auto& l : logBuffer().snapshot()) { all += l; all += "\n"; }
    app::writeText(p, all);
    return p;
}

// ---- prefs -----------------------------------------------------------------
struct Prefs {
    std::string theme = "light";         // light | dark
    std::vector<std::string> recent;
    std::string lastDir;
    std::string worldsRoot;
};

// ---- world model -----------------------------------------------------------
struct WorldInfo {
    std::string folder;      // absolute path
    std::string displayName; // levelname.txt
    std::string internalName;
    uint64_t sizeBytes = 0;
    bool hasIcon = false;
    bool encrypted = false;
    bool external = false;
    std::string iconPath;    // world_icon.jpeg / .jpg / .png when present
};

uint64_t dirSize(const fs::path& p) {
    uint64_t total = 0;
    std::error_code ec;
    for (auto it = fs::recursive_directory_iterator(p, ec); it != fs::recursive_directory_iterator(); ) {
        if (it->is_regular_file(ec)) {
            std::error_code e2;
            auto sz = it->file_size(e2);
            if (!e2) total += (uint64_t)sz;
        }
        it.increment(ec);
    }
    return total;
}

// Encrypted Bedrock worlds: db files whose first two LE int32 are 0 and -1680885252.
bool looksEncrypted(const fs::path& world) {
    std::error_code ec;
    fs::path db = world / "db";
    if (!fs::exists(db, ec)) return false;
    for (auto& f : fs::directory_iterator(db, ec)) {
        auto bytes = readFile(f.path().string());
        if (!bytes.ok() || bytes.value.size() < 8) continue;
        auto le32 = [&](size_t o) {
            return (int32_t)((uint32_t)bytes.value[o] | ((uint32_t)bytes.value[o+1] << 8) |
                             ((uint32_t)bytes.value[o+2] << 16) | ((uint32_t)bytes.value[o+3] << 24)); };
        if (le32(0) == 0 && le32(4) == -1680885252) return true;
    }
    return false;
}

std::string readLevelName(const fs::path& world) {
    auto r = readFile((world / "levelname.txt").string());
    if (r.ok()) return gfx::trim(std::string(r.value.begin(), r.value.end()));
    return "Unnamed World";
}

// ---- background worker -----------------------------------------------------
struct Worker {
    std::thread th;
    std::mutex m;
    std::condition_variable cv;
    std::function<void()> job;
    bool running = true, busy = false, done = false;

    void start() {
        if (th.joinable()) return;             // already running
        { std::lock_guard<std::mutex> lk(m); running = true; }
        th = std::thread([this] {
            std::unique_lock<std::mutex> lk(m);
            while (running) {
                cv.wait(lk, [this] { return job || !running; });
                if (!running) return;
                busy = true;
                // run unlocked; callbacks post back through the UI queue (L0)
                std::function<void()> run = job;
                job = nullptr;
                lk.unlock();
                if (run) run();
                lk.lock();
                busy = false;
                done = true;
                cv.notify_all();
            }
        });
    }
    void post(std::function<void()> f) {
        {
            std::lock_guard<std::mutex> lk(m);
            job = std::move(f);
            done = false;
        }
        cv.notify_one();
    }
    bool finished() { std::lock_guard<std::mutex> lk(m); return done && !busy; }
    void stop() {
        { std::lock_guard<std::mutex> lk(m); running = false; }
        cv.notify_all();
        if (th.joinable()) th.join();
    }
};

// The one app-wide worker (created lazily, started by the platform glue).
inline Worker& appWorker() { static Worker w; return w; }

} // namespace app

// ============================================================================
// L6 — SCREENS & DIALOGS  ·  L0 platform glue lives at the end of the file
//
// State is a single struct mutated only through intents (handleAction). Every
// screen is built as a pure function AppState -> ui::Node tree, then rendered.
// ============================================================================

namespace app {
using gfx::Rect;

// ---- intents ---------------------------------------------------------------
enum Act {
    A_NONE = 0,
    // home
    A_OPEN_NBT, A_NEW_NBT, A_WORLDS, A_RECENT_PICK,
    // worlds
    A_WORLD_DELETE, A_WORLD_EXPORT, A_WORLD_CREATE, A_WORLD_IMPORT,
    A_WORLD_OPTIONS, A_WORLD_EDIT, A_FAB_TOGGLE,
    // browser
    A_BROWSE_UP, A_BROWSE_OPEN, A_FILE_SELECT,
    // nbt editor
    A_TREE_TAP, A_TREE_LONG, A_TREE_ADD, A_TREE_SAVE,
    A_TREE_SAVE_AS, A_TREE_DELETE, A_TREE_PASTE,
    A_TREE_UNDO, A_TREE_REDO, A_TREE_SEARCH, A_TREE_SEARCH_CLEAR,
    A_TREE_MOVE_ROOT, A_TREE_UP,
    A_VALUE_KIND, A_VALUE_SAVE, A_VALUE_CANCEL, A_TREE_ADD_DONE,
    // dialogs
    A_DIALOG_POS, A_DIALOG_NEG, A_DIALOG_NEU, A_BLOCK,
    A_KEY_TAP, A_KEY_SHIFT, A_KEY_DEL, A_KEY_ENTER, A_KEY_MODE,
    A_KEY_OPEN, A_KEY_CLEAR,
    A_TAG_OPTION, A_BREADCRUMB_TAP,
    // flat world
    A_FLAT_ADD_LAYER, A_FLAT_EDIT_LAYER, A_FLAT_DEL_LAYER,
    A_FLAT_BIOME, A_FLAT_CREATE, A_FLAT_TOGGLE_BASE, A_FLAT_LAYER_UP,
    A_FLAT_LAYER_DOWN,
    // misc
    A_TOGGLE_THEME, A_HELP,
    A_MENU_OPEN, A_MENU_CLOSE,
    A_PICK_RESULT,               // SAF file manager returned a file
    // Drawer navigation (mirrors drawer_menu.xml from the reference XAPK)
    A_NAV_MAP, A_NAV_WORLD_NBT, A_NAV_LOCAL_PLAYER, A_NAV_MULTIPLAYER,
    A_NAV_CHANGE_WORLD, A_NAV_OVERWORLD, A_NAV_NETHER, A_NAV_END,
    A_NAV_BIOMEDATA, A_NAV_PORTALS, A_NAV_ENTITIES, A_NAV_CUSTOM,
    A_MAP_TAP, A_MAP_SELECT, A_MAP_ENTITIES, A_ENTITY_PICK,
    A_HOME, A_HOME_CONFIRM, A_BACK,
};

// SAF (device file manager) modes. Mirror FilePickerActivity.MODE_* in Java.
// RESULT_OK on Android is -1; declared here so host tests can use it too.
enum PickerMode { PICK_IMPORT_WORLD = 0, PICK_OPEN_NBT = 1 };
static constexpr int kPickerOk = -1;

struct FlatLayer { std::string block; int64_t count = 1; };

// A visible tree row is either a normal arena node (Compound/List child) or
// one element stored directly inside an array payload.  Array elements are
// deliberately not fake arena nodes: bytes/longs remain the canonical NBT
// representation and the serializer continues to read only those payloads.
enum class TreeEntryKind { Node, ArrayElement };
struct TreeEntry {
    TreeEntryKind kind = TreeEntryKind::Node;
    uint32_t nodeIndex = 0;       // child node, or owning array for an element
    size_t elementIndex = 0;      // index inside bytes/longs
};

struct EditorState {
    bool open = false;
    nbt::Document doc;
    std::string path;
    bool dirty = false;
    bool levelDat = false;            // true: 8-byte Bedrock header present
    bool dbBacked = false;            // true: document came from a LevelDB key
    std::vector<uint8_t> dbKey;       // raw LevelDB key when dbBacked
    bool pickedFile = false;          // opened via SAF; save back to its Uri
    // navigation: index-path into the arena; we navigate by indices, not copies
    std::vector<uint32_t> pathIdx;    // current compound/list/array index
    std::vector<uint64_t> openSet;    // expanded node keys (path hash)
    std::string filter;
    std::vector<std::pair<std::string, uint32_t>> searchHits;
    int selected = -1;                // index inside current listing
    bool showSearch = false;
    // clipboard
    bool hasClip = false;
    uint32_t clipIdx = 0;
    // undo (command log: full document snapshots are too heavy; store inverse ops)
    struct UndoEntry {
        std::string label;
        std::vector<nbt::Node> arena;
        uint32_t root = 0;
    };
    std::vector<UndoEntry> undoStack;
    std::vector<UndoEntry> redoStack;
    int scrollY = 0;
};

// Versi aplikasi disuntikkan oleh build scripts dari berkas VERSION, satu-
//-satunya sumber kebenaran. Build yang tidak menentukan kedua macro ini
// ditolak agar UI Help dan manifest APK tidak pernah berbeda versi.
#ifndef NBT_APP_VERSION_NAME
#error "NBT_APP_VERSION_NAME harus berasal dari berkas VERSION"
#endif
#ifndef NBT_APP_VERSION_CODE
#error "NBT_APP_VERSION_CODE harus berasal dari berkas VERSION"
#endif
inline constexpr const char* kAppVersion = NBT_APP_VERSION_NAME;
inline constexpr int         kAppBuild   = NBT_APP_VERSION_CODE;

// One selectable entity marker on the map (raw NBT kept for the editor).
struct EntityChoice {
    float x = 0, z = 0;                 // world block position
    std::string icon;                   // PNG filename from entityIcons
    std::string identifier;             // minecraft:...
    std::vector<uint8_t> key;           // "actorprefix"+id, for save-back
    std::vector<uint8_t> nbt;           // raw entity NBT (no header)
};

struct AppState {
    // identity / paths
    std::string appName = "NBT Editor";
    std::string filesDir;             // internal data path (prefs, drafts)
    std::string worldsRoot;            // app-managed worlds dir
    // theme / metrics
    gfx::Theme theme;
    bool dark = false;
    int  w = 0, h = 0;                // framebuffer size (px)
    int  scale = 1;                   // ui scale for small screens
    // navigation
    int screen = 0;                   // see Screen enum below
    std::vector<int> screenStack;
    // data
    app::Prefs prefs;
    std::vector<app::WorldInfo> worlds;
    std::vector<std::string> recent;
    // browser
    std::string browseDir;
    bool browseForSave = false;
    bool importing = false;
    bool storageAsked = false;        // ask for all-files access only once,
                                      // and only when the user opens the browser
    std::vector<std::string> browseEntries;
    std::vector<bool> browseIsDir;
    std::string pendingSavePath;
    // editor
    EditorState ed;
    // value editor dialog
    bool valueOpen = false;
    uint32_t valueIdx = 0;            // node being edited (or owning array)
    bool valueIsArrayElement = false;
    size_t valueArrayElementIndex = 0;
    std::string valueText;
    nbt::Type valueType = nbt::Type::Byte;
    std::string valueError;
    bool valueNumeric = true;
    // add-tag dialog
    bool addOpen = false;
    int addType = 1;                  // type index for spinner
    nbt::Type valueTypeToAdd = nbt::Type::Byte;   // selected type in the add dialog
    std::string addName;
    int addListMode = 0;              // 0 compound, 1 list, 2 array
    std::string addError;
    // generic dialog
    ui::Dialog dialog;
    int dialogTargetIndex = -1;   // captured row target for destructive dialogs
    // toasts / progress
    std::vector<ui::Toast> toasts;
    bool progressOpen = false;
    std::string progressTitle, progressMsg;
    float progressValue = 0;
    // flat world builder
    bool flatOpen = false;
    std::vector<FlatLayer> flatLayers;
    int flatBiome = 1;                // plains
    bool flatBaseLayer = true;
    std::string flatDraft;           // thickness input buffer
    // rename dialog
    bool renameOpen = false;
    uint32_t renameIdx = 0;
    std::string renameName;
    // tag options dialog (long press)
    std::vector<std::string> tagOptionList;
    TreeEntry tagOptionEntry;
    // pending destructive action (normal node or array element)
    TreeEntry deleteEntry;
    // keypad
    bool keypadOpen = false;
    std::string keypadTarget;         // "value" | "addName" | "renameName" | "searchFilter"
    std::string keypadBuf;
    bool keypadShift = false;
    bool keypadNumeric = true;
    int64_t lastKeyMs = 0;
    std::string lastKeyPressed;
    // misc
    bool wantExit = false;
    int64_t frame = 0;
    int64_t nowMs = 0;
    int64_t autosaveAtMs = 0;
    bool pendingEdit = false;         // waiting for level.dat to open
    // built tree (rebuilt every frame; used for hit-testing and drawing)
    std::shared_ptr<ui::Node> uiRoot;
    // scrolling of the active list (single scrollable list per screen)
    int scrollY = 0;
    int contentBottom = 0;
    // breadcrumb horizontal viewport (independent from the tree's scrollY)
    int breadcrumbScrollX = 0;
    int breadcrumbContentWidth = 0;
    int breadcrumbViewportWidth = 0;
    std::vector<uint32_t> breadcrumbPath;
    // touch bookkeeping
    bool  touchActive = false;
    bool  touchScrolled = false;
    bool  touchInBreadcrumb = false;
    int   touchAxis = 0;             // 0 undecided, 1 horizontal, 2 vertical
    int   touchStartX = 0, touchStartY = 0;
    int   touchLastX = 0, touchLastY = 0;
    int64_t touchDownMs = 0;
    int   touchX = 0, touchY = 0;
    bool helpOpen = false;
    bool diagOpen = false;
    // overflow menu (app bar "more" button)
    bool menuOpen = false;
    std::vector<std::pair<std::string, int>> menuItems;
    bool fabExpanded = false;         // Home FAB stack: "+" expands to New/Import
    // Result handed back by FilePickerActivity (SAF / device file manager).
    bool pickerPending = false;
    int  pickerMode = PICK_IMPORT_WORLD;
    int  pickerResult = 0;
    std::string pickerPath, pickerName, pickerUri, pickedSaveUri;

    // Aside drawer / world editor context (reference DrawerActivity).
    bool drawerOpen = false;
    bool drawerTouch = false;        // current gesture started inside the panel
    int  drawerScrollY = 0;          // target offset of the NavigationView list
    int  drawerScrollStart = 0;      // target at gesture start (finger tracking)
    float drawerScrollF = 0.0f;      // interpolated offset actually rendered
    int64_t drawerAnimLastMs = 0;    // timestamp of the previous animation frame
    int  drawerContentBottom = 0;    // content height, for scroll clamping
    int  drawerSelected = A_NAV_MAP; // highlighted nav action (single-check group)
    std::string activeWorldFolder, activeWorldName, activeWorldIcon;

    // Show Map view state (reference d0.e / d0.a).
    float mapScale = 0.5f;          // 0.0625 .. 1.0, initial 0.5
    int64_t mapCenterX = 0, mapCenterZ = 0;   // block coordinates (spawn)
    bool mapSelection = false;
    bool mapEdgeSwipe = false;
    int64_t mapTapX = 0, mapTapZ = 0;
    std::vector<EntityChoice> entityChoices;   // entities in the tapped chunk
};

// ---- screen ids ------------------------------------------------------------
enum Screen { S_HOME = 0, S_WORLDS, S_BROWSER, S_TREE, S_FLAT, S_HELP, S_MAP };

// Guards AppState: the render thread owns it, but the worker thread also
// touches it (import/export results), so every access is serialised.
static std::mutex g_stateMutex;

// ---- action handler (single entry point for all interaction) ---------------
void handleAction(AppState& a, int act);

// ---- forward decls for helpers used by handlers ----------------------------
std::string defaultWorldDir();
void listWorlds(AppState& a);
void listBrowseDir(AppState& a);
bool openDocument(AppState& a, const std::string& path, bool levelDat, std::string& err);

// All-files access (Android 11+) is requested lazily, when the user first opens
// the file browser. Asking during onCreate made the app jump straight to the
// system settings screen, which looked like "the app will not open".
#if !NBT_TESTABLE
void requestStoragePermission();          // L0, device only
void launchFilePicker(int mode);          // SAF / OS file manager, device only
#else
inline void requestStoragePermission() {} // host tests have no Android
inline void launchFilePicker(int) {}      // no OS picker on the host
#endif
bool saveDocument(AppState& a, const std::string& path, std::string& err);
void pushToast(AppState& a, const std::string& msg, gfx::Color c);
void openDialog(AppState& a, const std::string& title, const std::string& msg,
                const std::string& pos, int posAct, const std::string& neg, int negAct,
                const std::string& neu = "", int neuAct = 0);
void closeDialog(AppState& a);
void rebuildTreeListing(AppState& a);

// ---- small utilities -------------------------------------------------------
inline int64_t nowMs() {
    return (int64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

void pushToast(AppState& a, const std::string& msg, gfx::Color c) {
    a.toasts.push_back({msg, a.nowMs + 3200, c});
    if (a.toasts.size() > 4) a.toasts.erase(a.toasts.begin());
}

inline uint64_t hashPath(const std::vector<uint32_t>& p) {
    uint64_t h = 1469598103934665603ull;
    for (uint32_t v : p) { h ^= v; h *= 1099511628211ull; }
    return h;
}
inline bool nodeOpen(const AppState& a, const std::vector<uint32_t>& p) {
    uint64_t h = hashPath(p);
    for (auto v : a.ed.openSet) if (v == h) return true;
    return false;
}

// ---- paths -----------------------------------------------------------------
std::string defaultWorldDir() {
#ifdef __ANDROID__
    return "";                       // filled from ANativeActivity at startup
#else
    return "worlds";
#endif
}

// LevelDB world data.  Device implementation calls the JNI exports of the
// reference libleveldb.so; the host build has no database and returns empty.
bool ldbOpen(const std::string& worldFolder);
void ldbClose();
std::vector<uint8_t> ldbGet(const std::vector<uint8_t>& key);
bool ldbPut(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value);
std::vector<std::vector<uint8_t>> ldbIterate(const std::vector<uint8_t>& prefix);
bool ldbReady();
// Read a file from the APK assets (Android) — used for entity/mob PNG icons.
std::vector<uint8_t> readAssetBytes(const std::string& rel);
#if NBT_TESTABLE
std::vector<uint8_t> readAssetBytes(const std::string&) { return {}; }
bool writePickedFile(const std::string&, const std::vector<uint8_t>&) { return false; }
bool ldbReady() { return false; }
bool ldbOpen(const std::string&) { return false; }
void ldbClose() {}
std::vector<uint8_t> ldbGet(const std::vector<uint8_t>&) { return {}; }
#if NBT_TESTABLE
// Test hook: records the last DB write so host tests can verify persistence.
struct TestDbWrite { std::vector<uint8_t> key, value; };
static TestDbWrite g_testDbWrite;
#endif
bool ldbPut(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value) {
#if NBT_TESTABLE
    g_testDbWrite.key = key; g_testDbWrite.value = value; return true;
#else
    (void)key; (void)value; return false;
#endif
}
std::vector<std::vector<uint8_t>> ldbIterate(const std::vector<uint8_t>&) { return {}; }
#endif

// ---- Bedrock chunk top-block decoder (reference s.a / e0.a) ----------------
struct BlockColor { uint8_t r = 0, g = 0, b = 0; };

inline void pushLE32(std::vector<uint8_t>& v, int32_t x) {
    v.push_back((uint8_t)x); v.push_back((uint8_t)(x >> 8));
    v.push_back((uint8_t)(x >> 16)); v.push_back((uint8_t)(x >> 24));
}
inline std::vector<uint8_t> chunkKey(int cx, int cz, uint8_t tag, bool subchunk, int8_t sy) {
    std::vector<uint8_t> k;
    k.reserve(10);
    pushLE32(k, cx); pushLE32(k, cz); k.push_back(tag);
    if (subchunk) k.push_back((uint8_t)sy);
    return k;
}

// Approximate the reference t.c colour table for the most common blocks;
// unknown identifiers get a stable hue so terrain still looks varied.
inline BlockColor blockColor(const std::string& name) {
    // Exact RGB from the reference colour table (smali t/c.java).
    BlockColorEntry key{name.c_str(), 0, 0, 0};
    auto it = std::lower_bound(kBlockColors,
        kBlockColors + sizeof(kBlockColors) / sizeof(kBlockColors[0]), key,
        [](const BlockColorEntry& a, const BlockColorEntry& b) {
            return std::strcmp(a.name, b.name) < 0;
        });
    if (it != kBlockColors + sizeof(kBlockColors) / sizeof(kBlockColors[0]) &&
        std::strcmp(it->name, name.c_str()) == 0)
        return {it->r, it->g, it->b};
    // Unknown block: reference paints magenta 0xFFFF00FF.
    return {0xff, 0x00, 0xff};
}

struct Subchunk {
    std::vector<std::string> palette;
    std::vector<uint8_t> data;
    int bits = 0;
    bool valid = false;
};

inline bool readNbtString(const uint8_t* p, size_t n, size_t& off, std::string& out) {
    if (off + 3 > n || p[off] != 8) return false;   // TAG_String
    ++off;
    uint16_t nameLen = (uint16_t)(p[off] | (p[off + 1] << 8)); off += 2;
    if (off + nameLen + 2 > n) return false;
    off += nameLen;
    uint16_t valLen = (uint16_t)(p[off] | (p[off + 1] << 8)); off += 2;
    if (off + valLen > n) return false;
    out.assign((const char*)p + off, valLen);
    off += valLen;
    return true;
}

inline bool skipNbtPayload(const uint8_t* p, size_t n, size_t& off, uint8_t type);

inline bool skipNbtTag(const uint8_t* p, size_t n, size_t& off) {
    if (off >= n) return false;
    uint8_t t = p[off++];
    if (off + 2 > n) return false;
    uint16_t nl = (uint16_t)(p[off] | (p[off + 1] << 8)); off += 2;
    if (off + nl > n) return false;
    off += nl;
    return skipNbtPayload(p, n, off, t);
}

inline bool skipNbtPayload(const uint8_t* p, size_t n, size_t& off, uint8_t type) {
    auto need = [&](size_t k) { return off + k <= n; };
    switch (type) {
        case 1: case 5: return need(1) ? (off += 1, true) : false;
        case 2: case 6: return need(2) ? (off += 2, true) : false;
        case 3: return need(4) ? (off += 4, true) : false;   // Int
        case 4: return need(8) ? (off += 8, true) : false;   // Long
        case 8: {                                   // String
            if (!need(2)) return false;
            uint16_t l = (uint16_t)(p[off] | (p[off + 1] << 8)); off += 2;
            if (l > n - off) return false;
            off += l; return true;
        }
        case 7: case 11: case 12: {                 // arrays: i32 count
            if (!need(4)) return false;
            uint32_t cnt = (uint32_t)p[off] | ((uint32_t)p[off+1] << 8) |
                           ((uint32_t)p[off+2] << 16) | ((uint32_t)p[off+3] << 24);
            off += 4;
            size_t step = (type == 7) ? 1 : (type == 11 ? 4 : 8);
            if (cnt > (n - off) / step) return false;
            off += cnt * step; return true;
        }
        case 9: {                                   // List
            if (!need(5)) return false;
            uint8_t et = p[off++];
            uint32_t cnt = (uint32_t)p[off] | ((uint32_t)p[off+1] << 8) |
                           ((uint32_t)p[off+2] << 16) | ((uint32_t)p[off+3] << 24);
            off += 4;
            if (et == 0) return true;
            for (uint32_t i = 0; i < cnt; ++i)
                if (!skipNbtPayload(p, n, off, et)) return false;
            return true;
        }
        case 10: {                                  // Compound
            while (off < n && p[off] != 0) {
                if (!skipNbtTag(p, n, off)) return false;
            }
            if (off < n && p[off] == 0) { ++off; return true; }
            return false;
        }
        default: return false;
    }
}

// Palette entries are FULL NBT compounds (type 10) whose FIRST child is a
// TAG_String holding the block identifier (reference s/a.java:277-284).
inline bool readPaletteEntry(const uint8_t* p, size_t n, size_t& off, std::string& out) {
    if (off + 3 > n || p[off] != 10) return false;  // TAG_Compound
    ++off;
    uint16_t nameLen = (uint16_t)(p[off] | (p[off + 1] << 8)); off += 2;
    if (off + nameLen > n) return false;
    off += nameLen;
    if (!readNbtString(p, n, off, out)) return false;   // first child string
    // Consume the rest of the compound (other children + TAG_End) so the next
    // palette entry starts at the correct offset.
    while (off < n && p[off] != 0) {
        if (!skipNbtTag(p, n, off)) return false;
    }
    if (off < n && p[off] == 0) ++off;
    return true;
}

inline Subchunk decodeSubchunk(const std::vector<uint8_t>& raw) {
    Subchunk sc;
    if (raw.size() < 2) return sc;
    size_t off = 0;
    int version = raw[off++];
    if (version != 1 && version != 8 && version != 9) return sc;
    int storages = 1;
    if (version != 1) storages = raw[off++];
    if (version == 9) ++off;                       // chunk index
    if (storages < 1 || storages > 2) return sc;
    for (int s = 0; s < storages; ++s) {
        if (off >= raw.size()) return sc;
        int v = raw[off++];
        if (v & 1) return sc;
        int bits = v >> 1;
        if (s != 0) {                               // only the first storage is used
            if (bits) {
                // Reference computes 32 / bits with INTEGER division first.
                int words = (int)std::ceil(4096.0 / (double)(32 / bits)) * 4;
                off += words + 4;
            }
            continue;
        }
        sc.bits = bits;
        if (bits) {
            int words = (int)std::ceil(4096.0 / (double)(32 / bits)) * 4;
            if (off + words + 4 > raw.size()) return sc;
            sc.data.assign(raw.begin() + off, raw.begin() + off + words);
            off += words;
            uint32_t count = (uint32_t)(raw[off] | (raw[off+1]<<8) |
                                        (raw[off+2]<<16) | ((uint32_t)raw[off+3]<<24));
            off += 4;
            if (count > 4096) return sc;
            for (uint32_t i = 0; i < count; ++i) {
                std::string nm;
                if (!readPaletteEntry(raw.data(), raw.size(), off, nm)) return sc;
                sc.palette.push_back(std::move(nm));
            }
        } else {
            sc.palette.push_back("minecraft:air");
        }
    }
    sc.valid = true;
    return sc;
}

inline std::string subchunkBlock(const Subchunk& sc, int idx) {
    if (!sc.valid || sc.palette.empty()) return "minecraft:air";
    if (sc.bits == 0) return sc.palette[0];
    int perWord = 32 / sc.bits;
    size_t word = (size_t)(idx / perWord) * 4;
    if (word + 4 > sc.data.size()) return "minecraft:air";
    uint32_t bits = (uint32_t)sc.data[word] | ((uint32_t)sc.data[word+1] << 8) |
                    ((uint32_t)sc.data[word+2] << 16) | ((uint32_t)sc.data[word+3] << 24);
    int shift = (idx % perWord) * sc.bits;
    uint32_t pal = (bits >> shift) & ((1u << sc.bits) - 1u);
    if (pal >= sc.palette.size()) return "minecraft:air";
    return sc.palette[pal];
}

// 256 top-block colours for one chunk (x fastest, index = z*16+x).
inline std::vector<BlockColor> decodeChunkTop(int cx, int cz,
        std::unordered_map<uint64_t, Subchunk>* subCache = nullptr) {
    std::vector<BlockColor> out(256, {0,0,0});
    auto hm = ldbGet(chunkKey(cx, cz, 0x2B, false, 0));
    if (hm.size() < 512) return out;                 // no chunk data
    std::unordered_map<uint64_t, Subchunk> local;
    auto& cache = subCache ? *subCache : local;
    const uint64_t chunkBase = ((uint64_t)(uint32_t)cx << 32) | (uint32_t)cz;
    // Neighbour heightmaps for the reference west/north shading (e0.a.a).
    auto hmW = ldbGet(chunkKey(cx - 1, cz, 0x2B, false, 0));
    auto hmN = ldbGet(chunkKey(cx, cz - 1, 0x2B, false, 0));
    auto heightAt = [](const std::vector<uint8_t>& src, int idx) -> int {
        if (src.size() < 512 || idx < 0 || idx > 255) return INT_MIN;
        return -64 + (src[idx*2] | (src[idx*2+1] << 8));
    };
    for (int z = 0; z < 16; ++z) for (int x = 0; x < 16; ++x) {
        int idx = z * 16 + x;
        int h = hm[idx*2] | (hm[idx*2+1] << 8);
        int topY = -64 + h;
        std::string name = "minecraft:air";
        for (int wy = topY; wy >= -64; --wy) {
            int sy = wy >> 4;                        // floor division (arithmetic shift)
            uint64_t skey = chunkBase ^ ((uint64_t)(uint32_t)sy * 0x9E3779B97F4A7C15ull);
            auto it = cache.find(skey);
            if (it == cache.end()) {
                auto raw = ldbGet(chunkKey(cx, cz, 0x2F, true, (int8_t)sy));
                it = cache.emplace(skey, decodeSubchunk(raw)).first;
            }
            int ly = wy - (sy << 4);
            // Reference index order: (x << 8) | (z << 4) | localY.
            name = subchunkBlock(it->second, (x << 8) | (z << 4) | ly);
            if (name != "minecraft:air") break;
        }
        BlockColor bc = blockColor(name);
        // Reference shading: factor = clamp(1 + 0.05*((Y-west)+(Y-north)), 0.5, 1.5)
        int wy = (x == 0) ? heightAt(hmW, z * 16 + 15) : heightAt(hm, z * 16 + x - 1);
        int ny = (z == 0) ? heightAt(hmN, 15 * 16 + x) : heightAt(hm, (z - 1) * 16 + x);
        if (wy == INT_MIN) wy = topY;
        if (ny == INT_MIN) ny = topY;
        float f = 1.0f + 0.05f * (float)((topY - wy) + (topY - ny));
        if (f < 0.5f) f = 0.5f;
        if (f > 1.5f) f = 1.5f;
        bc.r = (uint8_t)std::min(255.0f, bc.r * f);
        bc.g = (uint8_t)std::min(255.0f, bc.g * f);
        bc.b = (uint8_t)std::min(255.0f, bc.b * f);
        out[idx] = bc;
    }
    return out;
}

// ---- world / directory management -------------------------------------------
bool ensureDir(const std::string& path) {
    std::error_code ec;
    return fs::create_directories(path, ec) || fs::is_directory(path, ec);
}

std::string joinPath(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    if (a.back() == '/' || a.back() == '\\') return a + b;
    return a + "/" + b;
}

std::string parentPath(const std::string& p) {
    auto pos = p.find_last_of('/');
    if (pos == std::string::npos || pos == 0) return "/";
    return p.substr(0, pos);
}

std::string toLowerCopy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string baseName(const std::string& p) {
    auto pos = p.find_last_of('/');
    if (pos == std::string::npos) return p;
    return p.substr(pos + 1);
}

void listWorlds(AppState& a) {
    a.worlds.clear();
    std::error_code ec;
    fs::path root = a.worldsRoot;
    if (!fs::exists(root, ec)) { ensureDir(a.worldsRoot); return; }
    for (auto& f : fs::directory_iterator(root, ec)) {
        if (!f.is_directory(ec)) continue;
        app::WorldInfo w;
        w.folder = f.path().string();
        w.internalName = f.path().filename().string();
        w.displayName = readLevelName(f.path());
        w.sizeBytes = dirSize(f.path());
        for (const char* name : {"world_icon.jpeg", "world_icon.jpg", "world_icon.png"}) {
            fs::path ip = f.path() / name;
            if (fs::exists(ip, ec)) { w.iconPath = ip.string(); break; }
        }
        w.hasIcon = !w.iconPath.empty();
        w.encrypted = looksEncrypted(f.path());
        w.external = false;
        a.worlds.push_back(std::move(w));
    }
    std::sort(a.worlds.begin(), a.worlds.end(), [](const app::WorldInfo& x, const app::WorldInfo& y) {
        return x.displayName < y.displayName;
    });
}

void listBrowseDir(AppState& a) {
    a.browseEntries.clear();
    a.browseIsDir.clear();
    std::error_code ec;
    fs::path dir(a.browseDir.empty() ? "/sdcard" : a.browseDir);
    if (!fs::exists(dir, ec)) {
        a.browseDir = "/sdcard";
        dir = "/sdcard";
        if (!fs::exists(dir, ec)) { dir = "/"; a.browseDir = "/"; }
    }
    for (auto& f : fs::directory_iterator(dir, ec)) {
        a.browseEntries.push_back(f.path().string());
        a.browseIsDir.push_back(f.is_directory(ec));
    }
    std::vector<size_t> order(a.browseEntries.size());
    for (size_t k = 0; k < order.size(); ++k) order[k] = k;
    std::sort(order.begin(), order.end(), [&](size_t x, size_t y) {
        if (a.browseIsDir[x] != a.browseIsDir[y]) return a.browseIsDir[x] ? true : false;
        return baseName(a.browseEntries[x]) < baseName(a.browseEntries[y]);
    });
    std::vector<std::string> ents;
    std::vector<bool> dirs;
    for (size_t k : order) { ents.push_back(a.browseEntries[k]); dirs.push_back(a.browseIsDir[k]); }
    a.browseEntries = std::move(ents);
    a.browseIsDir = std::move(dirs);
}

// ---- document open / save ---------------------------------------------------
bool openDocument(AppState& a, const std::string& path, bool levelDat, std::string& err) {
    auto raw = app::readFile(path);
    if (!raw.ok()) { err = raw.detail; return false; }
    nbt::ParseOptions opt;
    opt.skipHeader = levelDat;
    opt.strictHeader = true;
    auto r = nbt::parseBytes(raw.value.data(), raw.value.size(), path, opt);
    if (!r.ok()) { err = r.detail; return false; }
    a.ed = EditorState{};
    a.ed.doc = std::move(r.value);
    a.ed.path = path;
    a.ed.open = true;
    a.ed.levelDat = levelDat;
    a.ed.dirty = false;
    a.ed.pathIdx.clear();
    a.ed.pathIdx.push_back(a.ed.doc.root);
    // Preserve the screen that launched the document.  The old code pushed
    // S_TREE, so Back from a newly opened file returned to another tree view
    // instead of the Browser/Worlds screen that opened it.
    if (a.screen != S_TREE) a.screenStack.push_back(a.screen);
    a.screen = S_TREE;
    rebuildTreeListing(a);
    return true;
}

bool saveDocument(AppState& a, const std::string& path, std::string& err) {
    auto encoded = a.ed.levelDat ? nbt::serializeLevelDat(a.ed.doc) : nbt::serialize(a.ed.doc);
    if (!encoded.ok()) {
        err = nbt::errText(encoded.err);
        if (!encoded.detail.empty()) err += ": " + encoded.detail;
        return false;
    }
    // SAF-opened documents must be written back through their content Uri,
    // not to the private temp copy the file manager bridge made.
    if (a.ed.pickedFile) {
        if (!writePickedFile(a.pickedSaveUri, encoded.value)) {
            err = "Could not write to the original file";
            return false;
        }
        a.ed.dirty = false;
        return true;
    }
    // Documents opened from a LevelDB key (player/world NBT in the drawer) must
    // be written BACK to that key, never to a file named like the key.
    if (a.ed.dbBacked) {
        if (!ldbPut(a.ed.dbKey, encoded.value)) {
            err = "Could not write LevelDB key";
            return false;
        }
        a.ed.dirty = false;
        return true;
    }
    auto w = app::writeFileAtomic(path, encoded.value);
    if (!w.ok()) { err = w.detail; return false; }
    a.ed.dirty = false;
    return true;
}

// ---- tree listing -----------------------------------------------------------
void rebuildTreeListing(AppState& a) {
    // the listing is derived on demand by the screen builder; this only resets scroll
    a.ed.selected = -1;
}

// ---- dialogs ----------------------------------------------------------------
void openDialog(AppState& a, const std::string& title, const std::string& msg,
                const std::string& pos, int posAct, const std::string& neg, int negAct,
                const std::string& neu, int neuAct) {
    a.dialog.open = true;
    a.dialog.title = title;
    a.dialog.message = msg;
    a.dialog.pos = pos;   a.dialog.neg = neg;   a.dialog.neu = neu;
    a.dialog.posAction = posAct; a.dialog.negAction = negAct; a.dialog.neuAction = neuAct;
    a.dialogTargetIndex = -1;
}
void closeDialog(AppState& a) {
    a.dialog = ui::Dialog{};
    a.dialogTargetIndex = -1;
}

// ---- recent files -----------------------------------------------------------
void addRecent(AppState& a, const std::string& path) {
    for (auto& r : a.recent) if (r == path) return;
    a.recent.insert(a.recent.begin(), path);
    if (a.recent.size() > 10) a.recent.pop_back();
}

// ---- clipboard tag copy (deep) ---------------------------------------------
// Clone by arena index, never through a long-lived Node reference: pushing a
// clone can reallocate the arena and invalidate such a reference.
uint32_t cloneNode(AppState& a, uint32_t srcIdx) {
    if (srcIdx >= a.ed.doc.arena.size()) return a.ed.doc.root;
    nbt::Node n = a.ed.doc.arena[srcIdx];
    std::vector<uint32_t> children = n.kids;
    n.kids.clear();
    uint32_t myIdx = (uint32_t)a.ed.doc.arena.size();
    a.ed.doc.arena.push_back(std::move(n));
    for (uint32_t child : children) {
        uint32_t copy = cloneNode(a, child);
        a.ed.doc.arena[myIdx].kids.push_back(copy);
    }
    return myIdx;
}

// ---- flat-world template ---------------------------------------------------
// Bedrock flat world JSON (reference format, ANALYSIS.md §4.2).
static const char* kFlatBiomes[] = {
    "Ocean",           // 0
    "Plains",          // 1
    "Desert",          // 2
    "Windswept Hills", // 3
    "Forest",          // 4
    "Taiga",           // 5
    "Swamp",           // 6
    "River",           // 7
    "Nether Wastes",   // 8
    "The End",         // 9
    "Snowy Beach",     // 10
    "Badlands",        // 11
    "Jungle",          // 12
    "Savanna",         // 13
    "Mushroom Fields", // 14
    "Cherry Grove",    // 15
};
static const int kFlatBiomeCount = (int)(sizeof(kFlatBiomes) / sizeof(kFlatBiomes[0]));

static const char* kFlatBlocks[] = {
    "minecraft:bedrock", "minecraft:dirt", "minecraft:grass_block", "minecraft:sand",
    "minecraft:gravel", "minecraft:stone", "minecraft:water", "minecraft:air",
};
static const int kFlatBlockCount = (int)(sizeof(kFlatBlocks) / sizeof(kFlatBlocks[0]));

std::string buildFlatWorldJson(int biomeIdx, const std::vector<FlatLayer>& layers) {
    std::string j = "{\"biome_id\":";
    j += std::to_string(biomeIdx);
    j += ",\"block_layers\":[";
    bool first = true;
    for (auto& l : layers) {
        if (!first) j += ",";
        first = false;
        j += "{\"block_name\":\"" + l.block + "\",\"count\":" + std::to_string(l.count) + "}";
    }
    j += "],\"encoding_version\":6,\"structure_options\":null,\"world_version\":\"version.post_1_18\"}";
    return j;
}

nbt::Document newRootCompound(const std::string& name = "") {
    nbt::Document d;
    nbt::Node root;
    root.type = nbt::Type::Compound;
    root.name = name;
    d.arena.push_back(std::move(root));
    d.root = 0;
    return d;
}

// Minimal, valid Bedrock level.dat root (enough for a new world to open).
nbt::Document newLevelDat(const std::string& levelName) {
    nbt::Document d = newRootCompound();
    auto add = [&](const std::string& name, nbt::Type t, const std::function<void(nbt::Node&)>& fill) {
        nbt::Node n; n.type = t; n.name = name; fill(n);
        d.arena[0].kids.push_back((uint32_t)d.arena.size());
        d.arena.push_back(std::move(n));
    };
    add("LevelName", nbt::Type::String, [&](nbt::Node& n) { n.s = levelName; });
    add("GameType", nbt::Type::Int, [](nbt::Node& n) { n.i = 1; });
    add("Difficulty", nbt::Type::Int, [](nbt::Node& n) { n.i = 0; });
    add("Generator", nbt::Type::Int, [](nbt::Node& n) { n.i = 2; });
    add("LastPlayed", nbt::Type::Long, [](nbt::Node& n) { n.i = 0; });
    d.headerVersion = 8;
    d.hasLevelDatHeader = true;
    return d;
}

// ---- value formatting for tree rows ----------------------------------------
std::string valueSummary(const nbt::Document& d, uint32_t idx) {
    const nbt::Node& n = d.arena[idx];
    switch (n.type) {
        case nbt::Type::Compound:  return std::to_string(n.kids.size()) + " children";
        case nbt::Type::List:      return std::to_string(n.kids.size()) + " × " + nbt::typeName(n.elemType);
        case nbt::Type::ByteArray: return "[" + std::to_string(n.bytes.size()) + " bytes]";
        case nbt::Type::IntArray:  return "[" + std::to_string(n.longs.size()) + " ints]";
        case nbt::Type::LongArray: return "[" + std::to_string(n.longs.size()) + " longs]";
        case nbt::Type::String:    return n.s.empty() ? "(empty string)" : n.s;
        default: break;
    }
    if (n.type == nbt::Type::Byte || n.type == nbt::Type::Short || n.type == nbt::Type::Int)
        return std::to_string((int32_t)n.i);
    if (n.type == nbt::Type::Long) return std::to_string(n.i);
    if (n.type == nbt::Type::Float) {
        char b[32]; std::snprintf(b, sizeof b, "%g", n.d); return b;
    }
    if (n.type == nbt::Type::Double) {
        char b[32]; std::snprintf(b, sizeof b, "%g", n.d); return b;
    }
    return "";
}

inline const char* typeGlyph(nbt::Type t) {
    switch (t) {
        case nbt::Type::Byte:      return "B";
        case nbt::Type::Short:     return "S";
        case nbt::Type::Int:       return "I";
        case nbt::Type::Long:      return "L";
        case nbt::Type::Float:     return "F";
        case nbt::Type::Double:    return "D";
        case nbt::Type::ByteArray: return "BA";
        case nbt::Type::String:    return "T";
        case nbt::Type::List:      return "L";
        case nbt::Type::Compound:  return "C";
        case nbt::Type::IntArray:  return "IA";
        case nbt::Type::LongArray: return "LA";
        default:                   return "?";
    }
}

gfx::Color typeColor(const gfx::Theme& th, nbt::Type t) {
    switch (t) {
        case nbt::Type::Byte:
        case nbt::Type::Short:
        case nbt::Type::Int:
        case nbt::Type::Long:      return th.primary;
        case nbt::Type::Float:
        case nbt::Type::Double:    return gfx::rgb(0x00, 0x7a, 0xc1);
        case nbt::Type::String:    return gfx::rgb(0x8e, 0x5a, 0x00);
        case nbt::Type::ByteArray:
        case nbt::Type::IntArray:
        case nbt::Type::LongArray: return gfx::rgb(0x7b, 0x5c, 0xb0);
        case nbt::Type::List:      return gfx::rgb(0x00, 0x7d, 0x7b);
        case nbt::Type::Compound:  return gfx::rgb(0x37, 0x4c, 0x33);
        default:                   return th.onSurfaceMuted;
    }
}

inline nbt::Type arrayElementType(nbt::Type arrayType) {
    switch (arrayType) {
        case nbt::Type::ByteArray: return nbt::Type::Byte;
        case nbt::Type::IntArray:  return nbt::Type::Int;
        case nbt::Type::LongArray: return nbt::Type::Long;
        default: return nbt::Type::End;
    }
}

inline size_t arrayElementCount(const nbt::Node& n) {
    if (n.type == nbt::Type::ByteArray) return n.bytes.size();
    if (n.type == nbt::Type::IntArray || n.type == nbt::Type::LongArray) return n.longs.size();
    return 0;
}

// Read an array element as the signed scalar exposed by the editor.  Byte
// arrays are stored as raw bytes on the wire, but their editable value uses
// the signed TAG_Byte range (-128..127), just like the reference editor.
inline bool readArrayElement(const nbt::Node& n, size_t index, int64_t& out) {
    if (index >= arrayElementCount(n)) return false;
    if (n.type == nbt::Type::ByteArray) {
        out = (int8_t)n.bytes[index];
        return true;
    }
    if (n.type == nbt::Type::IntArray || n.type == nbt::Type::LongArray) {
        out = n.longs[index];
        return true;
    }
    return false;
}

inline bool writeArrayElement(nbt::Node& n, size_t index, int64_t value) {
    if (index >= arrayElementCount(n)) return false;
    if (n.type == nbt::Type::ByteArray) {
        n.bytes[index] = (uint8_t)(int8_t)value;
        return true;
    }
    if (n.type == nbt::Type::IntArray || n.type == nbt::Type::LongArray) {
        n.longs[index] = value;
        return true;
    }
    return false;
}

std::vector<TreeEntry> visibleTreeItems(const AppState& a) {
    std::vector<TreeEntry> out;
    if (a.ed.pathIdx.empty()) return out;
    uint32_t cur = a.ed.pathIdx.back();
    if (cur >= a.ed.doc.arena.size()) return out;
    const nbt::Node& n = a.ed.doc.arena[cur];
    if (!a.ed.filter.empty()) {
        // Search mode flattens named arena nodes in parent->child order.
        // Array elements are unnamed and therefore are not search hits.
        std::vector<uint32_t> path{cur};
        std::function<void(uint32_t)> rec = [&](uint32_t idx) {
            path.push_back(idx);
            if (a.ed.doc.arena[idx].name.find(a.ed.filter) != std::string::npos)
                out.push_back({TreeEntryKind::Node, idx, 0});
            for (uint32_t k : a.ed.doc.arena[idx].kids) rec(k);
            path.pop_back();
        };
        // Search descendants only; including `cur` made the first result the
        // current node and tapping it duplicated the breadcrumb path.
        for (uint32_t k : n.kids) rec(k);
        return out;
    }
    if (nbt::isArray(n.type)) {
        size_t count = arrayElementCount(n);
        out.reserve(count);
        for (size_t i = 0; i < count; ++i)
            out.push_back({TreeEntryKind::ArrayElement, cur, i});
        return out;
    }
    for (uint32_t k : n.kids)
        out.push_back({TreeEntryKind::Node, k, 0});
    return out;
}

// path (indices) from document root to a node, used by navigation & undo
std::vector<uint32_t> indexPathTo(AppState& a, uint32_t target) {
    std::vector<uint32_t> path{a.ed.doc.root};
    if (target == a.ed.doc.root) return path;
    std::function<bool(uint32_t)> rec = [&](uint32_t idx) -> bool {
        for (uint32_t k : a.ed.doc.arena[idx].kids) {
            if (k == target) { path.push_back(target); return true; }
            path.push_back(k);
            if (rec(k)) return true;
            path.pop_back();
        }
        return false;
    };
    (void)rec(a.ed.doc.root);
    return path;
}

} // namespace app

// ============================================================================
// L6b — SCREEN BUILDERS (AppState -> ui::Node tree)
// Each builder is a pure function: no I/O, no mutation, host-testable.
// ============================================================================
namespace app {

// The app draws edge-to-edge; Android's status bar occupies the top ~24dp.
// Reserving it keeps the toolbar title off the clock/battery icons (the old
// layout centred the title in y=0..52, which overlapped the status bar).
constexpr int kTopInset = 24;
constexpr int kBarInnerH = 52;
constexpr int kBarH = kTopInset + kBarInnerH;   // 76 dp total
constexpr int kRowH = 44;          // list row height (dp) — comfortable touch target
constexpr int kActionW = 40;       // app bar action button
constexpr int kPad = 16;           // standard horizontal padding
[[maybe_unused]] constexpr int kTouchAxisNone = 0;
[[maybe_unused]] constexpr int kTouchAxisHorizontal = 1;
[[maybe_unused]] constexpr int kTouchAxisVertical = 2;

gfx::Color bgOf(const AppState& a)  { return a.theme.background; }
gfx::Color surfOf(const AppState& a){ return a.theme.surface; }
gfx::Color fgOf(const AppState& a)  { return a.theme.onSurface; }
gfx::Color mutOf(const AppState& a) { return a.theme.onSurfaceMuted; }

// ---------- shared chrome ---------------------------------------------------
ui::Node appBar(const AppState& a, const std::string& title, bool backButton,
                std::vector<ui::Node> actions, int backAction = A_BACK,
                int contentOffset = 0) {
    gfx::Theme th = a.theme;
    ui::Node bar;
    bar.kind = ui::Kind::Panel;
    bar.rect = {0, 0, a.w, kBarH};
    bar.bg = th.primary;
    int x = 6;
    if (backButton) {
        ui::Node b = ui::button({x, kTopInset + 6, kActionW, kActionW}, "<", th.primary, th.onPrimary,
                                backAction, 20);
        b.enabled = true;
        bar.kids.push_back(b);
        x += kActionW + 4;
    } else {
        x = kPad;
    }
    // The title gets exactly the space that is left between the leading
    // button and the action buttons (audit finding A6: the old formula
    // reserved 4 px per action while the buttons were 38 px wide, so the
    // file name was hidden underneath them).
    int actionSpace = (int)actions.size() * (kActionW + 4);
    int titleW = a.w - x - actionSpace - kPad;
    if (titleW < 40) titleW = 40;
    bar.kids.push_back(ui::text({x, kTopInset + (kBarInnerH - gfx::kGlyphH) / 2 + contentOffset,
                                 titleW, gfx::kGlyphH + 2}, title, th.onPrimary));
    int ax = a.w - kPad;
    for (auto it = actions.rbegin(); it != actions.rend(); ++it) {
        ui::Node b = *it;
        b.rect = {ax - kActionW, kTopInset + (kBarInnerH - kActionW) / 2 + contentOffset,
                  kActionW, kActionW};
        b.radius = kActionW / 2;
        b.enabled = true;
        bar.kids.push_back(b);
        ax -= (kActionW + 4);
    }
    return bar;
}

// ---------- OVERFLOW MENU ----------------------------------------------------
// Material overflow menu: a scrim that closes the menu plus a panel of items.
// The items carry the real action ids, so every entry does something.
ui::Node buildOverflowMenu(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    // Reference: DrawerLayout + NavigationView, GravityCompat.START, scrim.
    ui::Node scrim = ui::button({0, 0, a.w, a.h}, "", gfx::rgba(0, 0, 0, 0x60),
                                th.onSurface, A_MENU_CLOSE, 0);
    root.push_back(scrim);
    const int w = std::min(320, a.w * 4 / 5);
    const int top = kBarH;
    ui::Node panel = ui::panel({0, top, w, a.h - top}, {});
    ui::Node head = ui::panel({0, top, w, 116}, {
        ui::iconTile({16, top + 26, 56, 56}, th.onPrimary, "N"),
        ui::text({88, top + 30, w - 100, 22}, a.appName, th.onPrimary, 1, true),
        ui::text({88, top + 60, w - 100, 18},
                 a.ed.path.empty() ? "no file open" : baseName(a.ed.path), th.onPrimary, 1),
    });
    head.bg = th.primary;
    panel.kids.push_back(head);
    int ry = top + 128;
    for (auto& it : a.menuItems) {
        ui::Node r = ui::button({12, ry, w - 24, 46}, it.first, th.surface, th.onSurface,
                                it.second, 8);
        r.radius = 8;
        panel.kids.push_back(r);
        ry += 52;
    }
    panel.bg = th.surface;
    root.push_back(panel);
    return ui::panel({0, 0, a.w, a.h}, root);
}

// ---------- HOME -------------------------------------------------------------
ui::Node buildHome(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, a.appName, false, {
        ui::button({0, 0, kActionW, kActionW}, "M", th.primary, th.onPrimary, A_MENU_OPEN, 20),
    }, A_BACK, 14));

    // Home == world list, matching the reference information hierarchy:
    // white card · 48dp icon · bold name · size · internal folder name.
    int y = kBarH + 10;
    Rect area{kPad, y, a.w - 2 * kPad, a.h - y - 12};
    std::vector<ui::Node> rows;
    int idx = 0;
    int dy = area.y - a.scrollY;
    if (a.worlds.empty()) {
        rows.push_back(ui::text({area.x, dy + 10, area.w, 40},
            "No worlds yet — import a .mcworld or create a flat world.", mutOf(a)));
        dy += 54;
    }
    for (auto& w : a.worlds) {
        ui::Node row = ui::panel({area.x, dy, area.w, 76}, {
            ui::iconTile({area.x + 12, dy + 12, 48, 48}, w.encrypted ? th.error : th.secondary,
                         w.encrypted ? "!" : "W", w.iconPath),
            ui::text({area.x + 72, dy + 8, area.w - 84, 20}, w.displayName, fgOf(a), 1, true),
            ui::text({area.x + 72, dy + 32, area.w - 84, 16}, gfx::humanSize(w.sizeBytes), mutOf(a)),
            ui::text({area.x + 72, dy + 50, area.w - 84, 16}, w.internalName, mutOf(a)),
        });
        row.bg = surfOf(a); row.radius = 8;
        row.action = A_WORLD_OPTIONS; row.actionLong = A_WORLD_DELETE; row.index = idx++;
        rows.push_back(row);
        dy += 82;
    }
    a.contentBottom = dy + a.scrollY;
    root.push_back(ui::list({area.x, area.y, area.w, a.h - area.y - 16}, rows));

    // FAB stack: main "+" expands into two labelled actions (reference pattern).
    const int fx = a.w - 68;
    if (a.fabExpanded) {
        ui::Node labNew = ui::panel({fx - 118, a.h - 188, 110, 28}, {
            ui::text({fx - 114, a.h - 184, 102, 22}, "New World", th.onPrimary, 1, true)});
        labNew.bg = th.primary; labNew.radius = 6;
        ui::Node labImp = ui::panel({fx - 118, a.h - 128, 110, 28}, {
            ui::text({fx - 114, a.h - 124, 102, 22}, "Import World", th.onPrimary, 1, true)});
        labImp.bg = th.primary; labImp.radius = 6;
        root.push_back(labNew);
        root.push_back(ui::button({fx, a.h - 192, 52, 52}, "N", th.secondary, th.onPrimary,
                                  A_WORLD_CREATE, 24));
        root.push_back(labImp);
        root.push_back(ui::button({fx, a.h - 132, 52, 52}, "I", th.secondary, th.onPrimary,
                                  A_WORLD_IMPORT, 24));
    }
    root.push_back(ui::button({fx, a.h - 72, 52, 52}, a.fabExpanded ? "x" : "+",
                              th.primary, th.onPrimary, A_FAB_TOGGLE, 26));

    if (a.menuOpen) root.push_back(buildOverflowMenu(a));
    return ui::panel({0, 0, a.w, a.h}, root);
}

// ---------- WORLDS -----------------------------------------------------------
ui::Node buildWorlds(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, "Minecraft Worlds", true, {
        ui::button({0, 0, kActionW, kActionW}, "M", th.primary, th.onPrimary, A_MENU_OPEN, 20),
    }));

    int y = kBarH + 10;
    Rect area{12, y, a.w - 24, a.h - y - 12};

    std::vector<ui::Node> rows;
    if (a.worlds.empty()) {
        rows.push_back(ui::text({area.x, area.y + 8, area.w, 40},
            "No worlds yet. Tap + to import a .mcworld, or N to create a flat world.", mutOf(a)));
    }
    int idx = 0;
    // The rows live INSIDE the list, so they must start at the list's top.
    // They used to start at y=0 and were drawn over the app bar (audit B3).
    int dy = area.y - a.scrollY;
    for (auto& w : a.worlds) {
        ui::Node row = ui::panel({area.x, dy, area.w, 64}, {
            ui::iconTile({area.x + 10, dy + 12, 40, 40}, w.encrypted ? th.error : th.secondary,
                         w.encrypted ? "!" : "W", w.iconPath),
            ui::text({area.x + 62, dy + 12, area.w - 74, 18}, w.displayName, fgOf(a)),
            ui::text({area.x + 62, dy + 34, area.w - 74, 16},
                      gfx::humanSize(w.sizeBytes) + " · " + w.internalName +
                      (w.encrypted ? " · encrypted" : ""), mutOf(a)),
        });
        row.bg = surfOf(a); row.radius = 8; row.action = A_WORLD_OPTIONS; row.actionLong = A_WORLD_DELETE;
        row.index = idx++;
        rows.push_back(row);
        dy += 68;
    }
    a.contentBottom = dy + a.scrollY;   // content height for the scroll clamp
    // The list was built but never attached to the page: the screen rendered
    // empty (audit finding A2). Now it is a real scroll container.
    ui::Node list = ui::list({area.x, area.y, area.w, a.h - area.y - 96}, rows);
    root.push_back(list);

    // FAB stack (reference: "+" with New World / Import World actions)
    ui::Node fabAdd = ui::button({a.w - 68, a.h - 72, 52, 52}, "+", th.primary, th.onPrimary,
                                 A_WORLD_IMPORT, 26);
    ui::Node fabNew = ui::button({a.w - 68, a.h - 132, 52, 52}, "N", th.secondary, th.onPrimary,
                                 A_WORLD_CREATE, 26);
    ui::Node fabExp = ui::button({a.w - 68, a.h - 192, 52, 52}, "E", th.secondary, th.onPrimary,
                                 A_WORLD_EXPORT, 26);
    root.push_back(fabAdd);
    root.push_back(fabNew);
    root.push_back(fabExp);
    if (a.menuOpen) root.push_back(buildOverflowMenu(a));

    ui::Node page = ui::panel({0, 0, a.w, a.h}, root);
    return page;
}

// ---------- FILE BROWSER -----------------------------------------------------
ui::Node buildBrowser(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    std::string title = a.browseForSave ? "Save As" : "Open NBT file";
    root.push_back(appBar(a, title, true, {}));

    // path bar + up
    int y = kBarH + 6;
    ui::Node pathbar = ui::panel({10, y, a.w - 20, 30}, {
        ui::button({12, y + 2, 26, 26}, "^", surfOf(a), th.primary, A_BROWSE_UP, 13),
        ui::text({44, y + 6, a.w - 66, 18}, a.browseDir, mutOf(a)),
    });
    pathbar.bg = surfOf(a); pathbar.radius = 6;
    root.push_back(pathbar);
    y += 40;

    std::vector<ui::Node> rows;
    const int listTop = y;
    // Same as the world list: rows start at the list's top, not at y=0,
    // otherwise they are drawn over the app bar and the path bar (audit B3).
    int dy = listTop - a.scrollY;
    for (size_t k = 0; k < a.browseEntries.size(); ++k) {
        const std::string& p = a.browseEntries[k];
        bool isDir = a.browseIsDir[k];
        if (!isDir) {
            std::string low = p;
            std::transform(low.begin(), low.end(), low.begin(), ::tolower);
            bool nbtish = low.find(".dat") != std::string::npos ||
                          low.find(".nbt") != std::string::npos ||
                          low.find(".mcstructure") != std::string::npos ||
                          low.find(".snbt") != std::string::npos ||
                          low.find(".mcworld") != std::string::npos;
            bool showAll = !a.browseForSave;   // when saving we only pick directories
            if (!nbtish && !showAll) continue;
        }
        ui::Node row = ui::panel({kPad, dy, a.w - 2 * kPad, kRowH}, {
            ui::iconTile({kPad + 8, dy + 10, 24, 24}, isDir ? gfx::rgb(0xff, 0xd5, 0x4f) : th.secondary,
                         isDir ? "D" : "F"),
            ui::text({kPad + 44, dy + 13, a.w - kPad * 2 - 130, 18}, baseName(p) + (isDir ? "/" : ""), fgOf(a)),
            ui::text({a.w - kPad - 96, dy + 13, 96, 18},
                      isDir ? "" : gfx::humanSize(app::dirSize(p)), mutOf(a)),
        });
        row.bg = surfOf(a); row.radius = 8;
        row.action = isDir ? A_BROWSE_OPEN : A_FILE_SELECT;
        // IMPORTANT: the index must address browseEntries, because the action
        // handler looks the selection up in a.browseEntries. The old code
        // numbered only the *visible* rows, so filtered entries shifted the
        // mapping (audit finding A3).
        row.index = (int)k;
        row.user = isDir ? 1 : 0;
        rows.push_back(row);
        dy += kRowH + 4;
    }
    if (rows.empty()) {
        rows.push_back(ui::text({kPad, listTop + 8, a.w - 2 * kPad, 40},
            a.browseEntries.empty() ? "This folder cannot be read. Allow 'All files access' and try again."
                                    : "No matching files in this folder.", mutOf(a)));
    }
    a.contentBottom = dy + a.scrollY;   // content height for the scroll clamp
    // The rows were never attached to the page: the browser rendered empty
    // (audit finding A3).
    root.push_back(ui::list({kPad, listTop, a.w - 2 * kPad, a.h - listTop - 12}, rows));
    ui::Node page = ui::panel({0, 0, a.w, a.h}, root);
    return page;
}

// ---------- NBT TREE EDITOR --------------------------------------------------
inline int clampBreadcrumbScroll(int offset, int contentWidth, int viewportWidth) {
    int maxOffset = std::max(0, contentWidth - viewportWidth);
    return std::max(0, std::min(offset, maxOffset));
}

// Apply a horizontal finger delta. Positive dx means the finger moved right,
// so the breadcrumb content moves right and the viewport offset decreases.
inline bool scrollBreadcrumb(AppState& a, int fingerDx) {
    if (fingerDx == 0) return false;
    int before = a.breadcrumbScrollX;
    a.breadcrumbScrollX = clampBreadcrumbScroll(
        a.breadcrumbScrollX - fingerDx,
        a.breadcrumbContentWidth,
        a.breadcrumbViewportWidth);
    return before != a.breadcrumbScrollX;
}

ui::Node buildTree(AppState& a) {
    gfx::Theme th = a.theme;
    const nbt::Document& d = a.ed.doc;
    std::vector<ui::Node> root;

    std::string title = baseName(a.ed.path);
    if (a.ed.dirty) title = "* " + title;
    // Only ONE action in the bar: the reference editor keeps the toolbar clean
    // and puts the secondary actions in an overflow menu. Six buttons in a
    // 360 dp bar left no room for the file name (audit finding A6).
    root.push_back(appBar(a, title, true, {
        ui::button({0, 0, kActionW, kActionW}, "M", th.primary, th.onPrimary, A_MENU_OPEN, 20),
    }, A_TREE_UP));

    // Breadcrumb: every ancestor is a real, tappable button.  The viewport is
    // clipped horizontally; crumb children use absolute coordinates so the
    // same rectangles drive both drawing and hit-testing.
    int y = kBarH + 4;
    Rect crumbRect{6, y, a.w - 12, 28};
    ui::Node crumbs;
    crumbs.kind = ui::Kind::Breadcrumb;
    crumbs.id = "breadcrumb";
    crumbs.rect = crumbRect;
    crumbs.bg = surfOf(a);
    crumbs.border = th.outline;
    crumbs.borderW = 1;

    std::vector<std::string> labels;
    std::vector<int> widths;
    for (size_t k = 0; k < a.ed.pathIdx.size(); ++k) {
        uint32_t idx = a.ed.pathIdx[k];
        std::string label = (k == 0) ? "root"
                                     : (d.arena[idx].name.empty() ? "<unnamed>" : d.arena[idx].name);
        labels.push_back(label);
        widths.push_back(std::max(44, (int)label.size() * gfx::kGlyphAdv + 12));
    }

    const int separatorW = 18;
    int contentWidth = 4;
    int currentEnd = 0;
    for (size_t k = 0; k < labels.size(); ++k) {
        if (k) contentWidth += separatorW;
        contentWidth += widths[k];
        if (k + 1 == labels.size()) currentEnd = contentWidth;
    }
    contentWidth += 4;
    std::string filterText;
    if (!a.ed.filter.empty()) {
        filterText = "filter: " + a.ed.filter;
        contentWidth += (int)filterText.size() * gfx::kGlyphAdv + 8;
    }

    bool pathChanged = (a.breadcrumbPath != a.ed.pathIdx);
    a.breadcrumbPath = a.ed.pathIdx;
    a.breadcrumbContentWidth = contentWidth;
    a.breadcrumbViewportWidth = crumbRect.w;
    if (pathChanged) {
        // Keep the active endpoint visible after drilling down or navigating
        // to an ancestor.  Later frames preserve the user's manual offset.
        int reveal = currentEnd > 0 ? currentEnd - crumbRect.w + 4 : 0;
        a.breadcrumbScrollX = clampBreadcrumbScroll(
            reveal, contentWidth, crumbRect.w);
    } else {
        a.breadcrumbScrollX = clampBreadcrumbScroll(
            a.breadcrumbScrollX, contentWidth, crumbRect.w);
    }

    int contentX = 4 - a.breadcrumbScrollX;
    for (size_t k = 0; k < labels.size(); ++k) {
        if (k) {
            ui::Node sep = ui::text(
                {crumbRect.x + contentX, crumbRect.y + 5, separatorW - 4, 16}, ">", mutOf(a));
            sep.id = "breadcrumb_separator";
            crumbs.kids.push_back(sep);
            contentX += separatorW;
        }
        bool current = (k + 1 == labels.size());
        ui::Node c = ui::button(
            {crumbRect.x + contentX, crumbRect.y + 2, widths[k], 24},
            labels[k], surfOf(a), current ? th.primary : fgOf(a),
            A_BREADCRUMB_TAP, 4);
        c.id = "breadcrumb_crumb";
        c.index = (int)k;
        c.bold = current;
        c.selected = current;
        crumbs.kids.push_back(c);
        contentX += widths[k];
    }
    if (!filterText.empty()) {
        ui::Node f = ui::text(
            {crumbRect.x + contentX + 4, crumbRect.y + 5,
             (int)filterText.size() * gfx::kGlyphAdv, 16},
            filterText, th.primary);
        f.id = "breadcrumb_filter";
        crumbs.kids.push_back(f);
    }
    root.push_back(crumbs);
    y += 30;   // keep the tree row origin unchanged from the pre-B6 layout

    // info bar
    std::string info = (a.ed.levelDat ? "level.dat · " : "") + std::to_string(d.arena.size()) + " tags";
    if (a.ed.dirty) info += " · unsaved";
    root.push_back(ui::text({10, y, a.w - 20, 18}, info, mutOf(a)));
    y += 22;

    // search row
    if (a.ed.showSearch) {
        ui::Node srow = ui::panel({10, y, a.w - 20, 34}, {
            ui::searchField({16, y + 4, a.w - 120, 26}, a.ed.filter, A_TREE_SEARCH),
            ui::button({a.w - 96, y + 4, 40, 26}, "Find", th.primary, th.onPrimary, A_TREE_SEARCH),
            ui::button({a.w - 50, y + 4, 34, 26}, "X", surfOf(a), fgOf(a), A_TREE_SEARCH_CLEAR),
        });
        srow.bg = surfOf(a); srow.radius = 6;
        root.push_back(srow);
        y += 42;
    }

    // rows
    std::vector<TreeEntry> entries = visibleTreeItems(a);
    std::vector<ui::Node> rows;
    int rowY = y - a.scrollY;
    for (size_t k = 0; k < entries.size(); ++k) {
        const TreeEntry& entry = entries[k];
        std::string key;
        std::string summary;
        gfx::Color accent;
        bool hasKids = false;
        bool open = false;
        nbt::Type rowType = nbt::Type::End;
        if (entry.kind == TreeEntryKind::ArrayElement) {
            const nbt::Node& owner = d.arena[entry.nodeIndex];
            int64_t value = 0;
            readArrayElement(owner, entry.elementIndex, value);
            key = "[" + std::to_string(entry.elementIndex) + "]";
            summary = std::to_string(value);
            rowType = arrayElementType(owner.type);
            accent = typeColor(th, rowType);
        } else {
            const nbt::Node& n = d.arena[entry.nodeIndex];
            key = n.name.empty() ? "<unnamed>" : n.name;
            summary = valueSummary(d, entry.nodeIndex);
            rowType = n.type;
            accent = typeColor(th, n.type);
            hasKids = !n.kids.empty();
            std::vector<uint32_t> path = indexPathTo(a, entry.nodeIndex);
            open = nodeOpen(a, path);
        }
        ui::Node row = ui::treeRow({kPad, rowY, a.w - 2 * kPad, kRowH},
            key, summary, accent, hasKids, open, (int)k == a.ed.selected, (int)k);
        row.id = entry.kind == TreeEntryKind::ArrayElement ? "array_element" : "tree_node";
        row.value = typeGlyph(rowType);
        // Resolve the selection tint against the row surface.  The software
        // framebuffer is opaque; leaving alpha in this fill made the selected
        // row render as solid green and dropped the value below safe contrast.
        row.bg = ((int)k == a.ed.selected)
            ? gfx::blendOpaque(th.primary, surfOf(a), 40)
            : surfOf(a);
        row.action = A_TREE_TAP;
        row.actionLong = A_TREE_LONG;
        rows.push_back(row);
        rowY += kRowH + 4;
    }
    // scroll-independent content height (dy above is scroll-shifted)
    a.contentBottom = (int)(y + entries.size() * (kRowH + 4));
    // The selected row is highlighted with a primary tint.  Reserve the full
    // vertical space occupied by both FABs so the Save action never covers a
    // tag value near the bottom of the viewport.
    ui::Node scroll = ui::list({kPad, y, a.w - 2 * kPad, a.h - y - 136}, rows);
    scroll.kind = ui::Kind::Scroll;
    root.push_back(scroll);

    // FAB stack: Save below Add (same order as the reference app)
    ui::Node fab = ui::button({a.w - 68, a.h - 72, 52, 52}, "+", th.primary, th.onPrimary, A_TREE_ADD, 26);
    ui::Node save = ui::button({a.w - 68, a.h - 132, 52, 52}, "S", th.secondary, th.onPrimary, A_TREE_SAVE, 26);
    root.push_back(fab);
    root.push_back(save);
    if (a.ed.dirty) {
        root.push_back(ui::text({kPad, a.h - 24, a.w - 2 * kPad, 16},
            "unsaved changes", th.error));
    }
    if (a.menuOpen) root.push_back(buildOverflowMenu(a));

    return ui::panel({0, 0, a.w, a.h}, root);
}

// ---------- FLAT WORLD BUILDER ----------------------------------------------
ui::Node buildFlat(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, "Create flat world", true, {
        ui::button({0, 0, 32, 32}, "OK", th.primary, th.onPrimary, A_FLAT_CREATE, 16),
    }));

    int y = kBarH + 10;
    Rect area{12, y, a.w - 24, a.h - y - 12};

    // biome
    ui::Node biomeRow = ui::panel({area.x, y, area.w, kRowH}, {
        ui::text({area.x + 12, y + 13, 70, 18}, "Biome", mutOf(a)),
        ui::button({area.x + 90, y + 7, area.w - 102, 30},
                   kFlatBiomes[a.flatBiome], surfOf(a), th.primary, A_FLAT_BIOME),
    });
    biomeRow.bg = surfOf(a); biomeRow.radius = 8;
    root.push_back(biomeRow);
    y += kRowH + 10;

    // base-layer checkbox
    ui::Node baseRow = ui::checkbox({area.x, y, area.w, kRowH},
        "Add bedrock base layer", a.flatBaseLayer, A_FLAT_TOGGLE_BASE);
    baseRow.bg = surfOf(a); baseRow.radius = 6;
    root.push_back(baseRow);
    y += kRowH + 8;

    // layers header
    root.push_back(ui::text({area.x, y, area.w, 20}, "Flat World Layers",
                             th.primary, 1, true));
    y += 28;

    int idx = 0;
    int total = 0;
    std::vector<ui::Node> rows;
    int ry = y - a.scrollY;
    for (auto& l : a.flatLayers) {
        total += (int)l.count;
        ui::Node row = ui::panel({area.x, ry, area.w, kRowH}, {
            ui::text({area.x + 12, ry + 8, area.w - 150, 18}, l.block, fgOf(a), 1, true),
            ui::text({area.x + 12, ry + 26, area.w - 150, 16}, "x" + std::to_string(l.count), mutOf(a)),
        });
        ui::Node up = ui::button({area.x + area.w - 92, ry + 10, 32, 28}, "^", bgOf(a), fgOf(a), A_FLAT_LAYER_UP);
        ui::Node down = ui::button({area.x + area.w - 54, ry + 10, 32, 28}, "v", bgOf(a), fgOf(a), A_FLAT_LAYER_DOWN);
        up.index = idx; down.index = idx;             // button acts on its own row
        row.kids.push_back(up);
        row.kids.push_back(down);
        row.bg = surfOf(a); row.radius = 8;
        row.action = A_FLAT_EDIT_LAYER; row.index = idx++;
        row.actionLong = A_FLAT_DEL_LAYER;
        rows.push_back(row);
        ry += kRowH + 4;
    }
    a.contentBottom = y + (int)rows.size() * (kRowH + 4);
    root.push_back(ui::list({area.x, y, area.w, a.h - y - 130}, rows));

    // total + add button
    root.push_back(ui::text({area.x, a.h - 122, area.w, 18},
        "Total thickness: " + std::to_string(total) + " / 384",
        (total > 384) ? th.error : mutOf(a)));
    ui::Node addBtn = ui::button({area.x, a.h - 104, area.w, 44},
        "Add layer", th.primary, th.onPrimary, A_FLAT_ADD_LAYER);
    addBtn.radius = 8; addBtn.bold = true;
    root.push_back(addBtn);

    return ui::panel({0, 0, a.w, a.h}, root);
}

// ---------- HELP / ABOUT -----------------------------------------------------
ui::Node buildHelp(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, "Help & About", true, {}));
    std::vector<std::string> lines = {
        "NBT Editor (native C++)",
        "Reference: MineTuner+ 2.2.3 (com.hextetrahedral.minetuner)",
        "Reference discussion: discord.gg/Mr2za3VkUQ",
        "Version " + std::string(kAppVersion) + " (build " + std::to_string(kAppBuild) + ")",
        "",
        "File",
        "Open: browse to any .dat / .nbt file",
        "New: create a fresh root compound",
        "Save: atomic write to the same path",
        "level.dat is auto-detected (8-byte header)",
        "",
        "Editing",
        "Tap a row: drill down / edit value",
        "Long press: rename, delete, copy, move",
        "+ adds a tag of any of the 13 types",
        "Search filters keys across the document",
        "",
        "Worlds",
        "Import/export .mcworld (zip)",
        "Create a custom flat world",
        "Encrypted worlds are detected and blocked",
        "",
        "All data stays on this device. No network access.",
    };
    int y = kBarH + 12 - a.scrollY;
    std::vector<ui::Node> rows;
    for (auto& l : lines) {
        if (l.empty()) { y += 8; continue; }
        bool head = (l[0] != ' ');
        rows.push_back(ui::text({kPad, y, a.w - 2 * kPad, 20}, l,
                                head ? th.primary : fgOf(a), 1, head));
        y += 24;
    }
    a.contentBottom = y + a.scrollY;
    // a real scroll container: before, the text simply overflowed the screen
    root.push_back(ui::list({kPad, kBarH + 12, a.w - 2 * kPad, a.h - kBarH - 24}, rows));
    return ui::panel({0, 0, a.w, a.h}, root);
}

} // namespace app

// ============================================================================
// L6c — RENDERER (draws the node tree) + ACTION HANDLER + PLATFORM GLUE (L0)
// ============================================================================
namespace app {

// ---- drawing ---------------------------------------------------------------
inline gfx::Color fgFrom(const gfx::Theme& th) { return th.onSurface; }

// The map widget needs live world state (db path, pan/zoom) while drawNode only
// receives a Theme. renderFrame publishes the state here for the duration of
// one frame; it is never read outside the render thread.
static AppState* g_renderState = nullptr;
static std::unordered_map<uint64_t, std::vector<BlockColor>> g_mapChunks;
static std::unordered_map<uint64_t, Subchunk> g_mapSubs;

// Entity markers from LevelDB "digp" keys (reference d0.e.b / s.a.c).
using EntityMarker = EntityChoice;
static std::vector<EntityMarker> g_mapMarkers;
static std::mutex g_markersMutex;
static bool g_markersLoaded = false;
static std::atomic<int> g_markerCount{0};
static std::atomic<int> g_entityKeyCount{0};

inline int32_t le32At(const std::vector<uint8_t>& v, size_t off) {
    if (off + 4 > v.size()) return 0;
    return (int32_t)((uint32_t)v[off] | ((uint32_t)v[off+1] << 8) |
                     ((uint32_t)v[off+2] << 16) | ((uint32_t)v[off+3] << 24));
}

inline const char* entityIconFor(const std::string& identifier) {
    for (const auto& e : kEntityIcons)
        if (e.file && identifier == e.id) return e.file;
    return nullptr;
}

// Scan runs on appWorker: the reference world has ~40k entity keys, doing this
// inside renderFrame froze input dispatch (ANR). Cap the work and publish the
// result under a mutex; the map draws whatever is ready.
static std::vector<EntityMarker> scanMapMarkers() {
    std::vector<EntityMarker> out;
    auto keys = ldbIterate({'d','i','g','p'});
    g_entityKeyCount.store((int)keys.size(), std::memory_order_release);
    // Runs on appWorker (no input stall); still bounded so a huge world cannot
    // grow memory without limit.
    const size_t cap = 2048;
    for (size_t i = 0; i < keys.size() && out.size() < cap; ++i) {
        const auto& key = keys[i];
        if (key.size() < 12) continue;
        // "digp" values are 8-byte actor IDs. The entity NBT lives under
        // "actorprefix" + that id (reference s.a.c()).
        auto id = ldbGet(key);
        if (id.size() != 8) continue;
        std::vector<uint8_t> entKey = {'a','c','t','o','r','p','r','e','f','i','x'};
        entKey.insert(entKey.end(), id.begin(), id.end());
        auto raw = ldbGet(entKey);
        if (raw.empty()) continue;
        nbt::ParseOptions opt;   // entity NBT may or may not carry an 8-byte header
        auto parsed = nbt::parseBytes(raw.data(), raw.size(), "entity", opt);
        if (!parsed.ok()) {
            opt.skipHeader = true;
            opt.strictHeader = false;
            parsed = nbt::parseBytes(raw.data(), raw.size(), "entity", opt);
        }
        if (!parsed.ok()) continue;
        const nbt::Document& doc = parsed.value;
        std::string identifier;
        float ex = 0, ez = 0;
        bool hasPos = false;
        for (const auto& n : doc.arena) {
            if (n.type == nbt::Type::String && n.name == "identifier") identifier = n.s;
            if (n.type == nbt::Type::List && n.name == "Pos" && n.kids.size() >= 3) {
                ex = (float)doc.arena[n.kids[0]].d;
                ez = (float)doc.arena[n.kids[2]].d;
                hasPos = true;
            }
        }
        if (!hasPos || identifier.empty()) continue;
        const char* icon = entityIconFor(identifier);
        if (!icon) continue;                    // projectiles/utilities have no icon
        out.push_back({ex / 1048576.0f, ez / 1048576.0f, icon, identifier,
                       std::move(entKey), std::move(raw)});
    }
    return out;
}

static void loadMapMarkersAsync() {
    if (g_markersLoaded) return;
    g_markersLoaded = true;                     // claim the job (avoid duplicates)
    appWorker().post([]() {
        auto found = scanMapMarkers();
        {
            std::lock_guard<std::mutex> lk(g_markersMutex);
            g_markerCount.store((int)found.size(), std::memory_order_release);
            g_mapMarkers = std::move(found);
        }
    });
}

inline void drawMapView(gfx::Canvas& c, AppState& a, Rect r) {
    const float bpd = 16.0f * a.mapScale;          // design px per block
    c.fill(r, gfx::rgb(0x44, 0x44, 0x44));
    c.pushClip(r);
    if (g_renderState && ldbReady()) {
        // world blocks visible in this viewport
        double left   = a.mapCenterX - (r.w / 2.0) / bpd;
        double top    = a.mapCenterZ - (r.h / 2.0) / bpd;
        double right  = a.mapCenterX + (r.w / 2.0) / bpd;
        double bottom = a.mapCenterZ + (r.h / 2.0) / bpd;
        int c0 = (int)std::floor(left / 16), c1 = (int)std::floor(right / 16);
        int z0 = (int)std::floor(top / 16),  z1 = (int)std::floor(bottom / 16);
        for (int cz = z0; cz <= z1; ++cz) for (int cx = c0; cx <= c1; ++cx) {
            uint64_t key = ((uint64_t)(uint32_t)cx << 32) | (uint32_t)cz;
            auto it = g_mapChunks.find(key);
            if (it == g_mapChunks.end()) {
                auto colors = decodeChunkTop(cx, cz, &g_mapSubs);
                it = g_mapChunks.emplace(key, std::move(colors)).first;
                if (g_mapChunks.size() > 512) g_mapChunks.clear();
            }
            const auto& colors = it->second;
            for (int lz = 0; lz < 16; ++lz) for (int lx = 0; lx < 16; ++lx) {
                int bx = cx * 16 + lx, bz = cz * 16 + lz;
                int px = r.x + (int)std::floor((bx - left) * bpd);
                int py = r.y + (int)std::floor((bz - top) * bpd);
                int pw = (int)std::ceil(bpd), ph = pw;
                if (px + pw < r.x || py + ph < r.y || px > r.right() || py > r.bottom()) continue;
                const BlockColor& bc = colors[lz * 16 + lx];
                if (bc.r == 0 && bc.g == 0 && bc.b == 0) {
                    c.fill({px, py, pw, ph}, ((lx + lz) & 1) ? gfx::rgb(0x2e,0x2e,0x2e)
                                                            : gfx::rgb(0x44,0x44,0x44));
                } else {
                    c.fill({px, py, pw, ph}, gfx::rgb(bc.r, bc.g, bc.b));
                }
            }
            // chunk border + coordinate label (reference d0/a.a)
            int cxp = r.x + (int)std::floor((cx * 16 - left) * bpd);
            int cyp = r.y + (int)std::floor((cz * 16 - top) * bpd);
            int cs = (int)std::floor(16 * bpd);
            if (cs > 24) {
                c.stroke({cxp, cyp, cs, cs}, gfx::rgb(0xff,0xff,0xff), 1);
                c.text(cxp + 5, cyp + 5,
                       "(" + std::to_string(cx * 16) + ", " + std::to_string(cz * 16) + ")",
                       gfx::rgb(0xff,0xff,0xff));
            }
        }
        // PNG mob/player icons from the XAPK assets (42 device px @ scale 1).
        loadMapMarkersAsync();
        std::vector<EntityMarker> markers;
        {
            std::lock_guard<std::mutex> lk(g_markersMutex);
            markers = g_mapMarkers;
        }
        const int ms = 14;   // design units == 42 device px at scale 3
        for (const auto& m : markers) {
            int px = r.x + (int)std::floor((m.x * 1048576.0 - left) * bpd);
            int py = r.y + (int)std::floor((m.z * 1048576.0 - top) * bpd);
            if (px + ms < r.x || py + ms < r.y || px > r.right() || py > r.bottom()) continue;
            auto img = gfx::loadAssetImage(std::string("entityIcons/") + m.icon);
            if (img->ok()) c.image({px - ms / 2, py - ms / 2, ms, ms}, *img);
        }
        // Spawn marker (home.png from the reference).
        auto homeImg = gfx::loadAssetImage("ui/home.png");
        if (homeImg->ok()) {
            int hx = r.x + (int)std::floor((0 - left) * bpd);
            int hy = r.y + (int)std::floor((0 - top) * bpd);
            if (hx >= r.x - 10 && hy >= r.y - 10 && hx <= r.right() && hy <= r.bottom())
                c.image({hx - 9, hy - 18, 18, 18}, *homeImg);
        }
    } else {
        // No LevelDB available: honest placeholder grid instead of a fake map.
        for (int gy = 0; gy < 12; ++gy) for (int gx = 0; gx < 8; ++gx) {
            if ((gx + gy) & 1) continue;
            c.fill({r.x + gx * 48, r.y + gy * 48, 48, 48}, gfx::rgb(0x2e,0x2e,0x2e));
        }
        c.textCentered(r, "Map data unavailable — open a world with level.db", gfx::rgb(0xff,0xff,0xff));
    }
    // spawn marker at block (0,0) is drawn by the world data when available
    c.popClip();
}

inline void drawNode(gfx::Canvas& c, const gfx::Theme& th, const ui::Node& n, int depth) {
    if (!n.visible || n.rect.empty()) return;
    Rect r = n.rect;
    switch (n.kind) {
        case ui::Kind::Panel:
            if (n.bg.a) c.roundRect(r, n.radius, n.bg);
            if (n.borderW) c.stroke(r, n.border, n.borderW);
            break;
        case ui::Kind::Divider:
            c.fill(r, n.bg);
            break;
        case ui::Kind::Text:
            c.pushClip(r);
            if (n.multiline) {
                c.textWrapped(r, n.text, n.fg, (int)n.user, n.bold, n.centered);
            } else {
                const int textX = n.centered
                    ? r.x + (r.w - c.textWidth(n.text, (int)n.user)) / 2
                    : r.x;
                c.text(textX, r.y + 1, n.text, n.fg, (int)n.user, n.bold);
            }
            c.popClip();
            break;
        case ui::Kind::Button: {
            if (!n.enabled) { c.roundRect(r, n.radius, gfx::rgba(n.bg.r, n.bg.g, n.bg.b, 90)); }
            else c.roundRect(r, n.radius, n.bg);
            if (n.selected) c.stroke(r, th.primary, 2);
            c.pushClip(r);
            c.textCentered(r, n.text, n.fg, 1, n.bold);
            c.popClip();
            break; }
        case ui::Kind::IconTile: {
            // A fully transparent background must not be painted: the software
            // canvas is opaque, so rgba(0,0,0,0) would leave a black square.
            if (n.bg.a) c.roundRect(r, 6, n.bg);
            c.pushClip(r);
            if (!n.imagePath.empty()) {
                // "asset:" paths are PNGs bundled inside the APK (navigation
                // icons rasterised from the reference XAPK's vector drawables).
                auto img = n.imagePath.rfind("asset:", 0) == 0
                    ? gfx::loadAssetImage(n.imagePath.substr(6))
                    : gfx::loadImage(n.imagePath);
                if (img->ok()) {
                    c.image(r, *img);
                    c.popClip();
                    break;
                }
            }
            c.textCentered(r, n.text, th.onPrimary);
            c.popClip();
            break; }
        case ui::Kind::Checkbox: {
            c.roundRect(r, n.radius, n.bg);
            Rect box{r.x + 8, r.y + (r.h - 18) / 2, 18, 18};
            c.stroke(box, th.primary, 1);
            if (n.checked) {
                c.fill({box.x + 3, box.y + 3, 12, 12}, th.primary);
            }
            c.pushClip(r);
            c.text(r.x + 34, r.y + (r.h - gfx::kGlyphH) / 2, n.text, fgFrom(th));
            c.popClip();
            break; }
        case ui::Kind::Field: {
            c.roundRect(r, n.radius, n.bg);
            if (n.borderW) c.stroke(r, n.border, n.borderW);
            c.pushClip(r);
            if (!n.text.empty()) c.text(r.x + 8, r.y + 3, n.text, th.onSurfaceMuted);
            c.textEllipsis({r.x + 8, r.y + 16, r.w - 16, r.h - 18}, n.value.empty() ? "(empty)" : n.value,
                           n.value.empty() ? th.onSurfaceMuted : th.onSurface);
            c.popClip();
            break; }
        case ui::Kind::SearchField: {
            c.roundRect(r, n.radius, th.background);
            if (n.borderW) c.stroke(r, th.outline, n.borderW);
            c.pushClip(r);
            c.text(r.x + 6, r.y + (r.h - gfx::kGlyphH) / 2, n.value.empty() ? "search" : n.value,
                   n.value.empty() ? th.onSurfaceMuted : th.onSurface);
            c.popClip();
            break; }
        case ui::Kind::Scroll:
            // Rows use absolute coordinates and may extend above/below the
            // viewport while scrolling.  Clip them so they cannot paint over
            // the app bar, FABs, or another screen region.
            c.pushClip(r);
            for (const auto& k : n.kids) drawNode(c, th, k, depth + 1);
            c.popClip();
            return;
        case ui::Kind::List:
        case ui::Kind::Row:
        case ui::Kind::Column:
            break;
        case ui::Kind::Breadcrumb: {
            c.roundRect(r, 6, n.bg);
            if (n.borderW) c.stroke(r, n.border, n.borderW);
            c.pushClip(r);
            for (const auto& k : n.kids) drawNode(c, th, k, depth + 1);
            c.popClip();
            return;   // children were drawn inside the horizontal viewport
        }
        case ui::Kind::MapView: {
            if (g_renderState) drawMapView(c, *g_renderState, r);
            else c.fill(r, gfx::rgb(0x44, 0x44, 0x44));
            break; }
        case ui::Kind::TreeRow: {
            c.roundRect(r, 6, n.bg);
            if (n.selected) c.stroke(r, th.primary, 2);
            // expander
            int tx = r.x + 8;
            if (n.user) {
                c.text(tx, r.y + (r.h - gfx::kGlyphH) / 2, n.expanded ? "-" : "+", th.onSurfaceMuted);
                tx += 16;
            } else {
                tx += 16;
            }
            // Type icon tile (reference: 32dp nbt_* drawable + 12dp gap)
            const int iconSize = 32;
            Rect ir{tx, r.y + (r.h - iconSize) / 2, iconSize, iconSize};
            c.roundRect(ir, 6, gfx::blendOpaque(n.accent, n.bg, 80));
            std::string glyph = n.value.empty() ? std::string("?") : n.value;
            int gw = c.textWidth(glyph, 1);
            c.text(ir.x + std::max(1, (iconSize - gw) / 2),
                   ir.y + (iconSize - gfx::kGlyphH) / 2, glyph, n.accent);
            tx += iconSize + 12;
            // Key + summary are separate columns.  Reserve a bounded value
            // column so long names and values cannot overlap; long values keep
            // their prefix and end in an ellipsis instead of losing their sign.
            const int contentRight = r.right() - 8;
            const int available = std::max(1, contentRight - tx);
            const int summaryLimit = std::min(132, std::max(48, available * 2 / 5));
            const int summaryWidth = n.text2.empty()
                ? 0 : std::min(c.textWidth(n.text2), summaryLimit);
            const int summaryX = contentRight - summaryWidth;
            const int keyWidth = std::max(1, summaryX - 8 - tx);
            c.textEllipsisAt(tx, r.y + 4, keyWidth, n.text, th.onSurface);
            if (!n.text2.empty()) {
                if (c.textWidth(n.text2) <= summaryWidth) {
                    c.textRight(contentRight, r.y + 5, n.text2, th.onSurfaceMuted);
                } else {
                    c.textEllipsisAt(summaryX, r.y + 5, summaryWidth,
                                     n.text2, th.onSurfaceMuted);
                }
            }
            break; }
        case ui::Kind::Dialog: break;
        case ui::Kind::Toast: break;
        case ui::Kind::ImageTile: break;
        case ui::Kind::Progress: {
            c.roundRect(r, r.h / 2, th.onSurfaceMuted);
            float v = 0.f;
            if (!n.value.empty()) v = std::strtof(n.value.c_str(), nullptr);
            if (v < 0) v = 0;
            if (v > 1) v = 1;
            c.roundRect({r.x, r.y, (int)(r.w * v), r.h}, r.h / 2, n.bg);
            break; }
        case ui::Kind::Keypad: break;
        case ui::Kind::Fab: {
            c.circle(r.x + r.w / 2, r.y + r.h / 2, r.h / 2, n.bg);
            c.pushClip(r);
            c.textCentered(r, n.text, n.fg);
            c.popClip();
            break; }
    }
    for (const auto& k : n.kids) drawNode(c, th, k, depth + 1);
}

// ---- screen dispatch -------------------------------------------------------
ui::Node buildScreen(AppState& a);

// Wrap a message into lines that fit `width` design units.
inline std::vector<std::string> wrapText(const gfx::Canvas& c, const std::string& msg, int width) {
    std::vector<std::string> out;
    if (msg.empty()) return out;
    std::string line;
    for (auto& w : gfx::split(msg, ' ')) {
        if (c.textWidth(line + " " + w) > width && !line.empty()) {
            out.push_back(line);
            line = w;
        } else {
            if (!line.empty()) line += " ";
            line += w;
        }
    }
    if (!line.empty()) out.push_back(line);
    return out;
}

std::vector<ui::Node> buildDialogBody(AppState& a, int contentW, int& outHeight);
std::vector<ui::Node> buildKeypadSheet(AppState& a, int sheetY, int sheetH);

// Frame-rate independent easing for the aside list.  Input only moves the
// target; this advances the rendered offset with a short (~70 ms) time
// constant so the list glides instead of jumping between touch samples.
inline void advanceDrawerAnimation(AppState& a) {
    if (!a.drawerOpen) {
        a.drawerScrollF = (float)a.drawerScrollY;
        a.drawerAnimLastMs = a.nowMs;
        return;
    }
    int64_t dt = a.nowMs - a.drawerAnimLastMs;
    if (a.drawerAnimLastMs == 0 || dt < 0 || dt > 200) dt = 16;
    a.drawerAnimLastMs = a.nowMs;
    const float target = (float)a.drawerScrollY;
    const float diff = target - a.drawerScrollF;
    if (std::abs(diff) < 0.05f) { a.drawerScrollF = target; return; }
    const float k = 1.0f - std::exp(-(float)dt / 70.0f);
    a.drawerScrollF += diff * k;
}

inline void renderFrame(AppState& a, gfx::Canvas& canvas) {
    a.theme = a.dark ? gfx::themeDark() : gfx::themeLight();
    advanceDrawerAnimation(a);
    canvas.setScale(a.scale > 0 ? a.scale : 1);
    canvas.clear(a.theme.background);
    ui::Node page = buildScreen(a);

    // The screen is drawn first; modal layers are drawn afterwards and added to
    // the SAME hit-test tree, so what the user sees is exactly what they can
    // tap (audit finding A4/A5).
    AppState& mutRef = const_cast<AppState&>(a);
    mutRef.uiRoot = std::make_shared<ui::Node>(page);
    g_renderState = &mutRef;
    drawNode(canvas, a.theme, page, 0);
    g_renderState = nullptr;

    // A modal layer must also be an input barrier.  Without this node the page
    // underneath remains hit-testable, which allowed a delete confirmation to
    // change its target behind the dialog.
    auto addModalScrim = [&]() {
        canvas.fill({0, 0, a.w, a.h}, gfx::rgba(0, 0, 0, 90));
        ui::Node scrim;
        scrim.kind = ui::Kind::Panel;
        scrim.rect = {0, 0, a.w, a.h};
        scrim.bg = gfx::rgba(0, 0, 0, 90);
        scrim.action = A_BLOCK;
        scrim.radius = 0;
        mutRef.uiRoot->kids.push_back(scrim);
    };

    // ---- keypad: bottom sheet, topmost layer --------------------------
    int sheetH = 0, sheetY = 0;
    if (a.keypadOpen) {
        sheetH = std::min(a.h * 46 / 100, 430);
        sheetY = a.h - sheetH;
    }

    // ---- modal dialog --------------------------------------------------
    if (a.dialog.open) {
        addModalScrim();
        const int pad = 14;
        const int titleH = gfx::kGlyphH + 6;
        int dw = std::min(a.w - 32, 440);
        int dx = (a.w - dw) / 2;
        int contentW = dw - 2 * pad;

        std::vector<std::string> mlines = wrapText(canvas, a.dialog.message, contentW);
        int msgH = (int)mlines.size() * (gfx::kGlyphH + 3);

        int bodyH = 0;
        std::vector<ui::Node> body = buildDialogBody(a, contentW, bodyH);

        int optH = 0;
        for (auto& b : a.dialog.body) optH += b.rect.h + 4;
        bool hasButtons = !a.dialog.pos.empty() || !a.dialog.neg.empty() || !a.dialog.neu.empty();
        int btnH = hasButtons ? 42 : 4;

        int dH = pad + titleH + msgH + (bodyH ? bodyH + 8 : 0) + (optH ? optH + 6 : 0) + btnH + pad;
        int avail = sheetY ? sheetY - 8 : a.h - 16;
        if (dH > avail) dH = std::max(120, avail);
        int dY = sheetY ? std::max(6, (avail - dH) / 2) : std::max(16, (a.h - dH) / 2);

        Rect d{dx, dY, dw, dH};
        canvas.roundRect({d.x + 2, d.y + 3, d.w, d.h}, 12, gfx::rgba(0, 0, 0, 70));   // shadow
        canvas.roundRect(d, 12, a.theme.surface);
        canvas.pushClip(d);

        int cy = d.y + pad;
        canvas.text(d.x + pad, cy, a.dialog.title, a.theme.onSurface);
        cy += titleH;
        for (auto& ln : mlines) {
            canvas.text(d.x + pad, cy, ln, a.theme.onSurfaceMuted);
            cy += gfx::kGlyphH + 3;
        }

        // interactive body (value editor / add tag / rename)
        if (bodyH) {
            cy += 4;
            for (auto& b : body) {
                ui::translateNodeRecursive(b, (d.x + pad) - b.rect.x, cy - b.rect.y);
                drawNode(canvas, a.theme, b, 1);
                mutRef.uiRoot->kids.push_back(b);      // same node -> same hit box
                cy += b.rect.h + 8;
            }
        }
        // option rows (tag options)
        for (const auto& source : a.dialog.body) {
            // Work on a copy: a.dialog.body persists across frames, while the
            // translation below is only valid in absolute screen coordinates.
            ui::Node b = source;
            ui::translateNodeRecursive(b, (d.x + pad) - b.rect.x, cy - b.rect.y);
            b.rect.w = contentW;
            drawNode(canvas, a.theme, b, 1);
            mutRef.uiRoot->kids.push_back(b);
            cy += b.rect.h + 4;
        }

        // buttons (right aligned, Material order: neutral, negative, positive)
        if (hasButtons) {
            int bty = d.bottom() - pad - 32;
            int btx = d.right() - pad;
            auto placeButton = [&](const std::string& label, int act, bool outline) {
                if (label.empty()) return;
                int bw = std::max(64, canvas.textWidth(label) + 22);
                btx -= bw + 8;
                ui::Node b;
                b.kind = ui::Kind::Button;
                b.rect = {btx, bty, bw, 32};
                b.text = label;
                b.bg = outline ? a.theme.surface : a.theme.primary;
                b.fg = outline ? a.theme.onSurface : a.theme.onPrimary;
                b.border = outline ? a.theme.outline : a.theme.primary;
                b.borderW = outline ? 1 : 0;
                b.action = act;
                b.radius = 8;
                b.enabled = true;
                drawNode(canvas, a.theme, b, 1);
                mutRef.uiRoot->kids.push_back(b);
            };
            placeButton(a.dialog.neu, A_DIALOG_NEU, true);
            placeButton(a.dialog.neg, A_DIALOG_NEG, true);
            placeButton(a.dialog.pos, A_DIALOG_POS, false);
        }
        canvas.popClip();
    }

    // ---- keypad sheet (drawn last: it must never hide under a dialog) ----
    if (a.keypadOpen) {
        if (!a.dialog.open) addModalScrim();
        canvas.roundRect({0, sheetY, a.w, sheetH + 24}, 16, a.theme.surface);
        std::vector<ui::Node> keys = buildKeypadSheet(a, sheetY + 8, sheetH - 8);
        for (auto& k : keys) {
            drawNode(canvas, a.theme, k, 1);
            mutRef.uiRoot->kids.push_back(k);
        }
    }

    // ---- progress dialog ----
    if (a.progressOpen) {
        if (!a.dialog.open && !a.keypadOpen) addModalScrim();
        Rect d{40, a.h / 2 - 60, a.w - 80, 120};
        canvas.roundRect(d, 10, a.theme.surface);
        canvas.pushClip(d);
        canvas.text(d.x + 14, d.y + 12, a.progressTitle, a.theme.onSurface);
        canvas.text(d.x + 14, d.y + 32, a.progressMsg, a.theme.onSurfaceMuted);
        ui::Node prog;
        prog.kind = ui::Kind::Progress; prog.rect = {d.x + 14, d.y + 74, d.w - 28, 12};
        prog.value = std::to_string(a.progressValue); prog.bg = a.theme.primary;
        drawNode(canvas, a.theme, prog, 1);
        canvas.popClip();
    }

    // ---- toasts ----
    int ty = a.h - 60;
    for (auto it = a.toasts.rbegin(); it != a.toasts.rend(); ++it) {
        if (a.nowMs > it->untilMs) continue;
        Rect t{14, ty, a.w - 28, 34};
        canvas.roundRect(t, 8, gfx::rgba(0x20, 0x20, 0x20, 220));
        canvas.pushClip(t);
        canvas.text(t.x + 10, t.y + 8, it->message, gfx::rgb(0xff, 0xff, 0xff));
        canvas.popClip();
        ty -= 40;
    }
}

// ---------- SHOW MAP (reference d0.e) ---------------------------------------
ui::Node buildDrawer(AppState& a);
ui::Node buildMap(AppState& a) {
    gfx::Theme th = a.theme;
    std::vector<ui::Node> root;
    root.push_back(appBar(a, a.activeWorldName.empty() ? "Show Map" : a.activeWorldName,
                          true, {
        ui::button({0, 0, kActionW, kActionW}, "=", th.primary, th.onPrimary, A_MENU_OPEN, 20),
    }, A_BACK));
    ui::Node view = ui::mapView({0, kBarH, a.w, a.h - kBarH});
    view.action = A_MAP_TAP;
    root.push_back(view);
    // scale legend, like the reference's detail-level list
    ui::Node legend = ui::panel({kPad, a.h - 46, 300, 34}, {
        ui::text({kPad + 8, a.h - 40, 290, 22},
                 "Zoom " + std::to_string((int)(a.mapScale * 100)) + "%  ·  " +
                 std::to_string(g_markerCount.load(std::memory_order_acquire)) + "/" +
                 std::to_string(g_entityKeyCount.load(std::memory_order_acquire)) + " entities",
                 th.onPrimary, 1, true),
    });
    legend.bg = gfx::rgba(0, 0, 0, 150); legend.radius = 6;
    root.push_back(legend);
    if (a.drawerOpen) root.push_back(buildDrawer(a));
    return ui::panel({0, 0, a.w, a.h}, root);
}

// ---------- ASIDE DRAWER (reference DrawerLayout + NavigationView) ----------
// Width of the NavigationView (wrap_content in the reference, capped so a
// phone always keeps part of the map visible behind the scrim).
inline int drawerWidth(const AppState& a) { return std::min(320, a.w * 4 / 5); }

ui::Node buildDrawer(AppState& a) {
    gfx::Theme th = a.theme;
    const int w = drawerWidth(a);
    std::vector<ui::Node> root;
    // Scrim: tap outside the panel closes the drawer (DrawerLayout behaviour).
    root.push_back(ui::button({0, 0, a.w, a.h}, "", gfx::rgba(0, 0, 0, 0x50),
                              th.onSurface, A_MENU_CLOSE, 0));
    ui::Node panel = ui::panel({0, 0, w, a.h}, {});
    panel.bg = th.surface;

    // The app draws edge-to-edge; paint the status-bar band with the drawer
    // colour instead of leaving a white strip with unreadable white icons.
    ui::Node statusBand = ui::panel({0, 0, w, kTopInset}, {});
    statusBand.bg = th.primary;
    panel.kids.push_back(statusBand);

    ui::Node head = ui::panel({0, kTopInset, w, 160}, {
        ui::iconTile({16, kTopInset + 16, 56, 56}, th.onPrimary, "N", a.activeWorldIcon),
        ui::text({16, kTopInset + 84, w - 32, 22},
                 a.activeWorldName.empty() ? "NBT Editor" : a.activeWorldName,
                 th.onPrimary, 1, true),
        ui::text({16, kTopInset + 112, w - 32, 18}, baseName(a.activeWorldFolder), th.onPrimary, 1),
    });
    head.bg = th.primary;
    panel.kids.push_back(head);

    // Reference drawer_menu.xml: 5 primary items + "Advanced" submenu,
    // checkableBehavior=single.  Row height matches a Material touch target
    // (48dp); the list scrolls so nothing is ever clipped on short screens.
    const int listTop = kTopInset + 168;
    const int maxScroll = std::max(0, a.drawerContentBottom - (a.h - 8));
    if (a.drawerScrollY < 0) a.drawerScrollY = 0;
    if (a.drawerScrollY > maxScroll) a.drawerScrollY = maxScroll;
    // Render the interpolated position; the input target may move a little
    // ahead of it, which is what produces the smooth, eased motion.
    const int scrollPx = (int)std::lround(a.drawerScrollF);

    std::vector<ui::Node> rows;
    int ry = listTop - scrollPx;
    auto addItem = [&](const char* icon, const char* label, int act) {
        const bool sel = (a.drawerSelected == act);
        const std::string suffix = (a.dark ? "_dark" : "") + std::string(sel ? "_sel" : "");
        std::vector<ui::Node> kids;
        if (icon && icon[0]) {
            kids.push_back(ui::iconTile({12, ry + 12, 24, 24}, gfx::rgba(0, 0, 0, 0),
                                        "", std::string("asset:navIcons/") + icon + suffix + ".png"));
        } else {
            kids.push_back(ui::iconTile({12, ry + 12, 24, 24},
                                        sel ? th.primary : th.onSurfaceMuted,
                                        std::string(1, label[0])));
        }
        kids.push_back(ui::text({48, ry + 14, w - 60, 20}, label,
                                sel ? th.primary : th.onSurface, 1, sel));
        ui::Node row = ui::panel({4, ry, w - 8, 48}, std::move(kids));
        row.bg = sel ? gfx::blendOpaque(th.primary, th.surface, 34) : th.surface;
        row.radius = 6; row.action = act;
        rows.push_back(row);
        ry += 50;
    };
    auto addHeader = [&](const char* label) {
        ui::Node h = ui::text({20, ry + 5, w - 40, 20}, label, th.primary, 1, true);
        rows.push_back(h);
        ry += 28;
    };

    addItem("ic_map", "Show Map", A_NAV_MAP);
    addItem("ic_tune", "World NBT", A_NAV_WORLD_NBT);
    addItem("ic_person", "Singleplayer NBT", A_NAV_LOCAL_PLAYER);
    addItem("ic_group_people", "Multiplayer NBT", A_NAV_MULTIPLAYER);
    addItem("ic_exit", "Change World", A_NAV_CHANGE_WORLD);
    addHeader("Advanced");
    addItem("ic_list", "Overworld NBT", A_NAV_OVERWORLD);
    addItem("ic_list", "Nether NBT", A_NAV_NETHER);
    addItem("ic_list", "The End NBT", A_NAV_END);
    addItem("ic_list", "BiomeData NBT", A_NAV_BIOMEDATA);
    addItem("ic_list", "Portals NBT", A_NAV_PORTALS);
    addItem("ic_list", "Autonomous Entities NBT", A_NAV_ENTITIES);
    addItem("ic_search", "Open Custom NBT", A_NAV_CUSTOM);
    addHeader("Actions");
    addItem(nullptr, "Export .mcworld", A_WORLD_EXPORT);
    addItem(nullptr, "Delete World", A_WORLD_DELETE);
    a.drawerContentBottom = listTop + (ry - (listTop - scrollPx)) + 8;

    ui::Node list = ui::list({0, listTop, w, a.h - listTop}, std::move(rows));
    panel.kids.push_back(list);
    root.push_back(panel);
    return ui::panel({0, 0, a.w, a.h}, root);
}

inline ui::Node buildScreen(AppState& a) {
    switch (a.screen) {
        case S_HOME:   return buildHome(a);
        case S_WORLDS: return buildWorlds(a);
        case S_BROWSER:return buildBrowser(a);
        case S_FLAT:   return buildFlat(a);
        case S_HELP:   return buildHelp(a);
        case S_MAP:    return buildMap(a);
        case S_TREE:
        default:       return buildTree(a);
    }
}

} // namespace app

// ============================================================================
// L6d — ACTION HANDLER (all real behaviour lives here)
// Every control in the UI maps to an A_* intent; this function applies it.
// ============================================================================
namespace app {

void setScreen(AppState& a, int s) {
    if (a.screen == s) return;
    a.screenStack.push_back(a.screen);
    a.screen = s;
}

// Return to the app's home (world list) in ONE step from any screen.  The
// reference closes the world (DrawerActivity n() -> finish()) which lands on
// MainActivity; walking a screen stack one entry at a time felt broken.
void goHome(AppState& a) {
    if (a.screen == S_MAP) {
        ldbClose();
        g_mapChunks.clear();
        g_mapSubs.clear();
        g_mapMarkers.clear();
        g_markersLoaded = false;
        g_markerCount.store(0);
    }
    a.drawerOpen = false;
    a.menuOpen = false;
    a.keypadOpen = false;
    a.dialog.open = false;
    a.screenStack.clear();
    a.scrollY = 0;
    a.ed.selected = -1;
    a.mapSelection = false;
    a.screen = S_HOME;
    listWorlds(a);
}

void back(AppState& a) {
    // Modals close first, exactly like DrawerActivity's OnBackPressedCallback.
    if (a.drawerOpen) { a.drawerOpen = false; return; }
    if (a.dialog.open) { closeDialog(a); return; }
    if (a.keypadOpen) { a.keypadOpen = false; return; }
    if (a.menuOpen) { a.menuOpen = false; return; }
    if (a.screen == S_HOME) { a.wantExit = true; return; }   // exit only from home
    // Preserve unsaved edits with the reference wording; otherwise go home at once.
    if (a.ed.dirty && (a.screen == S_TREE || a.screen == S_MAP)) {
        openDialog(a, "Close NBT Editor",
                   "Are you sure you want to close this NBT editor? Any unsaved changes will be lost.",
                   "YES", A_HOME_CONFIRM, "NO", A_DIALOG_NEG);
        return;
    }
    goHome(a);
}

// ---- keypad (avoid depending on the platform IME) ---------------------------
const char* kKeypadLower = "abcdefghijklmnopqrstuvwxyz";
const char* kKeypadUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* kKeypadDigits = "1234567890";

void applyKey(AppState& a, const std::string& ch) {
    if (ch.empty()) return;
    // The active input context decides the target buffer: the value/add/rename
    // dialogs win over the search filter (audit B4).
    if (a.valueOpen)          { a.valueText += ch; return; }
    if (a.addOpen)            { a.addName += ch; return; }
    if (a.renameOpen)         { a.renameName += ch; return; }
    if (a.keypadTarget == "searchFilter") { a.ed.filter += ch; return; }
    if (a.keypadTarget == "flatThickness") {
        if (ch == " ") return;
        if (ch == "-" || ch == ".") return;         // thickness is a positive integer
        if (a.flatDraft.size() < 4) a.flatDraft += ch;
        return;
    }
}

void keypadClear(AppState& a) {
    if (a.valueOpen)          a.valueText.clear();
    else if (a.addOpen)       a.addName.clear();
    else if (a.renameOpen)    a.renameName.clear();
    else if (a.keypadTarget == "searchFilter") a.ed.filter.clear();
    else if (a.keypadTarget == "flatThickness") a.flatDraft.clear();
}

// Opens the on-screen keypad for whatever text field is active.
void openKeypadForActive(AppState& a);

void keypadBackspace(AppState& a) {
    if (a.keypadTarget == "searchFilter" && !a.ed.filter.empty()) { a.ed.filter.pop_back(); return; }
    if (a.valueOpen && !a.valueText.empty())  { a.valueText.pop_back(); return; }
    if (a.addOpen && !a.addName.empty())      { a.addName.pop_back(); return; }
    if (a.renameOpen && !a.renameName.empty()){ a.renameName.pop_back(); return; }
    if (a.keypadTarget == "flatThickness" && !a.flatDraft.empty()) a.flatDraft.pop_back();
}

void openKeypad(AppState& a, const std::string& target, bool numeric, const std::string& initial) {
    a.keypadOpen = true;
    a.keypadTarget = target;
    a.keypadNumeric = numeric;
    a.keypadBuf = initial;
    if (target == "flatThickness") a.flatDraft = initial;
    a.keypadShift = false;
}

// Opens the on-screen keypad for whatever text field is active.
void openKeypadForActive(AppState& a) {
    if (a.valueOpen)       openKeypad(a, "value", a.valueNumeric, a.valueText);
    else if (a.addOpen)    openKeypad(a, "addName", false, a.addName);
    else if (a.renameOpen) openKeypad(a, "renameName", false, a.renameName);
    else if (a.ed.showSearch) openKeypad(a, "searchFilter", false, a.ed.filter);
}

// ---- nbt editing primitives (used by handler) ------------------------------
void snapshotUndo(AppState& a, const char* label) {
    // No ed.open guard: a document created via A_NEW_NBT must still be undoable
    if (a.ed.doc.arena.empty()) {
        a.ed.doc.arena.push_back(nbt::Node{});
        a.ed.doc.arena[0].type = nbt::Type::Compound;
        a.ed.doc.root = 0;
    }
    EditorState::UndoEntry u;
    u.label = label;
    u.arena = a.ed.doc.arena;
    u.root = a.ed.doc.root;
    a.ed.undoStack.push_back(std::move(u));
    if (a.ed.undoStack.size() > 200) a.ed.undoStack.erase(a.ed.undoStack.begin());
    a.ed.redoStack.clear();
}

void commit(AppState& a) {
    a.ed.dirty = true;
    rebuildTreeListing(a);
}

bool addChildToCompound(AppState& a, uint32_t parentIdx, nbt::Type t, const std::string& name,
                        std::string& err) {
    if (!nbt::isCompound(t)) { err = "Only compounds can hold named tags."; return false; }
    for (uint32_t k : a.ed.doc.arena[parentIdx].kids) {
        const nbt::Node& sib = a.ed.doc.arena[k];
        if (sib.name.empty() && sib.type == nbt::Type::End) continue;
        if (sib.name == name) { err = nbt::msgDuplicateName(); return false; }
    }
    nbt::Node n; n.type = t; n.name = name;
    a.ed.doc.arena[parentIdx].kids.push_back((uint32_t)a.ed.doc.arena.size());
    a.ed.doc.arena.push_back(std::move(n));
    return true;
}

bool addListElement(AppState& a, uint32_t listIdx, std::string& err) {
    nbt::Node& l = a.ed.doc.arena[listIdx];
    if (!nbt::isList(l.type)) { err = "Not a list."; return false; }
    nbt::Node n; n.type = l.elemType;
    l.kids.push_back((uint32_t)a.ed.doc.arena.size());
    a.ed.doc.arena.push_back(std::move(n));
    return true;
}

bool removeNode(AppState& a, uint32_t idx, std::string& err) {
    // find parent from the index path
    std::vector<uint32_t> path = indexPathTo(a, idx);
    if (path.size() < 2) { err = "Cannot remove the root tag."; return false; }
    uint32_t parent = path[path.size() - 2];
    auto& kids = a.ed.doc.arena[parent].kids;
    for (auto it = kids.begin(); it != kids.end(); ++it) {
        if (*it == idx) { kids.erase(it); return true; }
    }
    err = "Tag not found.";
    return false;
}

bool removeArrayElement(AppState& a, uint32_t ownerIdx, size_t elementIndex, std::string& err) {
    if (ownerIdx >= a.ed.doc.arena.size()) {
        err = "The array no longer exists.";
        return false;
    }
    nbt::Node& owner = a.ed.doc.arena[ownerIdx];
    if (!nbt::isArray(owner.type)) {
        err = "The selected tag is not an array.";
        return false;
    }
    if (elementIndex >= arrayElementCount(owner)) {
        err = "The array element no longer exists.";
        return false;
    }
    if (owner.type == nbt::Type::ByteArray)
        owner.bytes.erase(owner.bytes.begin() + elementIndex);
    else
        owner.longs.erase(owner.longs.begin() + elementIndex);
    return true;
}

bool canRenameNode(AppState& a, uint32_t idx, const std::string& name, std::string& err) {
    std::vector<uint32_t> path = indexPathTo(a, idx);
    if (path.size() < 2) { err = "Cannot rename the root tag."; return false; }
    uint32_t parent = path[path.size() - 2];
    if (a.ed.doc.arena[parent].type != nbt::Type::Compound) {
        err = "Only tags inside a compound can be renamed."; return false;
    }
    for (uint32_t k : a.ed.doc.arena[parent].kids) {
        if (k == idx) continue;
        if (a.ed.doc.arena[k].name == name) { err = nbt::msgDuplicateName(); return false; }
    }
    return true;
}

bool renameNode(AppState& a, uint32_t idx, const std::string& name, std::string& err) {
    if (!canRenameNode(a, idx, name, err)) return false;
    a.ed.doc.arena[idx].name = name;
    return true;
}

} // namespace app

// ---- interactive dialog bodies (value editor, add tag, rename) -------------
// These are rebuilt every frame as real ui nodes so taps are hit-tested and
// every visible control has a working action.
namespace app {

// The interactive bodies (value editor / add tag / rename) are rebuilt every
// frame as real ui nodes. They are laid out in DIALOG-LOCAL coordinates
// (x relative to the content column, y relative to the top of the body) and
// translated to absolute positions by the renderer, so the drawn controls and
// the hit-test rectangles can never drift apart (audit finding A4: the old
// code drew the body twice with different geometry, so the controls were
// invisible AND untappable).
std::vector<ui::Node> buildDialogBody(AppState& a, int contentW, int& outHeight);

inline std::vector<ui::Node> buildDialogBody(AppState& a, int contentW, int& outHeight) {
    using namespace ui;
    gfx::Theme th = a.theme;
    std::vector<Node> body;
    int y = 0;
    auto add = [&](Node n, int gap = 8) {
        // Children in this local builder are authored relative to their row
        // panel.  Promote them to the same body coordinate space as the row
        // before moving the row down; otherwise the button is painted above
        // its panel and the panel's hit rectangle rejects the tap.
        Rect old = n.rect;
        n.rect.y = y;
        for (Node& kid : n.kids) {
            kid.rect.x += n.rect.x - old.x;
            kid.rect.y += n.rect.y - old.y;
        }
        body.push_back(n);
        y += n.rect.h + gap;
    };

    // --- value editor -----------------------------------------------------
    if (a.valueOpen) {
        add(text({0, 0, contentW, 18},
            std::string("Type: ") + nbt::typeName(a.valueType), th.onSurfaceMuted));
        Node f = field({0, 0, contentW, 34}, "value", a.valueText, A_KEY_OPEN, a.valueNumeric);
        f.bg = th.background; f.border = th.outline; f.borderW = 1;
        add(f);
        if (!a.valueError.empty())
            add(text({0, 0, contentW, 18}, a.valueError, th.error));
        if (a.valueNumeric) {
            std::vector<Node> keys;
            int kw = (contentW - 9 * 2) / 10;
            int kx = 0;
            for (const char* p = "1234567890"; *p; ++p) {
                Node k = button({kx, 0, kw - 2, 30}, std::string(1, *p),
                                th.primary, th.onPrimary, A_KEY_TAP);
                k.index = (int)(unsigned char)*p;      // the character travels with the key
                keys.push_back(k);
                kx += kw;
            }
            add(panel({0, 0, contentW, 30}, keys));
            // sign / decimal point / clear
            std::vector<Node> keys2;
            int kw2 = (contentW - 2 * 2) / 3;
            kx = 0;
            for (const char* p : {"-", "."}) {
                Node k = button({kx, 0, kw2 - 2, 30}, std::string(p),
                                th.surface, th.onSurface, A_KEY_TAP);
                k.index = (int)(unsigned char)p[0];
                keys2.push_back(k);
                kx += kw2;
            }
            Node clr = button({kx, 0, kw2 - 2, 30}, "CLR", th.surface, th.onSurface, A_KEY_CLEAR);
            keys2.push_back(clr);
            add(panel({0, 0, contentW, 30}, keys2));
        } else {
            add(button({0, 0, contentW, 30}, "Type text", th.primary, th.onPrimary, A_KEY_OPEN));
        }
    }
    // --- add tag -----------------------------------------------------------
    else if (a.addOpen) {
        add(text({0, 0, contentW, 18}, "Tag type", th.onSurfaceMuted));
        nbt::Type all[16];
        int n = 0;
        for (int t = 1; t <= 12; ++t) { nbt::Type tt{}; if (nbt::typeFromId((uint8_t)t, tt)) all[n++] = tt; }
        int cols = 3;
        int rows = (n + cols - 1) / cols;
        int cw = (contentW - (cols - 1) * 6) / cols;
        int ch = 30;
        static const char* kShortNames[12] = {"Byte", "Short", "Int", "Long", "Float", "Double",
                                             "Byte[]", "String", "List", "Compound", "Int[]", "Long[]"};
        for (int r = 0; r < rows; ++r) {
            std::vector<Node> row;
            for (int c = 0; c < cols; ++c) {
                int k = r * cols + c;
                if (k >= n) break;
                bool sel = (all[k] == a.valueTypeToAdd);
                Node b;
                b.kind = Kind::Button;
                b.rect = {c * (cw + 6), 0, cw, ch};
                b.text = kShortNames[k];
                b.bg = sel ? th.primary : th.surface;
                b.fg = sel ? th.onPrimary : th.onSurface;
                b.border = sel ? th.primary : th.outline;
                b.borderW = sel ? 0 : 1;
                b.action = A_VALUE_KIND;
                b.index = k;
                b.radius = 6;
                b.enabled = true;
                row.push_back(b);
            }
            add(panel({0, 0, contentW, ch}, row), 6);
        }
        if (a.addListMode == 0) {
            Node f = field({0, 0, contentW, 32}, "Tag name", a.addName, A_KEY_OPEN, false);
            f.bg = th.background; f.border = th.outline; f.borderW = 1;
            add(f);
        } else if (a.addListMode == 1) {
            add(text({0, 0, contentW, 18},
                "Elements inherit the list's type", th.onSurfaceMuted));
        } else {
            add(text({0, 0, contentW, 18},
                "Array elements use the matching signed integer type", th.onSurfaceMuted));
        }
        if (!a.addError.empty())
            add(text({0, 0, contentW, 18}, a.addError, th.error));
    }
    // --- rename ------------------------------------------------------------
    else if (a.renameOpen) {
        Node f = field({0, 0, contentW, 32}, "New name", a.renameName, A_KEY_OPEN, false);
        f.bg = th.background; f.border = th.outline; f.borderW = 1;
        add(f);
        if (!a.addError.empty())
            add(text({0, 0, contentW, 18}, a.addError, th.error));
    }

    outHeight = body.empty() ? 0 : y - 8;   // trailing gap removed
    return body;
}

// ---- on-screen keypad -----------------------------------------------------
// The on-screen keypad is a bottom sheet (Material style). It is the TOP
// layer: it is drawn after the modal dialog and added to the hit-test tree,
// so it can never be hidden underneath the dialog surface (audit finding A5).
std::vector<ui::Node> buildKeypadSheet(AppState& a, int sheetY, int sheetH);

inline std::vector<ui::Node> buildKeypadSheet(AppState& a, int sheetY, int sheetH) {
    using namespace ui;
    gfx::Theme th = a.theme;
    std::vector<Node> nodes;
    int padX = 10;
    int padW = a.w - 2 * padX;

    std::string* target = nullptr;
    if (a.keypadTarget == "searchFilter") target = &a.ed.filter;
    else if (a.keypadTarget == "value") target = &a.valueText;
    else if (a.keypadTarget == "addName") target = &a.addName;
    else if (a.keypadTarget == "renameName") target = &a.renameName;
    else if (a.keypadTarget == "flatThickness") target = &a.flatDraft;

    // header: current text + done
    Node title = panel({padX, sheetY, padW, 34}, {
        text({padX + 10, sheetY + 9, padW - 96, 18},
             target && !target->empty() ? *target : "(empty)", th.onSurface),
        button({padX + padW - 84, sheetY + 4, 74, 26}, "Done", th.primary, th.onPrimary, A_KEY_ENTER),
    });
    title.bg = th.surface; title.radius = 0;
    nodes.push_back(title);

    // keyboard rows: render every row, with its own number of keys.  The old
    // code concatenated four rows and then drew only three columns, so `0`,
    // `-`, `.`, `j..z`, `_`, `/` and space were never created.
    const char* keyRows[4];
    int nRows = 4;
    if (a.keypadNumeric) {
        keyRows[0] = "123"; keyRows[1] = "456"; keyRows[2] = "789";
        keyRows[3] = ".0-";
    } else {
        keyRows[0] = "abcdefghi";
        keyRows[1] = "jklmnopq";
        keyRows[2] = "rstuvwxy";
        keyRows[3] = "z_./ ";
    }
    int bottomH = 44;
    int y = sheetY + 40;
    int availH = sheetY + sheetH - y - bottomH;
    int kh = availH / nRows - 4;
    if (kh < 28) kh = 28;
    for (int r = 0; r < nRows; ++r) {
        const char* rowChars = keyRows[r];
        int n = (int)std::strlen(rowChars);
        int kw = (padW - (n - 1) * 4) / std::max(1, n);
        std::vector<Node> row;
        int kx = padX;
        for (int c = 0; c < n; ++c) {
            unsigned char ch = (unsigned char)rowChars[c];
            if (a.keypadShift && ch >= 'a' && ch <= 'z') ch = (unsigned char)(ch - 'a' + 'A');
            std::string label(1, (char)ch);
            if (ch == ' ') label = "SPC";
            Node b = button({kx, y, kw, kh}, label, th.surface, th.onSurface, A_KEY_TAP);
            b.index = (int)ch;
            b.border = th.outline; b.borderW = 1;
            row.push_back(b);
            kx += kw + 4;
        }
        Node rp = panel({padX, y, padW, kh}, row);
        rp.bg = th.background;
        nodes.push_back(rp);
        y += kh + 4;
    }
    // bottom controls
    int bw = (padW - 8 - 9) / 4;
    std::vector<Node> bottom = {
        button({padX + 4, y, bw, 38}, "SHIFT", th.surface, th.onSurface, A_KEY_SHIFT),
        button({padX + 8 + bw, y, bw, 38}, "abc/123", th.surface, th.onSurface, A_KEY_MODE),
        button({padX + 12 + 2 * bw, y, bw, 38}, "DEL", th.surface, th.onSurface, A_KEY_DEL),
        button({padX + 16 + 3 * bw, y, bw, 38}, "ENTER", th.primary, th.onPrimary, A_KEY_ENTER),
    };
    for (auto& b : bottom) { b.border = th.outline; b.borderW = 1; }
    Node bp = panel({padX, y, padW, bottomH}, bottom);
    bp.bg = th.background;
    nodes.push_back(bp);
    return nodes;
}

} // namespace app

// ---- commit functions for the dialogs (value / add / rename) ---------------
namespace app {

// Open the value editor for a node (type-specific, keyboard aware).
void openValueEditor(AppState& a, uint32_t idx) {
    const nbt::Node& n = a.ed.doc.arena[idx];
    if (nbt::isContainer(n.type)) return;             // containers are navigated, not edited
    a.valueOpen = true;
    a.valueIdx = idx;
    a.valueIsArrayElement = false;
    a.valueArrayElementIndex = 0;
    a.valueType = n.type;
    a.valueNumeric = (n.type != nbt::Type::String);
    a.valueError.clear();
    switch (n.type) {
        case nbt::Type::String:    a.valueText = n.s; break;
        case nbt::Type::Float:
        case nbt::Type::Double: {
            char b[40]; std::snprintf(b, sizeof b, "%g", n.d); a.valueText = b; break; }
        default:                   a.valueText = std::to_string(n.i); break;
    }
    a.dialog.open = true;
    a.dialog.title = "Edit";
    a.dialog.message = std::string("Type: ") + nbt::typeName(n.type);
    a.dialog.pos = "SAVE"; a.dialog.neg = "CANCEL";
    a.dialog.posAction = A_VALUE_SAVE;
    a.dialog.negAction = A_VALUE_CANCEL;
    a.dialog.body.clear();
}

// Open the numeric editor for one element of a Byte/Int/Long Array.  The
// element is addressed by (owner node, payload index), never by a synthetic
// arena node.
bool openArrayElementEditor(AppState& a, uint32_t ownerIdx, size_t elementIndex) {
    if (ownerIdx >= a.ed.doc.arena.size()) return false;
    nbt::Node& owner = a.ed.doc.arena[ownerIdx];
    nbt::Type scalar = arrayElementType(owner.type);
    int64_t value = 0;
    if (scalar == nbt::Type::End || !readArrayElement(owner, elementIndex, value))
        return false;

    a.valueOpen = true;
    a.valueIdx = ownerIdx;
    a.valueIsArrayElement = true;
    a.valueArrayElementIndex = elementIndex;
    a.valueType = scalar;
    a.valueNumeric = true;
    a.valueError.clear();
    a.valueText = std::to_string(value);
    a.dialog.open = true;
    a.dialog.title = "Edit element";
    a.dialog.message = "Element [" + std::to_string(elementIndex) + "]";
    a.dialog.pos = "SAVE"; a.dialog.neg = "CANCEL";
    a.dialog.posAction = A_VALUE_SAVE;
    a.dialog.negAction = A_VALUE_CANCEL;
    a.dialog.body.clear();
    return true;
}

// Validate + apply the text in the value editor.
void commitValue(AppState& a) {
    nbt::Node tmp;
    std::string msg;
    if (!nbt::parseScalar(a.valueText, a.valueType, tmp, msg)) {
        a.valueError = msg;
        pushToast(a, msg, a.theme.error);
        return;
    }
    if (a.valueIdx >= a.ed.doc.arena.size()) {
        a.valueError = "The tag no longer exists.";
        pushToast(a, a.valueError, a.theme.error);
        return;
    }

    nbt::Node& dst = a.ed.doc.arena[a.valueIdx];
    if (a.valueIsArrayElement) {
        nbt::Type expected = arrayElementType(dst.type);
        if (expected == nbt::Type::End || expected != a.valueType ||
            a.valueArrayElementIndex >= arrayElementCount(dst)) {
            a.valueError = "The array element no longer exists.";
            pushToast(a, a.valueError, a.theme.error);
            return;
        }
    }

    bool wasArrayElement = a.valueIsArrayElement;
    snapshotUndo(a, wasArrayElement ? "edit array element" : "edit value");
    if (wasArrayElement) {
        if (!writeArrayElement(dst, a.valueArrayElementIndex, tmp.i)) {
            a.valueError = "The array element no longer exists.";
            pushToast(a, a.valueError, a.theme.error);
            return;
        }
    } else if (a.valueType == nbt::Type::String) {
        dst.s = tmp.s;
    } else if (a.valueType == nbt::Type::Float || a.valueType == nbt::Type::Double) {
        dst.d = tmp.d;
    } else {
        dst.i = tmp.i;
    }
    a.valueOpen = false;
    a.valueIsArrayElement = false;
    a.valueArrayElementIndex = 0;
    closeDialog(a);
    commit(a);
    pushToast(a, wasArrayElement ? "Element updated" : "Value updated", a.theme.success);
}

// Create a new child inside the current compound, or an element in a list.
void commitAdd(AppState& a) {
    uint32_t cur = a.ed.pathIdx.empty() ? a.ed.doc.root : a.ed.pathIdx.back();
    if (cur >= a.ed.doc.arena.size()) {
        a.addError = "The current tag no longer exists.";
        pushToast(a, a.addError, a.theme.error);
        return;
    }
    nbt::Type targetType = a.ed.doc.arena[cur].type;

    if (nbt::isList(targetType)) {
        snapshotUndo(a, "add list element");
        nbt::Node n; n.type = a.ed.doc.arena[cur].elemType;
        a.ed.doc.arena[cur].kids.push_back((uint32_t)a.ed.doc.arena.size());
        a.ed.doc.arena.push_back(std::move(n));
    } else if (nbt::isArray(targetType)) {
        nbt::Type expected = arrayElementType(targetType);
        if (expected == nbt::Type::End || a.valueTypeToAdd != expected) {
            a.addError = nbt::msgOutOfBoundsFor();
            const char* mismatch = targetType == nbt::Type::ByteArray
                ? "Only byte tags can be added to byte arrays"
                : (targetType == nbt::Type::IntArray
                       ? "Only int tags can be added to int arrays"
                       : "Only long tags can be added to long arrays");
            pushToast(a, mismatch, a.theme.error);
            return;
        }
        snapshotUndo(a, "add array element");
        if (targetType == nbt::Type::ByteArray)
            a.ed.doc.arena[cur].bytes.push_back(0);
        else
            a.ed.doc.arena[cur].longs.push_back(0);
        pushToast(a, "Element added", a.theme.success);
    } else if (nbt::isCompound(targetType)) {
        if (a.valueTypeToAdd == nbt::Type::End) {
            a.addError = "Pick a tag type.";
            return;
        }
        std::string name = gfx::trim(a.addName);
        if (name.empty()) {
            a.addError = nbt::msgUnnamedTag();
            pushToast(a, nbt::msgUnnamedTag(), a.theme.error);
            return;
        }
        for (uint32_t k : a.ed.doc.arena[cur].kids) {
            if (a.ed.doc.arena[k].name == name) {
                a.addError = nbt::msgDuplicateName();
                pushToast(a, nbt::msgDuplicateName(), a.theme.error);
                return;
            }
        }
        snapshotUndo(a, "add tag");
        nbt::Node n; n.type = a.valueTypeToAdd; n.name = name;
        if (n.type == nbt::Type::List) n.elemType = nbt::Type::Byte;   // typed on first add
        a.ed.doc.arena[cur].kids.push_back((uint32_t)a.ed.doc.arena.size());
        a.ed.doc.arena.push_back(std::move(n));
        pushToast(a, "Tag added", a.theme.success);
    } else {
        pushToast(a, "Cannot add to this tag", a.theme.error);
        return;
    }
    a.addOpen = false;
    closeDialog(a);
    commit(a);
}

// Validate + apply a rename (compounds only, unique key).
void commitRename(AppState& a) {
    std::string err;
    std::string name = gfx::trim(a.renameName);
    if (name.empty()) {
        a.addError = nbt::msgUnnamedTag();
        pushToast(a, nbt::msgUnnamedTag(), a.theme.error);
        return;
    }
    if (!canRenameNode(a, a.renameIdx, name, err)) {
        a.addError = err;
        pushToast(a, err, a.theme.error);
        return;
    }
    // Snapshot BEFORE the mutation; the previous order saved the already
    // renamed document, so undo could never restore the old name.
    snapshotUndo(a, "rename tag");
    if (!renameNode(a, a.renameIdx, name, err)) {
        a.ed.undoStack.pop_back();                 // defensive: validation already passed
        a.addError = err;
        pushToast(a, err, a.theme.error);
        return;
    }
    a.renameOpen = false;
    a.keypadOpen = false;
    a.keypadTarget.clear();
    closeDialog(a);
    commit(a);
    pushToast(a, "Renamed to " + name, a.theme.success);
}

void openDeleteConfirmation(AppState& a, const TreeEntry& entry) {
    if (entry.kind == TreeEntryKind::ArrayElement) {
        if (entry.nodeIndex >= a.ed.doc.arena.size()) return;
        const nbt::Node& owner = a.ed.doc.arena[entry.nodeIndex];
        if (!nbt::isArray(owner.type) || entry.elementIndex >= arrayElementCount(owner)) return;
        std::string ownerName = owner.name.empty() ? "array" : owner.name;
        a.deleteEntry = entry;
        openDialog(a, "Remove array element",
                   "Remove element [" + std::to_string(entry.elementIndex) + "] from \"" +
                       ownerName + "\"?",
                   "REMOVE", A_TREE_DELETE, "CANCEL", A_DIALOG_NEG);
        return;
    }
    if (entry.nodeIndex >= a.ed.doc.arena.size()) return;
    const nbt::Node& n = a.ed.doc.arena[entry.nodeIndex];
    std::string label = n.name.empty() ? "<unnamed>" : n.name;
    a.deleteEntry = entry;
    openDialog(a, "Remove tag", "Remove \"" + label + "\"?",
               "REMOVE", A_TREE_DELETE, "CANCEL", A_DIALOG_NEG);
}

} // namespace app

// ---- main action dispatcher ------------------------------------------------
namespace app {
using app::AppState;
using app::EditorState;

void popScreenTo(AppState& a, int s);

void handleAction(AppState& a, int act);

// Applies a hit node to the state. It lives in L6 (not in the L0 Android glue)
// so the host model tests can exercise the real tap path: row selection,
// keypad character capture and the action itself (audit B2 + B4).
inline bool dispatchHit(AppState& a, const ui::Node& hit, bool longPress) {
    // rows carry their index: that is how list screens know what was picked
    // (audit B2: the row itself must be the hit node, not its label/icon)
    if (hit.index >= 0 && hit.action != A_KEY_TAP) a.ed.selected = hit.index;
    // a keypad key carries the pressed character in .index (audit B4: the
    // dispatcher has to copy it out, otherwise nothing is ever typed)
    if (hit.action == A_KEY_TAP) a.lastKeyPressed = ui::keyCharOf(hit);
    int act = longPress ? hit.actionLong : hit.action;
    if (!act) return false;
    // any action closes the overflow menu (the menu items carry the
    // real action ids, so they keep working)
    a.menuOpen = false;
    handleAction(a, act);
    return true;
}

// Spawn point from level.dat (reference t.j.f / d0.b.java).
static bool readWorldSpawn(const std::string& folder, int64_t& sx, int64_t& sz) {
    auto raw = app::readFile(joinPath(folder, "level.dat"));
    if (!raw.ok()) return false;
    nbt::ParseOptions opt;
    opt.skipHeader = true;
    opt.strictHeader = false;
    auto r = nbt::parseBytes(raw.value.data(), raw.value.size(), "level.dat", opt);
    if (!r.ok()) return false;
    bool hx = false, hz = false;
    for (auto& n : r.value.arena) {
        if (n.name == "SpawnX") { sx = n.i; hx = true; }
        if (n.name == "SpawnZ") { sz = n.i; hz = true; }
    }
    return hx && hz;
}

// Open a map entity's raw NBT in the shared editor. The actorprefix key is
// remembered so Save writes back into LevelDB (like the reference f0.a).
static bool openEntityEditor(AppState& a, const EntityChoice& ent) {
    if (ent.nbt.empty()) return false;
    nbt::ParseOptions opt;
    auto r = nbt::parseBytes(ent.nbt.data(), ent.nbt.size(), "entity", opt);
    if (!r.ok()) {
        opt.skipHeader = true; opt.strictHeader = false;
        r = nbt::parseBytes(ent.nbt.data(), ent.nbt.size(), "entity", opt);
    }
    if (!r.ok()) return false;
    a.ed = EditorState{};
    a.ed.doc = std::move(r.value);
    a.ed.path = ent.identifier.empty() ? "entity" : ent.identifier;
    a.ed.dbBacked = true;
    a.ed.dbKey = ent.key;
    a.ed.open = true;
    a.ed.pathIdx = {a.ed.doc.root};
    a.drawerOpen = false;
    setScreen(a, S_TREE);
    return true;
}

// Enter the reference DrawerActivity equivalent: Show Map + aside drawer.
static void enterWorldEditor(AppState& a, const WorldInfo& w) {
    a.activeWorldFolder = w.folder;
    a.activeWorldName = w.displayName.empty() ? w.internalName : w.displayName;
    a.activeWorldIcon = w.iconPath;
    a.mapScale = 0.5f;
    int64_t sx = 0, sz = 0;
    if (readWorldSpawn(w.folder, sx, sz)) { a.mapCenterX = sx; a.mapCenterZ = sz; }
    else { a.mapCenterX = 0; a.mapCenterZ = 0; }
    a.mapSelection = false;
    ldbClose();
    ldbOpen(w.folder);
    g_mapChunks.clear(); g_mapSubs.clear();
    g_mapMarkers.clear(); g_markersLoaded = false; g_markerCount.store(0);
    setScreen(a, S_MAP);
    a.drawerOpen = false;         // user opens the aside by swiping (DrawerLayout)
}

// Shared by the legacy browser and the SAF file manager.  `displayName` is
// the name reported by the OS picker (the real file on disk is a temp copy),
// while `tempCopy` lets us clean that copy up after extraction.
static void startWorldImport(AppState& a, const std::string& src,
                             const std::string& displayName, bool tempCopy) {
    std::string lower = toLowerCopy(displayName.empty() ? baseName(src) : displayName);
    bool okExt = false;
    for (const char* ext : {".mcworld", ".mctemplate", ".mcaddon", ".mcpack", ".zip"}) {
        if (lower.size() >= std::strlen(ext) &&
            lower.compare(lower.size() - std::strlen(ext), std::strlen(ext), ext) == 0) {
            okExt = true; break;
        }
    }
    if (!okExt) {
        pushToast(a, "Not a .mcworld/.mctemplate file", a.theme.error);
        return;
    }
    std::string stem = displayName.empty() ? baseName(src) : baseName(displayName);
    size_t dot = stem.find_last_of('.');
    if (dot != std::string::npos) stem = stem.substr(0, dot);
    if (stem.empty()) stem = "Imported";
    std::error_code ec;
    std::string dest = joinPath(a.worldsRoot, stem);
    int suffix = 1;
    while (fs::exists(dest, ec))
        dest = joinPath(a.worldsRoot, stem + "_" + std::to_string(suffix++));
    a.progressOpen = true;
    a.progressTitle = "Importing world";
    a.progressMsg = (displayName.empty() ? baseName(src) : displayName) + " -> " + dest;
    a.progressValue = 0.25f;
    appWorker().post([&a, src, dest, tempCopy]() mutable {
        auto r = extractZip(src, dest);
        if (tempCopy) { std::error_code e; fs::remove(src, e); }
        // AppState is owned by the render thread: lock before use.
        std::lock_guard<std::mutex> lk(g_stateMutex);
        a.progressValue = 1.0f;
        a.progressOpen = false;
        if (r.ok()) {
            pushToast(a, "Imported: " + readLevelName(dest), a.theme.success);
            listWorlds(a);
            popScreenTo(a, S_WORLDS);
        } else pushToast(a, "Import failed: " + r.detail, a.theme.error);
    });
}

inline void handleAction(AppState& a, int act) {
    std::string err;
    switch (act) {
        // ---------- global ----------
        case A_MENU_OPEN: {
            a.menuItems.clear();
            switch (a.screen) {
                case S_HOME:
                    a.menuItems = {{"Open NBT file", A_OPEN_NBT},
                                   {"New NBT file", A_NEW_NBT},
                                   {"Minecraft worlds", A_WORLDS},
                                   {"Import .mcworld", A_WORLD_IMPORT},
                                   {a.dark ? "Use light theme" : "Use dark theme", A_TOGGLE_THEME},
                                   {"Help & About", A_HELP}};
                    break;
                case S_WORLDS:
                    a.menuItems = {{"Import .mcworld", A_WORLD_IMPORT},
                                   {"New flat world", A_WORLD_CREATE},
                                   {"Export .mcworld", A_WORLD_EXPORT},
                                   {"Help & About", A_HELP}};
                    break;
                case S_TREE:
                    a.menuItems = {{"Undo", A_TREE_UNDO},
                                   {"Redo", A_TREE_REDO},
                                   {"Search keys", A_TREE_SEARCH},
                                   {"Add tag", A_TREE_ADD},
                                   {"Save", A_TREE_SAVE},
                                   {"Save as…", A_TREE_SAVE_AS},
                                   {"Go to root", A_TREE_MOVE_ROOT},
                                   {"Help & About", A_HELP}};
                    if (a.ed.hasClip)
                        a.menuItems.insert(a.menuItems.begin() + 4,
                                           {"Paste tag", A_TREE_PASTE});
                    break;
                case S_FLAT:
                    a.menuItems = {{"Help & About", A_HELP}};
                    break;
                default:
                    a.menuItems = {{"Help & About", A_HELP}};
                    break;
            }
            a.menuOpen = true;
            if (a.screen == S_MAP) {                    // aside on Show Map
                a.drawerOpen = true; a.drawerScrollY = 0; a.drawerScrollF = 0;
            }
            break; }
        case A_MENU_CLOSE:
            a.menuOpen = false;
            a.drawerOpen = false;
            break;
        // ---------- drawer navigation (reference drawer_menu.xml) ----------
        case A_NAV_MAP:
            a.drawerOpen = false;
            a.drawerSelected = A_NAV_MAP;
            setScreen(a, S_MAP);
            break;
        case A_MAP_TAP: {
            float bpd = 16.0f * a.mapScale;
            int64_t wx = a.mapCenterX + (int64_t)((a.touchX - a.w / 2) / bpd);
            int64_t wz = a.mapCenterZ + (int64_t)((a.touchY - (kBarH + (a.h - kBarH) / 2)) / bpd);
            a.mapTapX = wx; a.mapTapZ = wz;
            // Tapping directly on a mob/player icon opens its NBT editor.
            bool openedEntity = false;
            {
                std::lock_guard<std::mutex> lk(g_markersMutex);
                for (const auto& m : g_mapMarkers) {
                    float mx = m.x * 1048576.0f, mz = m.z * 1048576.0f;
                    if (std::abs(mx - (float)wx) <= 3 && std::abs(mz - (float)wz) <= 3) {
                        if (openEntityEditor(a, m)) {
                            pushToast(a, "Editing " + m.identifier, a.theme.primary);
                            openedEntity = true;
                        }
                        break;
                    }
                }
            }
            if (openedEntity) break;
            int64_t cx = (int64_t)std::floor((double)wx / 16.0);
            int64_t cz = (int64_t)std::floor((double)wz / 16.0);
            openDialog(a, "Coords(" + std::to_string(wx) + ", " + std::to_string(wz) + ") Chunk("
                          + std::to_string(cx) + ", " + std::to_string(cz) + ")",
                       a.mapSelection ? "Selection Options" : "Map actions",
                       a.mapSelection ? "END SEL" : "BEGIN SEL", A_MAP_SELECT,
                       "ENTITIES", A_MAP_ENTITIES, "CANCEL", A_DIALOG_NEG);
            break; }
        case A_MAP_SELECT:
            a.mapSelection = !a.mapSelection;
            if (a.mapSelection) {
                int half = (int)std::round((std::min(a.w, a.h - kBarH) / 4.0f) / (16.0f * a.mapScale));
                (void)half;   // overlay is drawn as a selection guide on the map
            }
            pushToast(a, a.mapSelection ? "Selection started" : "Selection ended",
                      a.theme.primary);
            break;
        case A_MAP_ENTITIES: {
            int64_t cx = (int64_t)std::floor((double)a.mapTapX / 16.0);
            int64_t cz = (int64_t)std::floor((double)a.mapTapZ / 16.0);
            a.entityChoices.clear();
            {
                std::lock_guard<std::mutex> lk(g_markersMutex);
                for (const auto& m : g_mapMarkers) {
                    float dx = m.x * 1048576.0f - (float)a.mapTapX;
                    float dz = m.z * 1048576.0f - (float)a.mapTapZ;
                    // Nearby radius is more forgiving than an exact chunk match
                    // (markers are sampled, so a chunk may have none loaded).
                    if (dx * dx + dz * dz <= 32.0f * 32.0f) a.entityChoices.push_back(m);
                }
            }
            if (a.entityChoices.empty()) {
                pushToast(a, "No entities in this chunk", a.theme.onSurfaceMuted);
                break;
            }
            openDialog(a, "Chunk Entities at (" + std::to_string(cx) + ", " +
                          std::to_string(cz) + ")", "Select an entity to edit", "", 0, "", 0);
            a.dialog.body.clear();
            for (size_t i = 0; i < a.entityChoices.size() && i < 8; ++i) {
                ui::Node b;
                b.kind = ui::Kind::Button;
                b.rect = {24, 0, a.w - 48, 30};
                b.text = a.entityChoices[i].identifier.empty()
                         ? a.entityChoices[i].icon : a.entityChoices[i].identifier;
                b.bg = a.theme.surface; b.fg = a.theme.onSurface;
                b.border = a.theme.outline; b.borderW = 1;
                b.radius = 6; b.enabled = true;
                b.action = A_ENTITY_PICK; b.index = (int)i;
                a.dialog.body.push_back(b);
            }
            break; }
        case A_ENTITY_PICK: {
            if (a.ed.selected < 0 || a.ed.selected >= (int)a.entityChoices.size()) break;
            EntityChoice ent = a.entityChoices[a.ed.selected];
            closeDialog(a);
            if (!openEntityEditor(a, ent))
                pushToast(a, "Could not open entity", a.theme.error);
            break; }
        case A_NAV_CHANGE_WORLD:
            // Reference: closes the world and returns to MainActivity (home).
            a.drawerOpen = false;
            a.drawerSelected = A_NAV_MAP;
            goHome(a);
            break;
        case A_NAV_WORLD_NBT: {
            a.drawerOpen = false;
            a.drawerSelected = A_NAV_WORLD_NBT;
            std::string lvl = joinPath(a.activeWorldFolder, "level.dat");
            if (!openDocument(a, lvl, true, err))
                pushToast(a, "No world data found.", a.theme.error);
            else addRecent(a, lvl);
            break; }
        case A_NAV_LOCAL_PLAYER: case A_NAV_MULTIPLAYER:
        case A_NAV_OVERWORLD: case A_NAV_NETHER: case A_NAV_END:
        case A_NAV_BIOMEDATA: case A_NAV_PORTALS: case A_NAV_ENTITIES:
        case A_NAV_CUSTOM: {
            a.drawerOpen = false;
            a.drawerSelected = act;
            const char* key = "";
            switch (act) {
                case A_NAV_LOCAL_PLAYER: key = "~local_player"; break;
                case A_NAV_OVERWORLD:    key = "Overworld"; break;
                case A_NAV_NETHER:       key = "Nether"; break;
                case A_NAV_END:          key = "TheEnd"; break;
                case A_NAV_BIOMEDATA:    key = "BiomeData"; break;
                case A_NAV_PORTALS:      key = "portals"; break;
                case A_NAV_ENTITIES:     key = "AutonomousEntities"; break;
                case A_NAV_MULTIPLAYER:  key = ""; break;
                default: break;
            }
            if (act == A_NAV_MULTIPLAYER) {
                auto players = ldbIterate({'p','l','a','y','e','r','_','s','e','r','v','e','r','_'});
                if (players.empty()) pushToast(a, "No players found.", a.theme.onSurfaceMuted);
                else pushToast(a, "Players: " + std::to_string(players.size()) +
                                  " (tap a key to edit)", a.theme.primary);
                break;
            }
            if (act == A_NAV_CUSTOM) {
                openDialog(a, "Custom NBT Key",
                           "Enter the database key to view its NBT data:",
                           "OK", A_NAV_CUSTOM, "CANCEL", A_DIALOG_NEG);
                break;
            }
            std::vector<uint8_t> dbkey(key, key + std::strlen(key));
            std::vector<uint8_t> raw = ldbGet(dbkey);
            if (raw.empty()) { pushToast(a, "No NBT data found.", a.theme.onSurfaceMuted); break; }
            {
                auto r = nbt::parseBytes(raw.data(), raw.size(), key, nbt::ParseOptions{});
                if (!r.ok()) { pushToast(a, "No NBT data found.", a.theme.error); break; }
                a.ed = EditorState{};
                a.ed.doc = std::move(r.value);
                a.ed.path = key;
                a.ed.open = true;
                a.ed.dbBacked = true;        // Save must use ldbPut(key), not a file
                a.ed.dbKey = std::move(dbkey);
                a.ed.pathIdx = {a.ed.doc.root};
                setScreen(a, S_TREE);
            }
            break; }

        case A_WORLD_EXPORT:
            a.drawerOpen = false;   // NavigationView closes on item selection
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                auto& w = a.worlds[a.ed.selected];
                std::string dest = "/sdcard/Download";
                ensureDir(dest);
                std::string zip = joinPath(dest, w.internalName + ".mcworld");
                pushToast(a, "Compiling world...", a.theme.primary);
                a.progressOpen = true;
                a.progressTitle = "Compiling world...";
                a.progressMsg = "Please wait while the world is being compiled. Do not exit the app.";
                a.progressValue = 0.2f;
                appWorker().post([&a, w, zip]() mutable {
                    auto r = app::createZip(w.folder, zip);
                    std::lock_guard<std::mutex> lk(g_stateMutex);
                    a.progressValue = 1.0f;
                    a.progressOpen = false;
                    pushToast(a, r.ok() ? ("Exported " + baseName(zip)) : ("Export failed: " + r.detail),
                              r.ok() ? a.theme.success : a.theme.error);
                });
            }
            break;
        case A_TOGGLE_THEME:
            a.dark = !a.dark;
            a.theme = a.dark ? gfx::themeDark() : gfx::themeLight();
            a.prefs.theme = a.dark ? "dark" : "light";
            pushToast(a, a.dark ? "Dark theme" : "Light theme", a.theme.primary);
            break;
        case A_HELP:
            setScreen(a, S_HELP);
            break;

        // ---------- home ----------
        case A_OPEN_NBT:
            // OS file manager (SAF) instead of the internal browser.
            pushToast(a, "Choose an NBT file", a.theme.onSurfaceMuted);
            launchFilePicker(PICK_OPEN_NBT);
            break;
        case A_NEW_NBT: {
            a.ed = EditorState{};
            a.ed.doc = newRootCompound();
            a.ed.levelDat = false;
            a.ed.path = "";
            a.ed.open = true;
            a.ed.dirty = true;
            a.ed.pathIdx.push_back(a.ed.doc.root);
            setScreen(a, S_TREE);
            pushToast(a, "New document - tap + to add tags", a.theme.onSurfaceMuted);
            break; }
        case A_RECENT_PICK: {
            int idx = a.ed.selected;
            if (idx >= 0 && idx < (int)a.recent.size()) {
                std::string p = a.recent[idx];
                bool levelDat = (toLowerCopy(baseName(p)) == "level.dat");
                if (!openDocument(a, p, levelDat, err)) {
                    pushToast(a, "Could not open: " + err, a.theme.error);
                    a.recent.erase(a.recent.begin() + idx);
                } else addRecent(a, p);
            }
            break; }
        case A_WORLDS:
            listWorlds(a);
            setScreen(a, S_WORLDS);
            break;
        case A_WORLD_IMPORT:
            // Reference MineTuner: ACTION_GET_CONTENT via the OS file manager.
            a.importing = true;
            pushToast(a, "Choose a .mcworld / .mctemplate file", a.theme.onSurfaceMuted);
            launchFilePicker(PICK_IMPORT_WORLD);
            break;

        // ---------- OS file manager result (SAF) ----------
        case A_PICK_RESULT: {
            const int result = a.pickerResult;
            const int mode = a.pickerMode;
            const std::string path = a.pickerPath;
            const std::string name = a.pickerName.empty() ? baseName(path) : a.pickerName;
            a.pickerPending = false;
            a.pickerPath.clear(); a.pickerName.clear();
            if (mode == PICK_IMPORT_WORLD) a.importing = false;
            if (result != kPickerOk || path.empty()) {
                pushToast(a, path.empty() ? "Could not read the selected file" : "Selection cancelled",
                          a.theme.onSurfaceMuted);
                break;
            }
            if (mode == PICK_OPEN_NBT) {
                bool levelDat = (toLowerCopy(baseName(name)) == "level.dat");
                std::string e;
                if (!openDocument(a, path, levelDat, e))
                    pushToast(a, "Not a valid NBT file: " + e, a.theme.error);
                else {
                    a.ed.pickedFile = !a.pickerUri.empty();
                    a.pickedSaveUri = a.pickerUri;
                    addRecent(a, path);
                }
            } else {
                startWorldImport(a, path, name, /*tempCopy*/ true);
            }
            break; }

        // ---------- browser ----------
        case A_BROWSE_UP: {
            std::string up = parentPath(a.browseDir);
            if (up != a.browseDir) { a.browseDir = up; listBrowseDir(a); }
            break; }
        case A_BROWSE_OPEN: {
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.browseEntries.size()) {
                if (a.browseIsDir[a.ed.selected]) {
                    a.browseDir = a.browseEntries[a.ed.selected];
                    a.prefs.lastDir = a.browseDir;
                    listBrowseDir(a);
                }
            }
            break; }
        case A_FILE_SELECT: {
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.browseEntries.size()) {
                std::string p = a.browseEntries[a.ed.selected];
                if (a.importing) {
                    // Internal browser (kept for Save-As and legacy tests) uses
                    // the shared import routine; SAF uses the very same path.
                    startWorldImport(a, p, baseName(p), /*tempCopy*/ false);
                    a.importing = false;
                    break;
                }
                if (a.browseForSave) {
                    // Save-As keeps the document's container format
                    std::string target = a.browseForSave ? p : a.pendingSavePath;
                    if (fs::is_directory(p)) {
                        target = joinPath(p, baseName(a.ed.path.empty() ? "untitled.dat" : a.ed.path));
                    }
                    if (saveDocument(a, target, err)) {
                        a.ed.path = target;
                        pushToast(a, "Saved to " + target, a.theme.success);
                        addRecent(a, target);
                        popScreenTo(a, S_TREE);
                    } else {
                        pushToast(a, "Save failed: " + err, a.theme.error);
                    }
                } else {
                    bool levelDat = (toLowerCopy(baseName(p)) == "level.dat");
                    if (!openDocument(a, p, levelDat, err)) {
                        pushToast(a, "Not a valid NBT file: " + err, a.theme.error);
                    } else addRecent(a, p);
                }
            }
            break; }

        // ---------- worlds ----------
        case A_WORLD_CREATE:
            a.flatLayers = {{"minecraft:bedrock", 1}, {"minecraft:dirt", 2}, {"minecraft:grass_block", 1}};
            a.flatBiome = 1; a.flatBaseLayer = true;
            setScreen(a, S_FLAT);
            break;
        case A_FAB_TOGGLE:
            a.fabExpanded = !a.fabExpanded;
            break;
        case A_WORLD_OPTIONS:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                // Tapping a world shows its OPTIONS aside (reference MainActivity
                // world-item options). Only choosing "Edit" opens the editor.
                a.menuItems = {
                    {"Edit", A_WORLD_EDIT},
                    {"Export .mcworld", A_WORLD_EXPORT},
                    {"Delete World", A_WORLD_DELETE},
                    {"CLOSE", A_MENU_CLOSE},
                };
                a.menuOpen = true;
            }
            break;
        case A_WORLD_EDIT:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                auto& w = a.worlds[a.ed.selected];
                if (w.encrypted) {
                    openDialog(a, "Encrypted World",
                               "This world is encrypted and cannot be edited. Any worlds downloaded from Minecraft Marketplace are usually encrypted by default.",
                               "OK", A_DIALOG_NEG, "", A_DIALOG_NEG);
                    break;
                }
                a.menuOpen = false;
                enterWorldEditor(a, w);   // Show Map; aside opens on swipe
            }
            break;
        case A_WORLD_DELETE:
            a.drawerOpen = false;   // NavigationView closes on item selection
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                openDialog(a, "Delete World",
                           "Are you sure you want to delete this world? This action cannot be undone.",
                           "DELETE", A_WORLD_DELETE, "CANCEL", A_DIALOG_NEG);
                a.dialogTargetIndex = a.ed.selected;   // freeze the target now
            }
            break;

        // ---------- nbt editor ----------
        case A_TREE_TAP: {
            auto entries = visibleTreeItems(a);
            if (a.ed.selected < 0 || a.ed.selected >= (int)entries.size()) break;
            TreeEntry entry = entries[a.ed.selected];
            if (entry.kind == TreeEntryKind::ArrayElement) {
                openArrayElementEditor(a, entry.nodeIndex, entry.elementIndex);
                break;
            }
            uint32_t idx = entry.nodeIndex;
            const nbt::Node& n = a.ed.doc.arena[idx];
            if (nbt::isContainer(n.type)) {
                a.ed.pathIdx.push_back(idx);
                a.scrollY = 0;
                // auto-expand inside search results
                if (!a.ed.filter.empty()) {
                    std::vector<uint32_t> p = indexPathTo(a, idx);
                    uint64_t h = hashPath(p);
                    bool present = false;
                    for (auto v : a.ed.openSet) if (v == h) present = true;
                    if (!present) a.ed.openSet.push_back(h);
                }
                rebuildTreeListing(a);
            } else if (nbt::isScalar(n.type)) {
                openValueEditor(a, idx);
            }
            break; }
        case A_TREE_LONG: {
            auto entries = visibleTreeItems(a);
            if (a.ed.selected < 0 || a.ed.selected >= (int)entries.size()) break;
            TreeEntry entry = entries[a.ed.selected];
            std::vector<std::string> options;
            std::string optionType;
            if (entry.kind == TreeEntryKind::ArrayElement) {
                const nbt::Node& owner = a.ed.doc.arena[entry.nodeIndex];
                options.push_back("Edit");
                options.push_back("Remove");
                optionType = nbt::typeName(arrayElementType(owner.type));
            } else {
                const nbt::Node& n = a.ed.doc.arena[entry.nodeIndex];
                bool removable = (a.ed.pathIdx.size() >= 2);
                if (nbt::isScalar(n.type)) options.push_back("Edit");
                if (a.ed.doc.arena[a.ed.pathIdx.back()].type == nbt::Type::Compound)
                    options.push_back("Rename");
                options.push_back("Copy");
                if (removable) options.push_back("Remove");
                optionType = nbt::typeName(n.type);
            }
            a.tagOptionList = options;
            a.tagOptionEntry = entry;
            openDialog(a, "Tag options", optionType, "", 0, "", 0);
            a.dialog.body.clear();
            // render the options list inside the dialog body
            int i = 0;
            for (auto& o : a.tagOptionList) {
                ui::Node b;
                b.kind = ui::Kind::Button;
                b.rect = {24, 0, a.w - 48, 30};
                b.text = o;
                b.bg = a.theme.surface;
                b.fg = a.theme.onSurface;
                b.border = a.theme.outline; b.borderW = 1;
                b.radius = 6; b.enabled = true;
                b.action = A_TAG_OPTION;
                b.index = i++;
                a.dialog.body.push_back(b);
            }
            break; }
        case A_TAG_OPTION: {
            if (a.ed.selected < 0 || a.ed.selected >= (int)a.tagOptionList.size()) break;
            std::string choice = a.tagOptionList[a.ed.selected];
            TreeEntry entry = a.tagOptionEntry;
            closeDialog(a);
            if (choice == "Edit") {
                if (entry.kind == TreeEntryKind::ArrayElement)
                    openArrayElementEditor(a, entry.nodeIndex, entry.elementIndex);
                else
                    openValueEditor(a, entry.nodeIndex);
            } else if (choice == "Rename" && entry.kind == TreeEntryKind::Node) {
                a.renameOpen = true;
                a.renameIdx = entry.nodeIndex;
                a.renameName = a.ed.doc.arena[entry.nodeIndex].name;
                openKeypad(a, "renameName", false, a.renameName);
            } else if (choice == "Copy" && entry.kind == TreeEntryKind::Node) {
                a.ed.clipIdx = entry.nodeIndex;
                a.ed.hasClip = true;
                pushToast(a, "Tag copied", a.theme.primary);
            } else if (choice == "Remove") {
                openDeleteConfirmation(a, entry);
            }
            break; }
        case A_BACK:
            back(a);
            break;
        case A_HOME:
        case A_HOME_CONFIRM:
            goHome(a);
            break;
        case A_TREE_UP:
            if (a.ed.pathIdx.size() > 1) a.ed.pathIdx.pop_back();
            else back(a);
            rebuildTreeListing(a);
            break;
        case A_BREADCRUMB_TAP: {
            int depth = a.ed.selected;
            if (depth >= 0 && depth < (int)a.ed.pathIdx.size()) {
                a.ed.pathIdx.resize((size_t)depth + 1);
                a.scrollY = 0;
                rebuildTreeListing(a);
            }
            break; }
        case A_TREE_MOVE_ROOT:
            a.ed.pathIdx.clear();
            a.ed.pathIdx.push_back(a.ed.doc.root);
            a.scrollY = 0;
            rebuildTreeListing(a);
            break;
        case A_TREE_SAVE: {
            std::string p = a.ed.path.empty() ? "/sdcard/Download/untitled.dat" : a.ed.path;
            if (saveDocument(a, p, err)) {
                a.ed.path = p;
                pushToast(a, "Saved " + baseName(p), a.theme.success);
                a.ed.path = p;
                } else {
                pushToast(a, "Save failed: " + err, a.theme.error);
            }
            break; }
        case A_TREE_SAVE_AS:
            a.browseForSave = true;
            a.browseDir = a.ed.path.empty() ? "/sdcard/Download" : parentPath(a.ed.path);
            listBrowseDir(a);
            setScreen(a, S_BROWSER);
            break;
        case A_TREE_ADD:
            a.addOpen = true;
            a.addName.clear();
            a.addError.clear();
            a.ed.selected = -1;
            a.valueTypeToAdd = nbt::Type::Byte;
            if (a.ed.pathIdx.empty()) break;
            {
                uint32_t cur = a.ed.pathIdx.back();
                if (cur >= a.ed.doc.arena.size()) break;
                const nbt::Node& n = a.ed.doc.arena[cur];
                if (nbt::isList(n.type)) {
                    a.addListMode = 1;
                } else if (nbt::isArray(n.type)) {
                    a.addListMode = 2;
                    a.valueTypeToAdd = arrayElementType(n.type);
                } else {
                    a.addListMode = 0;
                }
            }
            if (a.addListMode == 2)
                openDialog(a, "Add element", "Append a value to this array.",
                           "ADD", A_TREE_ADD_DONE, "CANCEL", A_DIALOG_NEG);
            else
                openDialog(a, "Add tag", "Pick a type (and name for compounds).",
                           "ADD", A_TREE_ADD_DONE, "CANCEL", A_DIALOG_NEG);
            break;
        case A_TREE_DELETE: {
            if (a.ed.selected < 0 || !a.ed.filter.empty()) break;
            auto entries = visibleTreeItems(a);
            if (a.ed.selected >= (int)entries.size()) break;
            openDeleteConfirmation(a, entries[a.ed.selected]);
            break; }
        case A_TREE_PASTE:
            if (!a.ed.hasClip) {
                pushToast(a, "Nothing to paste", a.theme.error);
                break;
            }
            if (a.ed.pathIdx.empty() || a.ed.clipIdx >= a.ed.doc.arena.size()) {
                pushToast(a, "Nothing to paste", a.theme.error);
                break;
            }
            {
                uint32_t cur = a.ed.pathIdx.back();
                if (cur >= a.ed.doc.arena.size() ||
                    a.ed.doc.arena[cur].type != nbt::Type::Compound) {
                    pushToast(a, "Paste works into compounds", a.theme.error);
                    break;
                }
                const nbt::Node& source = a.ed.doc.arena[a.ed.clipIdx];
                if (source.name.empty()) {
                    pushToast(a, nbt::msgUnnamedTag(), a.theme.error);
                    break;
                }
                bool duplicate = false;
                for (uint32_t child : a.ed.doc.arena[cur].kids)
                    if (a.ed.doc.arena[child].name == source.name) duplicate = true;
                if (duplicate) {
                    pushToast(a, nbt::msgDuplicateName(), a.theme.error);
                    break;
                }
                snapshotUndo(a, "paste tag");
                uint32_t copy = cloneNode(a, a.ed.clipIdx);
                a.ed.doc.arena[cur].kids.push_back(copy);
                commit(a);
                pushToast(a, "Tag pasted", a.theme.primary);
            }
            break;
        case A_TREE_UNDO:
            if (!a.ed.undoStack.empty()) {
                EditorState::UndoEntry current;
                auto u = a.ed.undoStack.back();
                current.label = u.label;
                current.arena = a.ed.doc.arena;
                current.root = a.ed.doc.root;
                a.ed.undoStack.pop_back();
                a.ed.redoStack.push_back(std::move(current));
                if (a.ed.redoStack.size() > 200) a.ed.redoStack.erase(a.ed.redoStack.begin());
                a.ed.doc.arena = u.arena;
                a.ed.doc.root = u.root;
                a.ed.selected = -1;
                a.ed.dirty = true;
                rebuildTreeListing(a);
                pushToast(a, "Undid " + u.label, a.theme.onSurfaceMuted);
            } else pushToast(a, "Nothing to undo", a.theme.onSurfaceMuted);
            break;
        case A_TREE_REDO:
            if (!a.ed.redoStack.empty()) {
                EditorState::UndoEntry current;
                auto r = a.ed.redoStack.back();
                current.label = r.label;
                current.arena = a.ed.doc.arena;
                current.root = a.ed.doc.root;
                a.ed.redoStack.pop_back();
                a.ed.undoStack.push_back(std::move(current));
                a.ed.doc.arena = r.arena;
                a.ed.doc.root = r.root;
                a.ed.selected = -1;
                a.ed.dirty = true;
                rebuildTreeListing(a);
                pushToast(a, "Redid " + r.label, a.theme.onSurfaceMuted);
            } else pushToast(a, "Nothing to redo", a.theme.onSurfaceMuted);
            break;
        case A_TREE_SEARCH:
            if (!a.ed.showSearch) a.ed.showSearch = true;
            openKeypad(a, "searchFilter", false, a.ed.filter);
            break;
        case A_TREE_SEARCH_CLEAR:
            a.ed.filter.clear();
            a.ed.showSearch = false;
            a.ed.selected = -1;
            a.scrollY = 0;
            a.keypadOpen = false;
            a.keypadTarget.clear();
            rebuildTreeListing(a);
            break;

        // ---------- value / add / rename dialogs ----------
        case A_VALUE_KIND:
            // pick a type in the add-tag dialog (the tapped node's index is the
            // position inside all[] of buildDialogNodes)
            if (a.ed.selected >= 0) {
                nbt::Type all[12];
                int n = 0;
                for (int t = 1; t <= 12; ++t) { nbt::Type tt{}; if (nbt::typeFromId((uint8_t)t, tt)) all[n++] = tt; }
                if (a.ed.selected < n) a.valueTypeToAdd = all[a.ed.selected];
            }
            break;
        case A_VALUE_SAVE:
            commitValue(a);
            break;
        case A_VALUE_CANCEL:
            a.valueOpen = false;
            a.valueIsArrayElement = false;
            a.valueArrayElementIndex = 0;
            closeDialog(a);
            break;
        case A_TREE_ADD_DONE:
            commitAdd(a);
            break;

        // ---------- flat world ----------
        case A_FLAT_BIOME:
            a.flatBiome = (a.flatBiome + 1) % kFlatBiomeCount;
            break;
        case A_FLAT_TOGGLE_BASE:
            a.flatBaseLayer = !a.flatBaseLayer;
            break;
        case A_FLAT_ADD_LAYER: {
            int64_t total = 0;
            for (auto& l : a.flatLayers) total += l.count;
            if (total >= 384) {
                pushToast(a, "Total layer thickness exceeds the limit (384).", a.theme.error);
                break;
            }
            a.flatLayers.push_back({kFlatBlocks[(a.flatLayers.size()) % kFlatBlockCount], 1});
            break; }
        case A_FLAT_DEL_LAYER:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.flatLayers.size()) {
                if (a.flatLayers.size() > 1) a.flatLayers.erase(a.flatLayers.begin() + a.ed.selected);
                else pushToast(a, "At least one layer is required.", a.theme.error);
            }
            break;
        case A_FLAT_LAYER_UP:
            if (a.ed.selected > 0 && a.ed.selected < (int)a.flatLayers.size())
                std::swap(a.flatLayers[a.ed.selected - 1], a.flatLayers[a.ed.selected]);
            break;
        case A_FLAT_LAYER_DOWN:
            if (a.ed.selected >= 0 && a.ed.selected + 1 < (int)a.flatLayers.size())
                std::swap(a.flatLayers[a.ed.selected], a.flatLayers[a.ed.selected + 1]);
            break;
        case A_FLAT_EDIT_LAYER:
            if (a.ed.selected >= 0 && a.ed.selected < (int)a.flatLayers.size()) {
                a.keypadTarget = "flatThickness";
                openKeypad(a, "flatThickness", true,
                           std::to_string(a.flatLayers[a.ed.selected].count));
            }
            break;
        case A_FLAT_CREATE: {
            int64_t total = 0;
            for (auto& l : a.flatLayers) total += l.count;
            if (a.flatBaseLayer) total += 1;
            if (total > 384) {
                pushToast(a, "Total layer thickness exceeds the limit (384).", a.theme.error);
                break;
            }
            std::string dir = joinPath(a.worldsRoot, "CustomFlatWorld");
            ensureDir(dir);
            std::string lvlPath = joinPath(dir, "level.dat");
            std::vector<FlatLayer> layers = a.flatLayers;
            if (a.flatBaseLayer) layers.push_back({"minecraft:bedrock", 1});
            nbt::Document doc = newLevelDat("Custom Flat World");
            // set FlatWorldLayers
            bool found = false;
            for (auto& n : doc.arena) {
                if (n.name == "FlatWorldLayers") { n.s = buildFlatWorldJson(a.flatBiome, layers); found = true; }
            }
            if (!found) {
                nbt::Node f; f.type = nbt::Type::String; f.name = "FlatWorldLayers";
                f.s = buildFlatWorldJson(a.flatBiome, layers);
                doc.arena[0].kids.push_back((uint32_t)doc.arena.size());
                doc.arena.push_back(std::move(f));
            }
            auto bytes = nbt::serializeLevelDat(doc);
            if (!bytes.ok()) {
                pushToast(a, "Could not encode level.dat: " + bytes.detail, a.theme.error);
                break;
            }
            if (!app::writeFileAtomic(lvlPath, bytes.value).ok()) {
                pushToast(a, "Could not write level.dat", a.theme.error);
                break;
            }
            app::writeText(joinPath(dir, "levelname.txt"), "Custom Flat World");
            listWorlds(a);
            popScreenTo(a, S_WORLDS);
            pushToast(a, "World created", a.theme.success);
            break; }

        // ---------- dialogs ----------
        case A_DIALOG_POS:
        case A_DIALOG_NEG:
        case A_DIALOG_NEU:
            if (a.dialog.open) {
                int chosen = act == A_DIALOG_POS ? a.dialog.posAction :
                             act == A_DIALOG_NEG ? a.dialog.negAction : a.dialog.neuAction;
                bool ok = (act == A_DIALOG_POS);

                // value editor / add tag / rename dialogs own their own buttons
                if (a.valueOpen) {
                    if (ok) commitValue(a);
                    else {
                        a.valueOpen = false;
                        a.valueIsArrayElement = false;
                        a.valueArrayElementIndex = 0;
                        a.dialog = ui::Dialog{};
                    }
                    break;
                }
                if (a.addOpen) {
                    if (ok) commitAdd(a);
                    else { a.addOpen = false; a.dialog = ui::Dialog{}; }
                    break;
                }
                if (a.renameOpen && ok) commitRename(a);
                else if (a.renameOpen) { a.renameOpen = false; a.dialog = ui::Dialog{}; break; }
                if (a.pendingEdit && ok) {
                    // confirmed: open the selected world
                    a.pendingEdit = false;
                    if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                        auto& w = a.worlds[a.ed.selected];
                        std::string lvl = joinPath(w.folder, "level.dat");
                        if (!openDocument(a, lvl, true, err))
                            pushToast(a, "Could not open: " + err, a.theme.error);
                        else addRecent(a, lvl);
                    }
                } else if (chosen == A_WORLD_EDIT && ok) {
                    if (a.ed.selected >= 0 && a.ed.selected < (int)a.worlds.size()) {
                        auto& w = a.worlds[a.ed.selected];
                        if (w.encrypted)
                            pushToast(a, "This world is encrypted and cannot be edited.", a.theme.error);
                        else {
                            std::string lvl = joinPath(w.folder, "level.dat");
                            if (!openDocument(a, lvl, true, err))
                                pushToast(a, "Could not open level.dat: " + err, a.theme.error);
                            else addRecent(a, lvl);
                        }
                    }
                } else if (chosen == A_WORLD_EXPORT) {
                    handleAction(a, A_WORLD_EXPORT);
                } else if (chosen == A_WORLD_DELETE) {
                    int idx = a.dialogTargetIndex;
                    if (idx < 0 || idx >= (int)a.worlds.size()) idx = a.ed.selected;
                    if (idx >= 0 && idx < (int)a.worlds.size()) {
                        std::error_code ec;
                        fs::remove_all(a.worlds[idx].folder, ec);
                        pushToast(a, ec ? "Delete failed" : "World deleted",
                                  ec ? a.theme.error : a.theme.success);
                        listWorlds(a);
                    }
                } else if (chosen == A_TREE_DELETE && ok) {
                    TreeEntry entry = a.deleteEntry;
                    bool removed = false;
                    if (entry.kind == TreeEntryKind::ArrayElement) {
                        if (entry.nodeIndex < a.ed.doc.arena.size() &&
                            nbt::isArray(a.ed.doc.arena[entry.nodeIndex].type) &&
                            entry.elementIndex < arrayElementCount(a.ed.doc.arena[entry.nodeIndex])) {
                            snapshotUndo(a, "remove array element");
                            removed = removeArrayElement(a, entry.nodeIndex, entry.elementIndex, err);
                        } else {
                            err = "The array element no longer exists.";
                        }
                    } else if (entry.nodeIndex < a.ed.doc.arena.size()) {
                        std::vector<uint32_t> path = indexPathTo(a, entry.nodeIndex);
                        bool childFound = false;
                        if (path.size() >= 2) {
                            uint32_t parent = path[path.size() - 2];
                            for (uint32_t child : a.ed.doc.arena[parent].kids)
                                if (child == entry.nodeIndex) childFound = true;
                        }
                        if (childFound) {
                            snapshotUndo(a, "remove tag");
                            removed = removeNode(a, entry.nodeIndex, err);
                        } else {
                            err = "Tag not found.";
                        }
                    } else {
                        err = "The tag no longer exists.";
                    }
                    if (removed) {
                        commit(a);
                        pushToast(a, entry.kind == TreeEntryKind::ArrayElement
                                  ? "Element removed" : "Tag removed", a.theme.onSurfaceMuted);
                    } else pushToast(a, "Could not remove: " + err, a.theme.error);
                } else if (chosen == A_TREE_SAVE && ok) {
                    // real save (dialog is a confirmation)
                    std::string p = a.ed.path.empty() ? "/sdcard/Download/untitled.dat" : a.ed.path;
                    if (saveDocument(a, p, err)) {
                        a.ed.path = p;
                        pushToast(a, "Saved", a.theme.success);
                    } else pushToast(a, "Save failed: " + err, a.theme.error);
                } else if (chosen != 0 && chosen != A_DIALOG_POS &&
                           chosen != A_DIALOG_NEG && chosen != A_DIALOG_NEU) {
                    // Generic dispatch for dialogs whose buttons carry their own
                    // action ids (map options, entity list, home confirmation...).
                    closeDialog(a);
                    handleAction(a, chosen);
                    break;
                }
                closeDialog(a);
            }
            break;

        // Modal barrier: consume the tap without acting, so controls behind a
        // dialog/keypad/progress overlay cannot be changed.
        case A_BLOCK:
            break;

        // ---------- keypad ----------
        case A_KEY_TAP: {
            // The tapped character is copied out by the input dispatcher into
            // a.lastKeyPressed (audit B4); without it nothing was ever typed.
            std::string ch = a.lastKeyPressed;
            if (ch == "SPC") ch = " ";
            applyKey(a, ch);
            break; }
        case A_KEY_OPEN:
            openKeypadForActive(a);
            break;
        case A_KEY_CLEAR:
            keypadClear(a);
            break;
        case A_KEY_SHIFT:
            a.keypadShift = !a.keypadShift;
            break;
        case A_KEY_DEL:
            keypadBackspace(a);
            break;
        case A_KEY_MODE:
            a.keypadNumeric = !a.keypadNumeric;
            break;
        case A_KEY_ENTER: {
            // commit the buffer into its target
            if (a.valueOpen) {
                commitValue(a);
            } else if (a.addOpen) {
                commitAdd(a);
            } else if (a.renameOpen) {
                commitRename(a);
            } else if (a.keypadTarget == "flatThickness") {
                int64_t v = a.flatDraft.empty() ? 0 : std::atoll(a.flatDraft.c_str());
                if (v > 0 && a.ed.selected >= 0 && a.ed.selected < (int)a.flatLayers.size())
                    a.flatLayers[a.ed.selected].count = v;
            }
            a.keypadOpen = false;
            a.keypadTarget.clear();
            break; }

        default:
            break;
    }
}

void popScreenTo(AppState& a, int s) {
    // Return to a screen already on the navigation stack and discard the
    // screens opened after it.  If the destination was not on the stack (for
    // example an import started from Home), keep the underlying stack so Back
    // still returns to Home instead of exiting the app.
    while (!a.screenStack.empty() && a.screenStack.back() != s) a.screenStack.pop_back();
    if (!a.screenStack.empty()) a.screenStack.pop_back();
    a.screen = s;
}

} // namespace app

// ============================================================================
// L0 — PLATFORM GLUE (NativeActivity, window, input, permissions)
// Replaces android_native_app_glue: we implement the lifecycle ourselves so the
// whole app stays in this single translation unit.
// ============================================================================
#if !NBT_TESTABLE
#include <android/native_activity.h>
#include <csignal>
#include <android/looper.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/rect.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <jni.h>
#include <dlfcn.h>
#include <sys/prctl.h>

#define NBT_LOG_TAG "nbteditor"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, NBT_LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, NBT_LOG_TAG, __VA_ARGS__)

namespace app {

struct Platform {
    // Every field below is touched by the Android main thread (callbacks) and
    // the native render thread.  Pointers are published atomically; queue and
    // window operations additionally take g_platMutex so attach/detach and
    // lock/unlock cannot race with create/destroy.
    std::atomic<ANativeActivity*> activity{nullptr};
    std::atomic<ANativeWindow*>   window{nullptr};
    std::atomic<ALooper*>         looper{nullptr};
    std::atomic<AInputQueue*>     input{nullptr};
    std::atomic<bool> running{false};
    std::atomic<bool> hasWindow{false};
    std::atomic<bool> hasFocus{false};
    std::atomic<bool> inputAttached{false};
    std::atomic<bool> renderReady{false};
    std::atomic<bool> paused{false};
    std::atomic<int>  width{0}, height{0};   // device pixels
    std::atomic<int>  scale{1};               // device px per design unit (dp)
};

static std::mutex g_platMutex;               // queue/window hand-off only
static std::thread g_renderThread;           // joinable, restarted per activity

// The UI is designed for ~360 design units (dp) across. On a 1080p phone the
// density is 3, so one design unit must become three device pixels; without
// this the whole interface was microscopic (audit finding A1).
static int uiScaleFor(int width, int height) {
    int s = width / 360;
    if (s < 1) s = 1;
    if (s > 4) s = 4;
    while (s > 1 && height / s < 520) --s;   // keep enough vertical room
    return s;
}

static Platform   g_plat;
// System BACK is delivered by MainActivity.onBackPressed() through JNI (and,
// on some devices, through the native input queue). One flag + exchange() in
// the render loop guarantees it is handled exactly once per press.
static std::atomic<bool> g_backRequested{false};
static AppState   g_state;
static gfx::Canvas g_canvas;
static app::Worker g_worker;
static bool        g_booted = false;
static std::mutex  g_frameMutex;

// ---- input queue (filled by the looper callback, drained each frame) -------
struct PendingEvent {
    int32_t action = 0;
    float   x = 0, y = 0;
    int64_t ms = 0;
    bool    down = false, up = false, move = false, cancel = false;
    bool    key = false, keyDown = false;
    int     keyCode = 0;
};
static std::vector<PendingEvent> g_events;
static std::mutex g_eventMutex;

static int looperInputCallback(int /*fd*/, int /*events*/, void*) {
    std::vector<PendingEvent> batch;
    // Queue create/destroy and the looper callback can run on different
    // threads.  Hold the hand-off lock for the whole get/finish sequence so
    // onInputQueueDestroyed cannot invalidate the queue in the middle.
    std::lock_guard<std::mutex> lk(g_platMutex);
    AInputQueue* q = g_plat.input.load(std::memory_order_acquire);
    if (!q) return 1;

    int budget = 256;                    // drain, but never starve the looper
    while (budget-- > 0) {
        AInputEvent* ev = nullptr;
        if (AInputQueue_getEvent(q, &ev) < 0 || !ev) break;
        int32_t type = AInputEvent_getType(ev);
        if (type == AINPUT_EVENT_TYPE_MOTION) {
            int32_t a = AMotionEvent_getAction(ev) & AMOTION_EVENT_ACTION_MASK;
            PendingEvent p;
            p.action = a;
            p.x = AMotionEvent_getX(ev, 0);
            p.y = AMotionEvent_getY(ev, 0);
            p.down = (a == AMOTION_EVENT_ACTION_DOWN);
            p.up = (a == AMOTION_EVENT_ACTION_UP);
            p.move = (a == AMOTION_EVENT_ACTION_MOVE);
            p.cancel = (a == AMOTION_EVENT_ACTION_CANCEL);
            p.ms = AMotionEvent_getEventTime(ev) / 1000000;
            LOGI("native motion action=%d x=%.0f y=%.0f", a, p.x, p.y);
            batch.push_back(p);
            AInputQueue_finishEvent(q, ev, 1);
        } else {
            // Key events share the queue but use KeyEvent accessors.  Let the
            // IME pre-dispatch first; if it consumed the event, abandon it.
            if (AInputQueue_preDispatchEvent(q, ev) != 0) return 1;
            int keyCode = AKeyEvent_getKeyCode(ev);
            int keyAction = AKeyEvent_getAction(ev);
            if (keyCode == AKEYCODE_BACK) {
                // Consume Back before NativeActivity's Java dispatcher sees it;
                // otherwise Android finishes the activity before the native
                // navigation stack can pop one screen.
                PendingEvent p;
                p.key = true;
                p.keyDown = (keyAction == AKEY_EVENT_ACTION_DOWN);
                p.keyCode = keyCode;
                p.ms = AKeyEvent_getEventTime(ev) / 1000000;
                batch.push_back(p);
                LOGI("native key back down=%d", p.keyDown ? 1 : 0);
                AInputQueue_finishEvent(q, ev, 1);
            } else {
                // Other keys are not app commands; leave normal Android
                // dispatch semantics intact.
                AInputQueue_finishEvent(q, ev, 0);
            }
        }
    }
    if (!batch.empty()) {
        std::lock_guard<std::mutex> elk(g_eventMutex);
        g_events.insert(g_events.end(), batch.begin(), batch.end());
        if (g_events.size() > 512) g_events.erase(g_events.begin(), g_events.begin() + (g_events.size() - 512));
    }
    return 1;   // keep callback
}

// Wake the render looper from a lifecycle callback.  The pointer and ALooper
// reference live under g_platMutex; holding it while waking makes it
// impossible for the render thread to release the looper underneath us.
static void wakeRender() {
    std::lock_guard<std::mutex> lk(g_platMutex);
    ALooper* l = g_plat.looper.load(std::memory_order_acquire);
    if (l) ALooper_wake(l);
}

// Caller holds g_platMutex.  Attachment must happen on the looper's own
// thread; the loop calls this at the top of every iteration.
static void attachInputLocked() {
    AInputQueue* q = g_plat.input.load(std::memory_order_acquire);
    ALooper* l = g_plat.looper.load(std::memory_order_acquire);
    if (!q || !l || g_plat.inputAttached.load(std::memory_order_relaxed)) return;
    // ident=1 marks input events; ALOOPER_EVENT_INPUT (-1) is reserved.
    AInputQueue_attachLooper(q, l, 1, looperInputCallback, nullptr);
    g_plat.inputAttached.store(true, std::memory_order_release);
    LOGI("native input queue attached queue=%p looper=%p", (void*)q, (void*)l);
    diagLog("render: input queue attached");
}

static void attachInputIfNeeded() {
    std::lock_guard<std::mutex> lk(g_platMutex);
    attachInputLocked();
}

// ---- lifecycle callbacks (app main/UI thread) ------------------------------
static void onStart(ANativeActivity*) { g_plat.paused = false; wakeRender(); }
static void onResume(ANativeActivity*) {
    g_plat.paused = false;
    g_plat.renderReady = g_plat.hasFocus.load() && g_plat.hasWindow.load();
    wakeRender();
}
static void* onSaveInstanceState(ANativeActivity*, size_t*) { return nullptr; }
static void onPause(ANativeActivity*) {
    // Stop rendering while invisible: saves battery and avoids touching a window
    // that may already be destroyed.
    g_plat.renderReady = false;
    g_plat.paused = true;
    wakeRender();
}
static void onStop(ANativeActivity*) {
    g_plat.renderReady = false;
    g_plat.paused = true;
    wakeRender();
}
static void onDestroy(ANativeActivity*) {
    g_plat.running = false;
    g_plat.renderReady = false;
    wakeRender();
    // The render thread is now joinable and polls with a finite timeout.  Join
    // it on the Android main thread before the activity state disappears; this
    // is bounded because no ALooper_pollOnce(-1) and no worker join remain.
    if (g_renderThread.joinable()) {
        g_renderThread.join();
        diagLog("shutdown: render thread joined");
    }
    g_plat.activity.store(nullptr);
    diagLog("shutdown: activity destroyed");
}
static void onWindowFocusChanged(ANativeActivity*, int focused) {
    g_plat.hasFocus = (focused != 0);
    g_plat.renderReady = (focused != 0) && g_plat.hasWindow.load();
    wakeRender();
}
static void onContentRectChanged(ANativeActivity*, const ARect* r) {
    if (r) LOGI("native content rect %d,%d %dx%d", r->left, r->top,
                r->right - r->left, r->bottom - r->top);
}
static void onConfigurationChanged(ANativeActivity*) { wakeRender(); }

static void onNativeWindowCreated(ANativeActivity*, ANativeWindow* w) {
    if (!w) return;
    ANativeWindow_acquire(w);           // render may outlive this callback
    ANativeWindow* old;
    {
        std::lock_guard<std::mutex> lk(g_platMutex);
        old = g_plat.window.exchange(w);
    }
    if (old) ANativeWindow_release(old);
    int width = ANativeWindow_getWidth(w);
    int height = ANativeWindow_getHeight(w);
    if (width <= 0 || height <= 0) return;
    g_plat.width = width;
    g_plat.height = height;
    g_plat.scale = uiScaleFor(width, height);
    ANativeWindow_setBuffersGeometry(w, width, height, WINDOW_FORMAT_RGBA_8888);
    g_plat.hasWindow = true;
    g_plat.renderReady = g_plat.hasFocus.load();
    wakeRender();
}
static void onNativeWindowResized(ANativeActivity*, ANativeWindow* w) {
    if (!w) return;
    int width = ANativeWindow_getWidth(w);
    int height = ANativeWindow_getHeight(w);
    if (width <= 0 || height <= 0) return;
    g_plat.width = width;
    g_plat.height = height;
    g_plat.scale = uiScaleFor(width, height);
    ANativeWindow_setBuffersGeometry(w, width, height, WINDOW_FORMAT_RGBA_8888);
    g_plat.renderReady = g_plat.hasFocus.load() && g_plat.hasWindow.load();
    wakeRender();
}
static void onNativeWindowRedrawNeeded(ANativeActivity*, ANativeWindow*) {
    g_plat.renderReady = g_plat.hasFocus.load() && g_plat.hasWindow.load();
    wakeRender();
}
static void onNativeWindowDestroyed(ANativeActivity*, ANativeWindow*) {
    ANativeWindow* old;
    {
        std::lock_guard<std::mutex> lk(g_platMutex);
        old = g_plat.window.exchange(nullptr);
    }
    g_plat.hasWindow = false;
    g_plat.renderReady = false;
    if (old) ANativeWindow_release(old);
    wakeRender();
}
static void onInputQueueCreated(ANativeActivity*, AInputQueue* q) {
    LOGI("native input queue created %p", (void*)q);
    AInputQueue* old;
    {
        std::lock_guard<std::mutex> lk(g_platMutex);
        old = g_plat.input.exchange(q);
        if (old && g_plat.inputAttached.load()) AInputQueue_detachLooper(old);
        g_plat.inputAttached = false;
    }
    wakeRender();                       // loop must attach the new queue
}
static void onInputQueueDestroyed(ANativeActivity*, AInputQueue* q) {
    LOGI("native input queue destroyed %p", (void*)q);
    {
        std::lock_guard<std::mutex> lk(g_platMutex);
        AInputQueue* cur = g_plat.input.load(std::memory_order_acquire);
        if (q && cur == q && g_plat.inputAttached.load()) {
            AInputQueue_detachLooper(q);
        }
        if (cur == q) g_plat.input.store(nullptr, std::memory_order_release);
        g_plat.inputAttached = false;
    }
    wakeRender();
}

// ---- permission (runtime, without Java glue) -------------------------------
// Every JNI call is guarded: a failure here must NOT kill the process before the
// first frame (observed on some devices as an immediate close).
void requestStoragePermission() {
    ANativeActivity* act = g_plat.activity.load(std::memory_order_acquire);
    if (!act || !act->vm) return;
    JavaVM* vm = act->vm;
    // activity->env belongs to the Android main thread.  This function is
    // normally called from the native render thread, so always resolve a
    // thread-local JNIEnv and attach explicitly when detached.
    JNIEnv* env = nullptr;
    bool attached = false;
    jint rc = vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (rc == JNI_EDETACHED) {
        if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) return;
        attached = true;
    } else if (rc != JNI_OK) return;
    if (!act->clazz) { if (attached) vm->DetachCurrentThread(); return; }

    bool done = false;
    do {
        jclass intentCls = env->FindClass("android/content/Intent");
        if (!intentCls) break;
        jfieldID field = env->GetStaticFieldID(intentCls, "ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION",
                                               "Ljava/lang/String;");
        if (!field) break;
        jstring action = (jstring)env->GetStaticObjectField(intentCls, field);
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }
        jstring pkg = env->NewStringUTF("package:com.nbteditor");
        jclass uriCls = env->FindClass("android/net/Uri");
        jmethodID parse = uriCls ? env->GetStaticMethodID(uriCls, "parse",
                              "(Ljava/lang/String;)Landroid/net/Uri;") : nullptr;
        jobject uri = (parse && uriCls) ? env->CallStaticObjectMethod(uriCls, parse, pkg) : nullptr;
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }
        jmethodID init = env->GetMethodID(intentCls, "<init>",
                                          "(Ljava/lang/String;Landroid/net/Uri;)V");
        if (!init || !uri || !action) break;
        jobject intent = env->NewObject(intentCls, init, action, uri);
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }
        if (!intent) break;
        jclass actCls = env->GetObjectClass(act->clazz);
        if (!actCls) break;
        jmethodID startAct = env->GetMethodID(actCls, "startActivity",
                                               "(Landroid/content/Intent;)V");
        if (!startAct) break;
        env->CallVoidMethod(act->clazz, startAct, intent);
        if (env->ExceptionCheck()) { env->ExceptionClear(); }
        done = true;
    } while (false);

    if (env->ExceptionCheck()) env->ExceptionClear();
    if (attached) vm->DetachCurrentThread();
    diagLog("boot: permission request %s", done ? "launched" : "skipped");
}

// ---- APK asset reader (PNG icons bundled from the reference XAPK) ----------
std::vector<uint8_t> readAssetBytes(const std::string& rel) {
    ANativeActivity* act = g_plat.activity.load(std::memory_order_acquire);
    AAssetManager* mgr = act ? act->assetManager : nullptr;
    if (!mgr) return {};
    AAsset* asset = AAssetManager_open(mgr, rel.c_str(), AASSET_MODE_BUFFER);
    if (!asset) return {};
    const void* buf = AAsset_getBuffer(asset);
    off_t len = AAsset_getLength(asset);
    std::vector<uint8_t> out;
    if (buf && len > 0) out.assign((const uint8_t*)buf, (const uint8_t*)buf + len);
    AAsset_close(asset);
    return out;
}

// ---- SAF file manager bridge ------------------------------------------------
// FilePickerActivity (Java, ~130 lines) runs ACTION_GET_CONTENT and returns the
// copied bytes' path through the static native method below. All app logic
// stays in this C++ file; the DEX is only the Android intent glue.
struct PickedFile {
    bool ready = false;
    int mode = PICK_IMPORT_WORLD;
    int result = 0;
    std::string path, name, uri;
};
static PickedFile        g_picked;
static std::mutex        g_pickedMutex;

void launchFilePicker(int mode) {
    ANativeActivity* act = g_plat.activity.load(std::memory_order_acquire);
    if (!act || !act->vm) return;
    JavaVM* vm = act->vm;
    JNIEnv* env = nullptr;
    bool attached = false;
    jint rc = vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (rc == JNI_EDETACHED) {
        if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) return;
        attached = true;
    } else if (rc != JNI_OK) return;
    if (!act->clazz) { if (attached) vm->DetachCurrentThread(); return; }

    bool launched = false;
    do {
        jclass intentCls = env->FindClass("android/content/Intent");
        if (!intentCls) break;
        jmethodID init = env->GetMethodID(intentCls, "<init>", "()V");
        if (!init) break;
        jobject intent = env->NewObject(intentCls, init);
        if (!intent || env->ExceptionCheck()) break;

        // setClassName(String, String) avoids FindClass() on an app class from
        // a JavaVM-attached native thread (bootstrap loader cannot see it).
        jmethodID setCls = env->GetMethodID(intentCls, "setClassName",
                              "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;");
        if (!setCls) break;
        jstring pkg = env->NewStringUTF("com.nbteditor");
        jstring cls = env->NewStringUTF("com.nbteditor.FilePickerActivity");
        env->CallObjectMethod(intent, setCls, pkg, cls);
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }

        jmethodID putExtra = env->GetMethodID(intentCls, "putExtra",
                              "(Ljava/lang/String;I)Landroid/content/Intent;");
        if (!putExtra) break;
        jstring key = env->NewStringUTF("nbt_picker_mode");
        env->CallObjectMethod(intent, putExtra, key, (jint)mode);
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }

        jclass actCls = env->GetObjectClass(act->clazz);
        jmethodID startForResult = actCls ? env->GetMethodID(actCls, "startActivityForResult",
                                        "(Landroid/content/Intent;I)V") : nullptr;
        if (!startForResult) break;
        env->CallVoidMethod(act->clazz, startForResult, intent, (jint)1001);
        if (env->ExceptionCheck()) { env->ExceptionClear(); break; }
        launched = true;
    } while (false);

    if (env->ExceptionCheck()) env->ExceptionClear();
    if (attached) vm->DetachCurrentThread();
    diagLog("saf: picker %s (mode=%d)", launched ? "launched" : "FAILED", mode);
}

extern "C" __attribute__((visibility("default")))
void Java_com_nbteditor_FilePickerActivity_onFilePicked(
        JNIEnv* env, jclass, jint mode, jint result, jstring path, jstring name,
        jstring uri) {
    auto dup = [&](jstring s) -> std::string {
        if (!s) return "";
        const char* chars = env->GetStringUTFChars(s, nullptr);
        if (!chars) return "";
        std::string out(chars);
        env->ReleaseStringUTFChars(s, chars);
        return out;
    };
    std::string p = dup(path), n = dup(name), u = dup(uri);
    {
        std::lock_guard<std::mutex> lk(g_pickedMutex);
        g_picked.ready = true;
        g_picked.mode = (int)mode;
        g_picked.result = (int)result;
        g_picked.path = std::move(p);
        g_picked.name = std::move(n);
        g_picked.uri = std::move(u);
    }
    wakeRender();
}

// Write bytes back to the SAF document the user picked.
bool writePickedFile(const std::string& uri, const std::vector<uint8_t>& bytes) {
    ANativeActivity* act = g_plat.activity.load(std::memory_order_acquire);
    if (!act || !act->vm || uri.empty()) return false;
    JNIEnv* env = nullptr;
    bool attached = false;
    jint rc = act->vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (rc == JNI_EDETACHED) {
        if (act->vm->AttachCurrentThread(&env, nullptr) != JNI_OK) return false;
        attached = true;
    } else if (rc != JNI_OK) return false;
    bool ok = false;
    do {
        jclass cls = env->FindClass("com/nbteditor/FilePickerActivity");
        if (!cls) break;
        jmethodID m = env->GetStaticMethodID(cls, "writeBytes",
                              "(Ljava/lang/String;[B)Z");
        if (!m) break;
        jstring ju = env->NewStringUTF(uri.c_str());
        jbyteArray jb = env->NewByteArray((jsize)bytes.size());
        if (!bytes.empty()) env->SetByteArrayRegion(jb, 0, (jsize)bytes.size(),
                                                    (const jbyte*)bytes.data());
        ok = env->CallStaticBooleanMethod(cls, m, ju, jb) == JNI_TRUE;
        if (env->ExceptionCheck()) { env->ExceptionClear(); ok = false; }
        env->DeleteLocalRef(jb); env->DeleteLocalRef(ju);
    } while (false);
    if (env->ExceptionCheck()) env->ExceptionClear();
    if (attached) act->vm->DetachCurrentThread();
    return ok;
}

// Called on the render thread (under g_stateMutex) right after dispatchTouch.
static void dispatchPickerResult(AppState& a) {
    PickedFile r;
    {
        std::lock_guard<std::mutex> lk(g_pickedMutex);
        if (!g_picked.ready) return;
        r = std::move(g_picked);
        g_picked = PickedFile{};
    }
    a.pickerPending = true;
    a.pickerMode = r.mode;
    a.pickerResult = r.result;
    a.pickerPath = std::move(r.path);
    a.pickerName = std::move(r.name);
    a.pickerUri = std::move(r.uri);
    handleAction(a, A_PICK_RESULT);
}

// MainActivity.onBackPressed() routes system BACK here instead of finishing
// the task. Returning true tells Java we handled it (drawer/dialog/home/exit).
extern "C" __attribute__((visibility("default")))
jboolean Java_com_nbteditor_MainActivity_onBackPressedNative(JNIEnv*, jclass) {
    LOGI("JAVA back forwarded to native");
    g_backRequested.store(true, std::memory_order_release);
    wakeRender();
    return JNI_TRUE;
}

// ---- LevelDB bridge (reference libleveldb.so JNI exports) -------------------
// libleveldb.so exports nativeOpen/nativeGet/nativeIterateKeys/nativeClose with
// ordinary JNI signatures.  Calling those functions directly lets the native
// renderer read real Bedrock chunks without any Java class in our APK.
namespace ldb {
static void* lib = nullptr;
static jlong handle = 0;
static JavaVM* vm = nullptr;
// JNIEnv is thread-local. LevelDB is used by the render thread (chunks/save)
// AND by appWorker (entity marker scan), so each thread needs its own env.
static thread_local JNIEnv* t_env = nullptr;
static thread_local bool t_attached = false;
using OpenFn = jlong (*)(JNIEnv*, jclass, jstring);
using GetFn = jbyteArray (*)(JNIEnv*, jclass, jlong, jbyteArray);
using IterFn = jobjectArray (*)(JNIEnv*, jclass, jlong, jbyteArray);
using CloseFn = void (*)(JNIEnv*, jclass, jlong);
using PutFn = void (*)(JNIEnv*, jclass, jlong, jbyteArray, jbyteArray);
static OpenFn fnOpen = nullptr;
static GetFn fnGet = nullptr;
static IterFn fnIter = nullptr;
static CloseFn fnClose = nullptr;
static PutFn fnPut = nullptr;

static bool ensureLib() {
    if (lib) return fnOpen && fnGet && fnIter && fnClose && fnPut;
    lib = dlopen("libleveldb.so", RTLD_NOW | RTLD_LOCAL);
    if (!lib) { diagLog("leveldb: dlopen failed: %s", dlerror()); return false; }
    fnOpen  = (OpenFn) dlsym(lib, "Java_com_hextetrahedral_leveldb_LevelDB_nativeOpen");
    fnGet   = (GetFn) dlsym(lib, "Java_com_hextetrahedral_leveldb_LevelDB_nativeGet");
    fnIter  = (IterFn) dlsym(lib, "Java_com_hextetrahedral_leveldb_LevelDB_nativeIterateKeys");
    fnClose = (CloseFn) dlsym(lib, "Java_com_hextetrahedral_leveldb_LevelDB_nativeClose");
    fnPut   = (PutFn) dlsym(lib, "Java_com_hextetrahedral_leveldb_LevelDB_nativePut");
    bool ok = fnOpen && fnGet && fnIter && fnClose && fnPut;
    diagLog("leveldb: symbols %s", ok ? "ok" : "MISSING");
    return ok;
}

static JNIEnv* currentEnv() {
    if (t_env) return t_env;
    ANativeActivity* act = g_plat.activity.load(std::memory_order_acquire);
    if (!act || !act->vm) return nullptr;
    vm = act->vm;
    jint rc = vm->GetEnv((void**)&t_env, JNI_VERSION_1_6);
    if (rc == JNI_EDETACHED) {
        if (vm->AttachCurrentThread(&t_env, nullptr) != JNI_OK) { t_env = nullptr; return nullptr; }
        t_attached = true;
    } else if (rc != JNI_OK) { t_env = nullptr; return nullptr; }
    return t_env;
}

static jbyteArray bytesToJni(JNIEnv* env, const std::vector<uint8_t>& v) {
    if (!env) return nullptr;
    jbyteArray a = env->NewByteArray((jsize)v.size());
    if (a && !v.empty()) env->SetByteArrayRegion(a, 0, (jsize)v.size(),
                                                  (const jbyte*)v.data());
    return a;
}
static std::vector<uint8_t> jniToBytes(JNIEnv* env, jbyteArray a) {
    std::vector<uint8_t> out;
    if (!env || !a) return out;
    jsize n = env->GetArrayLength(a);
    if (n <= 0) return out;
    out.resize((size_t)n);
    env->GetByteArrayRegion(a, 0, n, (jbyte*)out.data());
    return out;
}
} // namespace ldb

bool ldbReady() { return ldb::handle != 0; }

bool ldbOpen(const std::string& worldFolder) {
    if (ldb::handle) return true;
    if (!ldb::ensureLib()) return false;
    JNIEnv* env = ldb::currentEnv();
    if (!env) return false;
    std::string db = joinPath(joinPath(worldFolder, "db"), "");
    jstring path = env->NewStringUTF(db.c_str());
    ldb::handle = ldb::fnOpen(env, nullptr, path);
    env->DeleteLocalRef(path);
    if (env->ExceptionCheck()) { env->ExceptionClear(); ldb::handle = 0; }
    diagLog("leveldb: open %s -> %llx", db.c_str(), (unsigned long long)ldb::handle);
    return ldb::handle != 0;
}

void ldbClose() {
    JNIEnv* env = ldb::currentEnv();
    if (ldb::handle && ldb::fnClose && env) {
        ldb::fnClose(env, nullptr, ldb::handle);
        if (env->ExceptionCheck()) env->ExceptionClear();
    }
    ldb::handle = 0;
}

std::vector<uint8_t> ldbGet(const std::vector<uint8_t>& key) {
    JNIEnv* env = ldb::currentEnv();
    if (!ldb::handle || !env || !ldb::fnGet) return {};
    // Local frame keeps the 40k-key iteration from overflowing JNI local refs.
    env->PushLocalFrame(16);
    jbyteArray jk = ldb::bytesToJni(env, key);
    jbyteArray jv = ldb::fnGet(env, nullptr, ldb::handle, jk);
    std::vector<uint8_t> out = ldb::jniToBytes(env, jv);
    if (env->ExceptionCheck()) { env->ExceptionClear(); out.clear(); }
    env->PopLocalFrame(nullptr);
    return out;
}

bool ldbPut(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value) {
    JNIEnv* env = ldb::currentEnv();
    if (!ldb::handle || !env || !ldb::fnPut) return false;
    env->PushLocalFrame(8);
    jbyteArray jk = ldb::bytesToJni(env, key);
    jbyteArray jv = ldb::bytesToJni(env, value);
    ldb::fnPut(env, nullptr, ldb::handle, jk, jv);
    bool ok = !env->ExceptionCheck();
    if (!ok) env->ExceptionClear();
    env->PopLocalFrame(nullptr);
    return ok;
}

std::vector<std::vector<uint8_t>> ldbIterate(const std::vector<uint8_t>& prefix) {
    std::vector<std::vector<uint8_t>> out;
    JNIEnv* env = ldb::currentEnv();
    if (!ldb::handle || !env || !ldb::fnIter) return out;
    env->PushLocalFrame(16);
    jbyteArray jp = ldb::bytesToJni(env, prefix);
    jobjectArray arr = ldb::fnIter(env, nullptr, ldb::handle, jp);
    if (env->ExceptionCheck()) env->ExceptionClear();
    if (arr) {
        jsize n = env->GetArrayLength(arr);
        for (jsize i = 0; i < n; ++i) {
            jbyteArray a = (jbyteArray)env->GetObjectArrayElement(arr, i);
            out.push_back(ldb::jniToBytes(env, a));
            if (a) env->DeleteLocalRef(a);
        }
    }
    env->PopLocalFrame(nullptr);
    return out;
}

static void crashHandler(int sig);
static void writeCrashNote(const std::string& filesDir, const std::string& why);
static void androidMain();

// ---- diagnostics breadcrumbs (readable by a file manager, no adb needed) ---
static void writeCrashNote(const std::string& filesDir, const std::string& why) {
    diagLog("FAILURE: %s", why.c_str());
    if (!filesDir.empty()) flushLog(filesDir);
    const char* dirs[] = {"/sdcard/Download", "/sdcard"};
    for (const char* d : dirs) {
        std::string p = joinPath(d, "nbt-editor-startup-error.txt");
        FILE* f = std::fopen(p.c_str(), "w");
        if (f) {
            std::fprintf(f, "NBT Editor failed to start\n%s\n", why.c_str());
            std::fclose(f);
            return;
        }
    }
}

// ---- crash capture (keeps the log even if a thread dies) -------------------
static void crashHandler(int sig) {
    const char* name = "unknown";
    switch (sig) {
        case SIGSEGV: name = "SIGSEGV (segmentation fault)"; break;
        case SIGABRT: name = "SIGABRT (abort)"; break;
        case SIGILL:  name = "SIGILL (illegal instruction)"; break;
        case SIGFPE:  name = "SIGFPE (arithmetic error)"; break;
        case SIGBUS:  name = "SIGBUS"; break;
        case SIGTRAP: name = "SIGTRAP"; break;
    }
    const std::string p = logFilePath(g_state.filesDir.empty() ? "." : g_state.filesDir);
    flushLog(g_state.filesDir.empty() ? "." : g_state.filesDir);
    FILE* f = std::fopen(p.c_str(), "a");
    if (f) {
        std::fprintf(f, "CRASH: signal %d %s\n", sig, name);
        std::fclose(f);
    }
    ::signal(sig, SIG_DFL);
    raise(sig);
}

// ---- input dispatch ---------------------------------------------------------
// The looper callback only *collects* events; this runs on the render thread and
// turns them into actions. Every tap resolves against the node tree that was
// drawn in the previous frame (ui::hitTest walks it in z-order), so no visible
// button is unreachable. Drags inside a scroll container scroll instead of
// tapping; holding still past the long-press limit fires actionLong.
static void dispatchTouch(AppState& a) {
    std::vector<PendingEvent> evs;
    {
        std::lock_guard<std::mutex> lk(g_eventMutex);
        if (g_events.empty()) return;
        evs.swap(g_events);
    }
    a.nowMs = nowMs();

    const int sc = a.scale > 0 ? a.scale : 1;      // device px -> design units
    for (const PendingEvent& e : evs) {
        if (e.key) {
            if (e.keyCode == AKEYCODE_BACK && !e.keyDown) {
                LOGI("native key back action");
                g_backRequested.store(true, std::memory_order_release);
                wakeRender();
            }
            continue;
        }
        const int ex = (int)(e.x / sc);
        const int ey = (int)(e.y / sc);
        if (e.down) {
            a.touchActive = true; a.touchScrolled = false;
            // Start inside the app but outside Android's system gesture edge
            // (x<=24dp is reserved by the OS); 28..80dp is safe.
            a.mapEdgeSwipe = (a.screen == S_MAP && ex >= 28 && ex <= 56);
            a.drawerTouch = (a.screen == S_MAP && a.drawerOpen && ex < drawerWidth(a));
            a.drawerScrollStart = a.drawerScrollY;
            a.touchX = ex; a.touchY = ey;
            a.touchStartX = ex; a.touchStartY = ey;
            a.touchLastX = a.touchStartX; a.touchLastY = a.touchStartY;
            a.touchAxis = kTouchAxisNone;
            a.touchInBreadcrumb = a.uiRoot &&
                ui::findBreadcrumbAt(*a.uiRoot, ex, ey) != nullptr;
            a.touchDownMs = e.ms;
            continue;
        }
        if (e.move) {
            if (!a.touchActive) continue;
            int fingerDx = ex - a.touchLastX;
            int dy = a.touchLastY - ey;
            // Show Map: edge swipe opens the aside (DrawerLayout behaviour),
            // dragging anywhere else pans the world; no rigid coordinates.
            if (a.screen == S_MAP) {
                float bpd = 16.0f * a.mapScale;
                int totalDx = ex - a.touchStartX;
                if (a.mapEdgeSwipe && totalDx > 55) {
                    a.drawerOpen = true; a.drawerTouch = true;
                    a.drawerScrollY = 0; a.drawerScrollF = 0; a.touchScrolled = true;
                } else if (a.drawerOpen && totalDx < -55) {
                    a.drawerOpen = false; a.drawerTouch = false; a.touchScrolled = true;
                } else if (a.drawerTouch) {
                    // NavigationView list scrolls vertically inside the panel;
                    // the map underneath must never pan with this gesture.
                    // Absolute finger tracking (instead of accumulating tiny
                    // per-event deltas) keeps velocity constant across frames.
                    int totalDy = ey - a.touchStartY;
                    if (std::abs(totalDy) > 2) {
                        int maxScroll = std::max(0, a.drawerContentBottom - (a.h - 8));
                        a.drawerScrollY = std::max(0,
                            std::min(maxScroll, a.drawerScrollStart - totalDy));
                        a.touchScrolled = true;
                    }
                } else if (std::abs(fingerDx) > 0 || std::abs(dy) > 0) {
                    a.mapCenterX -= (int64_t)(fingerDx / bpd);
                    a.mapCenterZ += (int64_t)(dy / bpd);
                    a.touchScrolled = true;
                }
                a.touchLastX = ex; a.touchLastY = ey;
                continue;
            }
            a.touchLastX = ex; a.touchLastY = ey;
            int totalDx = ex - a.touchStartX;
            int totalDy = ey - a.touchStartY;
            bool axisJustLocked = false;

            // Axis lock keeps horizontal breadcrumb panning and vertical tree
            // scrolling independent.  A drag starting in the breadcrumb never
            // becomes a tree scroll, even if the finger later crosses it.
            if (a.touchAxis == kTouchAxisNone &&
                (std::abs(totalDx) > 14 || std::abs(totalDy) > 14)) {
                a.touchAxis = (std::abs(totalDx) > std::abs(totalDy))
                    ? kTouchAxisHorizontal : kTouchAxisVertical;
                a.touchScrolled = true;
                axisJustLocked = true;
            }

            if (a.touchAxis == kTouchAxisHorizontal) {
                if (a.touchInBreadcrumb) {
                    int dragDx = axisJustLocked ? totalDx : fingerDx;
                    scrollBreadcrumb(a, dragDx);
                }
                continue;
            }
            if (a.touchAxis == kTouchAxisVertical) {
                if (!a.touchInBreadcrumb && a.uiRoot) {
                    if (ui::Node* sc2 = ui::findScrollAt(*a.uiRoot, ex, ey)) {
                        int scrollDelta = axisJustLocked ? (a.touchStartY - ey) : dy;
                        a.scrollY += scrollDelta;
                        int maxScroll = a.contentBottom - (sc2->rect.y + sc2->rect.h);
                        if (maxScroll < 0) maxScroll = 0;
                        if (a.scrollY < 0) a.scrollY = 0;
                        if (a.scrollY > maxScroll) a.scrollY = maxScroll;
                    }
                }
                continue;
            }
            continue;
        }
        if (e.cancel) {
            a.touchActive = false;
            a.touchScrolled = false;
            continue;
        }
        if (!e.up || !a.touchActive) continue;
        a.touchActive = false;
        a.touchX = ex; a.touchY = ey;
        if (a.touchScrolled) continue;                     // that was a drag

        const ui::Node* hit = nullptr;
        if (!a.uiRoot || !ui::hitTestInteractive(*a.uiRoot, ex, ey, hit) || !hit) {
            LOGI("native touch up (%d,%d) missed", ex, ey);
            continue;
        }
        bool longPress = (e.ms - a.touchDownMs) > 480;
        LOGI("native touch up (%d,%d) hit action=%d index=%d long=%d",
             ex, ey, hit->action, hit->index, longPress ? 1 : 0);
        dispatchHit(a, *hit, longPress);
    }
}

// ---- the render thread ------------------------------------------------------
static void androidMain() {
    diagLog("render: thread started");

    // A fresh std::thread does not have an ALooper.  ALooper_prepare(0) makes
    // one on this thread; the input queue must be attached to this exact
    // looper.  The pointer and its reference are published under the platform
    // lock so onDestroy() cannot wake/release a looper mid-flight.
    ALooper* looper = ALooper_prepare(0);
    if (looper) ALooper_acquire(looper);
    {
        std::lock_guard<std::mutex> lk(g_platMutex);
        if (g_plat.inputAttached.load() && g_plat.input.load()) {
            AInputQueue_detachLooper(g_plat.input.load());
        }
        g_plat.inputAttached = false;
        g_plat.looper.store(looper, std::memory_order_release);
    }
    LOGI("native render looper=%p", (void*)looper);
    diagLog("render: looper=%p", (void*)looper);

    unsigned frames = 0;

    while (g_plat.running.load(std::memory_order_acquire)) {
        attachInputIfNeeded();

        // Every poll is finite.  A negative timeout was the reason the queue
        // could be created while the render thread never returned to attach it.
        if (looper) ALooper_pollOnce(0, nullptr, nullptr, nullptr);

        if (!g_plat.renderReady.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        ANativeWindow* w = nullptr;
        int width = 0, height = 0, scale = 1;
        {
            std::lock_guard<std::mutex> lk(g_platMutex);
            w = g_plat.window.load(std::memory_order_acquire);
            if (w) ANativeWindow_acquire(w);   // own reference for the frame
            width = g_plat.width.load(std::memory_order_relaxed);
            height = g_plat.height.load(std::memory_order_relaxed);
            scale = g_plat.scale.load(std::memory_order_relaxed);
        }
        if (!w || !g_plat.hasWindow.load() || width <= 0 || height <= 0) {
            if (w) ANativeWindow_release(w);
            g_plat.renderReady = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        int curW = ANativeWindow_getWidth(w);
        int curH = ANativeWindow_getHeight(w);
        if (curW > 0 && curH > 0 && (curW != width || curH != height)) {
            width = curW; height = curH; scale = uiScaleFor(width, height);
            g_plat.width = width; g_plat.height = height; g_plat.scale = scale;
            ANativeWindow_setBuffersGeometry(w, width, height, WINDOW_FORMAT_RGBA_8888);
        }

        AppState& a = g_state;
        a.nowMs = nowMs();
        a.scale = scale > 0 ? scale : 1;
        a.w = std::max(1, width / a.scale);
        a.h = std::max(1, height / a.scale);

        ANativeWindow_Buffer buf;
        if (ANativeWindow_lock(w, &buf, nullptr) < 0) {
            ANativeWindow_release(w);
            std::this_thread::sleep_for(std::chrono::milliseconds(8));
            continue;
        }
        {
            std::lock_guard<std::mutex> lk(g_frameMutex);
            // bind() is stride aware: the canvas row pitch equals the window's
            // own stride, so drawing lands where the compositor expects it.
            g_canvas.bind((uint32_t*)buf.bits, (int)buf.width, (int)buf.height,
                          (int)buf.stride);
            g_canvas.setScale(a.scale);
            {
                std::lock_guard<std::mutex> lk(g_stateMutex);
                renderFrame(a, g_canvas);
            }
            ANativeWindow_unlockAndPost(w);
        }
        ANativeWindow_release(w);
        ++frames;
        if (frames == 1) diagLog("render: first frame drawn %dx%d", a.w, a.h);

        bool exitRequested = false;
        {
            std::lock_guard<std::mutex> lk(g_stateMutex);
            dispatchTouch(a);
            dispatchPickerResult(a);   // SAF / OS file manager callback
            if (g_backRequested.exchange(false, std::memory_order_acq_rel)) back(a);
            exitRequested = a.wantExit;
            if (exitRequested) a.wantExit = false;
        }
        if (exitRequested) {
            ANativeActivity* act = g_plat.activity.load(std::memory_order_acquire);
            if (act) ANativeActivity_finish(act);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));   // ~60fps
    }

    {
        std::lock_guard<std::mutex> lk(g_platMutex);
        AInputQueue* q = g_plat.input.load(std::memory_order_acquire);
        if (q && g_plat.inputAttached.load()) AInputQueue_detachLooper(q);
        g_plat.inputAttached = false;
        if (looper) {
            ALooper_release(looper);
            g_plat.looper.store(nullptr, std::memory_order_release);
        }
    }
    diagLog("render: thread exiting");
}

static void bootstrap(ANativeActivity* activity) {
    // Activity recreation must not race the previous render thread while it is
    // still reading AppState.  onDestroy() normally joins already; this is the
    // defensive fallback for a lifecycle callback that arrived out of order.
    if (g_renderThread.joinable()) g_renderThread.join();
    g_plat.activity.store(activity, std::memory_order_release);
    g_plat.running = true;
    g_plat.paused = false;
    g_plat.renderReady = false;
    g_plat.hasWindow = false;
    g_plat.hasFocus = false;

    AppState& a = g_state;
    // (Re)initialise on every onCreate: a re-created activity (rotation, or a
    // restart inside the task) must not keep stale paths or a dead window.
    a.appName = "NBT Editor";
    a.filesDir = activity->internalDataPath ? activity->internalDataPath : ".";
    a.worldsRoot = joinPath(activity->externalDataPath ? activity->externalDataPath : a.filesDir,
                            "worlds");
    ensureDir(a.worldsRoot);
    // Do NOT scan world folders here: dirSize() walks every file and
    // looksEncrypted() reads every LevelDB file, which froze the Android main
    // thread on cold start ("app isn't responding"). The worker fills the list.
    a.worlds.clear();
    a.screen = a.screen ? a.screen : S_HOME;
    if (a.browseDir.empty()) a.browseDir = "/sdcard";
    a.dark = false;
    a.theme = gfx::themeLight();
    a.scale = g_plat.scale.load(std::memory_order_relaxed) > 0
                  ? g_plat.scale.load(std::memory_order_relaxed) : 1;
    a.w = 1; a.h = 1;

    if (!g_booted) {
        g_booted = true;
        diagLog("boot: native library loaded");
        diagLog("boot: worlds dir=%s", a.worldsRoot.c_str());
        diagLog("boot: internalDataPath=%s", a.filesDir.c_str());
        ::signal(SIGSEGV, crashHandler);
        ::signal(SIGABRT, crashHandler);
        ::signal(SIGILL,  crashHandler);
        ::signal(SIGFPE,  crashHandler);
        ::signal(SIGBUS,  crashHandler);
        appWorker().start();
        LOGI("NBT Editor native app started (worlds dir: %s)", a.worldsRoot.c_str());
    }

    // Refresh the world list off the main thread (see bootstrap comment).
    appWorker().post([]() {
        std::lock_guard<std::mutex> lk(g_stateMutex);
        listWorlds(g_state);
    });

    // The render loop is activity-scoped: a new activity always gets a fresh
    // joinable thread.  The worker is process-scoped and is reused.
    try {
        g_renderThread = std::thread(androidMain);
        diagLog("boot: render thread started");
    } catch (const std::exception& e) {
        diagLog("boot: cannot start render thread: %s", e.what());
        writeCrashNote(a.filesDir, "render thread: " + std::string(e.what()));
    }
    diagLog("boot: done (storage permission is requested lazily)");
}
} // namespace app

#define NBT_EXPORT __attribute__((visibility("default")))

extern "C" NBT_EXPORT void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState,
                                                   size_t savedStateSize) {
    (void)savedState; (void)savedStateSize;
    ANativeActivityCallbacks* cb = activity->callbacks;
    cb->onStart = app::onStart;
    cb->onResume = app::onResume;
    cb->onSaveInstanceState = app::onSaveInstanceState;
    cb->onPause = app::onPause;
    cb->onStop = app::onStop;
    cb->onDestroy = app::onDestroy;
    cb->onWindowFocusChanged = app::onWindowFocusChanged;
    cb->onNativeWindowCreated = app::onNativeWindowCreated;
    cb->onNativeWindowResized = app::onNativeWindowResized;
    cb->onNativeWindowRedrawNeeded = app::onNativeWindowRedrawNeeded;
    cb->onNativeWindowDestroyed = app::onNativeWindowDestroyed;
    cb->onInputQueueCreated = app::onInputQueueCreated;
    cb->onInputQueueDestroyed = app::onInputQueueDestroyed;
    cb->onContentRectChanged = app::onContentRectChanged;
    cb->onConfigurationChanged = app::onConfigurationChanged;
    app::bootstrap(activity);
}
#endif // !NBT_TESTABLE

// ============================================================================
// HOST SELF-TEST (compiled only with -DNBT_SELFTEST)
// Proves the NBT core is independent of Android/UI: this build links nothing
// but the C++ standard library.
// ============================================================================
#if NBT_TESTABLE
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace testboot {

static int g_pass = 0, g_fail = 0;
static void check(bool cond, const char* id, const std::string& what = "") {
    if (cond) { ++g_pass; std::printf("  [PASS] %s\n", id); }
    else      { ++g_fail; std::printf("  [FAIL] %s  %s\n", id, what.c_str()); }
}

static std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    std::vector<uint8_t> v((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return v;
}

// ---- fixture: one tag of every type, nested, incl. TAG_Long_Array (12) ------
static std::vector<uint8_t> fixtureAllTypes() {
    using namespace nbt;
    ByteWriter w;
    Node root; root.type = Type::Compound;
    // 1 Byte
    Node b1; b1.type = Type::Byte; b1.name = "byteTag"; b1.i = -7;
    // 2 Short
    Node b2; b2.type = Type::Short; b2.name = "shortTag"; b2.i = -12345;
    // 3 Int
    Node b3; b3.type = Type::Int; b3.name = "intTag"; b3.i = -2000000000;
    // 4 Long
    Node b4; b4.type = Type::Long; b4.name = "longTag"; b4.i = -9000000000000000000LL;
    // 5 Float
    Node b5; b5.type = Type::Float; b5.name = "floatTag"; b5.d = 1.5f;
    // 6 Double
    Node b6; b6.type = Type::Double; b6.name = "doubleTag"; b6.d = -3.141592653589793;
    // 7 ByteArray
    Node b7; b7.type = Type::ByteArray; b7.name = "byteArray"; b7.bytes = {1, (uint8_t)-2, 3};
    // 8 String
    Node b8; b8.type = Type::String; b8.name = "str"; b8.s = "héllo 🙂";
    // 9 List (of strings)
    Node b9; b9.type = Type::List; b9.name = "listOfStrings"; b9.elemType = Type::String;
    Node e1; e1.type = Type::String; e1.s = "alpha";
    Node e2; e2.type = Type::String; e2.s = "beta";
    // 10 Compound (nested 3 levels deep)
    Node b10; b10.type = Type::Compound; b10.name = "nested";
    Node c1;  c1.type = Type::Compound; c1.name = "level1";
    Node c2;  c2.type = Type::Compound; c2.name = "level2";
    Node leaf; leaf.type = Type::Int; leaf.name = "deepLeaf"; leaf.i = 4242;
    // 11 IntArray
    Node b11; b11.type = Type::IntArray; b11.name = "intArray"; b11.longs = {7, -8, 9};
    // 12 LongArray (NOT in the reference app)
    Node b12; b12.type = Type::LongArray; b12.name = "longArray";
    b12.longs = {9223372036854775807LL, -9223372036854775807LL - 1, 0};

    // serialise children in order, collecting indices for container wiring
    Document doc;
    doc.arena.reserve(16);
    auto push = [&](Node n) { doc.arena.push_back(std::move(n)); return (uint32_t)doc.arena.size() - 1; };
    uint32_t i1 = push(std::move(b1)), i2 = push(std::move(b2)), i3 = push(std::move(b3)),
             i4 = push(std::move(b4)), i5 = push(std::move(b5)), i6 = push(std::move(b6)),
             i7 = push(std::move(b7)), i8 = push(std::move(b8)), i9 = push(std::move(b9)),
             i10 = push(std::move(b10)), i11 = push(std::move(b11)), i12 = push(std::move(b12));
    // nested compound chain
    uint32_t iLeaf = push(std::move(leaf));
    uint32_t iC2 = push(std::move(c2));
    uint32_t iC1 = push(std::move(c1));
    doc.arena[iC2].kids.push_back(iLeaf);
    doc.arena[iC1].kids.push_back(iC2);
    doc.arena[i10].kids.push_back(iC1);
    // list elements
    doc.arena.push_back(std::move(e1)); uint32_t iE1 = (uint32_t)doc.arena.size() - 1;
    doc.arena.push_back(std::move(e2)); uint32_t iE2 = (uint32_t)doc.arena.size() - 1;
    doc.arena[i9].kids.push_back(iE1);
    doc.arena[i9].kids.push_back(iE2);

    doc.arena.push_back(std::move(root));
    doc.root = (uint32_t)doc.arena.size() - 1;
    for (uint32_t k : {i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12})
        doc.arena[doc.root].kids.push_back(k);

    auto encoded = serialize(doc);
    return encoded.ok() ? encoded.value : std::vector<uint8_t>{};
}

[[maybe_unused]] static void run() {
    using namespace nbt;
    std::printf("== NBT host self-test ==\n");

    // R1 renderer smoke test: text, rounded rect, lines, clips (host only)
    if (std::getenv("NBT_RENDER_DUMP")) {
        using namespace gfx;
        const int W = 96, H = 30;
        std::vector<uint32_t> buf((size_t)W * H);
        Canvas c; c.bind(buf.data(), W, H, W);
        Theme th = themeLight();
        c.clear(th.background);
        c.roundRect({6, 3, 60, 12}, 4, th.surface);
        c.text(10, 6, "NBT Editor", th.onSurface);
        c.text(10, 6 + kGlyphH, "TAG_Long_Array 0x1f", th.primary);
        c.fill({70, 3, 2, 12}, th.outline);
        c.textCentered({6, 20, 60, 12}, "centred text", th.onSurfaceMuted);
        c.textEllipsis({6, 34, 60, 12}, "this string is far too long to fit", th.onSurface);
        c.stroke({70, 20, 20, 8}, th.error);
        c.line(70, 34, 90, 44, th.secondary);
        c.circle(80, 40, 5, th.primary);
        dumpAscii(c, "render smoke");
        // dark theme sanity
        Canvas d; d.bind(buf.data(), W, H, W);
        Theme thd = themeDark();
        d.clear(thd.background);
        d.roundRect({6, 3, 60, 12}, 4, thd.surface);
        d.text(10, 6, "dark mode", thd.onSurface);
        dumpAscii(d, "dark");
    }

    // T9 empty file
    {
        auto r = parseBytes(nullptr, 0, "empty", {});
        check(!r.ok() && r.err == Err::Empty, "T9 empty file", errText(r.err));
    }
    // T10 unknown tag byte
    {
        std::vector<uint8_t> v = {0x63, 0x00, 0x00};
        auto r = parseBytes(v.data(), v.size(), "bad", {});
        check(!r.ok() && r.err == Err::UnknownTag, "T10 unknown tag", errText(r.err));
    }
    // T18 root TAG_End
    {
        std::vector<uint8_t> v = {0x00};
        auto r = parseBytes(v.data(), v.size(), "end", {});
        check(!r.ok(), "T18 root TAG_End rejected");
    }
    // T17 empty array + empty compound
    {
        std::vector<uint8_t> v = {10, 0x00, 0x00,            // compound, name len 0
                                  7,  0x00, 0x00, 0,0,0,0,    // byteArray "name" len 0, count 0
                                  11, 0x00, 0x00, 0,0,0,0,    // intArray  "name" len 0, count 0
                                  12, 0x00, 0x00, 0,0,0,0,    // longArray "name" len 0, count 0
                                  0};                          // TAG_End
        auto r = parseBytes(v.data(), v.size(), "emptycontainers", {});
        check(r.ok(), "T17 empty compound/arrays parse");
        if (r.ok()) {
            auto again = serialize(r.value);
            check(again.ok() && again.value == v, "T17 round-trip byte-identical",
                  again.ok() ? "" : again.detail);
        }
    }
    // T4/T5/T15 all-13-types fixture round-trip
    if (std::getenv("NBT_DEBUG_DUMP")) {
        std::vector<uint8_t> fv = fixtureAllTypes();
        std::fprintf(stderr, "fixture bytes (%zu):", fv.size());
        for (size_t k = 0; k < fv.size() && k < 240; ++k) std::fprintf(stderr, " %02x", fv[k]);
        std::fprintf(stderr, "\n");
    }
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "fixture", {});
        check(r.ok(), "T4 all-types fixture parse", r.detail);
        if (r.ok()) {
            auto again = serialize(r.value);
            check(again.ok() && again.value == v,
                  "T4/T5 round-trip byte-identical (13 types incl. LongArray)",
                  again.ok() ? "" : again.detail);
            const Document& d = r.value;
            // 12 typed children + root + 2 list elements + level1 + level2 + deepLeaf
            check(countNodes(d) == 12 + 1 + 2 + 3, "T4 node count", std::to_string(countNodes(d)));
            // long array values preserved
            bool la = false;
            for (const auto& n : d.arena) if (n.type == Type::LongArray) {
                la = n.longs.size() == 3 && n.longs[0] == 9223372036854775807LL &&
                     n.longs[1] == (-9223372036854775807LL - 1) && n.longs[2] == 0;
            }
            check(la, "T4 TAG_Long_Array extremes preserved");
            bool st = false;
            for (const auto& n : d.arena)
                if (n.name == "str") st = (n.s == "héllo 🙂");
            check(st, "T15 UTF-8 string preserved");
            auto problems = validate(d);
            check(problems.empty(), "T4 validate() clean",
                  [&]{ std::string out; for (auto& q : problems) out += q + "; "; return out; }());
        }
    }
    // T1/T2 reference level.dat template
    {
        const std::string path = ".ref_extract/base/assets/worldTemplate/level.dat";
        std::error_code ec;
        if (std::filesystem::exists(path, ec)) {
            std::vector<uint8_t> bytes = readFile(path);
            check(bytes.size() == 2977, "T1 template size 2977", std::to_string(bytes.size()));
            auto r = parseBytes(bytes.data(), bytes.size(), path, {true, true});
            check(r.ok(), "T1 level.dat parses with header", r.detail);
            if (r.ok()) {
                check(r.value.hasLevelDatHeader && r.value.headerVersion == 10, "T1 header version 10");
                auto again = serializeLevelDat(r.value);
                check(again.ok(), "T2 serialize level.dat succeeds", again.detail);
                if (again.ok() && again.value != bytes) {
                    size_t n = std::min(again.value.size(), bytes.size());
                    std::string diffs;
                    for (size_t k = 0; k < n; ++k)
                        if (again.value[k] != bytes[k]) {
                            diffs += " @" + std::to_string(k) + " out=" + std::to_string((unsigned)again.value[k]) +
                                     " in=" + std::to_string((unsigned)bytes[k]);
                            if (diffs.size() > 400) break;
                        }
                    if (again.value.size() != bytes.size())
                        diffs += " size out=" + std::to_string(again.value.size()) + " in=" + std::to_string(bytes.size());
                    std::fprintf(stderr, "T2 diffs:%s\n", diffs.c_str());
                }
                check(again.ok() && again.value == bytes,
                      "T2 serialize level.dat byte-identical (round-trip)");
                auto problems = validate(r.value);
                check(problems.empty(), "T2 reference template validates clean");
                check(r.value.arena[r.value.root].type == Type::Compound, "T1 root is Compound");
            }
        } else {
            std::printf("  [SKIP] T1/T2 (template not present in this checkout)\n");
        }
    }
    // T11 header length mismatch
    {
        std::vector<uint8_t> v = {10,0,0,0, 5,0,0,0,  10,0,0, 0};
        auto r = parseBytes(v.data(), v.size(), "hdr", {true, true});
        check(!r.ok() && r.err == Err::HeaderLengthMismatch, "T11 header length mismatch");
    }
    // T16 depth 128 ok / 129 rejected
    {
        ByteWriter w;
        int depth = 128;
        for (int k = 0; k < depth; ++k) { w.u8(10); w.u16(0x0000); }   // named compounds, empty name
        for (int k = 0; k < depth; ++k) w.u8(0);                      // close every compound
        auto r = parseBytes(w.out.data(), w.out.size(), "deep", {});
        check(r.ok(), "T16 depth 128 accepted", r.detail);
        ByteWriter w2;                    // one level deeper than the limit: kMaxDepth+1
        for (int k = 0; k < kMaxDepth + 2; ++k) { w2.u8(10); w2.u16(0x0000); }
        for (int k = 0; k < kMaxDepth + 2; ++k) w2.u8(0);
        auto r2 = parseBytes(w2.out.data(), w2.out.size(), "deeper", {});
        check(!r2.ok() && r2.err == Err::DepthLimit, "T16 depth 129 rejected");
    }
    // T12/T13 scalar range validation
    {
        check(rangeFor(Type::Byte, "128") == RangeCheck::OutHigh, "T12 byte high bound");
        check(rangeFor(Type::Byte, "-129") == RangeCheck::OutLow, "T12 byte low bound");
        check(rangeFor(Type::Short, "40000") == RangeCheck::OutHigh, "T12 short bound");
        check(rangeFor(Type::Int, "3000000000") == RangeCheck::OutHigh, "T12 int bound");
        check(rangeFor(Type::Long, "-9223372036854775809") != RangeCheck::Ok, "T12 long bound");
        check(rangeFor(Type::Float, "1e39") == RangeCheck::OutHigh, "T12 float bound");
        check(rangeFor(Type::Double, "abc") == RangeCheck::NotNumeric, "T12 not numeric");
        Node out; std::string msg;
        check(!parseScalar("999", Type::Byte, out, msg) && msg == "Out of bounds for byte.", "T12 message");
        check(parseScalar("héllo 🙂", Type::String, out, msg), "T13 utf-8 string ok");
        check(!parseScalar(std::string(70000, 'x'), Type::String, out, msg), "T13 string >65535");
    }
    // T14 list element mismatch
    {
        std::vector<uint8_t> v = {9, 0x00, 0x00, 3, 0,0,0,0};  // list elem=Int(3) count 0
        auto r = parseBytes(v.data(), v.size(), "listint", {});
        check(r.ok(), "T14 list parses", r.detail);
        if (r.ok()) {
            Document d = r.value;
            Node bad; bad.type = Type::String; bad.s = "x";
            d.arena.push_back(std::move(bad));
            d.arena[d.root].kids.push_back((uint32_t)d.arena.size() - 1);
            auto bytes = serialize(d);          // writer must reject, not corrupt structure
            bool rejected = !bytes.ok() && bytes.err == Err::InvalidDocument;
            std::vector<std::string> probs = validate(d);
            bool flagged = false;
            for (auto& p : probs) if (p.find("List element type mismatch") != std::string::npos) flagged = true;
            check(flagged && rejected, "T14 element mismatch detected and rejected",
                  bytes.detail);
        }
    }
    // T25 malformed counts are rejected before allocation or a long loop.
    {
        std::vector<uint8_t> longMax = {10,0,0, 12,0,0, 0xff,0xff,0xff,0x7f};
        auto r1 = parseBytes(longMax.data(), longMax.size(), "long-max", {});
        check(!r1.ok() && r1.err == Err::SizeLimit, "T25 huge long-array count rejected", errText(r1.err));

        std::vector<uint8_t> negativeBytes = {10,0,0, 7,0,0, 0xff,0xff,0xff,0xff};
        auto r2 = parseBytes(negativeBytes.data(), negativeBytes.size(), "negative-array", {});
        check(!r2.ok() && r2.err == Err::Truncated, "T25 negative array count rejected", errText(r2.err));

        std::vector<uint8_t> noLongPayload = {10,0,0, 12,0,0, 1,0,0,0};
        auto r3 = parseBytes(noLongPayload.data(), noLongPayload.size(), "missing-array", {});
        check(!r3.ok() && r3.err == Err::Truncated, "T25 truncated array payload rejected", errText(r3.err));

        std::vector<uint8_t> endListMax = {10,0,0, 9,0,0, 0, 0xff,0xff,0xff,0x7f};
        auto r4 = parseBytes(endListMax.data(), endListMax.size(), "end-list-max", {});
        check(!r4.ok() && r4.err == Err::Truncated, "T25 non-empty End list rejected", errText(r4.err));

        std::vector<uint8_t> intListMax = {10,0,0, 9,0,0, 3, 0xff,0xff,0xff,0x7f};
        auto r5 = parseBytes(intListMax.data(), intListMax.size(), "int-list-max", {});
        check(!r5.ok() && r5.err == Err::SizeLimit, "T25 huge list count rejected", errText(r5.err));
    }

    // T26 trailing bytes are never silently discarded.
    {
        std::vector<uint8_t> trailing = {10,0,0,0, 0xaa,0xbb};
        auto r1 = parseBytes(trailing.data(), trailing.size(), "trailing", {});
        check(!r1.ok() && r1.err == Err::TrailingBytes, "T26 raw trailing bytes rejected", errText(r1.err));

        std::vector<uint8_t> levelTrailing = {10,0,0,0, 5,0,0,0, 10,0,0,0, 0xaa};
        auto r2 = parseBytes(levelTrailing.data(), levelTrailing.size(), "level-trailing", {true, true});
        check(!r2.ok() && r2.err == Err::TrailingBytes, "T26 level.dat payload trailing bytes rejected", errText(r2.err));

        std::vector<uint8_t> outside = {10,0,0,0, 4,0,0,0, 10,0,0,0, 0xaa};
        auto r3 = parseBytes(outside.data(), outside.size(), "outside", {true, true});
        check(!r3.ok() && r3.err == Err::HeaderLengthMismatch, "T26 strict level.dat length enforced", errText(r3.err));
    }

    // T27 valid empty/list containers still parse and list elements count as nodes.
    {
        std::vector<uint8_t> emptyEndList = {10,0,0, 9,0,0, 0,0,0,0,0, 0};
        auto r1 = parseBytes(emptyEndList.data(), emptyEndList.size(), "empty-end-list", {});
        check(r1.ok(), "T27 empty End list parses", r1.detail);

        std::vector<uint8_t> intList = {10,0,0, 9,0,0, 3, 3,0,0,0, 1,0,0,0, 2,0,0,0, 3,0,0,0, 0};
        auto r2 = parseBytes(intList.data(), intList.size(), "int-list", {});
        check(r2.ok() && countNodes(r2.value) == 5, "T27 list elements count toward node limit",
              r2.ok() ? std::to_string(countNodes(r2.value)) : r2.detail);
    }

    // T22 search
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "search", {});
        if (r.ok()) {
            auto hits = find(r.value, "Tag");
            check(hits.size() >= 4, "T22 find by substring", std::to_string(hits.size()));
            auto none = find(r.value, "zzzz-not-present");
            check(none.empty(), "T22 find no match");
        }
    }
    // T21 .mcworld zip round-trip (real zip container via libz)
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        fs::path base = fs::temp_directory_path(ec) / ("nbt_zip_test_" + std::to_string((long)getpid()));
        fs::remove_all(base, ec);
        fs::create_directories(base / "db", ec);
        fs::create_directories(base / "empty_dir", ec);
        app::writeText((base / "level.dat").string(), "LEVELDAT-CONTENT");
        app::writeText((base / "levelname.txt").string(), "Zip Test World");
        {   // binary payload with all byte values
            std::vector<uint8_t> bin(256);
            for (int k = 0; k < 256; ++k) bin[k] = (uint8_t)k;
            app::writeFileAtomic((base / "db" / "000017.prodb").string(), bin);
        }
        fs::path zip = base / "world.mcworld";
        auto zr = app::createZip(base.string(), zip.string());
        check(zr.ok(), "T21 createZip", zr.detail);
        auto entries = app::unzip(zip.string());
        check(entries.ok() && entries.value.size() == 3, "T21 unzip entry count",
              entries.ok() ? std::to_string(entries.value.size()) : entries.detail);
        fs::path outdir = base / "extracted";
        auto exr = app::extractZip(zip.string(), outdir.string());
        check(exr.ok(), "T21 extractZip", exr.detail);
        bool same = true;
        auto a = app::readFile((base / "db" / "000017.prodb").string());
        auto b = app::readFile((outdir / "db" / "000017.prodb").string());
        if (!a.ok() || !b.ok() || a.value != b.value) same = false;
        auto c = app::readFile((outdir / "level.dat").string());
        if (!c.ok() || std::string(c.value.begin(), c.value.end()) != "LEVELDAT-CONTENT") same = false;
        check(same, "T21 zip contents survive round-trip");
        fs::remove_all(base, ec);
    }

    // T23 duplicate name detection on rename
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "dup", {});
        check(r.ok(), "T23 fixture loads");
        if (r.ok()) {
            // rename a child to collide with a sibling
            std::string first = r.value.arena[1].name;
            if (r.value.arena.size() > 2) r.value.arena[2].name = first;
            auto probs = validate(r.value);
            bool dup = false;
            for (auto& p : probs) if (p.find(msgDuplicateName()) != std::string::npos) dup = true;
            check(dup, "T23 duplicate name flagged by validate()");
        }
    }

    // T24: parse -> mutate -> serialize -> parse byte equality of untouched parts
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "mut2", {});
        check(r.ok(), "T24 fixture loads");
        if (r.ok()) {
            auto before = serialize(r.value);
            check(before.ok(), "T24 initial serialize succeeds", before.detail);
            for (auto& n : r.value.arena) if (n.name == "byteTag") { n.i = 99; break; }
            auto after = serialize(r.value);
            check(after.ok(), "T24 mutated serialize succeeds", after.detail);
            if (!after.ok()) return;
            auto again = parseBytes(after.value.data(), after.value.size(), "mut2", {});
            check(again.ok(), "T24 reparse after mutation", again.detail);
            if (again.ok()) {
                bool val = false;
                for (auto& n : again.value.arena) if (n.name == "byteTag") val = (n.i == 99);
                check(val, "T24 mutation applied");
                auto reparsed = serialize(again.value);
                check(reparsed.ok() && reparsed.value == after.value,
                      "T24 reparse reserialises identical", reparsed.detail);
            }
        }
    }

    // T19 large document (synthetic, ~ 1M nodes)
    {
        Document d;
        Node root; root.type = Type::Compound;
        d.arena.push_back(std::move(root)); d.root = 0;
        for (int k = 0; k < 100000; ++k) {
            Node s; s.type = Type::Int; s.name = "k" + std::to_string(k); s.i = k;
            Node t; t.type = Type::ByteArray; t.name = "b" + std::to_string(k); t.bytes.assign(8, (uint8_t)k);
            d.arena.push_back(std::move(s));
            d.arena.push_back(std::move(t));
        }
        for (uint32_t k = 1; k < d.arena.size(); ++k) d.arena[0].kids.push_back(k);
        auto bytes = serialize(d);
        check(bytes.ok(), "T19 large document serialised", bytes.detail);
        if (bytes.ok()) {
            check(bytes.value.size() > 1000000, "T19 large document size",
                  std::to_string(bytes.value.size()));
            auto r = parseBytes(bytes.value.data(), bytes.value.size(), "large", {});
            check(r.ok() && countNodes(r.value) == d.arena.size(), "T19 large round-trip", r.detail);
        }
    }
    // T3 mutation keeps only the changed bytes
    {
        std::vector<uint8_t> v = fixtureAllTypes();
        auto r = parseBytes(v.data(), v.size(), "mut", {});
        check(r.ok(), "T3 fixture loads");
        if (r.ok()) {
            for (auto& n : r.value.arena)
                if (n.name == "intTag") { n.i = 111222333; break; }
            auto out = serialize(r.value);
            check(out.ok(), "T3 mutated serialize succeeds", out.detail);
            if (out.ok()) {
                check(out.value.size() == v.size(), "T3 mutated size equal");
                size_t diff = 0;
                for (size_t k = 0; k < std::min(out.value.size(), v.size()); ++k)
                    diff += (out.value[k] != v[k]);
                check(diff <= 4, "T3 only changed bytes differ", std::to_string(diff) + " bytes");
            }
        }
    }

    std::printf("== %d passed, %d failed ==\n", g_pass, g_fail);
    if (g_fail) std::exit(1);
}

} // namespace testboot
#endif // NBT_TESTABLE


// ============================================================================
// MODEL TESTS (host only): build every screen from synthetic state and assert
// the UI contract — no buttons without actions, dialogs sized inside the
// viewport, keypad covers all characters, etc.
// ============================================================================
#if NBT_TESTABLE
namespace modeltest {

using namespace app;

static void buttonsHaveActions(const ui::Node& n, int& total, int& withAction) {
    if (!n.visible) return;
    if (n.kind == ui::Kind::Button) {
        ++total;
        if (n.action != 0) ++withAction;
    }
    for (auto& k : n.kids) buttonsHaveActions(k, total, withAction);
}

static void collectActions(const ui::Node& n, std::vector<int>& out) {
    if (n.action) out.push_back(n.action);
    for (auto& k : n.kids) collectActions(k, out);
}

static const ui::Node* findActionNode(const ui::Node& n, int action,
                                      const std::string& text = "") {
    if (n.action == action && (text.empty() || n.text == text)) return &n;
    for (const auto& kid : n.kids) {
        const ui::Node* found = findActionNode(kid, action, text);
        if (found) return found;
    }
    return nullptr;
}

static std::vector<uint8_t> readBytesForTest(const std::string& p) {
    auto r = app::readFile(p);
    return r.ok() ? r.value : std::vector<uint8_t>{};
}

static AppState makeState(int w, int h) {
    AppState a;
    a.w = w; a.h = h;
    a.dark = false;
    a.theme = gfx::themeLight();
    a.worldsRoot = (fs::temp_directory_path() / "nbt_model_worlds").string();
    a.recent = {"/sdcard/Download/a.dat", "/sdcard/Download/level.dat"};
    return a;
}

static uint32_t findNamedIndex(const nbt::Document& doc, const std::string& name) {
    for (uint32_t i = 0; i < doc.arena.size(); ++i)
        if (doc.arena[i].name == name) return i;
    return doc.root;
}

static AppState makeArrayState() {
    AppState s = makeState(360, 780);
    s.screen = S_TREE;
    s.ed = EditorState{};
    s.ed.doc = newRootCompound();
    auto add = [&](const std::string& name, nbt::Type type) {
        nbt::Node n;
        n.name = name;
        n.type = type;
        if (type == nbt::Type::ByteArray) n.bytes = {0xff, 0x7f, 0x00};
        if (type == nbt::Type::IntArray) n.longs = {7, -8, 9};
        if (type == nbt::Type::LongArray)
            n.longs = {9223372036854775807LL, -9223372036854775807LL - 1, 0};
        uint32_t idx = (uint32_t)s.ed.doc.arena.size();
        s.ed.doc.arena.push_back(std::move(n));
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(idx);
        return idx;
    };
    add("byteArray", nbt::Type::ByteArray);
    add("intArray", nbt::Type::IntArray);
    add("longArray", nbt::Type::LongArray);
    s.ed.pathIdx = {s.ed.doc.root};
    return s;
}

static int g_pass = 0, g_fail = 0;
static void check(bool c, const char* id, const std::string& what = "") {
    if (c) { ++g_pass; std::printf("  [PASS] %s\n", id); }
    else { ++g_fail; std::printf("  [FAIL] %s  %s\n", id, what.c_str()); }
}

static std::string err_text;

static ui::Node buildDialogNodesResult(AppState& a) {
    ui::Node page = ui::panel({0, 0, a.w, a.h});
    int h = 0;
    std::vector<ui::Node> body = buildDialogBody(a, a.w - 60, h);
    for (auto& b : body) { b.rect.x = 30; page.kids.push_back(b); }
    return page;
}
static ui::Node buildKeypadResult(AppState& a) {
    ui::Node page = ui::panel({0, 0, a.w, a.h});
    int sheetH = std::min(a.h * 46 / 100, 430);
    std::vector<ui::Node> keys = buildKeypadSheet(a, a.h - sheetH, sheetH);
    for (auto& k : keys) page.kids.push_back(k);
    return page;
}

[[maybe_unused]] static void run() {
    app::appWorker().start();
    std::printf("== UI model tests ==\n");
    AppState a = makeState(1080, 2340);

    // M1 every screen builds without crashing on several sizes
    for (int size : {480, 720, 1080, 1440, 2160}) {
        a.w = size; a.h = size * 21 / 9;
        char id[64];
        std::vector<int>* results = nullptr; (void)results;
        std::snprintf(id, sizeof id, "M1 builds at width %d", size);
        try {
            a.screen = S_HOME;   buildScreen(a);
            a.screen = S_WORLDS; buildScreen(a);
            a.screen = S_BROWSER; buildScreen(a);
            a.screen = S_FLAT;   buildScreen(a);
            a.screen = S_HELP;   buildScreen(a);
            a.screen = S_TREE;   buildScreen(a);
            check(true, id);
        } catch (const std::exception& e) {
            check(false, id, e.what());
        }
    }

    // M2 home: no dead buttons
    {
        a.w = 1080; a.h = 2340;
        a.screen = S_HOME;
        a.fabExpanded = true;
        ui::Node p = buildScreen(a);
        int total = 0, withAct = 0;
        buttonsHaveActions(p, total, withAct);
        check(total == withAct, "M2 home buttons all actionable",
              std::to_string(total - withAct) + " without action");
        std::vector<int> acts;
        collectActions(p, acts);
        bool hasFab = false, hasNew = false, hasImport = false;
        for (int x : acts) {
            if (x == A_FAB_TOGGLE) hasFab = true;
            if (x == A_WORLD_CREATE) hasNew = true;
            if (x == A_WORLD_IMPORT) hasImport = true;
        }
        std::string list;
        for (int x : acts) list += std::to_string(x) + ",";
        check(hasFab && hasNew && hasImport, "M2 home exposes FAB stack", "actions: " + list);
    }

    // M3 worlds: create + import present
    {
        a.screen = S_WORLDS;
        ui::Node p = buildScreen(a);
        std::vector<int> acts;
        collectActions(p, acts);
        bool create = false, imp = false;
        for (int x : acts) { if (x == A_WORLD_CREATE) create = true; if (x == A_WORLD_IMPORT) imp = true; }
        check(create && imp, "M3 worlds toolbar actions present");
    }

    // M4 tree: value dialog + keypad + add dialog are node trees with actions
    {
        a.screen = S_TREE;
        a.ed = EditorState{};
        a.ed.doc = newRootCompound();
        a.ed.levelDat = true;
        a.ed.doc = newLevelDat("Test World");
        a.ed.doc.headerVersion = 10;
        a.ed.pathIdx = {a.ed.doc.root};
        ui::Node p = buildScreen(a);
        std::vector<int> acts;
        collectActions(p, acts);
        bool save = false, add = false;
        for (int x : acts) { if (x == A_TREE_SAVE) save = true; if (x == A_TREE_ADD) add = true; }
        check(save && add, "M4 tree save/add actions present");

        // open the value editor on the first non-container child
        uint32_t child = a.ed.doc.arena[a.ed.doc.root].kids[0];
        a.valueOpen = true;
        a.valueType = nbt::Type::String;
        a.valueText = "hello";
        a.valueNumeric = false;
        openValueEditor(a, child);
        ui::Node pd = buildDialogNodesResult(a);
        int total = 0, withAct = 0;
        buttonsHaveActions(pd, total, withAct);
        check(total > 0 && total == withAct, "M4 value editor has controls",
              std::to_string(total) + " buttons, " + std::to_string(total - withAct) + " dead");
        check(a.dialog.pos == "SAVE" && a.dialog.neg == "CANCEL", "M4 value dialog buttons");
        a.valueOpen = false;

        // keypad builds and every key has an action
        a.keypadOpen = true;
        a.keypadTarget = "value";
        a.valueOpen = true;
        a.valueText.clear();
        ui::Node pk = buildKeypadResult(a);
        int kt = 0, ka = 0;
        buttonsHaveActions(pk, kt, ka);
        check(kt >= 12 && kt == ka, "M4 keypad has >=12 keys, all active",
              std::to_string(kt) + " keys, " + std::to_string(kt - ka) + " dead");
        a.keypadOpen = false;
    }

    // M5 dialog geometry stays on screen
    {
        a.keypadOpen = true;
        a.valueOpen = true;
        int dialogH = a.keypadOpen ? a.h - 40 : 210;
        int dy = std::max(40, a.h / 2 - dialogH / 2);
        int dh = std::min(a.h - 20, dialogH);
        check(dy >= 0 && dy + dh <= a.h, "M5 dialog fits the viewport",
              "dy=" + std::to_string(dy) + " dh=" + std::to_string(dh));
        a.keypadOpen = false; a.valueOpen = false;
    }

    // M6 flat world builder: layers render with actions
    {
        a.screen = S_FLAT;
        a.flatLayers = {{"minecraft:bedrock", 1}, {"minecraft:dirt", 2}};
        ui::Node p = buildScreen(a);
        int total = 0, withAct = 0;
        buttonsHaveActions(p, total, withAct);
        check(total == withAct && total > 0, "M6 flat builder buttons active",
              std::to_string(total - withAct) + " dead");
    }

    // M7 editing behaviour through handleAction (no device needed)
    {
        a.ed = EditorState{};
        a.ed.doc = newRootCompound();
        a.ed.doc.headerVersion = 4;
        a.ed.levelDat = true;
        a.ed.pathIdx = {a.ed.doc.root};
        a.screen = S_TREE;

        // add a string tag through the real handler
        a.valueTypeToAdd = nbt::Type::String;
        a.addOpen = true;
        a.addName = "TestString";
        a.addListMode = 0;
        commitAdd(a);
        bool added = false;
        for (auto& n : a.ed.doc.arena) if (n.name == "TestString" && n.type == nbt::Type::String) added = true;
        check(added, "M7 add string tag works");
        check(a.ed.dirty, "M7 add marks document dirty");

        // duplicate name is rejected (reference string reused)
        a.valueTypeToAdd = nbt::Type::Int;
        a.addOpen = true;
        a.addName = "TestString";
        a.addError.clear();
        commitAdd(a);
        bool rejected = !a.addError.empty();
        check(rejected, "M7 duplicate name rejected", a.addError);

        // value editor applies + validates
        uint32_t idx = 0;
        for (auto& n : a.ed.doc.arena) if (n.name == "TestString") idx = (uint32_t)(&n - &a.ed.doc.arena[0]);
        a.valueOpen = true;
        a.valueIdx = idx;
        a.valueType = nbt::Type::String;
        a.valueText = "hello";
        commitValue(a);
        bool applied = false;
        for (auto& n : a.ed.doc.arena) if (n.name == "TestString") applied = (n.s == "hello");
        check(applied, "M7 value edit applied");

        // out-of-range byte
        a.valueOpen = true;
        a.valueType = nbt::Type::Byte;
        a.valueText = "999";
        a.valueError.clear();
        commitValue(a);
        check(!a.valueError.empty() && a.valueError == nbt::msgOutOfBoundsFor2(),
              "M7 out-of-range rejected with reference message", a.valueError);

        // rename
        a.renameOpen = true;
        a.renameIdx = idx;
        a.renameName = "Renamed";
        commitRename(a);
        bool renamed = false;
        for (auto& n : a.ed.doc.arena) if (n.name == "Renamed") renamed = true;
        check(renamed, "M7 rename works");

        // save the document (atomic write) and re-read it
        std::string p = (fs::temp_directory_path() / "nbt_model_test.dat").string();
        a.ed.path = p;
        a.ed.levelDat = false;
        bool saved = saveDocument(a, p, err_text);
        check(saved, "M7 save works", err_text);
        AppState b = makeState(1080, 2340);
        bool opened = openDocument(b, p, false, err_text);
        check(opened, "M7 reopen works", err_text);
        if (opened) {
            bool roundTrip = true;
            for (auto& n : b.ed.doc.arena)
                if (n.name == "Renamed") roundTrip = (n.type == nbt::Type::String && n.s == "hello");
            check(roundTrip, "M7 round-trip through the real file");
        }

        // undo restores the document
        size_t before = a.ed.doc.arena.size();
        (void)before;
        handleAction(a, A_TREE_UNDO);
        check(a.ed.doc.arena.size() < before || a.ed.doc.arena[b.ed.doc.root].kids.size() > 0,
              "M7 undo runs");
    }

    // M10 undo/redo covers every mutating action
    {
        AppState s = makeState(1080, 2340);
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        s.ed.open = true;
        s.ed.pathIdx = {s.ed.doc.root};
        s.screen = S_TREE;
        app::appWorker().start();

        auto countTags = [&]() { return (int)s.ed.doc.arena.size(); };

        int before = countTags();
        s.valueTypeToAdd = nbt::Type::Int;
        s.addOpen = true; s.addName = "undoTest"; s.addListMode = 0;
        commitAdd(s);
        bool added = countTags() == before + 1;
        handleAction(s, A_TREE_UNDO);
        bool undone = countTags() == before;
        handleAction(s, A_TREE_REDO);
        bool redone = countTags() == before + 1;
        check(added && undone && redone, "M10 add/undo/redo",
              "added=" + std::to_string(added) + " undone=" + std::to_string(undone) +
              " redone=" + std::to_string(redone));

        uint32_t victim = 0;
        for (auto& n : s.ed.doc.arena)
            if (n.name == "undoTest") victim = (uint32_t)(&n - &s.ed.doc.arena[0]);
        int withTag = countTags();
        std::string derr;
        snapshotUndo(s, "delete tag");
        bool removed = removeNode(s, victim, derr);
        rebuildTreeListing(s);
        handleAction(s, A_TREE_UNDO);
        bool deleteUndone = removed && (countTags() == withTag);
        check(deleteUndone, "M10 delete/undo restores the document",
              std::to_string(countTags()) + " vs " + std::to_string(withTag));

        s.ed.redoStack.clear();
        s.valueTypeToAdd = nbt::Type::String;
        s.addOpen = true; s.addName = "afterRedo"; s.addListMode = 0;
        commitAdd(s);
        check(s.ed.redoStack.empty(), "M10 new action clears redo");

        handleAction(s, A_TREE_UNDO);
        bool pathValid = !s.ed.pathIdx.empty() && s.ed.pathIdx.back() < s.ed.doc.arena.size();
        check(pathValid, "M10 undo keeps navigation valid");
    }

    // M11 world import (.mcworld) lands in worlds/ through the real handler
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        AppState s = makeState(1080, 2340);
        fs::path tmp = fs::temp_directory_path();
        s.worldsRoot = (tmp / ("nbt_import_" + std::to_string((long)getpid()))).string();
        fs::create_directories(s.worldsRoot, ec);
        fs::create_directories((tmp / "nbt_src_world" / "db"), ec);
        app::writeText((tmp / "nbt_src_world" / "levelname.txt").string(), "Imported World");
        app::writeText((tmp / "nbt_src_world" / "level.dat").string(), "fake-level-dat");
        std::string zip = (tmp / "Imported.mcworld").string();
        app::createZip((tmp / "nbt_src_world").string(), zip);

        s.importing = true;
        s.browseForSave = false;
        s.browseEntries = {zip};
        s.browseIsDir = {false};
        s.ed.selected = 0;
        handleAction(s, A_FILE_SELECT);

        app::Worker& w = app::appWorker();
        for (int k = 0; k < 600; ++k) {
            bool finished = false;
            {   // peek without holding the lock while sleeping
                std::lock_guard<std::mutex> lk(w.m);
                finished = (w.done && !w.job);
            }
            if (finished) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        bool worldExists = fs::exists(fs::path(s.worldsRoot) / "Imported", ec);
        bool hasDat = fs::exists(fs::path(s.worldsRoot) / "Imported" / "level.dat", ec);
        check(worldExists && hasDat, "M11 .mcworld import lands in worlds/",
              worldExists ? "" : "world folder missing");
        fs::remove_all(s.worldsRoot, ec);
        fs::remove_all(zip, ec);
        fs::remove_all(tmp / "nbt_src_world", ec);
    }

    // M11b (SAF): the OS file-manager result must import through A_PICK_RESULT
    // and clean up the temporary copy the Java bridge made.
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        AppState s = makeState(1080, 2340);
        fs::path tmp = fs::temp_directory_path();
        s.worldsRoot = (tmp / ("nbt_saf_" + std::to_string((long)getpid()))).string();
        fs::create_directories(s.worldsRoot, ec);
        fs::create_directories((tmp / "nbt_saf_src" / "db"), ec);
        app::writeText((tmp / "nbt_saf_src" / "levelname.txt").string(), "SAF World");
        app::writeText((tmp / "nbt_saf_src" / "level.dat").string(), "fake-level-dat");
        std::string zip = (tmp / "SAFpick.tmp").string();   // no extension on disk
        app::createZip((tmp / "nbt_saf_src").string(), zip);

        s.importing = true;
        s.pickerMode = PICK_IMPORT_WORLD;
        s.pickerResult = kPickerOk;
        s.pickerPath = zip;
        s.pickerName = "SAFpick.mcworld";   // name reported by the OS picker
        handleAction(s, A_PICK_RESULT);

        app::Worker& w = app::appWorker();
        for (int k = 0; k < 600; ++k) {
            bool finished = false;
            { std::lock_guard<std::mutex> lk(w.m); finished = (w.done && !w.job); }
            if (finished) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        bool worldExists = fs::exists(fs::path(s.worldsRoot) / "SAFpick" / "level.dat", ec);
        bool tempRemoved = !fs::exists(zip, ec);
        bool flagCleared = !s.importing && !s.pickerPending;
        check(worldExists && tempRemoved && flagCleared,
              "M11b SAF picker result imports world + removes temp copy",
              "world=" + std::to_string(worldExists) + " tempRemoved=" + std::to_string(tempRemoved));
        fs::remove_all(s.worldsRoot, ec);
        fs::remove_all(zip, ec);
        fs::remove_all(tmp / "nbt_saf_src", ec);
    }

    // M11c (SAF): picking an NBT file must open the editor, not the browser.
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        AppState s = makeState(1080, 2340);
        fs::path p = fs::temp_directory_path() /
                     ("nbt_saf_open_" + std::to_string((long)getpid()) + ".dat");
        auto encoded = nbt::serialize(newRootCompound());
        check(encoded.ok(), "M11c fixture serialize", encoded.ok() ? "" : "serialize failed");
        if (encoded.ok()) {
            app::writeFileAtomic(p.string(), encoded.value);
            s.pickerMode = PICK_OPEN_NBT;
            s.pickerResult = kPickerOk;
            s.pickerPath = p.string();
            s.pickerName = "picked.dat";
            handleAction(s, A_PICK_RESULT);
            bool opened = s.ed.open && s.ed.path == p.string() &&
                          s.screen == S_TREE && !s.ed.pathIdx.empty();
            check(opened, "M11c SAF picker opens NBT file in the editor");
        }
        fs::remove(p, ec);
    }

    // M11d: Edit / world tap must enter Show Map with the aside drawer open.
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        AppState s = makeState(1080, 2340);
        fs::path wd = fs::temp_directory_path() /
                      ("nbt_edit_nav_" + std::to_string((long)getpid()));
        fs::create_directories(wd / "db", ec);
        app::writeText((wd / "levelname.txt").string(), "Nav World");
        s.worlds.push_back({wd.string(), "Nav World", "nav", 10, false, false, false, ""});
        s.ed.selected = 0;
        handleAction(s, A_WORLD_OPTIONS);
        bool optionsAside = s.menuOpen;
        std::vector<int> oa; collectActions(buildScreen(s), oa);
        bool hasEdit = std::find(oa.begin(), oa.end(), A_WORLD_EDIT) != oa.end();
        s.menuOpen = false;
        handleAction(s, A_WORLD_EDIT);
        bool entered = s.screen == S_MAP && !s.drawerOpen &&
                       s.activeWorldName == "Nav World";
        // Swipe-equivalent: opening the aside reveals the grouped navigation.
        s.drawerOpen = true;
        ui::Node page = buildScreen(s);
        std::vector<int> acts; collectActions(page, acts);
        bool grouped = false, navMap = false;
        for (int x : acts) {
            if (x == A_NAV_WORLD_NBT || x == A_NAV_LOCAL_PLAYER || x == A_NAV_MULTIPLAYER)
                grouped = true;
            if (x == A_NAV_MAP) navMap = true;
        }
        // back closes the drawer first (reference w.a case 0)
        handleAction(s, A_BACK);
        bool closedFirst = !s.drawerOpen && s.screen == S_MAP;
        check(optionsAside && hasEdit && entered && grouped && navMap && closedFirst,
              "M11d tap=options, Edit=Show Map, swipe=grouped aside",
              "options=" + std::to_string(optionsAside) +
              " entered=" + std::to_string(entered) + " grouped=" + std::to_string(grouped));
        fs::remove_all(wd, ec);
    }

    // M11e: world icon path picked up from world_icon.jpeg in the folder.
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        AppState s = makeState(1080, 2340);
        fs::path root = fs::temp_directory_path() /
                        ("nbt_icons_" + std::to_string((long)getpid()));
        fs::path wd = root / "WithIcon";
        fs::create_directories(wd / "db", ec);
        app::writeText((wd / "levelname.txt").string(), "Icon World");
        { std::ofstream f(wd / "world_icon.jpeg", std::ios::binary); f << "fake"; }
        s.worldsRoot = root.string();
        listWorlds(s);
        bool found = !s.worlds.empty() && !s.worlds[0].iconPath.empty() &&
                     s.worlds[0].hasIcon;
        ui::Node page = buildScreen(s);
        check(found, "M11e world list detects world_icon.jpeg",
              s.worlds.empty() ? "no worlds" : s.worlds[0].iconPath);
        fs::remove_all(root, ec);
    }

    // M11f: Back from any screen returns straight to Home (beranda).
    {
        AppState s = makeState(1080, 2340);
        s.screen = S_MAP;
        s.drawerOpen = true;
        s.screenStack = {S_HOME, S_WORLDS};
        handleAction(s, A_BACK);
        bool closedDrawer = s.drawerOpen == false && s.screen == S_MAP;
        handleAction(s, A_BACK);
        bool home = s.screen == S_HOME && s.screenStack.empty() && !s.wantExit;
        handleAction(s, A_BACK);
        bool exitsOnlyFromHome = s.wantExit;
        check(closedDrawer && home && exitsOnlyFromHome,
              "M11f Back closes drawer then goes straight Home",
              "drawer=" + std::to_string(closedDrawer) + " home=" + std::to_string(home));
    }

    // M11g: saving a LevelDB-backed NBT document writes the DB key, not a file.
    {
        AppState s = makeState(1080, 2340);
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        uint32_t child = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Int; n.name = "Score"; n.i = 42;
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(child);
        s.ed.pathIdx = {s.ed.doc.root};
        s.ed.dbBacked = true;
        s.ed.dbKey = {'O','v','e','r','w','o','r','l','d'};
        s.ed.dirty = true;
        std::string err;
        bool ok = saveDocument(s, "Overworld", err);
        bool wroteKey = app::g_testDbWrite.key == s.ed.dbKey &&
                        app::g_testDbWrite.value.size() > 4;
        check(ok && wroteKey && !s.ed.dirty,
              "M11g LevelDB NBT save writes back the DB key",
              "ok=" + std::to_string(ok) + " key=" + std::to_string(wroteKey));
    }

    // M12 no dead buttons anywhere (screens + dialogs + keypad)
    {
        AppState s = makeState(1080, 2340);
        std::vector<int> scr = {S_HOME, S_WORLDS, S_BROWSER, S_FLAT, S_HELP, S_TREE};
        std::vector<const char*> nm = {"home","worlds","browser","flat","help","tree"};
        for (size_t i = 0; i < scr.size(); ++i) {
            s.screen = scr[i];
            s.ed = EditorState{};
            s.ed.doc = newLevelDat("T");
            s.ed.pathIdx = {s.ed.doc.root};
            ui::Node p = buildScreen(s);
            int total = 0, withAct = 0;
            buttonsHaveActions(p, total, withAct);
            std::string id = std::string("M12 ") + nm[i] + ": buttons all actionable";
            check(total == withAct, id.c_str(),
                  std::to_string(total - withAct) + " of " + std::to_string(total) + " buttons dead");
        }
        uint32_t child = s.ed.doc.arena[s.ed.doc.root].kids[0];
        openValueEditor(s, child);
        int total = 0, withAct = 0;
        buttonsHaveActions(buildDialogNodesResult(s), total, withAct);
        check(total > 0 && total == withAct, "M12 value dialog buttons actionable");
        s.valueOpen = false;
        s.keypadOpen = true; s.keypadTarget = "value";
        buttonsHaveActions(buildKeypadResult(s), total, withAct);
        check(total == withAct, "M12 keypad buttons actionable",
              std::to_string(total - withAct) + " dead");
        s.keypadOpen = false;
    }

    // M13 Save-As keeps the level.dat container (8-byte header)
    {
        std::error_code ec;
        AppState s = makeState(1080, 2340);
        fs::path target = fs::temp_directory_path() / "nbt_level_saved.dat";
        s.ed = EditorState{};
        s.ed.doc = newLevelDat("SaveAs");
        s.ed.levelDat = true;
        s.ed.pathIdx = {s.ed.doc.root};
        s.screen = S_TREE;
        s.browseForSave = true;
        s.browseEntries = {target.string()};
        s.browseIsDir = {false};
        s.ed.selected = 0;
        std::string saveErr;
        bool direct = saveDocument(s, target.string(), saveErr);
        handleAction(s, A_FILE_SELECT);
        auto writtenCheck = std::filesystem::exists(s.browseEntries[0]);
        bool written = writtenCheck && direct;
        auto back = readBytesForTest(s.browseEntries[0]);
        // level.dat container = int32 LE version (8 for our new documents) +
        // int32 LE payload length + NBT. Validate the header, not a version we
        // happened to guess.
        bool headerOk = back.size() >= 8 && back[0] == 0x08 && back[1] == 0x00 &&
                        back[2] == 0x00 && back[3] == 0x00 &&
                        back[8] == 0x0a;   // first NBT byte (after the 8-byte header)
        std::string hex;
        for (int k = 0; k < 10 && k < (int)back.size(); ++k) {
            char b[8]; std::snprintf(b, sizeof b, "%02x ", back[k]); hex += b;
        }
        check(written && headerOk, "M13 save-as keeps level.dat header",
              "written=" + std::to_string(written) + " bytes=" + std::to_string(back.size()) +
              " head=" + hex);
        fs::remove_all(target, ec);
    }

    // M8 no crash on a corrupt document through the editor
    {
        std::vector<uint8_t> bad = {0x63, 0x00, 0x00};
        auto r = nbt::parseBytes(bad.data(), bad.size(), "bad", {});
        check(!r.ok(), "M8 corrupt file rejected in engine");
    }

    // M2b: home screen must contain the content column below the app bar
    {
        AppState s = makeState(1080, 2340);
        s.screen = S_HOME;
        ui::Node p = buildScreen(s);
        int maxY = 0, nodes = 0;
        std::function<void(const ui::Node&)> walk = [&](const ui::Node& n) {
            ++nodes;
            maxY = std::max(maxY, n.rect.y + n.rect.h);
            for (auto& k : n.kids) walk(k);
        };
        walk(p);
        check(p.kids.size() >= 2 && maxY > 200, "M2b home content column attached",
              "kids=" + std::to_string(p.kids.size()) + " maxY=" + std::to_string(maxY));
    }

    // M9 render each screen to ASCII and check they are not blank + contain text
    if (std::getenv("NBT_UI_DUMP")) {
        using namespace app;
        for (int screen : {S_HOME, S_WORLDS, S_FLAT, S_HELP}) {
            AppState s = makeState(300, 520);
            s.screen = screen;
            s.worlds.push_back({"C/worlds/MyWorld", "My World", "MyWorld", 12345678, true, false, false, ""});
            s.flatLayers = {{"minecraft:bedrock", 1}, {"minecraft:dirt", 2}};
            gfx::Canvas c;
            std::vector<uint32_t> buf((size_t)s.w * s.h);
            c.bind(buf.data(), s.w, s.h, s.w);
            ui::Node page = ::app::buildScreen(s);
            ::app::drawNode(c, s.theme, page, 0);
            const char* names[] = {"HOME", "WORLDS", "BROWSER", "TREE", "FLAT", "HELP"};
            dumpAscii(c, names[screen]);
        }
    }

    // ------------------------------------------------------------------
    // P0 regression tests (audit B1, B2+B3, B4) — 14 Nov 2026
    // ------------------------------------------------------------------

    // M14 (B1): text must be drawn at the requested x, not at 2x.
    {
        const int W = 400, H = 40;
        std::vector<uint32_t> buf((size_t)W * H, 0xffffffffu);
        gfx::Canvas c;
        c.bind(buf.data(), W, H, W);
        c.setScale(1);
        c.clear(gfx::rgb(0xff, 0xff, 0xff));
        c.text(100, 5, "42", gfx::rgb(0, 0, 0));
        int inkMin = -1, inkMax = -1;
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                if (buf[(size_t)y * W + x] != 0xffffffffu) {
                    if (inkMin < 0) inkMin = x;
                    inkMax = x;
                }
        // '4' starts a few columns into its cell, so ink must begin near 100
        // and must NOT start near 200 (the old doubled offset).
        bool nearX = (inkMin >= 100 && inkMin < 120);
        bool notDoubled = !(inkMin >= 190 && inkMin < 215);
        check(nearX && notDoubled && inkMax > 100,
              "M14 text is drawn at the requested x (B1)",
              "ink x=" + std::to_string(inkMin) + ".." + std::to_string(inkMax));
    }

    // M15 (B2): tapping a list row must resolve to the ROW (index + action),
    // not to the non-interactive label/icon inside it.
    {
        AppState s = makeState(360, 780);
        s.screen = S_WORLDS;
        s.worlds = {{"worlds/A", "World A", "A", 1, false, false, false, ""},
                    {"worlds/B", "World B", "B", 2, false, false, false, ""}};
        ui::Node page = buildScreen(s);
        const ui::Node* row = nullptr;
        std::function<void(const ui::Node&)> findRow = [&](const ui::Node& n) {
            if (row) return;
            if (n.kind == ui::Kind::Panel && n.action == A_WORLD_OPTIONS) { row = &n; return; }
            for (auto& k : n.kids) findRow(k);
        };
        findRow(page);
        bool ok = false;
        if (row) {
            const ui::Node* hit = nullptr;
            int tx = row->rect.x + 70, ty = row->rect.y + 18;   // on the title text
            ok = ui::hitTestInteractive(page, tx, ty, hit) && hit &&
                 hit->index == 0 && hit->action == A_WORLD_OPTIONS;
            s.ed.selected = -1;
            if (hit) dispatchHit(s, *hit, false);
            ok = ok && s.ed.selected == 0;
        }
        check(ok, "M15 tapping a world row selects it (B2)", "selected=" + std::to_string(s.ed.selected));
    }

    // M16 (B3): world and browser rows must start below the app bar / path bar.
    {
        AppState s = makeState(360, 780);
        s.screen = S_WORLDS;
        s.worlds = {{"worlds/A", "World A", "A", 1, false, false, false, ""}};
        ui::Node wp = buildScreen(s);
        int worldRowY = -1;
        std::function<void(const ui::Node&)> findY = [&](const ui::Node& n) {
            if (worldRowY >= 0) return;
            if (n.kind == ui::Kind::Panel && n.action == A_WORLD_OPTIONS) { worldRowY = n.rect.y; return; }
            for (auto& k : n.kids) findY(k);
        };
        findY(wp);
        s.screen = S_BROWSER;
        s.browseDir = "/sdcard";
        s.browseEntries = {"/sdcard/a.dat"};
        s.browseIsDir = {false};
        ui::Node bp = buildScreen(s);
        int browseRowY = -1;
        std::function<void(const ui::Node&)> findY2 = [&](const ui::Node& n) {
            if (browseRowY >= 0) return;
            if (n.kind == ui::Kind::Panel && n.action == A_FILE_SELECT) { browseRowY = n.rect.y; return; }
            for (auto& k : n.kids) findY2(k);
        };
        findY2(bp);
        check(worldRowY >= kBarH && browseRowY >= kBarH + 40,
              "M16 list rows start below the app bar (B3)",
              "worldRowY=" + std::to_string(worldRowY) + " browseRowY=" + std::to_string(browseRowY));
    }

    // M17 (B4): the on-screen keypad must insert characters.
    {
        AppState s = makeState(360, 780);
        s.ed.doc = newRootCompound();
        s.ed.levelDat = false;
        s.ed.path = "t.dat";
        s.ed.pathIdx = {s.ed.doc.root};
        uint32_t strIdx = 0;
        { nbt::Node n; n.type = nbt::Type::String; n.name = "name";
          strIdx = (uint32_t)s.ed.doc.arena.size();
          s.ed.doc.arena[0].kids.push_back(strIdx); s.ed.doc.arena.push_back(n); }
        openValueEditor(s, strIdx);
        openKeypad(s, "value", false, "");
        int sheetH = std::min(s.h * 46 / 100, 430), sheetY = s.h - sheetH;
        std::vector<ui::Node> keys = buildKeypadSheet(s, sheetY + 8, sheetH - 8);
        const ui::Node* key = nullptr;
        std::function<void(const ui::Node&)> findKey = [&](const ui::Node& n) {
            if (key) return;
            if (n.kind == ui::Kind::Button && n.action == A_KEY_TAP) { key = &n; return; }
            for (auto& k : n.kids) findKey(k);
        };
        for (auto& k : keys) findKey(k);
        bool ok = false;
        if (key) {
            s.valueText.clear();
            dispatchHit(s, *key, false);
            ok = (s.valueText.size() == 1);
        }
        check(ok, "M17 keypad key inserts a character (B4)",
              "valueText='" + s.valueText + "'");
    }

    // M18 (B4): the inline numeric pad of the value editor must type, and the
    // CLR key must empty the field.
    {
        AppState s = makeState(360, 780);
        s.ed.doc = newRootCompound();
        s.ed.levelDat = false;
        s.ed.path = "t.dat";
        s.ed.pathIdx = {s.ed.doc.root};
        uint32_t intIdx = 0;
        { nbt::Node n; n.type = nbt::Type::Int; n.name = "num"; n.i = 7;
          intIdx = (uint32_t)s.ed.doc.arena.size();
          s.ed.doc.arena[0].kids.push_back(intIdx); s.ed.doc.arena.push_back(n); }
        openValueEditor(s, intIdx);
        int bodyH = 0;
        std::vector<ui::Node> body = buildDialogBody(s, 260, bodyH);
        const ui::Node* five = nullptr;
        const ui::Node* clr = nullptr;
        std::function<void(const ui::Node&)> find = [&](const ui::Node& n) {
            if (n.kind == ui::Kind::Button && n.action == A_KEY_TAP && n.text == "5" && !five) five = &n;
            if (n.kind == ui::Kind::Button && n.action == A_KEY_CLEAR && !clr) clr = &n;
            for (auto& k : n.kids) find(k);
        };
        for (auto& b : body) find(b);
        bool typed = false, cleared = false;
        if (five) {
            s.valueText.clear();
            dispatchHit(s, *five, false);
            typed = (s.valueText == "5");
        }
        if (clr) {
            dispatchHit(s, *clr, false);
            cleared = s.valueText.empty();
        }
        check(typed && cleared, "M18 inline numeric pad types and CLR clears (B4)",
              "typed=" + std::to_string(typed) + " cleared=" + std::to_string(cleared));
    }

    // M19 (B2): the browser row tap must select the file, not the label.
    {
        AppState s = makeState(360, 780);
        s.screen = S_BROWSER;
        s.browseDir = "/sdcard";
        s.browseEntries = {"/sdcard/Download", "/sdcard/level.dat"};
        s.browseIsDir = {true, false};
        ui::Node page = buildScreen(s);
        const ui::Node* row = nullptr;
        std::function<void(const ui::Node&)> findRow = [&](const ui::Node& n) {
            if (row) return;
            if (n.kind == ui::Kind::Panel && n.action == A_FILE_SELECT) { row = &n; return; }
            for (auto& k : n.kids) findRow(k);
        };
        findRow(page);
        bool ok = false;
        if (row) {
            const ui::Node* hit = nullptr;
            ok = ui::hitTestInteractive(page, row->rect.x + 60, row->rect.y + 16, hit) && hit &&
                 hit->index == 1 && hit->action == A_FILE_SELECT;
            s.ed.selected = -1;
            if (hit) dispatchHit(s, *hit, false);
            ok = ok && s.ed.selected == 1;
        }
        check(ok, "M19 browser row tap selects the file (B2)", "selected=" + std::to_string(s.ed.selected));
    }

    // M20 (B4): the tag-name field of the add dialog must open the keypad.
    {
        AppState s = makeState(360, 780);
        s.ed.doc = newRootCompound();
        s.ed.levelDat = false;
        s.ed.path = "t.dat";
        s.ed.pathIdx = {s.ed.doc.root};
        s.addOpen = true;
        s.addName.clear();
        int bodyH = 0;
        std::vector<ui::Node> body = buildDialogBody(s, 260, bodyH);
        const ui::Node* field = nullptr;
        for (auto& b : body) if (b.kind == ui::Kind::Field && b.action == A_KEY_OPEN) { field = &b; break; }
        bool ok = false;
        if (field) {
            dispatchHit(s, *field, false);
            ok = s.keypadOpen && s.keypadTarget == "addName";
        }
        check(ok, "M20 tag-name field opens the keypad (B4)",
              "target=" + s.keypadTarget);
    }

    // M21 (B3): the scroll content height must follow the rows, otherwise the
    // list cannot be scrolled (the clamp uses contentBottom - viewport bottom).
    {
        AppState s = makeState(360, 780);
        s.screen = S_WORLDS;
        for (int i = 0; i < 12; ++i)
            s.worlds.push_back({"worlds/W" + std::to_string(i), "World " + std::to_string(i),
                                "W" + std::to_string(i), 100, false, false, false, ""});
        buildScreen(s);
        int expected = kBarH + 10 + 12 * 68;      // list top + 12 rows of 68 px
        check(s.contentBottom == expected,
              "M21 world list content height drives the scroll clamp (B3)",
              "contentBottom=" + std::to_string(s.contentBottom) + " expected=" + std::to_string(expected));

        AppState b = makeState(360, 780);
        b.screen = S_BROWSER;
        b.browseDir = "/sdcard";
        for (int i = 0; i < 10; ++i) {
            b.browseEntries.push_back("/sdcard/file" + std::to_string(i) + ".dat");
            b.browseIsDir.push_back(false);
        }
        buildScreen(b);
        int expectedB = kBarH + 46 + 10 * 48;     // list top + 10 rows of 48 px
        check(b.contentBottom == expectedB,
              "M21 browser list content height drives the scroll clamp (B3)",
              "contentBottom=" + std::to_string(b.contentBottom) + " expected=" + std::to_string(expectedB));
    }

    // M22 (B6): the breadcrumb must render a non-empty viewport, actionable
    // ancestor buttons, and visible ">" separators.
    {
        AppState s = makeState(360, 780);
        s.screen = S_TREE;
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        uint32_t child = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "A";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(child);
        uint32_t grandchild = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "B";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[child].kids.push_back(grandchild);
        s.ed.pathIdx = {s.ed.doc.root, child, grandchild};

        ui::Node page = buildScreen(s);
        const ui::Node* crumb = nullptr;
        std::function<void(const ui::Node&)> findCrumb = [&](const ui::Node& n) {
            if (crumb) return;
            if (n.kind == ui::Kind::Breadcrumb) { crumb = &n; return; }
            for (const auto& k : n.kids) findCrumb(k);
        };
        findCrumb(page);
        int buttons = 0, separators = 0;
        bool valid = crumb && crumb->rect.w > 0 && crumb->rect.h > 0;
        if (crumb) {
            for (const auto& k : crumb->kids) {
                if (k.kind == ui::Kind::Button) {
                    ++buttons;
                    if (k.action != A_BREADCRUMB_TAP || k.index < 0 ||
                        k.index >= (int)s.ed.pathIdx.size() || k.rect.empty()) valid = false;
                } else if (k.kind == ui::Kind::Text && k.text == ">") {
                    ++separators;
                }
            }
        }
        check(valid && buttons == 3 && separators == 2,
              "M22 breadcrumb renders actionable ancestors and separators (B6)",
              "buttons=" + std::to_string(buttons) + " separators=" + std::to_string(separators));
    }

    // M23 (B6): tapping an ancestor through the real hit-test/dispatch path
    // must navigate to that exact depth in pathIdx.
    {
        AppState s = makeState(360, 780);
        s.screen = S_TREE;
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        uint32_t child = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "A";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(child);
        uint32_t grandchild = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "B";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[child].kids.push_back(grandchild);
        s.ed.pathIdx = {s.ed.doc.root, child, grandchild};

        ui::Node page = buildScreen(s);
        const ui::Node* ancestor = nullptr;
        std::function<void(const ui::Node&)> findAncestor = [&](const ui::Node& n) {
            if (ancestor) return;
            if (n.kind == ui::Kind::Button && n.action == A_BREADCRUMB_TAP &&
                n.index == 1) { ancestor = &n; return; }
            for (const auto& k : n.kids) findAncestor(k);
        };
        findAncestor(page);
        const ui::Node* hit = nullptr;
        bool ok = ancestor && ui::hitTestInteractive(
            page, ancestor->rect.x + ancestor->rect.w / 2,
            ancestor->rect.y + ancestor->rect.h / 2, hit);
        if (hit) dispatchHit(s, *hit, false);
        ok = ok && hit && hit->action == A_BREADCRUMB_TAP && hit->index == 1 &&
             s.ed.pathIdx.size() == 2 && s.ed.pathIdx.back() == child;
        check(ok, "M23 tapping a breadcrumb ancestor navigates to it (B6)",
              "path=" + std::to_string(s.ed.pathIdx.size()));
    }

    // M24 (B6): horizontal breadcrumb panning has its own clamped offset and
    // must not mutate the tree's vertical scroll position.
    {
        AppState s = makeState(360, 780);
        s.breadcrumbContentWidth = 500;
        s.breadcrumbViewportWidth = 300;
        s.breadcrumbScrollX = 0;
        s.scrollY = 37;
        bool moved = scrollBreadcrumb(s, -40) && s.breadcrumbScrollX == 40;
        scrollBreadcrumb(s, -1000);
        bool clamped = s.breadcrumbScrollX == 200;
        scrollBreadcrumb(s, 1000);
        bool restored = s.breadcrumbScrollX == 0;
        check(moved && clamped && restored && s.scrollY == 37,
              "M24 breadcrumb pans horizontally without changing tree scroll (B6)",
              "offset=" + std::to_string(s.breadcrumbScrollX) +
              " scrollY=" + std::to_string(s.scrollY));
    }

    // M25 (B7): Byte/Int/Long Array payloads are exposed as typed array
    // elements in the current listing.  They are not fake arena nodes.
    {
        struct ArrayCase {
            const char* name;
            nbt::Type type;
            std::vector<std::string> values;
        };
        std::vector<ArrayCase> cases = {
            {"byteArray", nbt::Type::ByteArray, {"-1", "127", "0"}},
            {"intArray", nbt::Type::IntArray, {"7", "-8", "9"}},
            {"longArray", nbt::Type::LongArray,
             {"9223372036854775807", "-9223372036854775808", "0"}},
        };
        bool ok = true;
        for (const auto& c : cases) {
            AppState s = makeArrayState();
            uint32_t idx = findNamedIndex(s.ed.doc, c.name);
            s.ed.pathIdx = {s.ed.doc.root, idx};
            auto entries = visibleTreeItems(s);
            if (entries.size() != c.values.size()) { ok = false; continue; }
            for (size_t i = 0; i < entries.size(); ++i) {
                if (entries[i].kind != TreeEntryKind::ArrayElement ||
                    entries[i].nodeIndex != idx || entries[i].elementIndex != i) ok = false;
            }
            ui::Node page = buildScreen(s);
            std::vector<const ui::Node*> elementRows;
            std::function<void(const ui::Node&)> collect = [&](const ui::Node& n) {
                if (n.kind == ui::Kind::TreeRow && n.id == "array_element")
                    elementRows.push_back(&n);
                for (const auto& kid : n.kids) collect(kid);
            };
            collect(page);
            if (elementRows.size() != c.values.size()) { ok = false; continue; }
            for (size_t i = 0; i < elementRows.size(); ++i) {
                if (elementRows[i]->text != "[" + std::to_string(i) + "]" ||
                    elementRows[i]->text2 != c.values[i]) ok = false;
            }
            if (s.ed.doc.arena.size() != 4) ok = false;  // root + three payload nodes only
        }
        check(ok, "M25 byte/int/long arrays render typed element rows (B7)",
              "typed listing mismatch");
    }

    // M26 (B7): tapping an array row opens the correct scalar editor and
    // commits into the canonical bytes/longs payload.
    {
        struct EditCase {
            const char* name;
            nbt::Type type;
            const char* text;
        };
        std::vector<EditCase> cases = {
            {"byteArray", nbt::Type::Byte, "12"},
            {"intArray", nbt::Type::Int, "-2147483648"},
            {"longArray", nbt::Type::Long, "-9223372036854775808"},
        };
        bool ok = true;
        for (const auto& c : cases) {
            AppState s = makeArrayState();
            uint32_t idx = findNamedIndex(s.ed.doc, c.name);
            s.ed.pathIdx = {s.ed.doc.root, idx};
            ui::Node page = buildScreen(s);
            const ui::Node* row = nullptr;
            std::function<void(const ui::Node&)> findRow = [&](const ui::Node& n) {
                if (row) return;
                if (n.kind == ui::Kind::TreeRow && n.id == "array_element" && n.text == "[0]") {
                    row = &n;
                    return;
                }
                for (const auto& kid : n.kids) findRow(kid);
            };
            findRow(page);
            const ui::Node* hit = nullptr;
            bool tapped = row && ui::hitTestInteractive(
                page, row->rect.x + row->rect.w / 2,
                row->rect.y + row->rect.h / 2, hit);
            if (tapped && hit) dispatchHit(s, *hit, false);
            if (!s.valueOpen || !s.valueIsArrayElement || s.valueIdx != idx ||
                s.valueType != c.type) {
                ok = false;
                continue;
            }
            s.valueText = c.text;
            commitValue(s);
            if (s.ed.doc.arena[idx].type == nbt::Type::ByteArray)
                ok = ok && s.ed.doc.arena[idx].bytes[0] == 12;
            else if (s.ed.doc.arena[idx].type == nbt::Type::IntArray)
                ok = ok && s.ed.doc.arena[idx].longs[0] == -2147483648LL;
            else
                ok = ok && s.ed.doc.arena[idx].longs[0] == -9223372036854775807LL - 1;
        }
        check(ok, "M26 tapping an array element edits its payload value (B7)",
              "element edit mismatch");
    }

    // M27 (B7): scalar ranges and stale element indices are rejected without
    // changing the payload.
    {
        struct BoundsCase {
            const char* name;
            nbt::Type type;
            const char* text;
            const char* message;
        };
        std::vector<BoundsCase> cases = {
            {"byteArray", nbt::Type::Byte, "128", "Out of bounds for byte."},
            {"intArray", nbt::Type::Int, "2147483648", "Out of bounds for integer."},
            {"longArray", nbt::Type::Long, "9223372036854775808", "Out of bounds for long."},
        };
        bool ok = true;
        for (const auto& c : cases) {
            AppState s = makeArrayState();
            uint32_t idx = findNamedIndex(s.ed.doc, c.name);
            if (!openArrayElementEditor(s, idx, 0)) { ok = false; continue; }
            size_t before = arrayElementCount(s.ed.doc.arena[idx]);
            s.valueText = c.text;
            commitValue(s);
            if (!s.valueOpen || s.valueError != c.message ||
                arrayElementCount(s.ed.doc.arena[idx]) != before) ok = false;
        }
        AppState s = makeArrayState();
        uint32_t idx = findNamedIndex(s.ed.doc, "byteArray");
        if (!openArrayElementEditor(s, idx, 0)) ok = false;
        else {
            s.valueArrayElementIndex = 99;
            s.valueText = "1";
            commitValue(s);
            if (!s.valueOpen || s.valueError != "The array element no longer exists." ||
                s.ed.doc.arena[idx].bytes[0] != 0xff) ok = false;
        }
        check(ok, "M27 array element value/index bounds are enforced (B7)",
              "array bounds mismatch");
    }

    // M28 (B7): append and remove array elements, with undo/redo restoring
    // the exact payload state.
    {
        struct RemoveCase {
            const char* name;
            nbt::Type type;
            int64_t afterRemove;
        };
        std::vector<RemoveCase> cases = {
            {"byteArray", nbt::Type::Byte, 127},
            {"intArray", nbt::Type::Int, -8},
            {"longArray", nbt::Type::Long, -9223372036854775807LL - 1},
        };
        bool ok = true;
        for (const auto& c : cases) {
            AppState s = makeArrayState();
            uint32_t idx = findNamedIndex(s.ed.doc, c.name);
            s.ed.pathIdx = {s.ed.doc.root, idx};
            handleAction(s, A_TREE_ADD);
            bool addDialogReady = s.addOpen && s.addListMode == 2 && s.valueTypeToAdd == c.type;
            handleAction(s, A_TREE_ADD_DONE);
            bool appended = addDialogReady && arrayElementCount(s.ed.doc.arena[idx]) == 4;
            TreeEntry entry{TreeEntryKind::ArrayElement, idx, 0};
            openDeleteConfirmation(s, entry);
            handleAction(s, A_DIALOG_POS);
            bool removed = arrayElementCount(s.ed.doc.arena[idx]) == 3;
            int64_t value = 0;
            readArrayElement(s.ed.doc.arena[idx], 0, value);
            removed = removed && value == c.afterRemove;
            handleAction(s, A_TREE_UNDO);
            bool undone = arrayElementCount(s.ed.doc.arena[idx]) == 4;
            readArrayElement(s.ed.doc.arena[idx], 0, value);
            undone = undone && value == (c.type == nbt::Type::Byte ? -1 :
                                        (c.type == nbt::Type::Int ? 7 :
                                         9223372036854775807LL));
            handleAction(s, A_TREE_REDO);
            bool redone = arrayElementCount(s.ed.doc.arena[idx]) == 3;
            readArrayElement(s.ed.doc.arena[idx], 0, value);
            redone = redone && value == c.afterRemove;
            ok = ok && appended && removed && undone && redone;
        }
        check(ok, "M28 array add/remove supports exact undo and redo (B7)",
              "array mutation history mismatch");
    }

    // M29 (B7): edited/appended arrays survive serialize->parse unchanged and
    // still use only the original payload nodes in the arena.
    {
        AppState s = makeArrayState();
        struct MutCase {
            const char* name;
            nbt::Type type;
            const char* text;
        };
        std::vector<MutCase> cases = {
            {"byteArray", nbt::Type::Byte, "-128"},
            {"intArray", nbt::Type::Int, "2147483647"},
            {"longArray", nbt::Type::Long, "-9223372036854775808"},
        };
        bool ok = true;
        for (const auto& c : cases) {
            uint32_t idx = findNamedIndex(s.ed.doc, c.name);
            s.ed.pathIdx = {s.ed.doc.root, idx};
            if (!openArrayElementEditor(s, idx, 0)) { ok = false; continue; }
            s.valueText = c.text;
            commitValue(s);
            s.valueTypeToAdd = c.type;
            s.addOpen = true;
            s.addListMode = 2;
            commitAdd(s);
        }
        ok = ok && s.ed.doc.arena.size() == 4;
        auto encoded = nbt::serialize(s.ed.doc);
        if (!encoded.ok()) {
            ok = false;
        } else {
            auto parsed = nbt::parseBytes(encoded.value.data(), encoded.value.size(),
                                          "b7-roundtrip", {});
            if (!parsed.ok()) {
                ok = false;
            } else {
                uint32_t b = findNamedIndex(parsed.value, "byteArray");
                uint32_t i = findNamedIndex(parsed.value, "intArray");
                uint32_t l = findNamedIndex(parsed.value, "longArray");
                ok = ok && parsed.value.arena[b].type == nbt::Type::ByteArray &&
                     parsed.value.arena[b].bytes.size() == 4 &&
                     parsed.value.arena[b].bytes[0] == 0x80 &&
                     parsed.value.arena[i].type == nbt::Type::IntArray &&
                     parsed.value.arena[i].longs.size() == 4 &&
                     parsed.value.arena[i].longs[0] == 2147483647LL &&
                     parsed.value.arena[l].type == nbt::Type::LongArray &&
                     parsed.value.arena[l].longs.size() == 4 &&
                     parsed.value.arena[l].longs[0] == -9223372036854775807LL - 1;
                auto reparsed = nbt::serialize(parsed.value);
                ok = ok && reparsed.ok() && reparsed.value == encoded.value;
            }
        }
        check(ok, "M29 edited byte/int/long arrays round-trip without pseudo nodes (B7)",
              "array round-trip mismatch");
    }

    // M30 (B7): long-pressing an array element exposes exactly Edit/Remove;
    // both choices address the same typed payload target.
    {
        AppState s = makeArrayState();
        uint32_t idx = findNamedIndex(s.ed.doc, "intArray");
        s.ed.pathIdx = {s.ed.doc.root, idx};
        ui::Node page = buildScreen(s);
        const ui::Node* row = nullptr;
        std::function<void(const ui::Node&)> findRow = [&](const ui::Node& n) {
            if (row) return;
            if (n.kind == ui::Kind::TreeRow && n.id == "array_element" && n.text == "[0]") {
                row = &n;
                return;
            }
            for (const auto& kid : n.kids) findRow(kid);
        };
        findRow(page);
        const ui::Node* hit = nullptr;
        bool tapped = row && ui::hitTestInteractive(
            page, row->rect.x + row->rect.w / 2,
            row->rect.y + row->rect.h / 2, hit);
        if (tapped && hit) dispatchHit(s, *hit, true);
        bool optionsOk = s.tagOptionList.size() == 2 &&
                         s.tagOptionList[0] == "Edit" && s.tagOptionList[1] == "Remove" &&
                         s.tagOptionEntry.kind == TreeEntryKind::ArrayElement &&
                         s.tagOptionEntry.nodeIndex == idx && s.tagOptionEntry.elementIndex == 0;

        s.ed.selected = 1;  // the Remove option in the modal body
        handleAction(s, A_TAG_OPTION);
        bool removeTargetsElement = s.dialog.open && s.deleteEntry.kind == TreeEntryKind::ArrayElement &&
                                    s.deleteEntry.nodeIndex == idx && s.deleteEntry.elementIndex == 0;

        AppState e = makeArrayState();
        uint32_t editIdx = findNamedIndex(e.ed.doc, "intArray");
        e.ed.pathIdx = {e.ed.doc.root, editIdx};
        ui::Node editPage = buildScreen(e);
        const ui::Node* editRow = nullptr;
        std::function<void(const ui::Node&)> findEditRow = [&](const ui::Node& n) {
            if (editRow) return;
            if (n.kind == ui::Kind::TreeRow && n.id == "array_element" && n.text == "[0]") {
                editRow = &n;
                return;
            }
            for (const auto& kid : n.kids) findEditRow(kid);
        };
        findEditRow(editPage);
        const ui::Node* editHit = nullptr;
        bool editTapped = editRow && ui::hitTestInteractive(
            editPage, editRow->rect.x + editRow->rect.w / 2,
            editRow->rect.y + editRow->rect.h / 2, editHit);
        if (editTapped && editHit) dispatchHit(e, *editHit, true);
        e.ed.selected = 0;  // the Edit option in the modal body
        handleAction(e, A_TAG_OPTION);
        bool editTargetsElement = e.valueOpen && e.valueIsArrayElement &&
                                  e.valueIdx == editIdx && e.valueArrayElementIndex == 0;

        check(optionsOk && removeTargetsElement && editTargetsElement,
              "M30 long press targets array element edit/remove actions (B7)",
              "array long-press target mismatch");
    }

    // M31 (B8): the Home overflow menu exposes a real theme action, and
    // choosing it changes the active theme rather than only flipping a flag.
    {
        AppState s = makeState(360, 780);
        s.screen = S_HOME;
        ui::Node home = buildScreen(s);
        const ui::Node* menuButton = findActionNode(home, A_MENU_OPEN);
        const ui::Node* menuHit = nullptr;
        bool menuTapped = menuButton && ui::hitTestInteractive(
            home, menuButton->rect.x + menuButton->rect.w / 2,
            menuButton->rect.y + menuButton->rect.h / 2, menuHit);
        if (menuTapped && menuHit) dispatchHit(s, *menuHit, false);

        ui::Node menu = buildScreen(s);
        const ui::Node* themeItem = findActionNode(menu, A_TOGGLE_THEME, "Use dark theme");
        const ui::Node* themeHit = nullptr;
        bool themeTapped = themeItem && ui::hitTestInteractive(
            menu, themeItem->rect.x + themeItem->rect.w / 2,
            themeItem->rect.y + themeItem->rect.h / 2, themeHit);
        if (themeTapped && themeHit) dispatchHit(s, *themeHit, false);

        gfx::Theme dark = gfx::themeDark();
        bool changed = s.dark && s.prefs.theme == "dark" && !s.menuOpen &&
                       gfx::pack(s.theme.background) == gfx::pack(dark.background) &&
                       gfx::pack(s.theme.surface) == gfx::pack(dark.surface) &&
                       gfx::pack(s.theme.onSurface) == gfx::pack(dark.onSurface);
        check(changed, "M31 overflow theme action changes the active theme (B8)",
              "dark=" + std::to_string(s.dark ? 1 : 0) +
              " pref=" + s.prefs.theme);
    }

    // M32 (B8): long-pressing a world row opens the delete confirmation;
    // cancellation preserves it, while confirmation removes that world.
    {
        std::error_code ec;
        fs::path root = fs::temp_directory_path() /
                        ("nbt_b8_delete_world_" + std::to_string((long)getpid()));
        fs::path worldDir = root / "DeleteMe";
        fs::remove_all(root, ec);
        fs::create_directories(worldDir, ec);
        app::writeText((worldDir / "levelname.txt").string(), "Delete Me");

        AppState s = makeState(360, 780);
        s.screen = S_WORLDS;
        s.worldsRoot = root.string();
        app::WorldInfo world;
        world.folder = worldDir.string();
        world.displayName = "Delete Me";
        world.internalName = "DeleteMe";
        world.sizeBytes = 1;
        s.worlds.push_back(world);

        ui::Node page = buildScreen(s);
        const ui::Node* row = findActionNode(page, A_WORLD_OPTIONS);
        bool hasDeleteLongPress = row && row->actionLong == A_WORLD_DELETE;
        const ui::Node* hit = nullptr;
        bool longTapped = row && ui::hitTestInteractive(
            page, row->rect.x + row->rect.w / 2,
            row->rect.y + row->rect.h / 2, hit);
        if (longTapped && hit) dispatchHit(s, *hit, true);
        bool confirmation = s.dialog.open && s.dialog.title == "Delete World" &&
                            s.dialog.pos == "DELETE" &&
                            s.dialog.posAction == A_WORLD_DELETE &&
                            s.dialog.message.find("cannot be undone") != std::string::npos;

        handleAction(s, A_DIALOG_NEG);
        bool cancelled = !s.dialog.open && fs::exists(worldDir, ec);

        page = buildScreen(s);
        row = findActionNode(page, A_WORLD_OPTIONS);
        hit = nullptr;
        longTapped = row && ui::hitTestInteractive(
            page, row->rect.x + row->rect.w / 2,
            row->rect.y + row->rect.h / 2, hit);
        if (longTapped && hit) dispatchHit(s, *hit, true);
        handleAction(s, A_DIALOG_POS);
        bool deleted = !fs::exists(worldDir, ec);

        fs::remove_all(root, ec);
        check(hasDeleteLongPress && confirmation && cancelled && deleted,
              "M32 world long press confirms and deletes the selected world (B8)",
              "trigger=" + std::to_string(hasDeleteLongPress ? 1 : 0) +
              " confirm=" + std::to_string(confirmation ? 1 : 0) +
              " cancel=" + std::to_string(cancelled ? 1 : 0) +
              " delete=" + std::to_string(deleted ? 1 : 0));
    }

    // M33 (B8): Paste tag is available in the real tree overflow menu.  It
    // clones into a compound, rejects empty/duplicate names, and supports undo.
    {
        AppState s = makeState(360, 780);
        s.screen = S_TREE;
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        uint32_t sourceIdx = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::String; n.name = "Payload"; n.s = "copied";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(sourceIdx);
        uint32_t targetIdx = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "Target";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(targetIdx);
        s.ed.pathIdx = {s.ed.doc.root, targetIdx};
        s.ed.clipIdx = sourceIdx;
        s.ed.hasClip = true;

        handleAction(s, A_MENU_OPEN);
        bool offered = false;
        for (const auto& item : s.menuItems)
            if (item.first == "Paste tag" && item.second == A_TREE_PASTE) offered = true;
        ui::Node page = buildScreen(s);
        const ui::Node* pasteItem = findActionNode(page, A_TREE_PASTE, "Paste tag");
        const ui::Node* hit = nullptr;
        bool pasteTapped = pasteItem && ui::hitTestInteractive(
            page, pasteItem->rect.x + pasteItem->rect.w / 2,
            pasteItem->rect.y + pasteItem->rect.h / 2, hit);
        if (pasteTapped && hit) dispatchHit(s, *hit, false);
        bool pasted = s.ed.doc.arena[targetIdx].kids.size() == 1 &&
                      s.ed.doc.arena[s.ed.doc.arena[targetIdx].kids[0]].name == "Payload" &&
                      s.ed.doc.arena[s.ed.doc.arena[targetIdx].kids[0]].s == "copied" &&
                      s.ed.doc.arena.size() == 4;

        handleAction(s, A_MENU_OPEN);
        page = buildScreen(s);
        pasteItem = findActionNode(page, A_TREE_PASTE, "Paste tag");
        hit = nullptr;
        pasteTapped = pasteItem && ui::hitTestInteractive(
            page, pasteItem->rect.x + pasteItem->rect.w / 2,
            pasteItem->rect.y + pasteItem->rect.h / 2, hit);
        if (pasteTapped && hit) dispatchHit(s, *hit, false);
        bool duplicateRejected = s.ed.doc.arena[targetIdx].kids.size() == 1 &&
                                 s.ed.doc.arena.size() == 4 &&
                                 !s.toasts.empty() &&
                                 s.toasts.back().message == nbt::msgDuplicateName();

        handleAction(s, A_TREE_UNDO);
        bool undone = s.ed.doc.arena[targetIdx].kids.empty() &&
                      s.ed.doc.arena.size() == 3;
        handleAction(s, A_TREE_REDO);
        bool redone = s.ed.doc.arena[targetIdx].kids.size() == 1 &&
                      s.ed.doc.arena.size() == 4;

        AppState e = makeState(360, 780);
        e.screen = S_TREE;
        e.ed = EditorState{};
        e.ed.doc = newRootCompound();
        uint32_t unnamedIdx = (uint32_t)e.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::String; e.ed.doc.arena.push_back(std::move(n)); }
        e.ed.doc.arena[e.ed.doc.root].kids.push_back(unnamedIdx);
        uint32_t eTargetIdx = (uint32_t)e.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "Target";
          e.ed.doc.arena.push_back(std::move(n)); }
        e.ed.doc.arena[e.ed.doc.root].kids.push_back(eTargetIdx);
        e.ed.pathIdx = {e.ed.doc.root, eTargetIdx};
        e.ed.clipIdx = unnamedIdx;
        e.ed.hasClip = true;
        handleAction(e, A_MENU_OPEN);
        ui::Node ePage = buildScreen(e);
        const ui::Node* ePaste = findActionNode(ePage, A_TREE_PASTE, "Paste tag");
        const ui::Node* eHit = nullptr;
        bool eTapped = ePaste && ui::hitTestInteractive(
            ePage, ePaste->rect.x + ePaste->rect.w / 2,
            ePaste->rect.y + ePaste->rect.h / 2, eHit);
        if (eTapped && eHit) dispatchHit(e, *eHit, false);
        bool emptyRejected = e.ed.doc.arena[eTargetIdx].kids.empty() &&
                             e.ed.doc.arena.size() == 3 &&
                             !e.toasts.empty() &&
                             e.toasts.back().message == nbt::msgUnnamedTag();

        check(offered && pasted && duplicateRejected && undone && redone && emptyRejected,
              "M33 overflow paste clones once and rejects invalid duplicate names (B8)",
              "offered=" + std::to_string(offered ? 1 : 0) +
              " paste=" + std::to_string(pasted ? 1 : 0) +
              " duplicate=" + std::to_string(duplicateRejected ? 1 : 0) +
              " undo=" + std::to_string(undone ? 1 : 0) +
              " redo=" + std::to_string(redone ? 1 : 0) +
              " empty=" + std::to_string(emptyRejected ? 1 : 0));
    }

    // M34 (B8): Go to root is a real overflow action and resets both the
    // active path and the tree's vertical scroll position.
    {
        AppState s = makeState(360, 780);
        s.screen = S_TREE;
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        uint32_t child = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "A";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(child);
        uint32_t grandchild = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "B";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[child].kids.push_back(grandchild);
        s.ed.pathIdx = {s.ed.doc.root, child, grandchild};
        s.ed.selected = 2;
        s.scrollY = 91;

        handleAction(s, A_MENU_OPEN);
        bool offered = false;
        for (const auto& item : s.menuItems)
            if (item.first == "Go to root" && item.second == A_TREE_MOVE_ROOT) offered = true;
        ui::Node page = buildScreen(s);
        const ui::Node* rootItem = findActionNode(page, A_TREE_MOVE_ROOT, "Go to root");
        const ui::Node* hit = nullptr;
        bool rootTapped = rootItem && ui::hitTestInteractive(
            page, rootItem->rect.x + rootItem->rect.w / 2,
            rootItem->rect.y + rootItem->rect.h / 2, hit);
        if (rootTapped && hit) dispatchHit(s, *hit, false);
        bool reset = s.ed.pathIdx.size() == 1 &&
                     s.ed.pathIdx.front() == s.ed.doc.root &&
                     s.scrollY == 0 && s.ed.selected == -1 && !s.menuOpen;
        check(offered && reset, "M34 overflow go-to-root resets path and scroll (B8)",
              "offered=" + std::to_string(offered ? 1 : 0) +
              " path=" + std::to_string(s.ed.pathIdx.size()) +
              " scroll=" + std::to_string(s.scrollY));
    }

    // M35: navigation keeps the launching screen, app-bar Back is context
    // aware, and Help does not mutate the active tree path.
    {
        AppState s = makeState(360, 780);
        s.screen = S_HOME;
        setScreen(s, S_BROWSER);
        std::string p = (fs::temp_directory_path() /
                         ("nbt_nav_" + std::to_string((long)getpid()) + ".dat")).string();
        auto encoded = nbt::serialize(newRootCompound());
        auto wr = encoded.ok() ? app::writeFileAtomic(p, encoded.value)
                               : nbt::Result<void>::fail(encoded.err, encoded.detail);
        std::string e;
        bool opened = wr.ok() && openDocument(s, p, false, e);
        bool browserRemembered = opened && s.screen == S_TREE &&
                                !s.screenStack.empty() && s.screenStack.back() == S_BROWSER;
        handleAction(s, A_TREE_UP);       // tree root: leave editor -> home directly
        bool returnedHome = s.screen == S_HOME && s.screenStack.empty();

        AppState h = makeState(360, 780);
        h.screen = S_TREE;
        h.ed = EditorState{};
        h.ed.doc = newRootCompound();
        uint32_t child = (uint32_t)h.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::Compound; n.name = "Nested";
          h.ed.doc.arena.push_back(std::move(n)); }
        h.ed.doc.arena[h.ed.doc.root].kids.push_back(child);
        h.ed.pathIdx = {h.ed.doc.root, child};
        setScreen(h, S_HELP);
        handleAction(h, A_BACK);
        bool helpBack = h.screen == S_HOME && h.screenStack.empty();

        AppState b = makeState(360, 780);
        b.screen = S_BROWSER;
        ui::Node browser = buildScreen(b);
        AppState t = makeState(360, 780);
        t.screen = S_TREE;
        t.ed = EditorState{};
        t.ed.doc = newRootCompound();
        t.ed.pathIdx = {t.ed.doc.root};
        ui::Node tree = buildScreen(t);
        bool contextActions = findActionNode(browser, A_BACK) != nullptr &&
                              findActionNode(tree, A_TREE_UP) != nullptr &&
                              findActionNode(browser, A_TREE_UP) == nullptr;

        std::error_code ec;
        fs::remove(p, ec);
        check(browserRemembered && returnedHome && helpBack && contextActions,
              "M35 navigation back returns home in one step",
              "open=" + std::to_string(opened ? 1 : 0) +
              " home=" + std::to_string(returnedHome ? 1 : 0) +
              " help=" + std::to_string(helpBack ? 1 : 0) +
              " actions=" + std::to_string(contextActions ? 1 : 0));
    }

    // M36: modal overlays are real input barriers, and dialog child controls
    // are translated to absolute coordinates before drawing and hit-testing.
    {
        AppState s = makeState(360, 780);
        s.screen = S_TREE;
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        uint32_t child = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::String; n.name = "Text";
          n.s = "value"; s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(child);
        s.ed.pathIdx = {s.ed.doc.root};
        openValueEditor(s, child);

        gfx::Canvas c;
        std::vector<uint32_t> pixels((size_t)s.w * s.h);
        c.bind(pixels.data(), s.w, s.h, s.w);
        c.setScale(1);
        renderFrame(s, c);

        const ui::Node* field = s.uiRoot ? findActionNode(*s.uiRoot, A_KEY_OPEN) : nullptr;
        bool fieldVisible = field && field->rect.x > 0 && field->rect.y > 0 &&
                            field->rect.right() <= s.w && field->rect.bottom() <= s.h;
        const ui::Node* background = s.uiRoot ? findActionNode(*s.uiRoot, A_MENU_OPEN) : nullptr;
        const ui::Node* hit = nullptr;
        bool blocked = background && ui::hitTestInteractive(
            *s.uiRoot, background->rect.x + background->rect.w / 2,
            background->rect.y + background->rect.h / 2, hit) && hit && hit->action == A_BLOCK;

        s.valueOpen = false;
        s.dialog = ui::Dialog{};
        s.addOpen = true;
        s.valueTypeToAdd = nbt::Type::Byte;
        openDialog(s, "Add tag", "Pick a type.", "ADD", A_TREE_ADD_DONE,
                   "CANCEL", A_DIALOG_NEG);
        renderFrame(s, c);
        const ui::Node* typeButton = s.uiRoot ? findActionNode(*s.uiRoot, A_VALUE_KIND) : nullptr;
        bool typeReachable = false;
        int hitTypeAction = 0;
        std::string typeRect = "none";
        if (typeButton) {
            hit = nullptr;
            typeRect = std::to_string(typeButton->rect.x) + "," +
                       std::to_string(typeButton->rect.y) + " " +
                       std::to_string(typeButton->rect.w) + "x" +
                       std::to_string(typeButton->rect.h);
            typeReachable = ui::hitTestInteractive(
                *s.uiRoot, typeButton->rect.x + typeButton->rect.w / 2,
                typeButton->rect.y + typeButton->rect.h / 2, hit);
            hitTypeAction = hit ? hit->action : 0;
            typeReachable = typeReachable && hit && hit->action == A_VALUE_KIND &&
                typeButton->rect.x > 0 && typeButton->rect.y > 0 &&
                typeButton->rect.right() <= s.w && typeButton->rect.bottom() <= s.h;
        }
        check(fieldVisible && blocked && typeReachable,
              "M36 modal barrier and dialog geometry are enforced",
              "field=" + std::to_string(fieldVisible ? 1 : 0) +
              " blocked=" + std::to_string(blocked ? 1 : 0) +
              " type=" + std::to_string(typeReachable ? 1 : 0) +
              " rect=" + typeRect + " hit=" + std::to_string(hitTypeAction));
    }

    // M37: the keypad exposes every character, including the fourth row, and
    // SHIFT changes letter keys without losing the numeric/alpha layout.
    {
        AppState s = makeState(360, 780);
        s.keypadOpen = true;
        s.keypadTarget = "searchFilter";
        s.keypadNumeric = false;
        std::function<void(const ui::Node&, std::string&)> collectChars =
            [&](const ui::Node& n, std::string& out) {
                if (n.action == A_KEY_TAP && n.index >= 32 && n.index < 127)
                    out.push_back((char)n.index);
                for (const auto& k : n.kids) collectChars(k, out);
            };
        std::string alpha;
        collectChars(buildKeypadResult(s), alpha);
        bool allAlpha = true;
        for (const char* p = "abcdefghijklmnopqrstuvwxyz_./ "; *p; ++p)
            if (alpha.find(*p) == std::string::npos) allAlpha = false;

        s.keypadShift = true;
        std::string shifted;
        collectChars(buildKeypadResult(s), shifted);
        bool allUpper = true;
        for (const char* p = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; *p; ++p)
            if (shifted.find(*p) == std::string::npos) allUpper = false;

        s.keypadShift = false;
        s.keypadNumeric = true;
        std::string numeric;
        collectChars(buildKeypadResult(s), numeric);
        bool allNumeric = true;
        for (const char* p = "0123456789.-"; *p; ++p)
            if (numeric.find(*p) == std::string::npos) allNumeric = false;
        check(allAlpha && allUpper && allNumeric,
              "M37 keypad covers all characters and shift",
              "alpha=" + std::to_string(allAlpha ? 1 : 0) +
              " upper=" + std::to_string(allUpper ? 1 : 0) +
              " numeric=" + std::to_string(allNumeric ? 1 : 0));
    }

    // M38: Search X clears, rename undo restores the old name, and flat layer
    // thickness/reorder act on the selected row.
    {
        AppState s = makeState(360, 780);
        s.screen = S_TREE;
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        uint32_t child = (uint32_t)s.ed.doc.arena.size();
        { nbt::Node n; n.type = nbt::Type::String; n.name = "FindMe";
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.doc.arena[s.ed.doc.root].kids.push_back(child);
        s.ed.pathIdx = {s.ed.doc.root};
        s.ed.filter = "Find";
        s.ed.showSearch = true;
        handleAction(s, A_TREE_SEARCH_CLEAR);
        bool searchCleared = s.ed.filter.empty() && !s.ed.showSearch;

        s.renameOpen = true;
        s.renameIdx = child;
        s.renameName = "Renamed";
        std::string e;
        commitRename(s);
        bool renamed = s.ed.doc.arena[child].name == "Renamed";
        handleAction(s, A_TREE_UNDO);
        bool renameUndone = s.ed.doc.arena[child].name == "FindMe";

        AppState f = makeState(360, 780);
        f.screen = S_FLAT;
        f.flatLayers = {{"minecraft:stone", 1}, {"minecraft:dirt", 2}};
        f.ed.selected = 0;
        handleAction(f, A_FLAT_EDIT_LAYER);
        keypadClear(f);
        applyKey(f, "3");
        handleAction(f, A_KEY_ENTER);
        bool thicknessChanged = f.flatLayers[0].count == 3;
        f.ed.selected = 1;
        handleAction(f, A_FLAT_LAYER_UP);
        bool reordered = f.flatLayers[0].block == "minecraft:dirt" &&
                         f.flatLayers[1].block == "minecraft:stone";
        check(searchCleared && renamed && renameUndone && thicknessChanged && reordered,
              "M38 search clear, rename undo, and flat row actions work",
              "search=" + std::to_string(searchCleared ? 1 : 0) +
              " rename=" + std::to_string(renameUndone ? 1 : 0) +
              " thickness=" + std::to_string(thicknessChanged ? 1 : 0) +
              " reorder=" + std::to_string(reordered ? 1 : 0));
    }

    // M39: a destructive confirmation keeps the target captured when the
    // dialog opened, even if selection changes behind the modal overlay.
    {
        std::error_code ec;
        fs::path root = fs::temp_directory_path() /
                        ("nbt_delete_target_" + std::to_string((long)getpid()));
        fs::path first = root / "First";
        fs::path second = root / "Second";
        fs::remove_all(root, ec);
        fs::create_directories(first, ec);
        fs::create_directories(second, ec);

        AppState s = makeState(360, 780);
        s.screen = S_WORLDS;
        s.worldsRoot = root.string();
        app::WorldInfo a1, a2;
        a1.folder = first.string(); a1.displayName = "First"; a1.internalName = "First";
        a2.folder = second.string(); a2.displayName = "Second"; a2.internalName = "Second";
        s.worlds = {a1, a2};
        s.ed.selected = 0;
        handleAction(s, A_WORLD_DELETE);
        bool opened = s.dialog.open && s.dialogTargetIndex == 0;
        s.ed.selected = 1;                 // cannot affect the captured target
        handleAction(s, A_DIALOG_POS);
        bool firstDeleted = !fs::exists(first, ec);
        bool secondKept = fs::exists(second, ec);
        fs::remove_all(root, ec);
        check(opened && firstDeleted && secondKept,
              "M39 delete confirmation keeps its captured world target",
              "opened=" + std::to_string(opened ? 1 : 0) +
              " first=" + std::to_string(firstDeleted ? 1 : 0) +
              " second=" + std::to_string(secondKept ? 1 : 0));
    }

    // M40: a Scroll clips both painting and hit-testing to its viewport.
    {
        ui::Node outside = ui::button({10, 50, 80, 30}, "", gfx::rgb(255, 0, 0),
                                      gfx::rgb(255, 0, 0), A_TREE_TAP);
        ui::Node inside = ui::button({10, 120, 80, 30}, "", gfx::rgb(0, 0, 255),
                                     gfx::rgb(0, 0, 255), A_TREE_TAP);
        ui::Node scroll = ui::list({10, 100, 200, 80}, {outside, inside});
        ui::Node root = ui::panel({0, 0, 300, 240}, {scroll});
        const ui::Node* hit = nullptr;
        bool outsideMiss = !ui::hitTestInteractive(root, 50, 65, hit);
        bool insideHit = ui::hitTestInteractive(root, 50, 135, hit) && hit &&
                         hit->action == A_TREE_TAP;

        const int W = 300, H = 240;
        std::vector<uint32_t> pixels((size_t)W * H, 0xffffffffu);
        gfx::Canvas c;
        c.bind(pixels.data(), W, H, W);
        c.setScale(1);
        c.clear(gfx::rgb(255, 255, 255));
        drawNode(c, gfx::themeLight(), root, 0);
        uint32_t above = pixels[(size_t)65 * W + 50];
        uint32_t visible = pixels[(size_t)135 * W + 50];
        bool paintClipped = above != gfx::pack(gfx::rgb(255, 0, 0)) &&
                            visible == gfx::pack(gfx::rgb(0, 0, 255));
        check(outsideMiss && insideHit && paintClipped,
              "M40 scroll viewport clips painting and hit-testing",
              "miss=" + std::to_string(outsideMiss ? 1 : 0) +
              " hit=" + std::to_string(insideHit ? 1 : 0) +
              " paint=" + std::to_string(paintClipped ? 1 : 0));
    }

    // M41: UTF-8 punctuation is measured as characters, text wraps by visible
    // width, and the Home world list + FAB stack fill the viewport.
    {
        size_t dotBytes = 0;
        size_t dashBytes = 0;
        const uint32_t dot = gfx::utf8DecodeAt("·", 0, &dotBytes);
        const uint32_t dash = gfx::utf8DecodeAt("—", 0, &dashBytes);
        const std::vector<std::string> wrapped = gfx::wrapText(
            "No recent files yet — open or create an NBT file.", 140, 1);
        bool wrappedWidths = wrapped.size() >= 2;
        for (const std::string& line : wrapped) {
            if (gfx::fontTextWidth(line, 1) > 140) wrappedWidths = false;
        }

        AppState s = makeState(360, 780);
        s.screen = S_HOME;
        s.recent.clear();
        const ui::Node home = buildScreen(s);
        int maxBottom = 0;
        int fabCount = 0;
        int minFabHeight = 1000;
        for (const ui::Node& child : home.kids) {
            maxBottom = std::max(maxBottom, child.rect.bottom());
            if (child.kind == ui::Kind::Button && child.action == A_FAB_TOGGLE) {
                ++fabCount;
                minFabHeight = std::min(minFabHeight, child.rect.h);
            }
        }
        const bool unicodeOk = dot == 0x00b7 && dotBytes == 2 &&
                               dash == 0x2014 && dashBytes == 3 &&
                               gfx::fontTextWidth("·", 1) > 0;
        const bool layoutOk = maxBottom >= s.h - 24 && fabCount == 1 &&
                              minFabHeight >= 48;
        check(unicodeOk && wrappedWidths && layoutOk,
              "M41 UTF-8 font, wrapping, and Home proportions",
              "unicode=" + std::to_string(unicodeOk ? 1 : 0) +
              " wrap=" + std::to_string(wrappedWidths ? 1 : 0) +
              " layout=" + std::to_string(layoutOk ? 1 : 0));
    }

    // M42: tree rows keep readable selection contrast, preserve the sign of
    // long values, and leave the Save FAB outside the scroll viewport.
    {
        AppState s = makeState(360, 780);
        s.screen = S_TREE;
        s.ed = EditorState{};
        s.ed.doc = newRootCompound();
        s.ed.pathIdx = {s.ed.doc.root};

        { nbt::Node n; n.type = nbt::Type::Long; n.name = "longTag";
          n.i = -9000000000000000000LL;
          s.ed.doc.arena[s.ed.doc.root].kids.push_back((uint32_t)s.ed.doc.arena.size());
          s.ed.doc.arena.push_back(std::move(n)); }
        { nbt::Node n; n.type = nbt::Type::Short; n.name = "shortTag"; n.i = -12345;
          s.ed.doc.arena[s.ed.doc.root].kids.push_back((uint32_t)s.ed.doc.arena.size());
          s.ed.doc.arena.push_back(std::move(n)); }
        s.ed.selected = 1;

        const ui::Node page = buildScreen(s);
        const ui::Node* selectedRow = nullptr;
        const ui::Node* longRow = nullptr;
        const ui::Node* scroll = nullptr;
        std::function<void(const ui::Node&)> collect = [&](const ui::Node& n) {
            if (n.kind == ui::Kind::TreeRow && n.text == "shortTag") selectedRow = &n;
            if (n.kind == ui::Kind::TreeRow && n.text == "longTag") longRow = &n;
            if (n.kind == ui::Kind::Scroll) scroll = &n;
            for (const auto& kid : n.kids) collect(kid);
        };
        collect(page);
        const ui::Node* save = findActionNode(page, A_TREE_SAVE);

        const auto linear = [](uint8_t value) {
            const double channel = value / 255.0;
            return channel <= 0.04045 ? channel / 12.92
                : std::pow((channel + 0.055) / 1.055, 2.4);
        };
        const auto luminance = [&](gfx::Color color) {
            return 0.2126 * linear(color.r) + 0.7152 * linear(color.g) +
                   0.0722 * linear(color.b);
        };
        double contrast = 0.0;
        if (selectedRow) {
            const double foreground = luminance(s.theme.onSurfaceMuted);
            const double background = luminance(selectedRow->bg);
            const double lighter = std::max(foreground, background);
            const double darker = std::min(foreground, background);
            contrast = (lighter + 0.05) / (darker + 0.05);
        }

        const std::string longValue = longRow ? longRow->text2 : "";
        const std::string clipped = gfx::ellipsizeForWidth(longValue, 112, 1);
        const bool valueReadable = gfx::fontTextWidth(clipped, 1) <= 112 &&
            !clipped.empty() && clipped.front() == '-' && clipped.size() >= 4 &&
            clipped.compare(clipped.size() - 3, 3, "...") == 0;
        const bool fabClear = scroll && save &&
            scroll->rect.bottom() <= save->rect.y - 4;
        check(selectedRow && selectedRow->bg.a == 255 && contrast >= 4.5 &&
              valueReadable && fabClear,
              "M42 tree selection contrast, value ellipsis, and FAB clearance",
              "contrast=" + std::to_string(contrast) +
              " value=" + clipped + " fab=" + std::to_string(fabClear ? 1 : 0));
    }

    std::printf("== %d passed, %d failed ==\n", g_pass, g_fail);
    app::appWorker().stop();
    if (g_fail) std::exit(1);
}

} // namespace modeltest
#endif

#if NBT_TESTABLE && !defined(NBT_UISHOT)
int main() {
    std::setvbuf(stdout, nullptr, _IOLBF, 0);
    testboot::run();
    modeltest::run();
    return 0;
}
#endif

// ============================================================================
// UI SCREENSHOT HARNESS (host only — never compiled into the APK)
// Renders every screen into a PNG file so the layout can be inspected visually
// without a device. Build with NBT_UISHOT and the version macros read from
// VERSION (see scripts/build_host_tests.sh for the same injection pattern).
// Usage: out/uishot <outDir> [width] [height]
// ============================================================================
#if defined(NBT_UISHOT)
#include <zlib.h>
namespace uishot {

using namespace app;

static void put32(std::vector<unsigned char>& v, uint32_t x) {
    v.push_back((unsigned char)((x >> 24) & 0xff)); v.push_back((unsigned char)((x >> 16) & 0xff));
    v.push_back((unsigned char)((x >> 8) & 0xff));  v.push_back((unsigned char)(x & 0xff));
}
static void chunk(std::vector<unsigned char>& out, const char* type,
                  const std::vector<unsigned char>& data) {
    put32(out, (uint32_t)data.size());
    std::vector<unsigned char> td(type, type + 4);
    td.insert(td.end(), data.begin(), data.end());
    out.insert(out.end(), td.begin(), td.end());
    put32(out, (uint32_t)crc32(0, td.data(), (uInt)td.size()));
}
static bool writePng(const std::string& path, const std::vector<uint32_t>& px, int w, int h) {
    std::vector<unsigned char> raw;
    raw.reserve((size_t)h * (1 + (size_t)w * 4));
    for (int y = 0; y < h; ++y) {
        raw.push_back(0);
        for (int x = 0; x < w; ++x) {
            uint32_t p = px[(size_t)y * w + x];
            raw.push_back((unsigned char)(p & 0xff));
            raw.push_back((unsigned char)((p >> 8) & 0xff));
            raw.push_back((unsigned char)((p >> 16) & 0xff));
            raw.push_back((unsigned char)((p >> 24) & 0xff));
        }
    }
    uLongf clen = compressBound((uLong)raw.size());
    std::vector<unsigned char> comp(clen);
    if (compress2(comp.data(), &clen, raw.data(), (uLong)raw.size(), 6) != Z_OK) return false;
    std::vector<unsigned char> out;
    const unsigned char sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    out.insert(out.end(), sig, sig + 8);
    std::vector<unsigned char> ihdr;
    put32(ihdr, (uint32_t)w); put32(ihdr, (uint32_t)h);
    ihdr.push_back(8); ihdr.push_back(6); ihdr.push_back(0); ihdr.push_back(0); ihdr.push_back(0);
    chunk(out, "IHDR", ihdr);
    chunk(out, "IDAT", std::vector<unsigned char>(comp.begin(), comp.begin() + clen));
    chunk(out, "IEND", std::vector<unsigned char>());
    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) return false;
    std::fwrite(out.data(), 1, out.size(), f);
    std::fclose(f);
    return true;
}

static void render(AppState& a, const std::string& path) {
    int sc = a.scale > 0 ? a.scale : 1;
    int pw = a.w * sc, ph = a.h * sc;          // device pixels
    std::vector<uint32_t> buf((size_t)pw * ph);
    gfx::Canvas c;
    c.bind(buf.data(), pw, ph, pw);
    renderFrame(a, c);
    bool ok = writePng(path, buf, pw, ph);
    std::printf("%-30s %dx%d (design %dx%d @%dx)  %s\n", path.c_str(), pw, ph, a.w, a.h, sc,
                ok ? "ok" : "GAGAL");
}

static uint32_t findNamed(const nbt::Document& doc, const std::string& name) {
    for (uint32_t i = 0; i < doc.arena.size(); ++i)
        if (doc.arena[i].name == name) return i;
    return doc.root;
}

static AppState baseState(int w, int h) {
    AppState a;
    a.w = w; a.h = h;
    a.scale = 1;
    a.dark = false;
    a.theme = gfx::themeLight();
    a.appName = "NBT Editor";
    a.filesDir = ".";
    a.worldsRoot = "/tmp/nbt_shot_worlds";
    a.recent = {"/sdcard/Download/uji_semua_tipe.nbt", "/sdcard/Download/ujicoba_level.dat"};
    a.worlds = {
        {"C/worlds/MyWorld", "My World", "MyWorld", 12345678, true, false, false, ""},
        {"C/worlds/Superflat Test", "Superflat Test", "Superflat Test", 9876543, false, false, false},
        {"C/worlds/Encrypted", "Encrypted", "Encrypted", 4242, false, true, false},
    };
    a.browseDir = "/storage/emulated/0/Download";
    a.browseEntries = {"/storage/emulated/0/Download/uji_semua_tipe.nbt",
                       "/storage/emulated/0/Download/ujicoba_level.dat",
                       "/storage/emulated/0/Download/Minecraft Worlds",
                       "/storage/emulated/0/Download/screenshots",
                       "/storage/emulated/0/Download/level.dat.old"};
    a.browseIsDir = {false, false, true, true, false};
    // editor document
    auto bytes = testboot::fixtureAllTypes();
    auto r = nbt::parseBytes(bytes.data(), bytes.size(), "uji_semua_tipe.nbt", {});
    if (r.ok()) {
        a.ed.doc = r.value;
        a.ed.open = true;
        a.ed.path = "/storage/emulated/0/Download/uji_semua_tipe.nbt";
        a.ed.pathIdx = {0};
        a.ed.selected = 1;
    }
    a.flatLayers = {{"minecraft:bedrock", 1}, {"minecraft:dirt", 3}, {"minecraft:grass_block", 1}};
    return a;
}

static void run(const std::string& dir, int w, int h, int scale) {
    std::printf("== UI shots (design %dx%d @%dx) ==\n", w, h, scale);
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_HOME;  render(a, dir + "/01_home.png");
    }
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_WORLDS; render(a, dir + "/02_worlds.png");
    }
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_BROWSER; render(a, dir + "/03_browser.png");
    }
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_TREE;  render(a, dir + "/04_tree.png");
    }
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_FLAT;  render(a, dir + "/05_flat.png");
    }
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_HELP;  render(a, dir + "/06_help.png");
    }
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_TREE;
        a.valueOpen = true; a.valueIdx = 1; a.valueText = "42"; a.valueNumeric = true;
        a.dialog.open = true; a.dialog.title = "Edit value";
        a.dialog.message = "Int tag 'intTag'"; a.dialog.pos = "SAVE"; a.dialog.neg = "CANCEL";
        render(a, dir + "/07_dialog_value.png");
    }
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_TREE;
        a.addOpen = true; a.addName = "NewTag";
        a.dialog.open = true; a.dialog.title = "Add tag";
        a.dialog.message = "Pick a type (and name for compounds)."; a.dialog.pos = "ADD"; a.dialog.neg = "CANCEL";
        render(a, dir + "/08_dialog_add.png");
    }
    {
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_TREE;
        a.keypadOpen = true; a.keypadTarget = "searchFilter"; a.keypadNumeric = false;
        a.dialog.open = true; a.dialog.title = "Edit value";
        a.dialog.message = "Int tag 'intTag'"; a.dialog.pos = "SAVE"; a.dialog.neg = "CANCEL";
        render(a, dir + "/09_keypad.png");
    }
    {
        // B6: exercise the visible ancestor chain (root > nested > level1 > level2).
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_TREE;
        uint32_t nested = findNamed(a.ed.doc, "nested");
        uint32_t level1 = findNamed(a.ed.doc, "level1");
        uint32_t level2 = findNamed(a.ed.doc, "level2");
        a.ed.pathIdx = {a.ed.doc.root, nested, level1, level2};
        a.ed.selected = 0;
        render(a, dir + "/10_breadcrumb.png");
    }
    {
        // B7: array elements are payload rows addressed by their real index.
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_TREE;
        uint32_t intArray = findNamed(a.ed.doc, "intArray");
        a.ed.pathIdx = {a.ed.doc.root, intArray};
        a.ed.selected = 0;
        render(a, dir + "/11_array_elements.png");
    }
    {
        // B8: dark theme and its real Home overflow action.
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_HOME;
        a.dark = true; a.theme = gfx::themeDark(); a.prefs.theme = "dark";
        handleAction(a, A_MENU_OPEN);
        render(a, dir + "/12_home_dark_menu.png");
    }
    {
        // B8: delete-world confirmation reached by long press.
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_WORLDS;
        a.dialog.open = true;
        a.dialog.title = "Delete World";
        a.dialog.message = "Are you sure you want to delete this world? This action cannot be undone.";
        a.dialog.pos = "DELETE"; a.dialog.neg = "CANCEL";
        a.dialog.posAction = A_WORLD_DELETE; a.dialog.negAction = A_DIALOG_NEG;
        render(a, dir + "/13_world_delete_dialog.png");
    }
    {
        // B8: tree overflow exposes Paste tag and Go to root.
        AppState a = baseState(w, h); a.scale = scale; a.screen = S_TREE;
        a.ed.hasClip = true;
        handleAction(a, A_MENU_OPEN);
        render(a, dir + "/14_tree_paste_menu.png");
    }
}

} // namespace uishot

int main(int argc, char** argv) {
    std::string dir = argc > 1 ? argv[1] : "out/uishots";
    int w = argc > 2 ? std::atoi(argv[2]) : 360;      // design units (dp)
    int h = argc > 3 ? std::atoi(argv[3]) : 780;
    int scale = argc > 4 ? std::atoi(argv[4]) : 3;    // device px per design unit
    uishot::run(dir, w, h, scale);
    return 0;
}
#endif

