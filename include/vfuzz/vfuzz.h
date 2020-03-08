#pragma once

#include "datasource.h"
#include "types.h"
#include <fuzzing/datasource/datasource.hpp>

extern "C" {
    void sensor_callback(vfuzz::SensorID id, size_t val, vfuzz::ProcessorType type);
    void sensor_set_name(vfuzz::SensorID id, const char* name);
    void vfuzz_discard(void);
    void vfuzz_dont_discard(void);
    void vfuzz_crash(void);
    void sensor_disable(vfuzz::SensorID id);
    void sensor_enable(vfuzz::SensorID id);
    void sensor_bind_builtin(const vfuzz::SensorID id, const vfuzz::SensorBuiltinType builtinType);
    void sensor_from_core_builtin(const vfuzz::SensorID id, const vfuzz::SensorCoreBuiltinType builtin_core_id, const vfuzz::ProcessorType type);
    void sensor_combine(const vfuzz::SensorID id, const vfuzz::SensorID id1, const vfuzz::SensorID id2, const vfuzz::ProcessorType type);
} /* extern "C" */
