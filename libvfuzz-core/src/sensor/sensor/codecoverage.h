#pragma once

#include <sensor/sensor/pc.h>

namespace vfuzz {
namespace sensor {

class CodeCoverageSensor : public PCSensor
{
    public:
        CodeCoverageSensor(const SensorID ID);
        void Update(const size_t PC) override;
};

} /* namespace sensor */
} /* namespace vfuzz */
