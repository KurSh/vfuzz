#include <logger/consolelogger.h>

namespace vfuzz {

ConsoleLogger::ConsoleLogger(const bool noColorCodes) :
    TextLogger(noColorCodes)
{
}

void ConsoleLogger::flush(void) const
{
    std::cout << getFlush() << std::flush;
}

} /* namespace vfuzz */
