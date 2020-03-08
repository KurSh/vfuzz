#pragma once

#include <random>
#include <stdlib.h>

namespace vfuzz {
namespace util {

#define PRNG std::minstd_rand0

class Random : public PRNG {
 public:
  Random(unsigned int seed) : PRNG(seed) {}
  result_type operator()() { return this->PRNG::operator()(); }
  size_t Get(void) {
      return operator()();
  }

  size_t Get(size_t n) {
      return n ? Get() % n : 0;
  }

  intptr_t Get(intptr_t From, intptr_t To) {
      intptr_t RangeSize = To - From + 1;
      return Get(RangeSize) + From;
  }

  size_t RandBool() { return Get() % 2; }
};

#undef PRNG

} /* namespace util */
} /* namespace vfuzz */
