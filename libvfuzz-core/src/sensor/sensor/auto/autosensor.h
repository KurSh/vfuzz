#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <base/types.h>

namespace vfuzz {
namespace sensor {

class AutoSensor {
    protected:
        std::map<size_t, bool> pcSeen;
        virtual std::string getSensorNamePrefix(void) const = 0;
        void setAutoSensorName(const SensorID ID, const size_t PC);
};

} /* namespace sensor */
} /* namespace vfuzz */
