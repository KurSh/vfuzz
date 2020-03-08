#include <sensor/driver.h>
#include <base/exception.h>
#include <sensor/processor/highest.h>
#include <sensor/processor/unique.h>
#include <sensor/processor/noop.h>
#include <sensor/sensor/allocationglobalmax.h>
#include <sensor/sensor/codecoverage.h>
#include <sensor/sensor/codeintensity.h>
#include <sensor/sensor/auto/cmpdiff.h>
#include <sensor/sensor/auto/autocodeintensity.h>
#include <sensor/sensor/auto/allocationsinglemax.h>
#include <cassert>

namespace vfuzz {

extern size_t NumGuards;

namespace sensor {

static const size_t kNumPCs = 1 << 21;
static const size_t kTracePcBits = 18;

template <class T> T Min(T a, T b) { return a < b ? a : b; }

static size_t GetNumPCs() {
    return NumGuards == 0 ? (1 << kTracePcBits) : Min(kNumPCs, NumGuards + 1);
}

Driver* g_sensor_driver = nullptr;

Driver::Driver(std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    loggerChain(loggerChain), sensorData(new Data()),
    sensorContainer(std::make_shared<Container>(sensorData, loggerChain))
{
    createBuiltin(kSensorBuiltinValueProfile, kSensorBuiltinValueProfile, "value profile");
    createBuiltin(kSensorBuiltinCMPDiff, kSensorBuiltinCMPDiff, "cmp diff");
    createBuiltin(kSensorBuiltinIntensity, kSensorBuiltinIntensity, "intensity");
    createBuiltin(kSensorBuiltinAutoCodeIntensity, kSensorBuiltinAutoCodeIntensity, "auto code intensity");
    createBuiltin(kSensorBuiltinCodeCoverage, kSensorBuiltinCodeCoverage, "code coverage");
    createBuiltin(kSensorBuiltinStackDepth, kSensorBuiltinStackDepth, "stack depth");
    createBuiltin(kSensorBuiltinStackUnique, kSensorBuiltinStackUnique, "stack unique");
    createBuiltin(kSensorBuiltinAllocGlobalMax, kSensorBuiltinAllocGlobalMax, "highest mem usage");
    createBuiltin(kSensorBuiltinAllocSingleMax, kSensorBuiltinAllocSingleMax, "single highest mem usage");
    createBuiltin(kSensorBuiltinFlowtrace, kSensorBuiltinFlowtrace, "flow trace");
}

template<typename SensorType, typename ProcessorType, typename... Args>
void Driver::addSensorProcessor(const SensorID ID, Args... args)
{
    sensorContainer->AddSensorProcessor(
            ID,
            std::make_shared<ProcessorType>(loggerChain),
            std::make_shared<SensorType>(args...));
}

void Driver::createBuiltin(const SensorID ID, const SensorBuiltinType builtinType, std::optional<std::string> name)
{
    switch ( builtinType ) {
        case    kSensorBuiltinCodeCoverage:
            addSensorProcessor<CodeCoverageSensor, ProcessorUnique>(ID, 0);
            break;
        case    kSensorBuiltinStackDepth:
            addSensorProcessor<StackSensor, ProcessorHighest>(ID, 0);
            break;
        case    kSensorBuiltinStackUnique:
            addSensorProcessor<StackSensor, ProcessorUnique>(ID, 0);
            break;
        case    kSensorBuiltinFlowtrace:
            addSensorProcessor<FlowtraceSensor, ProcessorHighest>(ID, 0);
            break;
        case    kSensorBuiltinIntensity:
            addSensorProcessor<CodeIntensitySensor, ProcessorHighest>(ID, 0);
            break;
        case    kSensorBuiltinAllocSingleMax:
            addSensorProcessor<AllocationSingleMaxSensor, ProcessorHighest>(ID, 0);
            break;
        case    kSensorBuiltinAllocGlobalMax:
            addSensorProcessor<AllocationGlobalMaxSensor, ProcessorHighest>(ID, 0);
            break;
        case    kSensorBuiltinAllocUnique:
            throw vfuzz::Exception("kSensorBuiltinAllocUnique binding currently not implemented");
        case    kSensorBuiltinValueProfile:
            addSensorProcessor<ValueProfileSensor, ProcessorUnique>(ID, 0, GetNumPCs());
            break;
        case    kSensorBuiltinCMPDiff:
            addSensorProcessor<CMPDiffSensor, ProcessorNoop>(ID, 0);
            break;
        case    kSensorBuiltinAutoCodeIntensity:
            addSensorProcessor<AutoCodeIntensitySensor, ProcessorNoop>(ID, 0);
            break;
        default:
            throw vfuzz::Exception("Not implemented");
    }
    sensorContainer->Disable(ID);

    if ( name != std::nullopt ) {
        SetSensorName(ID, *name);
    }
}

/* Start registering sensor signals */
void Driver::StartRecord(void)
{
    sensorData->increase = 0;
    sensorData->increase_per_sensor.clear();
    sensorContainer->Start();

    g_sensor_driver = this;
}

/* Stop registering sensor signals.
 * Returns true if new data was registered during this cycle
 */
bool Driver::StopRecord(void)
{
    sensorContainer->Stop();

    g_sensor_driver = nullptr;

    return sensorData->increase > 0;
}

void Driver::Enable(const SensorID ID)
{
    sensorContainer->Enable(ID);
}

void Driver::Disable(const SensorID ID)
{
    sensorContainer->Disable(ID);
}

void Driver::CustomCallback(const SensorID ID, const uint64_t data, const ProcessorType processorType)
{
    sensorContainer->AddCustomIfNotExists(ID, processorType)->Update(data);
}

void Driver::BindBuiltin(const SensorID ID, const SensorBuiltinType builtinType)
{
    createBuiltin(ID, builtinType);
}

void Driver::SensorFromCoreBuiltin(const SensorID ID, const SensorCoreBuiltinType builtin_core_ID, const ProcessorType processorType )
{
    std::shared_ptr<BaseProcessor> processor = nullptr;
    std::shared_ptr<BaseSensor> sensor = nullptr;

    switch ( builtin_core_ID ) {
        case    kSensorTypeCodeCoverage:
            sensor = std::make_shared<CodeCoverageSensor>(0);
            break;
        case    kSensorTypeValueProfile:
            sensor = std::make_shared<ValueProfileSensor>(0, GetNumPCs());
            break;
        default:
            throw vfuzz::Exception("Not implemented");
    }

    switch ( processorType ) {
        case    kProcessorTypeUnique:
            processor = std::make_shared<ProcessorUnique>(loggerChain);
            break;
        case    kProcessorTypeNoop:
            processor = std::make_shared<ProcessorNoop>(loggerChain);
            break;
        default:
            throw vfuzz::Exception("Not implemented");
    }

    assert(processor != nullptr);
    assert(sensor != nullptr);

    sensorContainer->AddSensorProcessor(ID, processor, sensor);
}

void Driver::UnbindBuiltin(const SensorID ID)
{
    (void)ID;
    /* TODO */
    throw vfuzz::Exception("Sensor unbinding currently not supported");
}

void Driver::CombineSensors(const SensorID ID, const SensorID ID1, const SensorID ID2, const ProcessorType processorType)
{
    sensorContainer->CombineSensors(ID, ID1, ID2, processorType);
}

void Driver::SetSensorName(const SensorID ID, const char* name)
{
    sensorContainer->SetSensorName(ID, std::string(name));
}

void Driver::SetSensorName(const SensorID ID, const std::string name)
{
    sensorContainer->SetSensorName(ID, name);
}

bool Driver::SensorWasUpdated(const SensorID ID) const
{
    return sensorContainer->SensorWasUpdated(ID);
}

size_t Driver::SensorGetValue(const SensorID ID) const
{
    return sensorContainer->SensorGetValue(ID);
}

Data Driver::GetSensorData(void) const {
    return *sensorData;
}

void Driver::builtin_AddPC(const size_t PC) {
    sensorContainer->ForEachPCSensor([&](std::shared_ptr<PCSensor> curSensor) {
        curSensor->Update(PC);
    });
}

void Driver::builtin_AddStack(void) {
    sensorContainer->ForEachStackSensor([&](std::shared_ptr<StackSensor> curSensor) {
        curSensor->Update();
    });
    /*
    for ( auto& curSensor : sensorContainer->GetSensorsForBuiltin(BaseSensor::SENSOR_BUILTIN_TYPE_STACK) ) {
        dynamic_cast<StackSensor*>(curSensor)->Update();
    }
    */
}

void Driver::builtin_HandleStacktrace(const uintptr_t caller) {
    sensorContainer->ForEachFlowtraceSensor([&](std::shared_ptr<FlowtraceSensor> curSensor) {
        curSensor->Update(caller);
    });
}

void Driver::builtin_AddFuncEnter(const size_t func, const size_t caller) {
    /* TODO */
    (void)func;
    (void)caller;
    /*
    for ( auto& curSensor : sensorContainer->GetSensorsForBuiltin(BaseSensor::SENSOR_BUILTIN_TYPE_FUNC_ENTER) ) {
        dynamic_cast<FuncEnterSensor*>(curSensor)->Update(func, caller);
    }
    */
}

void Driver::builtin_Malloc(const size_t PC, const void *ptr, const size_t size) {
    sensorContainer->ForEachAllocationSensor([&](std::shared_ptr<AllocationSensor> curSensor) {
        curSensor->AddMalloc(PC, ptr, size);
    });
}

void Driver::builtin_Free(const size_t PC, const void *ptr) {
    sensorContainer->ForEachAllocationSensor([&](std::shared_ptr<AllocationSensor> curSensor) {
        curSensor->AddFree(PC, ptr);
    });
}

void Driver::builtin_HandleCallerCallee(const uintptr_t Caller, const uintptr_t Callee)
{
    sensorContainer->ForEachValueProfileSensor([&](std::shared_ptr<ValueProfileSensor> curSensor) {
        curSensor->HandleCallerCallee(Caller, Callee);
    });
}

void Driver::builtin_AddValueForMemcmp(const void *caller_pc, const void *s1, const void *s2, const size_t n, const bool StopAtZero)
{
    sensorContainer->ForEachValueProfileSensor([&](std::shared_ptr<ValueProfileSensor> curSensor) {
        curSensor->AddValueForMemcmp(caller_pc, s1, s2, n, StopAtZero);
    });
}

void Driver::builtin_HandleCmp(const uintptr_t PC, const size_t Arg1, const size_t Arg2) {
    sensorContainer->ForEachValueProfileSensor([&](std::shared_ptr<ValueProfileSensor> curSensor) {
        curSensor->HandleCmp(PC, Arg1, Arg2);
    });
    sensorContainer->ForEachCMPSensor([&](std::shared_ptr<CMPSensor> curSensor) {
        curSensor->Update(PC, Arg1, Arg2);
    });
}

void Driver::MallocHook(const size_t PC, const void *ptr, const size_t size) {
    if ( g_sensor_driver == nullptr ) {
        return;
    }

    g_sensor_driver->builtin_Malloc(PC, ptr, size);
}

void Driver::FreeHook(const size_t PC, const void *ptr) {
    if ( g_sensor_driver == nullptr ) {
        return;
    }

    g_sensor_driver->builtin_Free(PC, ptr);
}

void Driver::HandleCallerCallee(const uintptr_t Caller, const uintptr_t Callee) {
    if ( g_sensor_driver == nullptr ) {
        return;
    }

    g_sensor_driver->builtin_HandleCallerCallee(Caller, Callee);
}

void Driver::AddValueForMemcmp(const void *caller_pc, const void *s1, const void *s2, const size_t n, const bool StopAtZero) {
    if ( g_sensor_driver == nullptr ) {
        return;
    }

    g_sensor_driver->builtin_AddValueForMemcmp(caller_pc, s1, s2, n, StopAtZero);
}

} /* namespace sensor */
} /* namespace vfuzz */

extern "C" void sensor_callback(vfuzz::SensorID ID, size_t val, vfuzz::ProcessorType type)
{
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->CustomCallback(ID, val, type);
}

extern "C" void sensor_set_name(const vfuzz::SensorID ID, const char* name)
{
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->SetSensorName(ID, name);
}

extern "C" void sensor_enable(const vfuzz::SensorID ID) {
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->Enable(ID);
}

extern "C" void sensor_disable(const vfuzz::SensorID ID) {
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->Disable(ID);
}

extern "C" void sensor_from_core_builtin(const vfuzz::SensorID ID, const vfuzz::SensorCoreBuiltinType builtin_core_ID, const vfuzz::ProcessorType processorType)
{
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->SensorFromCoreBuiltin(ID, builtin_core_ID, processorType);
}

extern "C" void sensor_bind_builtin(const vfuzz::SensorID ID, const vfuzz::SensorBuiltinType builtinType) {
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->BindBuiltin(ID, builtinType);
}

extern "C" void sensor_unbind_builtin(const vfuzz::SensorID ID) {
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->UnbindBuiltin(ID);
}

extern "C" void sensor_combine(const vfuzz::SensorID ID, const vfuzz::SensorID ID1, const vfuzz::SensorID ID2, const vfuzz::ProcessorType processorType) {
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->CombineSensors(ID, ID1, ID2, processorType);
}

extern "C" bool sensor_was_updated(const vfuzz::SensorID ID)
{
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        throw vfuzz::Exception("sensor_was_updated: g_sensor_driver == nullptr");
    }

    return vfuzz::sensor::g_sensor_driver->SensorWasUpdated(ID);
}

extern "C" size_t sensor_get_value(const vfuzz::SensorID ID)
{
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        throw vfuzz::Exception("sensor_get_value: g_sensor_driver == nullptr");
    }

    return vfuzz::sensor::g_sensor_driver->SensorGetValue(ID);
}
