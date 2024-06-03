#include "file.h"

#include <fstream>
#include <string>
#include "benchmark/benchmark.h"

namespace file {

std::vector<char> read(const char* filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  file.read(buffer.data(), size);
  return buffer;
}

}  // namespace file
