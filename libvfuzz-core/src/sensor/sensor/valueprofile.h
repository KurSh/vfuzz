#pragma once

#include <sensor/sensor/base.h>
#include <sensor/sensor/valuebitmap.h>

namespace vfuzz {
namespace sensor {

class ValueProfileSensor : public BaseSensor
{
    private:
        ValueBitMap ValueProfileMap;
        const size_t numPCs;
        void start(void) override;
        void stop(void) override;
    public:
        ValueProfileSensor(const SensorID ID, const size_t numPCs);
        void HandleCallerCallee(const uintptr_t Caller, const uintptr_t Callee);
        void AddValueForMemcmp(const void *caller_pc, const void *s1, const void *s2, const size_t n, const bool StopAtZero);
        void HandleCmp(const uintptr_t PC, const size_t Arg1, const size_t Arg2);
        enum BuiltinType GetBuiltinType(void) const override;
};

} /* namespace sensor */
} /* namespace vfuzz */
