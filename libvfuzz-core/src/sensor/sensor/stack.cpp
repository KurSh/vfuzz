#include <sensor/sensor/stack.h>

namespace vfuzz {
namespace sensor {

StackSensor::StackSensor(const SensorID ID) :
    BaseSensor(ID),
    stackDepthRecord(0), stackDepthBase(0)
{
}

size_t StackSensor::delta(void) const
{
    return stackDepthRecord - stackDepthBase;
}

void StackSensor::Update(void)
{
    if ( isEffectivelyEnabled() == false ) return;
    size_t SP;
    /* TODO use __builtin_return_address */
    asm("movq %%rsp,%0" : "=r"(SP));

    const size_t p = 0x8000000000000000 - SP;
    if ( p > stackDepthRecord ) {
        stackDepthRecord = p;
        if ( stackDepthBase == 0 ) {
            stackDepthBase = p;
        }
        propagate(delta());
    }
}

enum BaseSensor::BuiltinType StackSensor::GetBuiltinType(void) const
{
    return SENSOR_BUILTIN_TYPE_STACK;
}

} /* namespace sensor */
} /* namespace vfuzz */
