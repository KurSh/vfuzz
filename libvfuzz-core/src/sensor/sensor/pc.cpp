#include <sensor/sensor/pc.h>

namespace vfuzz {
namespace sensor {

enum BaseSensor::BuiltinType PCSensor::GetBuiltinType(void) const
{
    return SENSOR_BUILTIN_TYPE_PC;
}

PCSensor::PCSensor(const SensorID ID) :
    BaseSensor(ID)
{
}

} /* namespace sensor */
} /* namespace vfuzz */
