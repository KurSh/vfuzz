#pragma once

#include <sensor/sensor/base.h>

namespace vfuzz {
namespace sensor {

class StackSensor : public BaseSensor
{
    private:
        size_t stackDepthRecord, stackDepthBase;
        size_t delta(void) const;
    public:
        StackSensor(const SensorID ID);
        void Update(void);
        enum BuiltinType GetBuiltinType(void) const override;
};

} /* namespace sensor */
} /* namespace vfuzz */
