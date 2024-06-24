#include "hp.h"

#include <string.h>
#include <sys/mman.h>
#include <cerrno>
#include <stdexcept>
#include <string>

template <typename T>
__attribute__((cold)) constexpr T roundUp(T numToRound, T multiple) {
  return ((numToRound + multiple - 1) / multiple) * multiple;
}

__attribute__((cold)) void enableHugePages(void* buffer, const size_t bufferSize) {
  int result = madvise(buffer, bufferSize, MADV_HUGEPAGE);

  if (result != 0) {
    throw std::runtime_error(std::string("Failed to enable huge pages: ") + strerror(errno));
  }
}

__attribute__((cold)) void* hugePageAlignedAlloc(const size_t minBufferSize) {
  constexpr size_t HUGE_PAGE = 1 << 21;  // 2MiB

  size_t bufferSize = roundUp(minBufferSize, HUGE_PAGE);

  void* buffer = aligned_alloc(HUGE_PAGE, bufferSize);
  if (!buffer) {
    throw std::runtime_error(std::string("Failed to allocate aligned page: ") + strerror(errno));
  }

  enableHugePages(buffer, bufferSize);

  return buffer;
}
