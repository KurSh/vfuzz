#pragma once

#include <sensor/sensor/base.h>

namespace vfuzz {
namespace sensor {

class CustomSensor : public BaseSensor
{
    public:
        CustomSensor(const SensorID ID);
        void Update(Value data);
};

} /* namespace sensor */
} /* namespace vfuzz */
