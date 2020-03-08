#include <logger/loggerchain.h>

namespace vfuzz {

LoggerChain::LoggerChain(void)
{
}

void LoggerChain::AddLogger(std::shared_ptr<Logger> logger)
{
    loggers.push_back(logger);
}

void LoggerChain::Start(void)
{
    for ( const auto& curLogger : loggers ) {
        curLogger->Start();
    }
}

void LoggerChain::Stop(void)
{
    for ( const auto& curLogger : loggers ) {
        curLogger->Stop();
    }
}

void LoggerChain::Flush(void)
{
    for ( const auto& curLogger : loggers ) {
        curLogger->Flush();
    }
}

void LoggerChain::SetAutoFlush(const bool doAutoFlush)
{
    for ( const auto& curLogger : loggers ) {
        curLogger->SetAutoFlush(doAutoFlush);
    }
}

void LoggerChain::SensorUpdate(const SensorID ID, const std::string name, const size_t from, const size_t to)
{
    for ( const auto& curLogger : loggers ) {
        curLogger->SensorUpdate(ID, name, from, to);
    }
}
void LoggerChain::SetCorpusSize(const size_t newCorpusSize)
{
    for ( const auto& curLogger : loggers ) {
        curLogger->SetCorpusSize(newCorpusSize);
    }
}

void LoggerChain::AddInputCluster(const std::shared_ptr<container::InputCluster> inputCluster, std::optional<const mutator::History*> mutatorHistory)
{
    for ( const auto& curLogger : loggers ) {
        curLogger->AddInputCluster(inputCluster, mutatorHistory);
    }
}

void LoggerChain::AddInfo(const Logger::log_level_t logLevel, const std::string info)
{
    for ( const auto& curLogger : loggers ) {
        curLogger->AddInfo(logLevel, info);
    }
}

} /* namespace vfuzz */
