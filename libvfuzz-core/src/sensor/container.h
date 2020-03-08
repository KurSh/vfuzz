#pragma once

#include <map>
#include <set>
#include <memory>
#include <utility>
#include <sensor/sensor/base.h>
#include <sensor/sensor/pc.h>
#include <sensor/sensor/custom.h>
#include <sensor/sensor/stack.h>
#include <sensor/sensor/allocation.h>
#include <sensor/sensor/valueprofile.h>
#include <sensor/sensor/flowtrace.h>
#include <sensor/sensor/auto/cmpdiff.h>
#include <sensor/processor/sensor.h>
#include <logger/loggerchain.h>

namespace vfuzz {
namespace sensor {

class Container {
    private:
        std::shared_ptr<BaseData> sensorData;
        std::optional<std::shared_ptr<LoggerChain>> loggerChain;
        std::map<SensorID, std::shared_ptr<SensorProcessor>> IDToSensorProcessor;
        std::map<SensorID, std::shared_ptr<CustomSensor>> IDToCustomSensor;
        std::map<BaseSensor::BuiltinType, std::set<std::shared_ptr<BaseSensor>>> builtinTypeToSensors;
        std::map<SensorID, ProcessorType> customToProcessorType;
        std::map<SensorID, std::string> pending_SetSensorName;
        std::map<SensorID, std::tuple<SensorID, SensorID, SensorID, ProcessorType>> pending_CombineSensors;
        std::set<std::shared_ptr<BaseSensor>> createdSensors;

        std::vector<std::shared_ptr<PCSensor>> PCSensors;
        std::vector<std::shared_ptr<StackSensor>> StackSensors;
        std::vector<std::shared_ptr<FlowtraceSensor>> FlowtraceSensors;
        std::vector<std::shared_ptr<AllocationSensor>> AllocationSensors;
        std::vector<std::shared_ptr<ValueProfileSensor>> ValueProfileSensors;
        std::vector<std::shared_ptr<CMPSensor>> CMPSensors;

        bool haveID(const SensorID ID) const;

        template <class T> void addCustomIfNotExists_helper(const SensorID ID, const ProcessorType processorType);
        void setPendingSensorName(const SensorID ID);
    public:
        Container(std::shared_ptr<BaseData> sensorData, std::optional<std::shared_ptr<LoggerChain>> loggerChain);
        ~Container() = default;

        void AddSensorProcessor(const SensorID ID, std::shared_ptr<BaseProcessor> processor, std::shared_ptr<BaseSensor> sensor1 = nullptr, std::shared_ptr<BaseSensor> sensor2 = nullptr);
        //std::set<BaseSensor*> GetSensorsForBuiltin(const BaseSensor::BuiltinType builtinType) const;
        void addSensor(std::shared_ptr<BaseSensor> sensor);
        template <class Callback> void ForEachPCSensor(Callback CB) const
        {
            for ( const auto& sensor : PCSensors ) {
                CB(sensor);
            }
        }
        template <class Callback> void ForEachStackSensor(Callback CB) const
        {
            for ( const auto& sensor : StackSensors ) {
                CB(sensor);
            }
        }
        template <class Callback> void ForEachFlowtraceSensor(Callback CB) const
        {
            for ( const auto& sensor : FlowtraceSensors ) {
                CB(sensor);
            }
        }
        template <class Callback> void ForEachAllocationSensor(Callback CB) const
        {
            for ( const auto& sensor : AllocationSensors ) {
                CB(sensor);
            }
        }
        template <class Callback> void ForEachValueProfileSensor(Callback CB) const
        {
            for ( const auto& sensor : ValueProfileSensors ) {
                CB(sensor);
            }
        }
        template <class Callback> void ForEachCMPSensor(Callback CB) const
        {
            for ( const auto& sensor : CMPSensors ) {
                CB(sensor);
            }
        }
        template <class Callback> void SensorsForBuiltinForEach(const BaseSensor::BuiltinType builtinType, Callback CB) const
        {
            if ( builtinTypeToSensors.count(builtinType) == 0 ) {
                return;
            }

            for ( const auto sensor : builtinTypeToSensors.at(builtinType) ) {
                CB(sensor);
            }
        }
        std::shared_ptr<CustomSensor> AddCustomIfNotExists(const SensorID ID, const ProcessorType processorType);
        void Enable(const SensorID ID);
        void Disable(const SensorID ID);
        void Start(void);
        void Stop(void);
        void SetSensorName(const SensorID ID, const std::string name);
        void CombineSensors(const SensorID ID, const SensorID ID1, const SensorID ID2, const ProcessorType processorType);
        bool SensorWasUpdated(const SensorID ID) const;
        size_t SensorGetValue(const SensorID ID) const;

        /* TODO add support for deleting SensorProcessors */
};

} /* namespace sensor */
} /* namespace vfuzz */
