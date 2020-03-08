#include <mutator/util.h>
#include <map>
#include <set>

#include <mutator/copy.h>
#include <mutator/copypart.h>
#include <mutator/crossover.h>
#include <mutator/erasebytes.h>
#include <mutator/insertbyte.h>
#include <mutator/insertdictionary.h>
#include <mutator/insertrepeatedbytes.h>
#include <mutator/random.h>
#include <mutator/range_based.h>
#include <mutator/shufflebytes.h>

namespace vfuzz {
namespace mutator {

/* Given a set of pointers to Inputs containing metadata, returns a
 * vector of Mutator pointers sorted by usage, according to Input metadata */
std::vector<std::pair<std::shared_ptr<Mutator>, size_t>> SortMutators(std::vector<std::shared_ptr<container::Input>> inputs) {
    std::map<std::shared_ptr<Mutator>, size_t> mutatorCount;
    std::map<size_t, std::shared_ptr<Mutator>, std::greater<size_t>> mutatorCount2;
    /* Update the mutatorCount table for every mutator of every input */
    for ( auto &input : inputs ) {
        std::set<std::shared_ptr<Mutator>> mutators = input->GetMutators();
        for ( const auto &M : mutators ) {
            mutatorCount[M]++;
        }
        input->ClearMutators();
    }

    /* Convert mutatorCount to the sorted mutatorCount2 */
    for ( const auto &kv : mutatorCount ) {
        mutatorCount2.insert( std::make_pair(kv.second, kv.first) );
    }

    std::vector<std::pair<std::shared_ptr<Mutator>, size_t>> ret;
    std::map<size_t, std::shared_ptr<Mutator>, std::greater<size_t>>::iterator it;
    for (it = mutatorCount2.begin() ; it != mutatorCount2.end() ; it++) {
        ret.push_back(std::make_pair(it->second, it->first));
    }

    return ret;
}

std::shared_ptr<Mutator> RandMutatorByWeight(util::Random* Rand, std::vector<std::pair<std::shared_ptr<Mutator>, size_t>> mutatorCount) {
    size_t total = 0;
    for ( const auto &M : mutatorCount ) { total += M.second; }

    size_t num = Rand->Get(total+1);

    std::vector<std::pair<std::shared_ptr<Mutator>, size_t>>::iterator it;
    for (it = mutatorCount.begin(); it != mutatorCount.end(); it++) {
        if ( num <= it->second ) {
            const size_t prev = it->second;
            std::vector<std::shared_ptr<Mutator>> eligibleMutators;
            eligibleMutators.push_back(it->first);

            while ( it != mutatorCount.begin() && it->second == prev ) it--;
            if ( it->second != prev ) it++;
            for ( ; it != mutatorCount.end() && it->second == prev; it++) {
                eligibleMutators.push_back(it->first);
            }
            return eligibleMutators[ Rand->Get(eligibleMutators.size()) ];
        }
        num -= it->second;
    }

    /* This should never be reached */
    abort();

    return nullptr;
}

/* TODO move to different file ? */
std::set<Chunk> GetTopInsertions(std::vector<std::shared_ptr<container::Input>> inputs, const size_t max) {
    std::map<Chunk, size_t> insertionFrequency;
    std::map<size_t, Chunk, std::greater<size_t>> sortedInsertions;

    for ( const auto &input : inputs ) {
        const auto curInsertions = input->GetInsertions();
        for ( const auto &curInsertion : curInsertions ) {
            insertionFrequency[curInsertion]++;
        }
        input->ClearInsertions();
    }

    /* Convert mutatorCount to the sorted mutatorCount2 */
    for ( const auto &kv : insertionFrequency ) {
        sortedInsertions.insert( std::make_pair(kv.second, kv.first) );
    }
    std::set<Chunk> topInsertions;
    std::map<size_t, Chunk, std::greater<size_t>>::iterator it;
    for (it = sortedInsertions.begin() ; it != sortedInsertions.end() ; it++) {
        topInsertions.insert(it->second);
    }

    return topInsertions;
}

std::shared_ptr<Container> GetDefaultContainer(std::shared_ptr<util::Random> Rand, std::optional<History*> argMutatorHistory) {
    auto mutators = std::make_shared<Container>(Rand);

    mutators->Add( std::make_shared<Random>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<InsertByte>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<EraseBytes>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<InsertRepeatedBytes>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ShuffleBytes>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<CopyPart>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<And<false>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<And<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<Or<false>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<Or<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<Xor<false>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<Xor<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<Not<false>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<Not<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ShiftLeft<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ShiftLeft<false>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ShiftRight<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ShiftRight<false>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ChangeBit<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ChangeBit<false>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ChangeByte<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ChangeByte<false>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ReverseByte<true>>(Rand, argMutatorHistory) );
    mutators->Add( std::make_shared<ReverseByte<false>>(Rand, argMutatorHistory) );
    /*
    mutators.Add( std::make_shared<RORByte<true>>(Rand, argMutatorHistory) );
    mutators.Add( std::make_shared<RORByte<false>>(Rand, argMutatorHistory) );
    mutators.Add( std::make_shared<ROLByte<true>>(Rand, argMutatorHistory) );
    mutators.Add( std::make_shared<ROLByte<false>>(Rand, argMutatorHistory) );
    */

    return mutators;
}

} /* namespace mutator */
} /* namespace vfuzz */
