#include <sensor/container.h>
#include <tuple>
#include <sensor/processor/highest.h>
#include <sensor/processor/lowest.h>
#include <sensor/processor/noop.h>
#include <sensor/processor/unique.h>

namespace vfuzz {
namespace sensor {

Container::Container(std::shared_ptr<BaseData> sensorData, std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    sensorData(sensorData), loggerChain(loggerChain)
{
}

bool Container::haveID(const SensorID ID) const
{
    return IDToSensorProcessor.count(ID) > 0;
}

void Container::addSensor(std::shared_ptr<BaseSensor> sensor)
{
    if ( sensor == nullptr ) {
        return;
    }

    const auto builtinType = sensor->GetBuiltinType();

    switch ( builtinType ) {
        case    BaseSensor::SENSOR_BUILTIN_TYPE_PC:
            PCSensors.push_back(std::dynamic_pointer_cast<PCSensor>(sensor));
            break;
        case    BaseSensor::SENSOR_BUILTIN_TYPE_STACK:
            StackSensors.push_back(std::dynamic_pointer_cast<StackSensor>(sensor));
            break;
        case    BaseSensor::SENSOR_BUILTIN_TYPE_STACK_TRACE:
            FlowtraceSensors.push_back(std::dynamic_pointer_cast<FlowtraceSensor>(sensor));
            break;
        case    BaseSensor::SENSOR_BUILTIN_TYPE_ALLOCATION:
            AllocationSensors.push_back(std::dynamic_pointer_cast<AllocationSensor>(sensor));
            break;
        case    BaseSensor::SENSOR_BUILTIN_TYPE_VALUE_PROFILE:
            ValueProfileSensors.push_back(std::dynamic_pointer_cast<ValueProfileSensor>(sensor));
            break;
        case    BaseSensor::SENSOR_BUILTIN_TYPE_NONE:
            break;
        case    BaseSensor::SENSOR_BUILTIN_TYPE_CMP:
            //CMPSensors.push_back(std::dynamic_pointer_cast<CMPDiffSensor>(sensor));
            CMPSensors.push_back(std::dynamic_pointer_cast<CMPSensor>(sensor));
            break;
        default:
            throw std::runtime_error("addSensor: invalid builtin type");
    }

    builtinTypeToSensors[builtinType].insert(sensor);
}

void Container::AddSensorProcessor(const SensorID ID, std::shared_ptr<BaseProcessor> processor, std::shared_ptr<BaseSensor> sensor1, std::shared_ptr<BaseSensor> sensor2)
{
    if ( haveID(ID) == true ) {
        throw std::runtime_error("Attempted to add SensorProcessor with duplicate ID");
    }

    auto sensorProcessor = std::make_shared<SensorProcessor>(ID, sensorData, processor, sensor1, sensor2);

    createdSensors.insert(sensorProcessor);
    createdSensors.erase(sensor1);
    createdSensors.erase(sensor2);

    IDToSensorProcessor[ID] = sensorProcessor;

    addSensor(sensor1);
    addSensor(sensor2);

    if ( pending_SetSensorName.count(ID) ) {
        const std::string name = pending_SetSensorName[ID];
        pending_SetSensorName.erase(ID);
        SetSensorName(ID, name);
    }

}
/*
template <class Callback>
void Container::SensorsForBuiltinForEach(const BaseSensor::BuiltinType builtinType, Callback CB) const
//std::set<BaseSensor*> Container::SensorsForBuiltinForEach(const BaseSensor::BuiltinType builtinType, Callback CB) const
{
    if ( builtinTypeToSensors.count(builtinType) == 0 ) {
        return;
        //return std::set<BaseSensor*>();
    }

    for ( const auto sensor : builtinTypeToSensors.at(builtinType) ) {
        CB(sensor);
    }
}
*/

template <class T>
void Container::addCustomIfNotExists_helper(const SensorID ID, const ProcessorType processorType)
{
    const auto customSensor = std::make_shared<CustomSensor>(0);
    IDToCustomSensor[ID] = customSensor;
    customToProcessorType[ID] = processorType;
    AddSensorProcessor(ID, std::make_shared<T>(loggerChain), customSensor);
}

void Container::setPendingSensorName(const SensorID ID)
{
    if ( pending_SetSensorName.count(ID) > 0 ) {
        const std::string name = pending_SetSensorName[ID];
        pending_SetSensorName.erase(ID);
        SetSensorName(ID, name);
    }
}

std::shared_ptr<CustomSensor> Container::AddCustomIfNotExists(const SensorID ID, const ProcessorType processorType)
{
    if ( haveID(ID) == false ) {
        /* Does not exist */
        switch ( processorType ) {
            case    kProcessorTypeHighest:
                {
                    addCustomIfNotExists_helper<ProcessorHighest>(ID, processorType);
                }
                break;
            case    kProcessorTypeLowest:
                {
                    addCustomIfNotExists_helper<ProcessorLowest>(ID, processorType);
                }
                break;
            case    kProcessorTypeUnique:
                {
                    addCustomIfNotExists_helper<ProcessorUnique>(ID, processorType);
                }
                break;
            case    kProcessorTypeNoop:
                {
                    addCustomIfNotExists_helper<ProcessorNoop>(ID, processorType);
                }
                break;
            default:
                throw std::runtime_error("Invalid processor type");
        }
    } else {
        if ( customToProcessorType.count(ID) == 0 ) {
            /* should never happen */
            throw std::runtime_error("ID exists in IDToSensorProcessor but not in customToProcessorType. This is a bug in vfuzz.");
        }
        if ( customToProcessorType[ID] != processorType ) {
            throw std::runtime_error("Custom sensor already defined with different processor type");
        }
    }

    if ( pending_CombineSensors.count(ID) > 0 ) {
        CombineSensors(
                std::get<0>(pending_CombineSensors[ID]),
                std::get<1>(pending_CombineSensors[ID]),
                std::get<2>(pending_CombineSensors[ID]),
                std::get<3>(pending_CombineSensors[ID]) );
    }

    setPendingSensorName(ID);

    return IDToCustomSensor[ID];
}

void Container::Enable(const SensorID ID)
{
    if ( haveID(ID) == false ) {
        return; /* TODO throw? */
    }

    IDToSensorProcessor[ID]->Enable();
}

void Container::Disable(const SensorID ID)
{
    if ( haveID(ID) == false ) {
        return; /* TODO throw? */
    }

    IDToSensorProcessor[ID]->Disable();
}

void Container::Start(void)
{
    for (const auto& sp : IDToSensorProcessor) {
        sp.second->Start();
    }
}

void Container::Stop(void)
{
    for (const auto& sp : IDToSensorProcessor) {
        sp.second->Stop();
    }
}

void Container::SetSensorName(const SensorID ID, const std::string name)
{
    if ( haveID(ID) == false ) {
        /* Set name as soon as this ID becomes available */
        pending_SetSensorName[ID] = name;
    } else {
        IDToSensorProcessor[ID]->SetName(name);
    }
}

void Container::CombineSensors(const SensorID ID, const SensorID ID1, const SensorID ID2, const ProcessorType processorType)
{
    printf("try combine\n");
    if ( haveID(ID1) == false || haveID(ID2) == false ) {
        printf("pending combine sensors\n");
        pending_CombineSensors[ID1] = std::tuple<SensorID, SensorID, SensorID, ProcessorType>(ID, ID1, ID2, processorType);
        pending_CombineSensors[ID2] = std::tuple<SensorID, SensorID, SensorID, ProcessorType>(ID, ID1, ID2, processorType);
    } else {
        auto sensor1 = IDToSensorProcessor[ID1];
        auto sensor2 = IDToSensorProcessor[ID2];

        sensor1->Subordinate();
        sensor2->Subordinate();

        switch ( processorType ) {
            case    kProcessorTypeHighest:
                {
                    const auto processor = std::make_shared<ProcessorHighest>(loggerChain);
                    AddSensorProcessor(ID, processor, sensor1, sensor2);
                }
                break;
            case    kProcessorTypeLowest:
                {
                    const auto processor = std::make_shared<ProcessorLowest>(loggerChain);
                    AddSensorProcessor(ID, processor, sensor1, sensor2);
                }
                break;
            case    kProcessorTypeUnique:
                {
                    const auto processor = std::make_shared<ProcessorUnique>(loggerChain);
                    AddSensorProcessor(ID, processor, sensor1, sensor2);
                }
                break;
            case    kProcessorTypeNoop:
                {
                    const auto processor = std::make_shared<ProcessorNoop>(loggerChain);
                    AddSensorProcessor(ID, processor, sensor1, sensor2);
                }
                break;
            default:
                throw std::runtime_error("Invalid processor type");
        }

        pending_CombineSensors.erase(ID1);
        pending_CombineSensors.erase(ID2);

        setPendingSensorName(ID);
    }
}

bool Container::SensorWasUpdated(const SensorID ID) const
{
    if ( haveID(ID) == false ) {
        throw std::runtime_error("SensorWasUpdated on non-existing ID");
    }

    return IDToSensorProcessor.at(ID)->WasUpdated();
}

size_t Container::SensorGetValue(const SensorID ID) const
{
    if ( haveID(ID) == false ) {
        throw std::runtime_error("SensorGetValue on non-existing ID");
    }

    return IDToSensorProcessor.at(ID)->GetLastValue();
}

} /* namespace sensor */
} /* namespace vfuzz */
