#include <sensor/processor/highest.h>

namespace vfuzz {
namespace sensor {

ProcessorHighest::ProcessorHighest(std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    BaseProcessor(loggerChain), highest(0)
{
}

void ProcessorHighest::receiveInput(const SensorID ID, const Value data)
{
    if ( data > highest ) {
        onAddVal(data);
        onNew(ID, highest, data);
        highest = data;
    }
}

} /* namespace sensor */
} /* namespace vfuzz */
