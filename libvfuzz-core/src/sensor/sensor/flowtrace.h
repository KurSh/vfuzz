#pragma once

#include <sensor/sensor/base.h>
#include <set>

namespace vfuzz {
namespace sensor {

template <int TrackSize>
class FlowTracker {
    private:
        uintptr_t callers[TrackSize] = {};
        size_t index = 0;
    public:
        FlowTracker(void) = default;
        void Reset(void) {
            for (size_t i = 0; i < TrackSize; i++) {
                callers[i] = 0;
            }
        }

        void Add(const uintptr_t PC) {
            index++;
            if ( index == TrackSize ) {
                index = 0;
            }
            callers[index] = PC;
        }

        std::vector<uintptr_t> GetTrack(void) const {
            std::vector<uintptr_t> ret;

            size_t curIndex = index;
            for (size_t i = 0; i < TrackSize; i++) {
                ret.push_back( callers[curIndex] );
                if ( curIndex == 0 ) {
                    curIndex = TrackSize;
                }
                curIndex--;
            }

            return ret;
        }
};

class FlowtraceSensor : public BaseSensor
{
    private:
        std::set<std::vector<uintptr_t>> stackTraces;
        FlowTracker<3> flowTracker;
        void start(void) override;
    public:
        FlowtraceSensor(const SensorID ID);
        void Update(const uintptr_t caller);
        enum BuiltinType GetBuiltinType(void) const override;
};

} /* namespace sensor */
} /* namespace vfuzz */
