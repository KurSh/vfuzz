#pragma once

#include <mutator/base.h>
#include <util/conduit.h>
#include <boost/format.hpp>

namespace vfuzz {
namespace mutator {

template <class T>
class InsertDictionary : public Base {
    private:
        const util::Conduit<T> dictionary;
        const bool replace;
    protected:
        Chunk MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) override;
    public:
        InsertDictionary(std::shared_ptr<util::Random> Rand, const util::Conduit<T> dictionary, const std::string name, const bool replace, std::optional<History*> history = {});
};

} /* namespace mutator */
} /* namespace vfuzz */
