#include <sys/time.h>
#include <sys/resource.h>
#include <util/util.h>

namespace vfuzz {
namespace util {

/* From libFuzzer */
size_t GetPeakRSSMb() {
  struct rusage usage;
  if (getrusage(RUSAGE_SELF, &usage))
    return 0;
  return usage.ru_maxrss >> 10;
}

} /* namespace util */
} /* namespace vfuzz */
