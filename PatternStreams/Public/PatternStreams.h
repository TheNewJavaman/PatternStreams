#pragma once

#include <functional>
#include <string>
#include <vector>

namespace PS {
    typedef uint8_t Byte, *BytePtr;

    typedef std::vector<Byte> ByteBuffer;

    typedef std::function<void(BytePtr&)> BytePtrFunc;
    
    class PatternByte {
    public:
        PatternByte(Byte value);

        Byte Value;
        bool IsWildcard;

        static PatternByte Any();
    };

    static inline PatternByte Any = PatternByte::Any();

    class Pattern : public std::vector<PatternByte> {
    public:
        Pattern(std::initializer_list<PatternByte> l);

        virtual bool IsMatch(BytePtr& ptr) const;
    };

    struct OffsetInterval {
        int64_t Offset;
        size_t Length;
    };

    class PatternInRange : public Pattern {
    public:
        PatternInRange(const Pattern& pattern, OffsetInterval range = { 0x00, 0x40 }, bool replaceExisting = false);

        bool IsMatch(BytePtr& ptr) const override;

    private:
        OffsetInterval Range;
        bool ReplaceExisting;
    };

    struct AddOffset {
        int64_t Offset;
    };

    class WriteBuffer : public ByteBuffer {
    public:
        WriteBuffer(std::initializer_list<Byte> l);
    };
    
    class WriteBufferAndOffset : public WriteBuffer {
    public:
        WriteBufferAndOffset(std::initializer_list<Byte> l);
    };
    
    class ForEach : public BytePtrFunc {};

    class PatternStream : public std::vector<BytePtr> {
    public:
        PatternStream(std::initializer_list<PatternByte> l);

        PatternStream(const Pattern& pattern, const std::string& module = "");

        PatternStream operator|(const Pattern& pattern) const;

        PatternStream operator|(const AddOffset& offset) const;

        PatternStream operator|(const WriteBuffer& buffer) const;
        
        PatternStream operator|(const WriteBufferAndOffset& buffer) const;

        PatternStream operator|(const ForEach& func) const;

        BytePtr FirstOrNullptr() const;

    private:
        PatternStream() = default;

        PatternStream(size_t capacity);
    };

    namespace PatternPatcher {
        bool WriteBuffer(BytePtr ptr, const ByteBuffer& buffer);

        bool WriteBufferAndOffset(BytePtr& ptr, const ByteBuffer& buffer);
    }}
