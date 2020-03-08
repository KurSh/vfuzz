#pragma once

#include <util/random.h>
#include <container/corpus/corpus.h>
#include <options/options.h>
#include <logger/loggerchain.h>
#include <container/inputcluster.h>
#include <memory>
#include <utility>

namespace vfuzz {
namespace container {
namespace corpus {

class Manager {
    private:
        std::shared_ptr<util::Random> Rand = nullptr;
        const size_t maxChunks;
        const size_t maxChunkLen;
        std::shared_ptr<InputCluster> initialInputCluster = nullptr;
        std::shared_ptr<Corpus> corpus = nullptr;
        const options::FileList files;
        std::optional<std::shared_ptr<LoggerChain>> loggerChain;

        bool directorySpecified = false, filesSpecified = false;
        std::string workingDirectory;
        bool isProcessed = false;

        void loadInputCluster(const std::string& filename);
    public:
        Manager(std::shared_ptr<util::Random> Rand, const size_t maxChunks, const size_t maxChunkLen, options::FileList files, std::optional<std::shared_ptr<LoggerChain>> loggerChain);
        void Process(const bool addToCorpus);
        std::string GetWorkingDirectory(void) const;
        std::shared_ptr<Corpus> GetCorpus(void);
};

} /* namespace corpus */
} /* namespace container */
} /* namespace vfuzz */
