#pragma once

#include <fuzzing/datasource/datasource.hpp>
#include <memory>
#include <container/inputcluster.h>
#include <mutator/util.h>

namespace vfuzz {
namespace datasource {

class Datasource : public ::fuzzing::datasource::Base {
    private:
        std::shared_ptr<container::InputCluster> ic;
        std::shared_ptr<mutator::Container> mutators;
        std::vector<uint8_t> get(const size_t min, const size_t max, const uint64_t id) override;
    public:
        Datasource(std::shared_ptr<container::InputCluster> ic, std::shared_ptr<mutator::Container> mutators);
};

} /* namespace datasource */
} /* namespace vfuzz */
