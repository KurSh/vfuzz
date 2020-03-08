#include <exec/execmanager.h>
#include <logger/loggerchain.h>
#include <logger/consolelogger.h>
#include <memory>

int main(int argc, char** argv)
{
    using namespace vfuzz;

    auto Rand = std::make_shared<util::Random>(4);
    auto loggerChain = std::make_shared<LoggerChain>();
    loggerChain->AddLogger( std::make_shared<ConsoleLogger>() );
    auto gf = std::make_unique<ExecManager>(Rand, loggerChain);

    if ( gf->Initialize(argc, argv) == true ) {
        gf->Run();
    }

    return 0;
}
