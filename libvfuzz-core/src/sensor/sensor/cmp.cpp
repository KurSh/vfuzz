#include <sensor/sensor/cmp.h>

namespace vfuzz {
namespace sensor {

enum BaseSensor::BuiltinType CMPSensor::GetBuiltinType(void) const
{
    return SENSOR_BUILTIN_TYPE_CMP;
}

CMPSensor::CMPSensor(const SensorID ID) :
    BaseSensor(ID)
{
}

} /* namespace sensor */
} /* namespace vfuzz */
