#include <mutator/copypart.h>

namespace vfuzz {
namespace mutator {

CopyPart::CopyPart(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    Base(Rand, "CopyPart", history)
    { }

Chunk CopyPart::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    (void)maxChunkLen;

    Chunk ret = input;
    if ( input.empty() == true ) {
        return ret;
    }

    if ( Rand->RandBool() == true ) {
        CopyPartOf(ret, ret, insert);
    } else {
        InsertPartOf(ret, ret, maxChunkLen, insert);
    }

    LogHistory(gid, insert);

    return ret;
}

} /* namespace mutator */
} /* namespace vfuzz */
