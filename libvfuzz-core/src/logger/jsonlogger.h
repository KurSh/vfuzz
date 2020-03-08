#pragma once

#include <logger/logger.h>
#include <boost/property_tree/ptree.hpp>

namespace vfuzz {

class JSONLogger : public Logger {
    private:
        const std::string outputPath;
        void flush(void) const override;
        boost::property_tree::ptree serializeSensorUpdateDataSingle(const SensorUpdateData * sensorUpdateData) const;
        boost::property_tree::ptree serializeInput(const std::shared_ptr<container::Input> input) const;
        boost::property_tree::ptree serializeInputCluster(const container::InputCluster* inputCluster) const;
    public:
        JSONLogger(const std::string outputPath);
};

} /* namespace vfuzz */
