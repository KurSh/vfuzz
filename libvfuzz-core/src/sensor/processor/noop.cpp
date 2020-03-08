#include <sensor/processor/noop.h>

namespace vfuzz {
namespace sensor {

ProcessorNoop::ProcessorNoop(std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    BaseProcessor(loggerChain)
{
}

void ProcessorNoop::receiveInput(const SensorID ID, const Value data)
{
    (void)ID;
    onAddVal(data);
    /* TODO onNew? */
}

} /* namespace sensor */
} /* namespace vfuzz */
