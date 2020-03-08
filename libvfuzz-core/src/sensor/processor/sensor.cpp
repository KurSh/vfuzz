#include <sensor/processor/sensor.h>

namespace vfuzz {
namespace sensor {

void SensorProcessor::internalReceiveInput(const SensorID ID, const Value data)
{
    /* TODO this only makes sense if there is only 1 sensor */
    if ( sensors.size() == 1 ) {
        processor->ReceiveInput(GetID(), data);
    } else if ( sensors.size() == 2 ) {
        /* TODO use this if there are 2 sensors, and postprocess in Stop() */
        if ( ID == firstSensorID ) {
            //printf("push 1st sensor %zu\n", data);
            sensorDataSensor1.push_back(data);
        } else {
            //printf("push 2st sensor %zu\n", data);
            sensorDataSensor2.push_back(data);
        }
        //abort();
    } else {
        abort();
    }
}

SensorProcessor::SensorProcessor(
        const SensorID ID,
        std::shared_ptr<BaseData> sensorData,
        std::shared_ptr<BaseProcessor> processor,
        std::shared_ptr<BaseSensor> sensor1,
        std::shared_ptr<BaseSensor> sensor2) :
    BaseSensor(ID), processor(processor), disabled(false), firstSensorID(0)
{
    {
        const onReady_t functor =
            std::bind(&BaseData::ReceiveInput, sensorData, std::placeholders::_1, std::placeholders::_2);
        SetOnReady(functor);
    }

    {
        const auto functor =
            std::bind(&SensorProcessor::processorOnAddVal, this, std::placeholders::_1);
        processor->SetOnAddVal(functor);
    }

    if ( sensor1 != nullptr ) {
        addSensor(sensor1);
    }
    if ( sensor2 != nullptr ) {
        addSensor(sensor2);
    }
}

void SensorProcessor::processorOnAddVal(const Value data)
{
    onReady(GetID(), data);
}

void SensorProcessor::addSensor(std::shared_ptr<BaseSensor> sensor)
{
    /* TODO if ( sensor == nullptr ) return; */
    const onReady_t functor =
        std::bind(&SensorProcessor::internalReceiveInput, this, std::placeholders::_1, std::placeholders::_2);
    sensor->SetOnReady(functor);
    sensors.push_back(sensor);

    if ( sensors.size() == 1 ) {
        firstSensorID = sensor->GetID();
    }
}

void SensorProcessor::Enable(void)
{
    for (const auto& sensor : sensors) {
        sensor->Enable();
    }

    disabled = false;
}

void SensorProcessor::Disable(void)
{
    for (const auto& sensor : sensors) {
        sensor->Disable();
    }

    disabled = true;
}

void SensorProcessor::Start(void)
{
    for (const auto& sensor : sensors) {
        sensor->Start();
    }

    processor->Start();
}

void SensorProcessor::Stop(void)
{
    for (const auto& sensor : sensors) {
        sensor->Stop();
    }

    if ( disabled == true ) {
        goto end;
    }

    if ( sensors.size() == 1 ) {
        goto end;
    }

    if ( sensorDataSensor1.empty() == true || sensorDataSensor2.empty() == true ) {
        goto end;
    }

    for (const auto& sensor1Val : sensorDataSensor1) {
        for (const auto& sensor2Val : sensorDataSensor2) {
            const size_t sensor1Val_uint32_t = sensor1Val & 0xFFFFFFFF;
            const size_t sensor2Val_uint32_t = sensor2Val & 0xFFFFFFFF;
            size_t finalVal = sensor1Val_uint32_t << 32;
            finalVal += sensor2Val_uint32_t;
            //printf("Combine %zu and %zu into %zu\n", sensor1Val, sensor2Val, finalVal);
            processor->ReceiveInput(GetID(), finalVal);
        }
    }

end:
    processor->Stop();
    sensorDataSensor1.clear();
    sensorDataSensor2.clear();
}

void SensorProcessor::SetName(const std::string name)
{
    processor->SetName(name);
}

void SensorProcessor::Subordinate(void)
{
    subordinate();
}

} /* namespace sensor */
} /* namespace vfuzz */
