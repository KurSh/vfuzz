#pragma once

#include <util/random.h>
#include <base/types.h>
#include <memory>

namespace vfuzz {
namespace dictionary {

class Base {
    protected:
        std::shared_ptr<util::Random> Rand;
    public:
        Base(std::shared_ptr<util::Random> Rand);
        virtual ~Base() = default;
        virtual Chunk GetRandom(void) const = 0;
};

} /* namespace dictionary */
} /* namespace vfuzz */
