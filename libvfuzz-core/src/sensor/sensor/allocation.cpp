#include <sensor/sensor/allocation.h>

namespace vfuzz {
namespace sensor {

AllocationSensor::AllocationSensor(const SensorID ID) :
    BaseSensor(ID)
{
}

enum BaseSensor::BuiltinType AllocationSensor::GetBuiltinType(void) const
{
    return SENSOR_BUILTIN_TYPE_ALLOCATION;
}

} /* namespace sensor */
} /* namespace vfuzz */
