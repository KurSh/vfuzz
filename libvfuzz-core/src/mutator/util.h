#pragma once

#include <mutator/mutator.h>
#include <mutator/history.h>
#include <container/input.h>
#include <util/random.h>
#include <vector>
#include <optional>
#include <mutator/container.h>

namespace vfuzz {
namespace mutator {

std::vector<std::pair<std::shared_ptr<Mutator>, size_t>> SortMutators(std::vector<std::shared_ptr<container::Input>> inputs);
std::shared_ptr<Mutator> RandMutatorByWeight(util::Random* Rand, std::vector<std::pair<std::shared_ptr<Mutator>, size_t>> mutatorCount);
std::set<Chunk> GetTopInsertions(std::vector<std::shared_ptr<container::Input>> insertions, const size_t max);
std::shared_ptr<Container> GetDefaultContainer(std::shared_ptr<util::Random> Rand, std::optional<History*> argHistory = {});

} /* namespace mutator */
} /* namespace vfuzz */
