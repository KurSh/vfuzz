#pragma once

#include <mutator/mutator.h>
#include <mutator/history.h>
#include <vector>
#include <utility>

namespace vfuzz {
namespace mutator {

class Container {
    private:
        std::shared_ptr<util::Random> Rand;
        std::vector<std::shared_ptr<Mutator>> mutators;
    public:
        Container(std::shared_ptr<util::Random> Rand);
        ~Container() = default;
        std::shared_ptr<Mutator> GetRandomMutator(void);
        void Add(std::shared_ptr<Mutator> mutator);
};

} /* namespace mutator */
} /* namespace vfuzz */
