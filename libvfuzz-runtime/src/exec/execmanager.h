#pragma once

#include <exec/executor.h>
#include <container/corpus/corpus.h>
#include <dictionary/dictionary.h>
#include <dictionary/autodict.h>
#include <options/options.h>
#include <logger/loggerchain.h>
#include <mutator/container.h>
#include <mutator/history.h>
#include <container/corpus/manager.h>
#include <vector>
#include <memory>

namespace vfuzz {

class ExecManager {
    private:
        std::shared_ptr<util::Random> Rand;
        mutator::History mutatorHistory;
        std::shared_ptr<mutator::Container> mutators;
        std::shared_ptr<LoggerChain> loggerChain;

        std::shared_ptr<container::corpus::Corpus> corpus = nullptr;
        std::unique_ptr<Executor> executor = nullptr;
        std::shared_ptr<dictionary::Dictionary> dictionary = nullptr;
        std::shared_ptr<dictionary::AutoDict> autodict = nullptr;
        std::unique_ptr<container::corpus::Manager> corpusManagerExec = nullptr, corpusManagerInput = nullptr;
        bool isInitialized = false;
        bool runStatic, runDynamic;

        //void processMetadata(void);
        void processInsertions(const std::vector<std::shared_ptr<container::Input>>& inputs);
        //void processMutators(const std::vector<Input*>& inputs);

        void onNewInputCluster(const std::shared_ptr<container::InputCluster> inputCluster) const;
        void onCrash(const std::string prefix, const std::shared_ptr<container::InputCluster> inputCluster, const bool doExit) const;
        void initializeMutators(void);
        void initializeCallbacks(void);

        uint64_t numRuns;
    public:
        ExecManager(std::shared_ptr<util::Random> Rand, std::shared_ptr<LoggerChain> loggerChain);
        ~ExecManager() = default;
        bool Initialize(int argc, char** argv);
        void Run(void);
};

} /* namespace vfuzz */
