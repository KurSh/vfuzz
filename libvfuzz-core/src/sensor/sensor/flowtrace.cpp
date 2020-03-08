#include <sensor/sensor/flowtrace.h>

namespace vfuzz {
namespace sensor {

FlowtraceSensor::FlowtraceSensor(const SensorID ID) :
    BaseSensor(ID)
{
}

void FlowtraceSensor::start(void)
{
    flowTracker.Reset();
}

void FlowtraceSensor::Update(const uintptr_t caller)
{
    if ( isEffectivelyEnabled() == false ) return;
    flowTracker.Add(caller);
    stackTraces.insert( flowTracker.GetTrack() );
    propagate(stackTraces.size());
}

enum BaseSensor::BuiltinType FlowtraceSensor::GetBuiltinType(void) const
{
    return SENSOR_BUILTIN_TYPE_STACK_TRACE;
}


} /* namespace sensor */
} /* namespace vfuzz */
