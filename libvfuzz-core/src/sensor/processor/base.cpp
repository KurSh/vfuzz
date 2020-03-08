#include <sensor/processor/base.h>

namespace vfuzz {
namespace sensor {

void BaseProcessor::start(void)
{
}

void BaseProcessor::stop(void)
{
}

BaseProcessor::BaseProcessor(std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    disabled(false), loggerChain(loggerChain), onAddVal(std::bind(&BaseProcessor::defaultOnAddVal, this, std::placeholders::_1))
{
}

std::vector<Value> BaseProcessor::GetValues(void) const
{
    return vals;
}

size_t BaseProcessor::GetNumValues(void) const
{
    return vals.size();
}

void BaseProcessor::Start(void)
{
    vals.clear();
    start();
}

void BaseProcessor::Stop(void)
{
    stop();
}

void BaseProcessor::onNew(const SensorID ID, const Value prev, const Value cur) const
{
    if ( loggerChain != std::nullopt && *loggerChain != nullptr ) {
        (*loggerChain)->SensorUpdate(ID, name, prev, cur);
    }
}

void BaseProcessor::defaultOnAddVal(const Value data)
{
    vals.push_back(data);
}

void BaseProcessor::ReceiveInput(const SensorID ID, const Value data)
{
    if ( disabled == true ) {
        return;
    }
    receiveInput(ID, data);
}

void BaseProcessor::SetOnAddVal(const std::function<void(const Value data)> _onAddVal)
{
    onAddVal = _onAddVal;
}

void BaseProcessor::SetName(const std::string& name)
{
    this->name = name;
}

} /* namespace sensor */
} /* namespace vfuzz */
