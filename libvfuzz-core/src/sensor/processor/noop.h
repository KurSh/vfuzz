#pragma once

#include <sensor/processor/base.h>

namespace vfuzz {
namespace sensor {

class ProcessorNoop : public BaseProcessor
{
    private:
        void receiveInput(const SensorID ID, const Value data) override;
    public:
        ProcessorNoop(std::optional<std::shared_ptr<LoggerChain>> loggerChain = std::nullopt);
};

} /* namespace sensor */
} /* namespace vfuzz */
