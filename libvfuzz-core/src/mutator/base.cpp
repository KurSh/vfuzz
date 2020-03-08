#include <mutator/base.h>
#include <util/string/string.h>
#include <cstring>

namespace vfuzz {
namespace mutator {

Base::Base(std::shared_ptr<util::Random> Rand, std::string name, std::optional<History*> history) :
    Rand(Rand), name(name), history(history)
    { }

void Base::CopyPartOf(Chunk& dest, const Chunk& src, Chunk& insert) const {
    /* Position in 'dest' where to copy to */
    const size_t destPos = RandomPos(dest);

    /* Range in 'src' where to copy from */
    const auto copyRange = RandomRange( std::min(src.size(), dest.size() - destPos) );

    /* Copy. dest and src may overlap so use memmove */
    memmove(dest.data() + destPos, src.data() + copyRange.first, copyRange.second);

    /* Set inserted bytes */
    insert.resize(copyRange.second);
    memcpy(insert.data(), src.data() + copyRange.first, copyRange.second);
}

void Base::Split(Chunk& dest, const size_t pos, const size_t length) const {
    const size_t newSize = dest.size() + length;
    const size_t tailSize = dest.size() - pos;

    dest.resize(newSize);
    memmove(dest.data() + pos + length, dest.data() + pos, tailSize);
}

void Base::InsertPartOf(Chunk& dest, const Chunk& src, const size_t maxDestSize, Chunk& insert) const {
    /* Check if dest is already at least as large as the permitted size.
     * If so, no additional bytes can be inserted
     */
    if ( dest.size() >= maxDestSize ) {
        return;
    }

    /* Position in 'dest' where to insert to */
    const size_t destPos = RandomPos(dest);

    /* The number of bytes that can be inserted at most */
    size_t maxInsertSize = maxDestSize - dest.size();
    maxInsertSize = std::min(src.size(), maxInsertSize);

    /* Range in 'src' where to insert */
    const auto insertRange = RandomRange(maxInsertSize);

    Split(dest, destPos, insertRange.second);
    memmove(dest.data() + destPos, src.data() + insertRange.first, insertRange.second);

    /* Set inserted bytes */
    insert.resize(insertRange.second);
    memcpy(insert.data(), src.data() + insertRange.first, insertRange.second);
}

size_t Base::RandomPos(const Chunk& in) const {
    return RandomPos(in.size());
}

size_t Base::RandomPos(const size_t size) const {
    return Rand->Get(size + 1);
}

std::pair<size_t, size_t> Base::RandomRange(const size_t size) const {
    const size_t startPos = RandomPos(size);
    const size_t rangeSize = RandomPos(size - startPos);

    return {startPos, rangeSize};
}

namespace Base_detail {
    template <class T>
        std::string toString(const T& in) {
            return util::string::ToHexString(in);
        }

    template <>
        std::string toString(const std::string& in) {
            return in;
        }
}

template <class T>
void Base::LogHistory(const GeneratorID gid, const T& in) {
    if ( history != std::nullopt ) {
        const auto desc = Base_detail::toString(in);
        (*history)->Add(gid, name, desc);
    }
}

Chunk Base::Mutate(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    return MutateImpl(gid, input, insert, maxChunkLen);
}

/* Explicit instantiation */
template void Base::LogHistory(const GeneratorID gid, const Chunk& in);
template void Base::LogHistory(const GeneratorID gid, const uint8_t& in);
template void Base::LogHistory(const GeneratorID gid, const std::string& in);

} /* namespace mutator */
} /* namespace vfuzz */
