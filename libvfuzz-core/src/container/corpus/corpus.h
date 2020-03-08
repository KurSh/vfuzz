#pragma once

#include <util/random.h>
#include <mutator/mutator.h>
#include <container/input.h>
#include <container/inputcluster.h>
#include <util/pushpullcallback.h>
#include <container/base/serializable.h>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include <utility>

namespace vfuzz {
namespace container {
namespace corpus {

class Corpus : public Serializable {
    private:
        std::vector<std::shared_ptr<InputCluster>> corpus;
        std::shared_ptr<util::Random> Rand;

        /* Callbacks */
        std::optional<std::function<void(std::shared_ptr<InputCluster> inputCluster)>> onNew = {};

        void cleanup(void);
    public:
        Corpus(std::shared_ptr<util::Random> Rand, InputCluster inputCluster);
        ~Corpus();
        std::shared_ptr<InputCluster> GetRandomCopy(void) const;
        std::shared_ptr<InputCluster> GetRandomPtr(void) const;
        void AddInputCluster(std::shared_ptr<InputCluster> inputCluster);
        void SetOnNew(const std::function<void(std::shared_ptr<InputCluster> inputCluster)> _onNew);
        std::shared_ptr<InputCluster> Get(const size_t index) const;
        size_t Size(void) const;
        std::vector<uint8_t> Serialize(void) const override;
        void Deserialize(const std::vector<uint8_t> serialized) override;
        util::PushPullCallback<std::shared_ptr<InputCluster>> GetBidirectionalCallback(void);
};

} /* namespace corpus */
} /* namespace container */
} /* namespace vfuzz */
