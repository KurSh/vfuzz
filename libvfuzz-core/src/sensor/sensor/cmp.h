#pragma once

#include <sensor/sensor/base.h>

namespace vfuzz {
namespace sensor {

class CMPSensor : public BaseSensor
{
    public:
        CMPSensor(const SensorID ID);
        enum BuiltinType GetBuiltinType(void) const override;
        virtual void Update(const size_t PC, size_t A, size_t B) {
            (void)PC;
            (void)A;
            (void)B;
        };
};

} /* namespace sensor */
} /* namespace vfuzz */
