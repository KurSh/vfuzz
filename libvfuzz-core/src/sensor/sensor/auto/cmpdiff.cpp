#include <sensor/sensor/auto/cmpdiff.h>
#include <vfuzz/vfuzz.h>
#include <datasource/id.h>

namespace vfuzz {
namespace sensor {

CMPDiffSensor::CMPDiffSensor(const SensorID ID) :
    CMPSensor(ID)
{
}

void CMPDiffSensor::Update(const size_t PC, size_t A, size_t B)
{
    if ( isEffectivelyEnabled() == false ) return;
    const size_t newID = PC + fuzzing::datasource::ID("CMPDiffSensor");
    setAutoSensorName(newID, PC);

    size_t diff = (A > B) ? (A - B) : (B - A);
    sensor_callback(
            newID,
            diff,
            vfuzz::kProcessorTypeLowest);
}

std::string CMPDiffSensor::getSensorNamePrefix(void) const
{
    return "CMPDiff";
}

} /* namespace sensor */
} /* namespace vfuzz */
