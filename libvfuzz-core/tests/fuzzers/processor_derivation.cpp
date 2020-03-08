#include <fuzzing/datasource/datasource.hpp>

#include <base/exception.h>

#include <sensor/processor/highest.h>
#include <sensor/processor/lowest.h>
#include <sensor/processor/unique.h>
#include <sensor/processor/noop.h>
#include <sensor/processor/uniquemax.h>
#include <sensor/processor/sensor.h>

#include <sensor/sensor/custom.h>
#include <logger/consolelogger.h>

using namespace vfuzz;

constexpr size_t kMaxUniqueMax = 1024;

std::shared_ptr<sensor::BaseProcessor> getRandomProcessor(fuzzing::datasource::Datasource& ds, std::optional<std::shared_ptr<LoggerChain>> loggers) {
    const uint8_t which = ds.Get<uint8_t>();

    switch ( which ) {
        case 0:
            return std::make_shared<sensor::ProcessorHighest>(loggers);
        case 1:
            return std::make_shared<sensor::ProcessorLowest>(loggers);
        case 2:
            return std::make_shared<sensor::ProcessorUnique>(loggers);
        //case 3:
        //    return std::make_shared<sensor::ProcessorUniqueMax>(ds.Get<size_t>() % (kMaxUniqueMax+1) );
        default:
            return std::make_shared<sensor::ProcessorNoop>(loggers);
    }
}


std::shared_ptr<sensor::BaseSensor> getRandomSensor(
        fuzzing::datasource::Datasource& ds,
        std::vector<std::shared_ptr<sensor::CustomSensor>>& sensors,
        std::optional<std::shared_ptr<LoggerChain>> loggers = std::nullopt,
        const size_t depth = 0) {

    if ( depth > 16 ) {
        throw std::runtime_error("max depth");
    }

    std::shared_ptr<sensor::BaseSensor> ret;

    const uint8_t which = ds.Get<uint8_t>();

    switch ( which ) {
        case 0:
            {
                auto sensorData = std::make_shared<sensor::BaseData>();

                auto s = getRandomSensor(ds, sensors, loggers, depth + 1);
                std::shared_ptr<sensor::BaseSensor> s2 = nullptr;
                if ( ds.Get<bool>() == true ) {
                    s2 = getRandomSensor(ds, sensors, loggers, depth + 1);
                }

                auto p = getRandomProcessor(ds, loggers);
                ret = std::make_shared<sensor::SensorProcessor>(
                        ds.Get<SensorID>(),
                        sensorData,
                        p,
                        s,
                        s2);
            }
        default:
            ret = std::make_shared<sensor::CustomSensor>(ds.Get<SensorID>());
            //sensors.push_back(ret);
    }

    return ret;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    fuzzing::datasource::Datasource ds(data, size);

    auto loggers = std::make_shared<LoggerChain>();
    auto consolelogger = std::make_shared<ConsoleLogger>();
    loggers->AddLogger(consolelogger);
    consolelogger->Start();
    try {
        std::vector<std::shared_ptr<sensor::CustomSensor>> sensors;
        auto s = getRandomSensor(ds, sensors, loggers);
        /* UAF */
        while ( ds.Get<bool>() ) {
            if ( !sensors.empty() ) {
                const size_t sensorIndex = ds.Get<size_t>() % sensors.size();
                sensors[sensorIndex]->Update(ds.Get<Value>());
            }
        }
    } catch ( fuzzing::datasource::Base::OutOfData ) { }
      catch ( std::runtime_error ) { }
    consolelogger->Stop();

    return 0;
}
