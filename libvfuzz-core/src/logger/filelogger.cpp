#include <logger/filelogger.h>

namespace vfuzz {

FileLogger::FileLogger(const std::string& filename) :
    TextLogger(true), filedesc(std::make_unique<std::ofstream>())
{
    filedesc->open(filename, std::ios::out);
    if ( filedesc->is_open() == false ) {
        /* TODO throw */
    }
}

void FileLogger::flush(void) const
{
    *filedesc << getFlush() << std::flush;
}

FileLogger::~FileLogger() {
    filedesc->close();
}

} /* namespace vfuzz */

