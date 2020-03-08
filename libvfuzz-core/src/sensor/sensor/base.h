#pragma once

#include <functional>
#include <base/types.h>
#include <optional>

namespace vfuzz {
namespace sensor {

typedef std::function<void(const SensorID ID, const Value data)> onReady_t;
class BaseSensor
{
    private:
        const SensorID ID;
        bool subordinated;
        std::optional<Value> lastValue;
        bool isUpdated;
        void setLastValue(const Value val);
    protected:
        onReady_t onReady;
        bool stopped, disabled;
        virtual void start(void);
        virtual void stop(void);
        void propagate(Value val);
        void subordinate(void);
        bool isEffectivelyEnabled(void) const;
    public:
        enum BuiltinType {
            SENSOR_BUILTIN_TYPE_NONE,
            SENSOR_BUILTIN_TYPE_STACK,
            SENSOR_BUILTIN_TYPE_STACK_TRACE,
            SENSOR_BUILTIN_TYPE_PC,
            SENSOR_BUILTIN_TYPE_ALLOCATION,
            SENSOR_BUILTIN_TYPE_VALUE_PROFILE,
            SENSOR_BUILTIN_TYPE_CMP,
        };
        BaseSensor(const SensorID ID);
        virtual ~BaseSensor() = default;
        SensorID GetID(void) const;
        void SetOnReady(onReady_t _onReady);
        void Enable(void);
        void Disable(void);
        void Start(void);
        void Stop(void);
        virtual enum BuiltinType GetBuiltinType(void) const;
        Value GetLastValue(void) const;
        bool WasUpdated(void) const;
};

} /* namespace sensor */
} /* namespace vfuzz */
