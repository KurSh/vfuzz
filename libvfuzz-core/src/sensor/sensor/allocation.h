#pragma once

#include <memory>
#include <sensor/sensor/base.h>

namespace vfuzz {
namespace sensor {

class AllocationSensor : public BaseSensor
{
    public:
        AllocationSensor(const SensorID ID);
        virtual ~AllocationSensor() = default;
        virtual void AddMalloc(const size_t PC, const void* ptr, const size_t size) = 0;
        virtual void AddFree(const size_t PC, const void* ptr) = 0;
        enum BuiltinType GetBuiltinType(void) const override;
};

} /* namespace sensor */
} /* namespace vfuzz */
