#pragma once

#include <exception>
#include <string>

namespace vfuzz {

class Exception : public std::exception {
    private:
        const std::string msg;
    public:
        const char * what () const throw ()
        {
            return msg.c_str();
        }
        Exception(const std::string msg) : std::exception(), msg(msg) { }
};

class DeserializationException : public Exception {
    public:
        DeserializationException() : Exception("Deserialization exception") { }
};

} /* namespace vfuzz */
