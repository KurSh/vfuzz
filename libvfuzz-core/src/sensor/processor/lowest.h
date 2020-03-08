#pragma once

#include <sensor/processor/base.h>

namespace vfuzz {
namespace sensor {

class ProcessorLowest : public BaseProcessor
{
    private:
        Value lowest;
        bool lowestSet;
        void receiveInput(const SensorID ID, const Value data) override;
    public:
        ProcessorLowest(std::optional<std::shared_ptr<LoggerChain>> loggerChain = std::nullopt);
};

} /* namespace sensor */
} /* namespace vfuzz */
