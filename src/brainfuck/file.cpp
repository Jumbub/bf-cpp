#include "file.h"
#include "instruction.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace brainfuck {

std::optional<std::vector<char>> read(const std::string filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.good()) {
    return std::nullopt;
  }
  const auto size = std::filesystem::file_size(filename);
  std::vector<char> buffer(size);
  file.read(buffer.data(), static_cast<std::streamsize>(size));
  return buffer;
}

void write(const std::string filename, const char* unknown, const std::streamsize size) {
  std::ofstream file(filename, std::ios::binary);
  std::cout << size << std::endl;
  std::cout << sizeof(Instruction) << " " << std::endl;
  const char* outputSize = reinterpret_cast<const char*>(static_cast<const int64_t*>(&size));
  std::cout << *(reinterpret_cast<const int64_t*>(outputSize)) << std::endl;
  file.write(outputSize, sizeof(int64_t));
  file.write(unknown, size);
  file.close();
}

}  // namespace brainfuck
