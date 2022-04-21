#include "PatternStreams.h"

#include "PatternStreamsPrivate.h"

PatternByte::PatternByte(const Byte value) : Value(value), IsWildcard(false)
{
}

PatternByte PatternByte::Any()
{
    PatternByte pb(0x00);
    pb.IsWildcard = true;
    return pb;
}

Pattern::Pattern(const std::initializer_list<PatternByte> l): std::vector<PatternByte>(l)
{
}

bool Pattern::IsMatch(BytePtr& ptr)
{
    for (size_t i = 0; i < this->size(); i++)
    {
        if (!this->at(i).IsWildcard && this->at(i).Value != *(ptr + i))
        {
            return false;
        }
    }
    return true;
}

PatternInRange::PatternInRange(const std::initializer_list<PatternByte> l, const OffsetInterval range,
                               const bool replaceExisting)
    : Pattern(l),
      Range(range),
      ReplaceExisting(replaceExisting)
{
}

bool PatternInRange::IsMatch(BytePtr& ptr)
{
    for (size_t i = 0; i < Range.Length; i++)
    {
        bool fullMatch = true;
        for (size_t j = 0; j < this->size(); j++)
        {
            if (!this->at(j).IsWildcard && this->at(j).Value != *(ptr + i + j))
            {
                fullMatch = false;
                break;
            }
        }
        if (fullMatch)
        {
            if (ReplaceExisting) ptr += i;
            return true;
        }
    }
    return false;
}

PatternStream::PatternStream(Pattern pattern, const std::string& module)
{
    const auto interval = PatternInfo::GetModuleInterval(module);
    for (BytePtr i = interval.Start; i < interval.Start + interval.Length; i++)
    {
        if (pattern.IsMatch(i))
        {
            push_back(i);
        }
    }
}

PatternStream PatternStream::operator&(Pattern pattern) const
{
    PatternStream stream;
    for (const auto& i : *this)
    {
        auto j = i;
        if (pattern.IsMatch(j))
        {
            stream.push_back(j);
        }
    }
    return stream;
}

PatternStream PatternStream::operator|(const PatternStream& other) const
{
    PatternStream stream(size() + other.size());
    stream.insert(stream.begin(), begin(), end());
    stream.insert(stream.begin() + static_cast<int64_t>(size()), other.begin(), other.end());
    return stream;
}

PatternStream PatternStream::operator+(const int64_t offset) const
{
    PatternStream stream = *this;
    for (auto& i : stream)
    {
        i += offset;
    }
    return stream;
}

PatternStream PatternStream::operator-(const int64_t offset) const
{
    PatternStream stream = *this;
    for (auto& i : stream)
    {
        i -= offset;
    }
    return stream;
}

PatternStream PatternStream::operator<(const ByteBuffer& buffer) const
{
    const PatternStream stream = *this;
    for (const auto& i : stream)
    {
        PatternPatcher::Write(i, buffer, 0);
    }
    return *this;
}

BytePtr PatternStream::FirstOrNullptr() const
{
    return empty() ? nullptr : at(0);
}

PatternStream::PatternStream(const size_t capacity) : std::vector<BytePtr>(capacity)
{
}

bool PatternPatcher::Write(const BytePtr ptr, const ByteBuffer& buffer, const int64_t offset)
{
    const auto handle = PatternInfo::GetProcessHandle();
    return WriteProcessMemory(handle, ptr + offset, buffer.data(), buffer.size(), nullptr);
}
