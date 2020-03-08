#pragma once

#include <map>
#include <base/types.h>
#include <sensor/data/base.h>

namespace vfuzz {
namespace sensor {

class Data : public BaseData {
    public:
        Data(void);
        size_t increase;
        std::map<SensorID, size_t> increase_per_sensor;
        void ReceiveInput(const SensorID ID, const Value data) override;
        void SetSensorName(const SensorID ID, const char* name) override;
};

} /* namespace sensor */
} /* namespace vfuzz */
