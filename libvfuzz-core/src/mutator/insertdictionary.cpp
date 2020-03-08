#include <mutator/insertdictionary.h>
#include <util/string/string.h>
#include <dictionary/dictionary.h>
#include <dictionary/autodict.h>
#include <container/corpus/corpus.h>

namespace vfuzz {
namespace mutator {

template <class T>
InsertDictionary<T>::InsertDictionary(std::shared_ptr<util::Random> Rand, const util::Conduit<T> dictionary, const std::string name, const bool replace, std::optional<History*> history) :
    Base(Rand, (boost::format("InsertDictionary (%s)") % name).str(), history), dictionary(dictionary), replace(replace)
    { }

template <class T>
Chunk InsertDictionary<T>::MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
    Chunk ret;

    if ( replace == true ) {
        ret = dictionary.Get(&gid);
        insert = ret;

        LogHistory(gid, util::string::ToHexString(insert));

        return ret;
    } else {
        ret = input;
    }

    const size_t maxInsertions = maxChunkLen - input.size();
    if ( maxInsertions == 0 ) {
        return ret;
    }

    const Chunk toInsert = dictionary.Get(&gid);

    if ( toInsert.size() > maxInsertions ) {
        return ret;
    }

    if ( toInsert.empty() == true ) {
        return ret;
    }

    const size_t pos = Rand->Get(ret.size());
    for (size_t i = 0; i < toInsert.size(); i++) {
        ret.insert( ret.begin() + pos, toInsert[i] );
    }

    insert = toInsert;

    LogHistory(gid, insert);

    return ret;
}

/* Explicit instantiations */
template class InsertDictionary<std::shared_ptr<dictionary::Dictionary> >;
template class InsertDictionary<std::shared_ptr<container::corpus::Corpus> >;
template class InsertDictionary<std::shared_ptr<dictionary::AutoDict> >;

} /* namespace mutator */
} /* namespace vfuzz */
