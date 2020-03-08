#pragma once

#include <dictionary/base.h>
#include <util/random.h>
#include <base/types.h>
#include <set>
#include <memory>
#include <map>

namespace vfuzz {
namespace dictionary {

typedef enum {
    kDictionaryTypeMutator = 0,
    kDictionaryTypeMemcmp = 1,
    kDictionaryTypeCmp = 2,
} dictionary_type_t;

class Dictionary : public Base {
    private:
        using dictionary_entry_t = std::set<Chunk>;
        using dictionary_t = std::map<size_t, dictionary_entry_t>;

        dictionary_t dictionary;
        Chunk GetRandomFromSet(const size_t ID) const;
    public:
        Dictionary(std::shared_ptr<util::Random> Rand);
        void Add(const size_t ID, const Chunk chunk);
        void Add(const size_t ID, const std::pair<Chunk, Chunk> chunkPair);
        Chunk GetRandom(void) const override;
        void ClearSet(const size_t ID);
};

} /* namespace dictionary */
} /* namespace vfuzz */
