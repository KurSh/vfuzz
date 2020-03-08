#pragma once

#include <sensor/processor/base.h>

namespace vfuzz {
namespace sensor {

class ProcessorHighest : public BaseProcessor
{
    private:
        Value highest;
        void receiveInput(const SensorID ID, const Value data) override;
    public:
        ProcessorHighest(std::optional<std::shared_ptr<LoggerChain>> loggerChain = std::nullopt);
};

} /* namespace sensor */
} /* namespace vfuzz */
