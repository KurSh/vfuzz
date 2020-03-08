#pragma once

#include <mutator/mutator.h>
#include <base/types.h>
#include <container/base/serializable.h>
#include <map>
#include <memory>
#include <set>

namespace vfuzz {
namespace container {

typedef size_t ChunkIndex;

class Input : public std::enable_shared_from_this<Input>, public Serializable {
    private:
        std::vector< Chunk > chunks, insertions;
        std::shared_ptr<util::Random> Rand = nullptr;
        std::set<std::shared_ptr<mutator::Mutator>> mutators;
    public:
        Input(std::shared_ptr<util::Random> Rand);
        Input(const Input &input);
        Chunk Get(const ChunkIndex chunkIndex) const;
        Chunk GetRandom(void) const;
        void Add(const Chunk chunk);
        void Set(const ChunkIndex chunkIndex, const Chunk chunk);
        size_t Size(void) const;
        std::shared_ptr<Input> MutateRandom(const GeneratorID gid, std::shared_ptr<mutator::Mutator> mutator, const size_t maxChunks, const size_t maxChunkLen);
        void ClearMutators(void);
        void ClearInsertions(void);
        std::set<std::shared_ptr<mutator::Mutator>> GetMutators(void) const;
        std::vector<Chunk> GetInsertions(void) const;
        void MutateChunk(const GeneratorID gid, const size_t chunkIdx, std::shared_ptr<mutator::Mutator> mutator, const size_t maxChunkLen);
        void ExpandChunk(const ChunkIndex chunkIndex, const size_t min);
        void CropChunk(const ChunkIndex chunkIndex, const size_t max);
        std::vector<uint8_t> Serialize(void) const override;
        void Deserialize(const std::vector<uint8_t> serialized) override;
        size_t GetLargestChunkSize(void) const;
};

} /* namespace container */
} /* namespace vfuzz */
