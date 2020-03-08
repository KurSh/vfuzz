#pragma once

#include <mutator/base.h>

namespace vfuzz {
namespace mutator {

template <bool isRanged>
class Ranged : public Base {
    protected:
        std::pair<size_t, size_t> GetRange(const size_t size) {
            if ( isRanged == false ) {
                return {Rand->Get(size), 1};
            } else {
                auto From = Rand->Get(size);
                auto To = Rand->Get(size);
                if ( From == To ) {
                    return {From, 1};
                }
                if ( From > To ) {
                    std::swap(From, To);
                }
                return {From, Rand->Get(To - From)};
            }
        }
    public:
        Ranged(std::shared_ptr<util::Random> Rand, const std::string name, std::optional<History*> history = {}) :
            Base(Rand, name + (isRanged ? "Ranged" : ""), history) {
        }
};

template <bool isRanged, bool haveModifier>
class DefaultRanged : public Ranged<isRanged> {
    protected:
        Chunk MutateImpl(const GeneratorID gid, const Chunk& input, Chunk& insert, const size_t maxChunkLen) {
            /* maxChunkLen is not relevant - chunk length doesn't change */
            (void)maxChunkLen;
            (void)insert;

            if ( input.empty() == true ) {
                return input;
            }

            Chunk ret = input;
            const uint8_t modifier = haveModifier ? Base::RandCh() : 0;
            const auto range = Ranged<isRanged>::GetRange(ret.size());
            for (size_t i = range.first; i < range.first + range.second; i++) {
                ret[i] = process(ret[i], modifier);
            }

            auto description = desc(modifier);
            if ( isRanged == true ) {
                description += "-" + std::to_string(range.second);
            }
            Base::LogHistory(gid, description);
            return ret;
        }
        virtual uint8_t process(const uint8_t in, const uint8_t modifier) = 0;
        virtual std::string desc(const uint8_t modifier) {
            (void)modifier;
            return {};
        }
    public:
        DefaultRanged(std::shared_ptr<util::Random> Rand, const std::string name, std::optional<History*> history = {}) :
            Ranged<isRanged>(Rand, name, history) {
        }
};

} /* namespace mutator */
} /* namespace vfuzz */
