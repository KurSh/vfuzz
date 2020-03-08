#pragma once

#include <memory>
#include <sensor/sensor/allocation.h>
#include <util/allocationtracker.h>

namespace vfuzz {
namespace sensor {

class AllocationGlobalMaxSensor : public AllocationSensor
{
    private:
        std::unique_ptr<AllocationTracker> allocationTracker;
        void start(void) override;
        void stop(void) override;
    public:
        AllocationGlobalMaxSensor(const SensorID ID);
        ~AllocationGlobalMaxSensor() = default;
        void AddMalloc(const size_t PC, const void* ptr, const size_t size) override;
        void AddFree(const size_t PC, const void* ptr) override;
};

} /* namespace sensor */
} /* namespace vfuzz */
