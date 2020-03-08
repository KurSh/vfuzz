#include <sensor/sensor/auto/autocodeintensity.h>
#include <vfuzz/vfuzz.h>

namespace vfuzz {
namespace sensor {

AutoCodeIntensitySensor::AutoCodeIntensitySensor(const SensorID ID) :
    PCSensor(ID)
{
}

void AutoCodeIntensitySensor::start(void) {
    PCToIntensity.clear();
}

void AutoCodeIntensitySensor::Update(const size_t PC)
{
    if ( isEffectivelyEnabled() == false ) return;
    const size_t newID = PC + fuzzing::datasource::ID("AutoCodeIntensitySensor");
    setAutoSensorName(newID, PC);

    if ( PCToIntensity.count(PC) == 0 ) {
        PCToIntensity[PC] = 0;
    }
    PCToIntensity[PC]++;

    sensor_callback(
            newID,
            PCToIntensity.at(PC),
            vfuzz::kProcessorTypeHighest);
}

std::string AutoCodeIntensitySensor::getSensorNamePrefix(void) const
{
    return "AutoCodeIntensity";
}

} /* namespace sensor */
} /* namespace vfuzz */
