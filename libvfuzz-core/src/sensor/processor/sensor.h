#pragma once

#include <sensor/sensor/base.h>
#include <sensor/processor/base.h>
#include <memory>
#include <string>
#include <cstdint>

namespace vfuzz {
namespace sensor {

class SensorProcessor : public BaseSensor
{
    private:
        std::vector<std::shared_ptr<BaseSensor>> sensors;
        std::shared_ptr<BaseProcessor> processor;
        std::vector<Value> sensorDataSensor1;
        std::vector<Value> sensorDataSensor2;
        void internalReceiveInput(const SensorID ID, const Value data);
        void addSensor(std::shared_ptr<BaseSensor> sensor);
        bool disabled;
        void processorOnAddVal(const Value data);
        SensorID firstSensorID;
    public:
        SensorProcessor(const SensorID ID, std::shared_ptr<BaseData> sensorData, std::shared_ptr<BaseProcessor> processor, std::shared_ptr<BaseSensor> sensor1 = nullptr, std::shared_ptr<BaseSensor> sensor2 = nullptr);
        ~SensorProcessor() = default;

        void Enable(void);
        void Disable(void);
        void Start(void);
        void Stop(void);
        void SetName(const std::string name);
        void Subordinate(void);
};

} /* namespace sensor */
} /* namespace vfuzz */
