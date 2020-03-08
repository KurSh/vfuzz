#include <sensor/sensor/codecoverage.h>

namespace vfuzz {
namespace sensor {

CodeCoverageSensor::CodeCoverageSensor(const SensorID ID) :
    PCSensor(ID)
{
}

void CodeCoverageSensor::Update(const size_t PC)
{
    if ( isEffectivelyEnabled() == false ) return;
    propagate(PC);
}

} /* namespace sensor */
} /* namespace vfuzz */
