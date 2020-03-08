#include <fuzzing/datasource/datasource.hpp>

#include <base/exception.h>

#include <sensor/processor/highest.h>
#include <sensor/processor/lowest.h>
#include <sensor/processor/unique.h>
#include <sensor/processor/noop.h>
#include <sensor/processor/uniquemax.h>

using namespace vfuzz;

constexpr size_t kMaxUniqueMax = 1024;

template <class PT>
void assertProcessorValues(PT& p, const std::vector<Value>& insertedValues) {
    const auto processorValues = p.GetValues();

    for (const auto &v : processorValues) {
        if ( std::find(insertedValues.begin(), insertedValues.end(), v) == insertedValues.end() ) {
            printf("Found value %zu but it was not inserted\n", v);
            abort();
        }
    }
}

template <>
void assertProcessorValues<>(sensor::ProcessorUnique& p, const std::vector<Value>& insertedValues) {
    (void)p;
    (void)insertedValues;
}

template <>
void assertProcessorValues<>(sensor::ProcessorUniqueMax& p, const std::vector<Value>& insertedValues) {
    (void)p;
    (void)insertedValues;
}

template <class ProcessorType>
void testProcessor(fuzzing::datasource::Datasource& ds, std::unique_ptr<ProcessorType> p) {
    //ProcessorType p(nullptr);

    std::vector<Value> insertedValues;

    while ( ds.Get<bool>() ) {
        const auto v = ds.Get<Value>();

        p->ReceiveInput(ds.Get<SensorID>(), v, nullptr);

        insertedValues.push_back(v);
    }

    if ( p->GetNumValues() > insertedValues.size() ) abort();

    assertProcessorValues(*p, insertedValues);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    fuzzing::datasource::Datasource ds(data, size);

    try {
        testProcessor<>(ds, std::make_unique<sensor::ProcessorHighest>());
        testProcessor<>(ds, std::make_unique<sensor::ProcessorLowest>());
        testProcessor<>(ds, std::make_unique<sensor::ProcessorUnique>());
        testProcessor<>(ds, std::make_unique<sensor::ProcessorNoop>());
        try {
            testProcessor<>(ds, std::make_unique<sensor::ProcessorUniqueMax>(ds.Get<size_t>() % (kMaxUniqueMax+1) ));
        } catch ( vfuzz::Exception ) { }
    } catch ( fuzzing::datasource::Base::OutOfData ) { }


    return 0;
}
