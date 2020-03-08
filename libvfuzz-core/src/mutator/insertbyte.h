#pragma once

#include <mutator/base.h>

namespace vfuzz {
namespace mutator {

class InsertByte : public Base {
    protected:
        Chunk MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) override;
    public:
        InsertByte(std::shared_ptr<util::Random> Rand, std::optional<History*> history = {});
};

} /* namespace mutator */
} /* namespace vfuzz */
