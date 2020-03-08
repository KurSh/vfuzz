#include <logger/logger.h>
#include <set>
#include <map>
#include <util/util.h>

namespace vfuzz {

Logger::Logger(void) :
    started(false), doAutoFlush(true), prevCorpusSize(0), corpusSize(0), numExecs(0)
{
}

Logger::~Logger()
{
    eraseQueue();
}

bool Logger::isStarted(void) const
{
    return started == true;
}

void Logger::Start(void)
{
    do_pulse = false;
    started = true;
    start();
}

void Logger::Stop(void)
{
    numExecs++;
    started = false;
    if ( T_pulse.seconds_elapsed() >= 3 ) {
        T_pulse.reset();
        do_pulse = true;
    }
    stop();
    Flush();
}

void Logger::Flush(void)
{
    flush();
    eraseQueue();
}

void Logger::SensorUpdate(const SensorID ID, const std::string name, const size_t from, const size_t to)
{
    queue_sensorUpdateData.push_back( new SensorUpdateData(ID, name, from, to) );
    sensorUpdate(ID, name, from, to);
}

void Logger::SetCorpusSize(const size_t newCorpusSize)
{
    prevCorpusSize = corpusSize;
    corpusSize = newCorpusSize;
}

void Logger::AddInputCluster(const std::shared_ptr<container::InputCluster> inputCluster, std::optional<const mutator::History*> mutatorHistory)
{
    queue_inputClusters.push_back( new container::InputCluster(*inputCluster) );
    this->mutatorHistory = mutatorHistory;
}

void Logger::AddInfo(const log_level_t logLevel, const std::string info) {
    queue_info.push_back({logLevel, info});
}

void Logger::eraseQueue(void)
{
    for (const auto& sud : queue_sensorUpdateData ) {
        delete sud;
    }

    queue_sensorUpdateData.clear();

    for (const auto& ic : queue_inputClusters ) {
        delete ic;
    }

    queue_inputClusters.clear();

    queue_info.clear();
}

size_t Logger::getCorpusSize(void) const
{
    return corpusSize;
}

size_t Logger::getPrevCorpusSize(void) const
{
    return prevCorpusSize;
}

size_t Logger::getNumExecs(void) const
{
    return numExecs;
}

size_t Logger::getExecsPerSecond(void) const
{
    return numExecs / T.seconds_elapsed();
}

bool Logger::doPulse(void) const
{
    return do_pulse;
}

/* Adapter from https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c */
std::string Logger::getTimeString() const
{
    struct tm  tstruct, *tstruct_ptr;
    char       buf[80];
    time_t     now;

    now = time(0);
    if ( now == ((time_t)-1) ) {
        /* time() failed */
        buf[0] = 0;
        goto end;
    }

    tstruct_ptr = localtime(&now);
    if ( tstruct_ptr == nullptr ) {
        /* localtime() failed */
        buf[0] = 0;
        goto end;
    }

    tstruct = *tstruct_ptr;

    if ( strftime(buf, sizeof(buf), "%X", &tstruct) == 0 ) {
        /* Error occurred -- buf undefined */
        buf[0] = 0;
    }

end:
    return buf;
}

void Logger::SetAutoFlush(const bool _doAutoFlush)
{
    doAutoFlush = _doAutoFlush;
}

} /* namespace vfuzz */
