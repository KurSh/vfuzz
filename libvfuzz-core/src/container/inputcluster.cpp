#include <container/inputcluster.h>
#include <base/exception.h>
#include <tuple>
#include <cstring>
#include <cassert>

namespace vfuzz {
namespace container {

/* InputCluster copy constructor */
InputCluster::InputCluster(const InputCluster &inputCluster) :
    Rand(inputCluster.Rand),
    maxChunks(inputCluster.maxChunks),
    maxChunkLen(inputCluster.maxChunkLen) {
    for ( const auto& kv : inputCluster.cluster ) {
        auto inputCopy = std::make_shared<Input>(*kv.second);
        cluster.insert(std::pair<GeneratorID, std::shared_ptr<Input>>(kv.first, inputCopy));
    }
}

InputCluster::InputCluster(std::shared_ptr<util::Random> Rand, size_t maxChunks, size_t maxChunkLen) :
    Rand(Rand), maxChunks(maxChunks), maxChunkLen(maxChunkLen) {
}

/* Like Get(), but does not generate a new Input if the gid does not exist */
std::shared_ptr<Input> InputCluster::GetUnsafe(const GeneratorID gid) const {
    const auto it = cluster.find(gid);
    if ( it == cluster.end() ) {
        throw vfuzz::Exception("InputCluster::GetUnsafe: tried getting non-existent input");
    }
    return it->second;
}

bool InputCluster::HaveGID(const GeneratorID gid) const
{
    return cluster.count(gid) > 0;
}

std::shared_ptr<Input> InputCluster::Get(const GeneratorID gid) {
    const cluster_t::iterator it = cluster.find(gid);

    if ( it == cluster.end() ) {
        /* Not found -- create and return empty Input */
        const auto ret = std::make_shared<Input>(Rand);
        cluster.insert(std::pair<GeneratorID, std::shared_ptr<Input>>(gid, ret));
        return ret;
    } else {
        /* Found */
        return it->second;
    }
}

std::shared_ptr<Input> InputCluster::MutateRandom(const GeneratorID gid, std::shared_ptr<mutator::Mutator> mutator, const size_t chunkIndex) {
    return Get(gid)->MutateRandom(gid, mutator, std::min(chunkIndex, maxChunks), maxChunkLen);
}

std::vector<GeneratorID> InputCluster::GetGIDS(void) const {
    std::vector<GeneratorID> ret;
    for(const auto& kv: cluster ) {
        ret.push_back(kv.first);
    }
    return ret;
}

void InputCluster::Set(const GeneratorID gid, std::shared_ptr<Input> input) {
    cluster.insert(std::pair<GeneratorID, std::shared_ptr<Input>>(gid, input));
}

std::set<InputCluster*> InputCluster::ReplaceAll(const Chunk from, const Chunk to) {
    (void)from;
    (void)to;
    std::set<InputCluster*> ret;
    /* TODO */

    return ret;
}

std::vector<uint8_t> InputCluster::Serialize(void) const {
    std::vector<uint8_t> ret;
    std::vector<
        std::tuple<uint64_t, uint64_t, std::vector<uint8_t>>
    > serialized_inputs;

    /* Calculate and allocate required size */
    {
        size_t total_size = 0;
        for ( const auto& I : cluster ) {
            std::tuple<uint64_t, uint64_t, std::vector<uint8_t>> tuple;

            const uint64_t GeneratorID = I.first;
            const auto serialized_input = I.second->Serialize();
            const uint64_t serialized_size = serialized_input.size();

            std::get<0>(tuple) = GeneratorID;
            std::get<1>(tuple) = serialized_size;
            std::get<2>(tuple) = serialized_input;

            total_size += sizeof(uint64_t);
            total_size += sizeof(uint64_t);
            total_size += serialized_input.size();

            serialized_inputs.push_back(tuple);
        }

        ret.resize(sizeof(uint64_t) + total_size);
    }

    const uint64_t numInputs = cluster.size();
    size_t pos = 0;
    memcpy(&(ret[pos]), &numInputs, sizeof(numInputs));
    pos += sizeof(numInputs);

    for ( const auto& tpl : serialized_inputs ) {
        /* Write generator id */
        memcpy(&(ret[pos]), &(std::get<0>(tpl)), sizeof(std::get<0>(tpl)));
        pos += sizeof(std::get<0>(tpl));

        /* Write size of serialized input */
        memcpy(&(ret[pos]), &(std::get<1>(tpl)), sizeof(std::get<1>(tpl)));
        pos += sizeof(std::get<1>(tpl));

        /* Write serialized input */
        memcpy(&(ret[pos]), std::get<2>(tpl).data(), std::get<2>(tpl).size());
        pos += std::get<2>(tpl).size();
    }

    return ret;
}

void InputCluster::Deserialize(const std::vector<uint8_t> serialized) {
    static const std::string deserialize_error("InputCluster deserialization: input data corrupted");

    size_t pos = 0;
    size_t size_left = serialized.size();

    uint64_t numInputs;
    /* Deserialize numInputs */
    {
        if ( size_left < sizeof(numInputs) ) {
            throw DeserializationException();
        }
        memcpy(&numInputs, &(serialized[pos]), sizeof(numInputs));
        pos += sizeof(numInputs);
        size_left -= sizeof(numInputs);
    }

    for (size_t i = 0; i < numInputs; i++) {
        uint64_t GeneratorID;
        uint64_t serialized_size;
        std::shared_ptr<Input> input = nullptr;

        /* Deserialize GeneratorID */
        {
            if ( size_left < sizeof(GeneratorID) ) {
                throw DeserializationException();
            }
            memcpy(&GeneratorID, &(serialized[pos]), sizeof(GeneratorID));
            pos += sizeof(GeneratorID);
            size_left -= sizeof(GeneratorID);
        }

        if ( HaveGID(GeneratorID) == true ) {
            throw DeserializationException();
        }

        /* Deserialize serialized_size */
        {
            if ( size_left < sizeof(serialized_size) ) {
                throw DeserializationException();
            }
            memcpy(&serialized_size, &(serialized[pos]), sizeof(serialized_size));
            pos += sizeof(serialized_size);
            size_left -= sizeof(serialized_size);
        }

        /* Deserialize Input */
        {
            if ( size_left < serialized_size ) {
                throw DeserializationException();
            }

            std::vector<uint8_t> serialized_input;

            if ( serialized_size > 0 ) {
                serialized_input.resize(serialized_size);
                memcpy(&(serialized_input[0]), &(serialized[pos]), serialized_size);
                pos += serialized_size;
                size_left -= serialized_size;
            }

            input = std::make_shared<Input>(Rand);

            try {
                assert(input != nullptr);

                input->Deserialize(serialized_input);
            } catch ( DeserializationException ) {
                throw DeserializationException();
            }
        }

        assert(input != nullptr);
        this->Set(GeneratorID, input);
    }
}

void InputCluster::SetSensorData(sensor::BaseData _sensorData) {
    sensorData = _sensorData;
}

size_t InputCluster::GetLargestChunkSize(void) const {
    size_t largestChunkSize = 0;
    for ( const auto& kv : cluster ) {
        const auto curLargestChunkSize = kv.second->GetLargestChunkSize();
        if ( curLargestChunkSize > largestChunkSize ) {
            largestChunkSize = curLargestChunkSize;
        }
    }
    return largestChunkSize;
}

size_t InputCluster::GetLargestChunkSequence(void) const {
    size_t largestChunkSequence = 0;
    for ( const auto& kv : cluster ) {
        const auto curLargestChunkSequence = kv.second->Size();
        if ( curLargestChunkSequence > largestChunkSequence ) {
            largestChunkSequence = curLargestChunkSequence;
        }
    }
    return largestChunkSequence;
}

} /* namespace container */
} /* namespace vfuzz */
