#include <dictionary/base.h>

namespace vfuzz {
namespace dictionary {

Base::Base(std::shared_ptr<util::Random> Rand) :
    Rand(Rand)
{
}

} /* namespace dictionary */
} /* namespace vfuzz */
