#include <sensor/processor/unique.h>

namespace vfuzz {
namespace sensor {

ProcessorUnique::ProcessorUnique(std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    BaseProcessor(loggerChain)
{
}

void ProcessorUnique::receiveInput(const SensorID ID, const Value data)
{
    const auto prevNumUniqueVals = uniqueVals.size();
    uniqueVals.insert(data);
    const auto curNumUniqueVals = uniqueVals.size();
    if ( curNumUniqueVals > prevNumUniqueVals ) {
        onAddVal((Value)curNumUniqueVals);
        onNew(ID, (Value)prevNumUniqueVals, (Value)curNumUniqueVals);
    }
}

} /* namespace sensor */
} /* namespace vfuzz */
