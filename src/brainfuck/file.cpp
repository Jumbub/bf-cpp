#include "file.h"

#include <fstream>

namespace brainfuck {

std::optional<std::vector<char>> read(const std::string filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file.good()) {
    return std::nullopt;
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  file.read(buffer.data(), size);
  return buffer;
}

}  // namespace brainfuck
