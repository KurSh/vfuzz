#include <vfuzz/vfuzz.h>
#include "nlohmann/json.hpp"

extern "C" void VFuzzInit(int argc, char **argv) {
    (void)argc;
    (void)argv;

    /* Experiment with uncommenting any of the following lines. */
    //sensor_enable(vfuzz::kSensorBuiltinCodeCoverage);
    //sensor_enable(vfuzz::kSensorBuiltinStackDepth);
    //sensor_enable(vfuzz::kSensorBuiltinStackUnique);
    //sensor_enable(vfuzz::kSensorBuiltinFlowtrace);
    //sensor_enable(vfuzz::kSensorBuiltinIntensity);
    //sensor_enable(vfuzz::kSensorBuiltinAllocSingleMax);
    //sensor_enable(vfuzz::kSensorBuiltinAllocGlobalMax);
    //sensor_enable(vfuzz::kSensorBuiltinAllocUnique);
    //sensor_enable(vfuzz::kSensorBuiltinValueProfile);
    //sensor_enable(vfuzz::kSensorBuiltinFuncEnter);
    //sensor_enable(vfuzz::kSensorBuiltinCMPDiff);
    //sensor_enable(vfuzz::kSensorBuiltinAutoCodeIntensity);
}

extern "C" void VFuzzRun(void) {
    vfuzz::datasource::Datasource ds;

    const auto s = ds.Get<std::string>();
    try {
        auto j = nlohmann::json::parse(s);
    } catch ( ... ) {
        /* Experiment with uncommenting the following line.
         * It prevents the input getting added to the corpus
         * if json parsing failed */
         //vfuzz_discard();
    }
}
