#include <sensor/data/data.h>

namespace vfuzz {
namespace sensor {

Data::Data(void)
{
}

void Data::ReceiveInput(const SensorID ID, const Value data)
{
    /* TODO ?*/
    (void)ID;
    (void)data;
    increase++;
}

void Data::SetSensorName(const SensorID ID, const char* name)
{
    /* TODO ?*/
    (void)ID;
    (void)name;
}

} /* namespace sensor */
} /* namespace vfuzz */
