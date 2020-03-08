#pragma once

#include <memory>
#include <util/random.h>
#include <string>
#include <optional>
#include <mutator/history.h>
#include <cstdint>
#include <base/types.h>
#include <algorithm>

namespace vfuzz {
namespace mutator {

class Base {
    protected:
        std::shared_ptr<util::Random> Rand;
        const std::string name;
        std::optional<History*> history;
        uint8_t RandCh() {
            return (uint8_t)(Rand->Get(256));
        }
        void CopyPartOf(Chunk& dest, const Chunk& src, Chunk& insert) const;
        void InsertPartOf(Chunk& dest, const Chunk& src, const size_t maxDestSize, Chunk& insert) const;
        void Split(Chunk& dest, const size_t pos, const size_t length) const;
        size_t RandomPos(const Chunk& in) const;
        size_t RandomPos(const size_t size) const;
        std::pair<size_t, size_t> RandomRange(const size_t size) const;

        template <class T = uint8_t>
        void LogHistory(const GeneratorID gid, const T& in);

        virtual Chunk MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) = 0;
    public:
        Base(std::shared_ptr<util::Random> Rand, std::string name, std::optional<History*> history);
        std::string Name(void) const { return name; }
        virtual ~Base() { }
        Chunk Mutate(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen);
};

} /* namespace mutator */
} /* namespace vfuzz */
