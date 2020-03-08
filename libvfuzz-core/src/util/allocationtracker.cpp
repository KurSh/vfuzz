#include <util/allocationtracker.h>
#include <base/exception.h>
#include <unistd.h>

namespace vfuzz {

AllocationTracker::AllocationTracker(void) :
    enabled(false), curAlloc(0), allTimeMax(0) { }

void AllocationTracker::Enable(void)
{
    enabled = true;
}

void AllocationTracker::Disable(void)
{
    enabled = false;
}

void AllocationTracker::AddMalloc(const void* ptr, const size_t num)
{
    if ( enabled == false ) {
        return;
    }

    if ( allocMap.count(ptr) != 0 ) {
        return;
    }

    allocMap[ptr] = num;

    curAlloc += num;

    if ( curAlloc > allTimeMax ) {
        allTimeMax = curAlloc;
    }
}

void AllocationTracker::AddFree(const void* ptr)
{
    if ( enabled == false ) {
        return;
    }

    if ( allocMap.count(ptr) == 0 ) {
        return;
    }

    curAlloc -= allocMap.at(ptr);
    allocMap.erase(ptr);
}

void AllocationTracker::Reset(void)
{
    allocMap.clear();
    curAlloc = 0;
}

size_t AllocationTracker::GetMax(void)
{
    return allTimeMax;
}

} /* namespace vfuzz */
