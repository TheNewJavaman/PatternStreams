#include "PatternStreams.h"

#include "PatternStreamsPrivate.h"

namespace PS {
    PatternByte::PatternByte(const Byte value) : Value(value), IsWildcard(false) {}

    PatternByte PatternByte::Any() {
        PatternByte pb(0x00);
        pb.IsWildcard = true;
        return pb;
    }

    Pattern::Pattern(const std::initializer_list<PatternByte> l): std::vector<PatternByte>(l) {}

    bool Pattern::IsMatch(BytePtr& ptr) const {
        for (size_t i = 0; i < this->size(); i++) {
            if (!this->at(i).IsWildcard && this->at(i).Value != *(ptr + i)) {
                return false;
            }
        }
        return true;
    }

    PatternInRange::PatternInRange(const Pattern& pattern, const OffsetInterval range, const bool replaceExisting)
        : Pattern(pattern),
          Range(range),
          ReplaceExisting(replaceExisting) {}

    bool PatternInRange::IsMatch(BytePtr& ptr) const {
        for (size_t i = 0; i < Range.Length; i++) {
            bool fullMatch = true;
            for (size_t j = 0; j < this->size(); j++) {
                if (!this->at(j).IsWildcard && this->at(j).Value != *(ptr + Range.Offset + i + j)) {
                    fullMatch = false;
                    break;
                }
            }
            if (fullMatch) {
                if (ReplaceExisting) {
                    ptr += i + Range.Offset;
                }
                return true;
            }
        }
        return false;
    }

    WriteBuffer::WriteBuffer(const std::initializer_list<Byte> l) : ByteBuffer(l) {}

    WriteBufferAndOffset::WriteBufferAndOffset(const std::initializer_list<Byte> l) : WriteBuffer(l) {}

    PatternStream::PatternStream(const std::initializer_list<PatternByte> l) : PatternStream(l, "") {}

    PatternStream::PatternStream(const Pattern& pattern, const std::string& module) {
        const auto interval = PatternInfo::GetModuleInterval(module);
        for (BytePtr i = interval.Start; i < interval.Start + interval.Length; i++) {
            if (pattern.IsMatch(i)) {
                push_back(i);
            }
        }
    }

    PatternStream PatternStream::operator|(const Pattern& pattern) const {
        PatternStream stream;
        for (const auto& i : *this) {
            auto j = i;
            if (pattern.IsMatch(j)) {
                stream.push_back(j);
            }
        }
        return stream;
    }

    PatternStream PatternStream::operator|(const AddOffset& offset) const {
        PatternStream stream = *this;
        for (auto& i : stream) {
            i += offset.Offset;
        }
        return stream;
    }

    PatternStream PatternStream::operator|(const WriteBuffer& buffer) const {
        PatternStream stream = *this;
        for (const auto& i : stream) {
            PatternPatcher::WriteBuffer(i, buffer);
        }
        return stream;
    }

    PatternStream PatternStream::operator|(const WriteBufferAndOffset& buffer) const {
        PatternStream stream = *this;
        for (auto& i : stream) {
            PatternPatcher::WriteBufferAndOffset(i, buffer);
        }
        return stream;
    }

    PatternStream PatternStream::operator|(const ForEach& func) const {
        PatternStream stream = *this;
        for (auto& i : stream) {
            func(i);
        }
        return stream;
    }

    BytePtr PatternStream::FirstOrNullptr() const {
        return empty() ? nullptr : at(0);
    }

    PatternStream::PatternStream(const size_t capacity) : std::vector<BytePtr>(capacity) {}

    bool PatternPatcher::WriteBuffer(const BytePtr ptr, const ByteBuffer& buffer) {
        const auto handle = PatternInfo::GetProcessHandle();
        return WriteProcessMemory(handle, ptr, buffer.data(), buffer.size(), nullptr);
    }

    bool PatternPatcher::WriteBufferAndOffset(BytePtr& ptr, const ByteBuffer& buffer) {
        if (!WriteBuffer(ptr, buffer)) {
            return false;
        }
        ptr += buffer.size();
        return true;
    }
}
