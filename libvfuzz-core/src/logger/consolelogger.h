#pragma once

#include <logger/textlogger.h>

namespace vfuzz {

/* TODO derive ConsoleLogger from FileLogger */
class ConsoleLogger : public TextLogger {
    private:
        void flush(void) const override;
    public:
        ConsoleLogger(const bool noColorCodes = false);
};

} /* namespace vfuzz */
