#include <sensor/sensor/auto/autosensor.h>
#include <vfuzz/vfuzz.h>
#include <sstream>

namespace vfuzz {
namespace sensor {

void AutoSensor::setAutoSensorName(const SensorID ID, const size_t PC) {
    if ( pcSeen.count(PC) > 0 ) {
        return;
    }
    pcSeen[PC] = true;

    std::stringstream name;
    name << getSensorNamePrefix() << "/" << "0x" << std::hex << PC;
    sensor_set_name(ID, name.str().c_str());
}

} /* namespace sensor */
} /* namespace vfuzz */
