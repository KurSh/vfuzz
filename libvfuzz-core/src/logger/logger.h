#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <utility>
#include <iostream>
#include <fstream>
#include <container/inputcluster.h>
#include <mutator/history.h>

namespace vfuzz {

class Logger {
    public:
        typedef enum {
            LOG_LEVEL_INFO,
            LOG_LEVEL_WARNING,
            LOG_LEVEL_ERROR,
        } log_level_t;
    protected:
        class SensorUpdateData {
            private:
                const SensorID _ID;
                const std::string name;
                const size_t from, to;
            public:
                SensorUpdateData(const SensorID ID, const std::string name, const size_t from, const size_t to) :
                    _ID(ID), name(name), from(from), to(to) { }
                SensorID ID(void) const { return _ID; }
                std::string Name(void) const { return name; }
                size_t From(void) const { return from; }
                size_t To(void) const { return to; }
        };
        std::vector<SensorUpdateData*> queue_sensorUpdateData;
        std::vector<container::InputCluster*> queue_inputClusters;
        std::vector<std::pair<log_level_t, std::string>> queue_info;
        std::optional<const mutator::History*> mutatorHistory;
        virtual void start(void) { };
        virtual void flush(void) const = 0;
        virtual void stop(void) { };
        virtual void sensorUpdate(const SensorID ID, const std::string name, const size_t from, const size_t to) {
            (void)ID;
            (void)name;
            (void)from;
            (void)to;
        };
        bool isStarted(void) const;
        size_t getCorpusSize(void) const;
        size_t getPrevCorpusSize(void) const;
        size_t getNumExecs(void) const;
        size_t getExecsPerSecond(void) const;
        bool doPulse(void) const;
        std::string getTimeString() const;
    private:
        bool started, do_pulse, doAutoFlush;
        size_t prevCorpusSize, corpusSize, numExecs;
        void eraseQueue(void);
        /* From http://www.cplusplus.com/forum/beginner/91449/ */
        struct timer
        {
            typedef std::chrono::steady_clock clock ;
            typedef std::chrono::seconds seconds ;

            void reset() { start = clock::now() ; }

            unsigned long long seconds_elapsed() const {
                unsigned long long ret = std::chrono::duration_cast<seconds>( clock::now() - start ).count();
                return ret ? ret : 1;
            }

            clock::time_point start = clock::now() ;
        };
        timer T;
        timer T_pulse;
    public:
        Logger(void);
        virtual ~Logger(void);
        void Start(void);
        void Stop(void);
        void Flush(void);
        void SensorUpdate(const SensorID ID, const std::string name, const size_t from, const size_t to);
        void SetCorpusSize(const size_t newCorpusSize);
        void AddInputCluster(const std::shared_ptr<container::InputCluster> inputCluster, std::optional<const mutator::History*> mutatorHistory = {});
        void AddInfo(const log_level_t logLevel, const std::string info);
        void SetAutoFlush(const bool _doAutoFlush);
};

} /* namespace vfuzz */
