#include <exec/execmanager.h>
#include <mutator/util.h>
#include <util/io.h>
#include <base/exception.h>
#include <base/types.h>
#include <boost/format.hpp>
#include <iostream>

#include <logger/jsonlogger.h>
#include <logger/filelogger.h>

#include <util/conduit.h>

#include <mutator/copy.h>
#include <mutator/copypart.h>
#include <mutator/crossover.h>
#include <mutator/erasebytes.h>
#include <mutator/insertbyte.h>
#include <mutator/insertdictionary.h>
#include <mutator/insertrepeatedbytes.h>
#include <mutator/random.h>
#include <mutator/range_based.h>
#include <mutator/shufflebytes.h>

#include <mutator/util.h>

#include <interface/malloc_hooks.h>

namespace vfuzz {

ExecManager::ExecManager(std::shared_ptr<util::Random> Rand, std::shared_ptr<LoggerChain> loggerChain) :
    Rand(Rand),
    mutators(mutator::GetDefaultContainer(Rand, std::optional<mutator::History*>(&mutatorHistory))),
    loggerChain(loggerChain),
    dictionary(std::make_shared<dictionary::Dictionary>(Rand)),
    autodict(std::make_shared<dictionary::AutoDict>(Rand)) {
}

void ExecManager::processInsertions(const std::vector<std::shared_ptr<container::Input>>& inputs) {
    /* Get top insertions and add them to the dictionary */
    std::set<Chunk> topInsertions = mutator::GetTopInsertions(inputs, 10);

    for ( const auto insertion : topInsertions ) {
        dictionary->Add(dictionary::kDictionaryTypeMutator, insertion);
    }
}

void ExecManager::onNewInputCluster(const std::shared_ptr<container::InputCluster> inputCluster) const
{
    const auto workingDirectory = corpusManagerExec->GetWorkingDirectory();

    if ( workingDirectory.size() > 0 ) {
        util::WriteInputCluster(workingDirectory, "", inputCluster, loggerChain);
    }
}

void ExecManager::onCrash(const std::string prefix, const std::shared_ptr<container::InputCluster> inputCluster, const bool doExit) const
{
    util::WriteInputCluster(".", prefix, inputCluster, loggerChain);
    if ( doExit == true ) {
        loggerChain->Stop();
        _Exit(1);
    }
}

void ExecManager::initializeMutators(void) {
    const auto argMutatorHistory = std::optional<mutator::History*>(&mutatorHistory);
    {
        auto conduit = util::Conduit<std::shared_ptr<dictionary::Dictionary>>(dictionary,
                [](const std::shared_ptr<dictionary::Dictionary> dictionary, const void* param) -> Chunk {

                (void)param;
                return dictionary->GetRandom();

                });
        mutators->Add( std::make_shared<mutator::InsertDictionary<std::shared_ptr<dictionary::Dictionary>>>(Rand, conduit, "dictionary", false, argMutatorHistory) );
    }

    {
        auto conduit = util::Conduit<std::shared_ptr<container::corpus::Corpus>>(corpus,
                [](const std::shared_ptr<container::corpus::Corpus> corpus, const void* param) -> Chunk {

                const auto IC = corpus->GetRandomPtr();
                const auto I = IC->Get( *(static_cast<const size_t*>(param)) );
                return I->GetRandom();

                });
        mutators->Add( std::make_shared<mutator::CrossOver<std::shared_ptr<container::corpus::Corpus>>>(Rand, conduit, argMutatorHistory) );
    }

    if ( corpusManagerInput->GetCorpus()->Size() > 0 ) {
        auto conduit = util::Conduit<std::shared_ptr<container::corpus::Corpus>>(corpusManagerInput->GetCorpus(),
                [](const std::shared_ptr<container::corpus::Corpus> corpus, const void* param) -> Chunk {

                const auto IC = corpus->GetRandomPtr();
                const auto I = IC->Get( *(static_cast<const size_t*>(param)) );
                auto ret = I->GetRandom();
                return I->GetRandom();
                });
        mutators->Add( std::make_shared<mutator::InsertDictionary<std::shared_ptr<container::corpus::Corpus>>>(Rand, conduit, "input corpora", true, argMutatorHistory) );
    }

    {
        auto conduit = util::Conduit<std::shared_ptr<dictionary::AutoDict>>(autodict,
                [](const std::shared_ptr<dictionary::AutoDict> autodict, const void* param) -> Chunk {
                (void)param;

                return autodict->GetRandom();

                });
        mutators->Add( std::make_shared<mutator::InsertDictionary<std::shared_ptr<dictionary::AutoDict>>>(Rand, conduit, "autodict", false, argMutatorHistory) );
    }

    {
        auto conduit = util::Conduit<std::shared_ptr<container::corpus::Corpus>>(corpus,
                [](const std::shared_ptr<container::corpus::Corpus> corpus, const void* param) -> Chunk {

                const auto IC = corpus->GetRandomPtr();
                const auto I = IC->Get( *(static_cast<const size_t*>(param)) );
                return I->GetRandom();

                });
        mutators->Add( std::make_shared<mutator::Copy<std::shared_ptr<container::corpus::Corpus>>>(Rand, conduit, argMutatorHistory) );
    }
}

void ExecManager::initializeCallbacks(void) {
    executor->SetOnNew( std::bind(&ExecManager::onNewInputCluster, this, std::placeholders::_1) );

    executor->SetOnCrash( std::bind(&ExecManager::onCrash, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) );

    corpus->SetOnNew( std::bind(&LoggerChain::AddInputCluster, loggerChain, std::placeholders::_1, &mutatorHistory) );
}

bool ExecManager::Initialize(int argc, char** argv)
{
    auto options = std::make_unique<options::Options>();
    {
        const bool doExit = options->Parse(argc, argv);
        if ( doExit == true ) {
            return false;
        }
    }

    install_malloc_hooks();

    runStatic = options->GetExecutionMode()->Static();
    runDynamic = options->GetExecutionMode()->Dynamic();

    corpus = std::make_shared<container::corpus::Corpus>(Rand, container::InputCluster(Rand, options->MaxChunks(), options->MaxChunkLen()));

    if ( options->LogFilePath().empty() == false ) {
        loggerChain->AddLogger( std::make_shared<FileLogger>(options->LogFilePath()) );
    }

    if ( options->JsonPath().empty() == false ) {
        loggerChain->AddLogger( std::make_shared<JSONLogger>(options->JsonPath()) );
    }

    loggerChain->Start();

    corpusManagerExec = std::make_unique<container::corpus::Manager>(Rand, options->MaxChunks(), options->MaxChunkLen(), options->GetExecCorpora(), loggerChain);
    corpusManagerExec->Process(options->GetExecutionMode()->Static() ? true : false);

    corpusManagerInput = std::make_unique<container::corpus::Manager>(Rand, options->MaxChunks(), options->MaxChunkLen(), options->GetInputCorpora(), loggerChain);
    corpusManagerInput->Process(true);

    initializeMutators();

    numRuns = options->NumRuns();

    executor = std::make_unique<Executor>(Rand,
            corpus->GetBidirectionalCallback(),
            mutators,
            options->MaxChunks(),
            options->MaxChunkLen(),
            options->MaxMutations(),
            dictionary,
            loggerChain);

    executor->Initialize(argc, argv);
    initializeCallbacks();

    loggerChain->Stop();
    isInitialized = true;

    return true;
}

void ExecManager::Run(void) {
    if ( isInitialized == false ) {
        throw std::runtime_error("ExecManager::Run() called before initialization");
    }

    /* Run all inputs already in the corpus (loaded from disk) in static mode */
    if ( runStatic == true ) {
        const auto corpusDisk = corpusManagerExec->GetCorpus();
        const auto corpusSize = corpusDisk->Size();
        for (size_t i = 0; i < corpusSize; i++) {
            auto IC = corpusDisk->Get(i);
            loggerChain->Start();
            loggerChain->AddInfo(Logger::LOG_LEVEL_INFO, "Running " + std::to_string(i) + "/" + std::to_string(corpusSize));
            executor->RunInputCluster(IC);
            loggerChain->Stop();
            loggerChain->SetCorpusSize(corpus->Size());
        }
    }

    if ( runDynamic == false ) {
        return;
    }

    for (uint64_t curRuns = 0; numRuns ? curRuns < numRuns : true; curRuns++) {
        mutatorHistory.Clear();
        loggerChain->Start();

        if ( curRuns == 0 ) {
            loggerChain->AddInfo(Logger::LOG_LEVEL_INFO, "Corpus size is " + std::to_string(corpus->Size()));
        }

        /* Run once */
        executor->Run();

        loggerChain->SetCorpusSize(corpus->Size());
        loggerChain->Stop();
    }
}

} /* namespace vfuzz */
