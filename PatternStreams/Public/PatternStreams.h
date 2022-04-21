#pragma once

#include <string>
#include <vector>

typedef uint8_t Byte;

typedef uint8_t* BytePtr;

typedef std::vector<Byte> ByteBuffer;

class PatternByte
{
public:
    PatternByte(Byte value);

    Byte Value;
    bool IsWildcard;

    static PatternByte Any();
};

constexpr PatternByte Any = PatternByte::Any();

class Pattern : public std::vector<PatternByte>
{
public:
    Pattern(std::initializer_list<PatternByte> l);

    virtual bool IsMatch(BytePtr& ptr);
};

struct OffsetInterval
{
    int64_t Offset;
    size_t Length;
};

class PatternInRange : public Pattern
{
public:
    PatternInRange(std::initializer_list<PatternByte> l, OffsetInterval range = {0x00, 0x40},  bool replaceExisting = false);

    bool IsMatch(BytePtr& ptr) override;

private:
    OffsetInterval Range;
    bool ReplaceExisting;
};

class PatternStream : public std::vector<BytePtr>
{
public:
    PatternStream(Pattern pattern, const std::string& module = "");

    PatternStream operator&(Pattern pattern) const;

    PatternStream operator|(const PatternStream& other) const;

    BytePtr FirstOrNullptr() const;

private:
    PatternStream() = default;
};
