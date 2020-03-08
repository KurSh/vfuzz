#include <stdint.h>
#include <stddef.h>
#include <cstring>
#include <vector>
#include <functional>

#include <mutator/mutator.h>
#include <mutator/random.h>
#include <mutator/insertbyte.h>
#include <mutator/erasebytes.h>
#include <mutator/insertrepeatedbytes.h>
#include <mutator/copypart.h>
#include <mutator/shufflebytes.h>
#include <mutator/crossover.h>
#include <mutator/copy.h>
#include <mutator/range_based.h>

#include <container/corpus/corpus.h>
#include <util/random.h>
#include <util/conduit.h>
#include <fuzzing/datasource/datasource.hpp>

using namespace vfuzz;

/* Assert head-insert-tail, so:
 *      dataPost = [head of dataPre + inserted + tail of dataPre]
 */
static void AssertHIT(const Chunk& dataPre, const Chunk& dataPost, const Chunk& inserted, const bool resized) {
    if ( inserted.empty() == false ) {
        bool found = false;
        size_t curInsertPos = 0;
        while ( curInsertPos < dataPost.size() ) {
            /* Find 'inserted' in 'dataPost' */
            auto it = std::search(dataPost.begin() + curInsertPos, dataPost.end(), std::boyer_moore_searcher(inserted.begin(), inserted.end()));

            if ( it == dataPost.end() ) {
                /* It should always be found */
                abort();
            } else {
                curInsertPos = it - dataPost.begin();

                /* Reconstruct: head of (dataPre + inserted + tail of dataPre) should be dataPost */

                std::vector<uint8_t> reconstructed;
                reconstructed.insert(reconstructed.end(), dataPre.begin(), dataPre.begin() + curInsertPos);
                reconstructed.insert(reconstructed.end(), inserted.begin(), inserted.end());
                if ( resized == false ) {
                    reconstructed.insert(reconstructed.end(), dataPre.begin() + curInsertPos + inserted.size(), dataPre.end());
                } else {
                    reconstructed.insert(reconstructed.end(), dataPre.begin() + curInsertPos, dataPre.end());
                }

                if ( reconstructed == dataPost ) {
                    found = true;
                    break;
                }
            }

            curInsertPos++;
        }

        if ( found == false ) {
            abort();
        }
    }
}

class TestMutator : public mutator::Base {
    private:
        fuzzing::datasource::Datasource& ds;

        void TestRandomRange(void) {
            const auto data = ds.GetData(0);
            const auto range = RandomRange(data.size());

            if ( range.first > data.size() ) {
                abort();
            }

            if ( range.first + range.second > data.size() ) {
                abort();
            }
        }

        void TestRandomPos(void) {
            const auto data = ds.GetData(0);
            const auto pos = RandomPos(data.size());
            if ( pos > data.size() ) {
                abort();
            }
        }

        void TestCopyPartOf(void) {
            const auto dataFrom = ds.GetData(0);
            auto dataTo = ds.GetData(0);
            const auto dataToOriginal = dataTo;

            Chunk inserted;

            CopyPartOf(dataTo, dataFrom, inserted);

            AssertHIT(dataToOriginal, dataTo, inserted, false);
        }

        void TestInsertPartOf(void) {
            const auto dataFrom = ds.GetData(0);
            auto dataTo = ds.GetData(0);
            const auto dataToOriginal = dataTo;
            const auto maxToSize = ds.Get<uint64_t>() % (1024*1024*10);

            Chunk inserted;

            InsertPartOf(dataTo, dataFrom, maxToSize, inserted);

            AssertHIT(dataToOriginal, dataTo, inserted, true);
        }
    protected:
        Chunk MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) override {
            return input;
        }
    public:
        TestMutator(std::shared_ptr<util::Random> Rand, fuzzing::datasource::Datasource& ds) :
            Base(Rand, "TestMutator", {}),
            ds(ds)
        { }

        void Test(void) {
            const uint8_t which = ds.Get<uint8_t>();

            switch ( which ) {
                case    0:
                    TestRandomRange();
                    break;
                case    1:
                    TestRandomPos();
                    break;
                case    2:
                    TestCopyPartOf();
                    break;
                case    3:
                    TestInsertPartOf();
                    break;
            }
        }
};

static void TestTestMutator(fuzzing::datasource::Datasource& ds) {
    auto Rand = std::make_shared<util::Random>(ds.Get<uint64_t>());
    TestMutator tm(Rand, ds);
    tm.Test();
}

/* TODO overload DS.Get */
std::shared_ptr<container::Input> GetInput(std::shared_ptr<util::Random> Rand, fuzzing::datasource::Datasource& ds) {
    auto input = std::make_shared<container::Input>(Rand);

    while ( ds.Get<bool>(true) ) {
        const auto data = ds.GetData(0);
        input->Add(data);
    }

    return input;
}

/* TODO overload DS.Get */
std::shared_ptr<container::InputCluster> GetInputCluster(std::shared_ptr<util::Random> Rand, fuzzing::datasource::Datasource& ds) {
    auto ic = std::make_shared<container::InputCluster>(Rand, 100, 100);
    while ( ds.Get<bool>(true) ) {
        ic->Set(ds.Get<uint64_t>(), GetInput(Rand, ds));
    }
    return ic;
}

