#include <mutator/copy.h>
#include <container/corpus/corpus.h>

namespace vfuzz {
namespace mutator {

template <class T>
Copy<T>::Copy(std::shared_ptr<util::Random> Rand, const util::Conduit<T> corpus, std::optional<History*> history) :
    Base(Rand, "Copy", history), corpus(corpus)
    { }

template <class T>
Chunk Copy<T>::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    (void)insert;
    (void)input;
    (void)maxChunkLen;

    Chunk ret = corpus.Get(&gid);
    insert = ret;

    LogHistory(gid, insert);

    return ret;
}

/* Explicit instantiation */
template class Copy<std::shared_ptr<container::corpus::Corpus> >;

} /* namespace mutator */
} /* namespace vfuzz */
