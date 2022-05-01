#pragma once

#include <functional>
#include <string>
#include <vector>

namespace PS {
    typedef uint8_t Byte, *BytePtr;

    typedef std::vector<Byte> ByteBuffer;

    typedef std::function<void(BytePtr& ptr)> BytePtrFunc;

    typedef std::function<void(const BytePtr& ptr)> ConstBytePtrFunc;

    class PatternByte {
    public:
        PatternByte(const Byte value) : value(value) {}
        
        Byte value = 0x00;
        bool isWildcard = false;
    };

    inline constexpr PatternByte Any = { .isWildcard = true };

    class Pattern : public std::vector<PatternByte> {
    public:
        Pattern(std::initializer_list<PatternByte> l) : std::vector(l) {}

        bool IsMatch(const BytePtr& ptr) const;
    };

    class PatternStream : public std::vector<BytePtr> {
    public:
        PatternStream(const Pattern& pattern, const std::string& module = "");

        PatternStream HasPatternInRange(const Pattern& pattern, int64_t rangeOffset, size_t rangeLength,
                                        bool replaceExistingMatch = false) const;

        PatternStream AddOffset(int64_t offset) const;

        PatternStream ForEach(const BytePtrFunc& func) const;

        PatternStream ForEach(const ConstBytePtrFunc& func) const;

        BytePtr FirstOrNullPtr() const;

    private:
        PatternStream() = default;
    };
}