/* TODO overload DS.Get */
std::shared_ptr<container::corpus::Corpus> GetCorpus(std::shared_ptr<util::Random> Rand, fuzzing::datasource::Datasource& ds) {
    container::InputCluster ic(Rand, 4096, 4096);
    auto corpus = std::make_shared<container::corpus::Corpus>(Rand, ic);


    while ( ds.Get<bool>(true) ) {
    }

    return corpus;
}

template <class T>
void MutatorTester(std::shared_ptr<T> mutator, fuzzing::datasource::Datasource& ds) {
    const auto input = ds.GetData(0);
    const auto maxChunkLen = ds.Get<uint64_t>() % ((input.size() * 3) + 1);
    Chunk insert;
    const auto mutated = mutator->Mutate(
            ds.Get<uint64_t>() /* generator ID */,
            input,
            insert,
            maxChunkLen);

    if ( input.size() > maxChunkLen ) {
    } else {
        if ( mutated.size() > maxChunkLen ) {
            abort();
        }
    }
}

template <class T>
void MutatorTester(fuzzing::datasource::Datasource& ds) {
    auto Rand = std::make_shared<util::Random>(ds.Get<uint64_t>());
    auto mutator = std::make_shared<T>(Rand);
    MutatorTester<T>(mutator, ds);
}

template <class T>
void MutatorTesterCorpusBased(fuzzing::datasource::Datasource& ds) {
    auto Rand = std::make_shared<util::Random>(ds.Get<uint64_t>());
    auto corpus = GetCorpus(Rand, ds);
    auto conduit = util::Conduit<std::shared_ptr<container::corpus::Corpus>>(corpus, [](const std::shared_ptr<container::corpus::Corpus> corpus, const void* param) -> Chunk {
            const auto IC = corpus->GetRandomPtr();
            const auto I = IC->Get( *(static_cast<const size_t*>(param)) );
            return I->GetRandom();
            });
    auto mutator = std::make_shared<T>(Rand, conduit);
    MutatorTester<>(mutator, ds);
}

static void TestMutator(fuzzing::datasource::Datasource& ds) {
    const uint8_t which = ds.Get<uint8_t>();

    switch ( which ) {
        case 0:
            MutatorTester<mutator::Random>(ds);
            break;
        case 1:
            MutatorTester<mutator::InsertByte>(ds);
            break;
        case 2:
            MutatorTester<mutator::EraseBytes>(ds);
            break;
        case 3:
            MutatorTester<mutator::InsertRepeatedBytes>(ds);
            break;
        case 4:
            MutatorTester<mutator::CopyPart>(ds);
            break;
        case 5:
            MutatorTester<mutator::ShuffleBytes>(ds);
            break;
        case 6:
            MutatorTesterCorpusBased<mutator::CrossOver<std::shared_ptr<container::corpus::Corpus>>>(ds);
            break;
        case 7:
            MutatorTesterCorpusBased<mutator::Copy<std::shared_ptr<container::corpus::Corpus>>>(ds);
            break;
        case 8:
            MutatorTester<mutator::And<false>>(ds);
            break;
        case 9:
            MutatorTester<mutator::And<true>>(ds);
            break;
        case 10:
            MutatorTester<mutator::Or<false>>(ds);
            break;
        case 11:
            MutatorTester<mutator::Or<true>>(ds);
            break;
        case 12:
            MutatorTester<mutator::Xor<false>>(ds);
            break;
        case 13:
            MutatorTester<mutator::Xor<true>>(ds);
            break;
        case 14:
            MutatorTester<mutator::Not<false>>(ds);
            break;
        case 15:
            MutatorTester<mutator::Not<true>>(ds);
            break;
        case 16:
            MutatorTester<mutator::ShiftLeft<false>>(ds);
            break;
        case 17:
            MutatorTester<mutator::ShiftLeft<true>>(ds);
            break;
        case 18:
            MutatorTester<mutator::ShiftRight<false>>(ds);
            break;
        case 19:
            MutatorTester<mutator::ShiftRight<true>>(ds);
            break;
        case 20:
            MutatorTester<mutator::ChangeBit<false>>(ds);
            break;
        case 21:
            MutatorTester<mutator::ChangeBit<true>>(ds);
            break;
        case 22:
            MutatorTester<mutator::ChangeByte<false>>(ds);
            break;
        case 23:
            MutatorTester<mutator::ChangeByte<true>>(ds);
            break;
        case 24:
            MutatorTester<mutator::ReverseByte<false>>(ds);
            break;
        case 25:
            MutatorTester<mutator::ReverseByte<true>>(ds);
            break;
        /* TODO Insert Dictionary*/
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    fuzzing::datasource::Datasource ds(data, size);

    try {
        const uint8_t which = ds.Get<uint8_t>();

        switch ( which ) {
            case    0:
                TestTestMutator(ds);
                break;
            case    1:
                TestMutator(ds);
                break;
        }
    } catch ( fuzzing::datasource::Base::OutOfData ) { }

    return 0;
}
