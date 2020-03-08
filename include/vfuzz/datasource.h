#pragma once

#include <fuzzing/datasource/datasource.hpp>
#include <fuzzing/datasource/id.hpp>

namespace vfuzz {
namespace datasource {

class Datasource : public fuzzing::datasource::Base
{
    private:
        std::vector<uint8_t> get(const size_t min, const size_t max, const uint64_t id) override;
    public:
        Datasource(void);
};

void ApplySensorIDSet(const fuzzing::datasource::IDMap& sensorIDMap);
void ApplyGeneratorIDSet(const fuzzing::datasource::IDMap& generatorIDMap);

} /* namespace datasource */
} /* namespace vfuzz */
