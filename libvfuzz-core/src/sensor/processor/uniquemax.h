#pragma once

#include <sensor/processor/base.h>
#include <memory>

namespace vfuzz {
namespace sensor {

class ProcessorUniqueMax : public BaseProcessor
{
    private:
        const size_t maxSize;
        size_t prevNumUniqueVals;
        std::unique_ptr<size_t[]> uniqueVals;
        void receiveInput(const SensorID ID, const Value data) override;
    public:
        ProcessorUniqueMax(const size_t maxSize, std::optional<std::shared_ptr<LoggerChain>> loggerChain = std::nullopt);
        ~ProcessorUniqueMax() = default;
};

} /* namespace sensor */
} /* namespace vfuzz */
