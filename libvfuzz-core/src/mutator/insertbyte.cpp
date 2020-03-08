#include <mutator/insertbyte.h>

namespace vfuzz {
namespace mutator {

InsertByte::InsertByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    Base(Rand, "InsertByte", history)
    { }

/* Returns the input chunk with one byte inserted at a random position */
Chunk InsertByte::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {

    if ( input.size() == maxChunkLen ) {
        /* Cannot add one more byte */
        return input;
    }

    Chunk ret = input;
    const uint8_t byte = RandCh();
    insert.push_back(byte);

    /* Insert at random position */
    ret.insert( ret.begin() + Rand->Get(ret.size()), byte );

    LogHistory(gid, byte);

    return ret;
}

} /* namespace mutator */
} /* namespace vfuzz */
