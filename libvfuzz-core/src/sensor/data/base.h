#pragma once

#include <base/types.h>

namespace vfuzz {
namespace sensor {

class BaseData {
    public:
        BaseData(void);
        virtual ~BaseData();
        virtual void ReceiveInput(const SensorID ID, const Value data) { (void)ID; (void)data; }
        virtual void SetSensorName(const SensorID ID, const char* name) { (void)ID; (void)name; }
};

} /* namespace sensor */
} /* namespace vfuzz */
