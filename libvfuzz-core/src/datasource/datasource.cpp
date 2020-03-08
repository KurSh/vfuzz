#include "datasource.h"

namespace vfuzz {
namespace datasource {

Datasource::Datasource(std::shared_ptr<container::InputCluster> ic, std::shared_ptr<mutator::Container> mutators) :
    Base(), ic(ic), mutators(mutators) { }

std::vector<uint8_t> Datasource::get(const size_t min, const size_t max, const uint64_t id)
{
    auto input = ic->MutateRandom(id, mutators->GetRandomMutator(), 0);
    input->ExpandChunk(0, min);
    return input->Get(0);
}

} /* namespace datasource */
} /* namespace vfuzz */
