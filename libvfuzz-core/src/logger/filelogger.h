#pragma once
#include <logger/textlogger.h>
#include <memory>

namespace vfuzz {

class FileLogger : public TextLogger {
    private:
        void flush(void) const override;
        std::unique_ptr<std::ofstream> filedesc;
    public:
        FileLogger(const std::string& filename);
        ~FileLogger();
};

} /* namespace vfuzz */
