#pragma once

#include <logger/logger.h>

namespace vfuzz {

class NoopLogger : public Logger {
    private:
        void flush(void) const override { };
    public:
        NoopLogger(void) { };
};

} /* namespace vfuzz */
