#pragma once

#include <mutator/base.h>
#include <util/conduit.h>

namespace vfuzz {
namespace mutator {

template <class T>
class Copy : public Base {
    private:
        const util::Conduit<T> corpus;
    protected:
        Chunk MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) override;
    public:
        Copy(std::shared_ptr<util::Random> Rand, const util::Conduit<T> corpus, std::optional<History*> history = {});
};

} /* namespace mutator */
} /* namespace vfuzz */
