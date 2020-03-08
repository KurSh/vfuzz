#include <sensor/processor/highest.h>
#include <logger/consolelogger.h>
#include <container/corpus/corpus.h>
#include <util/random.h>
#include <iostream>

int main(void)
{
    using namespace vfuzz;

    auto Rand = std::make_shared<util::Random>(0);
    container::InputCluster ic(Rand, 10, 10);
    auto corpus = std::make_shared<container::corpus::Corpus>(Rand, ic);

    auto loggers = std::make_shared<LoggerChain>();
    auto consolelogger = std::make_shared<ConsoleLogger>();
    loggers->AddLogger(consolelogger);

    vfuzz::sensor::ProcessorHighest p(loggers);

    consolelogger->Start();

    for (const auto& v : {123, 999, 3}) {
        p.ReceiveInput(123, v);
    }

    consolelogger->Stop();

    std::cout << "Number of new values: " << p.GetNumValues() << std::endl;

    std::cout << "Values are: " << std::endl;
    p.GetValues([](const uint64_t v) {
            std::cout << "\t" << v << std::endl;
    });

    return 0;
}
