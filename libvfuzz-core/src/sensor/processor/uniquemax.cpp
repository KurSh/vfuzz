#include <sensor/processor/uniquemax.h>
#include <base/exception.h>

namespace vfuzz {
namespace sensor {

ProcessorUniqueMax::ProcessorUniqueMax(const size_t maxSize, std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    BaseProcessor(loggerChain), maxSize(maxSize), prevNumUniqueVals(0), uniqueVals(std::make_unique<size_t[]>(maxSize))
{
}

void ProcessorUniqueMax::receiveInput(const SensorID ID, const Value data)
{
    if ( data >= maxSize ) {
        throw vfuzz::Exception("ProcessorUniqueMax: input value larger than maxSize");
    }

    if ( uniqueVals[data] == 0 ) {
        uniqueVals[data] = 1;
        onAddVal((Value)(prevNumUniqueVals+1));
        onNew(ID, (Value)prevNumUniqueVals, (Value)(prevNumUniqueVals+1));
        prevNumUniqueVals++;
    }
}

} /* namespace sensor */
} /* namespace vfuzz */
