#include <sensor/sensor/auto/allocationsinglemax.h>
#include <vfuzz/vfuzz.h>
#include <datasource/id.h>

namespace vfuzz {
namespace sensor {

AllocationSingleMaxSensor::AllocationSingleMaxSensor(const SensorID ID) :
    AllocationSensor(ID)
{
}

void AllocationSingleMaxSensor::AddMalloc(const size_t PC, const void* ptr, const size_t size)
{
    (void)ptr;

    if ( isEffectivelyEnabled() == false ) return;
    const size_t newID = PC + fuzzing::datasource::ID("AllocationSingleMaxSensor");
    setAutoSensorName(newID, PC);

    sensor_callback(
            newID,
            size,
            vfuzz::kProcessorTypeHighest);
}

void AllocationSingleMaxSensor::AddFree(const size_t PC, const void* ptr)
{
    (void)PC;
    (void)ptr;
}

std::string AllocationSingleMaxSensor::getSensorNamePrefix(void) const
{
    return "AllocationSingleMax";
}

} /* namespace sensor */
} /* namespace vfuzz */
