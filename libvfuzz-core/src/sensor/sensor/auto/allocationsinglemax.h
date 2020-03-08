#pragma once

#include <memory>
#include <sensor/sensor/auto/autosensor.h>
#include <sensor/sensor/allocation.h>

namespace vfuzz {
namespace sensor {

class AllocationSingleMaxSensor : public AllocationSensor, public AutoSensor
{
    public:
        AllocationSingleMaxSensor(const SensorID ID);
        ~AllocationSingleMaxSensor() = default;
        void AddMalloc(const size_t PC, const void* ptr, const size_t size) override;
        void AddFree(const size_t PC, const void* ptr) override;
        std::string getSensorNamePrefix(void) const override;
};

} /* namespace sensor */
} /* namespace vfuzz */
