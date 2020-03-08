#include <mutator/container.h>
#include <base/exception.h>
#include <algorithm>

namespace vfuzz {
namespace mutator {

Container::Container(std::shared_ptr<util::Random> Rand) :
    Rand(Rand) {
}

std::shared_ptr<Mutator> Container::GetRandomMutator(void) {
    if ( mutators.empty() == true ) {
        throw vfuzz::Exception("GetRandomMutator: no mutators defined");
    }

    return mutators[Rand->Get(mutators.size())];
}

void Container::Add(std::shared_ptr<Mutator> mutator) {
    if ( std::find(mutators.begin(), mutators.end(), mutator) != mutators.end() ) {
        throw vfuzz::Exception("Tried adding duplicate mutator pointers");
    }

    mutators.push_back(mutator);
}

} /* namespace mutator */
} /* namespace vfuzz */
