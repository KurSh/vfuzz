#include <mutator/crossover.h>
#include <container/corpus/corpus.h>

namespace vfuzz {
namespace mutator {

template <class T>
CrossOver<T>::CrossOver(std::shared_ptr<util::Random> Rand, const util::Conduit<T> corpus, std::optional<History*> history) :
    Base(Rand, "CrossOver", history), corpus(corpus)
    { }

template <class T>
Chunk CrossOver<T>::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    (void)insert;
    (void)maxChunkLen;

    if ( input.empty() == true ) {
        return input;
    }

    Chunk ret = input;

    const Chunk crossOverChunk = corpus.Get(&gid);

    if ( crossOverChunk.empty() == false ) {
        if ( Rand->RandBool() == true ) {
            CopyPartOf(ret, crossOverChunk, insert);
        } else {
            InsertPartOf(ret, crossOverChunk, maxChunkLen, insert);
        }
    }

    LogHistory(gid, insert);

    return ret;
}

/* Explicit instantiation */
template class CrossOver<std::shared_ptr<container::corpus::Corpus> >;

} /* namespace mutator */
} /* namespace vfuzz */
