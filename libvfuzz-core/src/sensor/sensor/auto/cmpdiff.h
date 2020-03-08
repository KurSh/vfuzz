#pragma once

#include <sensor/sensor/cmp.h>
#include <sensor/sensor/auto/autosensor.h>
#include <sensor/processor/lowest.h>

namespace vfuzz {
namespace sensor {

class CMPDiffSensor : public CMPSensor, public AutoSensor
{
    public:
        CMPDiffSensor(const SensorID ID);
        void Update(const size_t PC, size_t A, size_t B) override;
        std::string getSensorNamePrefix(void) const override;
};

} /* namespace sensor */
} /* namespace vfuzz */
