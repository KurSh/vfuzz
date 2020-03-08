#pragma once

#include <logger/logger.h>
#include <memory>

namespace vfuzz {

class LoggerChain {
    private:
        std::vector<std::shared_ptr<Logger>> loggers;
    public:
        LoggerChain(void);
        ~LoggerChain() = default;
        void AddLogger(std::shared_ptr<Logger> logger);
        void Start(void);
        void SensorUpdate(const SensorID ID, const std::string name, const size_t from, const size_t to);
        void SetCorpusSize(const size_t newCorpusSize);
        void AddInputCluster(const std::shared_ptr<container::InputCluster> inputCluster, std::optional<const mutator::History*> mutatorHistory = {});
        void AddInfo(const Logger::log_level_t logLevel, const std::string info);
        void Stop(void);
        void Flush(void);
        void SetAutoFlush(const bool doAutoFlush);
};

} /* namespace vfuzz */
