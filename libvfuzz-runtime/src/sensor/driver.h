#pragma once

#include <sensor/sensor/base.h>
#include <sensor/container.h>
#include <utility>
#include <optional>

namespace vfuzz {
namespace sensor {

class Driver {
    private:
        std::optional<std::shared_ptr<LoggerChain>> loggerChain;
        std::shared_ptr<Data> sensorData;
        std::shared_ptr<Container> sensorContainer;

        template<typename SensorType, typename ProcessorType, typename... Args> void addSensorProcessor(const SensorID ID, Args... args);
        void createBuiltin(const SensorID ID, const SensorBuiltinType builtinType, std::optional<std::string> name = {});
    public:
        Driver(std::optional<std::shared_ptr<LoggerChain>> loggerChain = {});
        ~Driver() = default;

        void StartRecord(void);
        bool StopRecord(void);
        void Enable(const SensorID ID);
        void Disable(const SensorID ID);

        void CustomCallback(const SensorID ID, const uint64_t val, const ProcessorType type);

        void BindBuiltin(const SensorID ID, const SensorBuiltinType builtinType);
        void SensorFromCoreBuiltin(const SensorID ID, const SensorCoreBuiltinType builtin_core_ID, const ProcessorType processorType);
        void UnbindBuiltin(const SensorID ID);
        void CombineSensors(const SensorID ID, const SensorID ID1, const SensorID ID2, const ProcessorType processorType);
        void SetSensorName(const SensorID ID, const char* name);
        void SetSensorName(const SensorID ID, const std::string name);
        bool SensorWasUpdated(const SensorID ID) const;
        size_t SensorGetValue(const SensorID ID) const;

        Data GetSensorData(void) const;

        void builtin_Malloc(const size_t PC, const void *ptr, const size_t size);
        void builtin_Free(const size_t PC, const void *ptr);
        void builtin_AddPC(const size_t PC);
        void builtin_AddStack(void);
        void builtin_HandleStacktrace(const uintptr_t caller);
        void builtin_AddFuncEnter(const size_t func, const size_t caller);
        void builtin_HandleCallerCallee(const uintptr_t Caller, const uintptr_t Callee);
        void builtin_HandleCmp(const uintptr_t PC, const size_t Arg1, const size_t Arg2);
        void builtin_AddValueForMemcmp(const void *caller_pc, const void *s1, const void *s2, const size_t n, const bool StopAtZero);

        static void MallocHook(const size_t PC, const void *ptr, const size_t size);
        static void FreeHook(const size_t PC, const void *ptr);

        static void HandleCallerCallee(const uintptr_t Caller, const uintptr_t Callee);
        static void AddValueForMemcmp(const void *caller_pc, const void *s1, const void *s2, const size_t n, const bool StopAtZero);
        template <class T> static void HandleCmp(const uintptr_t PC, const T Arg1, const T Arg2);
};

extern Driver* g_sensor_driver;

} /* namespace sensor */
} /* namespace vfuzz */
