#include <stdint.h>
#include <stddef.h>

#include <dictionary/dictionary.h>
#include <fuzzing/datasource/datasource.hpp>

using namespace vfuzz;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    fuzzing::datasource::Datasource ds(data, size);

    try {
        auto Rand = std::make_shared<util::Random>(ds.Get<uint64_t>());
        auto dict = std::make_shared<dictionary::Dictionary>(Rand);

        while ( ds.Get<bool>() ) {
            const uint8_t which = ds.Get<uint8_t>();
            switch ( which ) {
                case    0:
                    {
                        dict->Add(
                                ds.Get<uint64_t>(),
                                ds.GetData(0)
                                );
                    }
                    break;
                case    1:
                    {
                        dict->Add(
                                ds.Get<uint64_t>(),
                                std::pair<Chunk, Chunk>{ds.GetData(0), ds.GetData(0)}
                                );
                    }
                    break;
                case    2:
                    dict->GetRandom();
                    break;
                case    3:
                    dict->ClearSet(ds.Get<uint64_t>());
                    break;
            }
        }
    } catch ( fuzzing::datasource::Base::OutOfData ) { }

    return 0;
}
