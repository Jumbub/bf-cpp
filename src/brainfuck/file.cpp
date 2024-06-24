#include "file.h"

#include <filesystem>
#include <fstream>

namespace brainfuck {

__attribute__((cold)) std::optional<Code> read(const std::string filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.good()) {
    return std::nullopt;
  }
  const auto size = std::filesystem::file_size(filename);
  std::vector<char> buffer(size);
  file.read(buffer.data(), static_cast<std::streamsize>(size));
  return buffer;
}

}  // namespace brainfuck
