#pragma once

#include <logger/logger.h>
#include <string>

namespace vfuzz {

class TextLogger : public Logger {
    private:
        std::string toGreen(const std::string& in) const;
        std::string toYellow(const std::string& in) const;
        std::string toRed(const std::string& in) const;
        std::string toItalic(const std::string& in) const;
        std::string toBoldWhite(const std::string& in) const;
        std::string toBoldWhite(const size_t in) const;
        std::string toUnderline(const std::string& in) const;
        std::string timeAndCategory(const std::string category) const;
        std::string getHeader(const std::string category) const;
        std::string toColor(const std::string& in, const SensorID ID) const;
        bool noColorCodes;
    protected:
        std::string getFlush(void) const;
    public:
        TextLogger(const bool noColorCodes = false);
};

} /* namespace vfuzz */
