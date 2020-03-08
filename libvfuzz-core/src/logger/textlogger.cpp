#include <logger/textlogger.h>
#include <util/util.h>
#include <iostream>
#include <sstream>

namespace vfuzz {

TextLogger::TextLogger(const bool noColorCodes) :
    Logger(), noColorCodes(noColorCodes)
{
}

std::string TextLogger::toGreen(const std::string& in) const
{
    if ( noColorCodes == true ) return in;
    return "\033[32m" + in + "\033[0m";
}

std::string TextLogger::toYellow(const std::string& in) const
{
    if ( noColorCodes == true ) return in;
    return "\033[93m" + in + "\033[0m";
}

std::string TextLogger::toRed(const std::string& in) const
{
    if ( noColorCodes == true ) return in;
    return "\033[31m" + in + "\033[0m";
}

std::string TextLogger::toItalic(const std::string& in) const
{
    if ( noColorCodes == true ) return in;
    return "\e[3m" + in + "\e[0m";
}

std::string TextLogger::toBoldWhite(const std::string& in) const
{
    if ( noColorCodes == true ) return in;
    return "\033[1m\033[37m" + in + "\033[0m";
}

std::string TextLogger::toBoldWhite(const size_t in) const
{
    return toBoldWhite(std::to_string(in));
}

std::string TextLogger::toUnderline(const std::string& in) const
{
    if ( noColorCodes == true ) return in;
    return "\e[4m" + in + "\e[0m";
}

std::string TextLogger::timeAndCategory(const std::string category) const {
    std::ostringstream out;
    out << getTimeString() << " ";
    out << "[" << toGreen(category) << "] ";
    return out.str();
}

std::string TextLogger::getHeader(const std::string category) const
{
    std::ostringstream out;
    out << timeAndCategory( toGreen(category) );
    out << toUnderline("N") << " " << getNumExecs() << " ";
    out << toUnderline("E/s") << " " << getExecsPerSecond() << " ";
    out << toUnderline("RSS") << " " << util::GetPeakRSSMb();
    if ( getCorpusSize() > getPrevCorpusSize() ) {
        out << ", " << toUnderline("C") << " " << getPrevCorpusSize() << " -> " << getCorpusSize();
    }

    return out.str();
}

std::string TextLogger::toColor(const std::string& in, const SensorID ID) const
{
    if ( noColorCodes == true ) return in;

    switch ( (size_t)(ID) % 3 ) {
        case    0:
            return "\033[33m" + in + "\033[0m";
            break;
        case    1:
            return "\033[34m" + in + "\033[0m";
            break;
        case    2:
            return "\033[36m" + in + "\033[0m";
            break;
    }

    return in;
}

std::string TextLogger::getFlush(void) const
{
    std::ostringstream out;
    std::set<SensorID> IDs;
    std::map<SensorID, std::string> id_name_map;
    std::map<SensorID, size_t> lowest_per_id;
    std::map<SensorID, size_t> highest_per_id;
    std::map<SensorID, size_t> steps_per_id;

    for (const auto& sud : queue_sensorUpdateData ) {
        const auto ID = sud->ID();
        if ( IDs.count(ID) == 0 ) {
            lowest_per_id[ID] = sud->From();
        } else {
            lowest_per_id[ID] = std::min(lowest_per_id[ID], sud->From());
        }
        IDs.insert(ID);
        if ( sud->Name().empty() == true ) {
            id_name_map[ID] = std::to_string(ID);
        } else {
            id_name_map[ID] = sud->Name();
        }
        highest_per_id[ID] = std::max(highest_per_id[ID], sud->To());
        steps_per_id[ID]++;
    }

    bool printed = false;
    bool change = false;
    if ( IDs.empty() == false ) {
        out << getHeader("CHANGE");

        for ( const auto& ID : IDs ) {
            out << " | " << toItalic(toColor(id_name_map[ID], ID)) <<
                   " " << toBoldWhite(lowest_per_id[ID]) <<
                   " -> " << toBoldWhite(highest_per_id[ID]) <<
                   " (" << toBoldWhite(steps_per_id[ID]) <<
                   ")";
        }

        if ( mutatorHistory ) {
            out << " " << toBoldWhite("<") << " " << (*mutatorHistory)->ToString();
        }

        change = true;
        printed = true;
    }

    if ( change == false && doPulse() ) {
        out << getHeader("PULSE");
        printed = true;
    }

    if ( printed == true ) {
        out << std::endl;
    }

    for (const auto& info : queue_info) {
        switch ( info.first ) {
            case    LOG_LEVEL_INFO:
                out << timeAndCategory(toGreen("INFO"));
                break;
            case    LOG_LEVEL_WARNING:
                out << timeAndCategory(toYellow("WARNING"));
                break;
            case    LOG_LEVEL_ERROR:
                out << timeAndCategory(toRed("ERROR"));
                break;
        }
        out << info.second << std::endl;
    }

    return out.str();
}

} /* namespace vfuzz */
