#include <mutator/insertrepeatedbytes.h>
#include <util/string/string.h>
#include <cstring>

namespace vfuzz {
namespace mutator {

InsertRepeatedBytes::InsertRepeatedBytes(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    Base(Rand, "InsertRepeatedBytes", history)
    { }

/* Returns the input chunk with a string of one random byte inserted at a random position */
Chunk InsertRepeatedBytes::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    if ( input.size() >= maxChunkLen ) {
        return input;
    }

    Chunk ret = input;

    /* Position in 'input' where to insert to */
    const size_t destPos = RandomPos(input);

    /* The number of bytes that can be inserted at most */
    const size_t maxInsertSize = maxChunkLen - input.size();

    const size_t numInsert = Rand->Get(maxInsertSize+1);
    Split(ret, destPos, numInsert);

    insert.resize(numInsert);

    const uint8_t byte = RandCh();
    for (size_t i = 0; i < numInsert; i++) {
        ret[destPos + i] = byte;
        insert[i] = byte;
    }

    LogHistory(gid, insert);

    return ret;
}

} /* namespace mutator */
} /* namespace vfuzz */
