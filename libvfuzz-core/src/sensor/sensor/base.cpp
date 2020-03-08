#include <sensor/sensor/base.h>

namespace vfuzz {
namespace sensor {

SensorID BaseSensor::GetID(void) const
{
    if ( ID == 0 || subordinated == true ) {
        return (SensorID)this;
    } else {
        return ID;
    }
}

void BaseSensor::subordinate(void)
{
    subordinated = true;
}

BaseSensor::BaseSensor(const SensorID ID) :
            ID(ID), subordinated(false), lastValue(std::nullopt),
            onReady(nullptr), stopped(false), disabled(false)
{
}

void BaseSensor::SetOnReady(onReady_t _onReady)
{
    onReady = _onReady;
}

void BaseSensor::start(void)
{
}

void BaseSensor::stop(void)
{
}

void BaseSensor::propagate(Value val)
{
    if ( isEffectivelyEnabled() == false ) return;
    isUpdated = true;
    setLastValue(val);
    onReady(GetID(), val);
}

bool BaseSensor::isEffectivelyEnabled(void) const
{
    return stopped == false && disabled == false;
}

void BaseSensor::Enable(void)
{
    disabled = false;
}

void BaseSensor::Disable(void)
{
    disabled = true;
}

void BaseSensor::Start(void)
{
    isUpdated = false;
    start();
    stopped = false;
}

void BaseSensor::Stop(void)
{
    stop();
    stopped = true;
}

enum BaseSensor::BuiltinType BaseSensor::GetBuiltinType(void) const
{
    return SENSOR_BUILTIN_TYPE_NONE;
}

void BaseSensor::setLastValue(const Value value)
{
    lastValue = value;
}

Value BaseSensor::GetLastValue(void) const
{
    if ( lastValue == std::nullopt ) {
        throw std::runtime_error("GetLastValue on unset sensor");
    }
    return *lastValue;
}

bool BaseSensor::WasUpdated(void) const
{
    return isUpdated;
}

} /* namespace sensor */
} /* namespace vfuzz */
