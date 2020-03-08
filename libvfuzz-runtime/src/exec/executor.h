#pragma once

#include <mutator/mutator.h>
#include <sensor/driver.h>
#include <container/input.h>
#include <container/inputcluster.h>
#include <logger/loggerchain.h>
#include <dictionary/dictionary.h>
#include <mutator/container.h>
#include <util/pushpullcallback.h>
#include <cstdlib>
#include <map>
#include <memory>
#include <functional>
#include <utility>

extern "C" void VFuzzInit(int* argc, char*** argv);
extern "C" void VFuzzRun(void);

namespace vfuzz {

class Executor;

extern Executor* g_executor;

typedef enum {
    ExecutorModeDynamic = 0,
    ExecutorModeStatic = 1,
} executor_mode_t;

class Executor {
    private:
        util::PushPullCallback<std::shared_ptr<container::InputCluster>> icPushPullCB;
        std::shared_ptr<container::InputCluster> curInputCluster = nullptr;
        std::shared_ptr<mutator::Container> mutators;
        std::shared_ptr<util::Random> Rand;
        std::shared_ptr<sensor::Driver> sensorDriver;
        std::map<GeneratorID, container::ChunkIndex> gidToCurrentIndex;
        std::vector<uint8_t*> toFree;
        executor_mode_t mode;
        std::vector<std::pair<mutator::Mutator*, size_t>> weightTable;
        void setModeDynamic(void) { mode = ExecutorModeDynamic; }
        void setModeStatic(void) { mode = ExecutorModeStatic; }

        std::set<std::pair<Chunk, Chunk>> replacings;
        bool recordReplace = false;
        //void replaceBatch(void);

        /* Callbacks */
        std::optional<std::function<void(const std::shared_ptr<container::InputCluster> inputCluster)>> onNew = {};
        std::optional<std::function<void(const std::string prefix, const std::shared_ptr<container::InputCluster> inputCluster, const bool doExit)>> onCrash = {};

        bool haveGenerated;
        size_t maxChunks, maxChunkLen;
        size_t maxMutations;
        bool doDiscard;
        std::optional<std::shared_ptr<LoggerChain>> loggerChain;
        std::optional<std::shared_ptr<dictionary::Dictionary>> dictionary;
    public:
        Executor(std::shared_ptr<util::Random> Rand, util::PushPullCallback<std::shared_ptr<container::InputCluster>> icPushPullCB, std::shared_ptr<mutator::Container> mutators,
            const size_t maxChunks, const size_t maxChunkLen, const size_t maxMutations,
            std::optional<std::shared_ptr<dictionary::Dictionary>> dictionary = {}, std::optional<std::shared_ptr<LoggerChain>> loggerChain = {});
        ~Executor() = default;
        void GetNextChunk(const GeneratorID gid, uint8_t** const data, size_t *const size, size_t min, size_t max);
        void GetNextChunk(const GeneratorID gid, uint8_t** const data, size_t *const size);
        static void StaticDeathCallback();
        static void HandleCrash(const std::string prefix, const bool doExit);
        void Initialize(int argc, char** argv);
        void Run(void);
        bool RunInputCluster(std::shared_ptr<container::InputCluster> inputCluster);
        executor_mode_t GetMode(void) const { return mode; }
        void SetMutatorWeights(std::vector<std::pair<mutator::Mutator*, size_t>> weightTable) {
            this->weightTable = weightTable;
        }

        void prepare(void);
        bool finish(void);
        void prepare(std::shared_ptr<sensor::Driver> sensorDriver);
        bool finish(std::shared_ptr<sensor::Driver> sensorDriver);
        sensor::Data InputClusterToSensorData(std::shared_ptr<container::InputCluster> inputCluster);
        container::InputCluster ShrinkInputCluster(std::shared_ptr<container::InputCluster> inputCluster);
        void AddReplace(std::pair<Chunk, Chunk> replacePair);
        //void ToggleRecordReplace(void);
        void SetDiscard(const bool _doDiscard);
        void SetOnNew(const std::function<void(const std::shared_ptr<container::InputCluster> inputCluster)> _onNew);
        void SetOnCrash(const std::function<void(const std::string prefix, const std::shared_ptr<container::InputCluster> inputCluster, const bool doExit)> _onCrash);
};

} /* namespace vfuzz */
