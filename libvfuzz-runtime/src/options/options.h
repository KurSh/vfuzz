#pragma once

#include <vector>
#include <string>
#include <memory>

namespace vfuzz {
namespace options {

using FileList = std::vector<std::string>;

class ExecutionMode {
    private:
        const bool dynamic, _static;
    public:
        ExecutionMode(const bool dynamic, const bool _static);
        bool Static(void) const;
        bool Dynamic(void) const;
};

class Options {
    private:
        size_t maxChunks, maxChunkLen, maxMutations, numJobs;
        uint64_t numRuns;
        uint32_t randSeed;
        FileList execCorpora;
        FileList inputCorpora;
        std::string logFilePath, jsonPath, execModeStr;
        std::shared_ptr<ExecutionMode> execMode = nullptr;

        bool isParsed;
        void parseExecMode(void);
    public:
        Options(void);
        ~Options(void) = default;
        bool Parse(int argc, char** argv);
        void checkIfParsed(void) const;
        size_t MaxChunks(void) const;
        size_t MaxChunkLen(void) const;
        size_t MaxMutations(void) const;
        size_t NumJobs(void) const;
        uint64_t NumRuns(void) const;
        std::vector<std::string> GetExecCorpora(void) const;
        std::vector<std::string> GetInputCorpora(void) const;
        std::string LogFilePath(void) const;
        std::string JsonPath(void) const;
        const std::shared_ptr<ExecutionMode> GetExecutionMode(void) const;
};

} /* namespace options */
} /* namespace vfuzz */
