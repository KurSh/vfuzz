#pragma once

#include <functional>

namespace vfuzz {
namespace util {

template <typename T>
class PushPullCallback {
    private:
        const std::function<void(T t)> push;
        const std::function<T(void)> pull;
    public:
        PushPullCallback(const std::function<void(T t)> push, const std::function<T(void)> pull) :
            push(push), pull(pull) { }
        void Push(T t) { push(t); }
        T Pull(void) { return pull(); }
};

} /* namespace util */
} /* namespace vfuzz */
