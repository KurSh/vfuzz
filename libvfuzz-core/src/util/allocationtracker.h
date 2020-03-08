#pragma once

#include <cstddef>
#include <map>

namespace vfuzz {

class AllocationTracker
{
    public:
        AllocationTracker(void);
        ~AllocationTracker(void) = default;
        void Enable(void);
        void Disable(void);
        void AddMalloc(const void* ptr, const size_t num);
        void AddFree(const void* ptr);
        void Reset(void);
        size_t GetMax(void);
        void CalcMax2(void);
        void CalcMax(void);
    private:
        std::map<const void*, size_t> allocMap;
        bool enabled;
        size_t curAlloc;
        size_t allTimeMax;
};

} /* namespace vfuzz */
