#include <datasource/id.h>
#include <vfuzz/vfuzz.h>

namespace vfuzz {
namespace datasource {

void ApplySensorIDSet(const fuzzing::datasource::IDMap& sensorIDMap)
{
    for (const auto& pair : sensorIDMap) {
        sensor_set_name(pair.second, pair.first);
    }
}

void ApplyGeneratorIDSet(const fuzzing::datasource::IDMap& generatorIDMap)
{
    for (const auto& pair : generatorIDMap) {
        (void)pair;
        /* TODO generator_set_name(pair.second, pair.first); */
    }
}

} /* namespace datasource */
} /* namespace vfuzz */
