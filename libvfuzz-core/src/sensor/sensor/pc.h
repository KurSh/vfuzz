#pragma once

#include <sensor/sensor/base.h>

namespace vfuzz {
namespace sensor {

class PCSensor : public BaseSensor
{
    public:
        PCSensor(const SensorID ID);
        enum BuiltinType GetBuiltinType(void) const override;
        virtual void Update() { };
        virtual void Update(const size_t PC) {
            (void)PC;
        };
};

} /* namespace sensor */
} /* namespace vfuzz */
