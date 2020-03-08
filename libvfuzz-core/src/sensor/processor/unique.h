#pragma once

#include <sensor/processor/base.h>
#include <optional>
#include <cstdint>
#include <string>
#include <unordered_set>

namespace vfuzz {
namespace sensor {

class ProcessorUnique : public BaseProcessor
{
    private:
        std::unordered_set<Value> uniqueVals;
        void receiveInput(const SensorID ID, const Value data) override;
    public:
        ProcessorUnique(std::optional<std::shared_ptr<LoggerChain>> loggerChain = std::nullopt);
};

} /* namespace sensor */
} /* namespace vfuzz */
