#include <mutator/random.h>
#include <util/string/string.h>

namespace vfuzz {
namespace mutator {

Random::Random(std::shared_ptr<util::Random> Rand, std::optional<History*> history) :
    Base(Rand, "Random", history)
    { }

/* Returns a chunk of size 1 - maxChunkLen entirely containing random bytes */
Chunk Random::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    (void)input;

    Chunk ret;
    size_t size = Rand->Get(maxChunkLen + 1);
    if ( size == 0 ) {
        /* Empty chunks (size=0) are useless */
        size = 1;
    }

    if ( size > maxChunkLen ) {
        return input;
    }

    ret.resize(size);
    insert.resize(size);
    uint8_t* ptrRet = ret.data();
    uint8_t* ptrInsert = insert.data();
    for (size_t i = 0; i < size; i++) {
        /* TODO RandCh is very slow */
        ptrRet[i] = RandCh();
        ptrInsert[i] = ptrRet[i];
    }

    LogHistory(gid, insert);

    return ret;
}

} /* namespace mutator */
} /* namespace vfuzz */
