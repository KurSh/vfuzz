#include <container/corpus/manager.h>
#include <util/io.h>
#include <base/exception.h>

namespace vfuzz {
namespace container {
namespace corpus {

Manager::Manager(
        std::shared_ptr<util::Random> Rand,
        const size_t maxChunks,
        const size_t maxChunkLen,
        options::FileList files,
        std::optional<std::shared_ptr<LoggerChain>> loggerChain) :
    Rand(Rand),
    maxChunks(maxChunks),
    maxChunkLen(maxChunkLen),
    initialInputCluster(std::make_shared<InputCluster>(Rand, maxChunks, maxChunkLen)),
    corpus(std::make_shared<Corpus>(Rand, *initialInputCluster)),
    files(files),
    loggerChain(loggerChain) {
}

void Manager::loadInputCluster(const std::string& filename) {
    const auto filedata = util::FileToVector(filename, 0);
    if ( filedata == std::nullopt ) {
        if ( loggerChain != std::nullopt ) {
            (*loggerChain)->AddInfo(Logger::LOG_LEVEL_WARNING, "Couldn't load \"" + filename + "\"");
        }
        return;
    }

    auto curIC = std::make_shared<InputCluster>(Rand, maxChunks, maxChunkLen);

    try {
        curIC->Deserialize(*filedata);
    } catch ( DeserializationException ) {
        if ( loggerChain != std::nullopt ) {
            (*loggerChain)->AddInfo(Logger::LOG_LEVEL_WARNING, "Couldn't deserialize \"" + filename + "\"");
        }
        return;
    }

    corpus->AddInputCluster(curIC);
}

void Manager::Process(const bool addToCorpus) {
    bool msgShown = false;

    for ( const auto& fn : files ) {
        const auto isDirectory = util::IsDirectory(fn);

        if ( isDirectory == true ) {
            const auto files_in_directory = util::FilesInDirectory(fn);
            if ( addToCorpus == true ) {
                for ( const auto& fn : files_in_directory ) {
                    loadInputCluster(fn);
                }
            }

            if ( workingDirectory.empty() == false ) {
                if ( loggerChain != std::nullopt && msgShown == false ) {
                    (*loggerChain)->AddInfo(Logger::LOG_LEVEL_INFO, "Multiple directories specified - will be using \"" + workingDirectory + "\" as working directory");
                    msgShown = true;
                }
            } else {
                workingDirectory = fn;
            }

            directorySpecified = true;
        } else {
            if ( addToCorpus == true ) {
                loadInputCluster(fn);
            }

            filesSpecified = true;
        }
    }

    isProcessed = true;
}

std::string Manager::GetWorkingDirectory(void) const {
    if ( isProcessed == false ) {
        /* TODO proper exception */
        abort();
    }

    return workingDirectory;
}

std::shared_ptr<Corpus> Manager::GetCorpus(void) {
    return corpus;
}

} /* namespace corpus */
} /* namespace container */
} /* namespace vfuzz */
