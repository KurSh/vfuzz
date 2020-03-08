#include <sensor/sensor/allocationglobalmax.h>

namespace vfuzz {
namespace sensor {

void AllocationGlobalMaxSensor::start(void)
{
    allocationTracker->Reset();
    allocationTracker->Enable();
}

void AllocationGlobalMaxSensor::stop(void)
{
    allocationTracker->Disable();
    propagate(allocationTracker->GetMax());
}

AllocationGlobalMaxSensor::AllocationGlobalMaxSensor(const SensorID ID) :
    AllocationSensor(ID), allocationTracker(std::make_unique<AllocationTracker>())
{
}

void AllocationGlobalMaxSensor::AddMalloc(const size_t PC, const void* ptr, const size_t size)
{
    (void)PC;

    if ( isEffectivelyEnabled() == false ) return;
    allocationTracker->AddMalloc(ptr, size);
}

void AllocationGlobalMaxSensor::AddFree(const size_t PC, const void* ptr)
{
    (void)PC;

    if ( isEffectivelyEnabled() == false ) return;
    allocationTracker->AddFree(ptr);
}

} /* namespace sensor */
} /* namespace vfuzz */
