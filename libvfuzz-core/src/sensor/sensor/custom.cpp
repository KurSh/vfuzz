#include <sensor/sensor/custom.h>

namespace vfuzz {
namespace sensor {

CustomSensor::CustomSensor(const SensorID ID) :
            BaseSensor(ID)
{
    /* TODO error if ID == 0 */
}

void CustomSensor::Update(const Value data)
{
    if ( isEffectivelyEnabled() == false ) return;
    if ( onReady != nullptr ) {
        onReady(GetID(), data);
    }
}

} /* namespace sensor */
} /* namespace vfuzz */
