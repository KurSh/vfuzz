#pragma once

#include <container/input.h>
#include <sensor/data/data.h>
#include <container/base/serializable.h>
#include <map>
#include <vector>
#include <set>
#include <memory>
#include <utility>

namespace vfuzz {
namespace container {

class InputCluster : public Serializable {
    private:
        using cluster_t = std::map<GeneratorID, std::shared_ptr<Input>>;
        cluster_t cluster;
        std::shared_ptr<util::Random> Rand = nullptr;
        const size_t maxChunks;
        const size_t maxChunkLen;
        std::map< GeneratorID, size_t > requestedGids;
        std::optional<sensor::BaseData> sensorData = {};
    public:
        InputCluster(const InputCluster &inputCluster);
        InputCluster(std::shared_ptr<util::Random> Rand, size_t maxChunks, size_t maxChunkLen);
        ~InputCluster() = default;
        std::shared_ptr<Input> GetUnsafe(const GeneratorID gid) const;
        bool HaveGID(const GeneratorID id) const;
        std::shared_ptr<Input> Get(const GeneratorID gid);
        std::shared_ptr<Input> MutateRandom(const GeneratorID gid, std::shared_ptr<mutator::Mutator> mutator, const size_t chunkIndex);
        std::vector<GeneratorID> GetGIDS(void) const;
        void Set(const GeneratorID gid, std::shared_ptr<Input> input);
        std::set<InputCluster*> ReplaceAll(const Chunk from, const Chunk to);
        std::vector<uint8_t> Serialize(void) const override;
        void Deserialize(const std::vector<uint8_t> serialized) override;
        void SetSensorData(sensor::BaseData _sensorData);
        size_t GetLargestChunkSize(void) const;
        size_t GetLargestChunkSequence(void) const;
};

} /* namespace container */
} /* namespace vfuzz */
