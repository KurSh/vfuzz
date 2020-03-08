#include <mutator/erasebytes.h>

namespace vfuzz {
namespace mutator {

EraseBytes::EraseBytes(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    Base(Rand, "EraseBytes", history)
    { }

/* Returns the input chunk with a random range removed */
Chunk EraseBytes::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    /* insert is not relevant - no bytes are getting inserted */
    (void)insert;
    /* maxChunkLen is not relevant - output is always shorter than the input */
    (void)maxChunkLen;

    const auto range = RandomRange(input.size());
    const Chunk erased(input.begin() + range.first, input.begin() + range.first + range.second);

    LogHistory(gid, {});

    return erased;
}

} /* namespace mutator */
} /* namespace vfuzz */
