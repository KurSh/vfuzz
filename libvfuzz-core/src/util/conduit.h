#pragma once

#include <functional>
#include <base/types.h>

namespace vfuzz {
namespace util {

/* This class implements a generic unidirectional "getter" between two classes,
 * to prevent having to grant the classes full access to each other.
 */
template <class T>
class Conduit {
    private:
        const T ptr;
        const std::function<Chunk(const T ptr, const void* param)> fn;
    public:
        Conduit(const T ptr, const std::function<Chunk(const T ptr, const void* param)> fn) :
            ptr(ptr), fn(fn) {
        }
        Chunk Get(const void* param = nullptr) const {
            return fn(ptr, param);
        }
};

} /* namespace util */
} /* namespace vfuzz */
