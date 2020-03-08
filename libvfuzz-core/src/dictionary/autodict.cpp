#include <dictionary/autodict.h>

namespace vfuzz {
namespace dictionary {

AutoDict::TableOfRecentCompares<uint32_t, 32> AutoDict::TORC4 = {};
AutoDict::TableOfRecentCompares<uint64_t, 32> AutoDict::TORC8 = {};

AutoDict::AutoDict(std::shared_ptr<util::Random> Rand) :
    Base(Rand)
{
}

void AutoDict::InsertTorc(const size_t Arg1, const size_t Arg2, const size_t length) {
    const uint64_t ArgXor = Arg1 ^ Arg2;

    if (length == 4) {
        TORC4.Insert(ArgXor, Arg1, Arg2);
    } else if (length == 8) {
        TORC8.Insert(ArgXor, Arg1, Arg2);
    } else {
        /* Nothing */
    }
}

Chunk AutoDict::GetRandom(void) const {
    if ( Rand->RandBool() ) {
        const auto valpair = TORC4.Get( Rand->Get() );
        const auto val = Rand->RandBool() ? valpair.A : valpair.B;
        return Chunk(&val, &val + 1);
    } else {
        const auto valpair = TORC8.Get( Rand->Get() );
        const auto val = Rand->RandBool() ? valpair.A : valpair.B;
        return Chunk(&val, &val + 1);
    }
}

} /* namespace dictionary */
} /* namespace vfuzz */
