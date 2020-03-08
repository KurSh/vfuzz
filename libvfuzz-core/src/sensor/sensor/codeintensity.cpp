#include <sensor/sensor/codeintensity.h>

namespace vfuzz {
namespace sensor {

CodeIntensitySensor::CodeIntensitySensor(const SensorID ID) :
    PCSensor(ID), val(0)
{
}

void CodeIntensitySensor::start(void) {
    val = 0;
}

void CodeIntensitySensor::Update(const size_t PC)
{
    (void)PC;
    if ( isEffectivelyEnabled() == false ) return;
    propagate(++val);
}

} /* namespace sensor */
} /* namespace vfuzz */
