#include <sensor/sensor/valueprofile.h>

namespace vfuzz {
namespace sensor {

ValueProfileSensor::ValueProfileSensor(const SensorID ID, const size_t numPCs) :
    BaseSensor(ID), numPCs(numPCs)
{
}

void ValueProfileSensor::start(void)
{
    ValueProfileMap.Reset();
}

void ValueProfileSensor::stop(void)
{
    ValueProfileMap.ForEach([&](const size_t Idx) {
        propagate(numPCs * 8 + Idx);
    });
}

void ValueProfileSensor::HandleCallerCallee(const uintptr_t Caller, const uintptr_t Callee)
{
    if ( isEffectivelyEnabled() == false ) return;
    const uintptr_t kBits = 12;
    const uintptr_t kMask = (1 << kBits) - 1;
    uintptr_t Idx = (Caller & kMask) | ((Callee & kMask) << kBits);
    ValueProfileMap.AddValueModPrime(Idx);
}

void ValueProfileSensor::AddValueForMemcmp(const void *caller_pc, const void *s1, const void *s2, const size_t n, const bool StopAtZero)
{
    if ( isEffectivelyEnabled() == false ) return;
    if (!n) return;
    size_t Len = std::min(n, (size_t)64);
    const uint8_t *A1 = reinterpret_cast<const uint8_t *>(s1);
    const uint8_t *A2 = reinterpret_cast<const uint8_t *>(s2);
    uint8_t B1[64];
    uint8_t B2[64];
    // Copy the data into locals in this non-msan-instrumented function
    // to avoid msan complaining further.
    size_t Hash = 0;  // Compute some simple hash of both strings.
    for (size_t i = 0; i < Len; i++) {
        B1[i] = A1[i];
        B2[i] = A2[i];
        size_t T = B1[i];
        Hash ^= (T << 8) | B2[i];
    }
    size_t I = 0;
    for (; I < Len; I++)
        if (B1[I] != B2[I] || (StopAtZero && B1[I] == 0))
            break;
    size_t PC = reinterpret_cast<size_t>(caller_pc);
    size_t Idx = (PC & 4095) | (I << 12);
    ValueProfileMap.AddValue(Idx);
    //TORCW.Insert(Idx ^ Hash, Word(B1, Len), Word(B2, Len));
}

void ValueProfileSensor::HandleCmp(const uintptr_t PC, const size_t Arg1, const size_t Arg2) {
    if ( isEffectivelyEnabled() == false ) return;
    uint64_t ArgXor = Arg1 ^ Arg2;
    uint64_t ArgDistance = __builtin_popcountll(ArgXor) + 1; // [1,65]
    uintptr_t Idx = ((PC & 4095) + 1) * ArgDistance;
    ValueProfileMap.AddValue(Idx);
}

enum BaseSensor::BuiltinType ValueProfileSensor::GetBuiltinType(void) const
{
    return SENSOR_BUILTIN_TYPE_VALUE_PROFILE;
}

} /* namespace sensor */
} /* namespace vfuzz */
