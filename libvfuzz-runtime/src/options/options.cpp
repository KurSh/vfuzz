#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <base/exception.h>
#include <options/options.h>
#include <util/io.h>
#include <util/string/string.h>
#include <cxxopts.hpp>
#include <utility>
#include <algorithm>
#include <cassert>

namespace vfuzz {
namespace options {

static void plath(void)
{
#define MUL 1.6
    printf("\n\n");
    printf("\t");
    printf("I "); fflush(stdout); usleep(100000*MUL);
    printf("took "); fflush(stdout); usleep(100000*MUL);
    printf("a "); fflush(stdout); usleep(100000*MUL);
    printf("deep "); fflush(stdout); usleep(100000*MUL);
    printf("breath "); fflush(stdout); usleep(350000*MUL);

    printf("and "); fflush(stdout); usleep(100000*MUL);
    printf("listened "); fflush(stdout); usleep(100000*MUL);
    printf("to "); fflush(stdout); usleep(100000*MUL);
    printf("the "); fflush(stdout); usleep(100000*MUL);
    printf("old "); fflush(stdout); usleep(100000*MUL);
    printf("bray "); fflush(stdout); usleep(100000*MUL);
    printf("of "); fflush(stdout); usleep(100000*MUL);
    printf("my "); fflush(stdout); usleep(100000*MUL);
    printf("heart "); fflush(stdout); usleep(100000*MUL);

    usleep(500000*MUL);
    printf("\n");
    usleep(500000*MUL);

    printf("\n\t\t\tI "); fflush(stdout); usleep(100000*MUL);
    printf("am "); fflush(stdout); usleep(100000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    usleep(500000*MUL);

    printf("\n\t\t\t\tI "); fflush(stdout); usleep(100000*MUL);
    printf("am "); fflush(stdout); usleep(100000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    usleep(500000*MUL);

    printf("\n\t\t\t\t\tI "); fflush(stdout); usleep(100000*MUL);
    printf("am "); fflush(stdout); usleep(100000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    printf(". "); fflush(stdout); usleep(80000*MUL);
    usleep(500000*MUL);

    printf("\n\n");

    usleep(500000*MUL);
#undef MUL
}

ExecutionMode::ExecutionMode(const bool dynamic, const bool _static) :
    dynamic(dynamic), _static(_static) {
}

bool ExecutionMode::Static(void) const {
    return _static;
}

bool ExecutionMode::Dynamic(void) const {
    return dynamic;
}

Options::Options(void) {
}

void Options::parseExecMode(void) {
    assert(execMode == nullptr);

    util::string::RemoveWhitespace(execModeStr);
    util::string::ToLowercase(execModeStr);

    const auto parts = util::string::Split(execModeStr, ",");

    bool haveDynamic, haveStatic, haveNone;
    {
        std::optional<bool> haveDynamic_opt, haveStatic_opt, haveNone_opt;

        for (const auto& P : parts) {
            if ( P == "dynamic" ) {
                haveDynamic_opt = true;
            } else if ( P == "static" ) {
                haveStatic_opt = true;
            } else if ( P == "none" ) {
                haveNone_opt = true;
            } else {
                std::cout << "Error: invalid execution mode specified: " << P << std::endl;
                exit(0);
            }
        }
        haveDynamic = haveDynamic_opt != std::nullopt && *haveDynamic_opt == true;
        haveStatic = haveStatic_opt != std::nullopt && *haveStatic_opt == true;
        haveNone = haveNone_opt != std::nullopt && *haveNone_opt == true;
    }

    if ( haveNone == true ) {
        if ( haveDynamic || haveStatic ) {
            std::cout << "Error: both none and dynamic/static execution modes specified" << std::endl;
            exit(0);
        }
    }

    execMode = std::make_shared<ExecutionMode>(haveDynamic, haveStatic);
}

bool Options::Parse(int argc, char** argv) {
    try {
        cxxopts::Options options("vfuzz usage");
        options
            .positional_help("[optional args]")
            .show_positional_help();

        options.add_options()
            ("h,help", "Display this help message")
            ("c,max-chunks", "Maximum # of chunks per generator", cxxopts::value<size_t>(maxChunks)->default_value("100"))
            ("C,max-chunk-len", "Maximum length per chunk", cxxopts::value<size_t>(maxChunkLen)->default_value("8192"))
            ("log-file", "Log file path", cxxopts::value<std::string>(logFilePath)->default_value(""))
            ("json-path", "JSON output path", cxxopts::value<std::string>(jsonPath)->default_value(""))
            ("max-mutations", "Maximum number of mutations", cxxopts::value<size_t>(maxMutations)->default_value("5"))
            ("files", "File(s)", cxxopts::value<FileList>(execCorpora))
            ("jobs", "Number of threads", cxxopts::value<size_t>(numJobs)->default_value("1"))
            ("seed", "PRNG seed", cxxopts::value<uint32_t>(randSeed)->default_value("0"))
            ("num-runs", "Number of runs (0 for infinite)", cxxopts::value<uint64_t>(numRuns)->default_value("0"))
            ("exec-mode", "Execution mode. One or more of the following: dynamic, static, none", cxxopts::value<std::string>(execModeStr)->default_value("dynamic,static"))
            ("I,input-corpora", "Paths to input corpora (comma-separated)", cxxopts::value<FileList>(inputCorpora))
            ("plath", "Sylvia Plath");

        options.parse_positional({"files"});
        auto result = options.parse(argc, argv);

        if ( result.count("help") ) {
            std::cout << options.help({""}) << std::endl;
            return true;
        }

        if ( result.count("plath") ) {
            plath();
        }

        if ( jsonPath.empty() == false ) {
            if ( util::IsDirectory(jsonPath) == false ) {
                std::cout << "Error: cannot open directory " << jsonPath << std::endl;
                exit(0);
            }

            if ( util::FilesInDirectory(jsonPath).empty() == false ) {
                std::cout << "Error: directory " << jsonPath << " is not empty " << std::endl;
                exit(0);
            }
        }

        if ( numJobs == 0 || numJobs > 1024 ) {
            std::cout << "Error: invalid number of threads specified" << std::endl;
            exit(0);
        }

        parseExecMode();
    } catch (const cxxopts::OptionException& e) {
        std::cout << "Error parsing options: " << e.what() << std::endl;
        exit(1);
    }

    isParsed = true;

    return false;
}

void Options::checkIfParsed(void) const {
    if ( this->isParsed == false ) {
        throw vfuzz::Exception("Tried to get options before parsing");
    }
}

/* Option getters */

size_t Options::MaxChunks(void) const { checkIfParsed(); return this->maxChunks; }
size_t Options::MaxChunkLen(void) const { checkIfParsed(); return this->maxChunkLen; }
std::vector<std::string> Options::GetExecCorpora(void) const { checkIfParsed(); return this->execCorpora; }
std::vector<std::string> Options::GetInputCorpora(void) const { checkIfParsed(); return this->inputCorpora; }
std::string Options::LogFilePath(void) const { checkIfParsed(); return this->logFilePath; }
std::string Options::JsonPath(void) const { checkIfParsed(); return this->jsonPath; }
size_t Options::MaxMutations(void) const { checkIfParsed(); return this->maxMutations; }
size_t Options::NumJobs(void) const { checkIfParsed(); return this->numJobs; }
uint64_t Options::NumRuns(void) const { checkIfParsed(); return this->numRuns; }

const std::shared_ptr<ExecutionMode> Options::GetExecutionMode(void) const {
    return execMode;
}

} /* namespace options */
} /* namespace vfuzz */
