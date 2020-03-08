#pragma once

#include <dictionary/base.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <util/random.h>
#include <base/types.h>

namespace vfuzz {
namespace dictionary {

class AutoDict : public Base {
    private:
        /* XXX from libFuzzer */
        template<class T, size_t kSizeT>
            struct TableOfRecentCompares {
                static const size_t kSize = kSizeT;
                struct Pair {
                    T A, B;
                };
                void Insert(size_t Idx, const T &Arg1, const T &Arg2) {
                    Idx = Idx % kSize;
                    Table[Idx].A = Arg1;
                    Table[Idx].B = Arg2;
                }

                Pair Get(size_t I) const { return Table[I % kSize]; }

                Pair Table[kSize];
            };
        static TableOfRecentCompares<uint32_t, 32> TORC4;
        static TableOfRecentCompares<uint64_t, 32> TORC8;
        //TableOfRecentCompares<size_t, 32> TORCW;
    public:
        AutoDict(std::shared_ptr<util::Random> Rand);
        static void InsertTorc(const size_t Arg1, const size_t Arg2, const size_t length);
        Chunk GetRandom(void) const override;
};

} /* namespace dictionary */
} /* namespace vfuzz */
