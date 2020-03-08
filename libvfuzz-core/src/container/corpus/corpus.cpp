#include <container/corpus/corpus.h>
#include <base/exception.h>
#include <tuple>
#include <cstring>
#include <cassert>

namespace vfuzz {
namespace container {
namespace corpus {

Corpus::Corpus(std::shared_ptr<util::Random> Rand, InputCluster inputCluster) :
    Rand(Rand) {
    corpus.push_back(std::make_shared<InputCluster>(inputCluster));
}

void Corpus::cleanup(void) {
    corpus.clear();
}

Corpus::~Corpus() {
    cleanup();
}

std::shared_ptr<InputCluster> Corpus::GetRandomCopy(void) const {
    if ( corpus.empty() == true ) {
        throw vfuzz::Exception("Tried retrieving entry from empty corpus");
        /* TODO */
    } else {
        return std::make_shared<InputCluster>(*(corpus[ Rand->Get(corpus.size()) ]));
    }
}

std::shared_ptr<InputCluster> Corpus::GetRandomPtr(void) const {
    if ( corpus.empty() == true ) {
        throw vfuzz::Exception("Tried retrieving entry from empty corpus");
        /* TODO */
    } else {
        return corpus[ Rand->Get(corpus.size()) ];
    }
}

std::shared_ptr<InputCluster> Corpus::Get(const size_t index) const {
    if ( index >= corpus.size() ) {
        throw vfuzz::Exception("Tried retrieving entry with invalid index from corpus");
        /* TODO */
    }
    return corpus[index];
}

size_t Corpus::Size(void) const {
    return corpus.size();
}

void Corpus::AddInputCluster(std::shared_ptr<InputCluster> inputCluster) {
    corpus.push_back( inputCluster );
    if ( onNew ) {
        (*onNew)(inputCluster);
    }
}

void Corpus::SetOnNew(const std::function<void(std::shared_ptr<InputCluster> inputCluster)> _onNew)
{
    onNew = _onNew;
}

std::vector<uint8_t> Corpus::Serialize(void) const {
    std::vector<uint8_t> ret;

    /* Calculate and allocate required size */
    {
        size_t total_size = 0;
        for ( const auto& IC : corpus ) {
            std::tuple<uint64_t, std::vector<uint8_t>> tuple;

            const auto serialized_inputcluster = IC->Serialize();
            const uint64_t serialized_size = serialized_inputcluster.size();

            std::get<0>(tuple) = serialized_size;
            std::get<1>(tuple) = serialized_inputcluster;

            total_size += sizeof(serialized_size);
            total_size += serialized_inputcluster.size();
        }

        ret.resize(sizeof(uint64_t) + total_size);
    }

    const uint64_t numInputClusters = corpus.size();
    size_t pos = 0;

    memcpy(&(ret[pos]), &numInputClusters, sizeof(numInputClusters));
    pos += sizeof(numInputClusters);

    for ( const auto& IC : corpus ) {
        const auto serializedIC = IC->Serialize();

        /* Write size of serialized InputCluster  */
        const uint64_t size = serializedIC.size();

        memcpy(&(ret[pos]), &size, sizeof(size));
        pos += sizeof(size);

        memcpy(&(ret[pos]), serializedIC.data(), serializedIC.size());
        pos += serializedIC.size();

    }

    return ret;
}

void Corpus::Deserialize(const std::vector<uint8_t> serialized) {
    static const std::string deserialize_error("Corpus deserialization: input data corrupted");

    size_t pos = 0;
    size_t size_left = serialized.size();

    uint64_t numInputClusters;
    /* Deserialize numInputClusters */
    {
        if ( size_left < sizeof(numInputClusters) ) {
            throw DeserializationException();
        }
        memcpy(&numInputClusters, &(serialized[pos]), sizeof(numInputClusters));
        pos += sizeof(numInputClusters);
        size_left -= sizeof(numInputClusters);
    }

    for (size_t i = 0; i < numInputClusters; i++) {
        uint64_t serialized_size;
        std::shared_ptr<InputCluster> inputCluster = nullptr;

        /* Deserialize serialized_size */
        {
            if ( size_left < sizeof(serialized_size) ) {
                throw DeserializationException();
            }

            memcpy(&serialized_size, &(serialized[pos]), sizeof(serialized_size));

            pos += sizeof(serialized_size);
            size_left -= sizeof(serialized_size);
        }

        /* Deserialize InputCluster */
        {
            if ( size_left < serialized_size ) {
                throw DeserializationException();
            }

            std::vector<uint8_t> serialized_inputcluster;

            if ( serialized_size > 0 ) {
                serialized_inputcluster.resize(serialized_size);

                memcpy(&(serialized_inputcluster[0]), &(serialized[pos]), serialized_size);

                pos += serialized_size;
                size_left -= serialized_size;
            }

            inputCluster = std::make_shared<InputCluster>(Rand, 4096, 4096);

            try {
                assert(inputCluster != nullptr);

                inputCluster->Deserialize(serialized_inputcluster);
            } catch ( DeserializationException ) {
                throw DeserializationException();
            }
        }

        assert(inputCluster != nullptr);

        AddInputCluster(inputCluster);
    }
}

util::PushPullCallback<std::shared_ptr<InputCluster>> Corpus::GetBidirectionalCallback(void) {
    return util::PushPullCallback<std::shared_ptr<InputCluster>>(
            std::bind(&Corpus::AddInputCluster, this, std::placeholders::_1),
            std::bind(&Corpus::GetRandomCopy, this)
        );
}

} /* namespace corpus */
} /* namespace container */
} /* namespace vfuzz */
