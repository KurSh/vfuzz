#include <sensor/processor/lowest.h>

namespace vfuzz {
namespace sensor {

ProcessorLowest::ProcessorLowest(std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    BaseProcessor(loggerChain), lowestSet(false)
{
}

void ProcessorLowest::receiveInput(const SensorID ID, const Value data)
{
    if ( lowestSet == false ) {
        onAddVal(data);
        onNew(ID, lowest, data);
        lowest = data;
        lowestSet = true;
    } else if ( data < lowest ) {
        onAddVal(data);
        onNew(ID, lowest, data);
        lowest = data;
    }
}

} /* namespace sensor */
} /* namespace vfuzz */
