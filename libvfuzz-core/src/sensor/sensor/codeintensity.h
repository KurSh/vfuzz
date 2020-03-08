#pragma once

#include <sensor/sensor/pc.h>

namespace vfuzz {
namespace sensor {

class CodeIntensitySensor : public PCSensor
{
    private:
        size_t val;
        void start(void) override;
    public:
        CodeIntensitySensor(const SensorID ID);
        void Update(const size_t PC) override;
};

} /* namespace sensor */
} /* namespace vfuzz */
