#pragma once

#include <sensor/sensor/pc.h>
#include <sensor/sensor/auto/autosensor.h>
#include <map>

namespace vfuzz {
namespace sensor {

class AutoCodeIntensitySensor : public PCSensor, public AutoSensor
{
    private:
        std::map<size_t, size_t> PCToIntensity;
        void start(void) override;
    public:
        AutoCodeIntensitySensor(const SensorID ID);
        void Update(const size_t PC) override;
        std::string getSensorNamePrefix(void) const override;
};

} /* namespace sensor */
} /* namespace vfuzz */
