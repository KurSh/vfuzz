#include <dictionary/dictionary.h>

namespace vfuzz {
namespace dictionary {

/* A Dictionary is a key-value map.
 * The key is a size_t also referred to as a 'set id'.
 * The value is a set of Chunk's.
 *
 * In Python/pseudocode:
 *
 * dictionary[ID] = set()
 */

Dictionary::Dictionary(std::shared_ptr<util::Random> Rand) :
    Base(Rand)
{
}

void Dictionary::Add(const size_t ID, const Chunk chunk) {
    if ( dictionary.count(ID) == 0 ) {
        /* Create a new set */
        dictionary[ID] = dictionary_entry_t();
    }

    dictionary[ID].insert(chunk);
}

void Dictionary::Add(const size_t ID, const std::pair<Chunk, Chunk> chunkPair) {
    Add(ID, chunkPair.first);
    Add(ID, chunkPair.second);
}

void Dictionary::ClearSet(const size_t ID) {
    if ( dictionary.count(ID) == 0 ) {
        /* Set does not exist, nothing to clear */
        return;
    }

    dictionary[ID].clear();
}

/* Gets a random chunk from the specified set */
Chunk Dictionary::GetRandomFromSet(const size_t ID) const {
    if ( dictionary.count(ID) == 0 || dictionary.at(ID).empty() == true ) {
        /* Set does not exist or is empty, return empty chunk */
        return Chunk();
    }

    /* Select a random element from the set */
    dictionary_entry_t::const_iterator it( dictionary.at(ID).begin() );
    std::advance(it, Rand->Get(dictionary.at(ID).size()));

    return *it;
}

/* Gets a random chunk from a random set */
Chunk Dictionary::GetRandom(void) const {
    if ( dictionary.empty() == false ) {
        /* Select a random set */
        dictionary_t::const_iterator it = dictionary.begin();
        std::advance(it, Rand->Get(dictionary.size()));

        return GetRandomFromSet(it->first);
    } else {
        /* Dictionary is empty */
        return Chunk();
    }
}

} /* namespace dictionary */
} /* namespace vfuzz */
