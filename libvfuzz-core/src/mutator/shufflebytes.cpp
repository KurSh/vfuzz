#include <mutator/shufflebytes.h>
#include <algorithm>

namespace vfuzz {
namespace mutator {

ShuffleBytes::ShuffleBytes(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    Base(Rand, "ShuffleBytes", history)
    { }

Chunk ShuffleBytes::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    (void)insert;
    (void)maxChunkLen;

    if ( input.empty() == true ) {
        return input;
    }

    Chunk ret = input;

    const auto range = RandomRange(ret.size());
    std::shuffle(ret.begin() + range.first, ret.begin() + range.first + range.second, *Rand);

    LogHistory(gid, {});

    return ret;
}

} /* namespace mutator */
} /* namespace vfuzz */
