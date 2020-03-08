#pragma once

#include <cstdint>
#include <vector>
#include <fuzzing/datasource/id.hpp>

namespace vfuzz {
    using Chunk = std::vector<uint8_t>;

    using Value = uint64_t;

    using SensorID = uint64_t;
    using SensorVal = uint64_t;
    using GeneratorID = uint64_t;

    typedef enum {
        kProcessorTypeHighest = 1,
        kProcessorTypeLowest = 2,
        kProcessorTypeUnique = 3,
        kProcessorTypeNoop = 4,
    } ProcessorType;

    typedef enum {
        kSensorBuiltinCodeCoverage = fuzzing::datasource::ID("vfuzz/kSensorBuiltinCodeCoverage"),
        kSensorBuiltinStackDepth = fuzzing::datasource::ID("vfuzz/kSensorBuiltinStackDepth"),
        kSensorBuiltinStackUnique = fuzzing::datasource::ID("vfuzz/kSensorBuiltinStackUnique"),
        kSensorBuiltinFlowtrace = fuzzing::datasource::ID("vfuzz/kSensorBuiltinFlowtrace"),
        kSensorBuiltinIntensity = fuzzing::datasource::ID("vfuzz/kSensorBuiltinIntensity"),
        kSensorBuiltinAllocSingleMax = fuzzing::datasource::ID("vfuzz/kSensorBuiltinAllocSingleMax"),
        kSensorBuiltinAllocGlobalMax = fuzzing::datasource::ID("vfuzz/kSensorBuiltinAllocGlobalMax"),
        kSensorBuiltinAllocUnique = fuzzing::datasource::ID("vfuzz/kSensorBuiltinAllocUnique"),
        kSensorBuiltinValueProfile = fuzzing::datasource::ID("vfuzz/kSensorBuiltinValueProfile"),
        kSensorBuiltinFuncEnter = fuzzing::datasource::ID("vfuzz/kSensorBuiltinFuncEnter"),
        kSensorBuiltinCMPDiff = fuzzing::datasource::ID("vfuzz/kSensorBuiltinCMPDiff"),
        kSensorBuiltinAutoCodeIntensity = fuzzing::datasource::ID("vfuzz/kSensorBuiltinAutoCodeIntensity"),
    } SensorBuiltinType;

    typedef enum {
        kSensorTypeCodeCoverage = 1,
        kSensorTypeCodeIntensity = 2,
        kSensorTypeStack = 3,
        kSensorTypeAlloc = 4,
        kSensorTypeValueProfile = 5,
        kSensorTypeFuncEnter = 6,
    } SensorCoreBuiltinType;
} /* namespace vfuzz */
